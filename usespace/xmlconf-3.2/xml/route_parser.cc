#include <iostream>
#include <fstream>
#include "route_parser.h"
#include "../global.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_WANCHANNEL_NUM	31

namespace maxnet{

	const struct dev_firmark_info wans[MAX_WAN_NUM] = {
			{"WAN0",0x100,100},
			{"WAN1",0x101,101},
			{"WAN2",0x102,102},
			{"WAN3",0x103,103},
			{"WAN4",0x104,104},
	};

	const struct dev_firmark_info ppps[MAX_PPP_NUM] = {
			{"ppp0",0x200,200},
			{"ppp1",0x201,201},
			{"ppp2",0x202,202},
			{"ppp3",0x203,203},
			{"ppp4",0x204,204},
			{"ppp5",0x205,205},
			{"ppp6",0x206,206},
			{"ppp7",0x207,207},
			{"ppp8",0x208,208},
			{"ppp9",0x209,209},
			{"ppp10",0x210,210},
			{"ppp11",0x211,211},
			{"ppp12",0x212,212},
			{"ppp13",0x213,213},
			{"ppp14",0x214,214},
	};

	const struct dev_firmark_info lans[MAX_LAN_NUM] = {
			{"LAN",0x300,300},
	};

	const struct ptotocol_id proto_ids[10] = {
		{PROTOCOL_ID_BEGIN,"null"},
	    {PROTOCOL_ID_TCP,"tcp"},
	    {PROTOCOL_ID_UDP,"udp"},
	    {PROTOCOL_ID_ICMP,"icmp"},
	    {PROTOCOL_ID_ALL,"all"},
	    {PROTOCOL_ID_END,"null"},
	};
	int32_t get_netmask_length(const char* mask);

  	RouteParser::RouteParser(Log *log, const std::string route_config_filename,const std::string route_xmlkey,
  						int check, int print_dns,int print_ddns, int print_interface_lan, 
  						int print_interface_wan, int print_adsl_client, 
  						int print_l2tpvpn_client, int print_loadb, int print_routing, 
  						int print_route_table, int print_route_rule, 
  						int print_dnat, int print_snat, int print_macbind,
  						int print_adsl_client_passwd, int print_routing_command)
	 {
		setLog(log);	
		setLogHeader("[R] ");
		this->route_config_filename = route_config_filename;
		this->route_xmlkey = route_xmlkey;
		this->check = check;
		this->print_dns = print_dns;
		this->print_ddns = print_ddns;
		
		this->print_interface_lan = print_interface_lan;
		this->print_interface_wan = print_interface_wan;
		this->print_loadb = print_loadb;
		this->print_routing = print_routing;
		this->print_route_table     = print_route_table;
		this->print_route_rule = print_route_rule;
		this->print_dnat = print_dnat;
		this->print_snat = print_snat;
		this->print_macbind = print_macbind;
		this->print_adsl_client_passwd = print_adsl_client_passwd;

		this->print_routing_command = print_routing_command;
	}

	RouteParser::~RouteParser(){
		
	}
	
