#include "networkdetection.h"
#include "../global.h"

namespace maxnet{

	NetworkDetection::NetworkDetection()
	{
		zeropingvalue = 0;
	}

	NetworkDetection::~NetworkDetection(){
		
	}

	void NetworkDetection::setZeroPingValue(int zeropingvalue)
	{
		this->zeropingvalue = zeropingvalue;
	}

	int NetworkDetection::getZeroPingValue(void)
	{
		return zeropingvalue;
	}
}
