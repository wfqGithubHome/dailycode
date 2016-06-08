#include <sstream>
#include "route_routing.h"
#include "route_routingmgr.h"
#include "../global.h"

namespace maxnet{

	RouteRoutingMgr::RouteRoutingMgr(){
		hasbal = false;
		balfirstid= 0;
	}

	RouteRoutingMgr::~RouteRoutingMgr(){
#if 1
		RouteRouting * routing = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			routing = (RouteRouting *)node_list.at(i);
			if(!routing->isGroup())
				delete routing;
		}
		node_list.clear();
#endif		
		return;
	}


	void RouteRoutingMgr::add(std::string id, std::string dst, std::string gateway, std::string ifname,
					std::string priority, std::string table, std::string rule, bool enable, std::string comment){
		RouteRouting * routing = new RouteRouting();
		routing->setID(id);
		routing->setDST(dst);
		routing->setGateway(gateway);
		routing->setIFName(ifname);
		routing->setPriority(priority);
		routing->setTable(table);
		routing->setRule(rule);
		routing->setEnable(enable);
		routing->setComment(comment);
		
		if(routing->isVaild()){
			addNode(routing);
		}
		else{
			delete routing;
		}

		if(!hasbal){
			if(ifname.compare(0,3,"bal") == 0){
				hasbal = true;
				std::stringstream tmp;
				tmp << id;
				tmp >> balfirstid;
				balfirstid++;
			}
		}
		return;
	}

	void RouteRoutingMgr::output(){
		RouteRouting * routing = NULL;
		std::cout  << "# routing " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			routing = (RouteRouting *)node_list.at(i);
			routing->output();
		}
		
		return;
	}

	int RouteRoutingMgr::getBalanceFirstId(){
		if(hasbal){
			return balfirstid;
		}

		return 0;
	}
}

