#include "route_tables.h"
#include "route_tablesmgr.h"
#include "../global.h"

namespace maxnet{

	RouteTablesMgr::RouteTablesMgr(){

	}

	RouteTablesMgr::~RouteTablesMgr(){
#if 1
		RouteTables * table = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			table = (RouteTables *)node_list.at(i);
			if(!table->isGroup())
				delete table;
		}
		node_list.clear();
#endif
		return;
	}

	void RouteTablesMgr::add(std::string id, bool enable,std::string value){

		RouteTables * table = new RouteTables();
		table->setId(id);
		table->setEnable(enable);
		table->setValue(value);
		
		if(table->isVaild()){
			addNode(table);
		}
		else{
			delete table;
		}
		return;
	}

	
	void RouteTablesMgr::output(){
		RouteTables * vlan = NULL;
		bool temp = false;
		std::string ids = "";
		std::cout  << "# route tables " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			vlan = (RouteTables *)node_list.at(i);
			vlan->output();

			if(temp){
				ids += " " + vlan->getId();
			}else{
				ids = vlan->getId();
				temp = true;
			}
		}

		std::cout << "ROUTE_TABLES_IDS=\"" << ids << "\"" << std::endl;
		return;
	}
}

