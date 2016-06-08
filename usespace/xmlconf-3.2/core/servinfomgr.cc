#include "servinfo.h"
#include "servinfomgr.h"
#include "../global.h"

namespace maxnet{

	ServInfoMgr::ServInfoMgr()
    {

	}

	ServInfoMgr::~ServInfoMgr()
    {
#if 1
		ServInfo * servinfo = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			servinfo = (ServInfo *)node_list.at(i);
			if(!servinfo->isGroup())
				delete servinfo;
		}
		node_list.clear();
#endif
		return;
	}

    
    void ServInfoMgr::add(std::string type, std::string name, std::string en_name, 
                    std::string zonename, std::string en_zonename,
                    std::string var1, std::string var2, std::string var3, std::string var4, 
                    std::string comment)
    {
		ServInfo* servinfo = new ServInfo();
        
		servinfo->setType(type);
		servinfo->setTypeName(name);
        servinfo->setEnTypeName(en_name);
		servinfo->setZoneName(zonename);
        servinfo->setEnZoneName(en_zonename);
		servinfo->setVar1(var1);
        servinfo->setVar2(var2);
        servinfo->setVar3(var3);
        servinfo->setVar4(var4);
        servinfo->setComment(comment);
       
        if(servinfo->isVaild())
        {
            addNode(servinfo);
		}
		else
        {
			delete servinfo;
		}

		return;
	}
    
}


