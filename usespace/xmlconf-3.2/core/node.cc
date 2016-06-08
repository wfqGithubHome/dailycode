#include "node.h"
#include "../global.h"

namespace maxnet{

	Node::Node(){
		isgroup = false;

		disabled = false;
	}

	Node::~Node(){
		subnode.clear();
	}
	

	void Node::setName(std::string name){
		this->name = name;
	}


	std::string Node::getName(){
		return name;
	}

  



	void Node::setComment(std::string comment){
		this->comment = comment;
	}


	std::string Node::getComment(){
		return comment;
	}

	void Node::setGroup(bool isgroup){
		this->isgroup = isgroup;
	}

	bool Node::isGroup(){
		return isgroup;
	}

	void Node::addSubNode(std::string name){
		subnode.push_back(name);
	}

	std::vector<std::string> Node::getSubNodes(){
		return subnode;
	}


	void Node::setDisabled(int disabled){
		this->disabled = disabled;
	}

	int Node::getDisabled(){
		return this->disabled;
	}

}

