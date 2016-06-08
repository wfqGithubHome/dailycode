#ifndef _ZERO_SERVINFOMGR_H
#define _ZERO_SERVINFOMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "servinfo.h"

namespace maxnet{


	class ServInfoMgr : public NodeMgr  {
		public:

			ServInfoMgr();
			~ServInfoMgr();

			void add(std::string type, std::string name, std::string en_name, 
                    std::string zonename, std::string en_zonename,
                    std::string var1, std::string var2, std::string var3, std::string var4, 
                    std::string comment);
	};

}

#endif

