#include "route_snat.h"
#include "route_snatmgr.h"
#include "../global.h"

namespace maxnet{

	RouteSNATMgr::RouteSNATMgr(){

	}

	RouteSNATMgr::~RouteSNATMgr(){
#if 1
		RouteSNAT * snat = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			snat = (RouteSNAT *)node_list.at(i);
			if(!snat->isGroup())
				delete snat;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteSNATMgr::add(std::string id, std::string ifname, std::string src, std::string dst, std::string protocol, 
						std::string action, std::string dstip, std::string dstport, bool enable, std::string comment){
		RouteSNAT * snat = new RouteSNAT();
		snat->setID(id);
		snat->setIfname(ifname);
		snat->setSRC(src);
		snat->setDST(dst);
		snat->setProtocol(protocol);
		snat->setAction(action);
		snat->setDSTIP(dstip);
		snat->setDSTPort(dstport);
		snat->setEnable(enable);
		snat->setComment(comment);
		
		if(snat->isVaild()){
			addNode(snat);
		}
		else{
			delete snat;
		}
		return;
	}

	void RouteSNATMgr::output(){
		RouteSNAT * snat = NULL;
		std::string ids = "";
		bool 		sign = false;
		
		std::cout  << "# SNAT " << std::endl;
		for(unsigned int i=0; i < node_list.size(); i++){
			snat = (RouteSNAT *)node_list.at(i);
			snat->output();
			if(sign){
				ids += " " + snat->getID();
			}else{
				ids = snat->getID();
				sign = true;
			}
		}
		std::cout  << "ROUTE_SNAT_IDS=\"" << ids << "\"" << std::endl;
		return;
	}
}

