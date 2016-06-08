#include "protocol.h"
#include "../global.h"

namespace maxnet{

	Protocol::Protocol(){
		id = 0;
	}

	Protocol::~Protocol(){
		
	}
	
	bool Protocol::isVaild(){
		if(getName().size() <= 0) return false;
		if(getProto().size() <=0) return false;
		return true;
	}

	std::string Protocol::getCommand(){
		return this->getMatch();
	}

	void Protocol::output(){
		if(isGroup()){
			std::cout << "# \"" << getName() << "\"(group)\t\t// " << getComment() << std::endl;

			std::vector<std::string> sub_nodes = getSubNodes();

			for(unsigned int i = 0; i < sub_nodes.size() ; i++){
				std::cout << "#\t- \"" << sub_nodes.at(i) << "\"" << std::endl;
			}
			std::cout << std::endl;
		}
		else{
			std::cout << "# \"" << getName() << "\tproto=\"" << getProto() << "\", dport=\"" << getDPort() << "\", sport=\"" << getSPort() << "\", match=\"" << getMatch() << "\", option=\"" << getOption() << "\"\t\t// " << getComment() << std::endl;

		}

		return;

	}

	void Protocol::setId( int id){
		if(id < 0){
			this->id = 0;
		}
		else{
			this->id = id;
		}
	}

	void Protocol::setProto(std::string protocol){
		this->protocol = protocol;
	}

	void Protocol::setSPort(std::string sport){
		this->sport = sport;
	}

	void Protocol::setDPort(std::string dport){
		this->dport = dport;
	}

	void Protocol::setMatch(std::string match){
		this->match = match;
	}
	void Protocol::setType(std::string type){
		this->type = type;
	}

	void Protocol::setOption(std::string option){
		this->option = option;
	}

	int Protocol::getId(){
		return this->id;
	}

	std::string Protocol::getProto(){
		return protocol;
	}

	std::string Protocol::getSPort(){
		return sport;
	}

	std::string Protocol::getDPort(){
		return dport;
	}

	std::string Protocol::getMatch(){
		return match;
	}
	std::string Protocol::getType(){
		return type;
	}

	std::string Protocol::getOption(){
		return option;
	}


}

