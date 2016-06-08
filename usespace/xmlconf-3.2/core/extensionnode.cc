#include "extensionnode.h"
#include "../global.h"

namespace maxnet{

	ExtensionNode::ExtensionNode(){

	}

	ExtensionNode::~ExtensionNode(){
		
	}
	
	bool ExtensionNode::isVaild(){
		if(name.size() <= 0) return false;
		if(value.size() <=0) return false;
		return true;
	}

	void ExtensionNode::output(){
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
							<< getValue()  << "\t\t// "<< getComment() << std::endl;

		}
	}

	void ExtensionNode::setValue(std::string value){
		this->value = value;
	}

	std::string ExtensionNode::getValue(){
		return value;
	}

	/*void ExtensionNode::setCheck(bool arg_check){
		this->check = arg_check;
	}


	int ExtensionNode::getCheck(){
		return check?1:0;
	}*/

	void ExtensionNode::setID(uint32_t id){
		this->this_id = id;
	}

	uint32_t ExtensionNode::getID(){
		return this_id;
	}

}

