#ifndef __GUIDE_DNAT_H__
#define __GUIDE_DNAT_H__


#include <iostream>
#include <string>
#include <vector>
#include "../nodemgr.h"

namespace maxnet
{
	class Dnat 
    {
		public:
			Dnat();
			~Dnat();
            bool isVaild();
	
	    void setEnable(std::string enable);
        std::string getEnable();       
	
            
        private:
            std::string enable;               	
	};
}
#endif

