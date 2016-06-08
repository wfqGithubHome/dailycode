#include <iostream>
#include <fstream>
#include "parser.h"
#include "../global.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "../core/portscanmgr.h"
#include "parser.h"


/*2012-10-07 debug head */
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include "../base/logwraper.h"

namespace maxnet{

	Parser::Parser(Log *log, const std::string main_config_filename,
						const std::string netaddr_config_filename,
						const std::string rfc_proto_config_filename,
						const std::string l7_proto_config_filename,
						const std::string server_info_config_filename,
						const int mgrAddDefault, int check, int print_fw,int print_action,
                        int print_portscan, int print_macbind, int print_macauth, 
						int print_quota,int print_globalip,	int print_portmirror, 
						int print_dnshelper , int print_httpdirpolicy)
						: netaddrmgr(mgrAddDefault),protocolmgr(mgrAddDefault),
						domainmgr(mgrAddDefault)
	{
		setLog(log);	
		setLogHeader("[P] ");
        
        this->main_config_filename = main_config_filename;
		this->netaddr_config_filename = netaddr_config_filename;
		this->rfc_proto_config_filename = rfc_proto_config_filename;
		this->l7_proto_config_filename = l7_proto_config_filename;
        this->server_info_config_filename = server_info_config_filename;
		this->check = check;
		this->print_fw = print_fw;
        this->print_portscan = print_portscan;
		this->print_action = print_action;
		this->print_macbind = print_macbind;
		this->print_macauth = print_macauth;
		this->print_quota     = print_quota;
		this->print_globalip = print_globalip;
		this->print_portmirror = print_portmirror;
		this->print_dnshelper = print_dnshelper;
		this->print_httpdirpolicy = print_httpdirpolicy;
	}

	Parser::~Parser(){
		mapLayer7prototype.clear();
		//mapLayer7protogid.clear();
		//doc.clear();
	}



#if 0
    /*21=012-10-07 add debug */ 

    static char log_buff[10000];

    int Parser::do_portscan_log(char *log)
    {
        int fd = 0;
        fd = open("/home/mgtcgi_log.txt", O_CREAT | O_APPEND | O_WRONLY,00644);
        
        write(fd, log, strlen(log));
        close(fd);  
    }
    
    void Parser::log_portscan_debug(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(log_buff,102400,fmt, args);
        do_portscan_log(log_buff);
        va_end(args);
    }

#endif

