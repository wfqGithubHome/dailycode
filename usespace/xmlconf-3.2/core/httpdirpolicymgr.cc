#include "httpdirpolicymgr.h"
#include "httpdirpolicynode.h"
#include "httpservermgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	HttpDirPolicyMgr::HttpDirPolicyMgr(){
		int i = 0;
		for(i = 0; i< MAX_BRIDGE_ID; ++i){
			policy_id[i] = 1;
		}
	}

	HttpDirPolicyMgr::~HttpDirPolicyMgr(){
#if 1
		HttpDirPolicy* policy = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			policy = (HttpDirPolicy *)node_list.at(i);
			if(!policy->isGroup())
				delete policy;
		}
		node_list.clear();
#endif
		return;
	}
	
	void HttpDirPolicyMgr::add(std::string name,std::string bridge_id, 
						std::string extgroup, std::string servergroup, 
						std::string comment,bool disabled){
		HttpDirPolicy* policy = new HttpDirPolicy();
		int ibridge_id = 0;
		
		std::stringstream bridge_id_string;
		bridge_id_string << bridge_id;
		bridge_id_string >> ibridge_id;

		policy->setName(name);
		policy->setID(policy_id[ibridge_id]);
		policy->setBridgeId(bridge_id);
		policy->setExtgroup(extgroup);
		policy->setServergroup(servergroup);
		policy->setComment(comment);
		policy->setDisabled(disabled);
		
		if(policy->isVaild()){
			addNode(policy);
			++policy_id[ibridge_id];
		}
		else{
			delete policy;
		}
		return;
	}

	void HttpDirPolicyMgr::output(){

		HttpDirPolicy* policy = NULL;
		
		std::cout  << std::endl << "# http dir policy rules" << std::endl<<std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			policy = (HttpDirPolicy *)node_list.at(i);
			policy->output();
		}
		return;

	}
}


