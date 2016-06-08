#ifndef _ZERO_INTERFACEMGR_H
#define _ZERO_INTERFACEMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class InterfaceMgr : public NodeMgr  {
		public:

			InterfaceMgr();
			~InterfaceMgr();

			void output();
			
			void add(std::string name, std::string mode, std::string speed, 
				std::string duplex, std::string linkstate, std::string bridgeid, std::string comment);

	};

}

#endif // _ZERO_INTERFACEMGR_H

