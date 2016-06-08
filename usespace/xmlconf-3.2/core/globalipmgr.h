#ifndef _ZERO_GLOBALIPMGR_H
#define _ZERO_GLOBALIPMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class GlobalIPMgr : public NodeMgr  {
		public:

			GlobalIPMgr();
			~GlobalIPMgr();

			void output();
			int	 getMaxBridgeID();
			void setTotalSessionLimit(std::string bridge_id, std::string limit, std::string overhead);

			int getTotalSessionLimitNum(int bridge_id);
			int getTotalSessionLimitOverhead(int bridge_id);
			std::string getTotalSessionLimitOverheadString(int bridge_id);
			
			void add(std::string name, std::string bridge_id, std::string addr, std::string dst,
				std::string per_ip_session_limit, std::string total_session_limit, std::string action, 
				bool httplog,bool dnshelperpolicy,  bool httpdirpolicy,bool sesslog, bool enable_auth,
				std::string dst_type, std::string comment);

		private:
			int  getBridgeIDbyName(std::string bridge_id_string);
			int total_session_limit_overhead[10];
			std::string total_session_limit_overhead_string[10];
			int total_session_limit_num[10];
			static int	max_bridge_id;
	};

}

#endif // _ZERO_GLOBALIPMGR_H