	int Parser::loadConfig(){
		if(!doc.LoadFile(main_config_filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr  << "open xml config file error: " << main_config_filename << std::endl;
			return 1;	
		}
		return 0;
	}

	void Parser::unloadConfig(){
		return;
	}

	int Parser::process(){
		int ret = 1,fd;

		char path[]="/var/lock/config.lock";

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

		processSystem();
		processManagement();
		processAuth();
		processQuota();
		processInterface();
		processVlan();
		processNetworkAddr();
		processMacAuth();
		processSchedule();
		processProtocol();
		processSession();
		processLogger();
		processTraffic();
		//processMacbind();
		processFirewall();
		processGlobalIP();
		processPortMirror();
		processDnsHelper();
		processHttpDirPolicy();

        /* 2012-09-29 */
        processPortScan();
		processPing();

		/* Add by chenhao : 2014-11-20 11:51:14 */
		processTiming();
		processWifiInfo();

		/* Add by chenhao,2015-7-9 09:40:25*/
		processFirstGameChannel();
		processFirewallPolicy();
		processRouteserverinfo();

        // std::cerr << print_action << std::endl; ??????
		unloadConfig();

		if(check == 0){
			if(print_fw){
			//	printCustomProtocol();
				//firewallmgr.output();
			//	printFirewallCommand(1);
				printFirewallPolicyCommand(1);
				return 0;
			}
			else if(print_action){
				printFirewallActionCommand(1);
				return 0;
			}
			else if(print_globalip)
			{	
				std::cout << "#!/bin/sh" << std::endl << std::endl;
				globalipmgr.output();
				printGlobalIPCommand(1);
				return 0;
			}
			if(print_portmirror)
			{
				std::cout << "#!/bin/sh" << std::endl << std::endl;
				portmirrormgr.output();
				printPortMirrorCommand(1);
				return 0;
			}
#if 0			
			if(print_macbind)
			{
				std::cout << "#!/bin/sh" << std::endl;
				macbindmgr.output();
				printMacBindCommand();
				return 0;
			}
#endif
			if(print_macauth)
			{
				std::cout << "#!/bin/sh" << std::endl;
				
				printMacAuthCommand();
				return 0;
			}
			if(print_quota)
			{
				std::cout << "#!/bin/sh" << std::endl;
				quotamgr.output();
				printQuotaCommand();
				return 0;
			}
			if(print_dnshelper)
			{
				std::cout << "#!/bin/sh" << std::endl;
				printDnsHelperCommand();
				return 0;
			}
			if(print_httpdirpolicy)
			{
				std::cout << "#!/bin/sh" << std::endl;
				printHttpDirPolicyCommand();
				return 0;
			}

            /* 2012-09-29 add print_port_scan */
            if(print_portscan)
			{
				std::cout << "#!/bin/sh" << std::endl;
				printPortScanCommand();
				return 0;
			}

			system.output();
			management.output();
			auth.output();
			wifimgr.output();
			interfacemgr.output();
			vlanmgr.output();
			netaddrmgr.output();
			schedulemgr.output();
//			protocolmgr.output();
			sessionmgr.output();
			trafficmgr.output();
			printTrafficCommand();
			firewallmgr.output();
			printFirewallConfig();
			printFirewallCommand(0);
		    printLogger();

			firstgcmgr.output();  //chenhao 2015-7-7 18:16:41
			routerserver.output();
			firewallpolicymgr.output();
			// logger.output(); pengyunsheng 2009/01/04
		}


		return 0;
	}

	int	Parser::processLogger(){
		std::string value;
		logger.setBlockLog(false);
		logger.setSessionLog(false);
		logger.setHTTPLog(false);
		logger.setSessionLayer7Log("");
		
		value = getNodeValuebyName("logger/block_log");

		if(value.compare("1") == 0)
		{
			logger.setBlockLog(true);
		}	
		value = getNodeValuebyName("logger/session_log");
		if(value.compare("1") == 0)
		{
			logger.setSessionLog(true);
		}	
		value = getNodeValuebyName("logger/http_log");
		if(value.compare("1") == 0)
		{
			logger.setHTTPLog(true);
		}	
		value = getNodeValuebyName("logger/session_layer7_log");
		logger.setSessionLayer7Log(value);
		return 0;

	}

	void Parser::printTrafficCommand(){
		std::vector<Node *> traffic_list = trafficmgr.getList();
		std::ostringstream traffic_list_string;
			
		std::string traffic_interface;

		int		count = 0;

		if(trafficmgr.getEnable()){
			std::cout << "TRAFFIC_CONTROL=\"1\"" << std::endl;
		}
		else{
			std::cout << "TRAFFIC_CONTROL=\"0\"" << std::endl;
		}

		for(int i = 0; i <= trafficmgr.getMaxBridgeID(); i++)
		{
			std::stringstream ss;
			ss << i;
			std::string inter = "";
			inter = "INT" + ss.str();
                        std::cout << "TRAFFIC_INT" << i << "_MAXRATE=\"" <<TrafficUtil::printRate( trafficmgr.getrootmaxrate(inter) )<< "\"" << std::endl;
			std::cout << "TRAFFIC_INT" << i << "_DEFAULT=\"" << trafficmgr.getDefaultIdbyInterface(inter) << "\"" << std::endl;
			inter = "EXT" + ss.str();			
                        std::cout << "TRAFFIC_EXT" << i << "_MAXRATE=\"" <<TrafficUtil::printRate( trafficmgr.getrootmaxrate(inter) )<< "\"" << std::endl;
			std::cout << "TRAFFIC_EXT" << i << "_DEFAULT=\"" << trafficmgr.getDefaultIdbyInterface(inter) << "\"" << std::endl;
		}

		for(unsigned int i = 0; i <	traffic_list.size(); i++){
			TrafficChannel * channel = (TrafficChannel *) traffic_list.at(i);
            if(channel->getClassType()==3)
				continue;
			count++;
			if(count > 1){
				traffic_list_string << " RULE_" << channel->getId();
			}
			else{
				traffic_list_string << "\"RULE_" << channel->getId();
			}

			channel->output();

			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_NAME=\"" << channel->getName() << "\"" << std::endl;
			//std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PARENT=\"" << channel->getParentName() << "\"" << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_INTERFACE=\"" << channel->getInterface() << "\"" << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PARENT_ID=" << channel->getParentId() << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_SHARE_ID=" << channel->getShareId() << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_CLASS_ID=" << channel->getClassId() << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_CLASS_TYPE=\"" << channel->getClassType() <<"\""<< std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_MAXRATE=\"" << TrafficUtil::printRate(channel->getRate()) << "\"" << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_LIMIT=\"" << channel->getLimit()<< "\"" << std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_WEIGHT=\"" << channel->getWeight()<< "\"" <<  std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_ATTRIBUTE=\"" << channel->getAttribute()<< "\"" <<  std::endl;
			std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PRIORITY=" << channel->getPriority() << std::endl;
			/*if(channel->isSFQ()){
				std::cout << "TRAFFIC_RULE_" << channel->getId() << "_SFQ=\"1\"" << std::endl;
			}
			else{
				std::cout << "TRAFFIC_RULE_" << channel->getId() << "_SFQ=\"0\"" << std::endl;
				if(channel->isPerHost()){
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_HOST=\"1\"" << std::endl;
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_DIR=\"" << channel->getPerDir() << "\"" << std::endl;
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_CIDR=\"" << channel->getPerCIDR() << "\"" << std::endl;
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_LIMIT=\"" << channel->getPerLimit() << "\"" << std::endl;
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_RATE=\"" << TrafficUtil::printRate(channel->getPerRate()) << "\"" << std::endl;
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_CEIL=\"" << TrafficUtil::printRate(channel->getPerCeil()) << "\"" << std::endl;
				}
				else{
					std::cout << "TRAFFIC_RULE_" << channel->getId() << "_PER_HOST=\"0\"" << std::endl;
				}
			}
			*/
		}

		if(count > 0){
		    traffic_list_string << "\"";
		}
		else{
		    traffic_list_string << "";
		}

		std::cout << "TRAFFIC_RULE_LIST=" << traffic_list_string.str() << std::endl;
	}

	void Parser::printFirewallConfig(){
		if(firewallmgr.getEnable()){
			std::cout << "APPLICATION_FIREWALL=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_FIREWALL=\"0\"" << std::endl;
		}

		if(firewallmgr.getMacAuthEnable()){
			std::cout << "MACAUTH_CONTROL=\"1\"" << std::endl;
		}
		else{
			std::cout << "MACAUTH_CONTROL=\"0\"" << std::endl;
		}

		if(firewallmgr.getDosDetect()){
			std::cout << "APPLICATION_DOS_DETECT=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_DOS_DETECT=\"0\"" << std::endl;
		}

		if(firewallmgr.getScanDetect()){
			std::cout << "APPLICATION_SCAN_DETECT=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_SCAN_DETECT=\"0\"" << std::endl;
		}
		if(networkdetectionmgr.getZeroPingValue()){
			std::cout << "APPLICATION_ZERO_PING=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_ZERO_PING=\"0\"" << std::endl;
		}
	}

	void Parser::printFirewallPolicyActionCommand(int print, int action_id, std::string src, std::string dst,std::string isp,
			std::string proto,std::string timed,std::string week,std::string day,std::string in_limit,std::string out_limit){
		std::string dot = ",";
		std::string tmp_str = "";
		std::string str = "";
		std::string week_tmp = "";
		std::string day_tmp = "";
		std::vector<std::string> weeks_info;
		std::vector<std::string> days_info;

		if(print == 0)
			return;

		if(proto.size() > 0){
			if(strtoint(proto) > 30){
				str += " --layer7 " + proto;
			}
			else{
				str += " --gid " + proto;
			}
		}

		if(src.size() > 0){
			str += " -s " + src;
		}

		if(dst.size() > 0){
			str += " -d " + dst;
		}

		if(isp.size() > 0){
			if(isp.compare("0") != 0){
				str += " --cid " + isp;
			}
		}

		if((timed.compare("on") == 0) &&
			(week.size() > 0) &&
			(day.size() > 0)){
			split(week,&weeks_info,dot.c_str());
			split(day,&days_info,dot.c_str());

			for(unsigned int i = 0; i < weeks_info.size(); i++){
				week_tmp = weeks_info.at(i);

				for(unsigned int j = 0 ; j < days_info.size(); j++){
					day_tmp = days_info.at(j);

					std::cout << "/usr/local/sbin/nf-hipac -A FORWARD0 " + str + " --days " + week_tmp + " --time " + day_tmp + " -j ACCEPT --set-mark=" + inttostr(action_id) + "," + inttostr(action_id) << std::endl;;
				}
			}
		}
		else{
			std::cout << "/usr/local/sbin/nf-hipac -A FORWARD0 " + str + " -j ACCEPT --set-mark=" + inttostr(action_id) + "," + inttostr(action_id) << std::endl;
		}
	}

	void Parser::printGlobalIPCommand(int print){
		int max_bridge_id = globalipmgr.getMaxBridgeID();
		std::vector<Node *> globalip_list = globalipmgr.getList();

		int		count[8] = { 0, 0, 0, 0, 0, 0, 0, 0};

		std::string source_cmd = "";
		std::string dstip_cmd = "";
		Session  * per_ip_session = NULL;
		Session  * total_session = NULL;
		NetAddr  * src_addr = NULL;
		NetAddr  * dst_addr = NULL;
		NetAddr	 * src_group_addr = NULL;
		NetAddr	 * dst_group_addr = NULL;
		int src_addr_index = 0;
		int dst_addr_index = 0;
		int per_ip_session_limit = 0;
		int total_session_limit = 0;
		int bridge_id = 0;

		
		for(int i = 0; i < max_bridge_id + 1; i++)
		{
			std::cout << "/usr/local/sbin/gipctl --set --bridge " << i << " --session_limit 0 --overhead 0 2> /dev/null" << std::endl;
			std::cout << "/usr/local/sbin/gipctl --flush --bridge " << i << " 2> /dev/null" << std::endl;
		}
		// global ip rules
		for(unsigned int i = 0; i <	globalip_list.size(); i++){
			GlobalIP * global_ip = (GlobalIP *) globalip_list.at(i);

			global_ip->output();
			
			per_ip_session_limit = 0;
			total_session_limit = 0;

			if(global_ip->getPerIPSessionLimit().size() > 0)
			{
				per_ip_session = (Session *)sessionmgr.find(global_ip->getPerIPSessionLimit());
				if(per_ip_session)
				{
					per_ip_session_limit = per_ip_session->getSessionLimitNum();
				}
			}

			if(global_ip->getTotalSessionLimit().size() > 0)
			{
				total_session = (Session *)sessionmgr.find(global_ip->getTotalSessionLimit());
				if(total_session)
				{
					total_session_limit = total_session->getSessionLimitNum();
				}
			}

			src_addr_index = 0;
			src_addr = (NetAddr *)netaddrmgr.find(global_ip->getAddr());
			
			src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
			if(src_addr)
			{
				bridge_id = global_ip->getBridgeID();
				if(global_ip->getAction().compare("bypass") != 0 && global_ip->getAction().compare("block") != 0)
				{
					count[bridge_id]++;
					std::cout << "/usr/local/sbin/gipctl --add --bridge " << bridge_id << " --name " << "\"" 
						<< global_ip->getName() 
						<< "\" --per_ip_sesle " << per_ip_session_limit 
						<< " --total_sesle " << total_session_limit 
						<< " --dirpolicy " << global_ip->getEnableHTTPDirPolicy()
						<< " --dnspolicy " << global_ip->getEnableDnsHelperPolicy()
						<< " --http_log " << global_ip->getEnableHTTPLog() 
						<< " --session_log " << global_ip->getEnableSessLog() 
						<< " --auth " << global_ip->getEnableAuth() 
						<< " --diptype " << global_ip->getDstIP_type() << std::endl;

				}

				while(src_addr){
					if(src_addr->getCommand().size() > 0){
						source_cmd = " --source " + src_addr->getCommand();
					}					
					else
					{
						source_cmd = "";
					}

					dst_addr_index = 0;
					dst_addr = (NetAddr *)netaddrmgr.find(global_ip->getDst());
					dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
					while(dst_addr){
						if(dst_addr->getCommand().size()>0){
							dstip_cmd = " -d " + dst_addr->getCommand();
						}
						else{
							dstip_cmd = "";
						}
						
						if(global_ip->getAction().compare("bypass") == 0)
						{
							std::cout << "/usr/local/sbin/nf-hipac -A GLOBALIP" << bridge_id << source_cmd << dstip_cmd << " -j ACCEPT --set-mark=65520,65520 2>/dev/null" << std::endl;
						}
						else if(global_ip->getAction().compare("block") == 0)
						{
							std::cout << "/usr/local/sbin/nf-hipac -A GLOBALIP" << bridge_id << source_cmd << dstip_cmd << " -j DROP 2>/dev/null" << std::endl;
						}
						else
						{
							std::cout << "/usr/local/sbin/nf-hipac -A GLOBALIP" << bridge_id << source_cmd << dstip_cmd << " -j ACCEPT --set-mark=" << count[bridge_id] << "," << count[bridge_id] << " 2>/dev/null" << std::endl;
						}
						if(dst_group_addr==NULL){
							dst_addr=NULL;
						}
						else{
							dst_addr = (NetAddr *)netaddrmgr.next((Node *)dst_group_addr, dst_addr_index);	
							dst_addr_index = netaddrmgr.getSubIndex();
							dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
						}
					}
					if(src_group_addr == NULL){
						src_addr = NULL;
					}
					else
					{
						src_addr = (NetAddr *)netaddrmgr.next((Node *)src_group_addr, src_addr_index);	
						src_addr_index = netaddrmgr.getSubIndex();
						src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
					}
				} // while src_addr
			}
		} // for
		for(int i = 0; i < max_bridge_id + 1; i++)
		{
			std::cout << "/usr/local/sbin/gipctl --set --bridge " << i << " --session_limit " << globalipmgr.getTotalSessionLimitNum(i) << " --overhead " << globalipmgr.getTotalSessionLimitOverhead(i) << " 2> /dev/null" << std::endl;
		}
	}

	void Parser::printPortMirrorCommand(int print){
		std::vector<Node *> rule_list = portmirrormgr.getList();
		int max_bridge_id = portmirrormgr.getMaxBridgeID();
		int		count = 0;
		Protocol * proto = NULL;
		NetAddr  * src_addr = NULL;
		NetAddr  * dst_addr = NULL;

		NetAddr	 * src_group_addr = NULL;
		NetAddr	 * dst_group_addr = NULL;
		int src_addr_index = 0;
		int dst_addr_index = 0;

		std::string layer7_proto = "";

		for(unsigned int i = 0; i <	rule_list.size(); i++){
			PortMirror * mirror = (PortMirror *) rule_list.at(i);
			mirror->output();

			proto = (Protocol *)protocolmgr.find(mirror->getProto());
			while(proto){
				if(proto->getProto().compare("any") != 0){
					logger.addSessionLayer7Log(proto->getMatch());
				}
				
				src_addr_index = 0;
				src_addr = (NetAddr *)netaddrmgr.find(mirror->getSrcAddr());
				src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
				//if(src_group_addr != NULL)
				//	std::cout << "group_addr" << src_group_addr << std::endl;
				
				while(src_addr){
					//std::cout << "while src_addr" << src_addr->getName() << std::endl;
					dst_addr_index = 0;
					dst_addr = (NetAddr *)netaddrmgr.find(mirror->getDstAddr());
					dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
					while(dst_addr){

						if(print)
							printPortMirrorRuleCommand(mirror->getName(), mirror->getBridgeID(), proto, mirror->getAction(), 
									src_addr, dst_addr, count);								

						count++;
						if(dst_group_addr == NULL)
							dst_addr = NULL;
						else{
							dst_addr = (NetAddr *)netaddrmgr.next((Node *)dst_group_addr, dst_addr_index);
							dst_addr_index = netaddrmgr.getSubIndex();
							dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
						}
					}
					if(src_group_addr == NULL){
						src_addr = NULL;
					}
					else
					{
						src_addr = (NetAddr *)netaddrmgr.next((Node *)src_group_addr, src_addr_index);	
						src_addr_index = netaddrmgr.getSubIndex();
						src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
					}
				}
				proto = (Protocol *)protocolmgr.next();
			}
		}
		for(int i = 0; i < max_bridge_id + 1; i++)
		{
			if(portmirrormgr.getEnable(i) == 0)
			{
				std::cout << "/usr/local/sbin/mirrorctl --bridge " << i << " --disable" << std::endl;
			}
			else
			{
				std::cout << "/usr/local/sbin/mirrorctl --bridge " << i << " --enable" << std::endl;
			}
		}
	}

	void Parser::printCustomProtocol(void)
	{
		Protocol * proto = NULL;
		std::vector<Node *> node_list;

		node_list = protocolmgr.getList();
		
		for(unsigned int i=0; i < node_list.size(); i++){
			proto = (Protocol *)node_list.at(i);
			if(!proto->isGroup() && (proto->getProto().compare("TCP") == 0 || proto->getProto().compare("UDP") == 0))
			{
				proto->output();
				//delete pengyunsheng 2009/01/04
//				std::cout << "/usr/local/sbin/signctl --add --layer4 --proto " << proto->getName() << std::endl;
//				std::cout << "/usr/local/sbin/signctl --status --enable 1 --proto " << proto->getName() << std::endl;
			}
		}
		return;

	}

	void Parser::printQuotaCommand(){
		std::vector<Node *> quota_list = quotamgr.getList();

		if(quotamgr.getQuotaControl()){
			std::cout << "APPLICATION_QUOTA=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_QUOTA=\"0\"" << std::endl;
		}

		for(unsigned int i = 0; i < quota_list.size(); i++){
			Quota* quota = (Quota*) quota_list.at(i);
			if(quota->getBandwidthQuota().compare("0000")){
				std::cout << "/usr/local/sbin/quota --add "<< "--name "<<quota->getName() << /*" --id "<<quota->getQuotaID()<<*/
					" -q "<<quota->getBandwidthQuota()<< " --iptype "<<quota->getIpType()<< " --cycletype "<<quota->getCycleType()<<std::endl;
			}
		}
		return;
	}

	void Parser::printMacBindCommand(){
		std::vector<Node *> macbind_list = macbindmgr.getList();

		if(macbindmgr.getMacBindControl()){
			std::cout << "APPLICATION_MAC_BIND=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_MAC_BIND=\"0\"" << std::endl;
		}

		for(unsigned int i = 0; i < macbind_list.size(); i++){
			MacBind* macbind = (MacBind *) macbind_list.at(i);
			macbind->output();
			std::cout << "/usr/local/sbin/mac-bind --add "<< "--name "<<macbind->getName() << " --bridge "<<macbind->getBridge()
				<< " --ip "<<macbind->getIpaddr()<< " --mac "<<macbind->getMacaddr()<<std::endl;
		}
		
		for(int j = 0; j < macbindmgr.getMacBindMaxBridge()+1;j++){
			if(macbindmgr.getMacBindAction(j)){
				std::cout << "/usr/local/sbin/mac-bind --set --bridge "<<j<<" --action accept"<< std::endl;
			}
			else{
				std::cout << "/usr/local/sbin/mac-bind --set --bridge "<<j<<" --action drop"<< std::endl;
			}
		}
		return;
	}

    /* 2012-09-29  add printPortScanCommand*/

    void Parser::printPortScanCommand(){
		std::vector<Node *> portscan_list = portscanmgr.getList();

		/*if(macbindmgr.getMacBindControl()){
			std::cout << "APPLICATION_MAC_BIND=\"1\"" << std::endl;
		}
		else{
			std::cout << "APPLICATION_MAC_BIND=\"0\"" << std::endl;
		}
        */
		for(unsigned int i = 0; i < portscan_list.size(); i++){
			PortScan* portscan = (PortScan *) portscan_list.at(i);
			//macbind->output();
			std::cout << "/usr/local/sbin/portscan --set "<< "--type "<<portscan->getType() << " --new "<<portscan->getNewer()
				<< " --invalid "<<portscan->getPending()<< " --interval "<<portscan->getInterval()<<std::endl;
		}
		
		return;
	}


	void Parser::printMacAuthCommand(){
		std::vector<Node *> macauth_list = macauthmgr.getList();

		for(unsigned int i = 0; i < macauth_list.size(); i++){
			MacAuth* macauth = (MacAuth*) macauth_list.at(i);

			if (macauth->isGroup()){
				continue;
			}

			if(macauth->getValue().compare("0-0-0-0-0-0") && macauth->getValue().compare("FF-FF-FF-FF-FF-FF"))
				std::cout << "/usr/local/sbin/mac-auth --add "<< "--name "<<macauth->getName() << " --mac "<<macauth->getValue()<<std::endl;
		}
		return;
	}

	void Parser::printFirewallPolicyCommand(int print){
		std::vector<Node *> firewallpolicy_list = firewallpolicymgr.getList();
		
		std::string id = "";
        std::string src = "";
        std::string dst = "";
        std::string isp = "";
        std::string proto = "";
		std::string session = "";
        std::string iface = "";
        std::string action = "";
        std::string timed = "";
        std::string week = "";
        std::string day = "";
        std::string in_limit = "";
        std::string out_limit = "";
        std::string comment = "";

		std::string dot = "-";
		std::string tmp = ":";
		std::string tmpstr = "";
		std::string::size_type pos=0;
		int start_action_id = 1;

		for(unsigned int i = 0; i < firewallpolicy_list.size(); i++){
			FirewallPolicy *firewallpolicy = (FirewallPolicy *)firewallpolicy_list.at(i);
		//	firewallpolicy->output();

			if(!firewallpolicy->getEnable())
				continue;

			id = firewallpolicy->getId();
			src = firewallpolicy->getSrc();
			dst = firewallpolicy->getDst();
			isp = firewallpolicy->getIsp();
			proto = firewallpolicy->getProto();
			session = firewallpolicy->getSession();
			iface = firewallpolicy->getIface();
			action = firewallpolicy->getAction();
			timed = firewallpolicy->getTimed();
			week = firewallpolicy->getWeek();
			day = firewallpolicy->getDay();
			in_limit = firewallpolicy->getInLimit();
			out_limit = firewallpolicy->getOutLimit();

			if(id.size() == 0){
				continue;
			}

			if(src.size() > 0){
				while((pos=src.find(dot,pos)) != std::string::npos)
				{
					src.replace(pos,dot.size(),tmp);
					pos += tmp.size();
				}
			}

			pos = 0;
			if(dst.size() > 0){
				while((pos=dst.find(dot,pos)) != std::string::npos)
				{
					dst.replace(pos,dot.size(),tmp);
					pos += tmp.size();
				}
			}

			if(session.size() > 0){
				if(session.compare("0") != 0){
					tmpstr += " --session_limit " + session;
				}
			}

			if(action.size() > 0){
				if(action.compare("0") == 0){
					tmpstr += " -j 0 ";
				}
				else{
					tmpstr += " -j 1 ";
				}
			}
			else{
				tmpstr += " -j 1 ";
			}

			if(iface.size() > 0){
				std::cout << "/usr/local/sbin/firewall_action --add --action_id " << start_action_id << " --name " << start_action_id << tmpstr <<  " --iface \"" << iface << "\"" << std::endl; 

				printFirewallPolicyActionCommand(1,start_action_id,src,dst,isp,proto,timed,week,day,in_limit,out_limit);
				start_action_id++;
			}

			id = "";
	        src = "";
	        dst = "";
	        isp = "";
	        proto = "";
			session = "";
	        iface = "";
	        action = "";
	        timed = "";
	        week = "";
	        day = "";
	        in_limit = "";
	        out_limit = "";
	        comment = "";
			tmpstr = "";
		}
	}
	
#if 1
	void Parser::printFirewallActionCommand(int print){
		std::vector<Node *> firewall_list = firewallmgr.getList();

			Session  * session = NULL;
			Quota *quota = NULL;
			TrafficChannel * channel = NULL;
			int session_limit = -1;
			int quota_id = -1;
			int proto_id = -1;
			int action = -1;
		
			for(unsigned int i = 0; i <	firewall_list.size(); i++){
				Firewall * firewall = (Firewall *) firewall_list.at(i);
				//firewall->output();

				int in_traffic_id = -1;
				int	out_traffic_id = -1;
				if(firewall->getInTraffic().size() > 0){
					std::string channelname="";
					std::stringstream channelname_string;
					channelname_string << "INT" << firewall->getBridgeId();
					channelname_string >> channelname;
			        	channel = (TrafficChannel *)trafficmgr.getChannelbyName(channelname, firewall->getInTraffic());
					if(channel != NULL){
							in_traffic_id = channel->getClassId();
					}else{
							in_traffic_id = trafficmgr.getDefaultIdbyInterface(channelname) ;
					}
				}

				if(firewall->getOutTraffic().size() > 0){
					std::string channelname="";
					std::stringstream channelname_string;
					channelname_string << "EXT" << firewall->getBridgeId();
					channelname_string >> channelname;
			        	channel = (TrafficChannel *)trafficmgr.getChannelbyName(channelname, firewall->getOutTraffic());
					if(channel != NULL){
							out_traffic_id = channel->getClassId();
					}else{
							out_traffic_id = trafficmgr.getDefaultIdbyInterface(channelname) ;
					}
				}

				int second_in_traffic_id = -1;
				int	second_out_traffic_id = -1;
				if(firewall->getSencondInTraffic().size() > 0){
					std::string channelname="";
					std::stringstream channelname_string;
					channelname_string << "INT" << firewall->getBridgeId();
					channelname_string >> channelname;
			        	channel = (TrafficChannel *)trafficmgr.getChannelbyName(channelname,firewall->getSencondInTraffic());
					if(channel != NULL){
							second_in_traffic_id = channel->getClassId();
					}else{
							second_in_traffic_id = trafficmgr.getDefaultIdbyInterface(channelname) ;
					}
				}
				
				if(firewall->getSencondOutTraffic().size() > 0){
					std::string channelname="";
					std::stringstream channelname_string;
					channelname_string << "EXT" << firewall->getBridgeId();
					channelname_string >> channelname;
			        	channel = (TrafficChannel *)trafficmgr.getChannelbyName(channelname, firewall->getSencondOutTraffic());
					if(channel != NULL){
							second_out_traffic_id = channel->getClassId();
					}else{
							second_out_traffic_id = trafficmgr.getDefaultIdbyInterface(channelname) ;
					}
				}

				session_limit = -1;

				if(firewall->getSessionLimit() != ""){
					session = (Session *)sessionmgr.find(firewall->getSessionLimit());
				if(session)
					session_limit = session->getSessionLimitNum();
				}

				quota_id = -1;
	
				quota = (Quota*)quotamgr.find(firewall->getQuota());
		
				if(quota == NULL){
					quota_id = 0;
				}else{
					quota_id = quota->getQuotaID();
				}
		
				proto_id = -1;
	
				if(firewall->getProto().compare("any")){
					proto_id = 1;
				}else{
					proto_id = 0;
				}

				if(firewall->getAction().compare("allow")){
					action = 0;
				}else{
					action = 1;
				}

				std::cout << "/usr/local/sbin/firewall_action --add "<< "--name " <<firewall->getName() << " --bridge "<<firewall->getBridgeId() <<
					" --in " <<in_traffic_id<<" --out " << out_traffic_id << " --quota_in "<<second_in_traffic_id<<" --quota_out "<<second_out_traffic_id <<
					" --action_id "<<firewall->getActionID()<<" --quota_id " << quota_id<<" --layer7_id "<<proto_id<<" -j "<<action<<" ";
				if(session_limit > 0){
					std::cout << "--session_limit " << session_limit << " ";
				}
			
				std::cout <<"--drop_log "<<firewall->getLogPacket()<<" --quota_action "<<firewall->getQuotaAction()<<std::endl;
			}

		}
#endif
	void Parser::printFirewallCommand(int print){
		std::vector<Node *> firewall_list = firewallmgr.getList();

		int		count = 0;
		Protocol * proto = NULL;
		Schedule * sche = NULL;
		Vlan	 * vlan = NULL;
		NetAddr  * src_addr = NULL;
		NetAddr  * dst_addr = NULL;
		MacAuth  * mac_addr = NULL;
		NetAddr	 * src_group_addr = NULL;
		NetAddr	 * dst_group_addr = NULL;
		MacAuth	 * mac_group_addr = NULL ;
		int src_addr_index = 0;
		int dst_addr_index = 0;
		int mac_addr_index = 0;

		std::string schedule_string = "";
		std::string vlan_string = "";
		std::string layer7_proto = "";

		std::vector<Node*> lst;
		std::string lanaguage = "english";
		std::string protoname = "";
		std::string matchname = "";
		unsigned int index = 0;
		
		protocolmgr.Clear();
		if(processProtocol(lanaguage) > 0)
			exit(-1);
			
		lst = protocolmgr.getList();
		/*for(; index < lst.size(); index++){
			Protocol* item = (Protocol *) lst.at(index);
			if(item->isGroup())
				continue;
			std::cout << "Name = " << item->getName() << std::endl;
			std::cout << "Match = " << item->getMatch() << std::endl;
			std::cout << "Type = " << item->getType() << std::endl;
		}*/
			
		for(unsigned int i = 0; i <	firewall_list.size(); i++){
			Firewall * firewall = (Firewall *) firewall_list.at(i);
			//firewall->output();

			index = 0;
			protoname = firewall->getProto();
			proto = (Protocol *)protocolmgr.find(firewall->getProto());
			while(proto || protoname.size() > 0){
				if (proto != NULL){	
					if(proto->getProto().compare("any") != 0){
						logger.addSessionLayer7Log(proto->getMatch());
					}
					protoname = "";
				} 
				else { //Ä¬ÈÏÐ­Òé·Ö×é´¦Àí
					if (protoname.compare("any") == 0){
						matchname = "";
						protoname = "";
					} 
					else{
						if (!(protoname.compare("Game") == 0 || 
								protoname.compare("Websp") == 0 ||
								protoname.compare("P2Psp") == 0 ||
								protoname.compare("P2P") == 0 ||
								protoname.compare("Webtransfer") == 0 ||
								protoname.compare("Web") == 0 ||
								protoname.compare("Chat") == 0 ||
								protoname.compare("NetworkIST") == 0 ||
								protoname.compare("FileTransfer") == 0 ||
								protoname.compare("Security") == 0 ||
								protoname.compare("Database") == 0 ||
								protoname.compare("Mail") == 0 ||
								protoname.compare("Stock") == 0 ||
								protoname.compare("Terminals") == 0 ||
								protoname.compare("Voip") == 0 ||
								protoname.compare("Rfc") == 0 ||
								protoname.compare("Others") == 0)){
							break;
						}
										
						matchname = "";
						for(; index < lst.size(); index++){
							Protocol* item = (Protocol *) lst.at(index);
							if(item->isGroup()){
								continue;
							}
							if (protoname.compare(item->getType()) == 0){
								matchname = item->getMatch();
								index++;
								break;
							}
						}
						if (matchname.size() < 1)
							break;
					}
				}
				schedule_string = firewall->getSchedule();
			
				if(firewall->getSchedule().compare("never") != 0){
					sche = (Schedule *)schedulemgr.find(firewall->getSchedule());

					while(1){
						src_addr_index = 0;
						src_addr = (NetAddr *)netaddrmgr.find(firewall->getSrc());
						src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
						//if(src_group_addr != NULL)
						//	std::cout << "group_addr" << src_group_addr << std::endl;
						
						while(src_addr){
							//std::cout << "while src_addr" << src_addr->getName() << std::endl;
							dst_addr_index = 0;
							dst_addr = (NetAddr *)netaddrmgr.find(firewall->getDst());
							dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
							while(dst_addr){
								mac_addr_index = 0;
								mac_addr = (MacAuth*)macauthmgr.find(firewall->getMac());
								mac_group_addr = (MacAuth *)macauthmgr.getCurrGroupNode();

								while(mac_addr){
								vlan = NULL;
								if(firewall->getVlan() != ""){
									vlan = (Vlan *)vlanmgr.find(firewall->getVlan());
								}
								if(print)
									printFirewallRuleCommand(firewall->getName(), firewall->getBridgeId(), proto, firewall->getAuth(), 
											sche, vlan, src_addr, firewall->getSrcInvent(), dst_addr, firewall->getDstInvent(),mac_addr,
											firewall->getMacInvent(),firewall->getActionID(),count, matchname);	

									count++;

									if(mac_group_addr == NULL)
										mac_addr = NULL;
									else{
										mac_addr = (MacAuth*)macauthmgr.next((Node *)mac_group_addr, mac_addr_index);
										mac_addr_index = macauthmgr.getSubIndex();
										mac_group_addr = (MacAuth *)macauthmgr.getCurrGroupNode();
									}

								}
								if(dst_group_addr == NULL)
									dst_addr = NULL;
								else{
									dst_addr = (NetAddr *)netaddrmgr.next((Node *)dst_group_addr, dst_addr_index);
									dst_addr_index = netaddrmgr.getSubIndex();
									dst_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
								}
							}
							if(src_group_addr == NULL){
								src_addr = NULL;
							}
							else
							{
								src_addr = (NetAddr *)netaddrmgr.next((Node *)src_group_addr, src_addr_index);	
								src_addr_index = netaddrmgr.getSubIndex();
								src_group_addr = (NetAddr *)netaddrmgr.getCurrGroupNode();
							}
						}

						sche = (Schedule *)schedulemgr.next();
						if(sche == NULL) break;


					}
				}
				if (proto != NULL)
					proto = (Protocol *)protocolmgr.next();
			}
		}
	}

	void Parser::printDomainDataCommand(){
		std::vector<Node *> domain_list = domainmgr.getList();
					
		for(unsigned int i = 0; i <	domain_list.size(); i++){
			DomainNode * domain = (DomainNode *) domain_list.at(i);
			
			if(domain->isGroup() || domain->getName().compare("any") == 0){
				continue;
			}		
			
			domain->output();			

			std::cout << "/usr/local/sbin/dns_helper --domainadd  --domainid " << domain->getID()
						<< "  --domain \"" << domain->getValue() << "\" "<< std::endl;		

		}

	}
	
	void Parser::printDnsPolicyCommand(){
		std::vector<Node *> policy_list = dnspolicymgr.getList();

		NetAddr *paddr= NULL;
		DomainNode * pdomain = NULL;
		std::vector<uint32_t> domainid_list(5);
		std::vector<std::string> srcaddr_list(5);
		std::vector<std::string> dstaddr_list(5);
		uint32_t isrc=0;
		uint32_t idst = 0; 
		uint32_t idomain = 0;	
			
		
		for(unsigned int i = 0; i <	policy_list.size(); i++){
			DnsPolicy* policy = (DnsPolicy *) policy_list.at(i);
			policy->output();
			
			domainid_list.clear();
			pdomain = (DomainNode *)domainmgr.find(policy->getDomainName());
			while(NULL != pdomain ){				
				domainid_list.push_back(pdomain->getID());
				pdomain = (DomainNode *)domainmgr.next();
			}

			srcaddr_list.clear();
			paddr = (NetAddr *)netaddrmgr.find(policy->getSrc());
			while(NULL != paddr ){
				srcaddr_list.push_back(paddr->getCommand());
				paddr = (NetAddr *)netaddrmgr.next();
			}

			dstaddr_list.clear();
			paddr = (NetAddr *)netaddrmgr.find(policy->getDst());
			while(NULL != paddr ){
				dstaddr_list.push_back(paddr->getCommand());
				paddr = (NetAddr *)netaddrmgr.next();
			}
			//printf("srcsize = %d ,dstsize=%d , domainsize=%d\n" ,  srcaddr_list.size(),dstaddr_list.size(),domainid_list.size());

// /usr/local/sbin/nf-hipac -A FORWARD0 $USER_AUTH $MAC_AUTH -s 0.0.0.0/0 -d 0.0.0.0/0 -j ACCEPT --layer7 1 --set-mark=1,1
			for(isrc = 0; isrc< srcaddr_list.size(); ++isrc){
				for(idst = 0; idst< dstaddr_list.size(); ++idst){
					for(idomain = 0; idomain< domainid_list.size(); ++idomain){

						//std::cout  << "USER_AUTH=\"\"" << std::endl;
						//std::cout  << "MAC_AUTH=\"\"" << std::endl;
									
						std::cout << "/usr/local/sbin/nf-hipac -A DNSHELPER" << policy->getBridgeId() ;//<< " " << "$USER_AUTH " << "$MAC_AUTH ";

						if(domainid_list.at(idomain) > 0)
							std::cout << " --layer7 "  << domainid_list.at(idomain) << " ";
						
						if(srcaddr_list.at(isrc).size() > 0){
							std::cout << " -s ";
							std::cout << srcaddr_list.at(isrc) << " ";
						}
						if(dstaddr_list.at(idst).size() > 0){
							std::cout << " -d ";
							std::cout << dstaddr_list.at(idst) << " ";
						}

						std::cout << " -j ACCEPT " << " --set-mark=" << policy->getName() << "," << policy->getName() << " ";
						std::cout << std::endl;
						
					}				
				}			
			}
				

			//dns_helper --add --actionid 1  --bridgeid 0 --ipaddr 10.200.1.1 --style 1
			paddr = (NetAddr *)netaddrmgr.find(policy->getTargetip());
		
			std::cout<< "/usr/local/sbin/dns_helper --add --actionid " << policy->getName() 
				<< " --bridgeid " << policy->getBridgeId() ;

			if(paddr != NULL)
				std::cout << " --ipaddr " << paddr->getCommand();
			else
				std::cout << " --ipaddr 0.0.0.0/0";

			std::cout<< " --style " << policy->getAction()
					 <<std::endl;				

			std::cout<<std::endl;	
		}
		domainid_list.clear();
		srcaddr_list.clear();
		dstaddr_list.clear();
		
	}	

	
	void Parser::printDnsHelperCommand(){
		//domainmgr.output();
		//dnspolicymgr.output();

		std::cout<< "# Domain Data : " << std::endl;
		printDomainDataCommand();
		
		std::cout << std::endl << "# Dns Policy : " << std::endl;
		printDnsPolicyCommand();
		
	/*	std::cout << "# <application_policy>" << dnspolicymgr.getEnable() <<"</application_policy>" <<std::endl;
		if(dnspolicymgr.getEnable()){
			std::cout<< "/usr/local/sbin/dns_helper --enable " <<std::endl;
		}
		else {			
			std::cout<< "/usr/local/sbin/dns_helper --disable " <<std::endl;			
		}*/
			
		std::cout<<std::endl;
		
		return;
	}


	void Parser::printExtensionCommand(){
		std::vector<Node *> node_list = extensionmgr.getList();
		
		for(unsigned int i = 0; i <	node_list.size(); i++){
			ExtensionNode * node = (ExtensionNode *) node_list.at(i);
								
			if(node->isGroup()){
				continue;
			}
			
			node->output();			

			std::cout << "/usr/local/sbin/http_directory_policy --nameadd --dirid " << node->getID()
						<< " --dirname \"" << node->getValue() << "\" "<< std::endl;
		}
	}

	void Parser::printUrlFormatCommand(){
		std::vector<Node *> node_list = urlformatmgr.getList();
					
		for(unsigned int i = 0; i <	node_list.size(); i++){
			UrlFormatNode* node = (UrlFormatNode *) node_list.at(i);
					
			node->output();			

			std::cout << "/usr/local/sbin/http_directory_policy --urlset --bridgeid " << node->getBridgeIdString()
						<< " --urlformat \"" << node->getFormat() << "\" "<< std::endl;
		}
	}
	
	void Parser::printHttpServerCommand(){
		std::vector<Node *> node_list = httpservermgr.getList();
		std::vector<std::string> subnode_list;
		Node* pgrpnode = NULL;
		HttpServerNode * psvrnode = NULL;
					
		for(unsigned int i = 0; i <	node_list.size(); i++){
			pgrpnode =  node_list.at(i);
			
			if(pgrpnode->isGroup()){
				std::cout << "#http server group name: " << pgrpnode->getName() << std::endl;
			
				subnode_list = pgrpnode->getSubNodes();
					
				for(unsigned int x = 0; x< subnode_list.size(); ++x){
					if((psvrnode = (HttpServerNode *)httpservermgr.find(subnode_list.at(x))) != NULL) {
						psvrnode->output();	
						
	//./http_directory_policy --serveradd  --ipaddr 10.200.1.1 --servertype "mini_http" --prefix "abc" --suffix "zzz" --groupid 4 
					std::cout << "/usr/local/sbin/http_directory_policy --serveradd " 
						<< " --ipaddr \"" << psvrnode->getIPAddr()<< "\" "
						<< " --servertype \"" << psvrnode->getServerType() << "\" "
						<< " --prefix \"" << psvrnode->getPrefix()<< "\" "
						<< " --suffix \"" << psvrnode->getSuffix()<< "\" "
						<< " --groupid  " << httpservermgr.getGroupID(pgrpnode->getName())
						<< std::endl;
	
					}
				}

			}
		}
	}	

	void Parser::printHttpPolicyCommand(){
		std::vector<Node *> node_list = httpdirpolicymgr.getList();

		ExtensionNode *extnode= NULL;
		//extensionmgr.output();
					
		for(unsigned int i = 0; i <	node_list.size(); i++){
			HttpDirPolicy* node = (HttpDirPolicy *) node_list.at(i);
					
			node->output();	

			extnode = (ExtensionNode *)extensionmgr.find(node->getExtgroup());
			while(NULL != extnode ){
					//./http_directory_policy --policyset --bridgeid  0 --dirid 1 --groupid 4
				std::cout << "/usr/local/sbin/http_directory_policy --policyset  --bridgeid " << node->getBridgeIdString()
					<< " --dirid " << extnode->getID()<< " "
					<< " --groupid  " << httpservermgr.getGroupID(node->getServergroup())
					<< std::endl;
					
				extnode = (ExtensionNode *)extensionmgr.next();
			}

			
		}


	}	

	
	void Parser::printHttpDirPolicyCommand(){

		std::cout<< "# Extension: " << std::endl;
		printExtensionCommand();
		
		std::cout << std::endl << "# http server: " << std::endl;
		printHttpServerCommand();		
		
		std::cout << std::endl << "# url format set : " << std::endl;
		printUrlFormatCommand();
		
		std::cout << std::endl << "# http directory policy: " << std::endl;
		printHttpPolicyCommand();		

		std::cout<<std::endl;
		
		return;
	}

    



	int Parser::myreadline(FILE * fp,char *buf,int len){//行读�?

		if(feof(fp))
			return -1;
		
		int i=0;

		char tmp;

		for (;i<len ;i++ )
		{
			if(fread(&tmp,1,1,fp)<1){

				break;
			}


			if(tmp=='\0'  || tmp=='\n'){

				break;
			}
			buf[i]=tmp;

		}

		buf[i]='\0';
		return i;
	}

	int	Parser::isDir(std::string name){

		struct stat sbuf;

		stat(name.c_str(),&sbuf);

		if( ((sbuf.st_mode)&S_IFMT)==S_IFDIR ){
			return  1;
		}

		return  0;

	} 

	int Parser::parseSignatureFile(std::string filename, std::string name){
//		std::cout << "*****************" << filename << ":" << name << std::endl;

		FILE *fp;

		char buff[2048];
		char *line = buff;
		int layer7_id = 0;
		fp = fopen(filename.c_str(), "r");
		if(!fp)
			return 0;
		while(myreadline(fp,buff, 2040) >-1)
		{
			
			if(strlen(buff) < 2 || buff[0] == '#'){//检测文件长度和是否注释�?
				continue;
			}
			
			if(strncmp(buff, name.c_str(), strlen(name.c_str())) == 0)
			{
				//std::cout << buff << std::endl;
				line = buff;
				while(1)
				{

					if( (*line)=='\0')
						break;
					
					if( (*line)==' ' || (*line)=='	'){
						line++;
						layer7_id = atoi(line);
						//std::cout << layer7_id << std::endl;
						fclose(fp);
						return layer7_id;

					}
					line++;
				}
			}
		}
		fclose(fp);
		return layer7_id;
	}
	
	int Parser::parseDir(std::string dir, std::string name){
		struct dirent   ** namelist;
		int n=0;
		int layer7_id = -1;
		
		std::string temp_dir;
		std::string filename = name + ".sig";
		n = scandir(dir.c_str(),&namelist,0,alphasort);
		if(n<0){
			return 0;
		}
		while(n--){
			if(strcmp(namelist[n]->d_name,"..")!=0 && strcmp(namelist[n]->d_name,".")!=0 ){
				temp_dir = dir + "/" + namelist[n]->d_name;

				if(isDir(temp_dir) ){
					//
					layer7_id = parseDir(temp_dir, name);
					if(layer7_id > 0){
						free(namelist);
						return layer7_id;
				    }
				}else{					
					if(strncmp(namelist[n]->d_name, filename.c_str(), strlen(filename.c_str()))==0){
						/////////////////////////////////////////////////END
						layer7_id = parseSignatureFile(temp_dir, name);
						free(namelist);
						return layer7_id;
					}

				}

			}

		}

		free(namelist);
		return layer7_id;

	}

	int		Parser::getLayer7Id(std::string name)
	{
		int id = -1;
		//std::cout << name << std::endl;
		if(name.compare("any") == 0)
		{
			id = -1;
		}
		else if(name.compare("analyzing_tcp") == 0)
		{
			id = 65535;
		}
		else if(name.compare("analyzing_udp") == 0)
		{
			id = 65534;
		}
		else if(name.compare("soft_bypass") == 0)
		{
			id = 65533;
		}
		else if(name.compare("others") == 0)
		{
			id = 65532;
		}
		else if(name.compare("others_ipv4") == 0)
		{
			id = 65531;
		}
		else if(name.compare("others_tcp") == 0)
		{
			id = 65530;
		}
		else if(name.compare("others_udp") == 0)
		{
			id = 65529;
		}
		else if(name.compare("icmp") == 0)
		{
			id = 65528;
		}
		else if(name.compare("arp") == 0)
		{
			id = 65527;
		}
		else if(name.compare("mpls") == 0)
		{
			id = 65526;
		}
		else if(name.compare("ipx") == 0)
		{
			id = 65525;
		}
		else if(name.compare("ipv6") == 0)
		{
			id = 65524;
		}
		else if(name.compare("pppoe") == 0)
		{
			id = 65523;
		}
		else if(name.compare("http-browse") == 0)
		{
			id = 65500;
		}
		else if(name.compare("http-download") == 0)
		{
			id = 65501;
		}
		else if(name.compare("http-part") == 0)
		{
			id = 65502;
		}
		else if(name.compare("http-mirror") == 0)
		{
			id = 65503;
		}
		else if(name.compare("http-tunnel") == 0)
		{
			id = 65504;
		}			
		else if(name.compare("http-others") == 0)
		{
			id = 65505;
		}
		else if(name.compare("p2p-common") == 0)
		{
		    id = 65518;
		}
		else if(name.compare("game-flow") == 0)
	    {
	    	id = 65517;
		}
		else if(name.compare(layer7_name_cache) == 0)
		{
			id = layer7_id_cache;
		}
		else
		{
			id = parseDir("/etc/signatures", name);
			layer7_name_cache = name;
			layer7_id_cache = id;
			//std::cout << id << std::endl;
		}
		if(id == 0)
			id = -1;

		return id;
	}

	void		Parser::printPortMirrorRuleCommand(std::string name, int bridge_id,
						Protocol * proto, std::string action, NetAddr * src_addr, NetAddr * dst_addr, int count){
			std::cout << "/usr/local/sbin/nf-hipac -A MIRROR" << bridge_id << " ";
			if(proto->getCommand().size() > 0 && getLayer7Id(proto->getCommand()) > 0){
				std::cout << "--layer7 "  << getLayer7Id(proto->getCommand()) << " ";
			}
			if(src_addr->getCommand().size() > 0){
				std::cout << "-s " << src_addr->getCommand() << " ";
			}
			if(dst_addr->getCommand().size() > 0){
				std::cout << "-d " << dst_addr->getCommand() << " ";
			}
			if(action.compare("in") == 0){
				std::cout << "-j ACCEPT --set-mark=1,1";
			}
			else if(action.compare("out") == 0)
			{
				std::cout << "-j ACCEPT --set-mark=2,2";
			}
			else if(action.compare("in_discard") == 0){
				std::cout << "-j ACCEPT --set-mark=4,4";
			}
			else if(action.compare("out_discard") == 0)
			{
				std::cout << "-j ACCEPT --set-mark=5,5";
			}
			else if(action.compare("all_discard") == 0)
			{
				std::cout << "-j ACCEPT --set-mark=6,6";
			}
            else
			{
				std::cout << "-j ACCEPT --set-mark=3,3";
			}
			std::cout << std::endl;
	}
	
	void		Parser::printFirewallRuleCommand(std::string name, int bridge_id,
						Protocol * proto, std::string auth, Schedule * sche, Vlan * vlan,
						NetAddr * src_addr, bool src_invent, NetAddr * dst_addr, bool dst_invent, 
						MacAuth* mac_addr, bool mac_invent,  int action_id, int count, std::string matchname){
#if 0
			if(auth.size() > 0)
			{
				std::cout  << "USER_AUTH=`/usr/local/sbin/getuserid \"" << auth << "\" " << bridge_id << "`" << std::endl;
			}
			else
			{
				std::cout  << "USER_AUTH=\"\"" << std::endl;
			}

			if(mac_addr->getCommand().size() > 0)
			{
				std::cout << "MAC_AUTH=`/usr/local/sbin/getmacauthid \"" << mac_addr->getCommand() << "\" " << "`" <<std::endl;
			}
			else
			{
				std::cout  << "MAC_AUTH=\"\"" << std::endl;
			}
#endif			
			std::cout << "/usr/local/sbin/nf-hipac -A FORWARD" << bridge_id << " ";// << "$USER_AUTH " << "$MAC_AUTH ";

			if(sche != NULL)
			{
				std::cout << sche->getCommand() << " ";
			}
			if(vlan != NULL)
			{
				std::cout << vlan->getCommand() << " ";
			}

			if (proto != NULL){
				if(proto->getCommand().size() > 0 && getLayer7Id(proto->getCommand()) > 0){
					std::cout << "--layer7 "  << getLayer7Id(proto->getCommand()) << " ";
				}
			} 
			else{
				if (matchname.size() > 0)
					std::cout << "--layer7 "  << getLayer7Id(matchname) << " ";
			}

			
			if(src_addr->getCommand().size() > 0){
				std::cout << "-s ";
				if(src_invent)
					std::cout << "! ";
				std::cout << src_addr->getCommand() << " ";
			}
			if(dst_addr->getCommand().size() > 0){
				std::cout << "-d ";
				if(dst_invent)
					std::cout << "! ";
				std::cout << dst_addr->getCommand() << " ";
			}

			if(action_id> 0 ){
				std::cout << "-j ACCEPT " << "--set-mark=" << action_id << "," << action_id << " ";

			}

			std::cout << std::endl;
	}

	void        Parser::printLogger(){
		Protocol * proto = NULL;
		std::cout << "# logger" << std::endl;
		std::cout << "LOGGER_BLOCK_LOG=\"" << logger.getIntbyBoolean(logger.isBlockLog()) << "\"" << std::endl;
		std::cout << "LOGGER_HTTP_LOG=\"" << logger.getIntbyBoolean(logger.isHTTPLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SESSION_LOG=\"" << logger.getIntbyBoolean(logger.isSessionLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SCAN_DETECT_LOG=\"" << logger.getIntbyBoolean(logger.isScanDetectLog()) << "\"" << std::endl;
		std::cout << "LOGGER_DOS_DETECT_LOG=\"" << logger.getIntbyBoolean(logger.isDoSDetectLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SESSION_LAYER7_LOG=\"";
		for(unsigned int i=0; i < logger.session_layer7_log.size(); i++){
			if(i != 0){
				std::cout << " ";
			}
			
			proto = (Protocol *)protocolmgr.find(logger.session_layer7_log.at(i));
            if(proto)
				std::cout << proto->getMatch();
		}	
		std::cout << "\"" << std::endl;
	}
	
	int Parser::processGlobalIP()
	{
		TiXmlNode * global_ip_node = findNode(&doc, "xmlconfig/global_ip_session_limits");
		if(global_ip_node != NULL){	
			parseGlobalIP(global_ip_node);
		}
		return 0;
	}

	void Parser::parseGlobalIP(TiXmlNode * cur){

		std::string name = "";
		std::string addr = "";
		std::string dstaddr ="";
		std::string bridge_id = "";
		std::string dst_type = "";
		std::string per_ip_session_limit = "";
		std::string total_session_limit = "";
		std::string action = "";
		std::string overhead = "";
		std::string comment = "";
		std::string limit = "";
		bool	httplog = false;
		bool	httpdirpolicy = false;
		bool	dnshelperpolicy = false;
		bool    sesslog = false;
		bool    enable_auth = false;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "total_session_limit") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "0";
						}
						else if(strcmp(attr->Name(), "limit") == 0){
							limit = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "overhead") == 0){
							overhead = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					if(bridge_id.size() > 0)
						globalipmgr.setTotalSessionLimit(bridge_id, limit, overhead);
					bridge_id = "";
					limit = "";
				}
				else if(strcmp(cur->Value(),  "global_ip_session_limit") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value() ? attr->Value() : "";
						}		
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "0";
						}						
						else if(strcmp(attr->Name(), "addr") == 0){
							addr = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(),"dst") == 0){
							dstaddr = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "httplog") == 0){
							if(strcmp(attr->Value(), "1") == 0){
								httplog = true;
							}
						}
						else if(strcmp(attr->Name(), "dnspolicy") == 0){
							if(strcmp(attr->Value(), "1") == 0){
								dnshelperpolicy = true;
							}
						}
						else if(strcmp(attr->Name(), "httpdirpolicy") == 0){
							if(strcmp(attr->Value(), "1") == 0){
								httpdirpolicy = true;
							}
						}
						else if(strcmp(attr->Name(),"sesslog") ==0) {
							if(strcmp(attr->Value(),"1") == 0) {
								sesslog = true;
							}
						}
						else if(strcmp(attr->Name(),"auth")==0){
							if(strcmp(attr->Value(),"1")==0){
								enable_auth = true;
							}
						}
						else if(strcmp(attr->Name(),"dst_type")==0){
							dst_type = attr->Value() ? attr->Value() : "";
					    }
						else if(strcmp(attr->Name(), "per_ip_session_limit") == 0){
							per_ip_session_limit = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "total_session_limit") == 0){
							total_session_limit = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					globalipmgr.add(name, bridge_id, addr,dstaddr,per_ip_session_limit, total_session_limit, action, httplog, dnshelperpolicy,httpdirpolicy,sesslog,enable_auth,dst_type,comment);
					name = "";
					addr = "";
					bridge_id = "";
					httplog = false;
					dnshelperpolicy= false;
					httpdirpolicy = false;
					per_ip_session_limit = "";
					total_session_limit = "";
					action = "";
					comment = "";
				}
				if(cur->NextSibling()){
					parseGlobalIP(cur->NextSibling());
				}				

			}

			cur=cur->FirstChild();
		}

	}

	int Parser::processPortMirror(bool forScript)
	{
		TiXmlNode * port_mirror_node = findNode(&doc, "xmlconfig/port_mirrors");
		if(port_mirror_node != NULL){	
			parsePortMirror(port_mirror_node , forScript);
		}
		return 0;
	}

	void Parser::parsePortMirror(TiXmlNode * cur, bool forScript){

		std::string name = "";
		std::string bridge_id = "";
		std::string src_addr = "";		
		std::string dst_addr = "";
		std::string action = "";
		std::string layer7 = "";
		std::string comment = "";
		bool		disabled = false;
		int enable = 0;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "port_mirror_enable") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "0";
						}
						else if(strcmp(attr->Name(), "enable") == 0){
							if(attr->Value() == NULL || strcmp(attr->Value(), "1") == 0)
							{
								enable = 1;
							}
						}
						attr=attr->Next();
					}
					if(bridge_id.size() > 0)
						portmirrormgr.setEnable(bridge_id, enable);
					bridge_id = "";
					enable = 0;
				}
				else if(strcmp(cur->Value(),  "port_mirror") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value() ? attr->Value() : "";
						}		
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "0";
						}						
						else if(strcmp(attr->Name(), "src") == 0){
							src_addr = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "dst") == 0){
							dst_addr = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "proto") == 0){
							layer7 = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "disabled") == 0){
							if(strcmp(attr->Value(), "0") == 0){
								disabled = false;
							}
							else{
								disabled = true;
							}
						}						
						attr=attr->Next();
					}
					if(!disabled || !forScript)
					{
						portmirrormgr.add(name, bridge_id, src_addr, 
								dst_addr, layer7, action, comment,disabled);
					}
					name = "";
					src_addr = "";
					dst_addr = "";
					bridge_id = "";
					layer7 = "";
					action = "";
					comment = "";
					disabled = false;
				}
				if(cur->NextSibling()){
					parsePortMirror(cur->NextSibling(),forScript);
				}
			}
			cur=cur->FirstChild();
		}

	}	

	void Parser::parseQuota(TiXmlNode * cur){
		std::string name = "";
		std::string bandwidth_quota = "";
		std::string ip_type = "";
		std::string cycle_type = "";
		std::string comment = "";
		static int cout=0;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "quota") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "bandwidth_quota") == 0){
							bandwidth_quota = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "ip_type") == 0){
							ip_type = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "cycle_type") == 0){
							cycle_type = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					
					cout++;
					quotamgr.add(name, bandwidth_quota, ip_type,cycle_type,cout,comment);
					
					name = "";
					bandwidth_quota = "";
					ip_type = "";
					cycle_type = "";
					comment = "";
				}
				else if(strcmp(cur->Value(), "application_quota") == 0){
					quotamgr.setQuotaControl(false);
					if(getNodeValue(cur).compare("1") == 0)
					{
						quotamgr.setQuotaControl(true);
					}	
				}
	
			}
						
			cur = cur->NextSibling();
		}

	}

	int Parser::processQuota(){
		TiXmlNode * macauths_node = findNode(&doc, "xmlconfig/quotas");
	
		if((macauths_node != NULL) && (macauths_node->FirstChild() != NULL)){	
			parseQuota(macauths_node->FirstChild());
		}
		
		return 0;
	}

	int Parser::processMacbind(){
		TiXmlNode * mac_bind_node = findNode(&doc, "xmlconfig/macbindings");
		if(mac_bind_node != NULL && mac_bind_node->FirstChild() !=NULL){	
			parseMacBind(mac_bind_node->FirstChild() );
		}
		return 0;
	}

	void Parser::parseMacBind(TiXmlNode * cur){
		std::string name = "";
		std::string bridge = "";
		std::string ipaddr = "";
		std::string macaddr = "";
		std::string action = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "ipmac") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "ip_addr") == 0){
							ipaddr = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "mac_addr") == 0){
							macaddr = attr->Value() ? attr->Value() : "";
						}

						attr=attr->Next();
					}
					macbindmgr.add(name,bridge ,ipaddr, macaddr);
				}
				else if(strcmp(cur->Value(), "application_macbinding") == 0){
					macbindmgr.setMacBindControl(false);
					if(getNodeValue(cur).compare("1") == 0)
					{
						macbindmgr.setMacBindControl(true);
					}									
				}
				else if(strcmp(cur->Value(), "macbinding_action") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "bridge") == 0){
							bridge= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					if(bridge.size() > 0)
						macbindmgr.setMacBindAction(bridge, action);
					bridge = "";
					action = "";
				}

			}

			cur=cur->NextSibling();
		}
	}


	int Parser::processFirewallPolicy(){
		TiXmlNode * firewallpolicy = findNode(&doc, "xmlconfig/firewalls_policy");
		if(firewallpolicy != NULL && firewallpolicy->FirstChild() !=NULL){	
			parseFirewallpolicyinfo(firewallpolicy->FirstChild() );
		}
		return 0;
	}

	void Parser::parseFirewallpolicy(TiXmlNode * cur){
			std::string id = "";
            bool enable = false;
            std::string src = "";
            std::string dst = "";
            std::string isp = "";
            std::string proto = "";
			std::string session = "";
            std::string iface = "";
            std::string action = "";
            std::string timed = "";
            std::string week = "";
            std::string day = "";
            std::string in_limit = "";
            std::string out_limit = "";
            std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "firewall") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "id") == 0){
							id = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "enable") == 0){
							if(strcmp(attr->Value(), "0") == 0){
								enable = false;
							}
							else{
								enable = true;
							}
						}
						else if(strcmp(attr->Name(), "src") == 0){
							src = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "dst") == 0){
							dst = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "isp") == 0){
							isp= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "proto") == 0){
							proto = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "session") == 0){
							session = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "iface") == 0){
							iface = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "timed") == 0){
							timed = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "week") == 0){
							week = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "day") == 0){
							day = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "in_limit") == 0){
							in_limit= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "out_limit") == 0){
							out_limit = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}

						attr=attr->Next();
					}
					firewallpolicymgr.add(id,enable ,src, dst,isp,proto,session,iface,action,timed,week,day,in_limit,out_limit,comment);

					id = "";
		            enable = false;
		            src = "";
		            dst = "";
		            isp = "";
		            proto = "";
					session = "";
		            iface = "";
		            action = "";
		            timed = "";
		            week = "";
		            day = "";
		            in_limit = "";
		            out_limit = "";
		            comment = "";
				}
			}
			cur=cur->NextSibling();
		}
	}

	void Parser::parseFirewallpolicyinfo(TiXmlNode * cur){
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "firewall_enable") == 0){
					firewallpolicymgr.setFirewallEnable(true);
					if(getNodeValue(cur).compare("0") == 0){
						firewallpolicymgr.setFirewallEnable(false);
					}
				}
				else if(strcmp(cur->Value(), "firewall") == 0){
					parseFirewallpolicy(cur);
					return;
				}
			}
			cur=cur->NextSibling();
		}
	}

	int	Parser::processTraffic(){
		TiXmlNode * traffic_node = findNode(&doc, "xmlconfig/traffic");

		if(traffic_node != NULL && traffic_node->FirstChild() != NULL){	
			parseTraffic(traffic_node->FirstChild());
		}
		return 0;
	}

	void Parser::parseTrafficChannel(TiXmlNode * cur, std::string traffic_interface){
		std::string class_name = "";
		std::string bridge_id = "0";
		std::string bandwidth = "";
		std::string parent_name = "";
		std::string rate = "";
		std::string is_default = "";
		std::string priority = "";
		std::string share_name = "";
		std::string class_type = "";
		std::string limit = "";
		std::string weight = "";
		std::string attribute = "";
		std::string comment = "";
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "channel") == 0){

					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();

					while(attr){
						if(strcmp(attr->Name(), "classname") == 0){
							class_name = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "parentname") == 0){
							parent_name = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "sharename") == 0){
							share_name = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "classtype") == 0){
							class_type = attr->Value() ? attr->Value() : "";
						}
						//else if(strcmp(attr->Name(), "bandwidth") == 0){
						//	bandwidth = attr->Value() ? attr->Value() : "";
						//}
						else if(strcmp(attr->Name(), "rate") == 0){
							rate = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "default") == 0){
							is_default = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "priority") == 0){
							priority = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "limit") == 0){
							limit = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "weight") == 0){
							weight = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "attribute") == 0){
							attribute = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(),"bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "0";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					if(class_name.size() > 0){
						//if(parent.compare("root") == 0){
						//	parent = traffic_interface;
						//}
						//std::cout <<
						if(comment.size() == 0){
							comment = class_name;
						}
						trafficmgr.add(bridge_id,traffic_interface, class_name, parent_name, share_name,class_type,rate, 
							(is_default.compare("1") == 0) ? true : false,priority,limit,weight,attribute,comment );										
					}
					class_name   = "";
					parent_name = "";
					share_name  = "";
					rate              = "";
					bandwidth     = "";
					is_default      = "";
					priority          = "";
					class_type     = "";
					limit              = "";
					weight 	      = "";
					attribute        = "";
					comment       = "";

				}
			}

			cur=cur->NextSibling();
		}
	}

	void Parser::parseTraffic(TiXmlNode * cur){
	
		std::string class_name = "";
		std::string bridge_id = "0";
		std::string bandwidth = "";
		std::string parent = "";
		std::string rate = "";
		std::string ceil = "";
		std::string sfq = "";
		std::string is_default = "";
		std::string priority = "";
		std::string per_host = "";
		std::string per_limit = "";
		std::string per_cidr = "";
		std::string per_rate = "";
		std::string per_ceil = "";
		std::string per_dir = "";
		std::string comment = "";
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "channels") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "classname") == 0){
							class_name= attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "bandwidth") == 0){
							bandwidth = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}

					if((class_name.compare("upload") == 0 || class_name.compare("download") == 0) && bandwidth.size() > 0){
						if(class_name.compare("download") == 0){

							//traffic_interface = bridge_id + "_INT_PORT"; 
							traffic_interface = "INT" + bridge_id;
						}
						else{
							//traffic_interface = bridge_id + "_OUT_PORT";
							traffic_interface = "EXT" + bridge_id;
						}
						trafficmgr.add(bridge_id,traffic_interface, class_name, "root", "", "3",bandwidth, 
							false, "", "", "", "", comment);	
					}
					parseTrafficChannel(cur->FirstChild(), traffic_interface);
					
					class_name = "";
					bandwidth = "";
					bridge_id = "0";
					comment = "";
				}
				else if(strcmp(cur->Value(), "traffic_control") == 0){
					trafficmgr.setEnable(false);
					if(getNodeValue(cur).compare("1") == 0)
					{
						trafficmgr.setEnable(true);
					}									
				}

			}

			cur=cur->NextSibling();
		}
	}


	int		Parser::processSession(){

		TiXmlNode * sessions_node = findNode(&doc, "xmlconfig/session_limit");

		if(sessions_node != NULL && sessions_node->FirstChild()){	
			parseSession(sessions_node->FirstChild());
		}

		return 0;

	}

	void	Parser::parseSession(TiXmlNode * cur){
	
		std::string name = "";
		std::string session = "";
		std::string comment = "";


		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						session = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				//std::cout << "protocol: " << name << std::endl;
				sessionmgr.add(name, session, comment);
				
				name = "";
				session = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseSession(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	/*int 	Parser::processProtocol(){
		processProtocol("english");
	}*/

	int		Parser::processProtocol(const std::string &lanaguage){
		std::string tmpKey = "xmlconfig/english/protocols";
		TiXmlDocument l7_doc;
		if(!l7_doc.LoadFile(l7_proto_config_filename.c_str()))
		{
			error("parse l7 protocols xml config file error: %s", l7_proto_config_filename.c_str());
			return 1;
		}

		if(lanaguage.length() > 0)
			tmpKey = "xmlconfig/" + lanaguage + "/protocols";
		else
			tmpKey = "xmlconfig/english/protocols";	
		
		TiXmlNode * l7_cur = findNode(&l7_doc, tmpKey);
		if(l7_cur != NULL && l7_cur->FirstChild() != NULL){
			parseProtocol(l7_cur->FirstChild());
		}

		TiXmlNode * protos_node = findNode(&doc, "xmlconfig/protocols");

		if(protos_node != NULL && protos_node->FirstChild() != NULL){	
			parseProtocol(protos_node->FirstChild());
		}

		TiXmlNode * protos_group_node = findNode(&doc, "xmlconfig/protocols_groups");
		if(protos_group_node != NULL && protos_group_node->FirstChild() != NULL){	
			parseProtocolGroup(protos_group_node->FirstChild());
		}
		
		return 0;

	}

	int		Parser::processLayer7Protocol(const std::string &lanaguage){
		std::string tmpKey = "xmlconfig/english/protocols";
		TiXmlDocument l7_doc;
		
		if(!l7_doc.LoadFile(l7_proto_config_filename.c_str()))
		{
			error("parse l7 protocols xml config file error: %s", l7_proto_config_filename.c_str());
			return 1;
		}
		
		if(lanaguage.length() > 0)
			tmpKey = "xmlconfig/" + lanaguage + "/protocols";
		
		TiXmlNode * l7_cur = findNode(&l7_doc, tmpKey);
		if(l7_cur != NULL && l7_cur->FirstChild() != NULL){
			parseProtocol(l7_cur->FirstChild());
		}
		
		if(lanaguage.length() > 0)
			tmpKey = "xmlconfig/" + lanaguage + "/protocol_type";
		else
			tmpKey = "xmlconfig/english/protocol_type";	

		TiXmlNode * l7_prototype = findNode(&l7_doc, tmpKey);
		if(l7_prototype != NULL && l7_prototype->FirstChild() != NULL){
			parseLayer7ProtoType(l7_prototype->FirstChild());
		}

		return 0;
	}

	int		Parser::processCustomProtocol(){
		TiXmlNode * protos_node = findNode(&doc, "xmlconfig/protocols");

		if(protos_node != NULL && protos_node->FirstChild() != NULL){	
			parseProtocol(protos_node->FirstChild());
		}

		return 0;
	}
	
	int		Parser::processCustomProtocolGroup(const std::string &lanaguage){
		return processProtocol(lanaguage);
	}

	void	Parser::parseLayer7ProtoType(TiXmlNode * cur){
		std::stringstream temp;
		std::string id_str = "";
		int			id = 0;

		while(cur){
			std::string name = "";
            std::string value = "";
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                id = 0;
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : " ";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						//name = name + "|" + attr->Value() ? attr->Value() : " "
						id_str = id_str + "|"  + attr->Value(); // ? attr->Value() : "";
						
						//value = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "catid") == 0){
						id_str = attr->Value() ? attr->Value() : "";
						//temp << id_str;
						//temp >> id;
					}
					else if (strcmp(attr->Name(),"show") ==0){
						if(attr->Value() && strcmp(attr->Value(),"0") == 0)
                        	id_str = id_str + "|" + attr->Value();
						else
							id_str = id_str + "|1";
						//id_str = attr->Value() ? attr->Value() : "";
						//temp << id_str;
						//temp >> show;
					}
					attr=attr->Next();
				}
				//std::cout << "protocol: " << name << std::endl;
				//mapLayer7prototype.add(name, value);
				mapLayer7prototype.insert(std::map<std::string,std::string>::value_type(name,id_str));
				//mapLayer7protogid.insert(std::map<std::string,int>::value_type(id,name));
				//name = "";
				//value = "";
				
				//if(cur->NextSibling()){
				//	parseLayer7ProtoType(cur->NextSibling());
				//}
				cur=cur->NextSibling();
				continue;
			}
			cur=cur->FirstChild();
		}
		return;

	}

	void	Parser::parseProtocol(TiXmlNode * cur ){
		while(cur){
			int chid = 0;
		    std::stringstream temp;
		    std::string id = "";
		    std::string name = "";
		    std::string proto = "";
		    std::string sport = "";
		    std::string dport = "";
		    std::string match = "";
		    std::string type = "";
		    std::string option = "";
		    std::string comment = "";
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
						temp << id;
						temp >> chid;
					}
					else if(strcmp(attr->Name(), "proto") == 0){
						proto = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "sport") == 0){
						sport = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "dport") == 0){
						dport = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "match") == 0){
						match = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "type") == 0){
						type = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "option") == 0){
						option = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				//std::cout << "protocol: " << name << std::endl;
				protocolmgr.add(chid,name, proto, sport, dport, match,type, option, comment);

				id = "";
				name = "";
				proto = "";
				sport = "";
				dport = "";
				match = "";
				type = "";
				option = "";
				comment = "";
				
				//if(cur->NextSibling()){
				//	parseProtocol(cur->NextSibling());
				//}
				cur=cur->NextSibling();
				continue;
			}
			cur=cur->FirstChild();
		}
		return;
	}

	void	Parser::parseProtocolGroup(TiXmlNode * cur){
		std::string name;
		std::string comment;
		while(cur){

			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				if(strcmp(cur->Value() , "group") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();					
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							//std::cout << "parseProtocolGroup: group_name :" << attr->Value() << std::endl;
							group_name = attr->Value();
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}
						attr=attr->Next();
					}
					if(group_name.size() > 0){
						protocolmgr.addGroup(group_name, comment);
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
						//std::cout << "protocolmgr.addSubNode: " << group_name << ":" << name << std::endl;
						protocolmgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseProtocolGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}

	int Parser::processWifiInfo()
	{
	      TiXmlNode *wifi_node = NULL;
		wifi_node = findNode(&doc,"xmlconfig/wifi");
		if((wifi_node != NULL) && (wifi_node->FirstChild() != NULL))
		{
			parseWifi(wifi_node->FirstChild());
		}
		return 0;
	}

	void Parser::parseWifi(TiXmlNode * cur)
	{
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "enable") == 0){
					wifimgr.setWifiEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						wifimgr.setWifiEnable(true);
					}
				}
				else if(strcmp(cur->Value(), "pc_discover_time") == 0){
					wifimgr.setPcTime(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "max_time") == 0){
					wifimgr.setMaxTime(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "share_num") == 0){
					wifimgr.setShareNum(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "admins") == 0){
					wifimgr.setAdmins(getNodeValue(cur));
				}
				else if(strcmp(cur->Value(), "address_pools") == 0){
					parseWifipool(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
	}

	void Parser::parseWifipool(TiXmlNode * cur)
	{
		std::string wifipoolid;
              std::string wifipoolstartip;
              std::string wifipoolendip;
              std::string wifipoolcomment;
              bool wifipoolenable = false;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "pool") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "id") == 0){
							wifipoolid = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "startip") == 0){
							wifipoolstartip = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "endip") == 0){
							wifipoolendip = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							wifipoolcomment = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "enable") == 0){
							wifipoolenable = false;
							if(strcmp(attr->Value(),"1") == 0){
								wifipoolenable = true;
							}
						}

						attr=attr->Next();
					}

					wifimgr.pool_add(wifipoolid, wifipoolstartip, wifipoolendip, wifipoolcomment, wifipoolenable);

					wifipoolid = "";
					wifipoolstartip = "";
					wifipoolendip = "";
					wifipoolcomment = "";
				}
			}

			cur=cur->NextSibling();
		}
	}	

    int	Parser::processServInfo()
    {
		std::string tmpKey = "xmlconfig/servinfos";
		TiXmlDocument si_doc;
              TiXmlNode * si_cur = NULL;
		
		if(!si_doc.LoadFile(server_info_config_filename.c_str()))
		{
			error("parse server info xml config file error: %s", server_info_config_filename.c_str());
			return 1;
		}
        
		si_cur = findNode(&si_doc, tmpKey);
		if(si_cur != NULL && si_cur->FirstChild() != NULL){
			parseServInfo(si_cur->FirstChild());
		}

		return 0;
	}

    void Parser::parseServInfo(TiXmlNode * cur){

		std::string type = "";
		std::string name = "";
        std::string en_name = "";
		std::string zonename = "";
        std::string en_zonename = "";
		std::string var1 = "";
		std::string var2 = "";
        std::string var3 = "";
		std::string var4 = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "type") == 0){
						type = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
                    else if(strcmp(attr->Name(), "en_name") == 0){
						en_name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "zonename") == 0){
						zonename = attr->Value() ? attr->Value() : "";
					}
                    else if(strcmp(attr->Name(), "en_zonename") == 0){
						en_zonename = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "var1") == 0){
						var1 = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "var2") == 0){
						var2 = attr->Value() ? attr->Value() : "";
					}
                    else if(strcmp(attr->Name(), "var3") == 0){
						var3 = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "var4") == 0){
						var4 = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}

				servinfomgr.add(type, name, en_name, zonename, en_zonename, 
                                var1, var2, var3, var4, comment);
				
				type = "";
				name = "";
                en_name = "";
				zonename = "";
                en_zonename = "";
				var1 = "";
				var2 = "";
                var3 = "";
				var4 = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseServInfo(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

    int Parser::processServConf()
    {
        TiXmlNode * sc_node = findNode(&doc, "xmlconfig/servinfos");

		//if(sc_node == NULL)
		//	return 1;

		if(sc_node != NULL && sc_node->FirstChild()){	
			parseServConf(sc_node->FirstChild());
		}

		return 0;
    }

    void Parser::parseServConf(TiXmlNode * cur){

        std::string type = "";
		std::string en_name = "";
		std::string en_zonename = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
                    if (strcmp(attr->Name(), "type") == 0) {
                        type = attr->Value() ? attr->Value() : "";
                    }
					else if(strcmp(attr->Name(), "en_name") == 0){
						en_name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "en_zonename") == 0){
						en_zonename = attr->Value() ? attr->Value() : "";
					}

					attr=attr->Next();
				}

				servconfmgr.add(type, en_name, en_zonename);

                type = "";
				en_name = "";
				en_zonename = "";
				
				if(cur->NextSibling()){
					parseServConf(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}

	int		Parser::processVlan(){
		TiXmlNode * vlans_node = findNode(&doc, "xmlconfig/vlans");

		if(vlans_node != NULL && vlans_node->FirstChild()){	
			parseVlan(vlans_node->FirstChild());
		}

		return 0;
	}

	void	Parser::parseVlan(TiXmlNode * cur){

		std::string name = "";
		std::string vlans = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "vlans") == 0){
						vlans = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				//std::cout << "schedule: " << name << start_time << stop_time << std::endl;
				vlanmgr.add(name, vlans, comment);
				
				name = "";
				vlans = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseVlan(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	}



    /*2012-09-26 add   processPort */
    int     Parser::processPortScan(){
            TiXmlNode * port_node = findNode(&doc, "xmlconfig/portscans");
         
            if(port_node==NULL){
                portscanmgr.add("tcp", "1000", "1000", "600");
                portscanmgr.add("udp", "1000", "1000", "300");
                portscanmgr.add("icmp", "1000", "1000", "300");
                
            }
            else{
                if(port_node != NULL && port_node->FirstChild()){ 
                    parsePortScan(port_node->FirstChild());
                }
            }
            return 0;
        }
    void    Parser::parsePortScan(TiXmlNode * cur){
    
            std::string type = "";
            std::string newer = "";
            std::string pending = "";
            std::string interval = "";

            while(cur){
                if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
    
                    TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                    while(attr){
                        //std::cout << " Name:   " << attr->Name() << " value: " << attr->Value() << std::endl;
                        if(strcmp(attr->Name(), "conntype") == 0){
                            type = attr->Value() ? attr->Value() : "";
                        }
                        else if(strcmp(attr->Name(), "newconn") == 0){
                            newer = attr->Value() ? attr->Value() : "";
                        }
                        else if(strcmp(attr->Name(), "doubtconn") == 0){
                            pending = attr->Value() ? attr->Value() : "";
                        }
                        else if(strcmp(attr->Name(), "loginterval") == 0){
                            interval = attr->Value() ? attr->Value() : "";
                        }
                        attr=attr->Next();
                    }
                    
                    portscanmgr.add(type, newer, pending, interval);
                    
                    type = "";
                    newer = "";
                    pending = "";
                    interval = "";

                    if(cur->NextSibling()){
                        parsePortScan(cur->NextSibling());
                    }
                }
                cur=cur->FirstChild();
            }
            return;
        }

	int Parser::processTiming()
	{
		TiXmlNode *timing_node = NULL;
		timing_node = findNode(&doc,"xmlconfig/timings");
		if((timing_node != NULL) && (timing_node->FirstChild() != NULL))
		{
			parseTiming(timing_node->FirstChild());
		}
		return 0;
	}

	void Parser::parseTiming(TiXmlNode * cur)
	{		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "adsls") == 0){
					parseTimingAdsl(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
	}

	void Parser::parseTimingAdsl(TiXmlNode * cur)
	{
		std::string name = "";
		std::string iface = "";
		std::string minute = "";
		std::string hour = "";
		std::string day = "";
		std::string month = "";
		std::string week = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "adsl") == 0){
					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "interface") == 0){
							iface = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "minute") == 0){
							minute = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "hour") == 0){
							hour = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "day") == 0){
							day = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "month") == 0){
							month = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "week") == 0){
							week = attr->Value() ? attr->Value() : "";
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value() ? attr->Value() : "";
						}
						attr=attr->Next();
					}
					timingmgr.add(name, iface, minute, hour, day, month, week, comment);

					name = "";
					iface = "";
					minute = "";
					hour = "";
					day = "";
					month = "";
					week = "";
					comment = "";
				}
			}

			cur=cur->NextSibling();
		}

	}

	void Parser::parseTimingreboot(TiXmlNode * cur)
	{
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "boot") == 0){
					return;
				}
			}
		}
	}

	 int	Parser::processPing()
	 {
		TiXmlNode *ping_node = NULL;
	/*	
		if(getNodeValuebyName("network_check") == "")
		{
			printf("Check node:network_check failed\n");
			goto exit;
		}

		if(getNodeValuebyName("network_check/zero_ping") == "")
		{
			printf("Check node:network_check/zero_ping failed\n");
			goto exit;
		}
		*/
		
	 	ping_node = findNode(&doc,"xmlconfig/network_check");
		if((ping_node != NULL) && (ping_node->FirstChild() != NULL))
		{
			parsePing(ping_node->FirstChild());
		}

		return 0;
	 }

	void    Parser::parsePing(TiXmlNode * cur)
	{
        while(cur)
		{
        	if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
			{
            	if(strcmp(cur->Value(), "zero_ping") == 0)
				{
					networkdetectionmgr.setZeroPingValue(0);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						networkdetectionmgr.setZeroPingValue(1);
					}
                }
            }
                cur=cur->FirstChild();
        }
        return;
	}

	int Parser::processFirstGameChannel()
	{
		TiXmlNode *f_game_channel = NULL;

		f_game_channel = findNode(&doc,"xmlconfig/first_game_channel");
		if((f_game_channel != NULL) && (f_game_channel->FirstChild() != NULL))
		{
			parseFirstGameChannel(f_game_channel->FirstChild());
		}

		return 0;
	}

	void Parser::parseFirstGameChannel(TiXmlNode * cur)
	{
		std::string id;
		std::string name;
		if(!cur)
			return;

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "id") == 0){
						id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				firstgcmgr.add(id,name);
				
				id = "";
				name = "";
				
				if(cur->NextSibling()){
					parseFirstGameChannel(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}

	int Parser::processRouteserverinfo(){
		TiXmlNode *routeserver = NULL;

		routeserver = findNode(&doc,"xmlconfig/routeserverinfos");
		if((routeserver != NULL) && (routeserver->FirstChild() != NULL))
		{
			parseRouteserverinfo(routeserver->FirstChild());
		}

		return 0;
	}

	void Parser::parseRouteserverinfo(TiXmlNode * cur){
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "enable") == 0){
					routerserver.setenable(true);
					if(getNodeValue(cur).compare("0") == 0) 
					{
						routerserver.setenable(false);
					}
				}
				else if(strcmp(cur->Value(), "passwd_enable") == 0){
					routerserver.setpasswdenable(true);
					if(getNodeValue(cur).compare("0") == 0) 
					{
						routerserver.setpasswdenable(false);
					}
				}
			}
			cur=cur->NextSibling();
		}
	}

    
	int	Parser::processSchedule(){
		TiXmlNode * schedules_node = findNode(&doc, "xmlconfig/schedules");

		if((schedules_node != NULL) && (schedules_node->FirstChild() != NULL)){	
			parseSchedule(schedules_node->FirstChild());
		}

		TiXmlNode * schedules_group_node = findNode(&doc, "xmlconfig/schedules_groups");
		if((schedules_group_node != NULL) && (schedules_group_node->FirstChild() != NULL)){	
			parseScheduleGroup(schedules_group_node->FirstChild());
		}

		return 0;
	}

	int		Parser::processXmllibSchedule(){
		TiXmlNode * schedules_node = findNode(&doc, "xmlconfig/schedules");
		
		if((schedules_node != NULL) && (schedules_node->FirstChild() != NULL)){	
			parseXmlLibSchedule(schedules_node->FirstChild());
		}
		
		TiXmlNode * schedules_group_node = findNode(&doc, "xmlconfig/schedules_groups");
		if((schedules_group_node != NULL) && (schedules_group_node->FirstChild() != NULL)){	
			parseXmllibScheduleGroup(schedules_group_node->FirstChild());
		}
		//xmllibschedulemgr.output();

		return 0;
	}

	int	Parser::processFirewall(bool forScript){
		TiXmlNode * firewalls_node = findNode(&doc, "xmlconfig/firewalls");
		if((firewalls_node != NULL) && (firewalls_node->FirstChild() != NULL)){	
			parseFirewall(firewalls_node->FirstChild(), forScript);
		}

		return 0;
	}

	void Parser::parseSchedule(TiXmlNode * cur ){
		
		std::string name = "";
		std::string start_time = "";
		std::string stop_time = "";
		std::string days = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "start_time") == 0){
						start_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "stop_time") == 0){
						stop_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "days") == 0){
						days = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				//std::cout << "schedule: " << name << start_time << stop_time << std::endl;
				schedulemgr.add(name, start_time, stop_time, days, comment);
				
				name = "";
				start_time = "";
				stop_time = "";
				days = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseSchedule(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
		return;
	
	}

	void		Parser::parseXmlLibSchedule(TiXmlNode * cur ){
		
		std::string name = "";
		std::string start_time = "";
		std::string stop_time = "";
		std::string days = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "start_time") == 0){
						start_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "stop_time") == 0){
						stop_time = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "days") == 0){
						days = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				//std::cout << "schedule: " << name << start_time << stop_time << std::endl;
				xmllibschedulemgr.add(name, start_time, stop_time, days, comment);
				
				name = "";
				start_time = "";
				stop_time = "";
				days = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseXmlLibSchedule(cur->NextSibling() );
				}
			}
			cur=cur->FirstChild();
		}
		return;
	
	}
	
	void	Parser::parseScheduleGroup(TiXmlNode * cur ){
	
		std::string name;
		std::string comment;
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
						schedulemgr.addGroup(group_name, comment);
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
						schedulemgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseScheduleGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}
	
	void	Parser::parseXmllibScheduleGroup(TiXmlNode * cur){
	
		std::string name;
		std::string comment;
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
						xmllibschedulemgr.addGroup(group_name, comment);
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
						xmllibschedulemgr.addSubNode(group_name, name);
					}
					name = "";
				}

				if(cur->FirstChild()){
					parseXmllibScheduleGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}

	void		Parser::parseNetworkAddr(TiXmlNode * cur){
		std::string name = "";
		std::string value = "";
		bool range = false;
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						value = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "range") == 0){
						range = strcasecmp(attr->Value(), "true")==0 ? true : false;
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				netaddrmgr.add(name, value, range,comment);
				
				name = "";
				value = "";
				range = false;
				comment = "";
				
				if(cur->NextSibling()){
					parseNetworkAddr(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}

	void Parser::parseMacAuth(TiXmlNode * cur){
		std::string name = "";
		std::string value = "";
		std::string comment = "";

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						value = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				macauthmgr.add(name, value, comment);
				
				name = "";
				value = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseMacAuth(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}

	}
		
	
	void Parser::parseFirewall(TiXmlNode * cur ,bool forScript){

		std::string name;
		std::string bridge_id;
		std::string auth;
		std::string action;
		std::string proto;
		std::string schedule;
		std::string vlan;
		std::string src;
		std::string dst;
		std::string mac;
		std::string in_traffic;
		std::string out_traffic;
		std::string session_limit;
		std::string quota_name;
		std::string quota_action;
		std::string second_in_traffic;
		std::string second_out_traffic;
		bool		log_packet = false;
		std::string comment;
		bool		disabled = false;
		
		while(cur){

			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value(), "application_firewall") == 0){
					firewallmgr.setEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						firewallmgr.setEnable(true);
					}
				}
				else if(strcmp(cur->Value(), "macauth_control") == 0){
					firewallmgr.setMacAuthEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						firewallmgr.setMacAuthEnable(true);
					}
				}
				else if(strcmp(cur->Value() , "firewall") == 0){

					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value();
						}
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value();
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action = attr->Value();
						}
						else if(strcmp(attr->Name(), "proto") == 0){
							proto = attr->Value();
						}
						else if(strcmp(attr->Name(), "schedule") == 0){
							schedule = attr->Value();
						}
						else if(strcmp(attr->Name(), "auth") == 0)
						{
							auth = attr->Value();
						}
						else if(strcmp(attr->Name(), "vlan") == 0){
							vlan = attr->Value();
						}
						else if(strcmp(attr->Name(), "src") == 0){
							src = attr->Value();
						}
						else if(strcmp(attr->Name(), "dst") == 0){
							dst = attr->Value();
						}
						else if(strcmp(attr->Name(), "mac") == 0){
							mac = attr->Value();
						}
						else if(strcmp(attr->Name(), "in_traffic") == 0){
							in_traffic = attr->Value();
						}
						else if(strcmp(attr->Name(), "out_traffic") == 0){
							out_traffic = attr->Value();
						}
						else if(strcmp(attr->Name(), "session_limit") == 0){
							session_limit = attr->Value();
						}
						else if(strcmp(attr->Name(), "quota") == 0){
							quota_name = attr->Value();
						}
						else if(strcmp(attr->Name(), "quota_action") == 0){
							quota_action = attr->Value();
						}
						else if(strcmp(attr->Name(), "second_in_traffic") == 0){
							second_in_traffic = attr->Value();
						}
						else if(strcmp(attr->Name(), "second_out_traffic") == 0){
							second_out_traffic = attr->Value();
						}
						else if(strcmp(attr->Name(), "log") == 0){
							if(strcmp(attr->Value(), "1") == 0)
								log_packet = true;
							else
								log_packet = false;
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}
						else if(strcmp(attr->Name(), "disabled") == 0){
							if(strcmp(attr->Value(), "0") == 0){
								disabled = false;
							}
							else{
								disabled = true;
							}
						}

						attr=attr->Next();
					}
					
					if(name.size() > 0 && (!disabled || !forScript)){
						firewallmgr.add(name, bridge_id, proto, auth, action, schedule, vlan,
								src, dst,mac, in_traffic, out_traffic, session_limit, log_packet, 
								quota_name,quota_action,second_in_traffic,
								second_out_traffic,comment,disabled);
					}
					name = "";
					bridge_id = "";
					auth = "";
					action = "";
					proto = "";
					schedule = "";
					vlan = "";
					src = "";
					dst = "";
					mac = "";
					in_traffic = "";
					out_traffic = "";
					session_limit = "";
					quota_name = "";
					quota_action = "";
					second_in_traffic = "";
					second_out_traffic = "";
					log_packet = false;
					comment = "";
					disabled = "";
				}

				if(cur->NextSibling()){
					parseFirewall(cur->NextSibling(),forScript);
				}
			}
			cur=cur->FirstChild();			
		}
		return;
	}

	void Parser::parseNetworkAddrGroup(TiXmlNode * cur){
		std::string name;
		std::string comment;
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
						netaddrmgr.addGroup(group_name, comment);
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
						netaddrmgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseNetworkAddrGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}

	void Parser::parseMacAuthGroup(TiXmlNode * cur){
		std::string name;
		std::string comment;

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
						macauthmgr.addGroup(group_name, comment);
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
						macauthmgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseMacAuthGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}

	int Parser::processNetworkAddr(){
		TiXmlNode * networks_node = findNode(&doc, "xmlconfig/networks");
		if((networks_node != NULL) && (networks_node->FirstChild() != NULL)){	
			parseNetworkAddr(networks_node->FirstChild());
		}

		TiXmlNode * networks_group_node = findNode(&doc, "xmlconfig/networks_groups");	
		if((networks_group_node != NULL) && (networks_group_node->FirstChild() != NULL)){	
			parseNetworkAddrGroup(networks_group_node->FirstChild());
		}

		return 0;
	}

	int Parser::processMacAuth(){
		TiXmlNode * macauths_node = findNode(&doc, "xmlconfig/macauths");
	
		if((macauths_node != NULL) && (macauths_node->FirstChild() != NULL)){	
			parseMacAuth(macauths_node->FirstChild());
		}

		TiXmlNode * macauths_group_node = findNode(&doc, "xmlconfig/macauth_groups");
		
		if((macauths_group_node != NULL) && (macauths_group_node->FirstChild() != NULL)){	
			parseMacAuthGroup(macauths_group_node->FirstChild());
		}
		return 0;
	}

	int Parser::processSystem(){
		int ret = 1;

		system.setHostName(getNodeValuebyName("system/hostname"));
		system.setDomainName(getNodeValuebyName("system/domainname"));
		system.setContact(getNodeValuebyName("system/contact"));
		system.setLocation(getNodeValuebyName("system/location"));
		system.setPassword(getNodeValuebyName("system/password"));
		system.setPassword2(getNodeValuebyName("system/password2"));
		system.setTimezone(getNodeValuebyName("system/timezone"));
		system.setNTPServer(getNodeValuebyName("system/ntp_server"));
		system.setMTU(getNodeValuebyName("system/bridge_mtu"));
		system.setSNMPRO(getNodeValuebyName("system/snmp_ro_community"));
		system.setSNMPRW(getNodeValuebyName("system/snmp_rw_community"));

		system.setbridgenum(getNodeValuebyName("system/bridgenum"));
		system.setmax_bridgenum(getNodeValuebyName("system/max_bridgenum"));

		std::string ntp_enable = getNodeValuebyName("system/ntp_enable");

		if(ntp_enable.compare("1") == 0){
			system.setEnableNTP(true);
		}
		else{
			system.setEnableNTP(false);
		}

		std::string enable = getNodeValuebyName("system/enable_stp");

		if(enable.compare("1") == 0){
			system.setEnableSTP(true);
		}
		else{
			system.setEnableSTP(false);
		}

		enable = getNodeValuebyName("system/dbridge_perf");

		if(enable.compare("1") == 0){
			system.setDBP(true);
		}
		else{
			system.setDBP(false);
		}
#ifdef CONFIG_IXP
        enable = getNodeValuebyName("system/firmware_auto_update");
		if(enable.compare("1") == 0){
			system.setEnableFAutoUpdate(true);
		}
		else{
			system.setEnableFAutoUpdate(false);
		}
		enable = getNodeValuebyName("system/module_auto_update");
		if(enable.compare("1") == 0){
			system.setEnableMAutoUpdate(true);
		}
		else{
			system.setEnableMAutoUpdate(false);
		}
#else
		enable = getNodeValuebyName("system/auto_update");
		if(enable.compare("1") == 0){
			system.setEnableAutoUpdate(true);
		}
		else{
			system.setEnableAutoUpdate(false);
		}
#endif
		system.setUpdateURL(getNodeValuebyName("system/update_url"));

		enable = getNodeValuebyName("system/snmp_enable");

		if(enable.compare("1") == 0){
			system.setSNMPenable(true);
		}
		else{
			system.setSNMPenable(false);
		}
		if(system.isVaild()){
			ret = 0;
		}
		else{
			error("system configura is not vaild!");
		}
		
		
		return ret;
	}

	int Parser::processAuth(){
		int ret = 1;
		std::string enable = getNodeValuebyName("auth/enable");

		if(enable.compare("1") == 0){
			auth.setEnable(true);
		}
		else{
			auth.setEnable(false);
		}

		auth.setTimeoutMode(getNodeValuebyName("auth/timeout_mode"));
		auth.setTimeoutSeconds(getNodeValuebyName("auth/timeout_seconds"));

		enable = getNodeValuebyName("auth/enable_multi_user");

		if(enable.compare("1") == 0){
			auth.setEnableMutilUser(true);
		}
		else{
			auth.setEnableMutilUser(false);
		}

		auth.setRedirectHTTPURL(getNodeValuebyName("auth/redirect_http_url"));
		auth.setDenyGuestAccess(getNodeValuebyName("auth/deny_guest_access"));
		
		enable = getNodeValuebyName("auth/redirect_http_request");

		if(enable.compare("1") == 0){
			auth.setRedirectHTTPRequest(true);
		}
		else{
			auth.setRedirectHTTPRequest(false);
		}

		if(auth.isVaild()){
			ret = 0;
		}
		else{
			error("auth configura is not vaild!");
		}		
		
		return ret;
	}


	int Parser::processManagement(){
		int ret = 1;
		
		management.setIP(getNodeValuebyName("management/ip"));
		management.setNetmask(getNodeValuebyName("management/netmask"));
		management.setBroadcast(getNodeValuebyName("management/broadcast"));
		management.setGateway(getNodeValuebyName("management/gateway"));
		management.setServer(getNodeValuebyName("management/server"));
		management.setDNSPrimary(getNodeValuebyName("management/dns_primary"));
		management.setDNSSecondary(getNodeValuebyName("management/dns_secondary"));

		if(management.isVaild()){
			ret = 0;
		}
		else{
			error("management configure is not vaild!");
		}
		return ret;

	}
	
	int Parser::processDnsHelper(bool forScript){
		
		TiXmlNode * domain_node = findNode(&doc, "xmlconfig/domain_data");

		if((domain_node != NULL) && (domain_node->FirstChild() != NULL)){	
			parseDomain(domain_node->FirstChild());
		}

		TiXmlNode * domain_group_node = findNode(&doc, "xmlconfig/domain_groups");
		if((domain_group_node != NULL) && (domain_group_node->FirstChild() != NULL)){	
			parseDomainGroup(domain_group_node->FirstChild());
		}	

		TiXmlNode * dnspolicy_node = findNode(&doc, "xmlconfig/dns_policy");
		if((dnspolicy_node != NULL) && (dnspolicy_node->FirstChild() != NULL)){	
			parseDnspolicy(dnspolicy_node->FirstChild(),forScript);
		}
		return 0;
	}
	
	void Parser::parseDomain(TiXmlNode * cur){

		std::string name = "";
		std::string value = "";
		std::string comment = "";		

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						value = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}
				domainmgr.add(name, value, comment);
				
				name = "";
				value = "";
				comment = "";
				
				if(cur->NextSibling()){
					parseDomain(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}

	void Parser::parseDomainGroup(TiXmlNode * cur){
	
		std::string name;
		std::string comment;
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
						domainmgr.addGroup(group_name, comment);
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
						domainmgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseDomainGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;
	}


	
	void Parser::parseDnspolicy(TiXmlNode * cur ,bool forScript){

		std::string name;
		std::string bridge_id;
		std::string src;
		std::string dst;
		std::string domain_name;
		std::string action;
		std::string targetip;
		std::string comment;
		bool disabled = false;
		
		
		while(cur){

			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				/*if(strcmp(cur->Value(), "application_policy") == 0){
					dnspolicymgr.setEnable(false);
					if(getNodeValue(cur).compare("1") == 0) 
					{
						dnspolicymgr.setEnable(true);
					}
				}*/
				if(strcmp(cur->Value() , "dataitem") == 0){

					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value();
						}
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value();
						}
						else if(strcmp(attr->Name(), "src") == 0){
							src = attr->Value();
						}
						else if(strcmp(attr->Name(), "dst") == 0){
							dst = attr->Value();
						}
						else if(strcmp(attr->Name(), "domain_name") == 0){
							domain_name = attr->Value();
						}
						else if(strcmp(attr->Name(), "action") == 0){
							action = attr->Value();
						}
						else if(strcmp(attr->Name(), "targetip") == 0){
							targetip = attr->Value();
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}
						else if(strcmp(attr->Name(), "disabled") == 0){
							if(strcmp(attr->Value(), "0") == 0){
								disabled = false;
							}
							else{
								disabled = true;
							}
						}

						attr=attr->Next();
					}
					
					if(name.size() > 0 && (!disabled || !forScript)){
						dnspolicymgr.add(name, bridge_id, 
								src, dst,domain_name, action, 
								targetip,comment,disabled);
					}
					name = "";
					bridge_id = "";
					action = "";
					src = "";
					dst = "";
					domain_name = "";
					targetip = "";
					comment = "";
					disabled = false;
				}

				if(cur->NextSibling()){
					parseDnspolicy(cur->NextSibling(),forScript);
				}
			}
			cur=cur->FirstChild();			
		}
		return;
	}
	
	int Parser::processHttpDirPolicy(bool forScript){
		
		TiXmlNode * node =NULL;
		
		node = findNode(&doc, "xmlconfig/extension");
		if((node != NULL) && (node->FirstChild() != NULL)){	
			parseExtension(node->FirstChild());
		}
		
		node = findNode(&doc, "xmlconfig/extension_group");		
		if((node != NULL) && (node->FirstChild() != NULL)){ 
			parseExtensionGroup(node->FirstChild());
		}
		
		node = findNode(&doc, "xmlconfig/httpserver");
		if((node != NULL) && (node->FirstChild() != NULL)){	
			parseHttpServer(node->FirstChild());
		}
		
		node = findNode(&doc, "xmlconfig/httpserver_group");		
		if((node != NULL) && (node->FirstChild() != NULL)){ 
			parseHttpServerGroup(node->FirstChild());
		}
		
		node = findNode(&doc, "xmlconfig/http_dir_policy");
		if((node != NULL) && (node->FirstChild() != NULL)){	
			parseHttpDirPolicy(node->FirstChild(),forScript);
		}

		return 0;
	}
	
	void Parser::parseExtension(TiXmlNode * cur){

		std::string name = "";
		std::string value = "";
		std::string comment = "";		

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "value") == 0){
						value = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}

				extensionmgr.add(trimstring(name), trimstring(value), comment);
				
				name = "";
				value = "";				
				comment = "";
				
				if(cur->NextSibling()){
					parseExtension(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}
	
	void Parser::parseExtensionGroup(TiXmlNode * cur){

		std::string name;
		std::string comment;
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
						extensionmgr.addGroup(group_name, comment);
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
						extensionmgr.addSubNode(group_name, name);
					}
				}

				if(cur->FirstChild()){
					parseExtensionGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;

	}
	
	void Parser::parseHttpServer(TiXmlNode * cur){

		std::string name = "";
		std::string value = "";
		std::string comment = "";	

		std::string bridge_id = "";
		std::string ipaddr = "";
		std::string servertype = "";
		std::string prefix = "";
		std::string suffix = "";	

		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){

				TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
				while(attr){
					if(strcmp(attr->Name(), "name") == 0){
						name = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "bridge") == 0){
						bridge_id = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "ip_addr") == 0){
						ipaddr = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "servertype") == 0){
						servertype = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "prefix") == 0){
						prefix = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "suffix") == 0){
						suffix = attr->Value() ? attr->Value() : "";
					}
					else if(strcmp(attr->Name(), "comment") == 0){
						comment = attr->Value() ? attr->Value() : "";
					}
					attr=attr->Next();
				}

				httpservermgr.add(trimstring(name), bridge_id, ipaddr,servertype,prefix,suffix, comment);
				
				name = "";
				value = "";				
				comment = "";

				bridge_id = "";
				ipaddr= "";				
				servertype= "";				
				prefix= "";				
				suffix= "";				
								
				if(cur->NextSibling()){
					parseHttpServer(cur->NextSibling());
				}
			}
			cur=cur->FirstChild();
		}
	}
	
	void Parser::parseHttpServerGroup(TiXmlNode * cur){

		std::string name;
		std::string comment;
		
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
						httpservermgr.addGroup(group_name, comment);
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
					};											
					if(name.size() > 0){
						httpservermgr.addSubNode(group_name, name);												
					}
				}

				if(cur->FirstChild()){
					parseHttpServerGroup(cur->FirstChild());
				}
			}
			cur=cur->NextSibling();
		}
		return;

	}

	void Parser::parseHttpDirPolicy(TiXmlNode * cur,bool forScript){

		std::string name;
		std::string bridge_id;
		std::string extgroup;
		std::string servergroup;
		std::string format;
		std::string comment;
		bool disabled = false;
		
		while(cur){

			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				if(strcmp(cur->Value() , "policy") == 0){

					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "name") == 0){
							name = attr->Value();
						}
						else if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value();
						}
						else if(strcmp(attr->Name(), "extgroup") == 0){
							extgroup = attr->Value();
						}
						else if(strcmp(attr->Name(), "servergroup") == 0){
							servergroup = attr->Value();
						}
						else if(strcmp(attr->Name(), "disabled") == 0){
							if(strcmp(attr->Value(), "0") == 0){
								disabled = false;
							}
							else{
								disabled = true;
							}
								
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}

						attr=attr->Next();
					}

					//if(! disabled){
					httpdirpolicymgr.add(name, bridge_id,trimstring(extgroup),
									trimstring(servergroup),comment,disabled);
					
				}
				else if(strcmp(cur->Value() , "url_format") == 0){

					TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
					while(attr){
						if(strcmp(attr->Name(), "bridge") == 0){
							bridge_id = attr->Value();
						}
						else if(strcmp(attr->Name(), "format") == 0){
							format = attr->Value();
						}
						else if(strcmp(attr->Name(), "comment") == 0){
							comment = attr->Value();
						}

						attr=attr->Next();
					}
						
					urlformatmgr.add( bridge_id,trimstring(format),comment);
					
				}

				name = "";
				bridge_id = "";
				extgroup = "";
				servergroup = "";
				format = "";
				comment = "";
				disabled = false;

				if(cur->NextSibling()){
					parseHttpDirPolicy(cur->NextSibling(),forScript);
				}
			}	
			
			cur=cur->FirstChild();						
		}
			
		return;

	}

	void		Parser::parseInterface(TiXmlNode * cur){

		std::string name = "";
		std::string mode = "";
		std::string speed = "";
		std::string duplex = "";
		std::string comment = "";
		std::string bridgeid = "";
		std::string linkstate = "";
		TiXmlNode * int_cur;
		
		while(cur){
			if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT){
				int_cur = cur;

				if(!cur)
					break;

				name = cur->Value();				

				mode = getNodeValuebyName("interface/" + name + "/mode");
				speed = getNodeValuebyName("interface/" + name + "/speed");
				duplex = getNodeValuebyName("interface/" + name + "/duplex");

				linkstate = getNodeValuebyName("interface/" + name + "/linkmode");
				bridgeid = getNodeValuebyName("interface/" + name + "/bridgeid");

//				std::cout << "name: " << name << "mode: " << mode << "duplex: " << duplex << "speed: " << speed << std::endl;
				interfacemgr.add(name, mode, speed, duplex, linkstate, bridgeid, "");

			}
			cur=int_cur->NextSibling();
		}
	}


	int Parser::processInterface(){
		TiXmlNode * interfaces_node = findNode(&doc, "xmlconfig/interface");

		if((interfaces_node != NULL) && (interfaces_node->FirstChild() != NULL)){	
			parseInterface(interfaces_node->FirstChild());
		}
		return 0;
	}
	

	std::string Parser::getNodeValue(TiXmlNode *node)
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
	
	std::string Parser::getNodeValuebyName(const std::string nodename){
		TiXmlNode *node;	

		node = findNode(&doc, "xmlconfig/" + nodename);
		return getNodeValue(node);
	}

	TiXmlNode  *Parser::findNode(TiXmlNode *root, const std::string nodename)
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
		


	/*  Split string of names separated by SPLIT_CHAR  */
	void Parser::split(std::string instr, std::vector<std::string> *lstoutstr , const char* splitchar) {

		std::string str = instr;
		unsigned int loc;
		/*  Break string into substrings and store beginning  */
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
	

/**
* Description:  删除字符串左右的空格
* @param  str : 需要删除左右空格的字符�?
* @param strlen : 字符串的长度
* @return        返回修改后的字符串指针，
		此指针是指向传入字符串的一个值，并不新申请内�?
*/
std::string Parser::trimstring( const std::string& str )
{
	std::string t = str;

	std::string::iterator i;
	for (i = t.begin(); i != t.end(); i++) {
		if (!isspace(*i)) {
			t.erase(t.begin(), i);
			break;
		}
	}

	if (i == t.end()) {
		return t;
	}

	for (i = t.end() - 1; i != t.begin(); i--) {
		if (!isspace(*i)) {
			t.erase(i + 1, t.end());
			break;
		}
	}

	return t;
}


/* 
remark: 分配一个内存段，大�?为size字节
return: 返回分配的内存段
*/
void * Parser::AllocBuf(int size)
{
/*#if 0
#define DEBUGPR(x) printf(x);
#else
#define DEBUGPR(x)
#endif*/

	void * tmpbuf = NULL;

	if(tmpbuf != NULL){
		free( tmpbuf);
		tmpbuf = NULL;
	}

	tmpbuf = malloc(sizeof(char)*size);

	return tmpbuf;	
}


int Parser::GetStructSystem(struct mgtcgi_xml_system ** pxmlsystem)
{
	struct mgtcgi_xml_system * tmpPtr = NULL;
	if(processSystem() > 0)
		return RET_ERROR;
	//printf("pxmlsystem=%x , *pxmlsystem=%x\n" , pxmlsystem,*pxmlsystem);

	if(*pxmlsystem != NULL)
		return RET_ERROR;
	
	*pxmlsystem =(struct mgtcgi_xml_system*)
					AllocBuf(sizeof(struct mgtcgi_xml_system));
	if(NULL == *pxmlsystem){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlsystem;
	
	memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_system));
	strncpy(tmpPtr->hostname, system.getHostName().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->domainname, system.getDomainName().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->contact, system.getContact().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->location, system.getLocation().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->password, system.getPassword().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->password2, system.getPassword2().c_str(),STRING_LENGTH-1);
	tmpPtr->enable_stp = system.isSTPenable()?1:0;
	tmpPtr->auto_update = system.isAutoUpdateEnable()?1:0;
	strncpy(tmpPtr->update_url, system.getUpdateURL().c_str(),STRING_LENGTH-1);
	tmpPtr->ntp_enable = system.isNTPenable()?1:0;
	strncpy(tmpPtr->ntp_server, system.getNTPServer().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->timezone, system.getTimezone().c_str(),STRING_LENGTH-1);

	tmpPtr->bridge_mtu =  strtoint(system.getMTU().c_str());
	tmpPtr->dbridge_perf = system.isDBP()?1:0;
	tmpPtr->snmp_enable = system.isSNMPenable()?1:0;
	strncpy(tmpPtr->snmp_ro_community, system.getSNMPRO().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->snmp_rw_community, system.getSNMPRW().c_str(),STRING_LENGTH-1);

	return sizeof(struct mgtcgi_xml_system);
}


