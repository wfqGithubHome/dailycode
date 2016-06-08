#include "route_vlan.h"
#include "../global.h"

namespace maxnet{

	RouteVLan::RouteVLan(){
		index = "0";
	}

	RouteVLan::~RouteVLan(){
		
	}
	
	bool RouteVLan::isVaild(){
		if(name.size() <= 0) return false;
		if(ifname.size() <= 0) return false;
		if(mac.size() <= 0) return false;
		return true;
	}

	std::string RouteVLan::getCommand(){
		std::string command;
		command = "--vlan " + getName();
		return command;
	}

	void RouteVLan::output(){
		std::cout << "ROUTE_INTERFACE_VLAN_NAME_" << getIndex() << "=\"" << getName() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_VLAN_IFNAME_" << getIndex() << "=\"" << getIfname() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_VLAN_MAC_" << getIndex() << "=\"" << getMAC() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_VLAN_ID_" << getIndex() << "=\"" << getID() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACE_VLAN_COMMENT_" << getIndex() << "=\"" << getComment() << "\"" << std::endl;
		return;
	}


	void RouteVLan::setName(const std::string name){
		this->name = name;
	}
	void RouteVLan::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	void RouteVLan::setMAC(const std::string mac){
		this->mac = mac;
	}
	void RouteVLan::setID(const std::string id){
		this->id = id;
	}
	void RouteVLan::setComment(const std::string comment){
		this->comment = comment;
	}
	void RouteVLan::setIndex(const std::string index){
		this->index = index;
	}

	std::string RouteVLan::getName(){
		return name;
	}
	std::string RouteVLan::getIfname(){
		return ifname;
	}
	std::string RouteVLan::getMAC(){
		return mac;
	}
	std::string RouteVLan::getID(){
		return id;
	}
	std::string RouteVLan::getComment(){
		return comment;
	}
	std::string RouteVLan::getIndex(){
		return index;
	}
}

