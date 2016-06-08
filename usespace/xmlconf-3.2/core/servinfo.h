#ifndef _ZERO_SERVINFO_H
#define _ZERO_SERVINFO_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class ServInfo : public Node{

		public:

			ServInfo();
			~ServInfo();
			bool isVaild();

            std::string getType();
            std::string getTypeName();
            std::string getEnTypeName();
            std::string getZoneName();
            std::string getEnZoneName();
            std::string getVar1();
            std::string getVar2();
            std::string getVar3();
            std::string getVar4();
            std::string getComment();

            void setType( std::string type);
            void setTypeName( std::string name);
            void setEnTypeName( std::string en_name);
            void setZoneName( std::string zonename);
            void setEnZoneName( std::string en_zonename);
            void setVar1(std::string var1);
            void setVar2(std::string var2);
            void setVar3(std::string var3);
            void setVar4(std::string var4);
            void setComment(std::string comment);
        
       
		private:
            std::string type;
            std::string name;
            std::string en_name;
            std::string zonename;
            std::string en_zonename;
            std::string var1;
            std::string var2;
            std::string var3;
            std::string var4;
            std::string comment;
	};

}

#endif

