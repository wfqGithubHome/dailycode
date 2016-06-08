#ifndef _ZERO_ROUTE_ARPBINDMGR_H
#define _ZERO_ROUTE_ARPBINDMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_arpbind.h"

namespace maxnet{

	class RouteARPBindMgr : public NodeMgr  {
		public:

			RouteARPBindMgr();
			~RouteARPBindMgr();
			void output();	
			
			void setAutoARPBind(bool autobind);
			void setCronMinute(std::string minute);
			
			bool getAutoARPBind();
			std::string getCronMinute();
			
			
			void add(std::string id, std::string ip, std::string mac, std::string status,
						std::string type, bool enable, std::string comment);
		private:
				bool	autobind;
				std::string minute;

	};

}

#endif // _ZERO_ROUTE_ARPBINDMGR_H