	int RouteParser::loadConfig(){

		if(!doc.LoadFile(route_config_filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr  << "open xml route config file error: " << route_config_filename << std::endl;
			return 1;	
		}
		return 0;
	}

	void RouteParser::unloadConfig(){
		return;
	}

	int RouteParser::process(){
		int ret = 1,fd;

		char path[64]="/var/lock/router.lock";

		fd=open(path,O_WRONLY|O_CREAT);
		if(fd < 0){
			return 0;
		}
	
		if(flock(fd,LOCK_EX)!=0){
			close(fd);
			return 0;
		}
		
		ret = loadConfig();
		if(ret != 0){
			flock(fd,LOCK_UN);
			close(fd);
			return 1;
		}
		flock(fd,LOCK_UN);
		close(fd);

		//processRouteDNS();
		processRouteDDns();  /* wuke 2009/12/01 */
		processRouteInterfaceLan();
		processRouteInterfaceWan();
		processRouteInterfaceLink();  /* chenhao，2016-3-22*/
		processRouteInterfaceVLan();
		processRouteInterfaceMacVLan();
		processRoutePPPDAuthenticate();
		processRouteADSLClient();
		processRouteVPNClient();
		processRouteLoadBalancing();
		processRouteRouting();
		//processRouteTable();   /*pengyunsheng 2016-4.21*/
		processRouteTables();    /*pengyunsheng 2016-4.21*/
		processRouteRule();
		processRouteDNAT();
		processRouteSNAT();
		processRouteMACBind();
		processRouteDhcpd();
		processRouteDhcpdHost();
		processWansChannel();
		//unloadConfig();

		if(check == 0)
		{
			if (print_loadb){
				if (route_xmlkey == "")
					printLoadBalancingCommand(1, NULL);
				else
					printLoadBalancingCommand(1, route_xmlkey.c_str());
				return 0;
			}

			if (print_routing){
				printRoutingCommand(1);
				return 0;
			}

			if (print_dnat){
				printDNATCommand(1);
				return 0;
			}

			if (print_snat){
				printSNATCommand(1);
				return 0;
			}

			if (print_macbind){
				printMACBindCommand(1);
				return 0;
			}

			if (print_adsl_client_passwd){
				printADSLClientPasswdCommand(1);
				return 0;
			}
			if (print_ddns){
				printRouteDdnsCommand(1);
				return 0;
			}

			if(print_route_rule){
				printRouteRuleCommand(1);
				return 0;
			}

			if(print_routing_command){
			//	printAllRoutingCommand(1);
				printWanChannelCommand(1);
				return 0;
			}

			if(print_interface_lan){
				printInterfaceLanCommand(1);
				return 0;
			}

			if(print_interface_wan){
				printInterfaceWanCommand(1);
				return 0;
			}
			
			
		//	route_dns.output();
			route_lanmgr.output();
			route_wanmgr.output();
			route_interfacelinkmgr.output();
			route_vlanmgr.output();
			route_macvlanmgr.output();
			route_pppdauth.output();
		//	route_adslclient_mgr.output();
		//	route_vpnclient_mgr.output();
		//	route_loadb_mgr.output();
		//	route_routing_mgr.output();
		//	route_table_mgr.output();
		//	route_rule_mgr.output();
			route_dnat_mgr.output();
			route_snat_mgr.output();
			route_macbind_mgr.output();
			printMACBindConfig();
		    route_dhcpd_mgr.output();
		    route_dhcpdhost_mgr.output();
			routetablesmgr.output();
			wanchannelmgr.output();
			
			ddns.output();	
		}
		return 0;
	}

	int gcd(int a, int b)
	{
		int tmp = 0;
		if (a < b){
			tmp = a;
			a = b;
			b = tmp;	
		}
		if (b == 0)
		  return a;
		else
		  return gcd(b, a%b);
	}
	
	int ngcd(int *a, int n)
	{
		if (n == 1)
		  return *a;
		return gcd(a[n-1], ngcd(a, n-1));
	}

	void RouteParser::printInterfaceLanCommand(int print)
	{
        std::string ip;
        std::string mask;
        std::string mac;
		std::string ifname;
        std::string dhcp;
        std::string startip;
        std::string endip;
        std::string lease_time;
        std::string gateway;
        std::string dhcp_mask;
        std::string first_dns;
        std::string second_dns;
		int			mask_len;
		int			subnetdataconunt;
		std::vector<RouteLan *> list = route_lanmgr.getlansubnetlist();
		
		if(print == 0)
			return;
		
		ip = route_lanmgr.getLanIp();
		mask = route_lanmgr.getLanMask();
		mac = route_lanmgr.getLanMac();
		ifname = route_lanmgr.getLanIfname();
		lease_time = route_lanmgr.getDhcpLeaseTime();
		
		if((ip.size() == 0)
			|| (mask.size() == 0)
			|| (mac.size() == 0))
			return;
		subnetdataconunt = route_lanmgr.getsubnetdatacount();
		mask_len = get_netmask_length(mask.c_str());

		std::cout << "#!/bin/sh\n" << std::endl;
		std::cout << "#ifname=\"" + ifname << std::endl;
		std::cout << "/usr/local/sbin/ip addr flush dev LAN >/dev/null 2>&1" << std::endl;
		std::cout << "/usr/local/sbin/ip addr add " + ip + "/" + inttostr(mask_len) + " dev LAN >/dev/null 2>&1" << std::endl;
		std::cout << "/usr/local/sbin/ip link set dev LAN address " + mac + " >/dev/null 2>&1" << std::endl;

		if(route_lanmgr.getSubnetEnable()){
			for(unsigned int i=0;i < list.size();i++){
				RouteLan *laninfo = (RouteLan *)list.at(i);

				ip = laninfo->getIP();
				mask_len = get_netmask_length(laninfo->getMask().c_str());
				std::cout << "/usr/local/sbin/ip addr add " + ip + "/" + inttostr(mask_len) + " dev LAN >/dev/null 2>&1" << std::endl;
			}
		}

		if(strcmp(route_lanmgr.getLanDhcp().c_str(),"off") != 0)
			std::cout << "/usr/local/sbin/dnsmasq" << std::endl;
	}

	void RouteParser::printInterfaceWanCommand(int print)
	{
	    std::string id;
        std::string ifname;
        std::string type;
        std::string ip;
        std::string mask;
        std::string gateway;
        std::string username;
        std::string passwd;
        std::string servername;
        std::string mtu;
        std::string mac;
        std::string dns;
        std::string up;
        std::string down;
        std::string isp;
        std::string time;
        std::string week;
        std::string day;

	//	std::vector<RouteWan *> list = route_wanmgr.getlansubnetlist();

		route_wanmgr.output();
	}
	
	void RouteParser::printLoadBalancingRuleCommand(int id, int every, int weight, RouteRule *route_rule, int line)
	{
		std::string protocol="";
		std::string src_port="";
		std::string dst_port="";
		std::string dscp="";
		std::string src="";
		std::string dst="";
		int tmp_every = every;

		if (route_rule != NULL){
			src = route_rule->getSRC();
			if (src != "")
				src=" -s " + src + " ";
			dst = route_rule->getDST();
			if (dst != "")
				dst = " -d " + dst + " ";
			protocol = route_rule->getProtocol();
			src_port = route_rule->getSRCPort();
			dst_port = route_rule->getDSTPort();
			dscp = route_rule->getTOS();
			if ((dscp != "") && (dscp != "null"))
				dscp = " -m dscp --dscp " + dscp + " ";
			else
				dscp = "";
		}

		/*std::cout << "src = " << src << std::endl;
		std::cout << "dst = " << dst << std::endl;
		std::cout << "protocol = " << protocol << std::endl;
		std::cout << "port = " << port << std::endl;
		std::cout << "dscp = " << dscp << std::endl;*/
		
		for (int i=0; i<weight; i++){
			std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting " << src << dst << dscp << " -m mark --mark 0 -m statistic --mode nth --every " << tmp_every << " --packet 0 -j MARK --set-mark " << id << std::endl;
#if 0
			if(0 == line){
				std::cout << "/usr/local/sbin/iptables -t mangle -A PREROUTING -i LAN " << src << dst << dscp << " -m mark --mark 0 -m state --state NEW -m statistic --mode nth --every " << every << " --packet 0 -j MARK --set-mark " << id << std::endl;
			}else{
				std::cout << "/usr/local/sbin/iptables -t mangle -I PREROUTING " << line << " -i LAN " << src << dst << dscp << " -m mark --mark 0 -m state --state NEW -m statistic --mode nth --every " << every << " --packet 0 -j MARK --set-mark " << id << std::endl;
			}
#endif
			tmp_every--;
		}
	}

	int RouteParser::printLoadBalancingCommand(int print, const char * loadb_name){
		std::vector<Node *> loadb_list = route_loadb_mgr.getList();
		RouteRule	* route_rule = NULL;
		std::string rulename = "lf";

		std::string id = "";
		std::string comment = "";
		std::string name = "";
		std::string ifname = "";
		std::string weight = "";
		
		std::string pre_name = "";
		std::string pre_ifname = "";
		std::string pre_weight = "";
		std::string next_name = "";
		std::string next_ifname = "";
		std::string next_weight = "";

		std::string temp = "";
		int base_id = 4000, maxgcd = 1, gcd_weight = 0, max_every = 0, every = 1;
		unsigned int i=0,j=0,print_first_flag=0,total_num = 0;
		int weight_array[256] = {0};
		int balance_route_first_id = 1000;
		std::string command = "";
		
		//weight的最大公约数
		if (loadb_list.size() > 0){
			for( i=0; i < loadb_list.size(); i++){
				RouteLoadBalancing * loadb = (RouteLoadBalancing *) loadb_list.at(i);
					if(system.checkLinkState(loadb->getIFName().c_str()) == false)
					{
						continue;
					}
					weight_array[total_num] = strtoint(loadb->getWeight());
					total_num++;
			}
			if(total_num == 0){
				return 0;
			}
			maxgcd = ngcd(weight_array, total_num);
			for( i=0; i < loadb_list.size(); i++){
				RouteLoadBalancing * loadb = (RouteLoadBalancing *) loadb_list.at(i);
					if(system.checkLinkState(loadb->getIFName().c_str()) == false)
					{
						continue;
					}
					gcd_weight = (strtoint(loadb->getWeight()) / maxgcd);
					max_every += gcd_weight;
					loadb->setWeight(inttostr(gcd_weight));
			}
		}
		//根据weight从小到大排序
		for( i=0; i < loadb_list.size(); i++){
			for (j=0; j < loadb_list.size()-i-1; j++){
				RouteLoadBalancing * pre_loadb = (RouteLoadBalancing *) loadb_list.at(j);
				RouteLoadBalancing * next_loadb = (RouteLoadBalancing *) loadb_list.at(j+1);
				
				pre_name = pre_loadb->getName();
				pre_ifname = pre_loadb->getIFName();
				pre_weight = pre_loadb->getWeight();
				
				next_name = next_loadb->getName();
				next_ifname = next_loadb->getIFName();
				next_weight = next_loadb->getWeight();

				if (strtoint(pre_weight) < strtoint(next_weight)){
					temp = pre_name;
					pre_loadb->setName(next_name);
					next_loadb->setName(temp);

					temp = pre_ifname;
					pre_loadb->setIFName(next_ifname);
					next_loadb->setIFName(temp);

					temp = pre_weight;
					pre_loadb->setWeight(next_weight);
					next_loadb->setWeight(temp);
				}
			}
		}

		balance_route_first_id = route_routing_mgr.getBalanceFirstId();
		if(0 == balance_route_first_id){
			command = "-A";
		}else{
			command = "-I";
		}
		
		//every = 1;	
		every = max_every;
		print_first_flag = 0;
		for(i=0; i < loadb_list.size(); i++){
			RouteLoadBalancing * loadb = (RouteLoadBalancing *) loadb_list.at(i);
			name = loadb->getName();
			ifname = loadb->getIFName();
			rulename = loadb->getRuleName();
			weight = loadb->getWeight();
			gcd_weight = (strtoint(weight) * maxgcd);	
			
			if(system.checkLinkState(ifname.c_str()) == false)
			{
				continue;
			}
			
			if (loadb_name != NULL){
				if (loadb_name == name){
					if (print == 0)
						return base_id + i;
					else
						std::cout << base_id + i;
					return 0;
				}
			}	
			/*--save-mark,--restore-mark*/
			else {
				if (print_first_flag == 0){
					if(0 == balance_route_first_id){
					//	std::cout << "/usr/local/sbin/iptables -t mangle -A PREROUTING -i LAN -j CONNMARK --restore-mark" << std::endl;
					}else{
					//	std::cout << "/usr/local/sbin/iptables -t mangle -I PREROUTING " << balance_route_first_id << " -i LAN -j CONNMARK --save-mark" << std::endl;
					}
					print_first_flag=1;
				}
					
				std::cout << "# id=\"" << base_id + i << "\",name=\"" << name << "\",ifname=\"" << ifname << "\",rulename=\"" << rulename
					<< "\",weight=\"" << gcd_weight << "\",maxgcd=\"" << maxgcd << "\",max_every=\"" << max_every << "\",comment=\"" << comment << "\"" << std::endl;
				if (print){
					if (strcmp(rulename.c_str(), "null") != 0){
						route_rule = (RouteRule *) route_rule_mgr.find(rulename); 
					}
					else {
						route_rule = NULL;
					}
				
					printLoadBalancingRuleCommand((base_id + i), every, strtoint(weight), route_rule, balance_route_first_id);
					//every += strtoint(weight);
					//if (every > max_every){
						//break;
					//}
					every = every - strtoint(weight);
					if(every < 1){
						break;
					}
				}
			}

		}

		if (print_first_flag > 0)
			if(0 == balance_route_first_id){
			//	std::cout << "/usr/local/sbin/iptables -t mangle -A PREROUTING -i LAN -j CONNMARK --save-mark" << std::endl;
			}else{
			//	std::cout << "/usr/local/sbin/iptables -t mangle -I PREROUTING " << balance_route_first_id <<" -i LAN -j CONNMARK --restore-mark" << std::endl;
			}
			
		return 0;
	}
	

	int RouteParser::checkRouting(int count, std::string id, std::string dst, std::string gateway,
							std::string ifname, std::string table, std::string rule, bool enable){
		char cmd[CMD_LENGTH]={0};
		char buffer[DIGITAL_LENGTH] = {0};
		int ret_value=-1;
		const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";
		FILE *pp = NULL;
		snprintf(cmd, sizeof(cmd), "%s/route_script.sh updaterouting count=%d,id=%s,dst=%s,gateway=%s,ifname=%s,table=%s,rule=%s,enable=%d", 
				script_path, count, id.c_str(), dst.c_str(), gateway.c_str(), ifname.c_str(), table.c_str(), rule.c_str(), enable);
		//printf ("cmd:%s\n", cmd);
		pp=popen(cmd,"r");
		if (pp != NULL){
			fgets(buffer, sizeof(buffer), pp);
			buffer[strlen(buffer)-1] = 0;
			if (strcmp(buffer, "0") == 0){
				ret_value = 0;
			}
			else {
				ret_value = -1;
			}
		}
		pclose(pp);
		return ret_value;
	}


	void RouteParser::printRoutingRuleCommand(std::string dst, Node * node_table, RouteRule *route_rule, int table_id, 
												const char *ifname)
	{
		static unsigned int mark_value = 1;
		char  mark_value_str[10] = {0};
		char buffer[20]={0};
		unsigned int i=0;
		int loadb_fwmark = 0;

		const std::string iptables_mangle= "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting ";
	//	const std::string iptables_filter = "/usr/local/sbin/iptables -I INPUT -i LAN ";
		std::string iptables_cmd = "";
		std::string str = "";
		std::string src = "";
		std::string ddst = "";
		std::string protocol = "";
		std::string src_port = "";
		std::string dst_port = "";
		std::string rule_command="";
		std::string fwmark_command = "";
		std::string fw_setmark = "";

		std::string dscp = "";
		std::string dscp_setmark = "";
		std::string action = "";

		iptables_cmd = iptables_mangle;
		
		if (table_id <=0 || table_id > 251){
			return;
		}
			
		for (i=0; i < strlen(ifname); i++){
			if (i >= 3)
				break;
			buffer[i] = ifname[i];
		}
		
		buffer[i] = '\0';
		if (strcmp(buffer, "bal") == 0){ //负载均衡
			loadb_fwmark = printLoadBalancingCommand(0, ifname);
			if(loadb_fwmark != 0 )
				std::cout << "/usr/local/sbin/ip rule add fwmark " << loadb_fwmark << " table " << table_id << std::endl;
			return;
		}

		if (route_rule != NULL){
			rule_command = route_rule->getCommand();

			src = route_rule->getSRC();
			ddst = route_rule->getDST();
			protocol = route_rule->getProtocol();
			src_port = route_rule->getSRCPort();
			dst_port = route_rule->getDSTPort();
			dscp = route_rule->getTOS();
			action = route_rule->getAction();
#if 0
			if(action == "DROP"){
				iptables_cmd = iptables_filter;
			}
#endif		
			memset(mark_value_str, 0, sizeof(mark_value_str));
			sprintf(mark_value_str, "%d", mark_value);

			fwmark_command = "";
			dscp_setmark = "";
			fw_setmark = "";
			//fw_setmark = fw_setmark + " -m conntrack --ctstate NEW -j CONNMARK --set-mark " + mark_value_str;
#if 0
			if(action == "DROP"){
				fw_setmark = fw_setmark + " -j DROP ";
			}
			else{
				fw_setmark = fw_setmark + " -j MARK --set-mark " + mark_value_str;
			}
#endif
			fw_setmark = fw_setmark + " -j MARK --set-mark " + mark_value_str;
	
			fwmark_command = fwmark_command + " fwmark " + mark_value_str;
			if ((dscp != "null") && 
				(dscp.size() > 0)){
				dscp_setmark = " -m dscp --dscp " + dscp + " ";
			}

			if ( src.size() > 0 ){
				str = str + " -s " + src;
			}
			if ( src_port.size() > 0 ){
				str = str + " -m multiport --sport " + src_port;
			}
			if ( ddst.size() > 0 ){
				str = str + " -d " + ddst;
			}
			if (dst_port.size() > 0 ){
				str = str + " -m multiport --dport " + dst_port;
			}

			str = str + dscp_setmark + fw_setmark;
			if (protocol == "tcp"){
				std::cout << iptables_cmd <<  " -p tcp " << str << std::endl;
			}
			else if (protocol == "udp"){
				std::cout << iptables_cmd << " -p udp " << str << std::endl;
			}
			else if(protocol == "all"){
				std::cout << iptables_cmd << " -p tcp " << str << std::endl;
				std::cout << iptables_cmd <<" -p udp " << str << std::endl;
			}
			else{
				std::cout << iptables_cmd << str << std::endl;
			}
#if 0
			if(action == "DROP"){
				return;
			}
#endif			
			mark_value++;

			if(fwmark_command != ""){
				std::cout << "/usr/local/sbin/ip rule add";
				std::cout << fwmark_command << " table " << table_id << std::endl;
			}
		/*	if ((protocol != "null") &&
				(dst_port.size() > 0 )){
				
				fwmark_command = fwmark_command + " fwmark " + mark_value_str;

				if (protocol == "tcp"){
					std::cout << iptables_mangle << " -p tcp -m multiport --dport " << dst_port << dscp_setmark << fw_setmark << std::endl;
				}
				else if (protocol == "udp"){
					std::cout << iptables_mangle << " -p udp -m multiport --dport " << dst_port << dscp_setmark << fw_setmark << std::endl;
				}
				else {
					std::cout << iptables_mangle << " -p udp -m multiport --dport " << dst_port << dscp_setmark << fw_setmark << std::endl;
					std::cout << iptables_mangle + " -p tcp -m multiport --dport " << dst_port << dscp_setmark << fw_setmark << std::endl;
				}
				
				mark_value++;
			}

			if (dscp_setmark != "") {
				fwmark_command=fwmark_command + " fwmark " + mark_value_str;
				std::cout << iptables_mangle << dscp_setmark << fw_setmark << std::endl;
			}
			mark_value++;*/

		/*	if ((rule_command != "") && (fwmark_command != "")){
				std::cout << "/usr/local/sbin/ip rule add ";
				std::cout << rule_command << fwmark_command << " table " << table_id << std::endl;
			}
			else if (rule_command != ""){
				std::cout << "/usr/local/sbin/ip rule add ";
				std::cout << rule_command << " table " << table_id << std::endl;
			}
			else if (fwmark_command != ""){
				std::cout << "/usr/local/sbin/ip rule add ";
				std::cout << fwmark_command << " table " << table_id << std::endl;
			}*/
		}
		
		if (node_table != NULL){
			std::vector<std::string> subnodes;
			subnodes = node_table->getSubNodes();
			for(int j=subnodes.size() - 1; j >= 0; j--){
				std::cout << "/usr/local/sbin/ip rule add to  " << subnodes.at(j) << " table " << table_id << std::endl;
			}
		}
		
		if (dst.size() > 0){
			std::cout << "/usr/local/sbin/ip rule add to " << dst << " table " << table_id << std::endl;
		}
	}

	void RouteParser::printAllRoutingCommand(int print){
		
		printRouteRuleCommand(1);

		std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting -m state --state ESTABLISHED,RELATED -j CONNMARK --restore-mark" << std::endl;

		if(check_balance_line() != 0){
			std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting -m dscp --dscp 0 -j ACCEPT" << std::endl;
		}
		
		std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting -m mark ! --mark 0 -j ACCEPT" << std::endl;

		printRoutingCommand(1);
		printLoadBalancingCommand(1,NULL);

		std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting -j CONNMARK --save-mark"<< std::endl;
		std::cout << "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting -j ACCEPT" << std::endl;
		
		std::cout << "/usr/local/sbin/iptables -t mangle -A PREROUTING -i LAN -j zone_lan_prerouting" << std::endl;
	}

	void RouteParser::printWanChannelCommand(int print){
		int 			index = 0;

		std::string		classname = "";
		std::string 	rate = "";
		unsigned int 	classid = 0;
		unsigned int 	classtype = 0;
		unsigned int    shareid = 0;
		unsigned int	parentid = 0;
		unsigned int 	weight = 0;
		unsigned int	attribute = 0;
		unsigned int	prio = 0;
		unsigned int	limit = 0;
		unsigned int	dir = 0;
		char	cmd[512] = {0};
		std::string		cmd_str = "";
		
		if(print == 0)
			return;

		std::vector<WanChannel *> wanchannel_list = wanchannelmgr.getWanChannelList();
		for(unsigned int i = 0; i < wanchannel_list.size() && (i < MAX_WANCHANNEL_NUM); i++){
			WanChannel *wanchannel = wanchannel_list.at(i);
			index = strtoint(wanchannel->getId());

			if(wanchannel->getEnable() == false){
				continue;
			}
			if(!((index > 0) && (index < MAX_WANCHANNEL_NUM))){
				continue;
			}

			std::vector<Channel *> channels_list = wanchannel->getchannelList();
			for(unsigned int j = 0; j < channels_list.size(); j++){
				Channel *channel = channels_list.at(j);
				
				classname = channel->getClassName();
				classtype = channel->getClassType();
				classid = channel->getClassId();
				shareid = channel->getShareId();
				parentid = channel->getParentId();
				weight = channel->getWeight();
				rate = channel->getRate();
				attribute = channel->getAttribute();
				prio = channel->getPriority();
				limit = channel->getLimit();
				dir = channel->getDir();

				if(classtype == 0){
					snprintf(cmd,sizeof(cmd),"/usr/local/sbin/ecbq --add --name %s -i %d --type %u --classid %u --shareid %u --parentid %u --weight %u --maxrate %s --attr %u --dir %u",
								classname.c_str(),index,classtype,classid,shareid,parentid,weight,rate.c_str(),attribute,dir);
				}
				else if(classtype == 1){
					snprintf(cmd,sizeof(cmd),"/usr/local/sbin/ecbq --add --name %s -i %d --type %u --classid %u --shareid %u --parentid %u --weight %u --maxrate %s --attr %u --dir %u",
								classname.c_str(),index,classtype,classid,shareid,parentid,weight,rate.c_str(),attribute,dir);
				}
				else if(classtype == 2){
					snprintf(cmd,sizeof(cmd),"/usr/local/sbin/ecbq --add --name %s --index %d --type %u --classid %u --shareid %u --parentid %u --weight %u --maxrate %s --attr %u --prio %u --limit %u --dir %u",
								classname.c_str(),index,classtype,classid,shareid,parentid,weight,rate.c_str(),attribute,prio,limit,dir);
				}
				else{
					continue;
				}

				cmd_str = cmd;
				std::cout << cmd_str << std::endl;

				classname = "";
				rate = "";
				classid = 0;
				classtype = 0;
				shareid = 0;
				parentid = 0;
				weight = 0;
				attribute = 0;
				prio = 0;
				limit = 0;
				dir = 0;
				cmd_str = "";
				memset(cmd,0,sizeof(cmd));
			}
			
		}
	}

	void RouteParser::printRoutingCommand(int print){
		std::vector<Node *> routing_list = route_routing_mgr.getList();

		RouteRule	* route_rule = NULL;
		Node 		* node_table = NULL;
		std::string id = "";
		std::string dst = "";
		std::string gateway = "";
		std::string ifname = "";
		std::string priority = "1";
		std::string table = "";
		std::string rule = "";
		std::string comment = "";
		bool		enable = false;
		int count = 0;
		int i = 0;
		int ret_value = 0;
		int table_id = -1;

		if (routing_list.size() == 0){
			count = -1;
			checkRouting(count, id, dst, gateway, ifname, table, rule, enable);
		}
		
		for(count = 0, i = (routing_list.size() - 1); i >= 0; i--, count++){
			RouteRouting * routing = (RouteRouting *) routing_list.at(i);

			id = routing->getID();
			dst = routing->getDST();
			gateway = routing->getGateway();
			ifname = routing->getIFName();
			priority = routing->getPriority();
			table = routing->getTable();
			rule = routing->getRule();
			comment = routing->getComment();
			enable = routing->getEnable();

			std::cout << "# id=\"" << id << "\",dst=\"" << dst << "\",gateway=\"" << gateway
					<< "\",ifname=\"" << ifname << "\",priority=\"" <<	priority << "\",table=\"" << table
					<< "\",rule=\"" << rule << "\",enable=\"" << enable << "\",comment=\"" << comment << "\"" << std::endl;

			if (print == 0)
				continue;
			table_id = -1;
			table_id = strtoint(id);
			if (table_id  < 0 || table_id > 250){
				continue;
			}
			table_id += 1; 
			ret_value = checkRouting(count, id, dst, gateway, ifname, table, rule, enable);
			if (ret_value != 0){
				continue;
			}
			if(system.checkLinkState(ifname.c_str()) == false)
			{
				continue;
			}
			
			if (enable == false)
				continue;
			
			if (strcmp(table.c_str(), "null") != 0){
				node_table = (Node *)route_table_mgr.Onlyfind(routing->getTable());
			}
			else {
				node_table = NULL;
			}

			if (strcmp(rule.c_str(), "null") != 0){
				route_rule = (RouteRule *) route_rule_mgr.find(routing->getRule()); 
			}
			else {
				route_rule = NULL;
			}

			if (print){
				printRoutingRuleCommand(dst, node_table, route_rule, table_id, ifname.c_str());
			}
			
		}
	}

	int32_t get_netmask_length(const char* mask)
	{
	    int32_t masklen=0, i=0;
	    uint32_t netmask=0;

	    if(mask == NULL)
	    {
	        return 0;
	    }

	    struct in_addr ip_addr;
	    if( inet_aton(mask, &ip_addr) )
	    {
	        netmask = ntohl(ip_addr.s_addr);
	    }else{
	        netmask = 0;
	        return 0;
	    }
	    
	    while(0 == (netmask & 0x01) && i<32)
	    {
	        i++;
	        netmask = netmask>>1;
	    }
	    masklen = 32-i;
	    return masklen;
	}

	int check_interface_type(std::string name){
		int ret_value=0;

		if (strncmp(name.c_str(), "LAN",3) == 0)	//LAN口
			ret_value = 1;
		else if (strncmp(name.c_str(), "WAN",3) == 0)//WAN口
			ret_value = 2;
		else if (strncmp(name.c_str(), "ads",3) == 0)//adsl客服端
			ret_value = 3;
		else if (strncmp(name.c_str(), "l2t",3) == 0)//l2tpvpn客服端
			ret_value = 4;
		else if (strncmp(name.c_str(), "bal",3) == 0)//balance负载均衡接口
			ret_value = 5;
		else 
			ret_value = -1;
		
		return ret_value;
	}

	int str_to_hex(char *string, char *cbuf, int len)  
	{  
		char high, low;  
		int idx, ii=0;  
		for (idx=0; idx<len; idx+=2)   
		{  
		    high = string[idx];  
		    low = string[idx+1];  
		      
		    if(high>='0' && high<='9')  
		        high = high-'0';  
		    else if(high>='A' && high<='F')  
		        high = high - 'A' + 10;  
		    else if(high>='a' && high<='f')  
		        high = high - 'a' + 10;  
		      
		    if(low>='0' && low<='9')  
		        low = low-'0';  
		    else if(low>='A' && low<='F')  
		        low = low - 'A' + 10;  
		    else if(low>='a' && low<='f')  
		        low = low - 'a' + 10;  
		      
		    cbuf[ii++] = high<<4 | low;  
		}  
		return 0;  
	}  

	void get_ip_by_interface(std::string ifname, char *ip, int len){
		char *script = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh";
		FILE *pp = NULL;

		char cmd[120] = {""};
		snprintf(cmd,sizeof(cmd),"%s get_ip_by_interface %s",script,ifname.c_str());
		pp=popen(cmd,"r");
		if (pp != NULL){
			fgets(ip, len, pp);
			if(ip[strlen(ip) - 1] == '\n'){
				ip[strlen(ip)-1] = '\0';
			}
		}
		pclose(pp);
		return;
	}

	void get_gateway_by_interface(std::string ifname, char *gateway, int len){
		char *script = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh";
		FILE *pp = NULL;

		char cmd[120] = {""};
		snprintf(cmd,sizeof(cmd),"%s get_gateway_by_interface %s",script,ifname.c_str());
		pp=popen(cmd,"r");
		if (pp != NULL){
			fgets(gateway, len, pp);
			if(gateway[strlen(gateway) - 1] == '\n'){
				gateway[strlen(gateway)-1] = '\0';
			}
		}
		pclose(pp);
		return;
	}

	void get_dev_by_interface(std::string ifname, char *dev, int len){
		char *script = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh";
		FILE *pp = NULL;

		char cmd[120] = {""};
		snprintf(cmd,sizeof(cmd),"%s print_dev %s",script,ifname.c_str());
		pp=popen(cmd,"r");
		if (pp != NULL){
			fgets(dev, len, pp);
			if(dev[strlen(dev) - 1] == '\n'){
				dev[strlen(dev)-1] = '\0';
			}
		}
		pclose(pp);
		return;
	}

	void print_dnat_proto(int id, char *proto){
		int i;
		
		if(id <= PROTOCOL_ID_BEGIN || id >= PROTOCOL_ID_END)
			return;

		for(i=0;i<10;i++){
			if(id == proto_ids[i].id){
				strcpy(proto,proto_ids[i].name);
			}
		}
		return;
	}

	int get_fwmark(std::string ifname,int *fwmark,int *table)
	{
	    int ret_value = -1,i;
	    char cmd[CMD_LENGTH] = {0};
	    char dev[STRING_LENGTH] = {0};
	    const char *print_dev_cmd = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh print_dev";
		FILE *pp = NULL;

	    snprintf(cmd,sizeof(cmd),"%s %s",print_dev_cmd,ifname.c_str());

	    if(ifname.size() == 0){
	        return -1;
	    }

		pp=popen(cmd,"r");
		if (pp != NULL){
			fgets(dev, sizeof(dev), pp);
			if(dev[strlen(dev) - 1] == '\n'){
				dev[strlen(dev)-1] = '\0';
			}
		}
	    
	    if(strncmp(dev,"WAN",3) == 0){
	        for(i=0;i<MAX_WAN_NUM;i++){
	            if(strcmp(dev,wans[i].dev_name) == 0){
	                *fwmark = wans[i].firmark;
	                *table  = wans[i].table;
	                ret_value = 0;
	                break;
	            }
	        }
	    }
	    else if(strncmp(dev,"ppp",3) == 0){
	        for(i=0;i<MAX_PPP_NUM;i++){
	            if(strcmp(dev,ppps[i].dev_name) == 0){
	                *fwmark = ppps[i].firmark;
	                *table  = ppps[i].table;
	                ret_value = 0;
	                break;
	            }
	        }
	    }
	    else if(strncmp(dev,"LAN",3) == 0){
	        for(i=0;i<MAX_LAN_NUM;i++){
	            if(strcmp(dev,lans[i].dev_name) == 0){
	                *fwmark = lans[i].firmark;
	                *table  = lans[i].table;
	                ret_value = 0;
	                break;
	            }
	        }
	    }
	    else{
	        *fwmark = 0;
	        *table = 0;
	        ret_value = -1;
	    }

	    return ret_value;
	}

	int RouteParser::check_balance_line(void){
#if 0
		int main_line = 0;
		unsigned int i;
		std::string default_ifname = "";

		std::vector<Node *> ifwans = route_wanmgr.getList();
		std::vector<Node *> ifadsls = route_adslclient_mgr.getList();
		std::vector<Node *> ifvlans = route_vlanmgr.getList();
		std::vector<Node *> ifl2tps = route_vpnclient_mgr.getList();

		std::vector<Node *> balances = route_loadb_mgr.getList();

		for(i=0;i<ifwans.size();i++){
			RouteWan *item_wan = (RouteWan *) ifwans.at(i);
			if(item_wan->getDefaultRoute()){
				main_line = 1;
				default_ifname = item_wan->getIfname();
				break;
			}
		}

		if(main_line == 0){
			for(i=0;i<ifadsls.size();i++){
				RouteADSLClient *item_adsl = (RouteADSLClient *)ifadsls.at(i);
				if(item_adsl->getDefaultRoute()){
					main_line = 1;
					default_ifname = item_adsl->getName();
					break;
				}
			}
		}

		if(main_line == 0){
			for(i=0;i<ifvlans.size();i++){
				RouteVLan *item_vlan = (RouteVLan *)ifvlans.at(i);
				if(strtoint(item_vlan->getDefaultroute()) == 1){
					main_line = 1;
					default_ifname = item_vlan->getName();
					break;
				}
			}
		}

		if(main_line == 0){
			for(i=0;i<ifl2tps.size();i++){
				RouteVPNClient *item_l2tp = (RouteVPNClient*)ifl2tps.at(i);
				if(item_l2tp->getDefaultRoute()){
					main_line = 1;
					default_ifname = item_l2tp->getName();
					break;
				}
			}
		}

		if((main_line == 0) || (default_ifname == "")){
			return 1;
		}


		for(i=0;i<balances.size();i++){
			RouteLoadBalancing *item_balan = (RouteLoadBalancing *)balances.at(i);
			if(default_ifname.compare(item_balan->getIFName()) == 0){
				return 0;
			}
		}
		
		return 1;
	#endif
	    return 0;
	}

	void RouteParser::printDNATCommand(int print){
		std::vector<Node *> dnat_list = route_dnat_mgr.getList();
		std::string ifname = "";
		std::string protocol_id = "";
		std::string protocol = "";
		std::string wan_ip = "";
		std::string src_ip = "";
		std::string wan_port_before = "";
		std::string wan_port_after = "";
		std::string lan_port_before = "";
		std::string lan_port_after = "";
		std::string wan_gateway = "";
		std::string wan_port = "";
		std::string lan_ip = "";
		std::string lan_port = "";
		std::string fwmark = "";
		std::string table = "";
		std::string comment = "";
		std::string ifname_ip="";
		std::string cmd_out = "";
		std::string cmd_out2 = "";
		std::string cmd_mark = "";
		std::string dev = "";
		std::vector<std::string> devs;
		bool enable = false;
		char ip[16] = {0};
		char proto[16] = {0};
		char gateway[16] = {0};
		char dev_str[16] = {0};
		std::string in = ":";
		std::string out = "-";
//		int pos,proto_id;
		int sign = 0;
		int intfwmark = 0,inttable = 0;
		char fstr[16] = {0};
		char tstr[16] = {0};

		for(unsigned int i = 0; i < dnat_list.size(); i++){
			RouteDNAT * dnat = (RouteDNAT *) dnat_list.at(i);
			enable = dnat->getEnable();
			comment = dnat->getcomment();
			protocol_id = dnat->getProtocoLId();
			src_ip = dnat->getWanIp();
			wan_port_before = dnat->getWanPortBefore();
			wan_port_after = dnat->getWanPortAfter();
			lan_port_before = dnat->getLanPortBefore();
			lan_port_after = dnat->getLANPortAfter();
			lan_ip = dnat->getLanIp();
			ifname = dnat->getIfname();

			get_ip_by_interface(ifname,ip,sizeof(ip));
			wan_ip = ip;
			get_gateway_by_interface(ifname,gateway,sizeof(gateway));
			wan_gateway = gateway;
			get_dev_by_interface(ifname,dev_str,sizeof(dev_str));
			dev = dev_str;
			print_dnat_proto(strtoint(protocol_id),proto);
			protocol = proto;

		//	if((fwmark.size() == 0) || (table.size() == 0)){
				get_fwmark(ifname,&intfwmark,&inttable);
				sprintf(fstr,"%d",intfwmark);
				sprintf(tstr,"%d",inttable);

				fwmark = fstr;
				table = tstr;
		//	}

			std::cout << "# enable=\"" << enable << "\",protocol_id=\"" << protocol_id << "\",comment=\"" << comment
					<< "\",src_ip=\"" << src_ip << "\",wan_port_before=\"" << wan_port_before << "\",wan_port_after=\"" << wan_port_after
					<< "\",lan_port_before=\"" << lan_port_before << "\",lan_port_after=\"" << lan_port_after
					<< "\",lan_ip\"" << lan_ip << "\",ifname\"" << ifname << std::endl;
	
			if (print == 0)
				continue;
			if (enable == false)
				continue;

			for(unsigned int j = 0;j < devs.size();j++){
				std::string tmp_dev = devs.at(j);
				if(dev.compare(tmp_dev) == 0){
					sign = 1;
					break;
				}
			}
			if(sign == 0){
				devs.push_back(dev);
			}

			if(((ifname.size()) >= 3) && 
				((protocol.size()) > 0) &&
				((lan_ip.size()) > 0)){
				if (strcmp(ifname.c_str(), "all") == 0){
					cmd_out = "/usr/local/sbin/iptables -t nat -A PREROUTING ";
					cmd_mark = "/usr/local/sbin/iptables -t mangle -A PREROUTING ";
				}
				else {
					switch(check_interface_type(ifname)){
				    case 3:
						std::cout << "IF_NAME=`/usr/local/sbin/pppoe-status /etc/ppp/" << ifname << ".conf 2>/dev/null | grep \"peer\" | awk '{print $NF}'`"  << std::endl;
						break;
					case 4:
						std::cout << "IF_NAME=`/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh xl2tpd-status /etc/ppp/" << ifname << ".l2tpd 2>/dev/null |grep \"peer\" |awk '{print $NF}'`" << std::endl;
						break;
				    default:
						std::cout << "IF_NAME=\"" << ifname <<"\"" << std::endl;
						break;
					}
						
					cmd_out = "/usr/local/sbin/iptables -t nat -A PREROUTING -i $IF_NAME ";
					cmd_mark = "/usr/local/sbin/iptables -t mangle -A PREROUTING -i $IF_NAME ";
				}

				if(src_ip.size() > 0){
					if(src_ip.find("-") != std::string::npos){
						cmd_out += " -m iprange --src-range " + src_ip;
					}else{
						cmd_out += " --src " + src_ip;
					}
				}

				if ((wan_ip.size()) > 0){
			//		cmd_mark += " --dst "+wan_ip;
					cmd_out += " --dst " + wan_ip;
				}

				if(wan_port_before.size() > 0){
					if(wan_port_after.size() > 0){
						wan_port = wan_port_before + ":" + wan_port_after;
					}
					else{
						wan_port = wan_port_before;
					}
				}

				if ((strcmp(protocol.c_str(), "tcp") == 0) && ((wan_port.size()) > 0)){
					cmd_out += " -p tcp  -m multiport --dport "+wan_port;
			//		cmd_mark += " -p tcp  -m multiport --dport "+wan_port;
				}
				else if ((strcmp(protocol.c_str(), "udp") == 0) && ((wan_port.size()) > 0)){
					cmd_out += " -p udp  -m multiport --dport "+wan_port;
			//		cmd_mark += " -p udp  -m multiport --dport "+wan_port;
				}
				else if((strcmp(protocol.c_str(), "all") == 0) && ((wan_port.size()) > 0)){
					cmd_out2 = cmd_out;
					cmd_out += " -p tcp  -m multiport --dport " + wan_port;
					cmd_out2 += " -p udp  -m multiport --dport " + wan_port;
				}

				if(lan_port_before.size() > 0){
					if(lan_port_after.size() > 0){
						lan_port = lan_port_before + "-" + lan_port_after;
					}
					else{
						lan_port = lan_port_before;
					}
				}
				else{
					lan_port = wan_port_before;
					if(wan_port_after.size() > 0){
						lan_port += "-" + wan_port_after;
					}
				}

				if(lan_ip.size() > 0 && lan_port.size() > 0){
					cmd_out += " -j DNAT --to-destination " + lan_ip + ":" + lan_port;
					if(cmd_out2.size() > 0){
						cmd_out2 += " -j DNAT --to-destination " + lan_ip + ":" + lan_port;
					}
				}

				std::cout << cmd_out << std::endl;
				std::cout << cmd_out2 << std::endl;

		/*		if((lan_port.size()) > 0){
					pos = lan_port.find(in);
					while(pos != -1){
						lan_port.replace(pos,in.length(),out);
						pos = lan_port.find(in);
					}
					
					std::stringstream port(lan_port);
					std::string sub_str = "";
					while(getline(port,sub_str,',')){
						std::cout << cmd_out << ":" << sub_str << std::endl;
						sub_str = "";
					}
				}else{
					std::cout << cmd_out  << std::endl;
				}*/
/*
				cmd_mark += " -m state --state NEW -j CONNMARK --set-mark ";
		//		std::cout << "/usr/local/sbin/iptables -t mangle -A PREROUTING -i $IF_NAME -d "<< wan_ip << " -p"<<" -m state --state NEW -j CONNMARK --set-mark " << fwmark << std::endl;
		//		std::cout << "/usr/local/sbin/iptables -t mangle -A POSTROUTING -o $IF_NAME -m state --state NEW -j CONNMARK --set-mark " << fwmark << std::endl;
				if((sign == 0) && 
					(fwmark.size() > 0) &&
					(table.size() > 0) &&
					(strcmp(table.c_str(), "0") != 0) &&
					(strcmp(fwmark.c_str(), "0") != 0) &&
					(strcmp(ifname.c_str(),"LAN") != 0)){
					std::cout << cmd_mark << fwmark << std::endl;
					std::cout << "/usr/local/sbin/ip route flush table " << table << std::endl;;
					std::cout << "/usr/local/sbin/ip rule delete fwmark " << fwmark << " table " << table << std::endl;
					std::cout << "/usr/local/sbin/ip rule add fwmark " << fwmark << " table " << table << std::endl;
					std::cout << "/usr/local/sbin/ip route add default via " << wan_gateway << " dev $IF_NAME table " << table << std::endl;
				}

				sign = 0;
				wan_gateway = "";
				memset(gateway,0,sizeof(gateway));*/
			}	
		}
	}
	void RouteParser::printSNATCommand(int print){
		std::vector<Node *> snat_list = route_snat_mgr.getList();

		std::string ifname = "";
		std::string src = "";
		std::string dst = "";
		std::string protocol = "";
		std::string action = "";
		std::string dstip = "";
		std::string dstport = "";
		std::string comment = "";
		bool enable = false;

		for(unsigned int i = 0; i < snat_list.size(); i++){
			RouteSNAT * dnat = (RouteSNAT *) snat_list.at(i);
			ifname = dnat->getIfname();
			src = dnat->getSRC();
			dst = dnat->getDST();
			protocol = dnat->getProtocol();
			action = dnat->getAction();
			dstip = dnat->getDSTIP();
			dstport = dnat->getDSTPort();
			comment = dnat->getComment();
			enable = dnat->getEnable();

			std::cout << "# ifname=\"" << ifname << ",\"src=\"" << src << "\",dst=\"" << dst << "\",protocol=\"" << protocol << "\",action=\"" << action
					<< ",\"dstip=\"" << dstip << "\",dstport=\"" << dstport << "\",enable=\"" << enable << "\",comment=\"" << comment << "\"" << std::endl;

			if (print == 0)
				continue;
			if (enable == false)
				continue;
				
			if(((ifname.size()) > 0) && 
				((protocol.size()) > 0) &&
				((action.size()) > 0)){

				//action不为MASQUERADE,dstip不能为空。
				if ((strcmp(action.c_str(), "MASQUERADE") != 0) && ((dstip.size()) <= 0)){
					continue;
				}
				//protocol为all时，dstport必须为空。
				if ((strcmp(protocol.c_str(), "all") == 0) && ((dstport.size()) > 0)){
					continue;
				}
				
				if (strcmp(ifname.c_str(), "all") == 0){
					std::cout << "/usr/local/sbin/iptables -t nat -A POSTROUTING";
				}
				else {
					std::cout << "/usr/local/sbin/iptables -t nat -A POSTROUTING -o " << ifname;
				}

				if ((src.size()) > 0){
					std::cout << " -s " << src;
				}
				if ((dst.size()) > 0){
					std::cout << " -d " << dst;
				}

				if (strcmp(protocol.c_str(), "tcp") == 0){
					std::cout << " -p tcp ";
				}
				else if (strcmp(protocol.c_str(), "udp") == 0){
					std::cout << " -p udp ";
				}

				if (strcmp(action.c_str(), "MASQUERADE") == 0){
					std::cout << " -j MASQUERADE" << std::endl;
				}
				else {
					if ((dstport.size()) > 0){
						std::cout << " -j SNAT --to-source " << dstip << ":" << dstport << std::endl;
					}
					else {
						std::cout << " -j SNAT --to-source " << dstip << std::endl;
					}
				}
			}	
		}

		std::cout << "/usr/local/sbin/iptables -t nat -A POSTROUTING -s 10.222.222.0/24 -j MASQUERADE" << std::endl;
	}


	void RouteParser::printMACBindCommand(int print){
		std::vector<Node *> arpbind_list = route_macbind_mgr.getList();

		if(route_macbind_mgr.getLearnt()){
			std::cout << "# MAC_LEARNT=1" << std::endl;
			std::cout << "/usr/local/sbin/mac-bind -b 0 --set --action accept" << std::endl;
		}else{
			std::cout << "# MAC_LEARNT=0" << std::endl;
			std::cout << "/usr/local/sbin/mac-bind -b 0 --set --action drop" << std::endl;
		}
		
		std::string ip = "";
		std::string mac = "";
		std::string action = "";
		std::string bridge = "";
		std::string comment = "";
		for(unsigned int i = 0; i < arpbind_list.size(); i++){
			RouteMACBind * macbind = (RouteMACBind *) arpbind_list.at(i);
			ip = macbind->getIP();
			mac = macbind->getMAC();
			action = macbind->getAction();
			bridge = macbind->getBridge();
			comment = macbind->getComment();
			
			std::cout << "# ip=" << ip << ",mac=" << mac << ",action=" << action << ",bridge=" << bridge << ",comment=\"" << comment << "\""<< std::endl;
			if (print == 0)
				continue;
			if (action == "1"){
				std::cout << "/usr/local/sbin/ip neigh replace " << ip << " dev LAN lladdr " << mac << " nud perm" << std::endl;
				
				std::cout << "/usr/local/sbin/mac-bind -b " << bridge <<" --add --ip " << ip << " --mac " << mac << " --action drop " << std::endl;
			}
			else{
				std::cout << "/usr/local/sbin/ip neigh replace " << ip << " dev LAN lladdr " << mac << " nud reachable" << std::endl;
				
				std::cout << "/usr/local/sbin/mac-bind -b " << bridge <<" --add --ip " << ip << " --mac " << mac << " --action accept " << std::endl;
			}
		}
	}

	void RouteParser::printADSLClientPasswdCommand(int print)
	{
		std::vector<Node *> wansinfo = route_wanmgr.getList();
		std::string access = "pppoe";
		std::string username = "";
		std::string password = "";

		if(print == 0)
			return;
		
		for(unsigned int i = 0; i < wansinfo.size(); i++){
			RouteWan * wan = (RouteWan *) wansinfo.at(i);
			if(wan->getAccess().compare(access) == 0){
				username = wan->getUsername();
				password = wan->getPasswd();
				std::cout << username << "\t*\t" << password << std::endl;
			}			
		}
	}


	int RouteParser::processRouteDNS(){
		int ret = 1;

		route_dns.setDNSPrimary(getNodeValuebyName("dns/dns_primary"));
		route_dns.setDNSSecondary(getNodeValuebyName("dns/dns_secondary"));
		route_dns.setDNSThirdary(getNodeValuebyName("dns/dns_thirdary"));

		if(route_dns.isVaild()){
			ret = 0;
		}
		else{
			error("system configura is not vaild!");
		}
		
		return ret;
	}

	void RouteParser::	printRouteDdnsCommand(int print)
	{	

		bool ddns_enable = false;
		std::string provider ="";
		std::string username = "";
		std::string password = "";
		std::string dns = "";

		ddns_enable = ddns.isDDnsEnable();
		provider = ddns.getDDnsProvider();	
		username = ddns.getUsername();
		password = ddns.getPassword();
		dns = ddns.getDDnsDns();

		
		
		if (print == 0)
		{
			return;
		}

		if (ddns_enable == true)
		{
			if(provider=="1")
			{
				std::cout << " wget -q -O- \"http://" << username << ":" << password << "@members.3322.org/dyndns/update?system=dyndns&hostname="
				<<dns<<"\""<<std::endl;	
			}
			if(provider=="2")
			{
				std::cout << " wget -q -O- \"http://" << username << ":" << password << "@ddns.oray.com:80/ph/update?hostname="
				<<dns<<"\""<<std::endl;		
			}
		}
			
	 }

	void RouteParser::printRouteRuleCommand(int print)
	{
		std::string src = "";
		std::string dst = "";
		std::string src_port = "";
		std::string dst_port = "";
		std::string protol = "";
		std::string dscp = "";
		std::string action = "";
		std::string cmd = "";
		std::string iptables_cmd = "/usr/local/sbin/iptables -t mangle -A zone_lan_prerouting";
		std::vector<Node*> lst = route_rule_mgr.getList();
		
		if(print == 0){
			return;
		}
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteRule *item = (RouteRule *)lst.at(i);
			src = item->getSRC();
			dst = item->getDST();
			src_port = item->getSRCPort();
			dst_port = item->getDSTPort();
			protol = item->getProtocol();
			dscp = item->getTOS();
			action = item->getAction();

			item->output();

			if(action == "ACCEPT" || action == ""){
				continue;
			}
			if(src.size() > 0){
				cmd = cmd + " -s " + src;
			}
			if(src_port.size() > 0){
				cmd= cmd + " -m multiport --sport " + src_port;
			}
			if(dst.size() > 0){
				cmd = cmd + " -d " + dst;
			}
			if(dst_port.size() > 0){
				cmd = cmd + " -m multiport --dport " + dst_port;
			}
			if ((dscp != "null") && 
				(dscp.size() > 0)){
				cmd = cmd + " -m dscp --dscp " + dscp;
			}
			if(protol == "tcp"){
				std::cout << iptables_cmd << " -p tcp " << cmd << " -j DROP" << std::endl;
			}
			else if(protol == "udp"){
				std::cout << iptables_cmd << " -p udp " << cmd << " -j DROP" << std::endl;
			}
			else if(protol == "all"){
				std::cout << iptables_cmd << " -p tcp " << cmd << " -j DROP" << std::endl;
				std::cout << iptables_cmd << " -p udp " << cmd << " -j DROP" << std::endl;
			}
			else{
				std::cout << iptables_cmd << cmd << " -j DROP" << std::endl;
			}

			src = "";
			dst = "";
			src_port = "";
			dst_port = "";
			protol = "";
			dscp = "";
			action = "";
			cmd= "";
		}
	}
		
