#include "layer7prototype.h"
#include "../global.h"
#include "layer7prototypemgr.h"
#include <sstream>

namespace maxnet{

	Layer7ProtoType::Layer7ProtoType(){
	}

	Layer7ProtoType::~Layer7ProtoType(){
		
	}
	
	bool Layer7ProtoType::isVaild(){
		if(getName().size() <= 0) return false;
		if(getValue().size() <=0) return false;
		
		return true;
	}

	void Layer7ProtoType::output(){
		std::cout << "# \"" << getName() << "\"\tvalue=\"" << getValue() << std::endl;
	}

	void Layer7ProtoType::setValue(std::string value)
	{		
		this->value = value;
	}

	std::string Layer7ProtoType::getValue()
	{
		return this->value;
	}
}


