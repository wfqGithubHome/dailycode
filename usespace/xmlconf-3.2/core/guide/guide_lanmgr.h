#ifndef _ZERO_GUIDE_LANMGR_H
#define _ZERO_GUIDE_LANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_lan.h"


namespace maxnet{

	class LanMgr : public NodeMgr  {
		public:

			LanMgr();
			~LanMgr();
			
			void add(std::string ifname, std::string mac, std::string ipaddr, std::string netmask);
		private:

	};

}

#endif 




























