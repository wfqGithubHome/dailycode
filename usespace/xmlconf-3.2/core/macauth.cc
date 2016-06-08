#include "macauth.h"
#include "../global.h"

namespace maxnet{

	MacAuth::MacAuth(){

	}

	MacAuth::~MacAuth(){
		
	}
	
	bool MacAuth::isVaild(){
		if(name.size() <= 0) return false;
		if(value.size() <=0) return false;
		return true;
	}

	std::string MacAuth::getCommand(){
		std::string command;
		command = getValue();
		if(command.size() <= 0){
			command = "";
		}
#if 0
		std::string::size_type pos = command.find("-");
		if(pos != std::string::npos){
			command.replace(pos, 1, ":");
		}
#endif
		return command;
	}

	void MacAuth::output(){
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
							<< getValue() << "\"\t\t// " << getComment() << std::endl;

		}

		return;

	}

	void MacAuth::setValue(std::string value){
		this->value = value;
	}


	std::string MacAuth::getValue(){
		return value;
	}




}
