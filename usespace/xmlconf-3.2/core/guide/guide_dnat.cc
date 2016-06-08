
#include "guide_dnat.h"
#include "../../global.h"


namespace maxnet
{
	Dnat::Dnat()
    {

	}
	
	Dnat::~Dnat()
    {

	}
	
	bool Dnat::isVaild()
    {
		return true;
	}
	
	void Dnat::setEnable(std::string enable)
    {
		this->enable = enable;
	}
	
	std::string Dnat::getEnable()
    {
		return enable;
	}
	
}
