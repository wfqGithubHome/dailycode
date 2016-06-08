#ifndef _ZERO_ROUTE_DHCPD_H
#define _ZERO_ROUTE_DHCPD_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{
	class RouteDhcpd : public Node{
		public:
			RouteDhcpd();
			~RouteDhcpd();
			bool isVaild();
			std::string getCommand();
			void output();
			
			void setIfname(const std::string ifname);
			void setRange(const std::string range);
			void setMask(const std::string mask);
			void setGateway(const std::string gateway);
			void setDnsName(std::string dnsname);
			void setDNS(const std::string dns);
			void setLeaseTime(const std::string leasetime);
			void setMaxLeaseTime(const std::string maxleasetime);
			void setEnable(bool enable);

			std::string getIfname();
			std::string getRange();
			std::string getMask();
			std::string getGateway();
			std::string getDnsName();
			std::string getDNS();
			std::string getLeaseTime();
			std::string getMaxLeaseTime();
			bool		getEnable();

		private:
			std::string ifname;
			std::string range;
			std::string mask;
			std::string gateway;
			std::string dnsname;
			std::string dns;
			std::string leasetime;
			std::string maxleasetime;
			bool		enable;
	};

}

#endif // _ZERO_VLAN_H
