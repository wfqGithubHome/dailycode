#include "timing.h"
#include "timingmgr.h"
#include "../global.h"

namespace maxnet{
	TimingMgr::TimingMgr()
    	{

	}

	TimingMgr::~TimingMgr()
    	{
#if 1
		Timing * timing = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			timing = (Timing *)node_list.at(i);
			if(!timing->isGroup())
				delete timing;
		}
		node_list.clear();
#endif
		return;
	}

	void TimingMgr::output(){
		Timing *timing = NULL;
		std::cout << "# Timing" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			timing = (Timing *)node_list.at(i);
			timing->output();
		}
	}

	 void TimingMgr::add(std::string name, std::string iface, std::string minute,
                std::string hour, std::string day, std::string month, std::string week, std::string comment){
			Timing *timing = new Timing();
			timing->setName(name);
			timing->setIface(iface);
			timing->setMinute(minute);
			timing->setHour(hour);
			timing->setDay(day);
			timing->setMonth(month);
			timing->setWeek(week);
			timing->setComment(comment);

			if(timing->isValid()){
				addNode(timing);
			}else{
				delete timing;
			}

			return;
	}

	
}
