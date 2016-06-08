#include "route_snat.h"
#include "../global.h"

namespace maxnet{

	RouteSNAT::RouteSNAT(){

	}

	RouteSNAT::~RouteSNAT(){
		
	}
	
	bool RouteSNAT::isVaild(){
		if(ifname.size() <= 0) return false;
		if(protocol.size() <= 0) return false;
		if(action.size() <= 0) return false;
		return true;
	}

	std::string RouteSNAT::getCommand(){
		std::string command;
		command = "--SNAT " + getIfname();
		return command;
	}

	void RouteSNAT::output(){
		std::cout << "# \"" << getID() << "\"\t\""
							<< getIfname() << "\"\t\""
							<< getSRC() << "\"\t\""
							<< getDST() << "\"\t\""
							<< getProtocol() << "\"\t\""
							<< getAction() << "\"\t\""
							<< getDSTIP() << "\"\t\""
							<< getDSTPort() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;
		
		std::cout << "ROUTE_SNAT_IFNAME_" << getID() << "=\"" << getIfname() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_SRC_" << getID() << "=\"" << getSRC() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_DST_" << getID() << "=\"" << getDST() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_PROTOCOL_" << getID() << "=\"" << getProtocol() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_ACTION_" << getID() << "=\"" << getAction() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_DSTIP_" << getID() << "=\"" << getDSTIP() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_DSTPORT_" << getID() << "=\"" << getDSTPort() << "\"" << std::endl;
		std::cout << "ROUTE_SNAT_ENABLE_" << getID() << "=\"" << getEnable() << "\"" << std::endl;
		
		return;

	}

	void RouteSNAT::setID(const std::string id){
		this->id = id;
	}
	
	void RouteSNAT::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	
	void RouteSNAT::setSRC(const std::string src){
		this->src = src;
	}
	
	void RouteSNAT::setDST(const std::string dst){
		this->dst = dst;
	}

	void RouteSNAT::setProtocol(const std::string protocol){
		this->protocol = protocol;
	}

	void RouteSNAT::setAction(const std::string action){
		this->action = action;
	}

	void RouteSNAT::setDSTIP(const std::string dstip){
		this->dstip = dstip;
	}
	
	void RouteSNAT::setDSTPort(const std::string dstport){
		this->dstport = dstport;
	}

	void RouteSNAT::setEnable(bool enable){
		this->enable = enable;
	}


	std::string RouteSNAT::getID(){
		return id;
	}
	
	std::string RouteSNAT::getIfname(){
		return ifname;
	}
	
	std::string RouteSNAT::getSRC(){
		return src;
	}
	
	std::string RouteSNAT::getDST(){
		return dst;
	}

	std::string RouteSNAT::getProtocol(){
		return protocol;
	}
	
	std::string RouteSNAT::getAction(){
		return action;
	}
	
	std::string RouteSNAT::getDSTIP(){
		return dstip;
	}
	
	std::string RouteSNAT::getDSTPort(){
		return dstport;
	}

	bool RouteSNAT::getEnable(){
		return enable;
	}	

}

