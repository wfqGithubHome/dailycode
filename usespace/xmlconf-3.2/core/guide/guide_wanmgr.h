#ifndef _ZERO_GUIDE_WANMGR_H
#define _ZERO_GUIDE_WANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_wan.h"

namespace maxnet
{

	class WanMgr : public NodeMgr 
   {
		public:

			WanMgr();
			~WanMgr();
			
			void add(std::string ifname, std::string isp, std::string quality, std::string lineid,   
                       std::string upload, std::string download, std::string mac, std::string ipaddr,
				       std::string netmask, std::string gateway, std::string conntype);
		private:

	};

}

#endif 