	int		RouteParser::processRouteInterfaceLan(){
		TiXmlNode * if_lans_node = findNode(&doc, "xmlconfig/interface_lans");

		if(if_lans_node != NULL && if_lans_node->FirstChild()){	
			parseRouteInterfaceLan(if_lans_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteInterfaceWan(){
		TiXmlNode * if_wans_node = findNode(&doc, "xmlconfig/interface_wans");

		if(if_wans_node != NULL && if_wans_node->FirstChild()){	
			parseRouteInterfaceWan(if_wans_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteInterfaceLink(){
		TiXmlNode * if_links_node = findNode(&doc, "xmlconfig/interface_link");

		if(if_links_node != NULL && if_links_node->FirstChild()){	
			parseRouteInterfaceLink(if_links_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteInterfaceVLan(){
		TiXmlNode * if_vlans_node = findNode(&doc, "xmlconfig/interface_vlans");

		if(if_vlans_node != NULL && if_vlans_node->FirstChild()){	
			parseRouteInterfaceVLan(if_vlans_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteInterfaceMacVLan(){
		TiXmlNode * if_macvlans_node = findNode(&doc, "xmlconfig/interface_macvlans");

		if(if_macvlans_node != NULL && if_macvlans_node->FirstChild()){	
			parseRouteInterfaceMacVLan(if_macvlans_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteTables(){
		TiXmlNode * if_routetables_node = findNode(&doc, "xmlconfig/route_tables");

		if(if_routetables_node != NULL && if_routetables_node->FirstChild()){	
			parseRouteTables(if_routetables_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRoutePPPDAuthenticate(){
		int ret = 1;	

		bool auth = false;
		std::string pap = "";
		std::string chap = "";
		std::string mschap = "";
		std::string mschapv2 = "";
		std::string eap = "";

		pap = getNodeValuebyName("pppd_authenticate/pap");
		chap = getNodeValuebyName("pppd_authenticate/chap");
		mschap = getNodeValuebyName("pppd_authenticate/mschap");
		mschapv2 = getNodeValuebyName("pppd_authenticate/mschapv2");
		eap = getNodeValuebyName("pppd_authenticate/eap");
		if(strcmp(pap.c_str(), "0") == 0){
			auth = false;
		}
		else{
			auth = true;
		}
		route_pppdauth.setRequirePAP(auth);

		if(strcmp(chap.c_str(), "0") == 0){
			auth = false;
		}
		else{
			auth = true;
		}
		route_pppdauth.setRequireCHAP(auth);

		if(strcmp(mschap.c_str(), "0") == 0){
			auth = false;
		}
		else{
			auth = true;
		}
		route_pppdauth.setRequireMSCHAP(auth);

		if(strcmp(mschapv2.c_str(), "0") == 0){
			auth = false;
		}
		else{
			auth = true;
		}
		route_pppdauth.setRequireMSCHAPV2(auth);

		if(strcmp(eap.c_str(), "0") == 0){
			auth = false;
		}
		else{
			auth = true;
		}
		route_pppdauth.setRequireEAP(auth);

		
		if(route_pppdauth.isVaild()){
			ret = 0;
		}
		else{
			error("system configura is not vaild!");
		}
		
		return ret;
	}

	int		RouteParser::processRouteADSLClient(){
		TiXmlNode * adslclient_node = findNode(&doc, "xmlconfig/adsl_clients");

		if(adslclient_node != NULL && adslclient_node->FirstChild()){	
			parseRouteADSLClient(adslclient_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteVPNClient(){
		TiXmlNode * vpnclient_node = findNode(&doc, "xmlconfig/l2tpvpn_clients");

		if(vpnclient_node != NULL && vpnclient_node->FirstChild()){	
			parseRouteVPNClient(vpnclient_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteLoadBalancing(){	
		TiXmlNode * loadb_node = findNode(&doc, "xmlconfig/load_balancings");
		if(loadb_node != NULL && loadb_node->FirstChild()){	
			parseRouteLoadBalancing(loadb_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteRouting(){
		TiXmlNode * routing_node = findNode(&doc, "xmlconfig/routings");

		if(routing_node != NULL && routing_node->FirstChild()){	
			parseRouteRouting(routing_node->FirstChild());
		}

		return 0;
	}
#if 1
	int		RouteParser::processRouteTable(){

		TiXmlNode * tables_group_node = findNode(&doc, "xmlconfig/route_tables");
		if((tables_group_node != NULL) && (tables_group_node->FirstChild() != NULL)){	
			parseRouteTable(tables_group_node->FirstChild());
		}	
		return 0;
	}	
#endif
	int		RouteParser::processRouteRule(){
		TiXmlNode * rule_node = findNode(&doc, "xmlconfig/route_rules");

		if(rule_node != NULL && rule_node->FirstChild()){	
			parseRouteRule(rule_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteDNAT(){
		TiXmlNode * dnat_node = findNode(&doc, "xmlconfig/nat_dnats");

		if(dnat_node != NULL && dnat_node->FirstChild()){	
			parseRouteDNAT(dnat_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteSNAT(){
		TiXmlNode * snat_node = findNode(&doc, "xmlconfig/nat_snats");

		if(snat_node != NULL && snat_node->FirstChild()){	
			parseRouteSNAT(snat_node->FirstChild());
		}

		return 0;
	}

	int		RouteParser::processRouteMACBind(){
		TiXmlNode * macbind_node = findNode(&doc, "xmlconfig/mac_binds");

		if(macbind_node != NULL && macbind_node->FirstChild()){	
			parseRouteMACBind(macbind_node->FirstChild());
		}

		return 0;
	}


	void RouteParser::printMACBindConfig(){
 		if(route_macbind_mgr.getLearnt()){
			std::cout << "# MAC_LEARNT=1" << std::endl;
		}else{
			std::cout << "# MAC_LEARNT=0" << std::endl;
		}
	}

	int		RouteParser::processPptpServer(){
		TiXmlNode * pptpd_node = findNode(&doc, "xmlconfig/pptpd_server");
	
		if(pptpd_node != NULL && pptpd_node->FirstChild()){ 
			parsePptpServer(pptpd_node->FirstChild());
		}
		return 0;
	}

	int 	RouteParser::processPptpdAccount(){
		TiXmlNode * pptpdacounts_node = findNode(&doc, "xmlconfig/pptpd_accounts");
	
		if(pptpdacounts_node != NULL && pptpdacounts_node->FirstChild()){ 
			parsePptpAccount(pptpdacounts_node->FirstChild());
		}
		return 0;
	}

	int 	RouteParser::processRouteDhcpd(){
		TiXmlNode * dhcpd_node = findNode(&doc, "xmlconfig/dhcpd_servers");
	
		if(dhcpd_node != NULL && dhcpd_node->FirstChild()){ 
			parseRouteDhcpd(dhcpd_node->FirstChild());
		}
		return 0;
	}

	int 	RouteParser::processRouteDhcpdHost(){
		TiXmlNode * host_node = findNode(&doc, "xmlconfig/dhcpd_host");
	
		if(host_node != NULL && host_node->FirstChild()){ 
			parseRouteDhcpdHost(host_node->FirstChild());
		}
		return 0;
	}

	int RouteParser::processWansChannel(){
		TiXmlNode * firewallpolicy = findNode(&doc, "xmlconfig/wan_channels");
		if(firewallpolicy != NULL && firewallpolicy->FirstChild() !=NULL){	
			parseWansChannel(firewallpolicy->FirstChild() );
		}
		return 0;
	} 

	void RouteParser::parseWansChannel(TiXmlNode * cur){
		std::string 	id = "";
		bool			enable = true;
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "wans") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "id") == 0){
							id = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "enable") == 0){
							enable = true;
							if(strcmp(attr->Value(),"0") == 0){
								enable = false;
							}
						}
						attr=attr->Next();
					}
					wanchannelmgr.add(id, enable);

					if(cur->FirstChild()){
						parsewanchannel(cur->FirstChild(),id);
					}

					id = "";
					enable = false;
				}
			}
			cur=cur->NextSibling();
		}
	}

	void RouteParser::parsewanchannel(TiXmlNode * cur, std::string id){
		std::string 	classname = "";
		std::string 	rate = "";
		std::string 	comment = "";

		std::string 	classtype_str = "";
		std::string 	classid_str = "";
		std::string 	dir_str = "";
		std::string 	parentid_str = "";
		std::string 	shareid_str = "";
		std::string 	attribute_str = "";
		std::string 	prio_str = "";
		std::string 	limit_str = "";
		std::string 	weight_str = "";
		
		unsigned int 	classtype = 0;
		unsigned int 	classid = 0;
		unsigned int 	dir = 0;
		unsigned int 	parentid = 0;
		unsigned int 	shareid = 0;
		unsigned int 	attribute = 0;
		unsigned int 	prio = 0;
		unsigned int 	limit = 0;
		unsigned int 	weight = 0;
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "channel") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "classname") == 0){
							classname = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "classtype") == 0){
							classtype_str = attr->Value() ? attr->Value() : "";
							classtype = strtoint(classtype_str);
						}
						else if(strcmp(attr->Name(), "classid") == 0){
							classid_str = attr->Value() ? attr->Value() : "";
							classid = strtoint(classid_str);
						}
						else if(strcmp(attr->Name(), "dir") == 0){
							dir_str = attr->Value() ? attr->Value() : "";
							dir = strtoint(dir_str);
						}
						else if(strcmp(attr->Name(), "parentid") == 0){
							parentid_str = attr->Value() ? attr->Value() : "";
							parentid = strtoint(parentid_str);
						}
						else if(strcmp(attr->Name(), "shareid") == 0){
							shareid_str = attr->Value() ? attr->Value() : "";
							shareid = strtoint(shareid_str);
						}
						else if(strcmp(attr->Name(), "rate") == 0){
							rate = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "attribute") == 0){
							attribute_str = attr->Value() ? attr->Value() : "";
							attribute = strtoint(attribute_str);
						}
						else if(strcmp(attr->Name(), "prio") == 0){
							prio_str = attr->Value() ? attr->Value() : "";
							prio = strtoint(prio_str);
						}
						else if(strcmp(attr->Name(), "limit") == 0){
							limit_str = attr->Value() ? attr->Value() : "";
							limit = strtoint(limit_str);
						}
						else if(strcmp(attr->Name(), "weight") == 0){
							weight_str = attr->Value() ? attr->Value() : "";
							weight = strtoint(weight_str);
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					wanchannelmgr.wanChannelAddbyId(id, classname,classtype,classid,dir,parentid,shareid,rate,
							attribute,prio, limit,weight,comment);

					classtype_str = "";
					classid_str = "";
					dir_str = "";
					parentid_str = "";
					shareid_str = "";
					attribute_str = "";
					prio_str = "";
					limit_str = "";
					weight_str = "";
					classtype = 0;
					classid = 0;
					dir = 0;
					parentid = 0;
					shareid = 0;
					attribute = 0;
					prio = 0;
					limit = 0;
					weight = 0;

				//	if(cur->FirstChild()){
				//		parsewanchannel(cur->FirstChild(),id);
				//	}
				}
			}
			cur=cur->NextSibling();
		}
	}

	void   RouteParser::parseLanSubnet(TiXmlNode * cur){
		std::string ip = "";
		std::string mask = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "subnet") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "ip") == 0){
							ip = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "mask") == 0){
							mask = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}

					route_lanmgr.subnetadd(ip,mask);
					ip = "";
					mask = "";
				}
			}
			cur=cur->NextSibling();
		}
	}
	
	void	RouteParser::parseRouteInterfaceLan(TiXmlNode * cur){
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "ip") == 0){
					route_lanmgr.setLanIp(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "mask") == 0){
					route_lanmgr.setLanMask(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "mac") == 0){
					route_lanmgr.setLanMac(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "ifname") == 0){
					route_lanmgr.setLanIfname(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "dhcp") == 0){
					route_lanmgr.setLanDhcp(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "startip") == 0){
					route_lanmgr.setDhcpStartip(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "endip") == 0){
					route_lanmgr.setDhcpEndip(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "lease_time") == 0){
					route_lanmgr.setDhcpLeaseTime(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "gateway") == 0){
					route_lanmgr.setDhcpGateway(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "dhcp_mask") == 0){
					route_lanmgr.setDhcpMask(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "first_dns") == 0){
					route_lanmgr.setDhcpFirstDns(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "second_dns") == 0){
					route_lanmgr.setDhcpSecondDns(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "subnet_enable") == 0){
					route_lanmgr.setSubnetEnable(false);
					if(getNodeValue(cur).compare("1") == 0)
						route_lanmgr.setSubnetEnable(true);
				}
				else if(strcmp(cur->Value(), "subnets") == 0){
					parseLanSubnet(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}

	void	RouteParser::parseRouteInterfaceWan(TiXmlNode * cur){
	
        std::string id = "";
        std::string ifname = "";
		std::string access = "";
        std::string type = "";
        std::string ip = "";
        std::string mask = "";
        std::string gateway = "";
        std::string username = "";
        std::string passwd = "";
        std::string servername = "";
        std::string mtu = "";
        std::string mac = "";
        std::string dns = "";
        std::string up = "";
        std::string down = "";
        std::string isp = "";
		bool		default_route = false;
		bool		enable = false;
        std::string time = "";
        std::string week = "";
        std::string day = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "access") == 0){
						access = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "type") == 0){
						type = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ip") == 0){
						ip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mask") == 0){
						mask = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "gateway") == 0){
						gateway = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "username") == 0){
						username = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "passwd") == 0){
						passwd = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "servername") == 0){
						servername = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mtu") == 0){
						mtu = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mac") == 0){
						mac = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dns") == 0){
						dns = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "up") == 0){
						up = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "down") == 0){
						down = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "isp") == 0){
						isp = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "default") == 0){
						if(strcmp(attr->Value(),"0") == 0){
							default_route = false;
						}
						else{
							default_route = true;
						}
					}
					else if(strcmp(attr->Name(), "time") == 0){
						time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "week") == 0){
						week = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "day") == 0){
						day = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(),"0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					attr=attr->Next();
				}
				route_wanmgr.add(id,ifname,access,type,ip,mask,gateway,username,passwd,servername,mtu,mac,dns,up,down,isp,default_route,time,week,day,enable);
				
		        id = "";
		        ifname = "";
				access = "";
		        type = "";
		        ip = "";
		        mask = "";
		        gateway = "";
		        username = "";
		        passwd = "";
		        servername = "";
		        mtu = "";
		        mac = "";
		        dns = "";
		        up = "";
		        down = "";
		        isp = "";
				default_route = false;
		        time = "";
		        week = "";
		        day = "";
				
				if(cur->NextSibling()){
					parseRouteInterfaceWan(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void RouteParser::parseRouteInterfaceLink(TiXmlNode * cur){
		std::string ifname = "";
		std::string type = "";
		std::string mode = "";
		std::string speed = "";
		std::string duplex = "";
		std::string linkmode = "";
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "type") == 0){
						type = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mode") == 0){
						mode = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "speed") == 0){
						speed = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "duplex") == 0){
						duplex = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "linkmode") == 0){
						linkmode = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				route_interfacelinkmgr.add(ifname,type, mode, speed,duplex,linkmode);
				
				ifname = "";
				type = "";
				mode = "";
				speed = "";
				duplex = "";
				linkmode = "";
				
				if(cur->NextSibling()){
					parseRouteInterfaceLink(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteInterfaceVLan(TiXmlNode * cur){
		std::string name = "";
		std::string ifname = "";
		std::string mac = "";
		std::string id = "";
		std::string comment = "";
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mac") == 0){
						mac = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				route_vlanmgr.add(name,ifname, id, mac,comment);
				
				name = "";
				ifname = "";
				id = "";
				mac = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteInterfaceVLan(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteInterfaceMacVLan(TiXmlNode * cur){
		std::string name = "";
		std::string ifname = "";
		std::string mac = "";
		std::string comment = "";
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mac") == 0){
						mac = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				route_macvlanmgr.add(name,ifname, mac,comment);
				
				name = "";
				ifname = "";
				mac = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteInterfaceMacVLan(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteTables(TiXmlNode * cur){
		std::string id = "";
		bool enable = true;
		std::string value = "";
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(),"0") == 0)
							enable = false;
					}
					else if(strcmp(attr->Name(), "value") == 0){
						value = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				routetablesmgr.add(id,enable,value);
				
				id = "";
				enable = true;
				value = "";
				
				if(cur->NextSibling()){
					parseRouteTables(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteADSLClient(TiXmlNode * cur){
	
		std::string name = "";
		std::string ifname = "";
		std::string username = "";
		std::string password = "";
		std::string servername = "";
		std::string mtu = "1480";
		std::string weight = "1";
		std::string comment = "";
		bool		defaultroute = false;
		bool		peerdns = false;
		bool		enable = false;
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "username") == 0){
						username = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "password") == 0){
						password = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "servername") == 0){
						servername = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mtu") == 0){
						mtu = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "weight") == 0){
						weight = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "defaultroute") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							defaultroute = false;
						}
						else{
							defaultroute = true;
						}
					}
					else if(strcmp(attr->Name(), "peerdns") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							peerdns = false;
						}
						else{
							peerdns = true;
						}
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_adslclient_mgr.add(name, ifname, username, password, servername, mtu, weight,
									defaultroute, peerdns, enable, comment);

				name = "";
				ifname = "";
				username = "";
				password = "";
				mtu = "1480";
				weight = "1";
				defaultroute = false;
				peerdns = false;
				enable = false;
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteADSLClient(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}


	void	RouteParser::parseRouteVPNClient(TiXmlNode * cur){
	
		std::string name = "";
		std::string ifname = "";
		std::string username = "";
		std::string password = "";
		std::string servername = "";
		std::string mtu = "1480";
		std::string weight = "1";
		std::string comment = "";
		bool		defaultroute = false;
		bool		peerdns = false;
		bool		enable = false;
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "username") == 0){
						username = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "password") == 0){
						password = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "servername") == 0){
						servername = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mtu") == 0){
						mtu = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "weight") == 0){
						weight = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "defaultroute") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							defaultroute = false;
						}
						else{
							defaultroute = true;
						}
					}
					else if(strcmp(attr->Name(), "peerdns") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							peerdns = false;
						}
						else{
							peerdns = true;
						}
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_vpnclient_mgr.add(name, ifname, username, password, servername, mtu, weight,
									defaultroute, peerdns, enable, comment);

				name = "";
				ifname = "";
				username = "";
				password = "";
				mtu = "1480";
				weight = "1";
				defaultroute = false;
				peerdns = false;
				enable = false;
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteVPNClient(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteRouting(TiXmlNode * cur){

		std::string id = "";
		std::string dst = "";
		std::string gateway = "";
		std::string ifname = "";
		std::string priority = "1";
		std::string table = "";
		std::string rule = "";
		std::string comment = "";
		bool		enable = false;
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dst") == 0){
						dst = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "gateway") == 0){
						gateway = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "priority") == 0){
						priority = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "table") == 0){
						table = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "rule") == 0){
						rule = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_routing_mgr.add(id, dst, gateway, ifname, priority, table, rule, enable, comment);
										
				id = "";
				dst = "";
				gateway = "";
				ifname = "";
				priority = "1";
				table = "";
				rule = "";
				comment = "";
				enable = false;

				if(cur->NextSibling()){
					parseRouteRouting(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}


	void	RouteParser::parseRouteLoadBalancing(TiXmlNode * cur){
		std::string name;
		std::string ifname;
		std::string rulename;
		std::string weight;
		std::string comment;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "rulename") == 0){
						rulename = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "weight") == 0){
						weight = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_loadb_mgr.add(name, ifname, rulename, weight, comment);
										
				name = "";
				ifname = "";
				rulename = "";
				weight = "";
				comment = "";

				if(cur->NextSibling()){
					parseRouteLoadBalancing(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}



	void	RouteParser::parseRouteTable(TiXmlNode * cur){
		
		std::string name = "";
		std::string comment = "";
		
		while(cur){

			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "group") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							group_name = attr->Value();
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}
						attr=attr->Next();
					}
					if(group_name.size() > 0){
						route_table_mgr.addGroup(group_name, comment);
					}
					comment = "";
				}
				else if(strcmp(cur->Value(), "include") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value();
						}
						attr=attr->Next();
					}
					if(name.size() > 0){
						route_table_mgr.addOnlySubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseRouteTable(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}



	void	RouteParser::parseRouteRule(TiXmlNode * cur){
	
		std::string name = "";
		std::string src = "";
		std::string dst = "";
		std::string protocol = "";
		std::string src_port = "";
		std::string dst_port = "";
		std::string tos = "";
		std::string fwmark = "";
		std::string action = "";
		std::string comment = "";
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "src") == 0){
						src = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dst") == 0){
						dst = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "protocol") == 0){
						protocol = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "src_port") == 0){
						src_port = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dst_port") == 0){
						dst_port = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "tos") == 0){
						tos = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "action") == 0){
						action = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "fwmark") == 0){
						fwmark = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_rule_mgr.add(name, src, dst, protocol, src_port, dst_port, tos, fwmark, action, comment);
										
				name = "";
				src = "";
				dst = "";
				protocol = "";
				src_port = "";
				dst_port = "";
				tos = "";
				fwmark = "";
				action = "";
				comment = "";

				if(cur->NextSibling()){
					parseRouteRule(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteDNAT(TiXmlNode * cur){
		std::string id = "";
		bool		enable = false;
		bool        loop_enable=false;
		std::string protocol_id = "";
		std::string comment = "";
		std::string wan_ip = "";
		std::string wan_port_before = "";
		std::string wan_port_after = "";
		std::string lan_port_before = "";
		std::string lan_port_after = "";
		std::string lan_ip = "";
		std::string ifname = "";
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "protocol_id") == 0){
						protocol_id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "wan_ip") == 0){
						wan_ip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "wan_port_before") == 0){
						wan_port_before = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "wan_port_after") == 0){
						wan_port_after = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "lan_port_before") == 0){
						lan_port_before = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "lan_port_after") == 0){
						lan_port_after = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "lan_ip") == 0){
						lan_ip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "loop") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							loop_enable = false;
						}
						else{
							loop_enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_dnat_mgr.add(id,enable, loop_enable,protocol_id, comment, wan_ip, wan_port_before, wan_port_after,
					lan_port_before, lan_port_after, lan_ip, ifname);

				id = "";
				enable = false;
				protocol_id = "";
				comment = "";
				wan_ip = "";
				wan_port_before = "";
				wan_port_after = "";
				lan_port_before = "";
				lan_port_after = "";
				lan_ip = "";
				ifname = "";

				if(cur->NextSibling()){
					parseRouteDNAT(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteSNAT(TiXmlNode * cur){

		std::string id = "";
		std::string ifname = "";
		std::string src = "";
		std::string dst = "";
		std::string protocol = "all";
		std::string action = "MASQUERADE";
		std::string dstip = "";
		std::string dstport = "";
		std::string comment = "";
		bool		enable = false;
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "src") == 0){
						src = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dst") == 0){
						dst = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "protocol") == 0){
						protocol = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "action") == 0){
						action = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dstip") == 0){
						dstip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dstport") == 0){
						dstport = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_snat_mgr.add(id, ifname, src, dst, protocol, action, dstip, dstport, enable, comment);
										
				id = "";
				ifname = "";
				src = "";
				dst = "";
				protocol = "all";
				action = "MASQUERADE";
				dstip = "";
				dstport = "";
				comment = "";
				enable = false;

				if(cur->NextSibling()){
					parseRouteSNAT(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}


	void	RouteParser::parseRouteMACBind(TiXmlNode * cur){
		std::string id = "";
		std::string ip = "";
		std::string mac = "";
		std::string action= "0";
		std::string bridge= "0";
		std::string comment = "";
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "learnt") == 0){
					route_macbind_mgr.setLearnt(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						route_macbind_mgr.setLearnt(true);
					}
				}
				
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ip") == 0){
						ip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mac") == 0){
						mac = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "action") == 0){
						action = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "bridge") == 0){
						bridge = attr->Value() ? attr->Value() : "";
					}
					
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				route_macbind_mgr.add(id, ip, mac, action, bridge, comment);
										
				id = "";
				ip = "";
				mac = "";
				action = "0";
				bridge = "0";
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteMACBind(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void 	RouteParser::parsePptpServer(TiXmlNode *cur){
		std::string startip = "";
		std::string endip = "";
		std::string localip = "";
		std::string dns = "";
		std::string entry = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "enable") == 0){
					pptpd.setPptpdEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						pptpd.setPptpdEnable(true);
					}
				}
				else if(strcmp(cur->Value(), "startip") == 0){
					pptpd.setPptpdstartip(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "endip") == 0){
					pptpd.setPptpdendip(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "localip") == 0){
					pptpd.setPptpdlocalip(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "dns") == 0){
					pptpd.setPptpddns(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "entry") == 0){
					pptpd.setPptpdentry(getNodeValue(cur));
				}
			}
			cur=cur->NextSibling();
		}
	}

	void   RouteParser::parsePptpAccount(TiXmlNode *cur){
		std::string username = "";
		std::string passwd = "";
		std::string ip = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){				
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "username") == 0){
						username = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "passwd") == 0){
						passwd = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ip") == 0){
						ip = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				pptpdaccountmgr.add(username,passwd,ip);
										
				username = "";
				passwd =  "";
				ip = "";
				
				if(cur->NextSibling()){
					parsePptpAccount(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}

	void	RouteParser::parseRouteDhcpd(TiXmlNode *cur){
		std::string name = "";
		std::string ifname = "";
		std::string range = "";
		std::string mask = "";
		std::string gateway = "";
		std::string dnsname = "";
		std::string dns = "";
		std::string lease_time = "";
		std::string max_lease_time = "";
		bool		enable = true;
		std::string comment = "";
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "dhcpd_enable") == 0){
					route_dhcpd_mgr.setDhcpdEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						route_dhcpd_mgr.setDhcpdEnable(true);
					}
				}
				
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ifname") == 0){
						ifname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "range") == 0){
						range = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mask") == 0){
						mask = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "gateway") == 0){
						gateway = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dnsname") == 0){
						dnsname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dns") == 0){
						dns = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "lease_time") == 0){
						lease_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "max_lease_time") == 0){
						max_lease_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				route_dhcpd_mgr.add(name, ifname, range, mask, gateway, dnsname, dns, 
									lease_time, max_lease_time, enable, comment);
										
				name = "";
				ifname = "";
				range = "";
				mask = "";
				gateway = "";
				dnsname = "";
				dns = "";
				lease_time = "";
				max_lease_time = "";
				enable = true;
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteDhcpd(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	RouteParser::parseRouteDhcpdHost(TiXmlNode * cur){
		std::string name = "";
		std::string dhcpname = "";
		std::string mac = "";
		std::string ip = "";
		std::string mask = "";
		std::string gateway = "";
		std::string dnsname = "";
		std::string dns = "";
		std::string lease_time = "";
		std::string max_lease_time = "";
		bool		enable = true;
		std::string comment = "";
	
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dhcpname") == 0){
						dhcpname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mac") == 0){
						mac = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ip") == 0){
						ip = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "mask") == 0){
						mask = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "gateway") == 0){
						gateway = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dnsname") == 0){
						dnsname = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dns") == 0){
						dns = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "lease_time") == 0){
						lease_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "max_lease_time") == 0){
						max_lease_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "enable") == 0){
						if(strcmp(attr->Value(), "0") == 0){
							enable = false;
						}
						else{
							enable = true;
						}
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				
				route_dhcpdhost_mgr.add(name, dhcpname, mac, ip, mask, gateway, dnsname, dns, 
									lease_time, max_lease_time, enable, comment);
										
				name = "";
				dhcpname = "";
				mac = "";
				ip = "";
				mask = "";
				gateway = "";
				dnsname = "";
				dns = "";
				lease_time = "";
				max_lease_time = "";
				enable = true;
				comment = "";
				
				if(cur->NextSibling()){
					parseRouteDhcpdHost(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}
	

	std::string RouteParser::getNodeValue(TiXmlNode *node)
	{
		TiXmlNode *child;		
		std::string value = "";
		if(node == NULL)
		{
			return value;
		}
		if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			return value;
		}
		child = node;
	
		while(child && child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			child = child->FirstChild();
		}
		if(child == NULL || child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			return value;
		}
		value = child->ToText()->Value();
		return value;		
	}

	std::string RouteParser::getNodeValuebyName(const std::string nodename){
		TiXmlNode *node;	

		node = findNode(&doc, "xmlconfig/" + nodename);
		return getNodeValue(node);
	}


	TiXmlNode  *RouteParser::findNode(TiXmlNode *root, const std::string nodename)
	{

		TiXmlNode * parent = root;
		TiXmlNode * child = NULL;
		std::vector<std::string> token;

		// split the node name to keyword token
		split(nodename, &token);

		for(unsigned int i = 0; i < token.size(); i++)
		{
			for(child = parent->FirstChild(); child; child = child->NextSibling())
			{
				if(strcmp(child->Value(), token.at(i).c_str()) == 0)
				{
					parent = child;
					break;
				}
			}
		}
		token.clear();
		return child;
	}

	/*	Split string of names separated by SPLIT_CHAR  */
	void RouteParser::split(std::string instr, std::vector<std::string> *lstoutstr , const char* splitchar) {

		std::string str = instr;
		unsigned int loc;
		/*	Break string into substrings and store beginning  */
		while(1){
			loc = str.find(splitchar);
			if( loc == std::string::npos ){
				lstoutstr->push_back(str);
				return;
			}
			std::string substr = str.substr(0, loc);
			str.erase(0, loc + 1);
			lstoutstr->push_back(substr);
		}
	}

	//extern C,CGIC Code interface
	void * RouteParser::AllocBuf(int size)
	{
		void * tmpbuf = NULL;
		tmpbuf = malloc(sizeof(char) * size);
		return tmpbuf;	
	}


  	int RouteParser:: processRouteDDns(){ /* writen by wuke @ 2009.12.17 */
		int ret = 1;
		std::string enable;

		enable = getNodeValuebyName("ddns/enable_ddns");
		
		if(enable.compare("1") == 0)
		{
			ddns.setDDnsEnable(true);
						
		}else
		{
			ddns.setDDnsEnable(false);
			
		}
		ddns.setDDnsProvider(getNodeValuebyName("ddns/provider"));
		ddns.setUsername(getNodeValuebyName("ddns/username"));
		ddns.setPassword(getNodeValuebyName("ddns/password"));
		ddns.setDDnsDns(getNodeValuebyName("ddns/dns"));

		if(ddns.isVaild())
		{
			ret = 0;
		}
		else
		{
			error("dynamic dns configure is not valid!");
		}
		
		return ret;
	}
/*
	int RouteParser::DeleteXmlRouteMacBind(const char *filename,struct mgtcgi_xml_mac_binds **node)
	{
		#define BUF_MAC_BIND_LEN  125000
		char *buf = NULL;
		char path[]="/var/lock/config.lock";
		int struct_size = 0;
		int bufindex = 0;
		int index,fd;
		std::string ip = *node->pinfo[0].ip;
		std::string mac = *node->pinfo[0].mac;
		std::vector<Node*> lst;
		std::string strtemp="";
		struct mgtcgi_xml_mac_binds * tmpPtr = NULL;

		buf = (char *)malloc(BUF_MAC_BIND_LEN);
		if(NULL == buf){
			return 0;
		}

		maxnet::Log pLog("/var/log/mac_bind_lib.log", false,false,false);	
		const char* main_format_str =	
		"<mac_binds>\n"
		"<learnt>%d</learnt>\n"
		"%s"
		"</mac_binds>";
	
		const char* format_str =	
		"	<macbind id=\"%d\" ip=\"%s\" mac=\"%s\" action=\"%d\" bridge=\"%d\" comment=\"%s\" />\n";
		
		if(processRouteMACBind() > 0)
			return RET_ERROR;
		route_macbind_mgr.del(ip,mac);

		lst = route_macbind_mgr.getList();	
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteMACBind *item = (RouteMACBind *)lst.at(i);
			if(item->isGroup()){
				continue;
			}

			bufindex += snprintf(buf + bufindex, sizeof(buf) -bufindex , format_str,
			strtoint(item->getID()),
			item->getIP().c_str(),
			item->getMAC().c_str(),
			strtoint(item->getAction()),
			strtoint(item->getBridge()),
			item->getCommand().c_str(),);
		}
		strtemp = buf;
		memset(buf,'\0',sizeof(buf));
		snprintf(buf, sizeof(buf) - strlen(buf), main_format_str, route_macbind_mgr.getLearnt()?1:0, strtemp.c_str());

		fd=open(path,O_WRONLY|O_CREAT);
    		if(flock(fd,LOCK_EX)!=0){
			free(buf);
			close(fd);
			return XML_ERROR;
    		}
		maxnet::RWXml rwxml(&pLog , filename);
		if(rwxml.replaceNode("mac_binds" , buf) == 0){
			flock(fd,LOCK_UN);
			free(buf);
			buf = NULL;
			buf(fd);
			return bufindex;	
		}

		free(buf);
		buf = NULL;
		flock(fd,LOCK_UN);
		close(fd);
		return XML_ERROR;
	}
		
		struct_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				route_macbind_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_mac_binds_info);
		
		*p_xml =(struct mgtcgi_xml_mac_binds*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_macbind_mgr.dataNodeCount; 
		tmpPtr->learnt = route_macbind_mgr.getLearnt()?1:0;
		lst = route_macbind_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteMACBind *item = (RouteMACBind *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			strncpy(tmpPtr->pinfo[iIndex].ip, item->getIP().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mac , item->getMAC().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].action = strtoint(item->getAction());
			tmpPtr->pinfo[iIndex].bridge= strtoint(item->getBridge());
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
}
	*/
	int RouteParser::GetStructRouteDNS(struct mgtcgi_xml_dns ** p_xml)
	{
		struct mgtcgi_xml_dns * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;

		if(processRouteDNS() > 0)
			return RET_ERROR;
		
		*p_xml =(struct mgtcgi_xml_dns*)
						malloc(sizeof(struct mgtcgi_xml_dns));
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_dns));
		
		strncpy(tmpPtr->dns_primary, route_dns.getDNSPrimary().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->dns_secondary, route_dns.getDNSSecondary().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->dns_thirdary, route_dns.getDNSThirdary().c_str(),STRING_LENGTH-1);

		return sizeof(struct mgtcgi_xml_dns);
	}

	int RouteParser::GetStructWansChannnel(struct mgtcgi_xml_wans_channels ** pxmlwanschannel)
	{
		int caclSize= 0;
		int iIndex = 0;
		U8 id = 0;
		U8 enable = 0;
		std::vector<WanChannel *> lst;
		std::vector<Channel *> channel_list;
		
		struct mgtcgi_xml_wans_channels * tmpPtr = NULL;
		if(processWansChannel() > 0)
			return RET_ERROR;
		
		if(*pxmlwanschannel != NULL)
			return RET_ERROR;
		
		caclSize = sizeof(struct mgtcgi_xml_wans_channels) + 
				wanchannelmgr.getDataNum()* 
					sizeof(struct mgtcgi_xml_wans_channel_info);
		
		*pxmlwanschannel = (struct mgtcgi_xml_wans_channels*)AllocBuf(caclSize);
		if(NULL == *pxmlwanschannel){
			return RET_ERROR;
		}	
		tmpPtr = *pxmlwanschannel;
		memset( tmpPtr , 0 ,  caclSize);	
		
		tmpPtr->num = wanchannelmgr.getDataNum();
		lst = wanchannelmgr.getWanChannelList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			WanChannel* item = (WanChannel *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
			
			id = strtoint(item->getId());
			enable = (item->getEnable()?1:0);
			channel_list = item->getchannelList();
			for(unsigned int j = 0; j < channel_list.size(); j++){
				Channel *tmp_channel = channel_list.at(j);
				
				tmpPtr->pinfo[iIndex].id = id;
				tmpPtr->pinfo[iIndex].enable = enable;
				tmpPtr->pinfo[iIndex].classtype = tmp_channel->getClassType();
				tmpPtr->pinfo[iIndex].classid = tmp_channel->getClassId();
				tmpPtr->pinfo[iIndex].parentid = tmp_channel->getParentId();
				tmpPtr->pinfo[iIndex].shareid = tmp_channel->getShareId();
				tmpPtr->pinfo[iIndex].priority = tmp_channel->getPriority();
				tmpPtr->pinfo[iIndex].limit = tmp_channel->getLimit();
				tmpPtr->pinfo[iIndex].attribute = tmp_channel->getAttribute();
				tmpPtr->pinfo[iIndex].weight = tmp_channel->getWeight();
				tmpPtr->pinfo[iIndex].direct = tmp_channel->getDir();
				strncpy(tmpPtr->pinfo[iIndex].classname,tmp_channel->getClassName().c_str(),LITTER_STRING-1);
				strncpy(tmpPtr->pinfo[iIndex].rate ,tmp_channel->getRate().c_str(),LITTER_STRING-1);
				strncpy(tmpPtr->pinfo[iIndex].comment,tmp_channel->getComment().c_str(),LITTER_LENGTH-1);

				++iIndex;
			}	
		}
		return caclSize;
	}

	int RouteParser::GetStructRouteDDNS(struct mgtcgi_xml_ddns ** p_xml)	
	{
		struct mgtcgi_xml_ddns * tmpPtr = NULL;		

		if(*p_xml != NULL)
		{	
			return RET_ERROR;
		}	

		if(processRouteDDns() > 0)
		{
			return RET_ERROR;
		}
		
		*p_xml =(struct mgtcgi_xml_ddns*)malloc(sizeof(struct mgtcgi_xml_ddns));
	
		if(NULL == *p_xml)
		{
			return RET_ERROR;
		}		

		tmpPtr = *p_xml;	
		memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_ddns));		
		tmpPtr->enable_ddns = ddns.isDDnsEnable() ? 1 : 0;
	
		strncpy(tmpPtr->username, ddns.getUsername().c_str(),STRING_LENGTH-1);		
		strncpy(tmpPtr->password, ddns.getPassword().c_str(),STRING_LENGTH-1);		
		strncpy(tmpPtr->dns, ddns.getDDnsDns().c_str(),STRING_LENGTH-1);			
		strncpy(tmpPtr->provider, ddns.getDDnsProvider().c_str(), STRING_LENGTH-1);
	
		return sizeof(struct mgtcgi_xml_ddns);
	}

	
