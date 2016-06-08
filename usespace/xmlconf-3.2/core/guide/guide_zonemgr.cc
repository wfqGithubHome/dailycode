#include "guide_zone.h"
#include "guide_zonemgr.h"


namespace maxnet
{

	ZoneMgr::ZoneMgr()
    {
       

	}

	ZoneMgr::~ZoneMgr()
    {
#if 1
		Zone * zone = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			zone = (Zone *)node_list.at(i);
			if(!zone->isGroup())
				delete zone;
		}
		node_list.clear();
#endif
		return;
	}
	
	void ZoneMgr::add(std::string type, std::string oem, std::string isp)
    {
		Zone* zone = new Zone();
        zone->setType(type);
		zone->setOEM(oem);
		zone->setISP(isp);
       		
		if(zone->isVaild())
        {
			addNode(zone);
		}
		else
        {
			delete zone;
		}
        
		return;
	}
		
}

