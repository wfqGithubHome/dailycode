#include "urlformatnode.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	UrlFormatNode::UrlFormatNode(){
		bridge_id = 0;
	}

	UrlFormatNode::~UrlFormatNode(){
		
	}
	
	bool UrlFormatNode::isVaild(){
		if(getFormat().size() <= 0) return false;
		
		return true;
	}

	void UrlFormatNode::output(){
		
		std::cout << "# bridgeid=" << getBridgeId()
					<<", format=\""<<getFormat() 
					<<"\",comment=" << getComment()<< std::endl;
	}

	void UrlFormatNode::setBridgeId(std::string id){
		std::stringstream bridge_id_string;
		bridge_id_string << id;
		bridge_id_string >> this->bridge_id;
		if(this->bridge_id > 4 || this->bridge_id < 0)
			this->bridge_id = 0;	
	}


	void UrlFormatNode::setFormat(std::string  format){
		this->format = format;
	}

	std::string UrlFormatNode::getFormat(){
		return format;
	}

	int	UrlFormatNode::getBridgeId(){
		return this->bridge_id;
	}

	std::string UrlFormatNode::getBridgeIdString() {
		std::stringstream bridge_id_string;
		bridge_id_string << this->bridge_id;	
		std::string id_string;
		bridge_id_string >> id_string;
		//std::cout << "getBridgeIdString: " << id_string << std::endl;
		return id_string;
	}
		
}

