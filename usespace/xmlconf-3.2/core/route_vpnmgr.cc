#include "route_vpn.h"
#include "route_vpnmgr.h"
#include "../global.h"

namespace maxnet{

	RouteVPNClientMgr::RouteVPNClientMgr(){

	}

	RouteVPNClientMgr::~RouteVPNClientMgr(){
#if 1
		RouteVPNClient * vpn = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			vpn = (RouteVPNClient *)node_list.at(i);
			if(!vpn->isGroup())
				delete vpn;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteVPNClientMgr::add(std::string name, std::string ifname, std::string username,
				std::string password, std::string servername, std::string mtu, std::string weight,
				bool defaultroute, bool peerdns, bool enable, std::string comment){

		RouteVPNClient * vpn = new RouteVPNClient();
		vpn->setName(name);
		vpn->setIfname(ifname);
		vpn->setUserName(username);
		vpn->setPassword(password);
		vpn->setServerName(servername);
		vpn->setMTU(mtu);
		vpn->setWeight(weight);
		vpn->setDefaultRoute(defaultroute);
		vpn->setPeerDNS(peerdns);
		vpn->setEnable(enable);
		vpn->setComment(comment);
		
		if(vpn->isVaild()){
			addNode(vpn);
		}
		else{
			delete vpn;
		}
		return;
	}

	
	void RouteVPNClientMgr::output(){
		RouteVPNClient * vpn = NULL;
		std::cout  << "# VPN Client " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			vpn = (RouteVPNClient *)node_list.at(i);
			vpn->output();
		}
		
		return;
	}
}

