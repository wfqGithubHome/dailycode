#include <sstream>
#include "route_lan.h"
#include "route_lanmgr.h"
#include "../global.h"

namespace maxnet{

	RouteLanMgr::RouteLanMgr(){
		subnetdatacount = 0;
	}

	RouteLanMgr::~RouteLanMgr(){
#if 1
		RouteLan * if_lan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			if_lan = (RouteLan *)lan_subnet_list.at(i);
			delete if_lan;
		}
		lan_subnet_list.clear();
#endif
        return;
	}
	
	void RouteLanMgr::subnetadd( std::string ip, std::string mask){
		RouteLan * if_lan = new RouteLan();
		if_lan->setIP(ip);
		if_lan->setMask(mask);
		
		if(if_lan->isVaild()){
			lan_subnet_list.push_back(if_lan);
			++subnetdatacount;
		}
		else{
			delete if_lan;
		}
		return;
	}

	void RouteLanMgr::output(){
		RouteLan * if_lan = NULL;
		std::cout  << "# interface lan " << std::endl;

		std::cout  << "ROUTE_INTERFACE_LAN_IP=\"" << getLanIp() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_MASK=\"" << getLanMask() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_MAC=\"" << getLanMac() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_IFNAME=\"" << getLanIfname() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP=\"" << getLanDhcp() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_STARTIP=\"" << getDhcpStartip() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_ENDIP=\"" << getDhcpEndip() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_LEASETIME=\"" << getDhcpLeaseTime() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_GATEWAY=\"" << getDhcpGateway() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_MASK=\"" << getDhcpMask() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_FIRSTDNS=\"" << getDhcpFirstDns() << "\"" << std::endl;
		std::cout  << "ROUTE_INTERFACE_LAN_DHCP_SECONDDNS=\"" << getDhcpSecondDns() << "\"" << std::endl;
		if(getSubnetEnable()){
			std::cout  << "ROUTE_INTERFACE_LAN_MUTILAN_ENABLE=\"1\"" << std::endl;
		}
		else{
			std::cout  << "ROUTE_INTERFACE_LAN_MUTILAN_ENABLE=\"0\"" << std::endl;
		}

		unsigned int id = 1;
		std::string tmp_str = "";
		std::string ids = "";
		bool 		sign = false;
		for(unsigned int i=0; i < lan_subnet_list.size(); i++){
			std::stringstream  p;
			p<<id;
			p>>tmp_str;
			
			if_lan = (RouteLan *)lan_subnet_list.at(i);
			if_lan->setID(tmp_str);
			if_lan->output();

			if(sign){
				ids += " " + tmp_str;
			}else{
				ids = tmp_str;
				sign = true;
			}
			id++;
		}
		std::cout  << "ROUTE_INTERFACE_LAN_MUTILIPIDS=\"" << ids << "\"" << std::endl;
	
		return;
	}

	void RouteLanMgr::setLanIp(std::string ip){
		this->ip = ip;
	}
	void RouteLanMgr::setLanMask(std::string mask){
		this->mask = mask;
	}
	void RouteLanMgr::setLanMac(std::string mac){
		this->mac = mac;
	}
	void RouteLanMgr::setLanIfname(std::string ifname){
		this->ifname = ifname;
	}
	void RouteLanMgr::setLanDhcp(std::string dhcp){
		this->dhcp = dhcp;
	}
	void RouteLanMgr::setDhcpStartip(std::string startip){
		this->startip = startip;
	}
	void RouteLanMgr::setDhcpEndip(std::string endip){
		this->endip = endip;
	}
	void RouteLanMgr::setDhcpLeaseTime(std::string lease_time){
		this->lease_time = lease_time;
	}
	void RouteLanMgr::setDhcpGateway(std::string gateway){
		this->gateway = gateway;
	}
	void RouteLanMgr::setDhcpMask(std::string dhcp_mask){
		this->dhcp_mask = dhcp_mask;
	}
	void RouteLanMgr::setDhcpFirstDns(std::string first_dns){
		this->first_dns = first_dns;
	}
	void RouteLanMgr::setDhcpSecondDns(std::string second_dns){
		this->second_dns = second_dns;
	}
	void RouteLanMgr::setSubnetEnable(bool subnet_enable){
		this->subnet_enable = subnet_enable;
	}

	std::string RouteLanMgr::getLanIp(){
		return ip;
	}
	std::string RouteLanMgr::getLanMask(){
		return mask;
	}
	std::string RouteLanMgr::getLanMac(){
		return mac;
	}
	std::string RouteLanMgr::getLanIfname(){
		return ifname;
	}
	std::string RouteLanMgr::getLanDhcp(){
		return dhcp;
	}
	std::string RouteLanMgr::getDhcpStartip(){
		return startip;
	}
	std::string RouteLanMgr::getDhcpEndip(){
		return endip;
	}
	std::string RouteLanMgr::getDhcpLeaseTime(){
		return lease_time;
	}
	std::string RouteLanMgr::getDhcpGateway(){
		return gateway;
	}
	std::string RouteLanMgr::getDhcpMask(){
		return mask;
	}
	std::string RouteLanMgr::getDhcpFirstDns(){
		return first_dns;
	}
	std::string RouteLanMgr::getDhcpSecondDns(){
		return second_dns;
	}
	bool        RouteLanMgr::getSubnetEnable(){
		return subnet_enable;
	}

    int RouteLanMgr::getsubnetdatacount(){
		return subnetdatacount;
	}
    std::vector<RouteLan *> RouteLanMgr::getlansubnetlist(){
		return lan_subnet_list;
	}
}

