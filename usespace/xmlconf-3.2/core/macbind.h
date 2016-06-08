#ifndef _IP_MAC_H
#define _IP_MAC_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class MacBind : public Node{

		public:

			MacBind();
			~MacBind();
			bool isVaild();
			void output();

			void setName(std::string name);
			void setBridge(std::string bridge);
			void setIpaddr(std::string ipaddr);
			void setMacaddr(std::string macaddr);
			void setEnable(bool enable);

			bool getEnable();
			std::string getName();
			std::string getBridge();
			std::string getIpaddr();
			std::string getMacaddr();

		private:
			std::string name;
			std::string bridge;
			std::string ipaddr;
			std::string macaddr;
			bool		  enable;      
	};

}

#endif 

