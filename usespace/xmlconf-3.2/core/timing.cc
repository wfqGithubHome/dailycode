#include "timing.h"
#include "../global.h"
#include <sstream>

namespace maxnet{
	Timing::Timing(){
		
	}

	Timing::~Timing(){
		
	}

	bool Timing::isValid(){
		return true;
	}

	void Timing::output(){
		std::cout << "# \"" << getName() << "\"\t\"" << getIface() << "\"\t\""
			<< getMinute() << "\"\t\"" << getHour() << "\"\t\"" << getDay()
			<< "\"\t\"" << getMonth() << "\"\t\"" << getWeek() << "\"\t\"" << getComment() << std::endl;
		return;
	}

	void Timing::setName(std::string name){
		this->name = name;
	}

	void Timing::setIface(std::string iface){
		this->iface = iface;
	}

	void Timing::setMinute(std::string minute){
		this->minute = minute;
	}

	void Timing::setHour(std::string hour){
		this->hour = hour;
	}

	void Timing::setDay(std::string day){
		this->day = day;
	}
	
       void Timing::setMonth(std::string month){
		this->month = month;
	}
	   
       void Timing::setWeek(std::string week){
		this->week = week;
	}
	   
       void Timing::setComment(std::string comment){
		this->comment = comment;
	}

       std::string Timing::getName(){
		return name;
	}

	std::string Timing::getIface(){
		return iface;
	}
	
       std::string Timing::getMinute(){
		return minute;
	}
	   
       std::string Timing::getHour(){
		return hour;
	}
	   
       std::string Timing::getDay(){
		return day;
	}
	   
       std::string Timing::getMonth(){
		return month;
	}
	   
       std::string Timing::getWeek(){
		return week;
	}
	   
       std::string Timing::getComment(){
		return comment;
	}
	
}

