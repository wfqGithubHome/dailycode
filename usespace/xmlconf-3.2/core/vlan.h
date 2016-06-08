#ifndef _ZERO_VLAN_H
#define _ZERO_VLAN_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Vlan : public Node{

		public:

			Vlan();
			~Vlan();
			bool isVaild();
			std::string getCommand();

			void output();

			void setVlans(std::string vlans);
			std::string getVlans();


		private:
			std::string vlans;
	};

}

#endif // _ZERO_VLAN_H
