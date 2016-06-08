#include "schedule.h"
#include "xmllib_schedulemgr.h"
#include "../global.h"

namespace maxnet{

	Xmllib_ScheduleMgr::Xmllib_ScheduleMgr(){

	}

	Xmllib_ScheduleMgr::~Xmllib_ScheduleMgr(){
#if 1
		Schedule * sche = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			sche = (Schedule *)node_list.at(i);
			if(!sche->isGroup())
			delete sche;
		}
		node_list.clear();
#endif
		return;
	}

	
	void Xmllib_ScheduleMgr::add(std::string name, std::string start_time, std::string stop_time, std::string days, std::string comment){
	
		Schedule * sche = new Schedule();
		sche->setName(name);
		sche->setStartTime(start_time);
		sche->setStopTime(stop_time);
		sche->setDays(days);
		sche->setComment(comment);
		sche->setGroup(false);
		if(sche->isVaild()){
			addNode(sche);
		}
		else{
			delete sche;
		}
		
		return;
	}

	void Xmllib_ScheduleMgr::output(){
		Schedule * sche = NULL;
		std::cout  << "# schedules and schedules group"<< std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			sche = (Schedule *)node_list.at(i);
			sche->output();
		}
		

		
		return;

	}
}

