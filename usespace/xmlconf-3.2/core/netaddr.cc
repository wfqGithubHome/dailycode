#include "netaddr.h"
#include "../global.h"

namespace maxnet{

	NetAddr::NetAddr(){

	}

	NetAddr::~NetAddr(){
		
	}
	
	bool NetAddr::isVaild(){
		if(name.size() <= 0) return false;
		if(value.size() <=0) return false;
		return true;
	}

	std::string NetAddr::getCommand(){
		std::string command;
		command = getValue();
		if(command.size() <= 0){
			command = "";
		}

		std::string::size_type pos = command.find("-");
		if(pos != std::string::npos){
			command.replace(pos, 1, ":");
		}
		return command;
	}

	void NetAddr::output(){
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
							<< getValue() << "\"\t"
							//<< getRange()<< "\t\t"
							<< "// " << getComment() << std::endl;

		}

		return;

	}



	void NetAddr::setValue(std::string value){
		this->value = value;
	}


	void NetAddr::setRange(bool range){
		this->range = range;
	}

	std::string NetAddr::getValue(){
		return value;
	}

	bool NetAddr::getRange(){
		return range;
	}
	std::string NetAddr::getRangeString(){
		return range?"true":"false";
	}

}

