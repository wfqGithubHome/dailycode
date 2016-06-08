#include "route_arpbind.h"
#include "route_arpbindmgr.h"
#include "../global.h"

namespace maxnet{

	RouteARPBindMgr::RouteARPBindMgr(){

	}

	RouteARPBindMgr::~RouteARPBindMgr(){
#if 1
	    RouteARPBind * arpbind = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			arpbind = (RouteARPBind *)node_list.at(i);
			delete arpbind;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteARPBindMgr::setAutoARPBind(bool autobind){
		this->autobind = autobind;
	}
	void RouteARPBindMgr::setCronMinute(const std::string minute){
		this->minute = minute;
	}
	
	bool RouteARPBindMgr::getAutoARPBind(){
		return autobind;
	}
	std::string RouteARPBindMgr::getCronMinute(){
		return minute;
	}

	void RouteARPBindMgr::add(std::string id, std::string ip, std::string mac, std::string status,
						std::string type, bool enable, std::string comment){
		RouteARPBind * arpbind = new RouteARPBind();
		arpbind->setID(id);
		arpbind->setIP(ip);
		arpbind->setMAC(mac);
		arpbind->setStatus(status);
		arpbind->setType(type);
		arpbind->setEnable(enable);
		arpbind->setComment(comment);
		
		if(arpbind->isVaild()){
			addNode(arpbind);
		}
		else{
			delete arpbind;
		}
		return;
	}

	void RouteARPBindMgr::output(){
		RouteARPBind * arpbind = NULL;
		std::cout << "# ARP Bind" << std::endl;
		for(unsigned int i=0; i < node_list.size(); i++){
			arpbind = (RouteARPBind *)node_list.at(i);
			arpbind->output();
		}
		return;
	}
}

