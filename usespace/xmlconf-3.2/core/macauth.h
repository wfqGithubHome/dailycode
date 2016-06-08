#ifndef _MAC_AUTH_H
#define _MAC_AUTH_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class MacAuth : public Node{

		public:

			MacAuth();
			~MacAuth();
			std::string	getCommand();

			bool isVaild();
			void output();

			void setValue(std::string value);
	

			std::string getValue();



		private:
			std::string value;
	


	};

}

#endif
