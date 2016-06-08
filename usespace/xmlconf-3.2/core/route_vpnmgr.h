#ifndef _ZERO_ROUTE_VPNMGR_H
#define _ZERO_ROUTE_VPNMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_vpn.h"

namespace maxnet{

	class RouteVPNClientMgr : public NodeMgr  {
		public:

			RouteVPNClientMgr();
			~RouteVPNClientMgr();
			void output();

			void add(std::string name, std::string ifname, std::string username,
				std::string password, std::string servername, std::string mtu, std::string weight,
				bool defaultroute, bool peerdns, bool enable, std::string comment);
		private:

	};

}

#endif // _ZERO_VLANMGR_H
