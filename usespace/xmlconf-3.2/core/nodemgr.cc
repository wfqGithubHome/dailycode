#include "nodemgr.h"
#include "../global.h"

namespace maxnet{

	NodeMgr::NodeMgr(){
		group_node_ptr = NULL;
		sub_node_index = 0;

		groupNodeCount = 0;		//group对象统计
		includeNodeCount = 0;	//include对象统计
		dataNodeCount = 0;		//数据对象统计
	}

	NodeMgr::~NodeMgr(){
#if 1	
		Node * node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			delete node;
		}
		node_list.clear();

		group_node_ptr = NULL;
		sub_node_index = 0;
	
		groupNodeCount = 0; 	//group对象统计
		includeNodeCount = 0;	//include对象统计
		dataNodeCount = 0;		//数据对象统计
#endif
		return;
	}

	void NodeMgr::Clear(){
		group_node_ptr = NULL;
		sub_node_index = 0;
	
		groupNodeCount = 0; 	//group对象统计
		includeNodeCount = 0;	//include对象统计
		dataNodeCount = 0;		//数据对象统计

		Node * node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			delete node;
		}
		node_list.clear();
		return;
	}
	
	void	NodeMgr::addNode(Node * node){
		node_list.push_back(node);
		++dataNodeCount;
	}

	void	NodeMgr::addRouteNode(Node * node){
		node_list.push_back(node);
		++groupNodeCount;
	}

	void	NodeMgr::addGroup(std::string name, std::string comment){
		Node * node = new Node();
		node->setName(name);
		node->setComment(comment);
		node->setGroup(true);
		node_list.push_back(node);
		
		++groupNodeCount;
		//printf("name=%s , groupNodeCount =%d \n" , name.c_str(), groupNodeCount);
	}

	int		NodeMgr::getCount(){
		return node_list.size();
	}

	void	NodeMgr::addSubNode(std::string group_name, std::string sub_name){
		Node * node = NULL;
		Node * tmp_node = NULL;

		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			if(group_name.compare(node->getName()) == 0){
				for(unsigned int j = 0; j < node_list.size(); j++)
				{
					tmp_node = node_list.at(j);
					if(sub_name.compare(tmp_node->getName()) == 0)
					{
						if(tmp_node->isGroup())
						{
							std::vector<std::string> subnodes;
							
							subnodes = tmp_node->getSubNodes();
							for(unsigned int k = 0; k < subnodes.size(); k++){
								node->addSubNode(subnodes.at(k));
							}
						}
						else
						{
							node->addSubNode(sub_name);
						}
					}

				}

			}
			//node_list.erase((std::vector<std::string>::iterator)i);
			//node_list.push_back(node);
		}
		
		++includeNodeCount;
	}


	void	NodeMgr::addOnlySubNode(std::string group_name, std::string sub_name){
		Node * node = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			if(group_name.compare(node->getName()) == 0){
				node->addSubNode(sub_name);
			}
		}
		
		++includeNodeCount;
	}


	std::vector<Node *> NodeMgr::getList(){
		return node_list;
	}

	Node *  NodeMgr::find(std::string name){
		Node * node = NULL;

		group_node_ptr = NULL;
		sub_node_index = 0;

		std::vector<std::string> subnodes;
		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			if(name.compare(node->getName()) == 0){
				if(node->isGroup()){
					group_node_ptr = node;
					subnodes = node->getSubNodes();

					for(unsigned int j = 0; j < subnodes.size(); j++){
						for(unsigned int k = 0; k < node_list.size(); k++){
							node = node_list.at(k);
							if(node->getName().compare(subnodes.at(j)) == 0){
								
								sub_node_index = j;

								return node;
							}
						}
					}
					return NULL;

				}
				else{
					return node;
				}
			}
		}
		return NULL;
	}

	Node *  NodeMgr::Onlyfind(std::string name){
		Node * node = NULL;
		std::vector<std::string> subnodes;
		
		for(unsigned int i=0; i < node_list.size(); i++){
			node = node_list.at(i);
			if(name.compare(node->getName()) == 0){
				if(node->isGroup()){
					return node;
				}
				else{
					return NULL;
				}
			}
		}
		return NULL;
	}
	
	Node * NodeMgr::next(){
		int i = -1;
		return next(NULL, i);
	}

	int		NodeMgr::getSubIndex(){
		return sub_node_index;
	}

	Node *  NodeMgr::getCurrGroupNode(){
		return group_node_ptr;
	}

	Node * NodeMgr::next(Node * ptr, int index){
		if(ptr != NULL) group_node_ptr = ptr;
		if(group_node_ptr == NULL) return NULL;

		Node * node = group_node_ptr;

		std::vector<std::string> subnodes;

		if(index != -1){
			sub_node_index = index;
		}
		if(!node->isGroup()) return NULL;

		subnodes = node->getSubNodes();

		for(unsigned int i = sub_node_index + 1; i < subnodes.size(); i++){

			for(unsigned int j = 0; j < node_list.size(); j++){
				node = node_list.at(j);
				if(node->getName().compare(subnodes.at(i)) == 0){
						sub_node_index = i;
						return node;
				}
			}
		}
	
		return NULL;
	}
}

