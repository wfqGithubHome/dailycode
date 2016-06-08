#include "route_dnat.h"
#include "route_dnatmgr.h"
#include "../global.h"

namespace maxnet{

	RouteDNATMgr::RouteDNATMgr(){

	}

	RouteDNATMgr::~RouteDNATMgr(){
#if 1
		RouteDNAT * dnat = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			dnat = (RouteDNAT *)node_list.at(i);
			delete dnat;
		}
		node_list.clear();
#endif		
		return;
	}

	void RouteDNATMgr::add(std::string id,bool enable, bool loop, std::string protocol_id, std::string comment, std::string wanip, std::string wan_port_before, 
						std::string wan_port_after, std::string lan_port_before,  std::string lan_port_after,std::string lan_ip,std::string ifname){
		RouteDNAT * dnat = new RouteDNAT();
		dnat->setID(id);
		dnat->setEnable(enable);
		dnat->setProtocoLId(protocol_id);
		dnat->setcomment(comment);
		dnat->setWanIp(wanip);
		dnat->setWanPortBefore(wan_port_before);
		dnat->setWanPortAfter(wan_port_after);
		dnat->setLanPortBefore(lan_port_before);
		dnat->setLANPortAfter(lan_port_after);
		dnat->setLanIp(lan_ip);
		dnat->setIfname(ifname);
		dnat->setloopEnable(loop);
		
		if(dnat->isVaild()){
			addNode(dnat);
		}
		else{
			delete dnat;
		}
		return;
	}

	void RouteDNATMgr::output(){
		RouteDNAT * dnat = NULL;
		std::string ids = "";
		bool temp = false;
		std::cout  << "# DNAT " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			dnat = (RouteDNAT *)node_list.at(i);
			dnat->output();

			if(temp){
				ids += " " + dnat->getID();
			}else{
				ids = dnat->getID();
				temp = true;
			}
		}

		std::cout << "ROUTE_DNAT_IDS=\"" << ids << "\"" << std::endl;
		return;
	}
}

