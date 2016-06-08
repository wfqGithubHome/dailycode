#include "domainnode.h"
#include "../global.h"

namespace maxnet{

	DomainNode::DomainNode(){

	}

	DomainNode::~DomainNode(){
		
	}
	
	bool DomainNode::isVaild(){
		if(name.size() <= 0) return false;
		if(name.compare("any") != 0 && value.size() <=0) return false;
		return true;
	}

	std::string DomainNode::getCommand(){
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

	void DomainNode::output(){
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



	void DomainNode::setValue(std::string value){
		this->value = value;
	}


	std::string DomainNode::getValue(){
		return value;
	}

	void DomainNode::setID(uint32_t id){
		this->domain_id = id;
	}


	uint32_t DomainNode::getID(){
		return domain_id;
	}


}

