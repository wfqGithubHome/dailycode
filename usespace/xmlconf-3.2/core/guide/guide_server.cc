#include "guide_server.h"

namespace maxnet
{

	Server::Server()
    {

	}

	Server::~Server()
    {
		
	}
	
	bool Server::isVaild()
    {
		return true;
	}
	
	void Server::setParentid(std::string parentid)
    {
		this->parentid = parentid;
	}
	
	void Server::setValue(std::string value)
    {
		this->value = value;
	}

	std::string Server::getParentid()
    {
		return parentid;
	}
	
	std::string Server::getValue()
    {
		return value;
	}

}

