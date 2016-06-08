#ifndef _ZERO_URLFORMATMGR_H
#define _ZERO_URLFORMATMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class UrlFormatMgr : public NodeMgr  {
		public:

			UrlFormatMgr();
			~UrlFormatMgr();
			void output();

			void add(std::string bridge_id, 
						std::string format,
						std::string comment);



	};

}

#endif // _ZERO_URLFORMATMGR_H