int RouteParser::GetStructRouteInterfaceLAN(struct mgtcgi_xml_route_interface_lans ** p_xml)
{
	int struct_size = 0;	
	int iIndex = 0;	
	std::vector<RouteLan*> lst;	
	struct mgtcgi_xml_route_interface_lans * tmpPtr = NULL;		
	if(*p_xml != NULL)	
		return RET_ERROR;		
	if(processRouteInterfaceLan() > 0)
		return RET_ERROR;
	
	struct_size = sizeof(struct mgtcgi_xml_route_interface_lans) + 	
			route_lanmgr.getsubnetdatacount() * 	
			sizeof(struct mgtcgi_xml_interface_lan_subnets_info);	
	*p_xml =(struct mgtcgi_xml_route_interface_lans*)malloc(struct_size);
		if(NULL == *p_xml){	
		return RET_ERROR;	
	}		
	tmpPtr = *p_xml;	
	memset(tmpPtr , 0 ,  struct_size);	

	tmpPtr->sunbet_enable = route_lanmgr.getSubnetEnable()?1:0;
	tmpPtr->subnet_num = route_lanmgr.getsubnetdatacount(); 
	tmpPtr->dhcp_info.lease_time = strtoint(route_lanmgr.getDhcpLeaseTime());
	strncpy(tmpPtr->ip, route_lanmgr.getLanIp().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->mask, route_lanmgr.getLanMask().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->mac, route_lanmgr.getLanMac().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->ifname, route_lanmgr.getLanIfname().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_type, route_lanmgr.getLanDhcp().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.startip, route_lanmgr.getDhcpStartip().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.endip, route_lanmgr.getDhcpEndip().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.gateway, route_lanmgr.getDhcpGateway().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.dhcp_mask, route_lanmgr.getDhcpMask().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.first_dns, route_lanmgr.getDhcpFirstDns().c_str(),LITTER_LENGTH-1);
	strncpy(tmpPtr->dhcp_info.second_dns, route_lanmgr.getDhcpSecondDns().c_str(),LITTER_LENGTH-1);
	
	lst = route_lanmgr.getlansubnetlist();	
	for(unsigned int i = 0; i < lst.size(); i++){	
		RouteLan *item = (RouteLan *) lst.at(i);	
		if(item->isGroup()){	
			continue;	
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].ip, item->getIP().c_str(),LITTER_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mask, item->getMask().c_str(),LITTER_LENGTH-1);		
		++iIndex;	
	}	
	return struct_size;
	
}

	int RouteParser::GetStructRouteInterfaceWAN(struct mgtcgi_xml_interface_wans ** p_xml)
	{	
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_interface_wans * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteInterfaceWan() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_interface_wans) + 
				route_wanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_interface_wans_info);
		
		*p_xml =(struct mgtcgi_xml_interface_wans*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  struct_size); 		
		
		tmpPtr->num = route_wanmgr.dataNodeCount; 
		lst = route_wanmgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteWan *item = (RouteWan *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			tmpPtr->pinfo[iIndex].defaultroute = (item->getDefaultRoute()?1:0);
			tmpPtr->pinfo[iIndex].enable = (item->getEnable()?1:0);
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].access, item->getAccess().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].type, item->getType().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ip, item->getIP().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mask, item->getMask().c_str(),LITTER_LENGTH-1);	
			strncpy(tmpPtr->pinfo[iIndex].gateway, item->getGateway().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].username, item->getUsername().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].passwd, item->getPasswd().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].servername, item->getServername().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mtu, item->getMtu().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mac, item->getMac().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].dns, item->getDNS().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].up, item->getUp().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].down, item->getDown().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].isp, item->getIsp().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].time, item->getTime().c_str(),LITTER_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].week, item->getWeek().c_str(),LITTER_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].day, item->getDay().c_str(),LITTER_LENGTH-1); 
			++iIndex;
		}
	
		return struct_size;
	}

	int RouteParser::GetStructRouteInterfaceLink(struct mgtcgi_xml_interface_links ** p_xml)
	{	
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_interface_links * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteInterfaceLink() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_interface_links) + 
				route_interfacelinkmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_interface_link_info);
		
		*p_xml =(struct mgtcgi_xml_interface_links*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  struct_size); 		
		
		tmpPtr->num = route_interfacelinkmgr.dataNodeCount; 
		lst = route_interfacelinkmgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			Route_InterfaceLink *item = (Route_InterfaceLink *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].speed = strtoint(item->getSpeed());
			tmpPtr->pinfo[iIndex].linkmode = strtoint(item->getlinkstate());
			strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIfname().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].type, item->getType().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mode, item->getMode().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].duplex, item->getDuplex().c_str(),LITTER_LENGTH-1);
			++iIndex;
		}
	
		return struct_size;
	}

	int RouteParser::GetStructRouteInterfaceVLAN(struct mgtcgi_xml_interface_vlans ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_interface_vlans * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteInterfaceVLan() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_interface_vlans) + 
				route_vlanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_interface_vlans_info);
		
		*p_xml =(struct mgtcgi_xml_interface_vlans*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  struct_size);		
		
		tmpPtr->num = route_vlanmgr.dataNodeCount; 
		lst = route_vlanmgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteVLan *item = (RouteVLan *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
	
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mac , item->getMAC().c_str(),LITTER_LENGTH-1);
			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),LITTER_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}

	int RouteParser::GetStructRouteInterfaceMacVLAN(struct mgtcgi_xml_interface_macvlans ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_interface_macvlans * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
		
		if(processRouteInterfaceMacVLan() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_interface_macvlans) + 
				route_macvlanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_interface_macvlans_info);
		
		*p_xml =(struct mgtcgi_xml_interface_macvlans*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  struct_size);		
		
		tmpPtr->num = route_macvlanmgr.dataNodeCount; 
		lst = route_macvlanmgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteMacVLan *item = (RouteMacVLan *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
	
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mac , item->getMAC().c_str(),LITTER_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),COMMENT_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}

	int RouteParser::GetStructRouteTables(struct mgtcgi_xml_route_tables ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_route_tables * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
		
		if(processRouteTables() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_route_tables) + 
				routetablesmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_route_tables_info);
		
		*p_xml =(struct mgtcgi_xml_route_tables*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  struct_size);		
		
		tmpPtr->num = routetablesmgr.dataNodeCount; 
		lst = routetablesmgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteTables *item = (RouteTables *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
	
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			tmpPtr->pinfo[iIndex].id = strtoint(item->getId());
			strncpy(tmpPtr->pinfo[iIndex].value, item->getValue().c_str(),LITTER_LENGTH-1);
			++iIndex;
		}
		return struct_size;
	}
	
	int RouteParser::GetStructRoutePPPDAuth(struct mgtcgi_xml_pppd_authenticate ** p_xml)
	{
		struct mgtcgi_xml_pppd_authenticate * tmpPtr = NULL;

		if(*p_xml != NULL)
			return RET_ERROR;

		if(processRoutePPPDAuthenticate() > 0)
			return RET_ERROR;
		
		*p_xml =(struct mgtcgi_xml_pppd_authenticate*)
						malloc(sizeof(struct mgtcgi_xml_pppd_authenticate));
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_pppd_authenticate));
		
		tmpPtr->pap = route_pppdauth.getRequirePAP()?1:0;
		tmpPtr->chap = route_pppdauth.getRequireCHAP()?1:0;
		tmpPtr->mschap = route_pppdauth.getRequireMSCHAP()?1:0;
		tmpPtr->mschapv2 = route_pppdauth.getRequireMSCHAPV2()?1:0;
		tmpPtr->eap = route_pppdauth.getRequireEAP()?1:0;
		
		return sizeof(struct mgtcgi_xml_pppd_authenticate);
	}

	int RouteParser::GetStructRouteADSLClient(struct mgtcgi_xml_adsl_clients ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_adsl_clients * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteADSLClient() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_adsl_clients) + 
				route_adslclient_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
		
		*p_xml =(struct mgtcgi_xml_adsl_clients*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size); 		
		
		tmpPtr->num = route_adslclient_mgr.dataNodeCount; 
		lst = route_adslclient_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteADSLClient *item = (RouteADSLClient *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
			
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].username, item->getUserName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].password, item->getPassword().c_str(),STRING_LENGTH-1);	
			strncpy(tmpPtr->pinfo[iIndex].servername, item->getServerName().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].mtu = strtoint(item->getMTU());
			tmpPtr->pinfo[iIndex].weight = strtoint(item->getWeight());
			tmpPtr->pinfo[iIndex].defaultroute = item->getDefaultRoute()?1:0;
			tmpPtr->pinfo[iIndex].peerdns = item->getPeerDNS()?1:0;
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}

		return struct_size;
	}

	int RouteParser::GetStructRouteVPNClient(struct mgtcgi_xml_l2tpvpn_clients ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_l2tpvpn_clients * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteVPNClient() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_l2tpvpn_clients) + 
				route_vpnclient_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_l2tpvpn_clients_info);
		
		*p_xml =(struct mgtcgi_xml_l2tpvpn_clients*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_vpnclient_mgr.dataNodeCount; 
		lst = route_vpnclient_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteVPNClient *item = (RouteVPNClient *) lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
			
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].username, item->getUserName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].password, item->getPassword().c_str(),STRING_LENGTH-1);	
			strncpy(tmpPtr->pinfo[iIndex].servername, item->getServerName().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].mtu = strtoint(item->getMTU());
			tmpPtr->pinfo[iIndex].weight = strtoint(item->getWeight());
			tmpPtr->pinfo[iIndex].defaultroute = item->getDefaultRoute()?1:0;
			tmpPtr->pinfo[iIndex].peerdns = item->getPeerDNS()?1:0;
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}

		return struct_size;
	}


	int RouteParser::GetStructRouteLoadBalancing(struct mgtcgi_xml_load_balancings ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_load_balancings * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteLoadBalancing() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_load_balancings) + 
				route_loadb_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_load_balancings_info);
		
		*p_xml =(struct mgtcgi_xml_load_balancings*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_loadb_mgr.dataNodeCount; 
		lst = route_loadb_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteLoadBalancing *item = (RouteLoadBalancing *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIFName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].rulename, 	item->getRuleName().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].weight = strtoint(item->getWeight());
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}	
	

	int RouteParser::GetStructRouteRouting(struct mgtcgi_xml_routings ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_routings * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteRouting() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_routings) + 
				route_routing_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_routings_info);
		
		*p_xml =(struct mgtcgi_xml_routings*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_routing_mgr.dataNodeCount; 
		lst = route_routing_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteRouting *item = (RouteRouting *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
		
			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());	
			strncpy(tmpPtr->pinfo[iIndex].dst , item->getDST().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].gateway, item->getGateway().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIFName().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].priority = strtoint(item->getPriority());	
			strncpy(tmpPtr->pinfo[iIndex].table, item->getTable().c_str(),STRING_LENGTH-1);	
			strncpy(tmpPtr->pinfo[iIndex].rule, item->getRule().c_str(),STRING_LENGTH-1);	
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}

		return struct_size;
	}

	int RouteParser::GetStructRouteRule(struct mgtcgi_xml_route_rules ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_route_rules * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteRule() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_route_rules) + 
				route_rule_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_route_rules_info);
		
		*p_xml =(struct mgtcgi_xml_route_rules*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_rule_mgr.dataNodeCount; 
		lst = route_rule_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteRule *item = (RouteRule *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	
		
			strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].src, item->getSRC().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].dst, item->getDST().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].protocol, item->getProtocol().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].src_port, item->getSRCPort().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].dst_port, item->getDSTPort().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].tos, item->getTOS().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].action, item->getAction().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}

	int RouteParser::GetStructRouteDNAT(struct mgtcgi_xml_nat_dnats ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_nat_dnats * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteDNAT() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
				route_dnat_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_nat_dnats_info);
		
		*p_xml =(struct mgtcgi_xml_nat_dnats*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_dnat_mgr.dataNodeCount; 
		lst = route_dnat_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteDNAT *item = (RouteDNAT *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			strncpy(tmpPtr->pinfo[iIndex].protocol, item->getProtocoLId().c_str(),LITTER_STRING-1); 
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getcomment().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].wan_ip, item->getWanIp().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].wan_port_before, item->getWanPortBefore().c_str(),DIGITAL_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].wan_port_after, item->getWanPortAfter().c_str(),DIGITAL_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].lan_port_before, item->getLanPortBefore().c_str(),DIGITAL_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].lan_port_after, item->getLANPortAfter().c_str(),DIGITAL_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].lan_ip, item->getLanIp().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIfname().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			tmpPtr->pinfo[iIndex].loop_enable = item->getloopEnable()?1:0;
			
			++iIndex;
		}
		return struct_size;
	}

	int RouteParser::GetStructRouteSNAT(struct mgtcgi_xml_nat_snats ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_nat_snats * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteSNAT() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_nat_snats) + 
				route_snat_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_nat_snats_info);
		
		*p_xml =(struct mgtcgi_xml_nat_snats*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_snat_mgr.dataNodeCount; 
		lst = route_snat_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteSNAT *item = (RouteSNAT *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			strncpy(tmpPtr->pinfo[iIndex].ifname , item->getIfname().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].src , item->getSRC().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].dst , item->getDST().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].protocol, item->getProtocol().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].action, item->getAction().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].dstip, item->getDSTIP().c_str(),STRING_LENGTH-1); 
			strncpy(tmpPtr->pinfo[iIndex].dstport, item->getDSTPort().c_str(),STRING_LENGTH-1); 
			tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}


	int RouteParser::GetStructRouteMACBind(struct mgtcgi_xml_mac_binds ** p_xml)
	{
		int struct_size = 0;
		int iIndex = 0;
		std::vector<Node*> lst;
		struct mgtcgi_xml_mac_binds * tmpPtr = NULL;
		
		if(*p_xml != NULL)
			return RET_ERROR;
			
		if(processRouteMACBind() > 0)
			return RET_ERROR;
		
		struct_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				route_macbind_mgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_mac_binds_info);
		
		*p_xml =(struct mgtcgi_xml_mac_binds*)malloc(struct_size);
		if(NULL == *p_xml){
			return RET_ERROR;
		}	
		tmpPtr = *p_xml;
		memset(tmpPtr , 0, struct_size);		
		
		tmpPtr->num = route_macbind_mgr.dataNodeCount; 
		tmpPtr->learnt = route_macbind_mgr.getLearnt()?1:0;
		lst = route_macbind_mgr.getList();
		
		for(unsigned int i = 0; i < lst.size(); i++){
			RouteMACBind *item = (RouteMACBind *)lst.at(i);
			
			if(item->isGroup()){
				continue;
			}	

			tmpPtr->pinfo[iIndex].id = strtoint(item->getID());
			strncpy(tmpPtr->pinfo[iIndex].ip, item->getIP().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].mac , item->getMAC().c_str(),STRING_LENGTH-1);
			tmpPtr->pinfo[iIndex].action = strtoint(item->getAction());
			tmpPtr->pinfo[iIndex].bridge= strtoint(item->getBridge());
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
			++iIndex;
		}
		return struct_size;
	}

