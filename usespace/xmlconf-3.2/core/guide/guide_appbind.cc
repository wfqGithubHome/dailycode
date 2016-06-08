#include "guide_appbind.h"
#include "../../global.h"

namespace maxnet
{

	Appbind::Appbind()
    {

	}

	Appbind::~Appbind()
    {
		
	}
	
	bool Appbind::isVaild()
    {
		return true;
	}


	void Appbind::setName(std::string name)
    {
		this->name = name;
	}
	
	void Appbind::setAppType(std::string apptype)
    {
		this->apptype = apptype;
	}
	
	void Appbind::setBindType(std::string bindtype)
    {
		this->bindtype = bindtype;
	}

	void Appbind::setBindLineid(std::string bindlineid)
    {
		this->bindlineid = bindlineid;
	}

	void Appbind::setBypass(std::string bypass)
    {
		this->bypass = bypass;
	}

    void Appbind::setMaxUpload(std::string maxupload)
    {
		this->maxupload = maxupload;
	}

    void Appbind::setMaxDownload(std::string maxdownload)
    {
		this->maxdownload = maxdownload;
	}

    void Appbind::setWeight(std::string weight)
    {
		this->weight = weight;
	}

    
	std::string Appbind::getName()
    {
		return name;
	}

	std::string Appbind::getAppType()
    {
		return apptype;
	}
	
	std::string Appbind::getBindType()
    {
		return bindtype;
	}
	
	std::string Appbind::getBindLineid()
    {
		return bindlineid;
	}

	std::string Appbind::getBypass()
    {
		return bypass;
	}

    std::string Appbind::getMaxUpload()
    {
		return maxupload;
	}
    
    std::string Appbind::getMaxDownload()
    {
		return maxdownload;
	}
    
    std::string Appbind::getWeight()
    {
		return weight;
	}

}

