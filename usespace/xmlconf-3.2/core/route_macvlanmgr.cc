#include <sstream>
#include "route_macvlan.h"
#include "route_macvlanmgr.h"
#include "../global.h"

namespace maxnet{

	RouteMacVLanMgr::RouteMacVLanMgr(){

	}

	RouteMacVLanMgr::~RouteMacVLanMgr(){
#if 1
		RouteMacVLan * vlan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			vlan = (RouteMacVLan *)node_list.at(i);
			if(!vlan->isGroup())
				delete vlan;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteMacVLanMgr::add(std::string name, std::string ifname,std::string mac,std::string comment){

		RouteMacVLan * vlan = new RouteMacVLan();
		vlan->setName(name);
		vlan->setIfname(ifname);
		vlan->setMAC(mac);
		vlan->setComment(comment);
		
		if(vlan->isVaild()){
			addNode(vlan);
		}
		else{
			delete vlan;
		}
		return;
	}

	
	void RouteMacVLanMgr::output(){
		RouteMacVLan * macvlan = NULL;
		unsigned int id = 1;
		std::string tmp_str = "";
		std::string ids = "";
		bool 		sign = false;
		std::cout  << "# interface macvlan " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			std::stringstream  p;
			p<<id;
			p>>tmp_str;
			
			macvlan = (RouteMacVLan *)node_list.at(i);
			macvlan->setID(tmp_str);
			macvlan->output();
			
			if(sign){
				ids += " " + tmp_str;
			}else{
				ids = tmp_str;
				sign = true;
			}
			id++;
		}

		std::cout  << "ROUTE_INTERFACE_MACVLAN_IDS=\"" << ids << "\"" << std::endl;
		return;
	}
}

