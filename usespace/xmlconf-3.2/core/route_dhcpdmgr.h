#ifndef _ZERO_ROUTE_DHCPDMGR_H
#define _ZERO_ROUTE_DHCPDMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_dhcpd.h"

namespace maxnet{
	class RouteDhcpdMgr : public NodeMgr  {
		public:
			RouteDhcpdMgr();
			~RouteDhcpdMgr();
			void output();

			void setDhcpdEnable(bool dhcpd_enable);
			bool getDhcpdEnable();

			void add(std::string name, std::string ifname, std::string range, std::string mask,
				std::string gateway, std::string dnsname, std::string dns, std::string leasetime,
				std::string maxleasetime, bool enable, std::string comment);
			
		private:
			bool dhcpd_enable;

	};

}

#endif // _ZERO_VLANMGR_H
