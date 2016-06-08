#include "portmirror.h"
#include "portmirrormgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	
	int							PortMirrorMgr::max_bridge_id = 0;

	PortMirrorMgr::PortMirrorMgr(){
	}

	PortMirrorMgr::~PortMirrorMgr(){
#if 1
		PortMirror * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (PortMirror *)node_list.at(i);
			delete rule;
		}
		node_list.clear();
#endif
		return;
	}

	int PortMirrorMgr::getMaxBridgeID()
	{
		return this->max_bridge_id;
	}

	int PortMirrorMgr::getBridgeIDbyName(std::string bridge_id_string)
	{
		int id;
		std::stringstream bridge_id;
		bridge_id << bridge_id_string;
		bridge_id >> id;
		if(id < 0 || id >= MAX_BRIDGE_ID)
			return 0;

		if(id > this->max_bridge_id)
		{
			this->max_bridge_id = id;
		}
		return id;
	}

	void PortMirrorMgr::setEnable(std::string bridge_id_string, int enable){
		std::stringstream limit_num;
		int bridge_id = getBridgeIDbyName(bridge_id_string);

		if(bridge_id < 0 || bridge_id >= MAX_BRIDGE_ID)
			return;

		this->enable[bridge_id] = enable;
	}
	
	int PortMirrorMgr::getEnable(int bridge_id){
		if(bridge_id < 0 || bridge_id >= MAX_BRIDGE_ID)
			return 0;
		return this->enable[bridge_id];
	}
	

	void PortMirrorMgr::add(std::string name, std::string bridge_id, 
					std::string src_addr, std::string dst_addr, std::string proto, 
					std::string action, std::string comment,bool disabled){

		PortMirror * rule = new PortMirror();

		if(src_addr.compare("") == 0) {
			src_addr="any";
		}
		if(dst_addr.compare("") == 0) {
			dst_addr="any";
		}		
		if(action.compare("") == 0 || 
			(action.compare("in") != 0 && action.compare("out") != 0 && action.compare("all") != 0
			&&action.compare("in_discard") != 0 && action.compare("out_discard") != 0 && action.compare("all_discard") != 0)) {
			action = "all";
		}
		rule->setName(name);
		rule->setBridgeIDbyString(bridge_id);
		rule->setSrcAddr(src_addr);
		rule->setDstAddr(dst_addr);
		rule->setProto(proto);
		rule->setAction(action);
		rule->setComment(comment);
		rule->setGroup(false);
		
		rule->setDisabled(disabled);

		if(rule->isVaild()){
			addNode(rule);
		}
		else{
			delete rule;
		}
		return;
	}

	void PortMirrorMgr::output(){
#if 1
		PortMirror * rule = NULL;
		
		std::cout  << std::endl << "# port mirror rules, max bridge id: " << max_bridge_id << std::endl;
		for(int i = 0; i < max_bridge_id + 1; i++)
		{
			std::cout << "# ENABLE[" << i << "] = " << enable[i] << std::endl;
		}
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (PortMirror *)node_list.at(i);
			rule->output();
		}
#endif
		return;

	}
}


