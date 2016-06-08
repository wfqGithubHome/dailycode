#ifndef _ZERO_ROUTE_RULE_H
#define _ZERO_ROUTE_RULE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteRule : public Node{

		public:

			RouteRule();
			~RouteRule();
			bool isVaild();
			void output();
			std::string getCommand();

			//void setName(const std::string name);
			void setSRC(const std::string src);
			void setDST(const std::string dst);
			void setProtocol(const std::string protocol);
                     void setSRCPort(const std::string src_port);
			void setDSTPort(const std::string dst_port);
			void setTOS(const std::string tos);
            void setAction(const std::string action);
			void setFWMark(const std::string fwmark);

			//std::string getName();
			std::string getSRC();
			std::string getDST();
			std::string getProtocol();
                     std::string getSRCPort();
			std::string getDSTPort();
			std::string getTOS();
            std::string getAction();
			std::string	getFWMark();

		private:
			//std::string name;
			std::string src;
			std::string dst;
			std::string protocol;
			std::string src_port;
            std::string dst_port;
			std::string tos;
            std::string action;
			std::string fwmark;
	};

}

#endif // _ZERO_ROUTE_RULE_H
