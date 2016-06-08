#ifndef _ZERO_URLFORMATNODE_H
#define _ZERO_URLFORMATNODE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class UrlFormatNode : public Node{

		public:

			UrlFormatNode();
			~UrlFormatNode();
			bool isVaild();
			void output();

			void setBridgeId(std::string id);
			void setFormat(std::string format);

			int			getBridgeId();
			std::string getBridgeIdString();
			std::string	getFormat();
			

		private:
			int			bridge_id;
			std::string format;
	};

}

#endif // _ZERO_URLFORMATNODE_H
