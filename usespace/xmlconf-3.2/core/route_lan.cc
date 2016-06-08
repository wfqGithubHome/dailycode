#include "route_lan.h"
#include "../global.h"

namespace maxnet{

	RouteLan::RouteLan(){
		id = "1";
	}

	RouteLan::~RouteLan(){
		
	}
	
	bool RouteLan::isVaild(){
		if(ip.size() <= 0) return false;
		if(mask.size() <= 0) return false;
		return true;
	}

	std::string RouteLan::getCommand(){
		std::string command;
		command = "--lan " + getIfname();
		return command;
	}

	void RouteLan::output(){					
		std::cout << "ROUTE_INTERFACE_LAN_MUTILIP_" << this->getID() << "=\"" << this->getIP() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_LAN_MUTILMASK_" << this->getID() << "=\"" << this->getMask() << "\"" << std::endl;

		return;
	}

	void RouteLan::setIfname(std::string ifname){
		this->ifname = ifname;
	}

	void RouteLan::setMAC(std::string mac){
		this->mac = mac;
	}

	void RouteLan::setNewMAC(std::string newmac){
		this->newmac = newmac;
	}
	
	void RouteLan::setIP(std::string ip){
		this->ip = ip;
	}

	void RouteLan::setMask(std::string mask){
		this->mask = mask;
	}

	void RouteLan::setAliases(std::string aliases){
		this->aliases = aliases;
	}

	void RouteLan::setID(std::string id){
		this->id = id;
	}


	std::string RouteLan::getIfname(){
		return ifname;
	}

	std::string RouteLan::getMAC(){
		return mac;
	}

	std::string RouteLan::getNewMAC(){
		return newmac;
	}

	std::string RouteLan::getIP(){
		return ip;
	}
	
	std::string RouteLan::getMask(){
		return mask;
	}

	std::string RouteLan::getID(){
		return id;
	}

	std::string RouteLan::getAliases(){
		return aliases;
	}

}

