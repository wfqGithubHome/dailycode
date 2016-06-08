#ifndef _ZERO_DOMAINNODE_H
#define _ZERO_DOMAINNODE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

/**
* 	域名数据保存结点类
*/

namespace maxnet{


	class DomainNode : public Node{

		public:

			DomainNode();
			~DomainNode();
			std::string	getCommand();

			bool isVaild();
			void output();

			void setValue(std::string value);

			std::string getValue();

			void setID(uint32_t id);

			uint32_t getID();


		private:
			std::string value;
			uint32_t domain_id;


	};

}

#endif // _ZERO_DOMAINNODE_H
