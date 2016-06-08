#include "wifimgr.h"
#include "../global.h"

namespace maxnet{

	WifiMgr::WifiMgr(){
		wifienable = false;
		admindatacount = 0;
		pooldatacount = 0;
	}

	WifiMgr::~WifiMgr(){
		admindatacount = 0;
		pooldatacount = 0;

		Wifi *node = NULL;
		for(unsigned int i = 0; i < admin_node_list.size(); i++){
			node = admin_node_list.at(i);
			delete node;
		}

		for(unsigned int j = 0; j < pool_node_list.size(); j++){
			node = pool_node_list.at(j);
			delete node;
		}

		return;
	}

	void WifiMgr::output(){		
		std::cout << std::endl << "# wifi_auth" << std::endl;
		
		if(this->getWifiEnable()){
			std::cout << "WIFI_AUTH_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "WIFI_AUTH_ENABLE='0'" << std::endl;
		}

		std::cout << "WIFI_AUTH_PC_DISCOVER_TIME='" << this->getPcTime()<< "'" << std::endl;
		std::cout << "WIFI_AUTH_MAX_TIME='" << this->getMaxTime()<< "'" << std::endl;
		std::cout << "WIFI_AUTH_SHARE_NUM='" << this->getShareNum()<< "'" << std::endl;
		std::cout << "WIFI_AUTH_ADMINS='" << this->getAdmins()<< "'" << std::endl;
		std::cout << "WIFI_AUTH_REDIRECT_ADV_URL=${ROUTE_INTERFACE_LAN_IP}\"/AC/wifi/login.html\"" << std::endl;

		std::vector<Wifi *> pools = this->getPoolList();
		Wifi *pool = NULL;
		for(unsigned int i=0; i < pools.size(); i++){
			pool = (Wifi *)pool_node_list.at(i);
			pool->output();
		}
		return;
	}

	std::vector<Wifi *> WifiMgr::getAdminList(){
		return admin_node_list;
	}

	std::vector<Wifi *> WifiMgr::getPoolList(){
		return pool_node_list;
	}

	int WifiMgr::getAdminCount(){
		return admindatacount;
	}


	int WifiMgr::getPoolCount(){
		return pooldatacount;
	}

	void WifiMgr::setWifiEnable(bool wifienable){
		this->wifienable = wifienable;
	}
	
       void WifiMgr::setPcTime(std::string wifipctime){
		this->wifipctime = wifipctime;
	}
	   
       void WifiMgr::setMaxTime(std::string wifimaxtime){
		this->wifimaxtime = wifimaxtime;
	}
	   
       void WifiMgr::setShareNum(std::string wifisharenum){
		this->wifisharenum = wifisharenum;
	}

	void WifiMgr::setAdmins(std::string wifiadmins){
		this->wifiadmins = wifiadmins;
	}

       bool WifiMgr::getWifiEnable(){
		return wifienable;
	}
	   
       std::string WifiMgr::getPcTime(){
		return wifipctime;
	}
	   
       std::string WifiMgr::getMaxTime(){
		return wifimaxtime;
	}
	   
       std::string WifiMgr::getShareNum(){
		return wifisharenum;
	}

	std::string WifiMgr::getAdmins(){
		return wifiadmins;
	}
              
	void WifiMgr::admin_add(std::string username, std::string passwd){
		Wifi *node = new Wifi();
		node->setWifiAdminUsername(username);
		node->setWifiAdminpasswd(passwd);

		if(node->adminisvalid()){
			add_admin_node(node);
		}
		else{
			delete node;
		}

		return;
	}

	void WifiMgr::pool_add(std::string id, std::string startip, std::string endip, std::string comment, bool enable){
		Wifi *node = new Wifi();
		node->setWifiPoolId(id);
		node->setWifiPoolStartip(startip);
		node->setWifiPoolEndip(endip);
		node->setWifiPoolComment(comment);
		node->setWifiPoolEnable(enable);

		if(node->poolisvalid()){
			add_pool_node(node);
		}
		else{
			delete node;
		}

		return;
	}

	void WifiMgr::add_admin_node(Wifi *node){
		admin_node_list.push_back(node);
		++admindatacount;
	}

	void WifiMgr::add_pool_node(Wifi *node){
		pool_node_list.push_back(node);
		++pooldatacount;
	}

}
