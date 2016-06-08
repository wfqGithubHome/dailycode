#include "firstgamechannel.h"
#include "firstgamechannelmgr.h"
#include "../global.h"

namespace maxnet{

	FirstgamechannelMgr::FirstgamechannelMgr(){
		
	}

	FirstgamechannelMgr::~FirstgamechannelMgr(){

	}

	void FirstgamechannelMgr::add(std::string id, std::string name){
		Firstgamechannel *node = new Firstgamechannel;

		node->setId(id);
		node->setChannelName(name);

		if(node->isValid()){
			addNode(node);
		}
		else{
			delete node;
		}

		return;
	}

	void FirstgamechannelMgr::output(){
		Firstgamechannel *rule = NULL;
		int t = 0;
		std::cout << std::endl << "# first game channel" << std::endl;
		std::cout << "FIRST_GAME_CHANNEL=\"";

		for(unsigned int i=0; i < node_list.size(); i++){
			rule = (Firstgamechannel *)node_list.at(i);

			if(t == 1)
				std::cout << ",";
			std::cout << rule->getchannelId() << " " << rule->getChannelName();
			t = 1;
		}

		std::cout << "\"" << std::endl;
		
		return;
	}
}

