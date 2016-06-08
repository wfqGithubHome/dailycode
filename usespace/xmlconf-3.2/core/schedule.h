#ifndef _ZERO_SCHEDULE_H
#define _ZERO_SCHEDULE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Schedule : public Node{

		public:

			Schedule();
			~Schedule();
			bool isVaild();
			std::string getCommand();

			void output();

			void setStartTime(std::string start_time);
			void setStopTime(std::string stop_time);
			void setDays(std::string days);

			std::string getStartTime();
			std::string getStopTime();
			std::string getDays();


		private:
			std::string start_time;
			std::string stop_time;
			std::string days;


	};

}

#endif // _ZERO_SCHEDULE_H
