#include "guide_wan.h"

namespace maxnet
{

	Wan::Wan()
    {

	}

	Wan::~Wan()
    {
		
	}
	
	bool Wan::isVaild()
    {
        return true;
	}

	void Wan::setIfname(std::string ifname)
    {
		this->ifname = ifname;
	}
	void Wan::setISP(std::string isp)
    {
		this->isp = isp;
	}
	void Wan::setQuality(std::string quality)
    {
		this->quality = quality;
	}

	void Wan::setLineid(std::string lineid)
    {
		this->lineid = lineid;
	}
	
	void Wan::setUpload(std::string upload)
    {
		this->upload = upload;
	}

	void Wan::setDownload(std::string download)
    {
		this->download = download;
	}

	void Wan::setMAC(std::string mac)
    {
		this->mac = mac;
	}

	void Wan::setIPaddr(std::string ipaddr)
    {
		this->ipaddr = ipaddr;
	}

	void Wan::setNetmask(std::string netmask)
    {
		this->netmask = netmask;
	}

    void Wan::setGateway(std::string gateway){
		this->gateway = gateway;
	}

    void Wan::setConntype(std::string conntype)
    {
		this->conntype = conntype;
	}
    
	std::string Wan::getIfname()
    {
		return ifname;
	}
	std::string Wan::getISP()
    {
		return isp;
	}
	std::string Wan::getQuality()
    {
		return quality;
	}
	
	std::string Wan::getLineid()
    {
		return lineid;
	}

	std::string Wan::getUpload()
    {
		return upload;
	}
	
	std::string Wan::getDownload()
    {
		return download;
	}

	std::string Wan::getMAC()
    {
		return mac;
	}

	std::string Wan::getIPaddr()
    {
		return ipaddr;
	}

	std::string Wan::getNetmask()
    {
		return netmask;
	}

    std::string Wan::getGateway()
    {
		return gateway;
	}

    std::string Wan::getConntype()
    {
		return conntype;
	}
    
}

