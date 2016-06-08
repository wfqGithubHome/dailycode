#ifndef _ZERO_HTTPDIRPOLICY_H
#define _ZERO_HTTPDIRPOLICY_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{


	class HttpDirPolicy : public Node{

		public:

			HttpDirPolicy();
			~HttpDirPolicy();
			bool isVaild();
			void output();

			void setBridgeId(std::string id);
			void setExtgroup(std::string extgroup);
			void setServergroup(std::string servergroup);


			int			getBridgeId();
           	std::string getBridgeIdString();
			std::string getExtgroup();
			std::string getServergroup();
			
			void setID(uint32_t id);
			uint32_t getID();

		private:
			int			bridge_id;
			std::string extgroup;
			std::string servergroup;
			
			uint32_t this_id;
	};

}

#endif // _ZERO_HTTPDIRPOLICY_H
