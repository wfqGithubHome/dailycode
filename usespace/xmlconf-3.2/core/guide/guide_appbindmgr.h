#ifndef _ZERO_GUIDE_APPBINDMGR_H
#define _ZERO_GUIDE_APPBINDMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"
#include "guide_appbind.h"

namespace maxnet
{
	class AppbindMgr : public NodeMgr  
    {
		public:

			AppbindMgr();
			~AppbindMgr();
			
			void add(std::string name, std::string apptype, std::string bindtype, std::string bindlineid,
					  std::string bypass, std::string maxupload, std::string maxdownload, std::string weight);
		private:
				 
	};
}

#endif 

