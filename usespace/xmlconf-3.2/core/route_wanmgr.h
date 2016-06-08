#ifndef _ZERO_ROUTE_WANMGR_H
#define _ZERO_ROUTE_WANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_wan.h"

namespace maxnet{

	class RouteWanMgr : public NodeMgr  {
		public:

			RouteWanMgr();
			~RouteWanMgr();
			void output();

			void add(std::string id, std::string ifname,std::string access ,std::string type, std::string ip,
					std::string mask, std::string gateway, std::string username,std::string passwd,
					std::string servername, std::string mtu, std::string mac, std::string dns,
					std::string up, std::string down, std::string isp, bool default_route, std::string time,
					std::string week, std::string day, bool enable);
		private:

	};

}

#endif // _ZERO_VLANMGR_H
