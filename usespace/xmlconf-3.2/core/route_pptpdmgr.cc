#include <sstream>
#include "route_pptpd.h"
#include "route_pptpmgr.h"
#include "../global.h"

namespace maxnet{
	PptpdMgr::PptpdMgr(){
		
	}

	PptpdMgr::~PptpdMgr(){
		Pptpd * pptp = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			pptp = (Pptpd *)node_list.at(i);
			delete pptp;
		}
		node_list.clear();
	}

	void PptpdMgr::add(std::string localip, std::string startip, std::string endip, std::string dns,
                std::string entry, bool enable){
		Pptpd * pptp = new Pptpd();
		pptp->setPptpdEnable(enable);
		pptp->setPptpdlocalip(localip);
		pptp->setPptpdstartip(startip);
		pptp->setPptpdendip(endip);
		pptp->setPptpddns(dns);
		pptp->setPptpdentry( entry);

		if(pptp->isVaild()){
			addNode(pptp);
		}
		else{
			delete pptp;
		}
	}
}
