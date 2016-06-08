#ifndef _ZERO_ROUTE_DNATMGR_H
#define _ZERO_ROUTE_DNATMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_dnat.h"

namespace maxnet{

	class RouteDNATMgr : public NodeMgr  {
		public:

			RouteDNATMgr();
			~RouteDNATMgr();
			void output();		
			
			void add(std::string id, bool enable, bool loop, std::string protocol_id, std::string comment, std::string wanip, std::string wan_port_before, 
						std::string wan_port_after, std::string lan_port_before,  std::string lan_port_after,std::string lan_ip,std::string ifname);
		private:

	};

}

#endif // _ZERO_ROUTE_DNATMGR_H
