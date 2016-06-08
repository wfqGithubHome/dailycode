#include "netaddrmgr.h"
#include "netaddr.h"
#include "../global.h"

namespace maxnet{

	NetAddrMgr::NetAddrMgr(bool addDefault){
		if(addDefault){
			add("any", "0.0.0.0/0", false ,"any address");
		}
	}

	NetAddrMgr::~NetAddrMgr(){
#if 1
		NetAddr * addr = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			addr = (NetAddr *)node_list.at(i);
			if(!addr->isGroup())
				delete addr;
		}
		node_list.clear();
#endif
		return;
	}
	
	void NetAddrMgr::add(std::string name, std::string value, bool range, std::string comment){
		NetAddr * addr = new NetAddr();
		addr->setName(name);
		addr->setValue(value);
		addr->setRange(range);
		addr->setComment(comment);
		addr->setGroup(false);
		if(addr->isVaild()){
			addNode(addr);
		}
		else{
			delete addr;
		}
		return;
	}

	void NetAddrMgr::output(){
		NetAddr * addr = NULL;
		std::cout << std::endl << "# networks and networks group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			addr = (NetAddr *)node_list.at(i);
			addr->output();
		}
		

		
		return;

	}
}

