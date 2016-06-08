#ifndef _ZERO_GLOBALIP_H
#define _ZERO_GLOBALIP_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class GlobalIP : public Node{

		public:

			GlobalIP();
			~GlobalIP();
			bool isVaild();
			void output();

			void setBridgeID(int bridge_id);
			void setBridgeIDbyString(std::string bridge_id);
			void setAddr(std::string addr);
            void setDst(std::string dst);
            void setDstIP_type(int dstip_type);
            void setdstiptypebyString(std::string dstip_type_string);
			void setPerIPSessionLimit(std::string limit);
			void setTotalSessionLimit(std::string limit);
			void setAction(std::string action);
			void setEnableHTTPLog(bool enable);
            void setEnableSessLog(bool enable);
            void setEnableAuth(bool enable);
			void setEnableHTTPDirPolicy(bool enable);
			void setEnableDnsHelperPolicy(bool enable);

			std::string getAddr();
            std::string getDst();
            int 		getDstIP_type();
			std::string getPerIPSessionLimit();
			std::string getTotalSessionLimit();
			std::string getAction();
			int			getBridgeID();
			bool		getEnableHTTPLog();
            bool		getEnableSessLog();
            bool		getEnableAuth();
			int			getEnableHTTPDirPolicy();
			int			getEnableDnsHelperPolicy();
		private:
			std::string addr;
            std::string dst;
			std::string per_ip_session_limit;
			std::string total_session_limit;
			std::string action;
			bool		enable_http_log;
            bool        enable_sess_log;
            bool        enable_auth;
            int         dstip_type;
			int			bridge_id;
			int 		enable_httpdirpolicy;
			int 		enable_dnshelperpolicy;
	};

}

#endif // _ZERO_GLOBALIP_H
