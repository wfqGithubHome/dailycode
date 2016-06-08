#ifndef _ZERO_ROUTE_DHCPDHOST_H
#define _ZERO_ROUTE_DHCPDHOST_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{
	class RouteDhcpdHost : public Node{

		public:

			RouteDhcpdHost();
			~RouteDhcpdHost();
			bool isVaild();
			std::string getCommand();
			void output();
			
			void setDhcpName(const std::string dhcpname);
			void setMAC(const std::string mac);
			void setIP(const std::string ip);
			void setMask(const std::string mask);
			void setGateway(const std::string gateway);
			void setDnsName(std::string dnsname);
			void setDNS(const std::string dns);
			void setLeaseTime(const std::string leasetime);
			void setMaxLeaseTime(const std::string maxleasetime);
			void setEnable(bool enable);

			std::string getDhcpName();
			std::string getMAC();
			std::string getIP();
			std::string getMask();
			std::string getGateway();
			std::string getDnsName();
			std::string getDNS();
			std::string getLeaseTime();
			std::string getMaxLeaseTime();
			bool		getEnable();

		private:
			std::string dhcpname;
			std::string mac;
			std::string ip;
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
