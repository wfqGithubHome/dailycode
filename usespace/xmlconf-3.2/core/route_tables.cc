#include "route_tables.h"
#include "../global.h"

namespace maxnet{

	RouteTables::RouteTables(){

	}

	RouteTables::~RouteTables(){
		
	}
	
	bool RouteTables::isVaild(){
		if(id.size() <= 0) return false;
		if(value.size() <= 0) return false;
		return true;
	}

	std::string RouteTables::getCommand(){
		std::string command;
		command = "--route_tables " + getId();
		return command;
	}

	void RouteTables::output(){
		std::cout << "ROUTE_TABLES_ID_" << getId() << "=\"" << getId() << "\"" << std::endl;
		std::cout << "ROUTE_TABLES_ENABLE_" << getId() << "=\"" << (getEnable()?1:0) << "\"" << std::endl;
		std::cout << "ROUTE_TABLES_VALUE_" << getId() << "=\"" << getValue() << "\"" << std::endl;

		return;
	}

	void RouteTables::setId(const std::string id){
		this->id = id;
	}
	void RouteTables::setEnable(bool enable){
		this->enable = enable;
	}
	void RouteTables::setValue(const std::string value){
		this->value = value;
	}

	std::string RouteTables::getId(){
		return id;
	}
	bool        RouteTables::getEnable(){
		return enable;
	}
	std::string RouteTables::getValue(){
		return value;
	}
}

