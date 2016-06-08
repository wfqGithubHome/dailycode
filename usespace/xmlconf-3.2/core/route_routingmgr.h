#ifndef _ZERO_ROUTE_ROUTINGMGR_H
#define _ZERO_ROUTE_ROUTINGMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_routing.h"

namespace maxnet{

	class RouteRoutingMgr : public NodeMgr  {
		public:

			RouteRoutingMgr();
			~RouteRoutingMgr();
			void output();		
                     int getBalanceFirstId();
			void add(std::string id, std::string dst, std::string gateway, std::string ifname,
				std::string priority, std::string table, std::string rule, bool enable, std::string comment);
		private:
                    bool hasbal;
                    int balfirstid;
	};

}

#endif // _ZERO_ROUTE_ROUTINGMGR_H
