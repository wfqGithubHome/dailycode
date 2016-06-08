#ifndef _ZERO_ROUTE_WAN_H
#define _ZERO_ROUTE_WAN_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteWan : public Node{

		public:

			RouteWan();
			~RouteWan();
			bool isVaild();
			std::string getCommand();
			void output();

            void setID(const std::string id);
			void setIfname(const std::string ifname);
            void setAccess(const std::string access);
			void setType(const std::string type);	
			void setIP(const std::string ip);
			void setMask(const std::string mask);
            void setGateway(const std::string gateway);
			void setUsername(const std::string username);
            void setPasswd(const std::string passwd);
            void setServername(const std::string servername);
            void setMtu(const std::string mtu);
			void setMAC(std::string mac);
            void setDNS(std::string dns);
            void setUP(std::string up);
            void setDown(std::string down);
            void setISP(std::string isp);
            void setDefaultRoute(bool default_route);
            void setTime(std::string time);
            void setWeek(std::string week);
            void setDay(std::string day);
            void setEnable(bool enable);

			std::string getID();
			std::string getIfname();
            std::string getAccess();
			std::string getType();
			std::string getIP();
			std::string getMask();
			std::string getGateway();
			std::string getUsername();
			std::string getPasswd();
            std::string getServername();
			std::string getMtu();
            std::string getMac();
			std::string getDNS();
            std::string getUp();
			std::string getDown();
            std::string getIsp();
            bool        getDefaultRoute();
            std::string getTime();
			std::string getWeek();
            std::string getDay();
            bool        getEnable();

		private:
            std::string id;
            std::string ifname;
            std::string access;
            std::string type;
            std::string ip;
            std::string mask;
            std::string gateway;
            std::string username;
            std::string passwd;
            std::string servername;
            std::string mtu;
            std::string mac;
            std::string dns;
            std::string up;
            std::string down;
            std::string isp;
            bool        default_route;
            bool        enable;
            std::string time;
            std::string week;
            std::string day;
	};

}

#endif // _ZERO_ROUTE_WAN_H
