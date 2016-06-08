#include "guide_tc.h"
#include "../../global.h"

namespace maxnet
{

	TC::TC()
    {

	}

	TC::~TC()
    {
		
	}
	
	bool TC::isVaild()
    {
		return true;
	}
	
	void TC::setName(std::string name)
    {
		this->name = name;
	}
	
	void TC::setLineid(std::string lineid)
    {
		this->lineid = lineid;
	}

	void TC::setISP(std::string isp)
    {
		this->isp = isp;
	}
    
	void TC::setQuality(std::string quality)
    {
		this->quality = quality;
	}

	void TC::setBalance(std::string balance)
    {
		this->balance = balance;
	}

	void TC::setUpload(std::string upload)
    {
		this->upload = upload;
	}

	void TC::setDownload(std::string download)
    {
		this->download = download;
	}
      
	void TC::setEnable(std::string enable)
    {    
		this->enable = enable;
	}

	std::string TC::getName()
    {
		return name;
	}
	
	std::string TC::getLineid()
    {
		return lineid;
	}
    
	std::string TC::getISP()
    {
		return isp;
	}
  
	std::string TC::getQuality()
    {
		return quality;
	}
  
	std::string TC::getBalance()
    {
		return balance;
	}
    
	std::string TC::getUpload()
    {
		return upload;
	}

    std::string TC::getDownload()
    {
        return download;
    }

    std::string TC::getEnable()
    {
		return enable;
	}

}

