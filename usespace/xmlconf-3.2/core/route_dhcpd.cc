 #include "route_dhcpd.h"
#include "../global.h"

namespace maxnet{

	RouteDhcpd::RouteDhcpd(){

	}

	RouteDhcpd::~RouteDhcpd(){
		
	}
	
	bool RouteDhcpd::isVaild(){
		if(name.size() <= 0) return false;
		if(ifname.size() <= 0) return false;
		if(range.size() <= 0) return false;
		if(mask.size() <= 0) return false;
		if(gateway.size() <= 0) return false;
		if(dns.size() <= 0) return false;
		if(leasetime.size() <= 0) return false;
		if(maxleasetime.size() <= 0) return false;
		return true;
	}

	std::string RouteDhcpd::getCommand(){
		std::string command;
		command = "--dhcpd " + getName();
		return command;
	}

	void RouteDhcpd::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getIfname() << "\"\t\""
							<< getRange() << "\"\t\""
							<< getMask() << "\"\t\""
							<< getGateway() << "\"\t\""
							<< getDnsName() << "\"\t\""
							<< getDNS() << "\"\t\""
							<< getLeaseTime() << "\"\t\""
							<< getMaxLeaseTime() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;

		return;
	}

	void RouteDhcpd::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	void RouteDhcpd::setRange(const std::string range){
		this->range = range;
	}
	void RouteDhcpd::setMask(const std::string mask){
		this->mask = mask;
	}
	void RouteDhcpd::setGateway(const std::string gateway){
		this->gateway = gateway;
	}
	void RouteDhcpd::setDnsName(const std::string dnsname){
		this->dnsname= dnsname;
	}
	void RouteDhcpd::setDNS(const std::string dns){
		this->dns = dns;
	}
	void RouteDhcpd::setLeaseTime(const std::string leasetime){
		this->leasetime = leasetime;
	}
	void RouteDhcpd::setMaxLeaseTime(const std::string maxleasetime){
		this->maxleasetime = maxleasetime;
	}
	void RouteDhcpd::setEnable(bool enable){
		this->enable = enable;
	}

	std::string RouteDhcpd::getIfname(){
		return ifname;
	}
	std::string RouteDhcpd::getRange(){
		return range;
	}
	std::string RouteDhcpd::getMask(){
		return mask;
	}
	std::string RouteDhcpd::getGateway(){
		return gateway;
	}
	std::string RouteDhcpd::getDnsName(){
		return dnsname;
	}
	std::string RouteDhcpd::getDNS(){
		return dns;
	}
	std::string RouteDhcpd::getLeaseTime(){
		return leasetime;
	}
	std::string RouteDhcpd::getMaxLeaseTime(){
		return maxleasetime;
	}
	bool RouteDhcpd::getEnable(){
		return enable;
	}
}

