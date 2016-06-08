#ifndef __ROUTE_INTERFACELINKMGR_H__
#define __ROUTE_INTERFACELINKMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_interfacelink.h"

namespace maxnet{


	class Route_InterfaceLinkMgr : public NodeMgr  {
		public:

			Route_InterfaceLinkMgr();
			~Route_InterfaceLinkMgr();

			void output();
			
			void add(std::string ifname,  std::string type,std::string mode, std::string speed, 
				std::string duplex, std::string linkstate);

	};

}

#endif // _ZERO_INTERFACEMGR_H

