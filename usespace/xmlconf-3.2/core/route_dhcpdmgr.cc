#include "route_dhcpd.h"
#include "route_dhcpdmgr.h"
#include "../global.h"

namespace maxnet{

	RouteDhcpdMgr::RouteDhcpdMgr(){

	}

	RouteDhcpdMgr::~RouteDhcpdMgr(){
#if 1
		RouteDhcpd * dhcpd = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			dhcpd = (RouteDhcpd *)node_list.at(i);
			delete dhcpd;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteDhcpdMgr::setDhcpdEnable(bool dhcpd_enable){
		this->dhcpd_enable = dhcpd_enable;
	}

	bool RouteDhcpdMgr::getDhcpdEnable(){
		return dhcpd_enable;
	}

	void RouteDhcpdMgr::add(std::string name, std::string ifname, std::string range, std::string mask,
					std::string gateway, std::string dnsname, std::string dns, std::string leasetime,
					std::string maxleasetime, bool enable, std::string comment){

		RouteDhcpd * dhcpd = new RouteDhcpd();
		dhcpd->setName(name);
		dhcpd->setIfname(ifname);
		dhcpd->setRange(range);
		dhcpd->setMask(mask);
		dhcpd->setGateway(gateway);
		dhcpd->setDnsName(dnsname);
		dhcpd->setDNS(dns);
		dhcpd->setLeaseTime(leasetime);
		dhcpd->setMaxLeaseTime(maxleasetime);
		dhcpd->setEnable(enable);
		dhcpd->setComment(comment);
		
		if(dhcpd->isVaild()){
			addNode(dhcpd);
		}
		else{
			delete dhcpd;
		}
		return;
	}

	void RouteDhcpdMgr::output(){
		RouteDhcpd * dhcpd = NULL;
		std::cout  << "#dhcpd servers " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			dhcpd = (RouteDhcpd *)node_list.at(i);
			dhcpd->output();
		}
		return;
	}
}