int RouteParser::GetStructPptpdServer(struct mgtcgi_xml_pptpd_server **p_xml)
{
	int struct_size = 0;
	struct mgtcgi_xml_pptpd_server * tmpPtr = NULL;
	
	if(*p_xml != NULL)
		return RET_ERROR;
		
	if(processPptpServer() > 0)
		return RET_ERROR;
	
	struct_size = sizeof(struct mgtcgi_xml_pptpd_server);
	
	*p_xml =(struct mgtcgi_xml_pptpd_server*)malloc(struct_size);
	if(NULL == *p_xml){
		return RET_ERROR;
	}	
	tmpPtr = *p_xml;
	memset(tmpPtr , 0, struct_size);		
	
	tmpPtr->enable = (pptpd.getPptpdEnable()?1:0);
	strncpy(tmpPtr->localip,pptpd.getPptpdlocalip().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->startip,pptpd.getPptpdstartip().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->endip,pptpd.getPptpdendip().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->dns,pptpd.getPptpddns().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->entry_mode,pptpd.getPptpdentry().c_str(),STRING_LENGTH-1);
	return struct_size;
}

int RouteParser::GetStructPptpdAccount(struct mgtcgi_xml_pptpd_accounts_info **p_xml)
{
	int struct_size = 0;
	int index = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_pptpd_accounts_info * tmpPtr = NULL;

	if(*p_xml != NULL)
		return RET_ERROR;

	if(processPptpdAccount() > 0){
		return RET_ERROR;
	}

	struct_size = sizeof(struct mgtcgi_xml_pptpd_accounts_info) + 
		pptpdaccountmgr.dataNodeCount *
		sizeof(struct mgtcgi_xml_pptpd_account);
	*p_xml = (struct mgtcgi_xml_pptpd_accounts_info *)malloc(struct_size);
	if(NULL == *p_xml){
		return RET_ERROR;
	}
	tmpPtr = *p_xml;
	memset(tmpPtr,0,struct_size);

	tmpPtr->num = pptpdaccountmgr.dataNodeCount;
	lst = pptpdaccountmgr.getList();
	for(unsigned int i=0;i<lst.size();i++){
		PptpdAccount *pptpaccount = (PptpdAccount *)lst.at(i);
		if(pptpaccount->isValid() == false)
			continue;

		strncpy(tmpPtr->pinfo[index].username, pptpaccount->getusername().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[index].passwd, pptpaccount->getpasswd().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[index].ip, pptpaccount->getip().c_str(), STRING_LENGTH-1);
		index++;
	}

	return struct_size;
}

