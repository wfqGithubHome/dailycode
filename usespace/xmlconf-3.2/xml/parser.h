#ifndef _ZERO_PARSER_H
#define _ZERO_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include "../base/logwraper.h"
#include "../core/node.h"
#include "../core/system.h"
#include "../core/management.h"
#include "../core/ddns.h"
#include "../core/auth.h"
#include "../core/interface.h"
#include "../core/netaddrmgr.h"
#include "../core/schedulemgr.h"
#include "../core/xmllib_schedulemgr.h"
#include "../core/vlanmgr.h"
#include "../core/protocolmgr.h"
#include "../core/trafficmgr.h"
#include "../core/vlanmgr.h"
#include "../core/sessionmgr.h"
#include "../core/firewallmgr.h"
#include "../core/firewall.h"
#include "../core/macbind.h"
#include "../core/macbindmgr.h"
#include "../core/macauth.h"
#include "../core/macauthmgr.h"
#include "../core/quota.h"
#include "../core/quotamgr.h"
#include "../core/globalipmgr.h"
#include "../core/globalip.h"
#include "../core/portmirror.h"
#include "../core/portmirrormgr.h"
#include "../core/netaddr.h"
#include "../core/protocol.h"
#include "../core/layer7prototype.h"
#include "../core/layer7prototypemgr.h"
#include "../core/schedule.h"
#include "../core/traffic_util.h"
#include "../core/logger.h"
#include "../core/interfacemgr.h"
#include "../core/domainnode.h"
#include "../core/domainmgr.h"
#include "../core/dnspolicy.h"
#include "../core/dnspolicymgr.h"
#include "../core/extensionnode.h"
#include "../core/extensionmgr.h"
#include "../core/httpservernode.h"
#include "../core/httpservermgr.h"
#include "../core/httpdirpolicynode.h"
#include "../core/httpdirpolicymgr.h"
#include "../core/urlformatnode.h"
#include "../core/urlformatmgr.h"
#include "../tinyxml/tinyxml.h"

#include "../libxmlwrapper4c/mgtcgi_xmlconfig.h"

#include "../core/portscanmgr.h"
#include "../core/portscan.h"
#include "../core/networkdetection.h"
#include "../core/servconfmgr.h"
#include "../core/servconf.h"
#include "../core/servinfomgr.h"
#include "../core/servinfo.h"
#include "../core/timing.h"
#include "../core/timingmgr.h"
#include "../core/wifi.h"
#include "../core/wifimgr.h"
#include "../core/firstgamechannel.h"
#include "../core/firstgamechannelmgr.h"
#include "../core/routeserver.h"
#include "../core/firewall_policy.h"
#include "../core/firewall_policymgr.h"



namespace maxnet{
#define RET_ERROR 	-1


class Parser  : public LogWraper {

  public:	  
  	//mgrAddDefault : 在某些类中是否增加默认的数据
    Parser(Log *log, const std::string main_config_filename,
						const std::string netaddr_config_filename,
						const std::string rfc_proto_config_filename,
						const std::string l7_proto_config_filename,
						const std::string server_info_config_filename,
						const int mgrAddDefault, int check, int print_fw,int print_action,
                        int print_portscan, int print_macbind, int print_macauth, 
						int print_quota,int print_globalip,	int print_portmirror, 
						int print_dnshelper , int print_httpdirpolicy);
	~Parser();
	int		process();
	int		loadConfig();
	void	unloadConfig();
	
	#define SPLIT_CHAR "/"
	void split(std::string instr, std::vector<std::string> *lstoutstr , 
					const char* splitchar=SPLIT_CHAR);


	/*根据XML中的数据节点的struct存储结构
		return : 返回结构大小
	*/
	void * AllocBuf(int allocsize);
	int GetStructSystem(struct mgtcgi_xml_system ** pxmlsystem);
	int GetStructManagerment(struct mgtcgi_xml_management ** pxmlmgt);
	int GetStructAuth(struct mgtcgi_xml_auth ** pxmlauth);	
	int GetStructInterface(struct mgtcgi_xml_interfaces ** pxmlinterface);
	int GetStructVlans(struct mgtcgi_xml_vlans ** pxmlvlans);


    int GetStructPortscan(struct mgtcgi_xml_portscan** pxmlportscandata);
    int GetStructServInfo(struct mgtcgi_xml_servinfo **pxmlservinfodata);
    int GetStructServConf(struct mgtcgi_xml_servconf **pxmlservconfdata);
    
