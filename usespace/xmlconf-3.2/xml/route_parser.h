#ifndef ROUTE_ZERO_PARSER_H
#define ROUTE_ZERO_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <sstream>

#include "../base/logwraper.h"
#include "../core/node.h"
#include "../core/system.h"
#include "../core/logger.h"
#include "../tinyxml/tinyxml.h"
#include "../libxmlwrapper4c/mgtcgi_xmlconfig.h"
#include "../core/route_dns.h"
#include "../core/route_lanmgr.h"
#include "../core/route_wanmgr.h"
#include "../core/route_interfacelinkmgr.h"
#include "../core/route_pppd_authenticate.h"
#include "../core/route_adsl.h"
#include "../core/route_adslmgr.h"
#include "../core/route_vpnmgr.h"
#include "../core/route_loadbalancingmgr.h"
#include "../core/route_routingmgr.h"
#include "../core/route_tablemgr.h"
#include "../core/route_rulemgr.h"
#include "../core/route_dnatmgr.h"
#include "../core/route_snatmgr.h"
#include "../core/route_macbindmgr.h"
#include "../core/route_vlanmgr.h"
#include "../core/route_macvlanmgr.h"

#include "../core/ddns.h"
#include "../core/route_dhcpdmgr.h"
#include "../core/route_dhcpdhostmgr.h"
#include "../core/route_pptpmgr.h"
#include "../core/route_pptpd_accountmgr.h"
#include "../core/channel.h"
#include "../core/wan_channel.h"
#include "../core/wan_channelmgr.h"
#include "../core/route_tables.h"
#include "../core/route_tablesmgr.h"


namespace maxnet{
#define RET_ERROR 	-1

class RouteParser  : public LogWraper {

  public:	  
  	//mgrAddDefault : 在某些类中是否增加默认的数据
  	RouteParser(Log *log, const std::string route_config_filename, const std::string route_xmlkey,
  						int check, int print_dns, int print_ddns,int print_interface_lan, 
  						int print_interface_wan, int print_adsl_client, 
  						int print_l2tpvpn_client, int print_loadb, int print_routing, 
  						int print_route_table, int print_route_rule, 
  						int print_dnat, int print_snat, int print_macbind,
  						int print_adsl_client_passwd,int print_routing_command);

	~RouteParser();
	int		process();
	int		loadConfig();
	void	unloadConfig();
	
	#define SPLIT_CHAR "/"
	void split(std::string instr, std::vector<std::string> *lstoutstr , 
					const char* splitchar=SPLIT_CHAR);


	/*读取XML中的数据节点转换成struct结构数据
		return : 返回结构大小
	*/
	void * AllocBuf(int size);
	void printADSLClientPasswdCommand(int print);
	void printLoadBalancingRuleCommand(int id, int total_weight, int weight, RouteRule *route_rule, int line);
	int printLoadBalancingCommand(int print, const char * loadb_name);
	void printRoutingCommand(int print);
	void printDNATCommand(int print);
	void printSNATCommand(int print);
	void printMACBindCommand(int print);
    void printRouteDdnsCommand(int print);
    void printRouteRuleCommand(int print);
    void printAllRoutingCommand(int print);
    void printInterfaceLanCommand(int print);
    void printInterfaceWanCommand(int print);
    void printWanChannelCommand(int print);
    int check_balance_line(void);

	int  checkRouting(int count, std::string id, std::string dst, std::string gateway,
							std::string ifname, std::string table, std::string rule, bool enable);

	void printRoutingRuleCommand(std::string dst, Node * node_table, RouteRule *route_rule, int table_id, const char * ifname);

