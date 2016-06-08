#include "guide_tc.h"
#include "guide_tcmgr.h"


namespace maxnet
{

	TCMgr::TCMgr(){
       

	}

	TCMgr::~TCMgr(){
#if 1
		TC * tc = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			tc = (TC *)node_list.at(i);
			if(!tc->isGroup())
				delete tc;
		}
		node_list.clear();
#endif
		return;
	}
	
	void TCMgr::add(std::string name, std::string lineid, std::string isp, std::string quality, 
                std::string balance, std::string upload, std::string download, std::string enable)
    {
		TC * tc = new TC();
		tc->setName(name);
		tc->setLineid(lineid);
        tc->setISP(isp);
        tc->setQuality(quality);
        tc->setBalance(balance);
        tc->setUpload(upload);
        tc->setDownload(download);
        tc->setEnable(enable);
		
		if(tc->isVaild())
        {
			addNode(tc);
		}
		else
        {
			delete tc;
		}
		return;
	}
		
}

