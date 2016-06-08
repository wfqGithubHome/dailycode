#ifndef _ZERO_ROUTE_LOADB_H
#define _ZERO_ROUTE_LOADB_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteLoadBalancing: public Node{

		public:

			RouteLoadBalancing();
			~RouteLoadBalancing();
			bool isVaild();
			void output();
			std::string getCommand();

			void setIFName(const std::string ifname);
			void setRuleName(const std::string rulename);
			void setWeight(const std::string weight);

			std::string getIFName();
			std::string getRuleName();
			std::string getWeight();
			
		private:
			std::string ifname;
			std::string rulename;
			std::string weight;
	};

}

#endif // _ZERO_ROUTE_LOADB_H