int Parser::GetStructManagerment(struct mgtcgi_xml_management ** pxmlmgt)
{
	struct mgtcgi_xml_management * tmpPtr = NULL;
	if(processManagement() > 0)
		return RET_ERROR;

	if(*pxmlmgt != NULL)
		return RET_ERROR;
	
	*pxmlmgt =(struct mgtcgi_xml_management*)
				AllocBuf(sizeof(struct mgtcgi_xml_management));
	if(NULL == *pxmlmgt){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlmgt;

	memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_management));
	strncpy(tmpPtr->ip , management.getIP().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->netmask , management.getNetmask().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->broadcast , management.getBroadcast().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->gateway , management.getGateway().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->server , management.getServer().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->dns_primary , management.getDNSPrimary().c_str(),STRING_LENGTH-1);
	strncpy(tmpPtr->dns_secondary , management.getDNSSecondary().c_str(),STRING_LENGTH-1);

	return sizeof(struct mgtcgi_xml_management);
}

int Parser::GetStructAuth(struct mgtcgi_xml_auth ** pxmlauth)
{	
	struct mgtcgi_xml_auth * tmpPtr = NULL;
	if(processAuth() > 0)
		return RET_ERROR;

	if(*pxmlauth != NULL)
		return RET_ERROR;
	
	*pxmlauth = (struct mgtcgi_xml_auth*)
				AllocBuf(sizeof(struct mgtcgi_xml_auth));
	if(NULL == *pxmlauth){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlauth;	

	memset(tmpPtr , 0 ,  sizeof(struct mgtcgi_xml_auth));
	tmpPtr->enable = auth.getEnable()?1:0;
	tmpPtr->timeout_mode = auth.getTimeoutMode();
	tmpPtr->timeout_seconds = auth.getTimeoutSeconds();
	tmpPtr->enable_multi_user = auth.getEnableMutilUser()?1:0;
	tmpPtr->redirect_http_request = auth.getRedirectHTTPRequest()?1:0;
	strncpy(tmpPtr->redirect_http_url, auth.getRedirectHTTPURL().c_str(),URL_LENGTH-1);
	strncpy(tmpPtr->redirect_adv_url, auth.getDenyGuestAccess().c_str(),URL_LENGTH-1);

	return sizeof(struct mgtcgi_xml_auth);
}

int Parser::GetStructInterface(struct mgtcgi_xml_interfaces ** pxmlinterface)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_interfaces * tmpPtr = NULL;
	if(processInterface() > 0)
		return RET_ERROR;

	if(*pxmlinterface != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_interfaces) + 
			interfacemgr.dataNodeCount * 
			sizeof(struct mgtcgi_xml_interface_include_info);
	
	*pxmlinterface =(struct mgtcgi_xml_interfaces*)AllocBuf(caclSize);
	if(NULL == *pxmlinterface){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlinterface;
	memset( tmpPtr , 0 ,  caclSize);			
	
	tmpPtr->num = interfacemgr.dataNodeCount; 
	lst = interfacemgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		Interface* item = (Interface *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mode , item->getMode().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].speed = strtoint(item->getSpeed());
		tmpPtr->pinfo[iIndex].linkstate = strtoint(item->getlinkstate());
		tmpPtr->pinfo[iIndex].bridgeid = strtoint(item->getbridgeid());
		strncpy(tmpPtr->pinfo[iIndex].duplex, item->getDuplex().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}
int Parser::GetStructVlans(struct mgtcgi_xml_vlans ** pxmlvlans)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_vlans * tmpPtr = NULL;
	if(processVlan() > 0)
		return RET_ERROR;

	if(*pxmlvlans != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_vlans) + 
			vlanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_vlan_info);
	
	*pxmlvlans =(struct mgtcgi_xml_vlans*)AllocBuf(caclSize);
	if(NULL == *pxmlvlans){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlvlans;
	memset( tmpPtr , 0 ,  caclSize);		
	
	tmpPtr->num = vlanmgr.dataNodeCount;
	lst = vlanmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		Vlan* item = (Vlan *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].vlans = strtoint(item->getVlans());
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}


/*2012-09-26 add getstructport 
int Parser::GetStructPortScan(struct mgtcgi_xml_port_scan ** pxmlportscan)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_port_scan * tmpPtr = NULL;
	if(processPortScan() > 0)
		return RET_ERROR;

	if(*pxmlportscan != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_port_scan) + 
			portscanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_port_scan_info);
	
	*pxmlportscan =(struct mgtcgi_xml_port_scan*)AllocBuf(caclSize);
	if(NULL == *pxmlportscan){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlportscan;
	memset( tmpPtr , 0 ,  caclSize);		
	
	tmpPtr->num = portscanmgr.dataNodeCount;
	lst = portscanmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		PortScan* item = (PortScan *) lst.at(i);
		
			
		
		strncpy(tmpPtr->pinfo[iIndex].type , item->getType().c_str(),STRING_LENGTH-1);
	 
		strncpy(tmpPtr->pinfo[iIndex].newer, item->getNewer().c_str(),STRING_LENGTH-1);		
        strncpy(tmpPtr->pinfo[iIndex].pending, item->getPending().c_str(),STRING_LENGTH-1);     
        strncpy(tmpPtr->pinfo[iIndex].interval, item->getInterval().c_str(),STRING_LENGTH-1);		
		++iIndex;
	}

	return caclSize;
}


*/


int Parser::GetStructNetworks(struct mgtcgi_xml_networks ** pxmlnetwork)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_networks * tmpPtr = NULL;
	if(processNetworkAddr() > 0)
		return RET_ERROR;

	if(*pxmlnetwork != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_networks) + 
			netaddrmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_network_info);
	
	*pxmlnetwork = (struct mgtcgi_xml_networks*)AllocBuf(caclSize);
	if(NULL == *pxmlnetwork){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlnetwork;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->num = netaddrmgr.dataNodeCount;
	lst = netaddrmgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		NetAddr* item = (NetAddr *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].value, item->getValue().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].range, item->getRangeString().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}
	return caclSize;
}

