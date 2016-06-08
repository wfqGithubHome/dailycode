#include "session.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	Session::Session(){

	}

	Session::~Session(){
		
	}
	
	bool Session::isVaild(){
		if(getName().size() <= 0) return false;
		if(!getSessionLimitNum()) return false;
		return true;
	}

	std::string Session::getCommand(){
		std::string command = "";
		return command;
	}

	void Session::output(){
		std::cout << "# \"" << getName() << "\tsession_limit=\"" << getSession() << "\"" << std::endl;
		return;

	}



	void Session::setSession(std::string session){
		this->session = session;
	}

	std::string Session::getSession(){
		return session;
	}

	int	Session::getSessionLimitNum(){
		std::stringstream ss;
		ss << session;
		int ses = 0;
		ss >> ses;
		return ses;
	}
}


