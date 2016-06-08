#ifndef _ZERO_ROUTE_MACBIND_H
#define _ZERO_ROUTE_MACBIND_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{

	class RouteMACBind : public Node{
		
		public:

			RouteMACBind();
			~RouteMACBind();
			bool isVaild();
			void output();
			std::string getCommand();

			void setID(const std::string id);
			void setIP(const std::string ip);
			void setMAC(const std::string mac);
			void setAction(const std::string action);
			void setBridge(const std::string bridge);

			std::string getID();
			std::string getIP();
			std::string getMAC();
			std::string	getAction();
			std::string	getBridge();

		private:
			std::string id;
			std::string ip;
			std::string mac;
			std::string action;
			std::string bridge;
	};

}

#endif // _ZERO_ROUTE_MACBIND_H

