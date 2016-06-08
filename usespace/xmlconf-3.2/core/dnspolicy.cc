#include "dnspolicy.h"
#include "domainmgr.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	DnsPolicy::DnsPolicy(){
		bridge_id = 0;
	}

	DnsPolicy::~DnsPolicy(){
		
	}
	
	bool DnsPolicy::isVaild(){
		if(getName().size() <= 0) return false;
		if(getBridgeId() <0) return false;
		if( getAction() <= 0 ) return false;
		if(getAction() != DPI_DNS_DROP && getTargetip().size()==0) return false;
		if(getDomainName().size() <= 0) return false;
		return true;
	}

	void DnsPolicy::output(/*DomainMgr* domainmgr*/){
        //<dataitem name="1" bridge="0" src="all" dst="all" domain_name="any" action="redirect" targetip="localip"  disabled="0"  comment="redirect Action" />

		std::cout << "# actionid=\"" << getName() << "\", style=" << getAction() << "\", bridgeid=\"" << getBridgeId()
					<<"\", src=\""<<getSrc() << "\", dst=\"" << getDst() 
					<<"\", ipaddr=\""<<getTargetip() << "\", domain_name=\"" << getDomainName() 
					<<"\"\t\t// " << getComment() << std::endl;

		return;
	}

	void DnsPolicy::setBridgeId(std::string id){
		std::stringstream bridge_id_string;
		bridge_id_string << id;
		bridge_id_string >> this->bridge_id;
		if(this->bridge_id > 4 || this->bridge_id < 0)
			this->bridge_id = 0;	
	}


	void DnsPolicy::setAction(std::string action){
		this->action = action;
	}

	void DnsPolicy::setSrc(std::string src){
		this->src = src;
	}

	void DnsPolicy::setDst(std::string dst){
		this->dst = dst;
	}
	void DnsPolicy::setDomainName(std::string domainname){
		this->domain_name = domainname;
	}


	void DnsPolicy::setTargetip(std::string  targetip){
		this->targetip = targetip;
	}
	void DnsPolicy::setRawName(std::string rawname){
		this->raw_name= rawname;
	}

	int DnsPolicy::getAction(){
		int ret = -1;
		if(action.compare("1") == 0){
			ret = DPI_DNS_REDIRECT;
		}
		else if(action.compare("3") == 0){
			ret = DPI_DNS_HIJACK;
		}
		else if(action.compare("2") == 0){
			ret = DPI_DNS_DROP;
		}
			
		return ret;
	}

	std::string DnsPolicy::getDomainName(){
		return domain_name;
	}

	std::string DnsPolicy::getSrc(){
		return src;
	}

	std::string DnsPolicy::getDst(){
		return dst;
	}
	std::string DnsPolicy::getTargetip(){
		return targetip;
	}

	int	DnsPolicy::getBridgeId(){
		return this->bridge_id;
	}

	std::string DnsPolicy::getBridgeIdString() {
		std::stringstream bridge_id_string;
		bridge_id_string << this->bridge_id;	
		std::string id_string;
		bridge_id_string >> id_string;
		//std::cout << "getBridgeIdString: " << id_string << std::endl;
		return id_string;
	}
		
	void DnsPolicy::setEnable(bool enable){
		this->policy_enable = enable;
	}
	
	bool DnsPolicy::getEnable(){
		return this->policy_enable;
	}
	
	std::string DnsPolicy::getRawName(){
		return raw_name;
	}
}

