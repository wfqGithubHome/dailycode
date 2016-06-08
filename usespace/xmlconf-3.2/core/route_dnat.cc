#include "route_dnat.h"
#include "../global.h"

namespace maxnet{

	RouteDNAT::RouteDNAT(){

	}

	RouteDNAT::~RouteDNAT(){
		
	}
	
	bool RouteDNAT::isVaild(){
		if(ifname.size() <= 0) return false;
		if(comment.size() <= 0) return false;
		return true;
	}

	std::string RouteDNAT::getCommand(){
		std::string command;
		command = "--DNAT " + getIfname();
		return command;
	}

	void RouteDNAT::output(){
		std::cout << "ROUTE_DNAT_ID_" << getID() << "=\"" << getID() << "\"" << std::endl;
		if(getEnable())
		    std::cout << "ROUTE_DNAT_ENABLE_" << getID() << "=\"1\"" << std::endl;
		else
			std::cout << "ROUTE_DNAT_ENABLE_" << getID() << "=\"0\"" << std::endl;
		std::cout << "ROUTE_DNAT_PROTOCOL_" << getID() << "=\"" << getProtocoLId() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_COMMENT_" << getID() << "=\"" << getcomment() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_WANIP_" << getID() << "=\"" << getWanIp() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_WANPORTBEFORE_" << getID() << "=\"" << getWanPortBefore() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_WANPORTAFTER_" << getID() << "=\"" << getWanPortAfter() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_LANPORTBEFORE_" << getID() << "=\"" << getLanPortBefore() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_LANPORTAFTER_" << getID() << "=\"" << getLANPortAfter() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_LANIP_" << getID() << "=\"" << getLanIp() << "\"" << std::endl;
		std::cout << "ROUTE_DNAT_IFNAME_" << getID() << "=\"" << getIfname() << "\"" << std::endl;
        if(getloopEnable())
		    std::cout << "ROUTE_DNAT_LOOP_" << getID() << "=\"1\"" << std::endl;
		else
			std::cout << "ROUTE_DNAT_LOOP_" << getID() << "=\"0\"" << std::endl;
		
		return;
	}

	void RouteDNAT::setID(std::string id){
		this->id = id;
	}
	
	void RouteDNAT::setEnable(bool enable){
		this->enable = enable;
	}
	
	void RouteDNAT::setloopEnable(bool enable){
		this->loop = enable;
	}
	void RouteDNAT::setProtocoLId(const std::string protocol_id){
		this->protocol_id = protocol_id;
	}
	
	void RouteDNAT::setcomment(const std::string comment){
		this->comment = comment;
	}
	
	void RouteDNAT::setWanIp(const std::string wan_ip){
		this->wan_ip = wan_ip;
	}

	void RouteDNAT::setWanPortBefore(const std::string wan_port_before){
		this->wan_port_before = wan_port_before;
	}
	
	void RouteDNAT::setWanPortAfter(const std::string wan_port_after){
		this->wan_port_after = wan_port_after;
	}

	void RouteDNAT::setLanPortBefore(const std::string lan_port_before){
		this->lan_port_before = lan_port_before;
	}
	
	void RouteDNAT::setLANPortAfter(const std::string lan_port_after){
		this->lan_port_after = lan_port_after;
	}
	
	void RouteDNAT::setLanIp(const std::string lan_ip){
		this->lan_ip = lan_ip;
	}

	void RouteDNAT::setIfname(const std::string ifname){
		this->ifname = ifname;
	}

	std::string	RouteDNAT::getID(){
		return id;
	}

	bool RouteDNAT::getEnable(){
		return enable;
	}	

	bool RouteDNAT::getloopEnable(){
		return loop;
	}	
	std::string RouteDNAT::getProtocoLId(){
		return protocol_id;
	}
	
	std::string RouteDNAT::getcomment(){
		return comment;
	}

	std::string RouteDNAT::getWanIp(){
		return wan_ip;
	}

	std::string RouteDNAT::getWanPortBefore(){
		return wan_port_before;
	}
	
	std::string RouteDNAT::getWanPortAfter(){
		return wan_port_after;
	}
	
	std::string RouteDNAT::getLanPortBefore(){
		return lan_port_before;
	}
	
	std::string RouteDNAT::getLANPortAfter(){
		return lan_port_after;
	}

	std::string RouteDNAT::getLanIp(){
		return lan_ip;
	}

	std::string RouteDNAT::getIfname(){
		return ifname;
	}

}

