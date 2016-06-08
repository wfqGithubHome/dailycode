#include "route_pptpd.h"
#include "../global.h"

namespace maxnet{

	Pptpd::Pptpd(){
		startip = "";
		endip = "";
		localip = "";
		entry = "";
		pptpdenable = false;
	}

	Pptpd::~Pptpd(){
		
	}

	bool Pptpd::isVaild(){
		if(localip.size() <= 0) return false;
		if(startip.size() <= 0) return false;
		if(endip.size() <= 0) return false;
		if(entry.size() <= 0) return false;
		return true;
	}

	void Pptpd::setPptpdEnable(bool pptpdenable){
		this->pptpdenable = pptpdenable;
	}

	void Pptpd::setPptpdlocalip(std::string localip){
		this->localip = localip;
	}

	void Pptpd::setPptpdstartip(std::string startip){
		this->startip = startip;
	}

	void Pptpd::setPptpdendip(std::string endip){
		this->endip = endip;
	}

	void Pptpd::setPptpddns(std::string dns){
		this->dns = dns;
	}

	void Pptpd::setPptpdentry(std::string entry){
		this->entry = entry;
	}

	bool Pptpd::getPptpdEnable(){
		return pptpdenable;
	}

	std::string Pptpd::getPptpdlocalip(){
		return localip;
	}

	std::string Pptpd::getPptpdstartip(){
		return startip;
	}

	std::string Pptpd::getPptpdendip(){
		return endip;
	}

	std::string Pptpd::getPptpddns(){
		return dns;
	}

	std::string Pptpd::getPptpdentry(){
		return entry;
	}
}
