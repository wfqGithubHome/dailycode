#include "management.h"
#include "../global.h"

namespace maxnet{

	Management::Management(){

	}

	Management::~Management(){
		
	}
	
	bool Management::isVaild(){
		
		return true;
	}

	void	Management::output(){

		std::cout << std::endl << "# management" << std::endl;
		std::cout << "MANAGEMENT_IP=\"" << this->getIP() << "\"" << std::endl;
		std::cout << "MANAGEMENT_NETMASK=\"" << this->getNetmask() << "\"" << std::endl;
		std::cout << "MANAGEMENT_BROADCAST=\"" << this->getBroadcast() << "\"" << std::endl;
		std::cout << "MANAGEMENT_GATEWAY=\"" << this->getGateway() << "\"" << std::endl;
		std::cout << "MANAGEMENT_SERVER=\"" << this->getServer() << "\"" << std::endl;
		std::cout << "MANAGEMENT_DNS_PRIMARY=\"" << this->getDNSPrimary() << "\"" << std::endl;
		std::cout << "MANAGEMENT_DNS_SECONDARY=\"" << this->getDNSSecondary() << "\"" << std::endl;

		return;
	}

	void	Management::setIP(const std::string	ip){
		this->ip = ip;
	}

	void	Management::setNetmask(const std::string netmask){
		this->netmask = netmask;
	}

	void	Management::setBroadcast(const std::string	broadcast){
		this->broadcast = broadcast;
	}

	void	Management::setGateway(const std::string gateway){
		this->gateway = gateway;
	}

	void	Management::setServer(const std::string server){
		this->server = server;
	}

	void	Management::setDNSPrimary(const std::string dns_primary){
		this->dns_primary = dns_primary;
	}

	void	Management::setDNSSecondary(const std::string dns_secondary){
		this->dns_secondary = dns_secondary;
	}


	std::string 	Management::getIP(){
		return ip;
	}

	std::string 	Management::getNetmask(){
		return netmask;
	}

	std::string 	Management::getBroadcast(){
		return broadcast;
	}

	std::string 	Management::getGateway(){
		return gateway;
	}

	std::string 	Management::getServer(){
		return server;
	}

	std::string 	Management::getDNSPrimary(){
		return dns_primary;
	}

	std::string 	Management::getDNSSecondary(){
		return dns_secondary;
	}


}