	int GetStructRouteDNS(struct mgtcgi_xml_dns ** p_xml);
    int GetStructWansChannnel(struct mgtcgi_xml_wans_channels ** pxmlwanschannel);
	int GetStructRouteInterfaceLAN(struct mgtcgi_xml_route_interface_lans ** p_xml);
	int GetStructRouteInterfaceWAN(struct mgtcgi_xml_interface_wans ** p_xml);
    int GetStructRouteInterfaceLink(struct mgtcgi_xml_interface_links ** p_xml);
	int GetStructRouteInterfaceVLAN(struct mgtcgi_xml_interface_vlans ** p_xml);
    int GetStructRouteInterfaceMacVLAN(struct mgtcgi_xml_interface_macvlans ** p_xml);
    int GetStructRouteTables(struct mgtcgi_xml_route_tables ** p_xml);
	int GetStructRoutePPPDAuth(struct mgtcgi_xml_pppd_authenticate ** p_xml);
	int GetStructRouteADSLClient(struct mgtcgi_xml_adsl_clients ** p_xml);
	int GetStructRouteVPNClient(struct mgtcgi_xml_l2tpvpn_clients ** p_xml);
	int GetStructRouteLoadBalancing(struct mgtcgi_xml_load_balancings ** p_xml);
	int GetStructRouteRouting(struct mgtcgi_xml_routings ** p_xml);
	//int GetStructRouteTable(struct mgtcgi_xml_group_array ** p_xml);
	int GetStructRouteRule(struct mgtcgi_xml_route_rules ** p_xml);
	int GetStructRouteDNAT(struct mgtcgi_xml_nat_dnats ** p_xml);
	int GetStructRouteSNAT(struct mgtcgi_xml_nat_snats ** p_xml);
	int GetStructRouteMACBind(struct mgtcgi_xml_mac_binds ** p_xml);
    int GetStructRouteDDNS(struct mgtcgi_xml_ddns ** p_xml);
//    int DeleteXmlRouteMacBind(const char *filename,const struct mgtcgi_xml_mac_binds *const node);
    int GetStructPptpdServer(struct mgtcgi_xml_pptpd_server **p_xml);
    int GetStructPptpdAccount(struct mgtcgi_xml_pptpd_accounts_info **p_xml);

	int GetStructRouteDhcpd(struct mgtcgi_xml_dhcpd_servers ** p_xml);
	int GetStructRouteDhcpdHost(struct mgtcgi_xml_dhcpd_hosts ** p_xml);

	int GetStructRouteTableGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processRouteTable() > 0) return RET_ERROR;
		//netaddrmgr.output();
		return GetStructXmlRouteGroups(pxmlgroup , route_table_mgr);
	}	
	int GetStructXmlRouteGroups(struct mgtcgi_xml_group_array ** pxmlgroup,
					 NodeMgr & nodemgr);

