#include "macbindmgr.h"
#include "../global.h"
#include <sstream>
#include "macbind.h"


namespace maxnet{

	int	MacBindMgr::max_bridge = 0;

	MacBindMgr::MacBindMgr(){
		MacBindControl = false;
		
		for(int i = 0; i< 5 ; ++i) MacBindAction[i] = false; 

		bridge = "";
	}

	MacBindMgr::~MacBindMgr(){
#if 1	
		MacBind * rule = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (MacBind *)node_list.at(i);
			if(!rule->isGroup())
				delete rule;
		}
		node_list.clear();
#endif
		return;
	}
	
	bool MacBindMgr::isVaild(){
		
		return true;
	}

	void MacBindMgr::output(){
		std::cout << "# Macbind rules" << std::endl <<std::endl;
	}

	void MacBindMgr::add(std::string name, std::string bridge,std::string ipaddr, std::string macaddr){	
		MacBind * rule = new MacBind();
		if(name.compare("") == 0) {
			name="any";
		}
		if(ipaddr.compare("") == 0) {
			ipaddr="any";
		}
		if(macaddr.compare("") == 0) {
			macaddr="any";
		}
		if(bridge.compare("") == 0) {
			bridge="any";
		}
	
		rule->setName(name);
		rule->setBridge(bridge);
		rule->setIpaddr(ipaddr);
		rule->setMacaddr(macaddr);

		if(rule->isVaild()){
			addNode(rule);
		}
		else{
			delete rule;
		}
		
		return;
	}
	
	void MacBindMgr::setMacBindControl(bool status)
	{
		this->MacBindControl = status;
	}

	bool MacBindMgr::getMacBindAction(int bridge)
	{
		return this->MacBindAction[bridge];
	}

	int MacBindMgr::getMacBindMaxBridge()
	{
		return this->max_bridge;
	}

	bool MacBindMgr::getMacBindControl()
	{
		return this->MacBindControl;
	}

	int MacBindMgr::getBridgeIDbyName(std::string bridge_id_string)
	{
		int id;
		std::stringstream bridge_id;
		
		bridge_id << bridge_id_string;
		bridge_id >> id;
		
		if(id < 0 || id >= 10)
			return 0;
		
		if(id > this->max_bridge)
		{
			this->max_bridge = id;
		}
		
		return id;
	}

	void MacBindMgr::setMacBindAction(std::string bridge_id_string, std::string action){
		int bridge_id = getBridgeIDbyName(bridge_id_string);

		if(bridge_id < 0 || bridge_id >= 10)
			return;

		if(action.compare("accept") == 0)
		{
			this->MacBindAction[bridge_id] = 1;
		}
		else
		{	
			this->MacBindAction[bridge_id] = 0;
		}
	}

}

