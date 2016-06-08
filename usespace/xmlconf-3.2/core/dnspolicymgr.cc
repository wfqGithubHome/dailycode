#include "dnspolicy.h"
#include "dnspolicymgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	DnspolicyMgr::DnspolicyMgr():		
			enable(false)
	{
		int i = 0;
		for(i = 0; i< MAX_BRIDGE_ID; ++i){
			policy_id[i] = 1;
		}
		
	}

	DnspolicyMgr::~DnspolicyMgr(){
#if 1
		DnsPolicy* policy = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			policy = (DnsPolicy *)node_list.at(i);
			if(!policy->isGroup())
				delete policy;
		}
		node_list.clear();
#endif
        return;
	}
	
	void DnspolicyMgr::setEnable(bool enable){
		this->enable = enable;
	}
	
	bool DnspolicyMgr::getEnable(){
		return this->enable;
	}

	void DnspolicyMgr::add(std::string name, std::string bridge_id, 
						std::string src, std::string dst, 
						std::string domain_name,
						std::string action,
						std::string targetip,
						std::string comment,bool disabled){
		DnsPolicy* policy = new DnsPolicy();
		int ibridge_id = 0;
		
		if(src.compare("") == 0) {
			src="any";
		}
		if(dst.compare("") == 0) {
			dst="any";
		}
		if(domain_name.compare("") == 0) {
			domain_name="any";
		}

		std::stringstream bridge_id_string;
		bridge_id_string << bridge_id;
		bridge_id_string >> ibridge_id;

		std::stringstream _id_string;
		_id_string << policy_id[ibridge_id];	
		std::string id_string;
		_id_string >> id_string;
		
		policy->setRawName(name);
		policy->setName(id_string);
		policy->setBridgeId(bridge_id);
		policy->setSrc(src);
		policy->setDst(dst);
		policy->setDomainName(domain_name);
		policy->setAction(action);
		policy->setTargetip(targetip);
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

	void DnspolicyMgr::output(){

		DnsPolicy* policy = NULL;
		
		std::cout  << std::endl << "# dns policy rules" << std::endl<<std::endl;
		std::cout << "<application_policy>" << this->enable <<"</application_policy>" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			policy = (DnsPolicy *)node_list.at(i);
			policy->output();
		}

		
		return;

	}
}


