#ifndef _ZERO_SCHEDULEMGR_H
#define _ZERO_SCHEDULEMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class ScheduleMgr : public NodeMgr  {
		public:

			ScheduleMgr();
			~ScheduleMgr();
			void output();

			void add(std::string name, std::string start_time, std::string stop_time, std::string days, std::string comment);
		private:

			int parse_days_string(int *days, std::string days_string);
			int parse_day(int *days, int start, std::string days_string);

	};

}

#endif // _ZERO_SCHEDULEMGR_H
