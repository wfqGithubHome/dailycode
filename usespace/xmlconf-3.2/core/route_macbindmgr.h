#ifndef _ZERO_ROUTE_MACBINDMGR_H
#define _ZERO_ROUTE_MACBINDMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_macbind.h"

namespace maxnet{

	class RouteMACBindMgr : public NodeMgr  {
		public:

			RouteMACBindMgr();
			~RouteMACBindMgr();
			void output();	
			
			void setLearnt(bool learnt);
			
			bool getLearnt();
			
			void add(std::string id, std::string ip, std::string mac, std::string action,
						std::string bridge, std::string comment);
      //      void del(std::string ipaddr, std::string macaddr);
		private:
				bool learnt;
	};
}

#endif // _ZERO_ROUTE_MACBINDMGR_H

