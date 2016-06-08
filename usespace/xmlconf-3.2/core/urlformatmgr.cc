#include "urlformatnode.h"
#include "urlformatmgr.h"
#include "../global.h"

namespace maxnet{

	UrlFormatMgr::UrlFormatMgr(){

	}

	UrlFormatMgr::~UrlFormatMgr(){
#if 1
		UrlFormatNode* node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = (UrlFormatNode *)node_list.at(i);
			if(!node->isGroup())
				delete node;
		}
		node_list.clear();
#endif
		return;
	}
	
	void UrlFormatMgr::add(std::string bridge_id, 
						std::string format,
						std::string comment){
		UrlFormatNode* node = new UrlFormatNode();
		
		node->setBridgeId(bridge_id);
		node->setFormat(format);
		node->setComment(comment);
		
		if(node->isVaild()){
			addNode(node);
		}
		else{
			delete node;
		}
		return;
	}

	void UrlFormatMgr::output(){

		UrlFormatNode* node = NULL;
		
		std::cout  << std::endl << "# Url format node" << std::endl<<std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			node = (UrlFormatNode *)node_list.at(i);
			node->output();
		}

		
		return;

	}
}


