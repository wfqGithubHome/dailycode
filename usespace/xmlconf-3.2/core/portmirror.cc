#include "portmirror.h"
#include "../global.h"
#include "portmirrormgr.h"
#include <sstream>

namespace maxnet{

	PortMirror::PortMirror(){
		bridge_id = 0;
	}

	PortMirror::~PortMirror(){
		
	}
	
	bool PortMirror::isVaild(){
		if(getName().size() <= 0) return false;
		if(getSrcAddr().size() <=0) return false;
		if(getDstAddr().size() <=0) return false;		
		if(getAction().compare("in") != 0 && getAction().compare("out") != 0 && getAction().compare("all") != 0
		  &&getAction().compare("in_discard") != 0 && getAction().compare("out_discard") != 0 && getAction().compare("all_discard") != 0)
		  return false;
		return true;
	}

	void PortMirror::output(){
		std::cout << "# \"" << getName() << "\"\tsrc_addr=\"" << getSrcAddr() << "\", dst_addr=\"" << getDstAddr() 
			<< "\", bridge_id=\"" << getBridgeID() << "\", layer7=" << getProto() 
			<< ", action=\"" << getAction() << "\"\t\t// " << getComment() << std::endl;

		return;

	}

	void PortMirror::setBridgeID(int bridge_id)
	{
		this->bridge_id = bridge_id;
	}

	void PortMirror::setBridgeIDbyString(std::string bridge_id_string)
	{
		std::stringstream bridge_id;
		bridge_id << bridge_id_string;
		bridge_id >> this->bridge_id;
		if(this->bridge_id >= MAX_BRIDGE_ID || this->bridge_id < 0)
			this->bridge_id = 0;
	}

	void PortMirror::setSrcAddr(std::string addr)
	{
		this->src_addr = addr;
	}

	void PortMirror::setDstAddr(std::string addr)
	{
		this->dst_addr = addr;
	}

	void PortMirror::setProto(std::string proto)
	{
		this->proto = proto;
	}	

	void PortMirror::setAction(std::string action)
	{
		this->action = action;
	}

	std::string PortMirror::getSrcAddr()
	{
		return this->src_addr;
	}

	std::string PortMirror::getDstAddr()
	{
		return this->dst_addr;
	}

	std::string PortMirror::getProto()
	{
		return this->proto;
	}

	std::string PortMirror::getAction()
	{
		return this->action;
	}

	int			PortMirror::getBridgeID()
	{
		return this->bridge_id;
	}
}


