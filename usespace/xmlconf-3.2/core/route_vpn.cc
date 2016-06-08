#include "route_vpn.h"
#include "../global.h"

namespace maxnet{

	RouteVPNClient::RouteVPNClient(){

	}

	RouteVPNClient::~RouteVPNClient(){
		
	}
	
	bool RouteVPNClient::isVaild(){
		if(ifname.size() <= 0) return false;
		if(name.size() <= 0) return false;
		if(username.size() <= 0) return false;
		if(password.size() <= 0) return false;
		if(servername.size() <= 0) return false;
		if(mtu.size() <= 0) return false;
		
		return true;
	}

	std::string RouteVPNClient::getCommand(){
		std::string command;
		command = "--adsl " + getIfname();
		return command;
	}

	void RouteVPNClient::output(){
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

	void RouteVPNClient::setName(const std::string name){
		this->name = name;
	}
	
	void RouteVPNClient::setIfname(const std::string ifname){
		this->ifname = ifname;
	}

	void RouteVPNClient::setUserName(const std::string username){
		this->username = username;
	}
	
	void RouteVPNClient::setPassword(const std::string password){
		this->password = password;
	}

	void RouteVPNClient::setServerName(const std::string servername){
		this->servername = servername;
	}

	void RouteVPNClient::setMTU(const std::string mtu){
		this->mtu = mtu;
	}

	void RouteVPNClient::setWeight(const std::string weight){
		this->weight = weight;
	}

	void RouteVPNClient::setDefaultRoute(bool defaultroute){
		this->defaultroute = defaultroute;
	}

	void RouteVPNClient::setPeerDNS(bool peerdns){
			this->peerdns = peerdns;
		}
	
	void RouteVPNClient::setEnable(bool enable){
		this->enable = enable;
	}


	std::string RouteVPNClient::getName(){
		return name;
	}

	std::string RouteVPNClient::getIfname(){
		return ifname;
	}
	
	std::string RouteVPNClient::getUserName(){
		return username;
	}

	std::string RouteVPNClient::getPassword(){
		return password;
	}
	
	std::string RouteVPNClient::getServerName(){
		return servername;
	}

	std::string RouteVPNClient::getMTU(){
		return mtu;
	}

	std::string RouteVPNClient::getWeight(){
		return weight;
	}

	bool RouteVPNClient::getDefaultRoute(){
		return defaultroute;
	}

	bool RouteVPNClient::getPeerDNS(){
		return peerdns;
	}

	bool RouteVPNClient::getEnable(){
		return enable;
	}
}

