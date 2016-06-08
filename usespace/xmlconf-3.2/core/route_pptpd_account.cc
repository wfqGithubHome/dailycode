#include "route_pptpd_account.h"
#include "../global.h"

namespace maxnet{

	PptpdAccount::PptpdAccount(){
		username = "";
		passwd = "";
		ip = "";
	}

	PptpdAccount::~PptpdAccount(){
		
	}

	bool PptpdAccount::isValid(){
		if(username.size() <= 0) return false;
		if(passwd.size() <= 0) return false;
		return true;
	}

	void PptpdAccount::output(){
		
	}

	void PptpdAccount::setusername(std::string username){
		this->username = username;
	}

	void PptpdAccount::setpasswd(std::string passwd){
		this->passwd = passwd;
	}

	void PptpdAccount::setip(std::string ip){
		this->ip = ip;
	}

	std::string PptpdAccount::getusername(){
		return username;
	}

	std::string PptpdAccount::getpasswd(){
		return passwd;
	}

	std::string PptpdAccount::getip(){
		return ip;
	}

}