int RouteParser::GetStructRouteDhcpd(struct mgtcgi_xml_dhcpd_servers ** p_xml)
{
	int struct_size = 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_dhcpd_servers * tmpPtr = NULL;
	
	if(*p_xml != NULL)
		return RET_ERROR;
		
	if(processRouteDhcpd() > 0)
		return RET_ERROR;
	
	struct_size = sizeof(struct mgtcgi_xml_dhcpd_servers) + 
			route_dhcpd_mgr.dataNodeCount * 
			sizeof(struct mgtcgi_xml_dhcpd_servers_info);
	
	*p_xml =(struct mgtcgi_xml_dhcpd_servers*)malloc(struct_size);
	if(NULL == *p_xml){
		return RET_ERROR;
	}	
	tmpPtr = *p_xml;
	memset(tmpPtr , 0, struct_size);		
	
	tmpPtr->num = route_dhcpd_mgr.dataNodeCount; 
	tmpPtr->dhcpd_enable = route_dhcpd_mgr.getDhcpdEnable()?1:0;

	lst = route_dhcpd_mgr.getList();
	for(unsigned int i = 0; i < lst.size(); i++){
		RouteDhcpd *item = (RouteDhcpd *)lst.at(i);

		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name, item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIfname().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].range, item->getRange().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mask, item->getMask().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].gateway, item->getGateway().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dnsname, item->getDnsName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dns, item->getDNS().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].lease_time= strtoint(item->getLeaseTime());
		tmpPtr->pinfo[iIndex].max_lease_time= strtoint(item->getMaxLeaseTime());
		tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
		++iIndex;
	}
	return struct_size;
}

