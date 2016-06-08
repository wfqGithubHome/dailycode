#include "firewall.h"
#include "firewallmgr.h"
#include "../global.h"

namespace maxnet{

	FirewallMgr::FirewallMgr(){		
		enable = false;
		mac_auth = false;
		scan_detect = false;
		dos_detect = false;

		for(int i=0; i< MAX_BRIDGE_ID; ++i)
			bridge_action_id[i] = 1;
	}

	FirewallMgr::~FirewallMgr(){
#if 1	
		Firewall * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (Firewall *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}
	
	void FirewallMgr::setEnable(bool enable){
		this->enable = enable;
	}
	
	bool FirewallMgr::getEnable(){
		return this->enable;
	}

	void FirewallMgr::setMacAuthEnable(bool status){
		mac_auth = status;
	}

	bool FirewallMgr::getMacAuthEnable(){
		return mac_auth;
	}

	bool FirewallMgr::getScanDetect(){
		return scan_detect;
	}

	void FirewallMgr::setScanDetect(bool enable){
		scan_detect=enable;
	}

	bool FirewallMgr::getDosDetect(){
		return dos_detect;
	}

	void FirewallMgr::setDosDetect(bool enable){
		dos_detect = enable;
	}

	void FirewallMgr::add(std::string name, std::string bridge_id, std::string proto, std::string auth, std::string action, std::string schedule, std::string vlan,
						std::string src, std::string dst, std::string mac,std::string in_traffic, std::string out_traffic, std::string session_limit, bool log_packet,
						std::string quota_name,std::string quota_action,std::string second_in_traffic,std::string second_out_traffic,
						std::string comment,bool disabled){
		Firewall * rule = new Firewall();
		if(proto.compare("") == 0) {
			proto="any";
		}
		if(src.compare("") == 0) {
			src="any";
		}
		if(dst.compare("") == 0) {
			dst="any";
		}
		if(mac.compare("") == 0) {
			mac="any";
		}
		if(src.substr(0,1).compare("!") == 0){
			rule->setSrcInvent(true);
			src = src.substr(1, src.size() -1 );
		}
		else{
			rule->setSrcInvent(false);
		}
		if(dst.substr(0,1).compare("!") == 0){
			rule->setDstInvent(true);
			dst = dst.substr(1, dst.size() -1 );
		}
		else{
			rule->setDstInvent(false);
		}
		if(mac.substr(0,1).compare("!") == 0){////////////?
			rule->setMacInvent(true);
			mac = mac.substr(1, dst.size() -1 );
		}
		else{
			rule->setMacInvent(false);
		}
		rule->setName(name);
		rule->setBridgeId(bridge_id);
		rule->setProto(proto);
		rule->setAuth(auth);
		rule->setAction(action);
		rule->setSchedule(schedule);
		rule->setVlan(vlan);

		rule->setSrc(src);
		rule->setDst(dst);
		rule->setMac(mac);

		rule->setInTraffic(in_traffic);
		rule->setOutTraffic(out_traffic);
		rule->setComment(comment);
		rule->setGroup(false);
		rule->setSessionLimit(session_limit);
		rule->setLogPacket(log_packet);
		//add new		
		int bridgeid = atoi(bridge_id.c_str());
		rule->setActionID(bridge_action_id[bridgeid]);
		rule->setQuota(quota_name);
		rule->setQuotaAction(quota_action);
		rule->setSencondInTraffic(second_in_traffic);
		rule->setSencondOutTraffic(second_out_traffic);
		rule->setDisabled(disabled);

		if(rule->isVaild()){
			addNode(rule);
			++ bridge_action_id[bridgeid];
		}
		else{
			delete rule;
		}
		return;
	}

	void FirewallMgr::output(){
		Firewall * rule = NULL;
		std::cout  << std::endl << "# firewall rules" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (Firewall *)node_list.at(i);
			rule->output();
		}
		

		
		return;

	}
}

