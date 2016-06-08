#ifndef _ZERO_ROUTE_SNAT_H
#define _ZERO_ROUTE_SNAT_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{

	class RouteSNAT : public Node{

		public:

			RouteSNAT();
			~RouteSNAT();
			bool isVaild();
			void output();
			std::string getCommand();

			void setID(const std::string id);
			void setIfname(const std::string ifname);
			void setSRC(const std::string src);
			void setDST(const std::string dst);
			void setProtocol(const std::string protocol);
			void setAction(const std::string action);
			void setDSTIP(const std::string dstip);
			void setDSTPort(const std::string dstport);
			void setEnable(bool enable);

			std::string getID();
			std::string getIfname();
			std::string getSRC();
			std::string getDST();
			std::string getProtocol();
			std::string getAction();
			std::string	getDSTIP();
			std::string	getDSTPort();
			bool		getEnable();

		private:
			std::string id;
			std::string ifname;
			std::string src;
			std::string dst;
			std::string protocol;
			std::string action;
			std::string dstip;
			std::string dstport;
			bool		enable;
	};

}

#endif // _ZERO_ROUTE_SNAT_H

