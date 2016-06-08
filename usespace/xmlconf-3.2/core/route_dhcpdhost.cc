 #include "route_dhcpdhost.h"
#include "../global.h"

namespace maxnet{

	RouteDhcpdHost::RouteDhcpdHost(){

	}

	RouteDhcpdHost::~RouteDhcpdHost(){
		
	}
	
	bool RouteDhcpdHost::isVaild(){
		if(name.size() <= 0) return false;
		if(dhcpname.size() <= 0) return false;
		if(mac.size() <= 0) return false;
		if(ip.size() <= 0) return false;
		return true;
	}

	std::string RouteDhcpdHost::getCommand(){
		std::string command;
		command = "--dhcpdhost " + getName();
		return command;
	}

	void RouteDhcpdHost::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getDhcpName() << "\"\t\""
							<< getMAC() << "\"\t\""
							<< getIP() << "\"\t\""
							<< getMask() << "\"\t\""
							<< getGateway() << "\"\t\""
							<< getDnsName() << "\"\t\""
							<< getDNS() << "\"\t\""
							<< getLeaseTime() << "\"\t\""
							<< getMaxLeaseTime() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;

		return;
	}

	void RouteDhcpdHost::setDhcpName(const std::string dhcpname){
		this->dhcpname = dhcpname;
	}
	void RouteDhcpdHost::setMAC(const std::string mac){
		this->mac = mac;
	}
	void RouteDhcpdHost::setIP(const std::string ip){
		this->ip = ip;
	}
	void RouteDhcpdHost::setMask(const std::string mask){
		this->mask = mask;
	}
	void RouteDhcpdHost::setGateway(const std::string gateway){
		this->gateway = gateway;
	}
	void RouteDhcpdHost::setDnsName(const std::string dnsname){
		this->dnsname = dnsname;
	}
	void RouteDhcpdHost::setDNS(const std::string dns){
		this->dns = dns;
	}
	void RouteDhcpdHost::setLeaseTime(const std::string leasetime){
		this->leasetime = leasetime;
	}
	void RouteDhcpdHost::setMaxLeaseTime(const std::string maxleasetime){
		this->maxleasetime = maxleasetime;
	}
	void RouteDhcpdHost::setEnable(bool enable){
		this->enable = enable;
	}

	std::string RouteDhcpdHost::getDhcpName(){
		return dhcpname;
	}
	std::string RouteDhcpdHost::getMAC(){
		return mac;
	}
	std::string RouteDhcpdHost::getIP(){
		return ip;
	}
	std::string RouteDhcpdHost::getMask(){
		return mask;
	}
	std::string RouteDhcpdHost::getGateway(){
		return gateway;
	}
	std::string RouteDhcpdHost::getDnsName(){
		return dnsname;
	}
	std::string RouteDhcpdHost::getDNS(){
		return dns;
	}
	std::string RouteDhcpdHost::getLeaseTime(){
		return leasetime;
	}
	std::string RouteDhcpdHost::getMaxLeaseTime(){
		return maxleasetime;
	}
	bool RouteDhcpdHost::getEnable(){
		return enable;
	}
}

