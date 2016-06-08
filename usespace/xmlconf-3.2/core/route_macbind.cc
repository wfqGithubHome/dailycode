#include "route_macbind.h"
#include "../global.h"

namespace maxnet{

	RouteMACBind::RouteMACBind(){

	}

	RouteMACBind::~RouteMACBind(){
		
	}
	
	bool RouteMACBind::isVaild(){
		if(ip.size() <= 0) return false;
		if(mac.size() <= 0) return false;
		return true;
	}

	std::string RouteMACBind::getCommand(){
		std::string command;
		command = "--MACBind " + getIP();
		return command;
	}

	void RouteMACBind::output(){
		std::cout << "# \"" << getID() << "\"\t\""
							<< getIP() << "\"\t\""
							<< getMAC() << "\"\t\""
							<< getAction() << "\"\t\""
							<< getBridge() << "\"\t\t//" << getComment() << std::endl;

		return;

	}

	void RouteMACBind::setID(const std::string id){
		this->id = id;
	}
	
	void RouteMACBind::setIP(const std::string ip){
		this->ip = ip;
	}
	
	void RouteMACBind::setMAC(const std::string mac){
		this->mac = mac;
	}

	void RouteMACBind::setAction(const std::string action){
		this->action = action;
	}

	void RouteMACBind::setBridge(const std::string bridge){
		this->bridge = bridge;
	}


	std::string RouteMACBind::getID(){
		return id;
	}

	std::string RouteMACBind::getIP(){
		return ip;
	}
	
	std::string RouteMACBind::getMAC(){
		return mac;
	}
	
	std::string RouteMACBind::getAction(){
		return action;
	}

	std::string RouteMACBind::getBridge(){
		return bridge;
	}	

}

