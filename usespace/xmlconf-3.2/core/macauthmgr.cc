#include "macauthmgr.h"
#include "macauth.h"
#include "../global.h"

namespace maxnet{

	MacAuthMgr::MacAuthMgr(){
		add("any", "0-0-0-0-0-0", "any address");
		add("unknow", "FF-FF-FF-FF-FF-FF", "unknow address");
	}

	MacAuthMgr::~MacAuthMgr(){
#if 1
		MacAuth * mac_addr = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			mac_addr = (MacAuth *)node_list.at(i);
			if(!mac_addr->isGroup())
				delete mac_addr;
		}
		node_list.clear();
#endif
		return;
	}
	
	void MacAuthMgr::add(std::string name, std::string value, std::string comment){
		MacAuth * mac_addr = new MacAuth();
		mac_addr->setName(name);
		mac_addr->setValue(value);
		mac_addr->setComment(comment);
		mac_addr->setGroup(false);
		if(mac_addr->isVaild()){
			addNode(mac_addr);
		}
		else{
			delete mac_addr;
		}
		return;
	}

	void MacAuthMgr::output(){
		MacAuth * mac_addr = NULL;
		std::cout << std::endl << "# macauths and macauths group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			mac_addr = (MacAuth *)node_list.at(i);
			mac_addr->output();
		}
		return;

	}
}