protected:
	std::string	group_name;
	std::string		route_config_filename;
	std::string		route_xmlkey;
	int				check;
	int				print_dns;
	int                         print_ddns;

	
	int				print_interface_lan;
	int				print_interface_wan;
	int				print_adsl_client;
	int				print_l2tpvpn_client;
	int 			print_loadb;
	int 			print_routing;
	int				print_route_table;
    int             print_route_rule;
    int             print_dnat;
    int             print_snat;
	int				print_macbind;
	int				print_adsl_client_passwd;

    //add by chenhao,2015-12-22 16:55:10
    int             print_routing_command;

   std::string	   getNodeValue(TiXmlNode *node);
   std::string	   getNodeValuebyName(const std::string nodename);
   int		processRouteDNS();
   int		processRouteInterfaceLan();
   int		processRouteInterfaceWan();
   int		processRouteInterfaceLink();
   int		processRouteInterfaceVLan();
   int		processRouteInterfaceMacVLan();
   int		processRouteTables();
   int		processRoutePPPDAuthenticate();
   int		processRouteADSLClient();
   int		processRouteVPNClient();
   int		processRouteLoadBalancing();
   int		processRouteRouting();
   int		processRouteTable();
   int		processRouteRule();
   int		processRouteDNAT();
   int		processRouteSNAT();
   int		processRouteMACBind();
   int      processRouteDDns();
   int		processRouteDhcpd();
   int		processRouteDhcpdHost();
   int		processPptpServer();
   int 	    processPptpdAccount();
   int      processWansChannel();
  
   void		parseRouteInterfaceLan(TiXmlNode * cur);
   void     parseLanSubnet(TiXmlNode * cur);
   void		parseRouteInterfaceWan(TiXmlNode * cur);
   void     parseRouteInterfaceLink(TiXmlNode * cur);
   void		parseRouteInterfaceVLan(TiXmlNode * cur);
   void	    parseRouteInterfaceMacVLan(TiXmlNode * cur);
   void	    parseRouteTables(TiXmlNode * cur);
   void		parseRouteADSLClient(TiXmlNode * cur);
   void		parseRouteVPNClient(TiXmlNode * cur);
   void		parseRouteLoadBalancing(TiXmlNode * cur);
   void		parseRouteRouting(TiXmlNode * cur);
   void		parseRouteTable(TiXmlNode * cur);
   void		parseRouteRule(TiXmlNode * cur);
   void		parseRouteDNAT(TiXmlNode * cur);
   void		parseRouteSNAT(TiXmlNode * cur);
   void		parseRouteMACBind(TiXmlNode * cur);
   void		parseRouteDhcpd(TiXmlNode * cur);
   void		parseRouteDhcpdHost(TiXmlNode * cur);
   void 	parsePptpServer(TiXmlNode *cur);
   void     parsePptpAccount(TiXmlNode *cur);
   void     parseWansChannel(TiXmlNode * cur);
   void     parsewanchannel(TiXmlNode * cur, std::string id);
   void 	printMACBindConfig();
	 
   private:
	Logger			logger;
	TiXmlDocument   doc;

	RouteDNS				route_dns;
	RouteLanMgr				route_lanmgr;
	RouteWanMgr				route_wanmgr;
    Route_InterfaceLinkMgr  route_interfacelinkmgr;
	RouteVLanMgr			route_vlanmgr;
    RouteMacVLanMgr         route_macvlanmgr;
	RoutePPPDAuthenticate	route_pppdauth;
	RouteADSLClientMgr		route_adslclient_mgr;
	RouteVPNClientMgr		route_vpnclient_mgr;
	RouteLoadBalancingMgr	route_loadb_mgr;
	RouteRoutingMgr			route_routing_mgr;
	RouteTableMgr			route_table_mgr;
	RouteRuleMgr			route_rule_mgr;
	RouteDNATMgr			route_dnat_mgr;
	RouteSNATMgr			route_snat_mgr;
	RouteMACBindMgr			route_macbind_mgr;
	RouteDDns               ddns;
	RouteDhcpdMgr			route_dhcpd_mgr;
	RouteDhcpdHostMgr		route_dhcpdhost_mgr;
    WanChannelMgr           wanchannelmgr;
    RouteTablesMgr          routetablesmgr;

    System			        system;
    Pptpd                   pptpd;
    PptpdAccountMgr         pptpdaccountmgr;
	TiXmlNode  * findNode(TiXmlNode *parent, const std::string nodename);
		
	/**
	* Description:  删除字符串左右的空格
	* @param  str : 需要删除左右空格的字符串
	* @return        返回修改后的字符串指针，
	*/
	std::string trimstring( const std::string& str );

	/**
	* Description:  将一个字符串转换为int数字
	* @param  str : 需要转换为数字的字符串
	* @return        返回转换后的数字
	*/
	inline int strtoint(const std::string & str)
	{
		int itmp = 0;
		try{
		//printf("str = '%s'\n",str.c_str());
		std::stringstream stream_string;
		stream_string << str;
		stream_string >> itmp;	/**/
		//itmp = atoi(str.c_str());
		
		//printf("itmp = %d\n",itmp);

		}catch(...){}

		return itmp;
	}

	/**
	* Description:  将一个int数字转换字符串
	* @param  num : 需要转换为数字的字符串
	* @return        返回转换后的字符串
	*/
	inline std::string inttostr(const int num)
	{
		std::string tmpstr = "";
		try{
		std::stringstream stream_string;
		stream_string << num;
		stream_string >> tmpstr;
		}catch(...){}

		return tmpstr;
	}
	
	/**
	* Description:  将一个字符串转换为大写字符串
	* @param  str : 需要转换的字符串
	* @return        返回转换后的字符串
	*/
	inline char * strtoupper(char * str)
	{
		try{
			if(str == NULL)
				return str;
			
			for(int i= 0; str[i] != '\0'; ++i){
				str[i]=toupper(str[i]);
			}
		}catch(...){}
		return str;
	}
	
};

}

#endif // ROUTE_ZERO_PEASER_H
