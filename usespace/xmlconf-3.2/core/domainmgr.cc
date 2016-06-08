#include "domainmgr.h"
#include "domainnode.h"
#include "../global.h"

namespace maxnet{

	DomainMgr::DomainMgr(bool addDefault){
		domain_id = 0;
		if(addDefault){
			add("any", "any", "any domain");
		}
	}

	DomainMgr::~DomainMgr(){
#if 1	
		DomainNode * domain = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			domain = (DomainNode *)node_list.at(i);
			if(!domain->isGroup())
				delete domain;
		}
		node_list.clear();
#endif
        return;
	}

	void DomainMgr::add(std::string name, std::string value, std::string comment){
		DomainNode * domain = new DomainNode();
		domain->setName(name);
		domain->setValue(value);
		domain->setComment(comment);
		domain->setGroup(false);
		domain->setID(domain_id);
		if(domain->isVaild()){
			addNode(domain);
			domain_id ++;
		}
		else{
			delete domain;
		}
		return;
	}

	void DomainMgr::output(){
		DomainNode * domain = NULL;
		std::cout << std::endl << "# domain and domain group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			domain = (DomainNode *)node_list.at(i);
			domain->output();
		}
		

		
		return;

	}
}

