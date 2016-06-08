#ifndef _ZERO_ROUTE_SNATMGR_H
#define _ZERO_ROUTE_SNATMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_snat.h"

namespace maxnet{

	class RouteSNATMgr : public NodeMgr  {
		public:

			RouteSNATMgr();
			~RouteSNATMgr();
			void output();	
			
			void add(std::string id, std::string ifname, std::string src, std::string dst, std::string protocol, 
						std::string action, std::string dstip, std::string dstport, bool enable, std::string comment);
		private:

	};

}

#endif // _ZERO_ROUTE_SNATMGR_H

