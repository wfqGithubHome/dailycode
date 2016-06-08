#include "quotamgr.h"
#include "quota.h"
#include "../global.h"

namespace maxnet{

	QuotaMgr::QuotaMgr(){
		this->add("any", "0000", " " ," ",0,"any quota");
		QuotaControl = false;
	}

	QuotaMgr::~QuotaMgr(){
#if 1
		Quota * quota = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			quota = (Quota *)node_list.at(i);
			if(!quota->isGroup())
				delete quota;
		}
		node_list.clear();
#endif		
		return;
	}
	
	void QuotaMgr::add(const std::string &name, const std::string &bandwidth_quota, 
				const std::string &ip_type,const std::string &cycle_type,
				const int id,const std::string &comment){
		Quota * quota = new Quota();
		quota->setName(name);
		quota->setBandwidthQuota(bandwidth_quota);
		quota->setIpType(ip_type);
		quota->setCycleType(cycle_type);
		quota->setQuotaID(id);
		quota->setComment(comment);
		if(quota->isVaild()){
			addNode(quota);
		}
		else{
			delete quota;
		}
		return;
	}

	void QuotaMgr::output(){
		Quota * quota = NULL;
		std::cout << std::endl << "# quotas" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			quota = (Quota *)node_list.at(i);
			quota->output();
		}
		return;

	}


	void QuotaMgr::setQuotaControl(bool status){
		this->QuotaControl = status;
	}

	bool QuotaMgr::getQuotaControl(){
		return this->QuotaControl;
	}
		

}
