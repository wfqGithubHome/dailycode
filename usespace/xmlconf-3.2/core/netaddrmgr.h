#ifndef _ZERO_NETADDRMGR_H
#define _ZERO_NETADDRMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class NetAddrMgr : public NodeMgr  {
		public:

			NetAddrMgr(bool addDefault = true);
			~NetAddrMgr();
			void output();

			void add(std::string name, std::string value, bool range, std::string comment);
		private:



	};

}

#endif // _ZERO_NETADDRMGR_H
