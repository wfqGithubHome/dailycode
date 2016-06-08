#include "httpservernode.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	HttpServerNode::HttpServerNode(){
		//groupid = 0;
		bridge_id = 0;
		ip_addr = "";
		servertype = "";
		prefix = "";
		suffix = "";
	}

	HttpServerNode::~HttpServerNode(){
		
	}
	
	bool HttpServerNode::isVaild(){
		if(getIPAddr().size() <= 0) return false;
		if(getServerType().size() <= 0) return false;
		
		return true;
	}

	void HttpServerNode::output(){		
		std::cout << "# name=\"" << getName()
				  << ", ip_addr=\"" << getIPAddr() 
				  << "\", servertype=\""<<getServerType()
				  << "\", prefix=\"" << getPrefix()
				  << "\", suffix=\"" << getSuffix()<< "\"" 
				  << ", comment=\"" << getComment()<<"\""
				  << std::endl;
	}
	
	void HttpServerNode::setBridgeId(std::string id){
		std::stringstream bridge_id_string;
		bridge_id_string << id;
		bridge_id_string >> this->bridge_id;
		if(this->bridge_id > 4 || this->bridge_id < 0)
			this->bridge_id = 0;	
	}
	void HttpServerNode::setServerType(std::string servertype){
		this->servertype = servertype;
	}
	void HttpServerNode::setIPAddr(std::string  ipaddr){
		this->ip_addr = ipaddr;
	}

	void HttpServerNode::setPrefix(std::string  prefix){
		this->prefix = prefix;
	}
	void HttpServerNode::setSuffix(std::string  suffix){
		this->suffix = suffix;
	}

	/*void HttpServerNode::setGroupID(int grpid){
		this->groupid = grpid;
	}

	int HttpServerNode::getGroupID(){
		return groupid;
	}*/
		
	int HttpServerNode::getBridgeId(){
		return this->bridge_id;
	}
	
	std::string HttpServerNode::getBridgeIdString() {
		std::stringstream bridge_id_string;
		bridge_id_string << this->bridge_id;	
		std::string id_string;
		bridge_id_string >> id_string;
		return id_string;
	}
		

	std::string HttpServerNode::getIPAddr(){
		return ip_addr;
	}

	std::string HttpServerNode::getServerType(){
		return servertype;
	}
	std::string HttpServerNode::getPrefix(){
		return prefix;
	}
	std::string HttpServerNode::getSuffix(){
		return suffix;
	}

		
}

