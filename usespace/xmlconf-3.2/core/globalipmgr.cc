#include "globalip.h"
#include "globalipmgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	
	int	GlobalIPMgr::max_bridge_id = 0;

	GlobalIPMgr::GlobalIPMgr(){
		int i = 0;
		for(i = 0; i< 10; ++i){
			total_session_limit_overhead_string[i] = "";
			total_session_limit_overhead[i] = 0;
			total_session_limit_num[i] = 0;
		}
	}

	GlobalIPMgr::~GlobalIPMgr(){
#if 1		
		GlobalIP * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (GlobalIP *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}

	int GlobalIPMgr::getMaxBridgeID()
	{
		return this->max_bridge_id;
	}

	int GlobalIPMgr::getBridgeIDbyName(std::string bridge_id_string)
	{
		int id;
		std::stringstream bridge_id;
		bridge_id << bridge_id_string;
		bridge_id >> id;
		if(id < 0 || id >= 10)
			return 0;

		if(id > this->max_bridge_id)
		{
			this->max_bridge_id = id;
		}
		return id;
	}

	void GlobalIPMgr::setTotalSessionLimit(std::string bridge_id_string, std::string limit, std::string overhead){
		std::stringstream limit_num;
		int bridge_id = getBridgeIDbyName(bridge_id_string);

		if(bridge_id < 0 || bridge_id >= 10)
			return;

		limit_num << limit;
		limit_num >> this->total_session_limit_num[bridge_id];

		if(this->total_session_limit_num[bridge_id] < 0)
		{
			this->total_session_limit_num[bridge_id] = 0;
		}
		if(overhead.compare("block") == 0)
		{
			this->total_session_limit_overhead[bridge_id] = 1;
		}
		else
		{
			this->total_session_limit_overhead[bridge_id] = 0;
		}
		total_session_limit_overhead_string[bridge_id] = overhead;
	}
	
	int GlobalIPMgr::getTotalSessionLimitNum(int bridge_id){

		if(bridge_id < 0 || bridge_id >= 10)
			return 0;

		return this->total_session_limit_num[bridge_id];
	}
	
	int GlobalIPMgr::getTotalSessionLimitOverhead(int bridge_id){
		if(bridge_id < 0 || bridge_id >= 10)
			return 0;
		return this->total_session_limit_overhead[bridge_id];
	}

	std::string GlobalIPMgr::getTotalSessionLimitOverheadString(int bridge_id){
		if(bridge_id < 0 || bridge_id >= 10)
			return 0;
		return this->total_session_limit_overhead_string[bridge_id];
	}

	void GlobalIPMgr::add(std::string name, std::string bridge_id, std::string addr, std::string dst,
				std::string per_ip_session_limit, std::string total_session_limit, std::string action, 
				bool httplog,bool dnshelperpolicy, bool httpdirpolicy, 
				bool sesslog, bool enable_auth,std::string dst_type, std::string comment){

		GlobalIP * rule = new GlobalIP();

		if(addr.compare("") == 0) {
			addr="any";
		}
		if(dst.compare("") ==0) {
			dst = "any";
	    }
		if(action.compare("") == 0) {
			action = "trusted";
		}
		rule->setName(name);
		rule->setBridgeIDbyString(bridge_id);
		rule->setdstiptypebyString(dst_type);
		
		rule->setAddr(addr);
        rule->setDst(dst);
		
		rule->setPerIPSessionLimit(per_ip_session_limit);
		rule->setTotalSessionLimit(total_session_limit);
		rule->setAction(action);
		rule->setEnableHTTPLog(httplog);
		rule->setEnableDnsHelperPolicy(dnshelperpolicy);
		rule->setEnableHTTPDirPolicy(httpdirpolicy);
		rule->setEnableSessLog(sesslog);
		rule->setEnableAuth(enable_auth);
		
		rule->setComment(comment);
		rule->setGroup(false);


		if(rule->isVaild()){
			addNode(rule);
		}
		else{
			delete rule;
		}
		return;
	}

	void GlobalIPMgr::output(){
#if 1
		GlobalIP * rule = NULL;
		
		std::cout  << std::endl << "# global ip rules, max bridge id: " << max_bridge_id << std::endl;
		for(int i = 0; i < max_bridge_id + 1; i++)
		{
			std::cout << "# TOTAL_SESSION_LIMIT_NUM[" << i << "] = " << total_session_limit_num[i] << std::endl;
			std::cout << "# TOTAL_SESSION_LIMIT_OVERHEAD[" << i << "] = " << total_session_limit_overhead[i] << std::endl;
		}
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (GlobalIP *)node_list.at(i);
			rule->output();
		}
#endif
		return;

	}
}

