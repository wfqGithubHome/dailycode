#ifndef __ROUTE_TABLES_H__
#define __ROUTE_TABLES_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteTables : public Node{

		public:

			RouteTables();
			~RouteTables();
			bool isVaild();
			std::string getCommand();
			void output();

			void setId(const std::string id);
			void setEnable(bool enable);
			void setValue(const std::string value);

			std::string getId();
			bool        getEnable();
			std::string getValue();

		private:
			std::string id;
			bool        enable;
			std::string value;
	};
}

#endif // _ZERO_VLAN_H
