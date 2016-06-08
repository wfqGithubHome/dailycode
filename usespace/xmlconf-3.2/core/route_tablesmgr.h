#ifndef __ROUTE_TABLESMGR_H__
#define __ROUTE_TABLESMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_tables.h"

namespace maxnet{

	class RouteTablesMgr : public NodeMgr  {
		public:

			RouteTablesMgr();
			~RouteTablesMgr();
			void output();

			void add(std::string id, bool enable,std::string value);
		private:

	};

}

#endif // _ZERO_VLANMGR_H
