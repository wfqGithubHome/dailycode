#include "route_table.h"
#include "../global.h"

namespace maxnet{

	RouteTable::RouteTable(){

	}

	RouteTable::~RouteTable(){
		
	}

	std::string RouteTable::getCommand(){
		std::string command = "";
		return command;
	}
	
	bool RouteTable::isVaild(){
		return true;
	}


	void RouteTable::output(){
		if(isGroup()){
			std::cout << "# \"" << getName() << "\"(group)\t\t// " << getComment() << std::endl;

			std::vector<std::string> sub_nodes = getSubNodes();

			for(unsigned int i = 0; i < sub_nodes.size() ; i++){
				std::cout << "#\t\"" << sub_nodes.at(i) << "\"" << std::endl;
			}
			std::cout << std::endl;
		}
		return;

	}
}

