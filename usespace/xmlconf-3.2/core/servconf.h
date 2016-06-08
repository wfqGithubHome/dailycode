#ifndef _ZERO_SERVCONF_H
#define _ZERO_SERVCONF_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class ServConf : public Node{

		public:

			ServConf();
			~ServConf();
			bool isVaild();

            std::string getType();
            std::string getEnTypeName();
            std::string getEnZoneName();

            void setType( std::string type);
            void setEnTypeName( std::string en_name);
            void setEnZoneName( std::string en_zonename);

       
		private:
            std::string type;
            std::string en_name;
            std::string en_zonename;
	};


}

#endif

