#ifndef _ZERO_ROUTE_LAN_H
#define _ZERO_ROUTE_LAN_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteLan : public Node{

		public:

			RouteLan();
			~RouteLan();
			bool isVaild();
			void output();
			std::string getCommand();

			void setIfname(std::string ifname);
			void setMAC(std::string mac);
			void setNewMAC(std::string newmac);
			void setIP(std::string ip);
			void setMask(std::string mask);
            void setAliases(std::string aliases);
            void setID(std::string ID);
			
			std::string getIfname();
			std::string getMAC();
			std::string getNewMAC();
			std::string getIP();
			std::string getMask();
            std::string getAliases();
            std::string getID();
			
		private:
			std::string ifname;
			std::string mac;
			std::string newmac;
			std::string ip;
			std::string mask;
            std::string aliases;

            std::string id;
	};

}

#endif // _ZERO_VLAN_H
