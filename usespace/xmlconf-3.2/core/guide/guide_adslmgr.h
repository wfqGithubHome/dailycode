#ifndef _ZERO_GUIDE_ADSLMGR_H
#define _ZERO_GUIDE_ADSLMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_adsl.h"

namespace maxnet
{

	class AdslMgr : public NodeMgr  
    {
		public:
			AdslMgr();
			~AdslMgr();

			void add(std::string ifname, std::string isp, std::string quality, std::string lineid, 
                std::string upload, std::string download, std::string username, std::string password);
		private:

	};

}

#endif 
