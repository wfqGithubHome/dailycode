#include "guide_zone.h"

namespace maxnet
{
	Zone::Zone()
    {

	}

	Zone::~Zone()
    {
		
	}
	
	bool Zone::isVaild()
    {
		return true;
	}

    void Zone::setType(std::string type)
    {
        this->type = type;
    }
    
	void Zone::setOEM(std::string oem)
    {
		this->oem = oem;
	}
	
	void Zone::setISP(std::string isp)
    {
		this->isp = isp;
	}

    std::string Zone::getType()
    {
        return type;
    }
    
	std::string Zone::getOEM()
    {
		return oem;
	}

	std::string Zone::getISP()
    {
		return isp;
	}
	

}