int Parser::GetStructScheduler(struct mgtcgi_xml_schedules ** pxmlscheduler )
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_schedules * tmpPtr = NULL;
	if(processXmllibSchedule() > 0)
		return RET_ERROR;

	if(*pxmlscheduler != NULL)
		return RET_ERROR;
		
	caclSize = sizeof(struct mgtcgi_xml_schedules) + 
			xmllibschedulemgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_schedule_info);
	
	*pxmlscheduler = (struct mgtcgi_xml_schedules*)AllocBuf(caclSize);
	if(NULL == *pxmlscheduler){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlscheduler;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = xmllibschedulemgr.dataNodeCount;	
	lst = xmllibschedulemgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		Schedule* item = (Schedule *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].start_time, item->getStartTime().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].stop_time, item->getStopTime().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].days, item->getDays().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}

      /* 2012-10-07 change */
int Parser::GetStructPortscan(struct mgtcgi_xml_portscan ** pxmlportscandata)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_portscan * tmpPtr = NULL;
    
    if(processPortScan() > 0)
		return RET_ERROR;
    
	if(*pxmlportscandata != NULL)
		return RET_ERROR;
    
	caclSize = sizeof(struct mgtcgi_xml_portscan) + 
			portscanmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_portscan_info);
	
	*pxmlportscandata = (struct mgtcgi_xml_portscan*)AllocBuf(caclSize);
	if(NULL == *pxmlportscandata){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlportscandata;
	memset( tmpPtr , 0 ,  caclSize);
	
	tmpPtr->num = portscanmgr.dataNodeCount;
	lst = portscanmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		PortScan* item = (PortScan *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].type , item->getType().c_str(),GET_STRING_LENG);
		strncpy(tmpPtr->pinfo[iIndex].newer, item->getNewer().c_str(),GET_STRING_LENG);
        strncpy(tmpPtr->pinfo[iIndex].pending, item->getPending().c_str(),GET_STRING_LENG);
        strncpy(tmpPtr->pinfo[iIndex].interval, item->getInterval().c_str(),GET_STRING_LENG);				
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructNetworkDetection(struct mgtcgi_xml_networkdetection **pxmlNetworkDetection)
{
	int calsize = 0;
	struct mgtcgi_xml_networkdetection *tempPtr = NULL;

	if(processPing() != 0)
	{
		return RET_ERROR;
	}

	calsize = sizeof(struct mgtcgi_xml_networkdetection);
	*pxmlNetworkDetection = (struct mgtcgi_xml_networkdetection *)AllocBuf(calsize);
	if(NULL == *pxmlNetworkDetection){
		return RET_ERROR;
	}	
	tempPtr = *pxmlNetworkDetection;
	memset( tempPtr , 0 ,  calsize);	

	tempPtr->zeropingenable = networkdetectionmgr.getZeroPingValue();

	return calsize;
}

