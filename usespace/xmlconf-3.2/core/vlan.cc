#include "vlan.h"
#include "../global.h"

namespace maxnet{

	Vlan::Vlan(){

	}

	Vlan::~Vlan(){
		
	}
	
	bool Vlan::isVaild(){
		if(name.size() <= 0) return false;
		if(vlans.size() <=0) return false;
		return true;
	}

	std::string Vlan::getCommand(){
		std::string command;
		command = "--vlan " + getVlans();
		return command;
	}

	void Vlan::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getVlans() << "\"\t\t//" << getComment() << std::endl;

		return;

	}



	void Vlan::setVlans(std::string vlans){
		this->vlans = vlans;
	}


	std::string Vlan::getVlans(){
		return vlans;
	}



}