int RouteParser::GetStructRouteDhcpdHost(struct mgtcgi_xml_dhcpd_hosts ** p_xml)
{
	int struct_size = 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_dhcpd_hosts * tmpPtr = NULL;
	
	if(*p_xml != NULL)
		return RET_ERROR;
		
	if(processRouteDhcpdHost() > 0)
		return RET_ERROR;
	
	struct_size = sizeof(struct mgtcgi_xml_dhcpd_hosts) + 
			route_dhcpdhost_mgr.dataNodeCount * 
			sizeof(struct mgtcgi_xml_dhcpd_hosts_info);
	
	*p_xml =(struct mgtcgi_xml_dhcpd_hosts*)malloc(struct_size);
	if(NULL == *p_xml){
		return RET_ERROR;
	}	
	tmpPtr = *p_xml;
	memset(tmpPtr , 0, struct_size);		
	
	tmpPtr->num = route_dhcpdhost_mgr.dataNodeCount; 

	lst = route_dhcpdhost_mgr.getList();
	for(unsigned int i = 0; i < lst.size(); i++){
		RouteDhcpdHost *item = (RouteDhcpdHost *)lst.at(i);

		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name, item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dhcpname, item->getDhcpName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mac, item->getMAC().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].ip, item->getIP().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mask, item->getMask().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].gateway, item->getGateway().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dnsname, item->getDnsName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dns, item->getDNS().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].lease_time= strtoint(item->getLeaseTime());
		tmpPtr->pinfo[iIndex].max_lease_time= strtoint(item->getMaxLeaseTime());
		tmpPtr->pinfo[iIndex].enable = item->getEnable()?1:0;
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1); 
		++iIndex;
	}
	return struct_size;
}

