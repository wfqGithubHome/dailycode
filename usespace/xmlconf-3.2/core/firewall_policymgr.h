#ifndef __FIREWALL_POLICYMGR_H__
#define __FIREWALL_POLICYMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class FirewallPolicyMgr : public NodeMgr  {
		public:

			FirewallPolicyMgr();
			~FirewallPolicyMgr();
			void output();
            void setFirewallEnable(bool enable);
			void add(std::string id, bool enable, std::string src, std::string dst, std::string isp, std::string proto,std::string session,std::string iface, 
				std::string action,std::string timed,std::string week,std::string day,std::string in_limit, std::string out_limit,std::string comment);

            bool getFireWallEnable();
		private:
            bool firewall_enable;
	};

}

#endif // _ZERO_FIREWALLMGR_H