int Parser::GetStructFirstGameChannel(struct mgtcgi_xml_first_game_channels **pxmlfirstgamechannel)
{
	int calsize = 0, index = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_first_game_channels *tempPtr = NULL;

	if(processFirstGameChannel() != 0)
	{
		return RET_ERROR;
	}

	calsize = sizeof(struct mgtcgi_xml_first_game_channels) + firstgcmgr.dataNodeCount * sizeof(struct mgtcgi_xml_first_game_channel_info);
	*pxmlfirstgamechannel = (struct mgtcgi_xml_first_game_channels *)AllocBuf(calsize);
	if(NULL == *pxmlfirstgamechannel){
		return RET_ERROR;
	}	
	tempPtr = *pxmlfirstgamechannel;
	memset( tempPtr , 0 ,  calsize);	

	tempPtr->num = firstgcmgr.dataNodeCount;
	lst = firstgcmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		Firstgamechannel *item  = (Firstgamechannel *)lst.at(i);
		tempPtr->pinfo[index].id = item->getchannelId();
		strncpy(tempPtr->pinfo[index].name, item->getChannelName().c_str(), STRING_LENGTH - 1);
		++index;
	}

	return calsize;
}

int Parser::GetStructRouteServerInfo(struct mgtcgi_xml_routeserver **pxmlrouteserverinfo)
{
	int calsize = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_routeserver *tempPtr = NULL;

	if(processRouteserverinfo() != 0)
	{
		return RET_ERROR;
	}

	calsize = sizeof(struct mgtcgi_xml_routeserver);
	*pxmlrouteserverinfo = (struct mgtcgi_xml_routeserver *)AllocBuf(calsize);
	if(NULL == *pxmlrouteserverinfo){
		return RET_ERROR;
	}	
	tempPtr = *pxmlrouteserverinfo;
	memset( tempPtr , 0 ,  calsize);	

	tempPtr->enable = routerserver.getenable()?1:0;
	tempPtr->passwd_enable = routerserver.getpasswdenable()?1:0;
	
	return calsize;
}

