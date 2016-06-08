#include "vlan.h"
#include "vlanmgr.h"
#include "../global.h"

namespace maxnet{

	VlanMgr::VlanMgr(){

	}

	VlanMgr::~VlanMgr(){
#if 1
		Vlan * vlan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			vlan = (Vlan *)node_list.at(i);
			delete vlan;
		}
		node_list.clear();
#endif
		return;
	}
	
	void VlanMgr::add(std::string name, std::string vlans, std::string comment){
		Vlan * vlan = new Vlan();
		vlan->setName(name);
		vlan->setVlans(vlans);
		vlan->setComment(comment);
		vlan->setGroup(false);
		if(vlan->isVaild()){
			addNode(vlan);
		}
		else{
			delete vlan;
		}
		return;
	}

	void VlanMgr::output(){
		Vlan * vlan = NULL;
		std::cout  << "# vlans " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			vlan = (Vlan *)node_list.at(i);
			vlan->output();
		}
		

		
		return;

	}
}

