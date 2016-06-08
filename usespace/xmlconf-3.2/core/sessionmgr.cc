#include "sessionmgr.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	SessionMgr::SessionMgr(){

	}

	SessionMgr::~SessionMgr(){
#if 1
		Session * ses = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			ses = (Session *)node_list.at(i);
			if(!ses->isGroup())
				delete ses;
		}
		node_list.clear();
#endif		
		return;
	}
	

	void SessionMgr::add(std::string name, std::string session, std::string comment){
		Session * ses = new Session();
		ses->setName(name);
		ses->setSession(session);
		ses->setComment(comment);
		ses->setGroup(false);
	
		if(ses->isVaild()){
			addNode(ses);
		}
		else{
			delete ses;
		}
		return;
	}

	void SessionMgr::output(){
		Session * ses = NULL;
		std::cout  << "# session limit" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			ses = (Session *)node_list.at(i);
			ses->output();
		}
		return;

	}
}