	int GetStructNetworks(struct mgtcgi_xml_networks ** pxmlnetwork);
	int GetStructNetworksGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processNetworkAddr() > 0) 
            return RET_ERROR;
		return GetStructXmlGroups(pxmlgroup , netaddrmgr);
	}		
	int GetStructScheduler(struct mgtcgi_xml_schedules ** pxmlscheduler);
	int GetStructSchedulerGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processXmllibSchedule() > 0)	
            return RET_ERROR;
		return GetStructXmlGroups(pxmlgroup , xmllibschedulemgr);
	}	
	int GetStructDomainData(struct mgtcgi_xml_domains ** pxmldomaindata);
	int GetStructDomainDataGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processDnsHelper(false) > 0)	return RET_ERROR;
		return GetStructXmlGroups(pxmlgroup , domainmgr);
	}	
	int GetStructDnsPolicy(struct mgtcgi_xml_dns_policys ** pxmldnspolicy);
	int GetStructExtensions(struct mgtcgi_xml_extensions ** pxmlextensions);
	int GetStructExtensionGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processHttpDirPolicy(false) > 0)	return RET_ERROR;
		//extensionmgr.output();
		return GetStructXmlGroups(pxmlgroup , extensionmgr);
	}	
	int GetStructHttpserver(struct mgtcgi_xml_httpservers ** pxmlhttpserver );
	int GetStructHttpserverGroups(struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processHttpDirPolicy(false) > 0)	return RET_ERROR;
		return GetStructXmlGroups(pxmlgroup , httpservermgr);
	}	
	int GetStructHttpDirPolicy(struct mgtcgi_xml_http_dir_policys ** pxmlhttpdirpolicy);
	int GetStructIPSessionLimit(struct mgtcgi_xml_ip_session_limits ** pxmlipsessionlimit);
	int GetStructPortmirrors(struct mgtcgi_xml_port_mirrors ** pxmlportmirror);
	int GetStructSessionlimit(struct mgtcgi_xml_session_limits ** pxmlsessionlimit);
	int GetStructTraffic(struct mgtcgi_xml_traffics ** pxmltraffic);
	int GetStructFirewalls(struct mgtcgi_xml_firewalls ** pxmlfirewalls);	
	int GetStructNetworkDetection(struct mgtcgi_xml_networkdetection **pxmlNetworkDetection);
    int GetStructLayer7ProtocolType(const char * lanaguage,struct mgtcgi_xml_layer7_protocols_show** pxmll7protocolType);
	int GetStructProtocols(struct mgtcgi_xml_protocols ** pxmlprotocol);
	int GetStructProtocolGroups(const char * lanaguage,struct mgtcgi_xml_group_array ** pxmlgroup){
		if(processCustomProtocolGroup(lanaguage) > 0)
			return RET_ERROR;
		return GetStructXmlGroups(pxmlgroup , protocolmgr);
	}	
	std::string GetSessionLayer7Log(void){
		return getNodeValuebyName("logger/session_layer7_log");
	}
	int GetStructLayer7Protocols(const char * lanaguage,struct mgtcgi_xml_layer7_protocols_show** pxmll7protocol);
	int GetStructLogger(struct mgtcgi_xml_loggers ** pxmllogger);	

    /* Add by chenhao , 2014-11-20 17:13:25 */
    int GetStructTimingAdsl(struct mgtcgi_xml_adsl_timings **pxmltimingadsl);

    //Add by chenhao,2015-1-23 20:24:20
    int GetStructWifiConf(struct mgtcgi_xml_wifis **pxmlwifi);

    //Add by chenhao,2015-7-7 19:54:07
    int GetStructFirstGameChannel(struct mgtcgi_xml_first_game_channels **pxmlfirstgamechannel);

    //Add by chenhao,2015-11-5 11:29:58
    int GetStructRouteServerInfo(struct mgtcgi_xml_routeserver **pxmlrouteserverinfo);
    int GetStructFirewallPolicy(struct mgtcgi_xml_firewalls_policy ** pxmlfirewallpolicy);

