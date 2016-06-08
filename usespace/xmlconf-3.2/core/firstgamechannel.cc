#include <sstream>
#include "firstgamechannel.h"
#include "../global.h"

namespace maxnet{

	Firstgamechannel::Firstgamechannel(){
		channel_id = 0;
		name = "";
	}

	Firstgamechannel::~Firstgamechannel(){
		
	}

	bool Firstgamechannel::isValid(){
		if(name.size() <= 0)
			return false;
		return true;
	}

	void Firstgamechannel::setId( std::string id){
		std::stringstream id_string;
		id_string << id;
		id_string >> this->channel_id;

		if(this->channel_id < 0){
			this->channel_id = 0;
		}
	}

	void Firstgamechannel::setChannelName( std::string name){
		this->name = name;
	}

	int Firstgamechannel::getchannelId(){
		return this->channel_id;
	}

	std::string Firstgamechannel::getChannelName(){
		return this->name;
	}

	void Firstgamechannel::output(){
		
	}

}
