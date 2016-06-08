#include "route_adsl.h"
#include "route_adslmgr.h"
#include "../global.h"

namespace maxnet{

	RouteADSLClientMgr::RouteADSLClientMgr(){

	}

	RouteADSLClientMgr::~RouteADSLClientMgr(){
#if 1
		RouteADSLClient * adsl = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			adsl = (RouteADSLClient *)node_list.at(i);
			if(!adsl->isGroup())
				delete adsl;
		}
		node_list.clear();
#endif		
		return;
	}

	void RouteADSLClientMgr::add(std::string name, std::string ifname, std::string username,
				std::string password, std::string servername, std::string mtu, std::string weight,
				bool defaultroute, bool peerdns, bool enable, std::string comment){

		RouteADSLClient * adsl = new RouteADSLClient();
		adsl->setName(name);
		adsl->setIfname(ifname);
		adsl->setUserName(username);
		adsl->setPassword(password);
		adsl->setServerName(servername);
		adsl->setMTU(mtu);
		adsl->setWeight(weight);
		adsl->setDefaultRoute(defaultroute);
		adsl->setPeerDNS(peerdns);
		adsl->setEnable(enable);
		adsl->setComment(comment);
		adsl->setGroup(false);
		
		if(adsl->isVaild()){
			addNode(adsl);
		}
		else{
			delete adsl;
		}
		return;
	}

	
	void RouteADSLClientMgr::output(){
		RouteADSLClient * adsl = NULL;
		std::cout  << "# ADSL Client " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			adsl = (RouteADSLClient *)node_list.at(i);
			adsl->output();
		}
		
		return;
	}
}

