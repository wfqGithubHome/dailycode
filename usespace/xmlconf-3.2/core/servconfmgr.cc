#include "servconf.h"
#include "servconfmgr.h"
#include "../global.h"

namespace maxnet{

	ServConfMgr::ServConfMgr()
    {

	}

	ServConfMgr::~ServConfMgr()
    {
#if 1
		ServConf * servconf = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			servconf = (ServConf *)node_list.at(i);
			if(!servconf->isGroup())
				delete servconf;
		}
		node_list.clear();
#endif
		return;
	}

    void ServConfMgr::add(std::string type, std::string en_name, std::string en_zonename)
    {
		ServConf* servconf = new ServConf();

        servconf->setType(type);
		servconf->setEnTypeName(en_name);
		servconf->setEnZoneName(en_zonename);
       
        if(servconf->isVaild())
        {
            addNode(servconf);
		}
		else
        {
			delete servconf;
		}

		return;
	}
      
}

