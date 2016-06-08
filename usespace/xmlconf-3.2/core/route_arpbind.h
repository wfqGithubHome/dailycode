#ifndef _ZERO_ROUTE_ARPBIND_H
#define _ZERO_ROUTE_ARPBIND_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{

	class RouteARPBind : public Node{

		public:

			RouteARPBind();
			~RouteARPBind();
			bool isVaild();
			void output();
			std::string getCommand();

			void setID(const std::string id);
			void setIP(const std::string ip);
			void setMAC(const std::string mac);
			void setStatus(const std::string status);
			void setType(const std::string type);
			void setEnable(bool enable);

			std::string getID();
			std::string getIP();
			std::string getMAC();
			std::string	getStatus();
			std::string	getType();
			bool		getEnable();

		private:
			std::string id;
			std::string ip;
			std::string mac;
			std::string status;
			std::string type;
			bool		enable;
	};

}

#endif // _ZERO_ROUTE_ARPBIND_H

