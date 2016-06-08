#include "interface.h"
#include "interfacemgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	

	InterfaceMgr::InterfaceMgr(){
	}

	InterfaceMgr::~InterfaceMgr(){
#if 1
		Interface * inter = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			inter = (Interface *)node_list.at(i);
			if(!inter->isGroup())
				delete inter;
		}
		node_list.clear();
#endif
		return;	
	}

	void InterfaceMgr::add(std::string name, std::string mode, std::string speed, 
				std::string duplex, std::string linkstate, std::string bridgeid, std::string comment){

		Interface * inter = new Interface();

		inter->setName(name);
		inter->setMode(mode);
		inter->setSpeed(speed);
		inter->setDuplex(duplex);
		inter->setbridgeid(bridgeid);
		inter->setlinkstate(linkstate);

		inter->setComment(comment);
		inter->setGroup(false);


		if(inter->isVaild()){
			addNode(inter);
		}
		else{
			delete inter;
		}
		return;
	}

	void InterfaceMgr::output(){
		Interface * inter = NULL;
		
		std::cout  << std::endl << "# interfaces " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			inter = (Interface *)node_list.at(i);
			inter->output();
		}
		std::cout  << std::endl;
		return;

	}
}


