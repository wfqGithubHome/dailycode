#include "route_tablemgr.h"
#include "route_table.h"
#include "../global.h"

namespace maxnet{

	RouteTableMgr::RouteTableMgr(){
	
	}

	RouteTableMgr::~RouteTableMgr(){
#if 0
		RouteTable * table = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			table = (RouteTable *)node_list.at(i);
			
			delete table;
		}
		node_list.clear();
#endif
		return;	
	}

	void RouteTableMgr::output(){
		RouteTable * table = NULL;
		std::cout << std::endl << "# Route Tables" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			table = (RouteTable *)node_list.at(i);
			table->output();
		}
		
		return;
	}
}

