#ifndef _MACAUTHMGR_H
#define _MACAUTHMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class MacAuthMgr : public NodeMgr  {
		public:

			MacAuthMgr();
			~MacAuthMgr();
			void output();

			void add(std::string name, std::string value, std::string comment);
		private:



	};

}

#endif 

