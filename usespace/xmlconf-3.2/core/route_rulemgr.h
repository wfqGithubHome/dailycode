#ifndef _ZERO_ROUTE_RULEMGR_H
#define _ZERO_ROUTE_RULEMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_rule.h"

namespace maxnet{

	class RouteRuleMgr : public NodeMgr  {
		public:

			RouteRuleMgr();
			~RouteRuleMgr();
			void output();		
			void add(std::string name, std::string src, std::string dst, std::string protocol, std::string src_port,
					std::string dst_port, std::string tos, std::string fwmark, std::string action, std::string comment);
		private:

	};

}

#endif // _ZERO_ROUTE_RULEMGR_H
