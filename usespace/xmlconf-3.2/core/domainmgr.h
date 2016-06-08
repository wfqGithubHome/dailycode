#ifndef _ZERO_DOMAINMGR_H
#define _ZERO_DOMAINMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class DomainMgr : public NodeMgr  {
		public:

			DomainMgr(bool addDefault = true);
			~DomainMgr();
			void output();

			void add(std::string name, std::string value, std::string comment);

		private:			
			int domain_id;


	};

}

#endif // _ZERO_DOMAINMGR_H
