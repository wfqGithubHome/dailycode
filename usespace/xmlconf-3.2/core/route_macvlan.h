#ifndef __ROUTE_MACVLAN_H__
#define __ROUTE_MACVLAN_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteMacVLan : public Node{

		public:

			RouteMacVLan();
			~RouteMacVLan();
			bool isVaild();
			std::string getCommand();
			void output();

			void setName(const std::string name);
			void setIfname(const std::string ifname);
			void setMAC(const std::string mac);
            void setComment(const std::string comment);
            void setID(const std::string id);

			std::string getName();
			std::string getIfname();
			std::string getMAC();
            std::string getComment();
            std::string getID();

		private:
			std::string name;
			std::string ifname;
			std::string mac;
            std::string comment;
            std::string id;
	};
}

#endif // _ZERO_VLAN_H
