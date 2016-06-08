#include "quota.h"
#include "../global.h"

namespace maxnet{

	Quota::Quota(){

	}

	Quota::~Quota(){
		
	}
	
	bool Quota::isVaild(){
		if(name.size() <= 0) return false;
		return true;
	}
#if 0
	std::string Quota::getCommand(){
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
#endif
	void Quota::output(){
#if 0
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
#endif
		return;

	}

	//void Quota::setName(std::string name){
	//	this->name = name;
	//}
	
	void Quota::setBandwidthQuota(std::string bandwidth_quota){
		this->bandwidth_quota = bandwidth_quota;
	}
	void Quota::setIpType(std::string ip_type){
		this->ip_type = ip_type;
	}
	void Quota::setCycleType(std::string cycle_type){
		this->cycle_type = cycle_type;
	}

	void Quota::setQuotaID(int quotaid){
		this->quotaid = quotaid;
	}

	int Quota::getQuotaID(){
		return quotaid;
	}
	
	//std::string Quota::getName(){
	//	return name;
	//}
	
	std::string Quota::getBandwidthQuota(){
		return bandwidth_quota;
	}
	
	std::string Quota::getIpType(){
		return ip_type;
	}
	
	std::string Quota::getCycleType(){
		return cycle_type;
	}

}

