#ifndef _ZERO_GUIDE_DEVICEZONE_H
#define _ZERO_GUIDE_DEVICEZONE_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Zone : public Node
    {
		public:
			Zone();
			~Zone();
			bool isVaild();

            void setType(std::string type);
			void setOEM(std::string oem);
			void setISP(std::string isp);

            std::string getType();
			std::string getOEM();
			std::string getISP();
		
		private:
			std::string type;
			std::string oem;
			std::string isp;			
	};

}

#endif 
