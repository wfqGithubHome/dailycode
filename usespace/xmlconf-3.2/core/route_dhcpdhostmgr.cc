#include "route_dhcpdhost.h"
#include "route_dhcpdhostmgr.h"
#include "../global.h"

namespace maxnet{

	RouteDhcpdHostMgr::RouteDhcpdHostMgr(){

	}

	RouteDhcpdHostMgr::~RouteDhcpdHostMgr(){
#if 1		
		RouteDhcpdHost * dhcpdhost = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			dhcpdhost = (RouteDhcpdHost *)node_list.at(i);
			delete dhcpdhost;
		}
		node_list.clear();
#endif		
		return;
		
	}

	void RouteDhcpdHostMgr::add(std::string name, std::string dhcpname, std::string mac, std::string ip, 
				std::string mask, std::string gateway, std::string dnsname, std::string dns, 
				std::string leasetime, std::string maxleasetime, bool enable, std::string comment){

		RouteDhcpdHost * dhcpdhost = new RouteDhcpdHost();
		dhcpdhost->setName(name);
		dhcpdhost->setDhcpName(dhcpname);
		dhcpdhost->setMAC(mac);
		dhcpdhost->setIP(ip);
		dhcpdhost->setMask(mask);
		dhcpdhost->setGateway(gateway);
		dhcpdhost->setDnsName(dnsname);
		dhcpdhost->setDNS(dns);
		dhcpdhost->setLeaseTime(leasetime);
		dhcpdhost->setMaxLeaseTime(maxleasetime);
		dhcpdhost->setEnable(enable);
		dhcpdhost->setComment(comment);
		
		if(dhcpdhost->isVaild()){
			addNode(dhcpdhost);
		}
		else{
			delete dhcpdhost;
		}
		return;
	}

	void RouteDhcpdHostMgr::output(){
		RouteDhcpdHost * dhcpdhost = NULL;
		std::cout  << "#dhcpd host" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			dhcpdhost = (RouteDhcpdHost *)node_list.at(i);
			dhcpdhost->output();
		}
		return;
	}
}

