#include "route_adsl.h"
#include "../global.h"

namespace maxnet{

	RouteADSLClient::RouteADSLClient(){

	}

	RouteADSLClient::~RouteADSLClient(){
		
	}
	
	bool RouteADSLClient::isVaild(){
		if(ifname.size() <= 0) return false;
		if(name.size() <= 0) return false;
		if(username.size() <= 0) return false;
		if(password.size() <= 0) return false;
		if(mtu.size() <= 0) return false;
		if(weight.size() <= 0) return false;
		
		return true;
	}

	std::string RouteADSLClient::getCommand(){
		std::string command;
		command = "--adsl " + getIfname();
		return command;
	}

	void RouteADSLClient::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getIfname() << "\"\t\""
							<< getUserName() << "\"\t\""
							<< getPassword() << "\"\t\""
							<< getServerName() << "\"\t\""
							<< getMTU() << "\"\t\""
							<< getWeight() << "\"\t\""
							<< getDefaultRoute() << "\"\t\""
							<< getPeerDNS() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;

		return;
	}

	/*void RouteADSLClient::setName(const std::string name){
		this->name = name;
	}*/
	
	void RouteADSLClient::setIfname(const std::string ifname){
		this->ifname = ifname;
	}

	void RouteADSLClient::setUserName(const std::string username){
		this->username = username;
	}
	
	void RouteADSLClient::setPassword(const std::string password){
		this->password = password;
	}

	void RouteADSLClient::setServerName(const std::string servername){
		this->servername = servername;
	}

	void RouteADSLClient::setMTU(const std::string mtu){
		this->mtu = mtu;
	}

	void RouteADSLClient::setWeight(const std::string weight){
		this->weight = weight;
	}

	void RouteADSLClient::setDefaultRoute(bool defaultroute){
		this->defaultroute = defaultroute;
	}
	
	void RouteADSLClient::setPeerDNS(bool peerdns){
		this->peerdns = peerdns;
	}
	
	void RouteADSLClient::setEnable(bool enable){
		this->enable = enable;
	}


	/*std::string RouteADSLClient::getName(){
		return name;
	}*/

	std::string RouteADSLClient::getIfname(){
		return ifname;
	}
	
	std::string RouteADSLClient::getUserName(){
		return username;
	}

	std::string RouteADSLClient::getPassword(){
		return password;
	}
	
	std::string RouteADSLClient::getServerName(){
		return servername;
	}

	std::string RouteADSLClient::getMTU(){
		return mtu;
	}

	std::string RouteADSLClient::getWeight(){
		return weight;
	}

	bool RouteADSLClient::getDefaultRoute(){
		return defaultroute;
	}

	bool RouteADSLClient::getPeerDNS(){
		return peerdns;
	}

	bool RouteADSLClient::getEnable(){
		return enable;
	}
}

