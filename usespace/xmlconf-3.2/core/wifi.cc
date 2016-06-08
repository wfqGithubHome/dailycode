#include "wifi.h"
#include "../global.h"

namespace maxnet{

	Wifi::Wifi(){

	}

	Wifi::~Wifi(){

	}

	bool Wifi::adminisvalid(){
		if(wifiadminusername.size() <= 0) return false;
		if(wifiadminpasswd.size() <= 0) return false;
		return true;
	}

	bool Wifi::poolisvalid(){
		if(wifipoolstartip.size() <= 0) return false;
		if(wifipoolendip.size() <= 0) return false;
		return true;
	}

	void Wifi::output(){	
		std::cout << std::endl << "# wifi_auth_pool" << std::endl;
		
		if(this->getWifiPoolEnable()){
			std::cout << "WIFI_AUTH_POOL_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "WIFI_AUTH_POOL_ENABLE='0'" << std::endl;
		}

		std::cout << "WIFI_AUTH_POOL_STARTIP=\"" << this->getWifiPoolStartip() << "\"" << std::endl;

		std::cout << "WIFI_AUTH_POOL_ENDIP=\"" << this->getWifiPoolEndip()<< "\"" << std::endl;
		return;
	}

       void Wifi::setWifiAdminUsername(std::string wifiadminusername){
		this->wifiadminusername = wifiadminusername;
	}
	   
       void Wifi::setWifiAdminpasswd(std::string wifiadminpasswd){
		this->wifiadminpasswd = wifiadminpasswd;
	}

       void Wifi::setWifiPoolId(std::string wifipoolid){
		this->wifipoolid = wifipoolid;
	}
	   
       void Wifi::setWifiPoolStartip(std::string wifipoolstartip){
		this->wifipoolstartip = wifipoolstartip;
	}
	   
       void Wifi::setWifiPoolEndip(std::string wifipoolendip){
		this->wifipoolendip = wifipoolendip;
	}
	   
       void Wifi::setWifiPoolComment(std::string wifipoolcomment){
		this->wifipoolcomment = wifipoolcomment;
	}
	   
       void Wifi::setWifiPoolEnable(bool wifipoolenable){
		this->wifipoolenable = wifipoolenable;
	}

      
       std::string Wifi::getWifiAdminUsername(){
		return wifiadminusername;
	}
	   
       std::string Wifi::getWifiAdminpasswd(){
		return wifiadminpasswd;
	}

       std::string Wifi::getWifiPoolId(){
		return wifipoolid;
	}
	   
       std::string Wifi::getWifiPoolStartip(){
		return wifipoolstartip;
	}
	   
       std::string Wifi::getWifiPoolEndip(){
		return wifipoolendip;
	}
	   
       std::string Wifi::getWifiPoolComment(){
		return wifipoolcomment;
	}
	   
       bool Wifi::getWifiPoolEnable(){
		return wifipoolenable;
	}

}
