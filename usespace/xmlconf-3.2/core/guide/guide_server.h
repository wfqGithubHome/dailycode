#ifndef _ZERO_GUIDE_SERVER_H
#define _ZERO_GUIDE_SERVER_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Server : public Node
    {
		public:
			Server();
			~Server();
			bool isVaild();
			
			void setParentid(std::string parentid);
			void setValue(std::string value);
			
			std::string getParentid();
			std::string getValue();

		private:
			std::string parentid;
			std::string value;
	
	};

}

#endif 
