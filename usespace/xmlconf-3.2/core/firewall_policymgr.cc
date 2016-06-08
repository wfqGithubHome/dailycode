#include "firewall_policy.h"
#include "firewall_policymgr.h"
#include "../global.h"

namespace maxnet{

	FirewallPolicyMgr::FirewallPolicyMgr(){		
		firewall_enable = true;
	}

	FirewallPolicyMgr::~FirewallPolicyMgr(){
#if 1	
		FirewallPolicy * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (FirewallPolicy *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}

	void FirewallPolicyMgr::setFirewallEnable(bool enable){
		this->firewall_enable = enable;
	}

	bool FirewallPolicyMgr::getFireWallEnable(){
		return firewall_enable;
	}

	void FirewallPolicyMgr::add(std::string id, bool enable, std::string src, std::string dst, std::string isp, std::string proto,std::string session,std::string iface, 
				std::string action,std::string timed,std::string week,std::string day,std::string in_limit, std::string out_limit,std::string comment){
		FirewallPolicy * firewall = new FirewallPolicy();
#if 0
		if(proto.compare("") == 0) {
			proto="any";
		}
		if(src.compare("") == 0) {
			src="any";
		}
		if(dst.compare("") == 0) {
			dst="any";
		}
#endif
		firewall->setId(id);
		firewall->setEnable(enable);
		firewall->setSrc( src);
		firewall->setDst(dst);
		firewall->setIsp(isp);
		firewall->setProto(proto);
		firewall->setSession(session);
		firewall->setIface(iface);
		firewall->setAction(action);
		firewall->setTimed(timed);
		firewall->setWeek( week);
		firewall->setDay(day);
		firewall->setInLimit(in_limit);
		firewall->setOutLimit(out_limit);
		firewall->setComment( comment);

		if(firewall->isVaild()){
			addNode(firewall);
		}
		else{
			delete firewall;
		}
		return;
	}

	void FirewallPolicyMgr::output(){
		FirewallPolicy * rule = NULL;
		std::cout  << std::endl << "# firewall policy" << std::endl;

		std::cout << "# firewall_enable=\"" << (getFireWallEnable()?1:0) << "\"" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (FirewallPolicy *)node_list.at(i);
			rule->output();
		}
		
		return;
	}
}

