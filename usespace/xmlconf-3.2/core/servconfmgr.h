#ifndef _ZERO_SERVCONFMGR_H
#define _ZERO_SERVCONFMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "servconf.h"

namespace maxnet{


	class ServConfMgr : public NodeMgr  {
		public:

			ServConfMgr();
			~ServConfMgr();
            
			void add(std::string type, std::string en_name, std::string en_zonename);        
	};

}

#endif

