#include "route_macvlan.h"
#include "../global.h"

namespace maxnet{

	RouteMacVLan::RouteMacVLan(){
		id = "0";
	}

	RouteMacVLan::~RouteMacVLan(){
		
	}
	
	bool RouteMacVLan::isVaild(){
		if(name.size() <= 0) return false;
		if(ifname.size() <= 0) return false;
		return true;
	}

	std::string RouteMacVLan::getCommand(){
		std::string command;
		command = "--macvlan " + getName();
		return command;
	}

	void RouteMacVLan::output(){
		std::cout << "ROUTE_INTERFACE_MACVLAN_NAME_" << getID() << "=\"" << getName() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_MACVLAN_IFNAME_" << getID() << "=\"" << getIfname() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_MACVLAN_MAC_" << getID() << "=\"" << getMAC() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_MACVLAN_COMMENT_" << getID() << "=\"" << getComment() << "\"" << std::endl;
		return;
	}


	void RouteMacVLan::setName(const std::string name){
		this->name = name;
	}
	void RouteMacVLan::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	void RouteMacVLan::setMAC(const std::string mac){
		this->mac = mac;
	}
	void RouteMacVLan::setComment(const std::string comment){
		this->comment = comment;
	}
	void RouteMacVLan::setID(const std::string id){
		this->id = id;
	}

	std::string RouteMacVLan::getName(){
		return name;
	}
	std::string RouteMacVLan::getIfname(){
		return ifname;
	}
	std::string RouteMacVLan::getMAC(){
		return mac;
	}
	std::string RouteMacVLan::getComment(){
		return comment;
	}
	std::string RouteMacVLan::getID(){
		return id;
	}
}

