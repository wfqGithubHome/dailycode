#ifndef __TIMING_H__
#define __TIMING_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{

	class Timing : public Node{

		public:

			Timing();
			~Timing();

                    bool isValid();
                    void output();

                    void setName(std::string name);
                    void setIface(std::string iface);
                    void setMinute(std::string minute);
                    void setHour(std::string hour);
                    void setDay(std::string day);
                    void setMonth(std::string month);
                    void setWeek(std::string week);
                    void setComment(std::string comment);

                    std::string getName();
                    std::string getIface();
                    std::string getMinute();
                    std::string getHour();
                    std::string getDay();
                    std::string getMonth();
                    std::string getWeek();
                    std::string getComment();
       
		private:
                    std::string name;
                    std::string iface;
                    std::string minute;
                    std::string hour;
                    std::string day;
                    std::string month;
                    std::string week;
                    std::string comment;
	};

}


#endif
