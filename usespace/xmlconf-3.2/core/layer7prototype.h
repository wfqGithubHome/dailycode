#ifndef _ZERO_LAYER7_RPOTOTYPE_H
#define _ZERO_LAYER7_RPOTOTYPE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Layer7ProtoType : public Node{

		public:

			Layer7ProtoType();
			~Layer7ProtoType();
			bool isVaild();
			void output();

            void setValue(std::string value);

            std::string getValue();
		private:
			std::string value;
	};

}

#endif // _ZERO_LAYER7_RPOTOTYPE_H