int Parser::GetStructServInfo(struct mgtcgi_xml_servinfo **pxmlservinfodata)
{
    int caclSize= 0;
	int index = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_servinfo * tmpPtr = NULL;
    
    if(processServInfo() > 0)
		return RET_ERROR;
    
	if(*pxmlservinfodata != NULL)
		return RET_ERROR;
    
	caclSize = sizeof(struct mgtcgi_xml_servinfo) + 
			servinfomgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_servinfo_info);
	
	*pxmlservinfodata = (struct mgtcgi_xml_servinfo*)AllocBuf(caclSize);
	if(NULL == *pxmlservinfodata){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlservinfodata;
	memset( tmpPtr, 0, caclSize);
	
	tmpPtr->num = servinfomgr.dataNodeCount;
	lst = servinfomgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++)
    {
		ServInfo* item = (ServInfo *) lst.at(i);
        
		if(item->isGroup())
        {
			continue;
		}	
				
		tmpPtr->pinfo[index].type = strtoint(item->getType());
		strncpy(tmpPtr->pinfo[index].name, item->getTypeName().c_str(), STRING_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].en_name, item->getEnTypeName().c_str(), STRING_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].zonename, item->getZoneName().c_str(), STRING_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].en_zonename, item->getEnZoneName().c_str(), STRING_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].var1, item->getVar1().c_str(), URL_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].var2, item->getVar2().c_str(), URL_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].var3, item->getVar3().c_str(), URL_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].var4, item->getVar4().c_str(), URL_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].comment, item->getComment().c_str(), STRING_LENGTH - 1);
        
		++index;
	}

	return caclSize;
}

