#include "route_interfacelink.h"
#include "route_interfacelinkmgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	

	Route_InterfaceLinkMgr::Route_InterfaceLinkMgr(){
	}

	Route_InterfaceLinkMgr::~Route_InterfaceLinkMgr(){
#if 1
		Route_InterfaceLink * inter = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			inter = (Route_InterfaceLink *)node_list.at(i);
			if(!inter->isGroup())
				delete inter;
		}
		node_list.clear();
#endif
		return;	
	}

	void Route_InterfaceLinkMgr::add(std::string ifname,  std::string type,std::string mode, std::string speed, 
				std::string duplex, std::string linkstate){

		Route_InterfaceLink * inter = new Route_InterfaceLink();

		inter->setIfname(ifname);
		inter->setType(type);
		inter->setMode(mode);
		inter->setSpeed(speed);
		inter->setDuplex(duplex);
		inter->setlinkstate(linkstate);
		inter->setGroup(false);


		if(inter->isVaild()){
			addNode(inter);
		}
		else{
			delete inter;
		}
		return;
	}

	void Route_InterfaceLinkMgr::output(){
		std::string ifwan_links = "";
		bool temp = false;
		Route_InterfaceLink * inter = NULL;
		
		std::cout  << std::endl << "# interfaces link" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			inter = (Route_InterfaceLink *)node_list.at(i);
			inter->output();

			if(temp){
				ifwan_links += " " + inter->getIfname();
			}else{
				ifwan_links = inter->getIfname();
				temp = true;
			}
		}
		std::cout << "ROUTE_INTERFACELINK_IFNAME=\"" << ifwan_links << "\""  << std::endl;
		std::cout  << std::endl;
		return;

	}
}


