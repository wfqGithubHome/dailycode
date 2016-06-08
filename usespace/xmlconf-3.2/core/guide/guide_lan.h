#ifndef _ZERO_ROUTE_LAN_H
#define _ZERO_ROUTE_LAN_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Lan : public Node
    {
		public:
			Lan();
			~Lan();
			bool isVaild();

            void setIfName(std::string ifname);
			void setMAC(std::string mac);
			void setIP(std::string ipaddr);
			void setMask(std::string netmask);

            std::string getIfName();
			std::string getMAC();
			std::string getIP();
			std::string getMask();

		private:
			std::string ifname;
			std::string mac;
			std::string ipaddr;
			std::string netmask;
	};

}

#endif 
