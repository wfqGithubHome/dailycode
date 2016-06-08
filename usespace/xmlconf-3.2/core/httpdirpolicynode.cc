#include "httpdirpolicynode.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	HttpDirPolicy::HttpDirPolicy(){
		bridge_id = 0;
		this_id = 0;
	}

	HttpDirPolicy::~HttpDirPolicy(){
		
	}
	
	bool HttpDirPolicy::isVaild(){
		if(extgroup.size() <= 0) return false;
		if(getBridgeId() <0) return false;
		if(servergroup.size() <= 0) return false;
		return true;
	}

	void HttpDirPolicy::output(){

		std::cout << "# bridgeid=" << getBridgeId()
					<< ", extgroup=\"" <<getExtgroup() 
					<< "\", servergroup=\"" << getServergroup() 
					<< "\", comment=" << getComment()
					<< std::endl;


		return;

	}

	void HttpDirPolicy::setBridgeId(std::string id){
		std::stringstream bridge_id_string;
		bridge_id_string << id;
		bridge_id_string >> this->bridge_id;
		if(this->bridge_id > 4 || this->bridge_id < 0)
			this->bridge_id = 0;	
	}


	void HttpDirPolicy::setExtgroup(std::string extgroup){
		this->extgroup = extgroup;
	}

	void HttpDirPolicy::setServergroup(std::string servergroup){
		this->servergroup = servergroup;
	}

	std::string HttpDirPolicy::getExtgroup(){
		return extgroup;
	}

	std::string HttpDirPolicy::getServergroup(){
		return servergroup;
	}

	int	HttpDirPolicy::getBridgeId(){
		return this->bridge_id;
	}

	std::string HttpDirPolicy::getBridgeIdString() {
		std::stringstream bridge_id_string;
		bridge_id_string << this->bridge_id;	
		std::string id_string;
		bridge_id_string >> id_string;
		//std::cout << "getBridgeIdString: " << id_string << std::endl;
		return id_string;
	}
	
	void HttpDirPolicy::setID(uint32_t id){
		this->this_id = id;
	}

	uint32_t HttpDirPolicy::getID(){
		return this_id;
	}
}

