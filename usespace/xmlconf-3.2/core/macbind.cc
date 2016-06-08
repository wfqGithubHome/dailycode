#include "macbind.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	MacBind::MacBind(){
		enable = false;
	}

	MacBind::~MacBind(){
		
	}
	
	bool MacBind::isVaild(){
		if(name.size() <= 0) return false;
		if(ipaddr.size() <=0) return false;
		if(macaddr.size() <=0) return false;
		
		return true;
	}

	void MacBind::output(){

	}

	void MacBind::setEnable(bool enable)
	{
		this->enable = enable;
	}

	void MacBind::setName(std::string name)
	{
		this->name= name;
	}

	void MacBind::setBridge(std::string bridge)
	{
		this->bridge= bridge;
	}
	void MacBind::setIpaddr(std::string ipaddr)
	{
		this->ipaddr= ipaddr;
	}

	void MacBind::setMacaddr(std::string macaddr )
	{
		this->macaddr= macaddr;
	}

	bool MacBind::getEnable()
	{
		return this->enable;
	}

	std::string MacBind::getName()
	{
		return this->name;
	}

	std::string MacBind::getBridge()
	{
		return this->bridge;
	}
	std::string MacBind::getIpaddr()
	{
		return this->ipaddr;
	}
	
	std::string MacBind::getMacaddr()
	{
		return this->macaddr;
	}
}
