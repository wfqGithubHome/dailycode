#ifndef _ZERO_ROUTE_ROUTING_H
#define _ZERO_ROUTE_ROUTING_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteRouting : public Node{

		public:

			RouteRouting();
			~RouteRouting();
			bool isVaild();
			void output();
			std::string getCommand();

			void setID(const std::string id);
			void setDST(const std::string dst);
			void setGateway(const std::string gateway);
			void setIFName(const std::string ifname);
			void setPriority(const std::string priority);
			void setRule(const std::string rule);
			void setTable(const std::string table);
			void setEnable(bool enable);

			std::string getID();
			std::string getDST();
			std::string getGateway();
			std::string getIFName();
			std::string getPriority();
			std::string getRule();
			std::string getTable();
			bool		getEnable();

		private:
			std::string id;
			std::string dst;
			std::string gateway;
			std::string ifname;
			std::string priority;
			std::string rule;
			std::string table;
			bool		enable;
	};

}

#endif // _ZERO_ROUTE_ROUTING_H
