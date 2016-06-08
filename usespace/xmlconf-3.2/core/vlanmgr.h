#ifndef _ZERO_VLANMGR_H
#define _ZERO_VLANMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "vlan.h"

namespace maxnet{


	class VlanMgr : public NodeMgr  {
		public:

			VlanMgr();
			~VlanMgr();
			void output();

			void add(std::string name, std::string vlans, std::string comment);
		private:



	};

}

#endif // _ZERO_VLANMGR_H
