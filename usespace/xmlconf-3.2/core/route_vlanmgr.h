#ifndef _ZERO_ROUTE_VLANMGR_H
#define _ZERO_ROUTE_VLANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_vlan.h"

namespace maxnet{

	class RouteVLanMgr : public NodeMgr  {
		public:

			RouteVLanMgr();
			~RouteVLanMgr();
			void output();

			void add(std::string name, std::string ifname, std::string id,std::string mac,std::string comment);
		private:

	};

}

#endif // _ZERO_VLANMGR_H