#define XML_GROUP_DEBUG 0
	int RouteParser::GetStructXmlRouteGroups(struct mgtcgi_xml_group_array ** pxmlgroup,
				 NodeMgr & nodemgr)
	{
		std::string strtmp;
		int caclSize= 0;
		int iIndex = 0;
		int iInclude = 0;
		struct mgtcgi_xml_group_include_info * pIncludeStart = NULL;	
		struct mgtcgi_xml_group_include_info * pTempInclude = NULL;
		std::vector<Node*> lst;
		std::vector<std::string> subnodelst;
		struct mgtcgi_xml_group_array * tmpPtr = NULL;
	
		if(*pxmlgroup != NULL)
			return RET_ERROR;
			
		caclSize = sizeof(struct mgtcgi_xml_group_array) + 
				nodemgr.groupNodeCount * 
					sizeof(struct mgtcgi_xml_group) +
				nodemgr.includeNodeCount * 
					sizeof(struct mgtcgi_xml_group_include_info);
					
	#if XML_GROUP_DEBUG
		printf("caclSize=%d ; nodemgr.groupNodeCount =%d , nodemgr.includeNodeCount=%d\n",
			caclSize,nodemgr.groupNodeCount, nodemgr.includeNodeCount );
	#endif	
	
		*pxmlgroup =(struct mgtcgi_xml_group_array*)malloc(caclSize);
		if(NULL == *pxmlgroup){ 	
			return RET_ERROR;
		}	
			
		tmpPtr = *pxmlgroup;
		memset( tmpPtr , 0 ,  caclSize);	
		pIncludeStart = (struct mgtcgi_xml_group_include_info *)
					((char*)tmpPtr + sizeof(struct mgtcgi_xml_group_array) + 
						nodemgr.groupNodeCount * 
							sizeof(struct mgtcgi_xml_group));	
		tmpPtr->num = nodemgr.groupNodeCount;
		lst = nodemgr.getList();
		
	#if XML_GROUP_DEBUG
		printf("pInclude index=%d\n",sizeof(struct mgtcgi_xml_group_array) + 
				nodemgr.groupNodeCount *sizeof(struct mgtcgi_xml_group));
		printf("pIncludeStart-tmpPtr=%x\n",(char*)pIncludeStart-(char*)tmpPtr);
		printf("Group Count=%d\n",tmpPtr->num);
	#endif
	
		for(unsigned int i = 0; i < lst.size(); i++){
			Node* item = (Node *) lst.at(i);
			
			if(! item->isGroup()){
				continue;
			}
	
			subnodelst = item->getSubNodes();
			
			tmpPtr->pinfo[iIndex].num = subnodelst.size();
			tmpPtr->pinfo[iIndex].pinfo = (struct mgtcgi_xml_group_include_info  *)((char*)pIncludeStart + 
					iInclude * sizeof(struct mgtcgi_xml_group_include_info));
			
			strncpy(tmpPtr->pinfo[iIndex].group_name, item->getName().c_str(),STRING_LENGTH-1);
			strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),COMMENT_LENGTH-1);
			iInclude += subnodelst.size();
		
			pTempInclude = tmpPtr->pinfo[iIndex].pinfo;
			
	#if XML_GROUP_DEBUG
			printf("pIncludeStart=%x iInclude= %d pTempInclude=%x\n",
						pIncludeStart,iInclude,pTempInclude);
		printf("group_name=%s ,include Count=%d\n",
					tmpPtr->pinfo[iIndex].group_name,
					tmpPtr->pinfo[iIndex].num);
	#endif
				
			for(unsigned int k = 0; k < subnodelst.size(); k++){
				strncpy(pTempInclude[k].name , 
						subnodelst.at(k).c_str(),STRING_LENGTH-1);
			}
			
			++iIndex;
		}
		return caclSize;
	}

}
