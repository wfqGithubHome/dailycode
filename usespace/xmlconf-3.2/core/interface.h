#ifndef _ZERO_INTERFACE_H
#define _ZERO_INTERFACE_H

#include <iostream>
#include <string>
#include <vector>
#include "node.h"



namespace maxnet{


	class Interface : public Node{

		public:

			Interface();
			~Interface();
			bool isVaild();

			void output();

			void	setMode(const std::string	mode);
			void	setSpeed(const std::string speed);
			void	setDuplex(const std::string duplex);

            void    setlinkstate(const std::string    linkstate);
            void    setbridgeid(const std::string     bridgeid);

			std::string 	getMode();
			std::string 	getSpeed();
			std::string 	getDuplex();

            std::string     getlinkstate();
            std::string     getbridgeid();

		private:
			std::string	mode;
			std::string speed;
			std::string	duplex;
            std::string	linkstate;
            std::string	bridgeid;
	};

}

#endif // _ZERO_MANAGEMENT_H
