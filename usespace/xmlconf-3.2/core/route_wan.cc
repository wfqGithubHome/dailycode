#include "route_wan.h"
#include "../global.h"

namespace maxnet{

	RouteWan::RouteWan(){

	}

	RouteWan::~RouteWan(){
		
	}
	
	bool RouteWan::isVaild(){
		if(ifname.size() <= 0) return false;
		if(access.size() <= 0) return false;
		return true;
	}

	std::string RouteWan::getCommand(){
		std::string command;
		command = "--wan " + getIfname();
		return command;
	}

	void RouteWan::output(){
		std::cout << "ROUTE_WAN_ID_" << getID() << "=\"" << getID() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_IFNAME_" << getID() << "=\"" << getIfname() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_ACCESS_" << getID() << "=\"" << getAccess() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_TYPE_" << getID() << "=\"" << getType() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_IP_" << getID() << "=\"" << getIP() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_MASKE_" << getID() << "=\"" << getMask() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_GATEWAYE_" << getID() << "=\"" << getGateway() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_USERNAME_" << getID() << "=\"" << getUsername() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_PASSWD_" << getID() << "=\"" << getPasswd() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_SERVERNAME_" << getID() << "=\"" << getServername() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_MTU_" << getID() << "=\"" << getMtu() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_MAC_" << getID() << "=\"" << getMac() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_DNS_" << getID() << "=\"" << getDNS() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_UP_" << getID() << "=\"" << getUp() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_DOWN_" << getID() << "=\"" << getDown() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_ISP_" << getID() << "=\"" << getIsp() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_TIME_" << getID() << "=\"" << getTime() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_WEEK_" << getID() << "=\"" << getWeek() << "\"" << std::endl;
		std::cout << "ROUTE_WAN_DAY_" << getID() << "=\"" << getDay() << "\"" << std::endl;
		if(getDefaultRoute()){
			std::cout << "ROUTE_WAN_DEFAULTROUTE_" << getID() << "=\"1\"" << std::endl;
		}
		else{
			std::cout << "ROUTE_WAN_DEFAULTROUTE_" << getID() << "=\"0\"" << std::endl;
		}
		if(getEnable()){
			std::cout << "ROUTE_WAN_ENABLE_" << getID() << "=\"1\"" << std::endl;
		}
		else{
			std::cout << "ROUTE_WAN_ENABLE_" << getID() << "=\"0\"" << std::endl;
		}
		

		return;
	}

	void RouteWan::setID(const std::string id){
		this->id = id;
	}

	void RouteWan::setIfname(const std::string ifname){
		this->ifname = ifname;
	}
	void RouteWan::setAccess(const std::string access){
		this->access = access;
	}
	void RouteWan::setType(const std::string type){
		this->type = type;
	}
	void RouteWan::setIP(const std::string ip){
		this->ip = ip;
	}
	void RouteWan::setMask(const std::string mask){
		this->mask = mask;
	}
	void RouteWan::setGateway(const std::string gateway){
		this->gateway = gateway;
	}
	void RouteWan::setUsername(const std::string username){
		this->username = username;
	}
    void RouteWan::setPasswd(const std::string passwd){
		this->passwd = passwd;
	}
    void RouteWan::setServername(const std::string servername){
		this->servername = servername;
	}
    void RouteWan::setMtu(const std::string mtu){
		this->mtu = mtu;
	}
	void RouteWan::setMAC(const std::string mac){
		this->mac = mac;
	}
	void RouteWan::setDNS(std::string dns){
		this->dns = dns;
	}
    void RouteWan::setUP(std::string up){
		this->up = up;
	}
    void RouteWan::setDown(std::string down){
		this->down = down;
	}
    void RouteWan::setISP(std::string isp){
		this->isp = isp;
	}
	void RouteWan::setDefaultRoute(bool default_route){
		this->default_route = default_route;
	}
    void RouteWan::setTime(std::string time){
		this->time = time;
	}
    void RouteWan::setWeek(std::string week){
		this->week = week;
	}
    void RouteWan::setDay(std::string day){
		this->day = day;
	}
	void RouteWan::setEnable(bool enable){
		this->enable = enable;
	}

	std::string RouteWan::getID(){
		return id;
	}
	std::string RouteWan::getIfname(){
		return ifname;
	}
	std::string RouteWan::getAccess(){
		return access;
	}
	std::string RouteWan::getType(){
		return type;
	}
	std::string RouteWan::getIP(){
		return ip;
	}
	std::string RouteWan::getMask(){
		return mask;
	}
	std::string RouteWan::getGateway(){
		return gateway;
	}
	std::string RouteWan::getUsername(){
		return username;
	}
	std::string RouteWan::getPasswd(){
		return passwd;
	}
    std::string RouteWan::getServername(){
		return servername;
	}
	std::string RouteWan::getMtu(){
		return mtu;
	}
    std::string RouteWan::getMac(){
		return mac;
	}
	std::string RouteWan::getDNS(){
		return dns;
	}
    std::string RouteWan::getUp(){
		return up;
	}
	std::string RouteWan::getDown(){
		return down;
	}
    std::string RouteWan::getIsp(){
		return isp;
	}
    std::string RouteWan::getTime(){
		return time;
	}
	std::string RouteWan::getWeek(){
		return week;
	}
    std::string RouteWan::getDay(){
		return day;
	}

	bool RouteWan::getDefaultRoute(){
		return default_route;
	}
	bool RouteWan::getEnable(){
		return enable;
	}
}

