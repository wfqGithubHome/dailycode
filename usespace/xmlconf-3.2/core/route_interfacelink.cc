#include "route_interfacelink.h"
#include "../global.h"

namespace maxnet{

	Route_InterfaceLink::Route_InterfaceLink(){

	}

	Route_InterfaceLink::~Route_InterfaceLink(){
		
	}

	void Route_InterfaceLink::output(){
		std::cout << "ROUTE_INTERFACELINK_IFNAME_" << getIfname() << "=\"" << getIfname() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACELINK_TYPE_" << getIfname() << "=\"" << getType() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACELINK_DUPLEX_" << getIfname() << "=\"" << getDuplex() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACELINK_MODE_" << getIfname() << "=\"" << getMode() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACELINK_SPEED_" << getIfname() << "=\"" << getSpeed() << "\"" << std::endl;
		std::cout << "ROUTE_INTERFACELINK_LINKSTATE_" << getIfname() << "=\"" << getlinkstate() << "\"" << std::endl;

		return;
	}
	
	bool Route_InterfaceLink::isVaild(){
		
		return true;
	}

	void	Route_InterfaceLink::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	void    Route_InterfaceLink::setType(const std::string type){
		this->type = type;
	}
	void	Route_InterfaceLink::setlinkstate(const std::string	 linkstate){
		this->linkstate = linkstate;
	}
	void	Route_InterfaceLink::setMode(const std::string	 mode){
		this->mode = mode;
	}
	void	Route_InterfaceLink::setSpeed(const std::string speed){
		this->speed = speed;
	}
	void	Route_InterfaceLink::setDuplex(const std::string duplex){
		this->duplex = duplex;
	}

	std::string     Route_InterfaceLink::getIfname(){
		return this->ifname;
	}
	std::string     Route_InterfaceLink::getType(){
		return this->type;
	}
	std::string 	Route_InterfaceLink::getlinkstate(){
		return this->linkstate;
	}
	std::string 	Route_InterfaceLink::getMode(){
		return this->mode;
	}
	std::string 	Route_InterfaceLink::getSpeed(){
		return this->speed;
	}
	std::string 	Route_InterfaceLink::getDuplex(){
		return this->duplex;
	}

}

