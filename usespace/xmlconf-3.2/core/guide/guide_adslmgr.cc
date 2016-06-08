#include "guide_adsl.h"
#include "guide_adslmgr.h"
#include "../../global.h"

namespace maxnet
{

	AdslMgr::AdslMgr()
    {

	}

	AdslMgr::~AdslMgr()
    {
#if 1 
		Adsl * adsl= NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			adsl = (Adsl *)node_list.at(i);
			if(!adsl->isGroup())
				delete adsl;
		}
		node_list.clear();
#endif		
		return;
	}

	void AdslMgr::add(std::string ifname, std::string isp, std::string quality, std::string lineid, 
                std::string upload, std::string download, std::string username, std::string password)
	{

		Adsl * adsl = new Adsl();
		adsl->setAdslIfname(ifname);
		adsl->setAdslISP(isp);
		adsl->setAdslQuality(quality);
		adsl->setAdslLineid(lineid);
		adsl->setAdslUpload(upload);
		adsl->setAdslDownload(download);
		adsl->setAdslUsername(username);
		adsl->setAdslPassword(password);
		
		
		if(adsl->isVaild())
        {
			addNode(adsl);
		}
		else
        {
			delete adsl;
		}
		return;
	}

}

