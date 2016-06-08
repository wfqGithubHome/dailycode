#include "route_macbind.h"
#include "route_macbindmgr.h"
#include "../global.h"

namespace maxnet{

	RouteMACBindMgr::RouteMACBindMgr(){

	}

	RouteMACBindMgr::~RouteMACBindMgr(){
#if 1
		RouteMACBind * macbind = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			macbind = (RouteMACBind *)node_list.at(i);
			delete macbind;
		}
		node_list.clear();
#endif
		return;
    }

	void RouteMACBindMgr::setLearnt(bool learnt){
		this->learnt = learnt;
	}
	
	bool RouteMACBindMgr::getLearnt(){
		return learnt;
	}

/*	void RouteMACBindMgr::del(std::string ipaddr, std::string macaddr){
       	RouteMACBind * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (RouteMACBind *)node_list.at(i);
			if((rule->getIP()== ipaddr) && (rule->getMAC()== macaddr)){
				node_list.erase(rule);
				delete(rule);
			}
		}
	}*/

	void RouteMACBindMgr::add(std::string id, std::string ip, std::string mac, std::string action,
						std::string bridge, std::string comment){
		RouteMACBind *macbind = new RouteMACBind();
		macbind->setID(id);
		macbind->setIP(ip);
		macbind->setMAC(mac);
		macbind->setAction(action);
		macbind->setBridge(bridge);
		macbind->setComment(comment);
		
		if(macbind->isVaild()){
			addNode(macbind);
		}
		else{
			delete macbind;
		}
		return;
	}

	void RouteMACBindMgr::output(){
		RouteMACBind * macbind = NULL;
		std::cout << "# MAC Bind" << std::endl;
		for(unsigned int i=0; i < node_list.size(); i++){
			macbind = (RouteMACBind *)node_list.at(i);
			macbind->output();
		}
		return;
	}
}

