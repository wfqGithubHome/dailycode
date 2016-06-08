#ifndef _ZERO_LAYER7_RPOTOTYPE_MGR_H
#define _ZERO_LAYER7_RPOTOTYPE_MGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
namespace maxnet{


	class Layer7ProtoTypeMgr : public NodeMgr  {
		public:

			Layer7ProtoTypeMgr();
			~Layer7ProtoTypeMgr();

			void output();
            
			void add(std::string name, std::string value);
	};

}

#endif // _ZERO_LAYER7_RPOTOTYPE_MGR_H