protected:
	/**
	* Description:  通用的group/include结构转换为struct函数
	* @param  pxmlgroup : 输出的转换后的group内存指针
	* @param nodemgr : 需要转换的，包含group数据的NodeMgr对象
	* @return        返回转换后的struct结构大小 
	*/
	int GetStructXmlGroups(struct mgtcgi_xml_group_array ** pxmlgroup,
			 NodeMgr & nodemgr);

	
  protected:
    std::string     getNodeValue(TiXmlNode *node);
	std::string		getNodeValuebyName(const std::string nodename);

	
	int		processReservedNetworkAddr();

	int		processSystem();
	int		processManagement();
	int		processAuth();
	int		processMacbind();
	int		processInterface();
	int     processQuota();

	int		processNetworkAddr();
	int		processMacAuth();
	int		processSchedule();
	int		processXmllibSchedule();
	int		processProtocol(const std::string &lanaguage="english");
	int		processLayer7Protocol(const std::string &lanaguage="english");
    int     processCustomProtocol();
    int     processCustomProtocolGroup(const std::string &lanaguage="english");
	int		processSession();
	int		processTraffic();
	int		processFirewall(bool forScript = true);
	int		processLogger();
	int		processVlan();

    /* Add by chenhao , 2014-11-20 15:10:19 */
    int     processTiming();
    /*Add by chenhao,2015-1-26 17:30:07*/
    int     processWifiInfo();

    /*Add by chenhao,2015-7-7 19:56:31 优先游戏通道*/
    int     processFirstGameChannel();
    int     processFirewallPolicy();
    int     processRouteserverinfo();

    int     processPortScan();
    int     processServInfo();
    int     processServConf();
	int 	processPing();
	int		processGlobalIP();
    int     processPortMirror(bool forScript = true);
	
	//增加于2011-7-30日，用于解析DNSHelper功能模块所需配置数据
	int 	processDnsHelper(bool forScript = true);
	
	//增加于2011-8-5日，用于解析HTTP目录重定向功能模块所需配置数据
	int 	processHttpDirPolicy(bool forScript = true);

	void		parseNetworkAddr(TiXmlNode * cur);
	void		parseNetworkAddrGroup(TiXmlNode * cur);
	void		parseMacAuth(TiXmlNode * cur);
	void		parseMacAuthGroup(TiXmlNode * cur);
	void		parseSchedule(TiXmlNode * cur);
	void		parseXmlLibSchedule(TiXmlNode * cur);
	void		parseScheduleGroup(TiXmlNode * cur);
	void		parseXmllibScheduleGroup(TiXmlNode * cur);
	void		parseSession(TiXmlNode * cur);
	void		parseProtocol(TiXmlNode * cur );
	void		parseLayer7ProtoType(TiXmlNode * cur);
	void		parseProtocolGroup(TiXmlNode * cur);
	void		parseTraffic(TiXmlNode * cur);
	void		parseFirewall(TiXmlNode * cur,bool forScript);

	void		parseVlan(TiXmlNode * cur);

    void        parsePortScan(TiXmlNode * cur);
    void        parseServInfo(TiXmlNode * cur);
    void        parseServConf(TiXmlNode * cur);

	void	 	parsePing(TiXmlNode * cur);
	void		parseGlobalIP(TiXmlNode * cur);
	void		parseMacBind(TiXmlNode * cur);
	void		parseQuota(TiXmlNode * cur);
    void        parsePortMirror(TiXmlNode * cur,bool forScript);
    void        parseInterface(TiXmlNode * cur);

    void        parseTrafficChannel(TiXmlNode * cur , std::string interface);

	//增加于2011-7-30日，用于解析DNSHelper功能模块所需配置数据
	void 		parseDomain(TiXmlNode * cur);
	void		parseDomainGroup(TiXmlNode * cur);
	void 		parseDnspolicy(TiXmlNode * cur,bool forScript);

	//增加于2011-8-5日，用于解析HTTP目录重定向功能模块所需配置数据	
	void 		parseExtension(TiXmlNode * cur);
	void 		parseExtensionGroup(TiXmlNode * cur);	
	void 		parseHttpServer(TiXmlNode * cur);
	void 		parseHttpServerGroup(TiXmlNode * cur);
	void 		parseHttpDirPolicy(TiXmlNode * cur,bool forScript);

/* Add by chenhao , 2014-11-20 15:11:30 */
    void        parseTiming(TiXmlNode * cur);
    void        parseTimingAdsl(TiXmlNode * cur);
    void        parseTimingreboot(TiXmlNode * cur);