//Add by chenhao,2015-1-23 20:23:18
int Parser::GetStructWifiConf(struct mgtcgi_xml_wifis **pxmlwifi)
{
      int caclSize= 0;
      int index = 0;
      std::vector<Wifi *> pools;
	
      struct mgtcgi_xml_wifis * tmpPtr = NULL;
    
      if(processWifiInfo() > 0)
		return RET_ERROR;
    
	if(*pxmlwifi != NULL)
		return RET_ERROR;
    
	caclSize = sizeof(struct mgtcgi_xml_wifis) + 
			wifimgr.getPoolCount() * sizeof(struct mgtcgi_xml_wifi_address_pool);
	
	*pxmlwifi = (struct mgtcgi_xml_wifis*)AllocBuf(caclSize);
	if(NULL == *pxmlwifi){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlwifi;
	memset( tmpPtr, 0, caclSize);

	pools = wifimgr.getPoolList();
	
	tmpPtr->enable = wifimgr.getWifiEnable()?1:0;
	tmpPtr->pc_discover_time = strtoint(wifimgr.getPcTime());
	tmpPtr->max_time = strtoint(wifimgr.getMaxTime());
	tmpPtr->share_num = strtoint(wifimgr.getShareNum());
	tmpPtr->pool_num = wifimgr.getPoolCount();
	strncpy(tmpPtr->admins,wifimgr.getAdmins().c_str(),LONG_STRING_LENGTH);

	index = 0;
	for(unsigned int j = 0; j < pools.size(); j++)
	{
		Wifi *pool = pools.at(j);
		if(pool->isGroup()){
			continue;
		}
		
		tmpPtr->pinfo[index].enable = pool->getWifiPoolEnable()?1:0;
		tmpPtr->pinfo[index].id = strtoint(pool->getWifiPoolId());
		strncpy(tmpPtr->pinfo[index].startip, pool->getWifiPoolStartip().c_str(),STRING_LENGTH);
		strncpy(tmpPtr->pinfo[index].endip, pool->getWifiPoolEndip().c_str(), STRING_LENGTH);
		strncpy(tmpPtr->pinfo[index].comment, pool->getWifiPoolComment().c_str(), STRING_LENGTH);
		index++;
	}
	
	return caclSize;
}

int Parser::GetStructServConf(struct mgtcgi_xml_servconf **pxmlservconfdata)
{
    int caclSize= 0;
	int index = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_servconf * tmpPtr = NULL;
    
    if (processServConf() > 0)
		return RET_ERROR;
    
	if (*pxmlservconfdata != NULL)
		return RET_ERROR;
    
	caclSize = sizeof(struct mgtcgi_xml_servconf) + 
			servconfmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_servconf_info);
	
	*pxmlservconfdata = (struct mgtcgi_xml_servconf*)AllocBuf(caclSize);
	if(NULL == *pxmlservconfdata)
    {
		return RET_ERROR;
	}	
	tmpPtr = *pxmlservconfdata;
	memset( tmpPtr, 0, caclSize);
	
	tmpPtr->num = servconfmgr.dataNodeCount;
	lst = servconfmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++)
    {
		ServConf* item = (ServConf *) lst.at(i);

		if(item->isGroup())
        {
			continue;
		}	
		
		tmpPtr->pinfo[index].type = strtoint(item->getType());
		strncpy(tmpPtr->pinfo[index].en_name, item->getEnTypeName().c_str(),STRING_LENGTH - 1);
        strncpy(tmpPtr->pinfo[index].en_zonename, item->getEnZoneName().c_str(),STRING_LENGTH - 1);
		++index;
	}

	return caclSize;
}

int Parser::GetStructTimingAdsl(struct mgtcgi_xml_adsl_timings **pxmltimingadsl)
{
    	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_adsl_timings * tmpPtr = NULL;
	if(processTiming() > 0)
		return RET_ERROR;

	if(*pxmltimingadsl != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_adsl_timings) + 
			timingmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_adsl_timing_redial);

	*pxmltimingadsl = (struct mgtcgi_xml_adsl_timings*)AllocBuf(caclSize);
	if(NULL == *pxmltimingadsl){
		return RET_ERROR;
	}	
	tmpPtr = *pxmltimingadsl;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num= timingmgr.dataNodeCount;
	lst = timingmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		Timing * item = (Timing *) lst.at(i);
		if(item->isGroup()){
			continue;
		}
		strncpy(tmpPtr->pinfo[iIndex].name, item->getName().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].iface, item->getIface().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].minute, item->getMinute().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].hour, item->getHour().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].day, item->getDay().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].month, item->getMonth().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].week, item->getWeek().c_str(), STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(), STRING_LENGTH-1);
		++iIndex;
	}
	
	return caclSize;
}

int Parser::GetStructDnsPolicy(struct mgtcgi_xml_dns_policys ** pxmldnspolicy)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_dns_policys * tmpPtr = NULL;
	if(processDnsHelper(false) > 0)
		return RET_ERROR;

	if(*pxmldnspolicy != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_dns_policys) + 
			dnspolicymgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_dns_policy_info);

	*pxmldnspolicy = (struct mgtcgi_xml_dns_policys*)AllocBuf(caclSize);
	if(NULL == *pxmldnspolicy){
		return RET_ERROR;
	}	
	tmpPtr = *pxmldnspolicy;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = dnspolicymgr.dataNodeCount;
	tmpPtr->application_policy= dnspolicymgr.getEnable()?1:0;
	lst = dnspolicymgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		DnsPolicy* item = (DnsPolicy *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}		
		
		tmpPtr->pinfo[iIndex].name = strtoint(item->getRawName());
		strncpy(tmpPtr->pinfo[iIndex].domain_name, item->getDomainName().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].bridge = item->getBridgeId();
		strncpy(tmpPtr->pinfo[iIndex].src, item->getSrc().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dst, item->getDst().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].action = item->getAction();
		strncpy(tmpPtr->pinfo[iIndex].targetip, item->getTargetip().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		tmpPtr->pinfo[iIndex].disabled = item->getDisabled();
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructExtensions(struct mgtcgi_xml_extensions ** pxmlextensions)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_extensions * tmpPtr = NULL;
	if(processHttpDirPolicy(false) > 0)
		return RET_ERROR;

	if(*pxmlextensions != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_extensions) + 
			extensionmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_extension_info);

	*pxmlextensions = (struct mgtcgi_xml_extensions*)AllocBuf(caclSize);
	if(NULL == *pxmlextensions){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlextensions;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = extensionmgr.dataNodeCount;
	lst = extensionmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		ExtensionNode* item = (ExtensionNode *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].value, item->getValue().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}
	return caclSize;
}

int Parser::GetStructHttpDirPolicy(struct mgtcgi_xml_http_dir_policys ** pxmlhttpdirpolicy)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;

	struct mgtcgi_xml_http_dir_policys * tmpPtr = NULL;
	if(processHttpDirPolicy(false) > 0)
		return RET_ERROR;

	if(*pxmlhttpdirpolicy != NULL)
		return RET_ERROR;

	caclSize = sizeof(struct mgtcgi_xml_http_dir_policys) + 
			urlformatmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
			httpdirpolicymgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_http_dir_policy_info);

	*pxmlhttpdirpolicy = (struct mgtcgi_xml_http_dir_policys*)AllocBuf(caclSize);
	if(NULL == *pxmlhttpdirpolicy){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlhttpdirpolicy;
	memset( (char*)tmpPtr , 0 ,  caclSize);
	tmpPtr->p_urlinfo=(struct mgtcgi_xml_http_dir_policy_urlformat_info *)
					( (char*)tmpPtr+ sizeof(struct mgtcgi_xml_http_dir_policys));
	tmpPtr->p_policyinfo= (struct mgtcgi_xml_http_dir_policy_info *)
					( (char*)tmpPtr+ sizeof(struct mgtcgi_xml_http_dir_policys) +			
					urlformatmgr.dataNodeCount * 
						sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));
	tmpPtr->format_num = urlformatmgr.dataNodeCount;
	tmpPtr->policy_num = httpdirpolicymgr.dataNodeCount;

	lst = urlformatmgr.getList();
	iIndex = 0;
	for(unsigned int x = 0; x < lst.size(); x++){
		UrlFormatNode* item = (UrlFormatNode *) lst.at(x);
		
		if(item->isGroup()){
			continue;
		}	
		tmpPtr->p_urlinfo[iIndex].bridge = item->getBridgeId();
		strncpy(tmpPtr->p_urlinfo[iIndex].format , item->getFormat().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->p_urlinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}
	
	lst = httpdirpolicymgr.getList();
	iIndex = 0;
	for(unsigned int i = 0; i < lst.size(); i++){
		HttpDirPolicy* item = (HttpDirPolicy *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
	
		tmpPtr->p_policyinfo[iIndex].bridge = item->getBridgeId();
		strncpy(tmpPtr->p_policyinfo[iIndex].extgroup , item->getExtgroup().c_str(),STRING_LENGTH-1);
		tmpPtr->p_policyinfo[iIndex].name = strtoint(item->getName());
		strncpy(tmpPtr->p_policyinfo[iIndex].servergroup , item->getServergroup().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->p_policyinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);						
		tmpPtr->p_policyinfo[iIndex].disabled = item->getDisabled();
		++iIndex;
	}

	return caclSize;

}

int Parser::GetStructTraffic(struct mgtcgi_xml_traffics ** pxmltraffic)
{	
	const int UPLOAD = 0;	//上传标识
	const int DOWNLOAD = 1;//下载标识
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_traffics * tmpPtr = NULL;
	if(processTraffic() > 0)
		return RET_ERROR;

	if(*pxmltraffic != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_traffics) + 
			trafficmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_traffic_channel_info);
	
	*pxmltraffic =(struct mgtcgi_xml_traffics*)AllocBuf(caclSize);
	if(NULL == *pxmltraffic){
		return RET_ERROR;
	}	
	tmpPtr = *pxmltraffic;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = trafficmgr.dataNodeCount;
	tmpPtr->status = trafficmgr.getEnable()?1:0;
	lst = trafficmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		TrafficChannel* item = (TrafficChannel *) lst.at(i);

		if(item->isGroup()){
			continue;
		}	
		tmpPtr->pinfo[iIndex].direct = 
					item->getInterface().find("EXT")!=std::string::npos?UPLOAD:DOWNLOAD;
		strncpy(tmpPtr->pinfo[iIndex].classname , item->getClassName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].parentname, item->getParentName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].sharename, item->getShareName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].share_comment, item->getShareComment().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].classtype = item->getClassType();
		strncpy(tmpPtr->pinfo[iIndex].rate, item->getRateString().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].priority = item->getPriority();
		tmpPtr->pinfo[iIndex].limit = item->getLimit();
		tmpPtr->pinfo[iIndex].default_channel = item->isDefault() ? 1: 0;
		tmpPtr->pinfo[iIndex].attribute = item->getAttribute();
		tmpPtr->pinfo[iIndex].weight = item->getWeight();
		tmpPtr->pinfo[iIndex].bridge = strtoint(item->getInterface().substr(3,4));
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructFirewallPolicy(struct mgtcgi_xml_firewalls_policy ** pxmlfirewallpolicy)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
		
	struct mgtcgi_xml_firewalls_policy * tmpPtr = NULL;
	if(processFirewallPolicy() > 0)
		return RET_ERROR;

	if(*pxmlfirewallpolicy != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_firewalls_policy) + 
			firewallpolicymgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_firewall_policy_info);
	
	*pxmlfirewallpolicy = (struct mgtcgi_xml_firewalls_policy*)AllocBuf(caclSize);
	if(NULL == *pxmlfirewallpolicy){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlfirewallpolicy;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->num = firewallpolicymgr.dataNodeCount;
	tmpPtr->enable = (firewallpolicymgr.getFireWallEnable()?1:0);
	lst = firewallpolicymgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		FirewallPolicy* item = (FirewallPolicy *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	

		tmpPtr->pinfo[iIndex].id = strtoint(item->getId());
		tmpPtr->pinfo[iIndex].enable = (item->getEnable()?1:0);
		strncpy(tmpPtr->pinfo[iIndex].src, item->getSrc().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dst, item->getDst().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].isp, item->getIsp().c_str(),DIGITAL_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].proto, item->getProto().c_str(),STRING_LENGTH-1);	
		strncpy(tmpPtr->pinfo[iIndex].session, item->getSession().c_str(),STRING_LENGTH-1);	
		strncpy(tmpPtr->pinfo[iIndex].iface, item->getIface().c_str(),LONG_STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].action, item->getAction().c_str(),LITTER_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].timed, item->getTimed().c_str(),LITTER_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].week, item->getWeek().c_str(),STRING_LENGTH-1);	
		strncpy(tmpPtr->pinfo[iIndex].day, item->getDay().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].in_limits, item->getInLimit().c_str(),LITTER_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].out_limits, item->getOutLimit().c_str(),LITTER_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),COMMENT_LENGTH-1);	
		++iIndex;
	}
	return caclSize;
}

