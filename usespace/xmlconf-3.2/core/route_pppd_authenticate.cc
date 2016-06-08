#include "route_pppd_authenticate.h"
#include "../global.h"

namespace maxnet{

	RoutePPPDAuthenticate::RoutePPPDAuthenticate(){

	}

	RoutePPPDAuthenticate::~RoutePPPDAuthenticate(){
		
	}
	
	bool RoutePPPDAuthenticate::isVaild(){
		return true;
	}

	void	RoutePPPDAuthenticate::output(){
		std::cout << "# pppd authenticate" << std::endl;
		std::cout << "ROUTE_PPPD_PAP=\"" << this->getRequirePAP() << "\"" << std::endl;
		std::cout << "ROUTE_PPPD_CHAP=\"" << this->getRequireCHAP() << "\"" << std::endl;
		std::cout << "ROUTE_PPPD_MSCHAP=\"" << this->getRequireMSCHAP() << "\"" << std::endl;
		std::cout << "ROUTE_PPPD_MSCHAPV2=\"" << this->getRequireMSCHAPV2() << "\"" << std::endl;
		std::cout << "ROUTE_PPPD_EAP=\"" << this->getRequireEAP() << "\"" << std::endl;

		return;
	}


	void	RoutePPPDAuthenticate::setRequirePAP(bool require_pap){
		this->require_pap = require_pap;
	}

	void	RoutePPPDAuthenticate::setRequireCHAP(bool require_chap){
		this->require_chap = require_chap;
	}

	void	RoutePPPDAuthenticate::setRequireMSCHAP(bool require_mschap){
		this->require_mschap = require_mschap;
	}

	void	RoutePPPDAuthenticate::setRequireMSCHAPV2(bool require_mschapv2){
		this->require_mschapv2 = require_mschapv2;
	}

	void	RoutePPPDAuthenticate::setRequireEAP(bool require_eap){
		this->require_eap = require_eap;
	}


	bool	RoutePPPDAuthenticate::getRequirePAP(){
		return require_pap;
	}

	bool	RoutePPPDAuthenticate::getRequireCHAP(){
		return require_chap;
	}
	
	bool	RoutePPPDAuthenticate::getRequireMSCHAP(){
		return require_mschap;
	}

	bool	RoutePPPDAuthenticate::getRequireMSCHAPV2(){
		return require_mschapv2;
	}
	
	bool	RoutePPPDAuthenticate::getRequireEAP(){
		return require_eap;
	}
}

