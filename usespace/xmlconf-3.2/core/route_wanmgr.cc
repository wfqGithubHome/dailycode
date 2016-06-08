#include "route_wan.h"
#include "route_wanmgr.h"
#include "../global.h"

namespace maxnet{

	RouteWanMgr::RouteWanMgr(){

	}

	RouteWanMgr::~RouteWanMgr(){
#if 1
		RouteWan * if_wan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			if_wan = (RouteWan *)node_list.at(i);
			delete if_wan;
		}
		node_list.clear();
#endif		
		return;
	}

	void RouteWanMgr::add(std::string id, std::string ifname, std::string access, std::string type, 
					std::string ip,std::string mask, std::string gateway, std::string username,
					std::string passwd,std::string servername, std::string mtu, std::string mac, 
					std::string dns,std::string up, std::string down, std::string isp, bool default_route,
					std::string time, std::string week, std::string day, bool enable){
		RouteWan * if_wan = new RouteWan();
		if_wan->setID(id);
		if_wan->setIfname(ifname);
		if_wan->setAccess(access);
		if_wan->setType(type);
		if_wan->setIP(ip);
		if_wan->setMask(mask);
		if_wan->setGateway(gateway);
		if_wan->setUsername(username);
		if_wan->setPasswd(passwd);
		if_wan->setServername(servername);
		if_wan->setMtu(mtu);
		if_wan->setMAC(mac);
		if_wan->setDNS(dns);
		if_wan->setUP(up);
		if_wan->setDown( down);
		if_wan->setISP( isp);
		if_wan->setDefaultRoute( default_route);
		if_wan->setEnable(enable);
		if_wan->setTime( time);
		if_wan->setWeek( week);
		if_wan->setDay(day);
		
		if(if_wan->isVaild()){
			addNode(if_wan);
		}
		else{
			delete if_wan;
		}
		return;
	}

	
	void RouteWanMgr::output(){
		bool sign = false;
		std::string str_ids = "";
		RouteWan * if_wan = NULL;
		std::cout  << "# interface wan " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			if_wan = (RouteWan *)node_list.at(i);
			if_wan->output();

			if(sign){
				str_ids += " ";
			}
			str_ids += if_wan->getID();
			sign = true;
		}

		std::cout << "ROUTE_WAN_IDS=\"" << str_ids << "\"" << std::endl;
		
		return;
	}
}

