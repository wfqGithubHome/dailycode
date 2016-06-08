#ifndef _ZERO_HTTPDIRPOLICYMGR_H
#define _ZERO_HTTPDIRPOLICYMGR_H


#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{

	class HttpDirPolicyMgr : public NodeMgr  {
		public:

			HttpDirPolicyMgr();
			~HttpDirPolicyMgr();
			void output();

			void add(std::string name,std::string bridge_id, 
						std::string extgroup, std::string servergroup, 
						std::string comment,bool disabled);

		private:
			int  policy_id[MAX_BRIDGE_ID];

	};

}

#endif // _ZERO_HTTPDIRPOLICYMGR_H

