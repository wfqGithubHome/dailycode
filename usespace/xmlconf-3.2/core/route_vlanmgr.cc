#include <sstream>
#include "route_vlan.h"
#include "route_vlanmgr.h"
#include "../global.h"

namespace maxnet{

	RouteVLanMgr::RouteVLanMgr(){

	}

	RouteVLanMgr::~RouteVLanMgr(){
#if 1
		RouteVLan * vlan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			vlan = (RouteVLan *)node_list.at(i);
			if(!vlan->isGroup())
				delete vlan;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteVLanMgr::add(std::string name, std::string ifname, std::string id,std::string mac,std::string comment){

		RouteVLan * vlan = new RouteVLan();
		vlan->setName(name);
		vlan->setIfname(ifname);
		vlan->setID(id);
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

	
	void RouteVLanMgr::output(){
		RouteVLan * vlan = NULL;
		unsigned int id = 1;
		std::string tmp_str = "";
		std::string ids = "";
		bool 		sign = false;
		std::cout  << "# interface vlan " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			std::stringstream  p;
			p<<id;
			p>>tmp_str;
			
			vlan = (RouteVLan *)node_list.at(i);
			vlan->setIndex(tmp_str);
			vlan->output();

			if(sign){
				ids += " " + tmp_str;
			}else{
				ids = tmp_str;
				sign = true;
			}
			id++;
		}
		std::cout  << "ROUTE_INTERFACE_VLAN_IDS=\"" << ids << "\"" << std::endl;
		return;
	}
}

