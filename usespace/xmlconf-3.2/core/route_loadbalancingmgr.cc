#include "route_loadbalancingmgr.h"
#include "../global.h"

namespace maxnet{

	RouteLoadBalancingMgr::RouteLoadBalancingMgr(){

	}

	RouteLoadBalancingMgr::~RouteLoadBalancingMgr(){
#if 1
		RouteLoadBalancing * loadb = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			loadb = (RouteLoadBalancing *)node_list.at(i);
			delete loadb;
		}
		node_list.clear();
#endif		
		return;

	}

	void RouteLoadBalancingMgr::add(std::string name, std::string ifname, std::string rulename, std::string weight, std::string comment){
		RouteLoadBalancing * loadb = new RouteLoadBalancing();
		loadb->setName(name);
		loadb->setIFName(ifname);
		loadb->setRuleName(rulename);
		loadb->setWeight(weight);
		loadb->setComment(comment);
		
		if(loadb->isVaild()){
			addNode(loadb);
		}
		else{
			delete loadb;
		}
		return;
	}

	void RouteLoadBalancingMgr::output(){
		RouteLoadBalancing * loadb = NULL;
		std::cout  << "# Route Load Balancing" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			loadb = (RouteLoadBalancing *)node_list.at(i);
			loadb->output();
		}
		
		return;
	}
}

