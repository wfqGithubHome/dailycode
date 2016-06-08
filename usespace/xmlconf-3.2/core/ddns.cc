/*dynamic dns configure, writen by wuke, 2009/12/01 */

#include "ddns.h"

namespace maxnet
{

	RouteDDns::RouteDDns()
	{

	}
	
	RouteDDns::~RouteDDns()
	{

	}
	
	bool RouteDDns::isVaild()
	{
		return true;
	}
	
	void RouteDDns::output()
	{
		std::cout << std::endl<< "# ddns" << std::endl;
		
		if(this->isDDnsEnable()){
			std::cout << "DDNS_ENABLE='1'" << std::endl;
		}else{
			std::cout << "DDNS_ENABLE='0'" << std::endl;
		}
		std::cout << "DDNS_USERNAME=\"" << this->getUsername() << "\"" << std::endl;
		std::cout << "DDNS_PASSWORD=\"" << this->getPassword() << "\"" << std::endl;
		std::cout << "DDNS_DNS=\"" << this->getDDnsDns() << "\"" << std::endl;
		std::cout << "DDNS_PROVIDER=\"" << this->getDDnsProvider() << "\"" << std::endl;
		return;
	}

	void RouteDDns::setDDnsEnable(bool DDnsEnable)
	{
		this->DDnsEnable = DDnsEnable;
	}

	void RouteDDns::setDDnsProvider(std::string provider)
	{
		this->provider = provider;
	}
	
    void RouteDDns::setUsername(std::string username)
	{
		this->username = username;
    }
    void RouteDDns::setPassword(std::string password)
	{
		this->password = password;
    }
	void RouteDDns::setDDnsDns(std::string DDnsDns)
	{
		this->DDnsDns = DDnsDns;
    }
	
	bool  RouteDDns::isDDnsEnable()
	{
		return this->DDnsEnable;
	}
	std::string RouteDDns::getUsername()
	{
		return this->username;
	}
	std::string RouteDDns::getPassword()
	{
		return this->password;
	}
	std::string RouteDDns::getDDnsDns()
	{
		return this->DDnsDns;
	}

	std::string RouteDDns::getDDnsProvider()
	{
		return this->provider;
	}
	
}
