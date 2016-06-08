#include "routeserver.h"
#include "../global.h"

namespace maxnet{

	Routeserver::Routeserver(){
		enable = true;
		passwd_enable = true;
	}

	Routeserver::~Routeserver(){
		
	}
	
	bool Routeserver::isVaild(){
		return true;
	}

	void Routeserver::output(){
		std::cout << "#" << "ROUTESERVER INFO" << std::endl;
		if(this->getenable()){
			std::cout << "ROUTESERVER_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "ROUTESERVER_ENABLE='0'" << std::endl;
		}

		if(this->getpasswdenable()){
			std::cout << "ROUTESERVER_PASSWD_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "ROUTESERVER_PASSWD_ENABLE='0'" << std::endl;
		}
		return;
	}

	void Routeserver::setenable(bool enable){
		this->enable = enable;
	}

	void Routeserver::setpasswdenable(bool passwd_enable){
		this->passwd_enable = passwd_enable;
	}

	bool Routeserver::getenable(){
		return enable;
	}

	bool Routeserver::getpasswdenable(){
		return passwd_enable;
	}

}

