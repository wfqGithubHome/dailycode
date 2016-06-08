#include "route_arpbind.h"
#include "../global.h"

namespace maxnet{

	RouteARPBind::RouteARPBind(){

	}

	RouteARPBind::~RouteARPBind(){
		
	}
	
	bool RouteARPBind::isVaild(){
		if(ip.size() <= 0) return false;
		if(mac.size() <= 0) return false;
		return true;
	}

	std::string RouteARPBind::getCommand(){
		std::string command;
		command = "--ARPBind " + getIP();
		return command;
	}

	void RouteARPBind::output(){
		std::cout << "# \"" << getID() << "\"\t\""
							<< getIP() << "\"\t\""
							<< getMAC() << "\"\t\""
							<< getStatus() << "\"\t\""
							<< getType() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;

		return;

	}

	void RouteARPBind::setID(const std::string id){
		this->id = id;
	}
	
	void RouteARPBind::setIP(const std::string ip){
		this->ip = ip;
	}
	
	void RouteARPBind::setMAC(const std::string mac){
		this->mac = mac;
	}

	void RouteARPBind::setStatus(const std::string status){
		this->status = status;
	}

	void RouteARPBind::setType(const std::string type){
		this->type = type;
	}

	void RouteARPBind::setEnable(bool enable){
		this->enable = enable;
	}

	std::string RouteARPBind::getID(){
		return id;
	}

	std::string RouteARPBind::getIP(){
		return ip;
	}
	
	std::string RouteARPBind::getMAC(){
		return mac;
	}
	
	std::string RouteARPBind::getStatus(){
		return status;
	}

	std::string RouteARPBind::getType(){
		return type;
	}

	bool RouteARPBind::getEnable(){
		return enable;
	}	

}

