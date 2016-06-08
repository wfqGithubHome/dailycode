#ifndef _ZERO_EXTENSIONNODE_H
#define _ZERO_EXTENSIONNODE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

/**
* 	域名数据保存结点类
*/

namespace maxnet{


	class ExtensionNode : public Node{

		public:

			ExtensionNode();
			~ExtensionNode();

			bool isVaild();
			void output();

			void setValue(std::string value);

			std::string getValue();

			/*void setCheck(bool arg_check);

			int getCheck();*/
			
			void setID(uint32_t id);
			uint32_t getID();


		private:
			std::string value;
			//bool check;
			
			uint32_t this_id;


	};

}

#endif // _ZERO_EXTENSIONNODE_H
