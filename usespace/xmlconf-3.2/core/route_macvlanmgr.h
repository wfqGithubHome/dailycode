#ifndef __ROUTE_MACVLANMGR_H__
#define __ROUTE_MACVLANMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_macvlan.h"

namespace maxnet{

	class RouteMacVLanMgr : public NodeMgr  {
		public:

			RouteMacVLanMgr();
			~RouteMacVLanMgr();
			void output();

			void add(std::string name, std::string ifname,std::string mac,std::string comment);
		private:

	};

}

#endif // _ZERO_VLANMGR_H
