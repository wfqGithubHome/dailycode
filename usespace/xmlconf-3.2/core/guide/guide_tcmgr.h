#ifndef _ZERO_GUIDE_TCMGR_H
#define _ZERO_GUIDE_TCMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_tc.h"


namespace maxnet
{
	class TCMgr : public NodeMgr
    {
		public:
			TCMgr();
			~TCMgr();
			
			void add(std::string name, std::string lineid, std::string isp, std::string quality, 
                std::string balance, std::string upload, std::string download, std::string enable);
		private:

	};

}

#endif 

