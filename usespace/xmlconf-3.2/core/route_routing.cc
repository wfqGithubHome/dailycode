#include "route_routing.h"
#include "../global.h"

namespace maxnet{

	RouteRouting::RouteRouting(){

	}

	RouteRouting::~RouteRouting(){

	}
	
	bool RouteRouting::isVaild(){
		if(id.size() <= 0) return false;
		if(ifname.size() <= 0) return false;
		if(table.size() <= 0) return false;
		if(rule.size() <= 0) return false;
		return true;
	}

	std::string RouteRouting::getCommand(){
		std::string command;
		command = "--routing " + getGateway();
		return command;
	}

	void RouteRouting::output(){
		std::cout << "# \"" << getID() << "\"\t\""
							<< getDST() << "\"\t\""
							<< getGateway() << "\"\t\""
							<< getIFName() << "\"\t\""
							<< getTable() << "\"\t\""
							<< getRule() << "\"\t\""
							<< getEnable() << "\"\t\t//" << getComment() << std::endl;

		return;

	}


	void RouteRouting::setID(const std::string id){
		this->id = id;
	}

	void RouteRouting::setDST(const std::string dst){
		this->dst = dst;
	}
	
	void RouteRouting::setGateway(const std::string gateway){
		this->gateway = gateway;
	}
	
	void RouteRouting::setIFName(const std::string ifname){
		this->ifname = ifname;
	}

	void RouteRouting::setPriority(const std::string priority){
		this->priority = priority;
	}

	void RouteRouting::setTable(const std::string table){
		this->table = table;
	}
	
	void RouteRouting::setRule(const std::string rule){
		this->rule = rule;
	}
	
	void RouteRouting::setEnable(bool enable){
		this->enable = enable;
	}

	std::string RouteRouting::getID(){
		return id;
	}

	std::string RouteRouting::getDST(){
		return dst;
	}

	std::string RouteRouting::getGateway(){
		return gateway;
	}

	std::string RouteRouting::getIFName(){
		return ifname;
	}

	std::string RouteRouting::getPriority(){
		return priority;
	}
	
	std::string RouteRouting::getTable(){
		return table;
	}

	std::string RouteRouting::getRule(){
		return rule;
	}
	
	bool RouteRouting::getEnable(){
		return enable;
	}

}

