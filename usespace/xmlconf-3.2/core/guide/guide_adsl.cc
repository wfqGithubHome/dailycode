#include "guide_adsl.h"
#include "../../global.h"

namespace maxnet
{
	Adsl::Adsl()
    {

	}

	Adsl::~Adsl(){
		
	}
	
	bool Adsl::isVaild()
    {				
		return true;
	}
	
	void Adsl::setAdslIfname(std::string ifname)
	{
		this->ifname = ifname;
	}

	void Adsl::setAdslISP(std::string isp)
    {
		this->isp = isp;
	}
	
	void Adsl::setAdslQuality(std::string quality)
    {
		this->quality = quality;
	}

	void Adsl::setAdslLineid(std::string lineid)
    {
		this->lineid = lineid;
	}

	void Adsl::setAdslUpload(std::string upload)
    {
		this->upload =upload ;
	}

	void Adsl::setAdslDownload(std::string download)
    {
		this->download = download;
	}

	void Adsl::setAdslUsername(std::string username)
    {
		this->username = username;
	}
	
	void Adsl::setAdslPassword(std::string password)
    {
		this->password = password;
	}

	std::string Adsl::getAdslIfname()
	{
		return ifname;
	}
	
	std::string Adsl::getAdslISP()
    {
		return isp;
	}

	std::string Adsl::getAdslQuality()
    {
		return quality;
	}
	
	std::string Adsl::getAdslLineid()
    {
		return lineid;
	}

	std::string Adsl::getAdslUpload()
    {
		return upload;
	}

	std::string Adsl::getAdslDownload()
    {
		return download;
	}

	std::string Adsl::getAdslUsername()
    {
		return username;
	}

	std::string Adsl::getAdslPassword()
    {
		return password;
	}

}

