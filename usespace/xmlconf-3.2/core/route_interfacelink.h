#ifndef __ROUTE_INTERFACELINK_H__
#define __ROUTE_INTERFACELINK_H__

#include <iostream>
#include <string>
#include <vector>
#include "node.h"



namespace maxnet{


	class Route_InterfaceLink : public Node{

		public:

			Route_InterfaceLink();
			~Route_InterfaceLink();
			bool isVaild();

			void output();

            void	setIfname(const std::string ifname);
            void    setType(const std::string type);
			void	setMode(const std::string mode);
			void	setSpeed(const std::string speed);
			void	setDuplex(const std::string duplex);
            void    setlinkstate(const std::string    linkstate);

            std::string     getIfname();
            std::string     getType();
			std::string 	getMode();
			std::string 	getSpeed();
			std::string 	getDuplex();
            std::string     getlinkstate();

		private:
            std::string ifname;
            std::string type;
			std::string	mode;
			std::string speed;
			std::string	duplex;
            std::string	linkstate;
	};

}

#endif // _ZERO_MANAGEMENT_H
