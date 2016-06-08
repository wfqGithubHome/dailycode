#ifndef _ZERO_ROUTE_TABLESMGR_H
#define _ZERO_ROUTE_TABLESMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_table.h"

namespace maxnet{


	class RouteTableMgr : public NodeMgr  {
		public:

			RouteTableMgr();
			~RouteTableMgr();
			void output();
			
		private:



	};

}

#endif // _ZERO_ROUTE_TABLESMGR_H

