#include "guide_server.h"
#include "guide_servermgr.h"


namespace maxnet
{
	ServerMgr::ServerMgr()
    {
       
	}

	ServerMgr::~ServerMgr()
    {
#if 1  
		Server * serv = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			serv = (Server *)node_list.at(i);
			if(!serv->isGroup())
				delete serv;
		}
		node_list.clear();
#endif		
		return;
	}
	
	void ServerMgr::add(std::string parentid, std::string value)
    {
		Server* serv = new Server();
		serv->setParentid(parentid);
		serv->setValue(value);
   		
		if(serv->isVaild())
        {
			addNode(serv);
		}
		else
        {
			delete serv;
		}
        
		return;
	}
		
}

