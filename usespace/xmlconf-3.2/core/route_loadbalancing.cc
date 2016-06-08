#include "route_loadbalancing.h"
#include "../global.h"

namespace maxnet{

	RouteLoadBalancing::RouteLoadBalancing(){

	}

	RouteLoadBalancing::~RouteLoadBalancing(){
		
	}
	
	bool RouteLoadBalancing::isVaild(){
		if(name.size() <= 0) return false;
		if(ifname.size() <= 0) return false;
		if(rulename.size() <= 0) return false;
		if(weight.size() <= 0) return false;
		return true;
	}

	std::string RouteLoadBalancing::getCommand(){
		std::string command = "";
		std::string name = "";
		std::string ifname = "";
		std::string rulename = "";
		std::string weight = "";
		
		name = getName();
		ifname = getIFName();
		rulename = getRuleName();
		weight = getWeight();

		//command = name + " " + ifname " " + weight; 
		return command;
	}

	void RouteLoadBalancing::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getIFName() << "\"\t\""
							<< getRuleName() << "\"\t\""
							<< getWeight() << "\"\t\t//" << getComment() << std::endl;

		return;
	}

	
	void RouteLoadBalancing::setIFName(const std::string ifname){
		this->ifname = ifname;
	}

	void RouteLoadBalancing::setRuleName(const std::string rulename){
		this->rulename = rulename;
	}

	void RouteLoadBalancing::setWeight(const std::string weight){
		this->weight = weight;
	}

	std::string RouteLoadBalancing::getIFName(){
		return ifname;
	}
	
	std::string RouteLoadBalancing::getRuleName(){
		return rulename;
	}
	
	std::string RouteLoadBalancing::getWeight(){
		return weight;
	}

}

