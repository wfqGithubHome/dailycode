#include "route_rule.h"
#include "route_rulemgr.h"
#include "../global.h"

namespace maxnet{

	RouteRuleMgr::RouteRuleMgr(){

	}

	RouteRuleMgr::~RouteRuleMgr(){
#if 1
		RouteRule * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (RouteRule *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteRuleMgr::add(std::string name, std::string src, std::string dst, std::string protocol, std::string src_port,
						std::string dst_port, std::string tos, std::string fwmark, std::string action, std::string comment){
		RouteRule * rule = new RouteRule();
		rule->setName(name);
		rule->setSRC(src);
		rule->setDST(dst);
		rule->setProtocol(protocol);
		rule->setSRCPort(src_port);
		rule->setDSTPort(dst_port);
		rule->setTOS(tos);
		rule->setAction(action);
		rule->setFWMark(fwmark);
		rule->setComment(comment);
		
		if(rule->isVaild()){
			addNode(rule);
		}
		else{
			delete rule;
		}
		return;
	}

	void RouteRuleMgr::output(){
		RouteRule * rule = NULL;
		std::cout  << "# Route Rule " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (RouteRule *)node_list.at(i);
			rule->output();
		}
		
		return;
	}
}

