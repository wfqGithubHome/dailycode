#include "schedule.h"
#include "../global.h"

namespace maxnet{

	Schedule::Schedule(){

	}

	Schedule::~Schedule(){
		
	}
	
	bool Schedule::isVaild(){
		if(name.size() <= 0) return false;
		if(start_time.size() <=0) return false;
		if(stop_time.size() <=0) return false;
		return true;
	}

	std::string Schedule::getCommand(){
		std::string command;
		command = "--time " + getStartTime() + "-" + getStopTime();
		if(getDays().size() > 2){
			command = command + " --days " + getDays();
		}
		return command;
	}

	void Schedule::output(){
		if(isGroup()){
			std::cout << "# \"" << getName() << "\"(group)\t\t// " << getComment() << std::endl;

			std::vector<std::string> sub_nodes = getSubNodes();

			for(unsigned int i = 0; i < sub_nodes.size() ; i++){
				std::cout << "#\t- \"" << sub_nodes.at(i) << "\"" << std::endl;
			}
			std::cout << std::endl;
		}
		else{
			std::cout << "# \"" << getName() << "\"\t\""
							<< getStartTime() << "\"\t\"" << getStopTime() << "\t\"" << getDays() << "\"\t\t// " << getComment() << std::endl;

		}

		return;

	}



	void Schedule::setStartTime(std::string start_time){
		this->start_time = start_time;
	}

	void Schedule::setStopTime(std::string stop_time){
		this->stop_time = stop_time;
	}

	void Schedule::setDays(std::string days){
		this->days = days;
	}

	std::string Schedule::getStartTime(){
		return start_time;
	}

	std::string Schedule::getStopTime(){
		return stop_time;
	}

	std::string Schedule::getDays(){
		return days;
	}



}