/*chenhao,2015-1-26 20:50:57*/
    void        parseWifi(TiXmlNode * cur);
    void        parseWifipool(TiXmlNode * cur);

    /*chenhao,2015-7-7 19:57:34 优先游戏通道 */
    void        parseFirstGameChannel(TiXmlNode * cur);
    void        parseFirewallpolicyinfo(TiXmlNode * cur);
    void        parseFirewallpolicy(TiXmlNode * cur);
    void        parseRouteserverinfo(TiXmlNode * cur);

    void        printFirewallPolicyCommand(int print);
	void		printTrafficCommand();
	void		printFirewallCommand(int i);
	void		printFirewallActionCommand(int i);
	void		printGlobalIPCommand(int i);
    void        printPortMirrorCommand(int i);
	void     	printMacBindCommand();
	void     	printMacAuthCommand();
	void 		printQuotaCommand();
	void		printFirewallConfig();
    void        printCustomProtocol();
    void        printLogger();
    void        printFirewallPolicyActionCommand(int print, int action_id, std::string src, std::string dst,std::string isp,
			std::string proto,std::string timed,std::string week,std::string day,std::string in_limit,std::string out_limit);
	void		printFirewallRuleCommand(std::string name, int bridge_id,
						Protocol * proto, std::string auth,  Schedule * sche, Vlan * vlan,
						NetAddr * src_addr, bool src_invent, NetAddr * dst_addr, bool dst_invent, 
						MacAuth* mac_addr, bool mac_invent, int action_id ,int count, std::string matchname);

    void        printPortMirrorRuleCommand(std::string name, int bridge_id,
						Protocol * proto, std::string action, NetAddr * src_addr, NetAddr * dst_addr,int count);
	
	//增加于2011-7-30日，用于解析DNSHelper功能模块所需配置数据
	void		printDomainDataCommand();
	void 		printDnsPolicyCommand();
	void 		printDnsHelperCommand();
	
	//增加于2011-8-5日，用于解析HTTP目录重定向功能模块所需配置数据
	void		printExtensionCommand();
	void		printUrlFormatCommand();
	void		printHttpServerCommand();
	void		printHttpPolicyCommand();
	void 		printHttpDirPolicyCommand();

    /* 2012-09-29 add  printPortScanCommand*/
    void        printPortScanCommand();
    
	std::string		main_config_filename;
	std::string		netaddr_config_filename;
	std::string		rfc_proto_config_filename;
	std::string		l7_proto_config_filename;
    std::string     server_info_config_filename;
	int				check;
	int				print_fw;
	int				print_action;
	int				print_macbind;
	int				print_macauth;
	int 			print_quota;
	int				print_globalip;
    int             print_portmirror;
    int             print_dnshelper;
    int             print_httpdirpolicy;

    int             print_portscan;
    
   private:
	std::string		group_name;
    std::string		layer7_name_cache;
	int				layer7_id_cache;
    std::string		traffic_interface;

#if 0
    std::string     type;
    std::string     newer;
    std::string     pending;
    std::string     interval;
#endif

	int				isDir(std::string name);
	int				myreadline(FILE * fp,char *buf,int len);
	int				parseSignatureFile(std::string dir, std::string name);
	int				parseDir(std::string dir, std::string name);
	int				getLayer7Id(std::string name);

    Routeserver     routerserver;
	System			system;
    Management		management;
	Auth			auth;
	MacBindMgr		macbindmgr;
	QuotaMgr       	quotamgr;
	InterfaceMgr	interfacemgr;
	NetAddrMgr		netaddrmgr;
	MacAuthMgr		macauthmgr;
	ScheduleMgr		schedulemgr;
	Xmllib_ScheduleMgr		xmllibschedulemgr;
	VlanMgr			vlanmgr;
	NetworkDetection			networkdetectionmgr;

    /* Add by chenhao , 2014-11-20 15:08:36*/
    TimingMgr       timingmgr;
    WifiMgr         wifimgr;

    /* Add by chenhao, 2015-7-7 18:15:12*/
    FirstgamechannelMgr  firstgcmgr;
    /*2016-3-29， chenhao*/
    FirewallPolicyMgr    firewallpolicymgr;

    /*2012-09-26 add   portscanmgr */
    PortScanMgr     portscanmgr;
    ServInfoMgr     servinfomgr;
    ServConfMgr     servconfmgr;
    
	ProtocolMgr		protocolmgr;
	std::map<std::string,std::string> mapLayer7prototype;
    //std::map<std::string,int> mapLayer7protogid;
	SessionMgr		sessionmgr;
	TrafficMgr		trafficmgr;
	FirewallMgr		firewallmgr;
	GlobalIPMgr		globalipmgr;
    PortMirrorMgr   portmirrormgr;
	DomainMgr		domainmgr;
	DnspolicyMgr	dnspolicymgr;	
	ExtensionMgr	extensionmgr;
	HttpServerMgr	httpservermgr;
	HttpDirPolicyMgr httpdirpolicymgr;
	UrlFormatMgr	urlformatmgr;
	Logger			logger;
	TiXmlDocument   doc;

	
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

#endif // _ZERO_PEASER_H
