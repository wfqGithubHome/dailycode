
#include "guide_lan.h"
#include "guide_lanmgr.h"


namespace maxnet{

	LanMgr::LanMgr(){
       

	}

	LanMgr::~LanMgr(){
#if 1
		Lan * if_lan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			if_lan = (Lan *)node_list.at(i);
			if(!if_lan->isGroup())
				delete if_lan;
		}
		node_list.clear();
#endif
		return;
	}
	
	void LanMgr::add(std::string ifname, std::string mac, std::string ipaddr, std::string netmask){
		Lan * if_lan = new Lan();
        if_lan->setIfName(ifname);
        if_lan->setIP(ipaddr);
		if_lan->setMask(netmask);
        if_lan->setMAC(mac);
		
		if(if_lan->isVaild())
        {
			addNode(if_lan);
		}
		else{
			delete if_lan;
		}
		return;
	}
		
}

