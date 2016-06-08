#include "guide_appbind.h"
#include "guide_appbindmgr.h"
#include "../../global.h"

namespace maxnet
{

	AppbindMgr::AppbindMgr()
    {

	}

	AppbindMgr::~AppbindMgr()
    {
#if 1
		Appbind * appbind = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			appbind = (Appbind *)node_list.at(i);
			if(!appbind->isGroup())
				delete appbind;
		}
		node_list.clear();
#endif		
		return;
	}

	void AppbindMgr::add(std::string name, std::string apptype, std::string bindtype, std::string bindlineid,
					  std::string bypass, std::string maxupload, std::string maxdownload, std::string weight)
	{
		Appbind *appbind = new Appbind();
		appbind->setName(name);
		appbind->setAppType(apptype);
		appbind->setBindType(bindtype);
		appbind->setBindLineid(bindlineid);
		appbind->setBypass(bypass);
		appbind->setMaxUpload(maxupload);
        appbind->setMaxDownload(maxdownload);
        appbind->setWeight(weight);
		
		if(appbind->isVaild())
        {
			addNode(appbind);
		}    
		else
        {
			delete appbind;
		}
		return;
	}

}

