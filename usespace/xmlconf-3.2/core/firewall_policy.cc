#include "firewall_policy.h"
#include "../global.h"
#include <sstream>
namespace maxnet{

	FirewallPolicy::FirewallPolicy(){
		id = 1;
	}

	FirewallPolicy::~FirewallPolicy(){
		
	}
	
	bool FirewallPolicy::isVaild(){
		if(getIface().size() <=0) return false;
		return true;
	}

	void FirewallPolicy::output(){
		std::cout << "# \"id=" << getId() << "\"\tenable=\"" << (getEnable()?1:0) 
			<< "\", src=" << getSrc() << ", dst=\"" << getDst() << "\", isp=\"" << getIsp() << "\", proto=\"" << getProto() 
			<< "\", iface=\"" << getIface() << "\", action=\"" << getAction() << "\", timed=\""
			<< getTimed()<<"\", week=\"" << getWeek() << "\", day=\"" << getDay() << "\", in_limit=\"" << getInLimit() << "\", out_limit=\"" 
			<< getOutLimit()  << "\", comment=\"" << getComment() << "\"\t\t// " << std::endl;

		return;

	}

	void FirewallPolicy::setId(std::string id){
		this->id = id;
	}
	void FirewallPolicy::setEnable(bool enable){
		this->enable = enable;
	}
	void FirewallPolicy::setSrc(std::string src){
		this->src = src;
	}
	void FirewallPolicy::setDst(std::string dst){
		this->dst = dst;
	}
	void FirewallPolicy::setIsp(std::string isp){
		this->isp = isp;
	}
	void FirewallPolicy::setProto(std::string proto){
		this->proto = proto;
	}
	void FirewallPolicy::setSession(std::string session){
		this->session = session;
	}
	void FirewallPolicy::setIface(std::string iface){
		this->iface = iface;
	}
	void FirewallPolicy::setAction(std::string action){
		this->action = action;
	}
	void FirewallPolicy::setTimed(std::string timed){
		this->timed = timed;
	}
	void FirewallPolicy::setWeek(std::string week){
		this->week = week;
	}
	void FirewallPolicy::setDay(std::string day){
		this->day = day;
	}
    void FirewallPolicy::setInLimit(std::string in_limit){
		this->in_limit  = in_limit;
	}
    void FirewallPolicy::setOutLimit(std::string out_limit){
		this->out_limit = out_limit;
	}
	void FirewallPolicy::setComment(std::string comment){
		this->comment = comment;
	}

	std::string FirewallPolicy::getId(){
		return id;
	}
	bool FirewallPolicy::getEnable(){
		return enable;
	}
	std::string FirewallPolicy::getSrc(){
		return src;
	}
	std::string FirewallPolicy::getDst(){
		return dst;
	}
	std::string FirewallPolicy::getIsp(){
		return isp;
	}
	std::string FirewallPolicy::getProto(){
		return proto;
	}
	std::string FirewallPolicy::getSession(){
		return session;
	}
	std::string FirewallPolicy::getIface(){
		return iface;
	}
	std::string FirewallPolicy::getAction(){
		return action;
	}
	std::string FirewallPolicy::getTimed(){
		return timed;
	}
	std::string FirewallPolicy::getWeek(){
		return week;
	}
	std::string FirewallPolicy::getDay(){
		return day;
	}
    std::string FirewallPolicy::getInLimit(){
		return in_limit;
	}
    std::string FirewallPolicy::getOutLimit(){
		return out_limit;
	}
	std::string FirewallPolicy::getComment(){
		return comment;
	}
}

