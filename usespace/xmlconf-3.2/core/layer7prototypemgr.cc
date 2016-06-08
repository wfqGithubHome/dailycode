#include "layer7prototype.h"
#include "layer7prototypemgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	
	Layer7ProtoTypeMgr::Layer7ProtoTypeMgr(){
	}

	Layer7ProtoTypeMgr::~Layer7ProtoTypeMgr(){
#if 1
		Layer7ProtoType * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (Layer7ProtoType *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}

	void Layer7ProtoTypeMgr::add(std::string name, std::string value){

		Layer7ProtoType * rule = new Layer7ProtoType();

		rule->setName(name);
		rule->setValue(value);
		rule->setGroup(false);
		
		if(rule->isVaild()){
			addNode(rule);
		}
		else{
			delete rule;
		}
		return;
	}

	void Layer7ProtoTypeMgr::output(){
		Layer7ProtoType * rule = NULL;
		
		std::cout  << std::endl << "# layer7 protocol type display string:" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (Layer7ProtoType *)node_list.at(i);
			rule->output();
		}
	}
}


