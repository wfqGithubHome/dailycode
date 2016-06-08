#ifndef _ZERO_FIREWALLMGR_H
#define _ZERO_FIREWALLMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class FirewallMgr : public NodeMgr  {
		public:

			FirewallMgr();
			~FirewallMgr();
			void output();
			bool getEnable();
			void setEnable(bool enable);

			void setMacAuthEnable(bool status);
			bool getMacAuthEnable();

			bool getScanDetect();
			void setScanDetect(bool enable);

			bool getDosDetect();
			void setDosDetect(bool enable);

			void add(std::string name, std::string bridge_id, std::string proto, std::string auth, std::string action, std::string schedule, 
				std::string vlan,std::string src,std::string dst,std::string mac,std::string in_traffic, std::string out_traffic,
				std::string session_limit, bool log_packet,std::string quota_name,std::string quota_action,
				std::string second_in_traffic,std::string second_out_traffic,std::string comment,bool disabled);

		private:
			bool enable;
			bool mac_auth;
			bool scan_detect;
			bool dos_detect;

			int bridge_action_id[MAX_BRIDGE_ID];

	};

}

#endif // _ZERO_FIREWALLMGR_H
