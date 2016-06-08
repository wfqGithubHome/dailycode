#include "firewall.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	Firewall::Firewall(){
		bridge_id = 0;
	}

	Firewall::~Firewall(){
		
	}
	
	bool Firewall::isVaild(){
		if(getName().size() <= 0) return false;
		if(getProto().size() <=0) return false;
		if(getAction().compare("allow")!= 0 && getAction().compare("deny") != 0) return false;
		return true;
	}

	void Firewall::output(){
		std::cout << "# \"" << getName() << "\"\tproto=\"" << getProto() 
			<< "\", action=" << getAction() << ", sche=\"" << getSchedule() << "\", vlan=\"" << getVlan() << "\", src=\"" << getSrc() 
			<< "\", src_invent=\"" << src_invent << "\", dst=\"" << getDst() << "\", dst_invent=\""
			<< dst_invent <<"\", mac=\"" << getMac() << "\", mac_invent=\"" << mac_invent<< "\", in_traffic=\"" << getInTraffic() << "\", out_traffic=\"" 
			<< getOutTraffic() << "\", auth=\"" << getAuth() << "\", bridge=\"" << getBridgeId() << "\"\t\t// " << getComment() << std::endl;

		return;

	}

	void Firewall::setSrcInvent(bool invent){
		this->src_invent = invent;
	}

	void Firewall::setDstInvent(bool invent){
		this->dst_invent = invent;
	}

	void Firewall::setMacInvent(bool invent){
		this->mac_invent = invent;
	}
	bool Firewall::getSrcInvent(){
		return this->src_invent;
	}


	bool Firewall::getDstInvent(){
		return this->dst_invent;
	}
	bool Firewall::getMacInvent(){
		return this->mac_invent;
	}
	
	void Firewall::setAuth(std::string auth)
	{
		this->auth = auth;
	}

	void Firewall::setBridgeId(std::string id){
		std::stringstream bridge_id_string;
		bridge_id_string << id;
		bridge_id_string >> this->bridge_id;
		if(this->bridge_id > 4 || this->bridge_id < 0)
			this->bridge_id = 0;	
	}

	void Firewall::setLogPacket(bool log_packet){
		this->log_packet = log_packet;
	}

	void Firewall::setProto(std::string proto){
		this->proto = proto;
	}

	void Firewall::setAction(std::string action){
		this->action = action;
	}

	void Firewall::setSrc(std::string src){
		this->src = src;
	}

	void Firewall::setDst(std::string dst){
		this->dst = dst;
	}
	void Firewall::setMac(std::string mac){
		this->mac = mac;
	}

	void Firewall::setSchedule(std::string schedule){
		this->schedule = schedule;
	}

	void Firewall::setInTraffic(std::string in_traffic){
		this->in_traffic = in_traffic;
	}


	void Firewall::setOutTraffic(std::string out_traffic){
		this->out_traffic = out_traffic;
	}

	void Firewall::setSessionLimit(std::string session_limit){
		this->session_limit = session_limit;
	}

	void Firewall::setActionID(int  action_id){
		this->action_id = action_id;
	}
	
	void	Firewall::setQuota(std::string quota){
		this->quota = quota;
	}
	
	void Firewall::setQuotaAction(std::string quota_action){
		this->quota_action = quota_action;
	}
	
	void Firewall::setSencondInTraffic(std::string second_in_traffic){
		this->second_in_traffic = second_in_traffic;
	}
	
	void Firewall::setSencondOutTraffic(std::string second_out_traffic){
		this->second_out_traffic = second_out_traffic;
	}

	std::string Firewall::getAuth(){
		return auth;
	}

	std::string Firewall::getProto(){
		return proto;
	}

	std::string Firewall::getAction(){
		return action;
	}

	std::string Firewall::getSchedule(){
		return schedule;
	}

	std::string Firewall::getSrc(){
		return src;
	}

	std::string Firewall::getDst(){
		return dst;
	}
	std::string Firewall::getMac(){
		return mac;
	}

	std::string Firewall::getInTraffic(){
		return in_traffic;
	}

	std::string Firewall::getOutTraffic(){
		return out_traffic;
	}

	std::string Firewall::getSessionLimit(){
		return session_limit;
	}

	bool		Firewall::getLogPacket(){
		return log_packet;
	}

	int  Firewall::getActionID(){
		return action_id;
	}
	
	std::string Firewall::getQuota(){
		return quota;
	}
	
	std::string Firewall::getQuotaAction(){
		return quota_action;
	}
	
	std::string Firewall::getSencondInTraffic(){
		return second_in_traffic;
	}
	
	std::string Firewall::getSencondOutTraffic(){
		return second_out_traffic;
	}

	std::string Firewall::getVlan(){
		return vlan;
	}

	void Firewall::setVlan(std::string vlan){
		this->vlan = vlan;
	}

	int	Firewall::getBridgeId(){
		return this->bridge_id;
	}

	std::string Firewall::getBridgeIdString() {
		std::stringstream bridge_id_string;
		bridge_id_string << this->bridge_id;	
		std::string id_string;
		bridge_id_string >> id_string;
		//std::cout << "getBridgeIdString: " << id_string << std::endl;
		return id_string;
	}
}

