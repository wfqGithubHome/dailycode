#include "servinfo.h"
#include "../global.h"

namespace maxnet{

	ServInfo::ServInfo(){

	}

	ServInfo::~ServInfo(){
		
	}
	
	bool ServInfo::isVaild(){
		if(type.size() <= 0) return false;
		return true;
	}

    std::string ServInfo::getType()
    {
       return type;
    }
    
    std::string ServInfo::getTypeName()
    {
       return name;
    }

    std::string ServInfo::getEnTypeName()
    {
       return en_name;
    }
     
    std::string ServInfo::getZoneName()
    {
        return zonename;
    }

    std::string ServInfo::getEnZoneName()
    {
        return en_zonename;
    }
        
    std::string ServInfo::getVar1()
    {
        return var1;
    }

    std::string ServInfo::getVar2()
    {
        return var2;
    }

    std::string ServInfo::getVar3()
    {
        return var3;
    }

    std::string ServInfo::getVar4()
    {
        return var4;
    }

    std::string ServInfo::getComment()
    {
        return comment;
    }

    void ServInfo::setType(std::string type)
    {
         this->type = type;
    }
    
    void ServInfo::setTypeName(std::string name)
    {
         this->name = name;
    }

    void ServInfo::setEnTypeName(std::string en_name)
    {
         this->en_name = en_name;
    }

    void ServInfo::setZoneName(std::string zonename)
    {
        this->zonename = zonename;
    }

     void ServInfo::setEnZoneName(std::string en_zonename)
    {
        this->en_zonename = en_zonename;
    }
    
    void ServInfo::setVar1(std::string var1)
    {
        this->var1 = var1;
    }
    
    void ServInfo::setVar2(std::string var2)
    {
        this->var2 = var2;
    }

    void ServInfo::setVar3(std::string var3)
    {
        this->var3 = var3;
    }
    
    void ServInfo::setVar4(std::string var4)
    {
        this->var4 = var4;
    }

    void ServInfo::setComment(std::string comment)
    {
        this->comment = comment;
    }
    
}

