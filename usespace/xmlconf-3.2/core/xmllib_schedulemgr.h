#ifndef _ZERO_XMLLIB_SCHEDULEMGR_H
#define _ZERO_XMLLIB_SCHEDULEMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class Xmllib_ScheduleMgr : public NodeMgr  {
		public:

			Xmllib_ScheduleMgr();
			~Xmllib_ScheduleMgr();
			void output();

			void add(std::string name, std::string start_time, std::string stop_time, std::string days, std::string comment);
	};

}

#endif // _ZERO_XMLLIB_SCHEDULEMGR_H
