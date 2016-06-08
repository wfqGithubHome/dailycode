#ifndef _ZERO_ROUTE_LOADBMGR_H
#define _ZERO_ROUTE_LOADBMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_loadbalancing.h"


namespace maxnet{

	class RouteLoadBalancingMgr : public NodeMgr  {
		public:

			RouteLoadBalancingMgr();
			~RouteLoadBalancingMgr();
			void output();		
			void add(std::string name, std::string ifname, std::string rulename, std::string weight, std::string comment);
		private:

	};

}

#endif // _ZERO_ROUTE_LOADBMGR_H
