#include "route_dns.h"
#include "../global.h"

namespace maxnet{

	RouteDNS::RouteDNS(){

	}

	RouteDNS::~RouteDNS(){
		
	}
	
	bool RouteDNS::isVaild(){
		
		return true;
	}

	void	RouteDNS::output(){
		std::cout << " # DNS" << std::endl;
		std::cout << "ROUTE_DNS_PRIMARY=\"" << this->getDNSPrimary() << "\"" << std::endl;
		std::cout << "ROUTE_DNS_SECONDARY=\"" << this->getDNSSecondary() << "\"" << std::endl;
		std::cout << "ROUTE_DNS_THIRNDARY=\"" << this->getDNSThirdary() << "\"" << std::endl;

		return;
	}


	void	RouteDNS::setDNSPrimary(const std::string dns_primary){
		this->dns_primary = dns_primary;
	}

	void	RouteDNS::setDNSSecondary(const std::string dns_secondary){
		this->dns_secondary = dns_secondary;
	}
	
	void	RouteDNS::setDNSThirdary(const std::string dns_thirdary){
		this->dns_thirdary = dns_thirdary;
	}

	std::string	RouteDNS::getDNSPrimary(){
		return dns_primary;
	}

	std::string	RouteDNS::getDNSSecondary(){
		return dns_secondary;
	}
	
	std::string	RouteDNS::getDNSThirdary(){
		return dns_thirdary;
	}
}

