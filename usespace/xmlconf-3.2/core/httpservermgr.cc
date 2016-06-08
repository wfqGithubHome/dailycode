#include "../global.h"
#include "httpservermgr.h"
#include "httpservernode.h"

namespace maxnet{

	HttpServerMgr::HttpServerMgr(){

	}

	HttpServerMgr::~HttpServerMgr(){
#if 1
		HttpServerNode* node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = (HttpServerNode *)node_list.at(i);
			if(!node->isGroup())
				delete node;
		}
		node_list.clear();
		return;
#endif
	}
	
	void HttpServerMgr::add(std::string name,std::string bridgeid,
						std::string ipaddr, std::string servertype,
						std::string prefix, std::string suffix,
						std::string comment){
		HttpServerNode* node = new HttpServerNode();

		node->setName(name);
		node->setBridgeId(bridgeid);
		node->setIPAddr(ipaddr);
		node->setServerType(servertype);
		node->setPrefix(prefix);
		node->setSuffix(suffix);
		node->setComment(comment);
		
		if(node->isVaild()){
			addNode(node);
		}
		else{
			delete node;
		}
		return;
	}
	
	int HttpServerMgr::getGroupID(std::string groupname){
		int grpid = 0;
		Node * node = NULL;
		bool bflag = false;

		group_node_ptr = NULL;
		sub_node_index = 0;

		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			
			if(node->isGroup()){
				++grpid;
			}
			if(groupname.compare(node->getName()) == 0 && node->isGroup()){
				bflag = true;
				break;
			}
		}		

		return bflag ? grpid : 0;		
	}
		

	void HttpServerMgr::output(){

		HttpServerNode* node = NULL;
		
		std::cout  << std::endl << "# Http Server Info Data" << std::endl<<std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			node = (HttpServerNode *)node_list.at(i);
			node->output();
		}
		
		return;
	}
}


