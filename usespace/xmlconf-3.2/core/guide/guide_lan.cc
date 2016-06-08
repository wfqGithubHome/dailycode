#include "guide_lan.h"

namespace maxnet
{

	Lan::Lan()
    {

	}

	Lan::~Lan()
    {
		
	}
	
	bool Lan::isVaild()
    {
		return true;
	}

    void Lan::setIfName(std::string ifname)
    {
		this->ifname = ifname;
	}
    
	void Lan::setMAC(std::string mac)
    {
		this->mac = mac;
	}
	
	void Lan::setIP(std::string ipaddr)
    {
		this->ipaddr = ipaddr;
	}

	void Lan::setMask(std::string netmask)
    {
		this->netmask = netmask;
	}

    std::string Lan::getIfName()
    {
        return ifname;
    }
    
	std::string Lan::getMAC()
    {
		return mac;
	}

	std::string Lan::getIP()
    {
		return ipaddr;
	}
	
	std::string Lan::getMask()
    {
		return netmask;
	}

}

