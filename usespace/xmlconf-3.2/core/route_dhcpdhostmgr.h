#ifndef _ZERO_ROUTE_DHCPDHOSTMGR_H
#define _ZERO_ROUTE_DHCPDHOSTMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_dhcpdhost.h"

namespace maxnet{

	class RouteDhcpdHostMgr : public NodeMgr  {
		public:

			RouteDhcpdHostMgr();
			~RouteDhcpdHostMgr();
			void output();

			void add(std::string name, std::string dhcpname, std::string mac, std::string ip, 
				std::string mask, std::string gateway, std::string dnsname, std::string dns, 
				std::string leasetime, std::string maxleasetime, bool enable, std::string comment);
			
		private:

	};

}

#endif // _ZERO_VLANMGR_H
