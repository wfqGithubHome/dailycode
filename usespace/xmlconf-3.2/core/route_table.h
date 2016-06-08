#ifndef _ZERO_ROUTE_TABLE_H
#define _ZERO_ROUTE_TABLE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{

	class RouteTable : public Node{

		public:

			RouteTable();
			~RouteTable();

			bool isVaild();
			std::string getCommand();
			void output();

		private:
	};

}

#endif // _ZERO_ROUTE_TABLE_H
