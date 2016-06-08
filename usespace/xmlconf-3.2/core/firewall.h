#ifndef _ZERO_FIREWALL_H
#define _ZERO_FIREWALL_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Firewall : public Node{

		public:

			Firewall();
			~Firewall();
			bool isVaild();
			void output();

			void setProto(std::string proto);
			void setAction(std::string action);
			void setSrc(std::string src);
			void setDst(std::string dst);
			void setMac(std::string mac);
			void setVlan(std::string vlan);
			void setSchedule(std::string schedule);
			void setInTraffic(std::string in_traffic);
			void setOutTraffic(std::string out_traffic);
			void setSessionLimit(std::string session_limit);
			void setSrcInvent(bool invent);
			void setDstInvent(bool invent);
			void setMacInvent(bool invent);
			void setLogPacket(bool log_packet);
			void setBridgeId(std::string id);
			void setAuth(std::string auth);
//add new 
			void setActionID(int  action_id);
			void	setQuota(std::string quota);
			void setQuotaAction(std::string quota_action);
			void setSencondInTraffic(std::string second_in_traffic);
			void setSencondOutTraffic(std::string second_out_traffic);

			std::string getProto();
			std::string getAction();
			std::string getSrc();
			std::string getDst();
			std::string getMac();
			std::string getSchedule();
			std::string getInTraffic();
			std::string getOutTraffic();
			std::string getVlan();
			std::string getSessionLimit();
			std::string getAuth();

			int			getBridgeId();

			bool		getLogPacket();
			bool		getSrcInvent();
			bool		getDstInvent();
			bool		getMacInvent();
           	std::string getBridgeIdString();
//add new 			
			int 		  getActionID();
			std::string getQuota();
			std::string getQuotaAction();
			std::string getSencondInTraffic();
			std::string getSencondOutTraffic();


		private:
			std::string proto;
			std::string action;
			std::string auth;
			std::string src;
			int			bridge_id;
			bool		src_invent;
			std::string dst;
			bool		dst_invent;
			std::string mac;
			bool		mac_invent;
			std::string schedule;
			std::string vlan;
			bool		log_packet;
			std::string in_traffic;
			std::string out_traffic;
			std::string session_limit;

			int     action_id;
			std::string quota;
			std::string quota_action;
			std::string second_in_traffic;
			std::string second_out_traffic;

	};

}

#endif // _ZERO_FIREWALL_H
