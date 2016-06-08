#ifndef _ZERO_GUIDE_DEVICEZONEMGR_H
#define _ZERO_GUIDE_DEVICEZONEMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_zone.h"


namespace maxnet
{
    class ZoneMgr : public NodeMgr  
    {
		public:

			ZoneMgr();
			~ZoneMgr();
			
			void add(std::string type, std::string oem, std::string isp);
		private:

	};

}

#endif 

