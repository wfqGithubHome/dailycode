#include "interface.h"
#include "../global.h"

namespace maxnet{

	Interface::Interface(){

	}

	Interface::~Interface(){
		
	}

	void Interface::output(){

		std::string name = this->getName();
		
        if(this->getlinkstate().compare("3")==0){
			std::cout << "DEVICE_MANAGE_PORT=" << name << std::endl;
        }
		else if(this->getlinkstate().compare("1") == 0){
			std::cout << "BRIDGE_INT" << this->getbridgeid() << "=" << name << std::endl;
		}
		else if(this->getlinkstate().compare("2") == 0){
			std::cout << "BRIDGE_EXT" << this->getbridgeid() << "=" << name << std::endl;
		}
		
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		std::cout << "INTERFACE_" << name << "_MODE=\"" << this->getMode() << "\"" << std::endl;
		std::cout << "INTERFACE_" << name << "_SPEED=\"" << this->getSpeed() << "\"" << std::endl;
		std::cout << "INTERFACE_" << name << "_DUPLEX=\"" << this->getDuplex() << "\"" << std::endl;

		return;
	}
	
	bool Interface::isVaild(){
		
		return true;
	}

	void	Interface::setlinkstate(const std::string	 linkstate){
		this->linkstate = linkstate;
	}

    void	Interface::setbridgeid(const std::string	 bridgeid){
		this->bridgeid = bridgeid;
	}

	void	Interface::setMode(const std::string	 mode){
		this->mode = mode;
	}

	void	Interface::setSpeed(const std::string speed){
		this->speed = speed;
	}

	void	Interface::setDuplex(const std::string duplex){
		this->duplex = duplex;
	}

	std::string 	Interface::getlinkstate(){
		return this->linkstate;
	}

	std::string 	Interface::getbridgeid(){
		return this->bridgeid;
	}
	std::string 	Interface::getMode(){
		return this->mode;
	}

	std::string 	Interface::getSpeed(){
		return this->speed;
	}

	std::string 	Interface::getDuplex(){
		return this->duplex;
	}

}

