#ifndef _ZERO_GUIDE_SERVERMGR_H
#define _ZERO_GUIDE_SERVERMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_server.h"


namespace maxnet
{
	class ServerMgr : public NodeMgr  
    {
		public:
			ServerMgr();
			~ServerMgr();
			
			void add(std::string parentid, std::string value);
		private:

	};

}

#endif 




























