#include "schedule.h"
#include "schedulemgr.h"
#include "../global.h"

namespace maxnet{

	ScheduleMgr::ScheduleMgr(){

	}

	ScheduleMgr::~ScheduleMgr(){
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

	/* return 1->ok, return 0->error */
	int	ScheduleMgr::parse_day(int *days, int start, std::string days_string)
	{
		std::string day;
		char *days_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
		unsigned short int days_of_week[7] = {1, 2, 4, 8, 16, 32, 64};
		unsigned int i;

		day = days_string.substr(start, 3);
		for (i=0; i< 7; i++){
			if (day.compare(days_str[i]) == 0){
				*days |= days_of_week[i];
				return 0;
			}
		}
		/* if we are here, we didn't read a valid day */
		return -1;
	}

	int ScheduleMgr::parse_days_string(int *days, std::string days_string)
	{
		int len;
		int i=0;

		len = days_string.size();
		if(len < 3)
			return -1;

		while(i < len)
		{
			if (parse_day(days, i, days_string) == -1)
				return -1;
			i += 4;
		}
		return 0;
	}
	
	void ScheduleMgr::add(std::string name, std::string start_time, std::string stop_time, std::string days, std::string comment){
//		std::cout << name << ": " << days << std::endl;
		if(days.size() > 0){
			int days_num = 0;
			if(parse_days_string(&days_num, days) == -1){
				return;
			}
			addGroup(name, "GROUP");

			int min_day = 0;
			int max_day = 0;
			char *days_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
			unsigned short int days_of_week[7] = {1, 2, 4, 8, 16, 32, 64};
			
			// get min and max day
			for(int i = 0; i < 7; i++){
//				std::cout << days_num << ":" << (days_num & days_of_week[i]) << ":" << days_of_week[i] << std::endl;
				if((days_num & days_of_week[i]) == days_of_week[i]){

					if(min_day == 0){
						min_day = i + 1;
					}
					max_day = i + 1;

				}
			}
//			std::cout << "min_day: " << min_day << "max_day: " << max_day << std::endl;
			
			int first_day = 0;
			for(int i = 0; i < 7; i++)
			{
				if((days_num & days_of_week[i]) == days_of_week[i])
				{
					if(first_day == 0)
					{
						first_day = i + 1;
//						std::cout << "first_day: " << first_day << std::endl;
					}
				}
				else
				{
					if(first_day > 0)
					{
						std::string sche_name = name + days_str[first_day - 1];
						Schedule * sche = new Schedule();
						sche->setName(sche_name);
						sche->setStartTime(start_time);
						sche->setStopTime(stop_time);
						
						if(first_day - 1 == i - 1)
						{
							sche->setDays(days_str[first_day - 1]);
						}
						else
						{
							std::string days_temp = days_str[first_day - 1];
							days_temp = days_temp + ":";
							days_temp = days_temp + days_str[i - 1];
							sche->setDays(days_temp);
						}
						sche->setComment(comment);
						sche->setGroup(false);
						if(sche->isVaild()){
							addNode(sche);
							addSubNode(name, name + days_str[first_day - 1]);
						}
						else{
							delete sche;
						}
						
						first_day = 0;
					}
				}
			}

			if(max_day == 7){
				std::string sche_name = name + days_str[first_day - 1];
				Schedule * sche = new Schedule();
				sche->setName(sche_name);
				sche->setStartTime(start_time);
				sche->setStopTime(stop_time);
				if(first_day == max_day){
					sche->setDays(days_str[max_day - 1]);
				}
				else{
					std::string days_temp = days_str[first_day - 1];
					days_temp = days_temp + ":";
					days_temp = days_temp + days_str[max_day - 1];
					sche->setDays(days_temp);
				}

				sche->setComment(comment);
				sche->setGroup(false);
				if(sche->isVaild()){
					addNode(sche);
					addSubNode(name, sche_name);
				}
				else{
					delete sche;
				}

			}

		}
		else{
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
		}
		return;
	}

	void ScheduleMgr::output(){
		Schedule * sche = NULL;
		std::cout  << "# schedules and schedules group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			sche = (Schedule *)node_list.at(i);
			sche->output();
		}
		

		
		return;

	}
}

