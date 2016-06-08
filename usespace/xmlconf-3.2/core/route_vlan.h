#ifndef _ZERO_ROUTE_VLAN_H
#define _ZERO_ROUTE_VLAN_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteVLan : public Node{

		public:

			RouteVLan();
			~RouteVLan();
			bool isVaild();
			std::string getCommand();
			void output();

			void setName(const std::string name);
			void setIfname(const std::string ifname);
			void setMAC(const std::string mac);
			void setID(const std::string id);
            void setComment(const std::string comment);
            void setIndex(const std::string index);

			std::string getName();
			std::string getIfname();
			std::string getMAC();
			std::string getID();
            std::string getComment();
            std::string getIndex();

		private:
			std::string name;
			std::string ifname;
			std::string mac;
			std::string id;
            std::string comment;

            std::string index;
	};

}

#endif // _ZERO_VLAN_H
