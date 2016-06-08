#include "servconf.h"
#include "../global.h"

namespace maxnet{

	ServConf::ServConf()
    {

	}

	ServConf::~ServConf()
    {
		
	}
	
	bool ServConf::isVaild()
    {
		if(type.size() <= 0) return false;
		return true;
	}

    std::string ServConf::getType()
    {
        return type;
    }

    std::string ServConf::getEnTypeName()
    {
        return en_name;
    }
    
    std::string ServConf::getEnZoneName()
    {
        return en_zonename;
    }

    void ServConf::setType(std::string type)
    {
        this->type = type;
    }
    
    void ServConf::setEnTypeName(std::string en_name)
    {
        this->en_name = en_name;
    }
     
    void ServConf::setEnZoneName(std::string en_zonename)
    {
        this->en_zonename = en_zonename;
    }

}


