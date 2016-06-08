#ifndef _ZERO_HTTPSERVERMGR_H
#define _ZERO_HTTPSERVERMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class HttpServerMgr : public NodeMgr  {
		public:

			HttpServerMgr();
			~HttpServerMgr();
			void output();
			
			void add(std::string name,std::string bridgeid,
						std::string ipaddr, std::string servertype,
						std::string prefix, std::string suffix,
						std::string comment);

			int getGroupID(std::string groupname);

	};

}

#endif // _ZERO_HTTPSERVERMGR_H
