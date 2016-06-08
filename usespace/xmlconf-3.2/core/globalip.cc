#include "globalip.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	GlobalIP::GlobalIP(){		
		addr = "";
        dst = "";
		per_ip_session_limit = "";
		total_session_limit = "";
		action = "";
		enable_http_log = false;
        enable_sess_log = false;
        enable_auth = false;
        dstip_type = 0;
		bridge_id = 0;
		enable_httpdirpolicy = 0;
		enable_dnshelperpolicy = 0;
	}

	GlobalIP::~GlobalIP(){
		
	}
	
	bool GlobalIP::isVaild(){
		if(getName().size() <= 0) return false;
		if(getAddr().size() <=0) return false;
		if(getAction().compare("bypass") != 0 && getAction().compare("block") != 0 && getAction().compare("trusted") != 0) return false;
		return true;
	}

	void GlobalIP::output(){
		std::cout << "# \"" << getName() << "\"\taddr=\"" << getAddr() << "\", dst=\"" << getDst()
			<< "\", bridge_id=\"" << getBridgeID() << "\", httlog=" << getEnableHTTPLog() << ", per_ip_ses_le=\"" << getPerIPSessionLimit() << "\", total_ses_le=\"" 
			<< getTotalSessionLimit()  << ", httpdirpolicy=" << getEnableHTTPDirPolicy() << ", action=\"" << getAction() << "\"\t\t// " << getComment() << std::endl;

		return;

	}

	void GlobalIP::setBridgeID(int bridge_id)
	{
		this->bridge_id = bridge_id;
	}

	void GlobalIP::setBridgeIDbyString(std::string bridge_id_string)
	{
		std::stringstream bridge_id;
		bridge_id << bridge_id_string;
		bridge_id >> this->bridge_id;
		if(this->bridge_id >= 10 || this->bridge_id < 0)
			this->bridge_id = 0;
	}

	void GlobalIP::setAddr(std::string addr){
		this->addr = addr;
	}
	
	void GlobalIP::setDst(std::string dst){
		this->dst = dst;
	}
    
	void GlobalIP::setDstIP_type(int dstip_type){
		this->dstip_type = dstip_type;
	}
	void GlobalIP::setdstiptypebyString(std::string dstip_type_string)
	{
		std::stringstream dstip_typestream;
		dstip_typestream << dstip_type_string;
		dstip_typestream >> this->dstip_type;
		if(this->dstip_type >= 3 || this->dstip_type < 0)
			this->dstip_type = 0;
	}
	void GlobalIP::setPerIPSessionLimit(std::string limit)
	{
		this->per_ip_session_limit = limit;
	}

	void GlobalIP::setTotalSessionLimit(std::string limit)
	{
		this->total_session_limit = limit;
	}

	void GlobalIP::setEnableHTTPLog(bool enable)
	{
		this->enable_http_log = enable;
	}

	void GlobalIP::setEnableSessLog(bool enable)
	{
		this->enable_sess_log = enable;
	}

	void GlobalIP::setEnableAuth(bool enable)
	{
		this->enable_auth = enable;
	}
	
	void GlobalIP::setAction(std::string action)
	{
		this->action = action;
	}

	void GlobalIP::setEnableHTTPDirPolicy(bool enable)
	{
		this->enable_httpdirpolicy = enable ? 1 :0;
	}

	void GlobalIP::setEnableDnsHelperPolicy(bool enable)
	{
		this->enable_dnshelperpolicy= enable ? 1 :0;
	}

	std::string GlobalIP::getAddr()
	{
		return this->addr;
	}

    std::string GlobalIP::getDst()
	{
		return this->dst;
	}

	int			GlobalIP::getDstIP_type()
	{
		return this->dstip_type;
	}
	
	std::string GlobalIP::getPerIPSessionLimit()
	{
		return this->per_ip_session_limit;
	}

	std::string GlobalIP::getTotalSessionLimit()
	{
		return this->total_session_limit;
	}

	bool GlobalIP::getEnableHTTPLog()
	{
		return this->enable_http_log;
	}
	
	bool GlobalIP::getEnableSessLog()
	{
		return this->enable_sess_log;
	}

	bool GlobalIP::getEnableAuth()
	{
		return this->enable_auth;
	}
	
	std::string GlobalIP::getAction()
	{
		return this->action;
	}

	int			GlobalIP::getBridgeID()
	{
		return this->bridge_id;
	}
	
	int GlobalIP::getEnableHTTPDirPolicy()
	{
		return this->enable_httpdirpolicy;
	}
	int GlobalIP::getEnableDnsHelperPolicy()
	{
		return this->enable_dnshelperpolicy;
	}
}

