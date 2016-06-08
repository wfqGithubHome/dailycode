#ifndef _ZERO_NETADDR_H
#define _ZERO_NETADDR_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class NetAddr : public Node{

		public:

			NetAddr();
			~NetAddr();
			std::string	getCommand();

			bool isVaild();
			void output();

			void setValue(std::string value);
			void setRange(bool range);

			std::string getValue();
			bool getRange();
			std::string getRangeString();


		private:
			std::string value;
			bool range;


	};

}

#endif // _ZERO_SYSTEM_H
