#ifndef _ZERO_ROUTE_LANMGR_H
#define _ZERO_ROUTE_LANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_lan.h"

namespace maxnet{

	class RouteLanMgr : public NodeMgr  {
		public:

			RouteLanMgr();
			~RouteLanMgr();
			void output();

            void setLanIp(std::string ip);
            void setLanMask(std::string mask);
            void setLanMac(std::string mac);
            void setLanIfname(std::string ifname);
            void setLanDhcp(std::string dhcp);
            void setDhcpStartip(std::string startip);
            void setDhcpEndip(std::string endip);
            void setDhcpLeaseTime(std::string lease_time);
            void setDhcpGateway(std::string gateway);
            void setDhcpMask(std::string dhcp_mask);
            void setDhcpFirstDns(std::string first_dns);
            void setDhcpSecondDns(std::string second_dns);
            void setSubnetEnable(bool subnet_enable);

            std::string getLanIp();
            std::string getLanMask();
            std::string getLanMac();
            std::string getLanIfname();
            std::string getLanDhcp();
            std::string getDhcpStartip();
            std::string getDhcpEndip();
            std::string getDhcpLeaseTime();
            std::string getDhcpGateway();
            std::string getDhcpMask();
            std::string getDhcpFirstDns();
            std::string getDhcpSecondDns();
            bool        getSubnetEnable();
			
			void subnetadd( std::string ip, std::string mask);
            int getsubnetdatacount();
            std::vector<RouteLan *> getlansubnetlist();
		private:
            std::string ip;
            std::string mask;
            std::string mac;
            std::string ifname;
            std::string dhcp;
            std::string startip;
            std::string endip;
            std::string lease_time;
            std::string gateway;
            std::string dhcp_mask;
            std::string first_dns;
            std::string second_dns;
            bool        subnet_enable;

            std::vector<RouteLan *> lan_subnet_list;
            int subnetdatacount;

	};

}

#endif // _ZERO_VLANMGR_H