int Parser::GetStructFirewalls(struct mgtcgi_xml_firewalls ** pxmlfirewalls)
{	
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_firewalls * tmpPtr = NULL;
	if(processFirewall(false) > 0)
		return RET_ERROR;

	if(*pxmlfirewalls != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_firewalls) + 
			firewallmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_firewall_info);
	
	*pxmlfirewalls = (struct mgtcgi_xml_firewalls*)AllocBuf(caclSize);
	if(NULL == *pxmlfirewalls){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlfirewalls;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = firewallmgr.dataNodeCount;
	tmpPtr->application_firewall = firewallmgr.getEnable()?1:0;	
	tmpPtr->scan_detect = firewallmgr.getScanDetect()?1:0;
	tmpPtr->dos_detect = firewallmgr.getDosDetect()?1:0;
	lst = firewallmgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		Firewall* item = (Firewall *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	

		tmpPtr->pinfo[iIndex].name = strtoint(item->getName());
		strncpy(tmpPtr->pinfo[iIndex].proto , item->getProto().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].auth , item->getAuth().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].action , item->getAction().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].schedule , item->getSchedule().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].session_limit , item->getSessionLimit().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].vlan , item->getVlan().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].src , item->getSrc().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dst , item->getDst().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].mac , item->getMac().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].in_traffic , item->getInTraffic().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].out_traffic , item->getOutTraffic().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].quota , item->getQuota().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].quota_action = strtoint(item->getQuotaAction());
		strncpy(tmpPtr->pinfo[iIndex].second_in_traffic , item->getSencondInTraffic().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].second_out_traffic , item->getSencondOutTraffic().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].log = item->getLogPacket()?1:0;
		tmpPtr->pinfo[iIndex].disabled = item->getDisabled();
		tmpPtr->pinfo[iIndex].bridge = item->getBridgeId();
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructIPSessionLimit(struct mgtcgi_xml_ip_session_limits ** pxmlipsessionlimit)
{
	int caclSize= 0;
	int iIndex = 0;
	int bridgeCount = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_ip_session_limits * tmpPtr = NULL;
	if(processGlobalIP() > 0)
		return RET_ERROR;
	if(*pxmlipsessionlimit != NULL)
		return RET_ERROR;

	bridgeCount = globalipmgr.getMaxBridgeID()+1;
	caclSize = sizeof(struct mgtcgi_xml_ip_session_limits) + 
			bridgeCount * 
				sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
			globalipmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_ip_session_limit_global_info);
	
	*pxmlipsessionlimit = (struct mgtcgi_xml_ip_session_limits*)AllocBuf(caclSize);
	if(NULL == *pxmlipsessionlimit){
		return RET_ERROR;
	}

	tmpPtr = *pxmlipsessionlimit;
	memset( tmpPtr , 0 ,  caclSize);	
	tmpPtr->p_totalinfo =(struct mgtcgi_xml_ip_session_limit_total_info *)
					( (char*)tmpPtr+ sizeof(struct mgtcgi_xml_ip_session_limits));
	tmpPtr->p_globalinfo = (struct mgtcgi_xml_ip_session_limit_global_info *)
					( (char*)tmpPtr+ sizeof(struct mgtcgi_xml_ip_session_limits) +
			(bridgeCount) * 
				sizeof(struct mgtcgi_xml_ip_session_limit_total_info));

#if 0
	printf("bridgeCount = %d , caclSize=%d ,dataNodeCount=%d\n" , 
		bridgeCount,caclSize,globalipmgr.dataNodeCount);
	printf("sizeof(struct mgtcgi_xml_ip_session_limits) = %d \n" , 
		sizeof(struct mgtcgi_xml_ip_session_limits),caclSize);
	printf("sizeof(struct mgtcgi_xml_ip_session_limit_total_info) = %d \n" , sizeof(struct mgtcgi_xml_ip_session_limit_total_info));
	printf("sizeof(struct mgtcgi_xml_ip_session_limit_global_info) = %d \n" , sizeof(struct mgtcgi_xml_ip_session_limit_global_info));
	
	printf("tmpPtr =%d ,tmpPtr->p_totalinfo = %d , tmpPtr->p_globalinfo=%d\n" , 
		(char*)tmpPtr , (char*)(tmpPtr->p_totalinfo) ,
		(char*)(tmpPtr->p_globalinfo));
	printf("tmpPtr->p_totalinfo = %d , tmpPtr->p_globalinfo=%d\n" , 
		(char*)(tmpPtr->p_totalinfo)- (char*)tmpPtr,
		(char*)(tmpPtr->p_globalinfo) - (char*)(tmpPtr->p_totalinfo));
#endif
	
	lst = globalipmgr.getList();
		
	tmpPtr->total_limit_num = bridgeCount;
	for(int x = 0; x < bridgeCount; ++x){
		tmpPtr->p_totalinfo[x].bridge = x ;
		tmpPtr->p_totalinfo[x].limit= globalipmgr.getTotalSessionLimitNum(x);		
		strncpy(tmpPtr->p_totalinfo[x].overhead , 
				globalipmgr.getTotalSessionLimitOverheadString(x).c_str(),
				STRING_LENGTH-1);
	}
	
	tmpPtr->global_limit_num = globalipmgr.dataNodeCount;
	for(unsigned int i = 0; i < lst.size(); i++){
		GlobalIP* item = (GlobalIP *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		tmpPtr->p_globalinfo[iIndex].name = strtoint(item->getName());
		strncpy(tmpPtr->p_globalinfo[iIndex].addr , item->getAddr().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->p_globalinfo[iIndex].dst , item->getDst().c_str(),STRING_LENGTH-1);

		strncpy(tmpPtr->p_globalinfo[iIndex].per_ip_session_limit , item->getPerIPSessionLimit().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->p_globalinfo[iIndex].total_session_limit , item->getTotalSessionLimit().c_str(),STRING_LENGTH-1);
		tmpPtr->p_globalinfo[iIndex].httplog = item->getEnableHTTPLog()?1:0;
		tmpPtr->p_globalinfo[iIndex].sesslog = item->getEnableSessLog()?1:0;
		tmpPtr->p_globalinfo[iIndex].httpdirpolicy = item->getEnableHTTPDirPolicy()?1:0;
		tmpPtr->p_globalinfo[iIndex].dnspolicy = item->getEnableDnsHelperPolicy()?1:0;
		strncpy(tmpPtr->p_globalinfo[iIndex].action , item->getAction().c_str(),STRING_LENGTH-1);
		tmpPtr->p_globalinfo[iIndex].bridge = item->getBridgeID();
		strncpy(tmpPtr->p_globalinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
		
	}

	return caclSize;
}
int Parser::GetStructPortmirrors(struct mgtcgi_xml_port_mirrors ** pxmlportmirror)
{	
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_port_mirrors * tmpPtr = NULL;
	if(processPortMirror(false) > 0)
		return RET_ERROR;

	if(*pxmlportmirror != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_port_mirrors) + 
			portmirrormgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_port_mirror_info);
	
	*pxmlportmirror = (struct mgtcgi_xml_port_mirrors*)AllocBuf(caclSize);
	if(NULL == *pxmlportmirror){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlportmirror;
	memset( tmpPtr , 0 ,  caclSize);	
	
	tmpPtr->num = portmirrormgr.dataNodeCount;
	lst = portmirrormgr.getList();

	for(int x = 0; x < (portmirrormgr.getMaxBridgeID()+1); ++x){
		tmpPtr->pstatus[x].bridge = x ;
		tmpPtr->pstatus[x].enable= portmirrormgr.getEnable(x);	
	}	

	for(unsigned int i = 0; i < lst.size(); i++){
		PortMirror* item = (PortMirror *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	

		tmpPtr->pinfo[iIndex].name = strtoint(item->getName().c_str());
		strncpy(tmpPtr->pinfo[iIndex].proto, item->getProto().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].action, item->getAction().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].src, item->getSrcAddr().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dst, item->getDstAddr().c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].disabled = item->getDisabled();
		tmpPtr->pinfo[iIndex].bridge = item->getBridgeID();
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructHttpserver(struct mgtcgi_xml_httpservers ** pxmlhttpserver )
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_httpservers * tmpPtr = NULL;
	if(processHttpDirPolicy(false) > 0)
		return RET_ERROR;

	if(*pxmlhttpserver != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_httpservers) + 
			httpservermgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_httpserver_info);
	
	*pxmlhttpserver = (struct mgtcgi_xml_httpservers*)AllocBuf(caclSize);
	if(NULL == *pxmlhttpserver){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlhttpserver;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->num = httpservermgr.dataNodeCount;
	lst = httpservermgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		HttpServerNode* item = (HttpServerNode *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	

		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].ip_addr, item->getIPAddr().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].servertype, item->getServerType().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].prefix, item->getPrefix().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].suffix, item->getSuffix().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructSessionlimit(struct mgtcgi_xml_session_limits ** pxmlsessionlimit)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_session_limits * tmpPtr = NULL;
	if(processSession() > 0)
		return RET_ERROR;

	if(*pxmlsessionlimit != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_session_limits) + 
			sessionmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_session_limit_info);
	
	*pxmlsessionlimit = (struct mgtcgi_xml_session_limits*)AllocBuf(caclSize);
	if(NULL == *pxmlsessionlimit){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlsessionlimit;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->num = sessionmgr.dataNodeCount;
	lst = sessionmgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		Session* item = (Session *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].value, item->getSession().c_str(),STRING_LENGTH-1);
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructProtocols(struct mgtcgi_xml_protocols** pxmlprotocol)
{	
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	struct mgtcgi_xml_protocols * tmpPtr = NULL;
	if(processCustomProtocol() > 0)
		return RET_ERROR;

	if(*pxmlprotocol != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_protocols) + 
			protocolmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_protocol_info);
	
	//printf("caclSize : %d ,dataNodeCount=%d \n",caclSize,protocolmgr.dataNodeCount);	
	*pxmlprotocol = (struct mgtcgi_xml_protocols*)AllocBuf(caclSize);
	if(NULL == *pxmlprotocol){
		return RET_ERROR;
	}	
	tmpPtr = *pxmlprotocol;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->num = protocolmgr.dataNodeCount;
	lst = protocolmgr.getList();
	
	for(unsigned int i = 0; i < lst.size(); i++){
		Protocol* item = (Protocol *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].proto, item->getProto().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].dport, item->getDPort().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].sport, item->getSPort().c_str(),STRING_LENGTH-1);
		
		if(item->getProto().compare("TCP") != 0 && item->getProto().compare("UDP") != 0){
			strncpy(tmpPtr->pinfo[iIndex].match, item->getMatch().c_str(),STRING_LENGTH-1);
		}
		strncpy(tmpPtr->pinfo[iIndex].option, item->getOption().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);

	//printf("iIndex : %d\n",iIndex);	
		++iIndex;
	}

	return caclSize;
}
#if 1
#include <string.h>
#include <stdio.h>
    
int get_cat_info(char * catinfo,U32 *gid, char * value, U8 * show)
{
    char *p;
	const char s[]="|";
    p = strtok(catinfo,s);
	*gid = atoi(p);
	p=strtok(NULL,s);
	*show = atoi(p);
	p=strtok(NULL,s);
	strncpy(value, p,STRING_LENGTH-1);;
	//catinfo.split("|");
	return 0;
}	
#endif
int Parser::GetStructLayer7Protocols(const char * lanaguage,struct mgtcgi_xml_layer7_protocols_show** pxmll7protocol)
{	
	int caclSize= 0;
	struct mgtcgi_xml_protocols* pxmlprotocol = NULL;
	int iIndex = 0;
	U32 gid = 0;
	U8 show = 0;
	std::vector<Node*> lst;
	std::map<std::string,std::string>::iterator iter;
	std::map<std::string,int>::iterator itemp;
	std::string type_name;
	unsigned int i = 0;
	char comment[265]={0};
	
	struct mgtcgi_xml_layer7_protocols_show * tmpPtr = NULL;
#if 1
	//获取自定义协议，将自定义协议信息保存到layer7�?
	if(GetStructProtocols(&pxmlprotocol) <= 0)
		return RET_ERROR;	
#endif
	protocolmgr.Clear();
	if(processLayer7Protocol(lanaguage) > 0)
		return RET_ERROR;
	if(*pxmll7protocol != NULL)
		return RET_ERROR;
		
	caclSize = sizeof(struct mgtcgi_xml_layer7_protocols_show) + 
			protocolmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_layer7_protocol_show_info) +
			pxmlprotocol->num * sizeof(struct mgtcgi_xml_layer7_protocol_show_info);
	
	*pxmll7protocol = (struct mgtcgi_xml_layer7_protocols_show*)AllocBuf(caclSize);
	if(NULL == *pxmll7protocol){
		return RET_ERROR;
	}
	tmpPtr = *pxmll7protocol;
	memset( tmpPtr , 0 ,  caclSize);
	
	tmpPtr->num = protocolmgr.dataNodeCount + pxmlprotocol->num;
	lst = protocolmgr.getList();	
	
	iIndex = 0;
#if 1	
	for(i =0; i < pxmlprotocol->num; ++i){	
		strncpy(tmpPtr->pinfo[iIndex].match , pxmlprotocol->pinfo[iIndex].name,STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].name , (pxmlprotocol->pinfo[iIndex].name),STRING_LENGTH-1);		
		strncpy(tmpPtr->pinfo[iIndex].type , "custom" ,STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].type_name , "自定义协� ",STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment , pxmlprotocol->pinfo[iIndex].comment,STRING_LENGTH-1);
		++ iIndex;
	}
	if(pxmlprotocol != NULL ){
		free(pxmlprotocol);
		pxmlprotocol = NULL;
	}	
#endif
	for(i = 0; i < lst.size(); i++){
		Protocol* item = (Protocol *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}		
		tmpPtr->pinfo[iIndex].id = item->getId();
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].match, item->getMatch().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].type, item->getType().c_str(),STRING_LENGTH-1);
	
		iter = mapLayer7prototype.find(item->getType());
		type_name = iter->first;
		memset(comment,0,COMMENT_LENGTH);
		strncpy(comment,iter->second.c_str(),COMMENT_LENGTH-1);
		get_cat_info((char *)comment,&gid,tmpPtr->pinfo[iIndex].type_name,&show);
		
#if 0		
		if (iter != mapLayer7prototype.end()){
			gid = itemp->second;
			type_name = iter->third;
		}
		else{
			gid=0;
			type_name = item->getType();
		}
#endif		
		//strncpy(tmpPtr->pinfo[iIndex].type_name, type_name.c_str(),STRING_LENGTH-1);
		tmpPtr->pinfo[iIndex].gid = gid;
		tmpPtr->pinfo[iIndex].show = show;
		
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),COMMENT_LENGTH-1);
		++iIndex;
	}

	return caclSize;
}

int Parser::GetStructLayer7ProtocolType(const char * lanaguage,struct mgtcgi_xml_layer7_protocols_show** pxmll7protocolType){
	int caclSize = 0;
	int type_num = 0,index = 0;
	std::map<std::string,std::string>::iterator it;
	std::map<std::string,int>::iterator tmp;
	//std::string value;
	
	struct mgtcgi_xml_layer7_protocols_show * tmpPtr = NULL;
    char comment[256];
	
	if(*pxmll7protocolType != NULL)
		return RET_ERROR;

	if(processLayer7Protocol(lanaguage) > 0)
		return RET_ERROR;

	type_num = mapLayer7prototype.size();
	caclSize = sizeof(struct mgtcgi_xml_layer7_protocols_show) + 
			type_num * sizeof(struct mgtcgi_xml_layer7_protocol_show_info);

	*pxmll7protocolType = (struct mgtcgi_xml_layer7_protocols_show*)AllocBuf(caclSize);
	if(NULL == *pxmll7protocolType){
		return RET_ERROR;
	}
	tmpPtr = *pxmll7protocolType;
	memset( tmpPtr , 0 ,  caclSize);

	tmpPtr->num = type_num;
	for(it = mapLayer7prototype.begin(); it != mapLayer7prototype.end(); it++){
		//tmpPtr->pinfo[index].id = 0;
		memset(comment,0,COMMENT_LENGTH);
		strncpy(comment,it->second.c_str(),COMMENT_LENGTH-1);
		get_cat_info((char *)comment,&tmpPtr->pinfo[index].gid,tmpPtr->pinfo[index].match,&tmpPtr->pinfo[index].show);

		// = it->second;
		//value=it->second.c_str()
		strncpy(tmpPtr->pinfo[index].name, it->first.c_str(),STRING_LENGTH-1);
		//strncpy(tmpPtr->pinfo[index].match, it->third.c_str(),STRING_LENGTH-1);
		//tmpPtr->pinfo[index].selected = it->fourth;
		tmpPtr->pinfo[index].show = 1;
		index++;
	}
	return caclSize;
}

int Parser::GetStructLogger(struct mgtcgi_xml_loggers ** pxmllogger)
{
	int caclSize= 0;
	struct mgtcgi_xml_loggers * tmpPtr = NULL;
	
	if(processLogger() > 0)
		return RET_ERROR;

	if(*pxmllogger != NULL)
		return RET_ERROR;
	
	caclSize = sizeof(struct mgtcgi_xml_loggers);
	
	*pxmllogger = (struct mgtcgi_xml_loggers*)AllocBuf(caclSize);
	if(NULL == *pxmllogger){
		return RET_ERROR;
	}	
	tmpPtr = *pxmllogger;
	memset( tmpPtr , 0 ,  caclSize);		

	tmpPtr->block_log_enable = logger.isBlockLog() ? 1:0;
	tmpPtr->http_log_enable = logger.isHTTPLog() ? 1:0;
	tmpPtr->session_log_enable = logger.isSessionLog() ? 1:0;
	tmpPtr->scan_detect_log_enable = logger.isScanDetectLog() ? 1:0;
	tmpPtr->dos_detect_log_enable = logger.isDoSDetectLog() ? 1:0;

	return caclSize;
}

#define XML_GROUP_DEBUG 0
int Parser::GetStructXmlGroups(struct mgtcgi_xml_group_array ** pxmlgroup,
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
		caclSize,nodemgr.groupNodeCount,nodemgr.includeNodeCount );
#endif	

	*pxmlgroup =(struct mgtcgi_xml_group_array*)AllocBuf(caclSize);
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
		iInclude += subnodelst.size();
	
		pTempInclude = tmpPtr->pinfo[iIndex].pinfo;
		
#if XML_GROUP_DEBUG
		printf("pIncludeStart=%x iInclude= %d pTempInclude=%x\n",pIncludeStart,iInclude,pTempInclude);
	    printf("group_name=%s ,include Count=%d\n",tmpPtr->pinfo[iIndex].group_name,tmpPtr->pinfo[iIndex].num);
#endif
			
		for(unsigned int k = 0; k < subnodelst.size(); k++){
			strncpy(pTempInclude[k].name , subnodelst.at(k).c_str(),STRING_LENGTH-1);/**/
		}
		
		++iIndex;
	}
	
	return caclSize;
}


int Parser::GetStructDomainData(struct mgtcgi_xml_domains ** pxmldomaindata)
{
	int caclSize= 0;
	int iIndex = 0;
	std::vector<Node*> lst;
	
	struct mgtcgi_xml_domains * tmpPtr = NULL;
	if(processDnsHelper(false) > 0)
		return RET_ERROR;

	if(*pxmldomaindata != NULL)
		return RET_ERROR;
	caclSize = sizeof(struct mgtcgi_xml_domains) + 
			domainmgr.dataNodeCount * 
				sizeof(struct mgtcgi_xml_domain_info);
	
	*pxmldomaindata = (struct mgtcgi_xml_domains*)AllocBuf(caclSize);
	if(NULL == *pxmldomaindata){
		return RET_ERROR;
	}	
	tmpPtr = *pxmldomaindata;
	memset( tmpPtr , 0 ,  caclSize);	
//	printf("caclSize=%d ; domainmgr.dataNodeCount=%d\n",caclSize,domainmgr.dataNodeCount);
	
	tmpPtr->num = domainmgr.dataNodeCount;
	lst = domainmgr.getList();

	for(unsigned int i = 0; i < lst.size(); i++){
		DomainNode* item = (DomainNode *) lst.at(i);
		
		if(item->isGroup()){
			continue;
		}	
		
		strncpy(tmpPtr->pinfo[iIndex].name , item->getName().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].value, item->getValue().c_str(),STRING_LENGTH-1);
		strncpy(tmpPtr->pinfo[iIndex].comment, item->getComment().c_str(),STRING_LENGTH-1);				
		++iIndex;
	}

	return caclSize;
}



}





