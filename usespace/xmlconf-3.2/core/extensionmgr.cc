#include "extensionmgr.h"
#include "extensionnode.h"
#include "../global.h"

namespace maxnet{

	ExtensionMgr::ExtensionMgr(){
		add_id = 1;
	}

	ExtensionMgr::~ExtensionMgr(){	
#if 1
		ExtensionNode * node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = (ExtensionNode *)node_list.at(i);
			if(!node->isGroup())
				delete node;
		}
		node_list.clear();
#endif
		return;
	}

	void ExtensionMgr::add(std::string name, std::string value, std::string comment){
		ExtensionNode* node = new ExtensionNode();
		node->setName(name);
		node->setValue(value);
		node->setComment(comment);
		node->setGroup(false);
		node->setID(add_id);
		if(node->isVaild()){
			addNode(node);
			add_id ++;
		}
		else{
			delete node;
		}
		return;
	}

	void ExtensionMgr::output(){
		ExtensionNode * node = NULL;
		std::cout << std::endl << "# http direction extension and extension group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			node = (ExtensionNode *)node_list.at(i);
			node->output();
		}
			
		return;
	}
}

