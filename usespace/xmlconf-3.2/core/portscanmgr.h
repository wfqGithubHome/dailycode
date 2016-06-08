#ifndef _ZERO_PORTSCANMGR_H
#define _ZERO_PORTSCANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "portscan.h"

namespace maxnet{


	class PortScanMgr : public NodeMgr  {
		public:

			PortScanMgr();
			~PortScanMgr();
			void output();

			void add(std::string type, std::string newer, std::string pending,std::string interval);
		private:
            


	};

}

#endif // _ZERO_PORTSCANMGR_H

