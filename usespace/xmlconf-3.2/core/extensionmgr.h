#ifndef _ZERO_EXTENSIONMGR_H
#define _ZERO_EXTENSIONMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class ExtensionMgr : public NodeMgr  {
		public:

			ExtensionMgr();
			~ExtensionMgr();
			void output();

			void add(std::string name, std::string value,std::string comment);

	
	private:			
		int add_id;


	};

}

#endif // _ZERO_EXTENSIONMGR_H
