#include "trafficmgr.h"
#include "traffic_channel.h"
#include "traffic_util.h"
#include "../global.h"

int check_type=0;

namespace maxnet{

	int							TrafficMgr::max_bridge_id = 0;

	TrafficMgr::TrafficMgr(){
		count = 0;
		enable = false;
	}

	TrafficMgr::~TrafficMgr(){
		TrafficId * id;
		unsigned int i=0;
#if 1		
		TrafficChannel * channel = NULL;
        for(i = 0; i < node_list.size(); i++){
			channel = (TrafficChannel *)node_list.at(i);
			delete channel;
		}
		node_list.clear();
#endif		
		for(i=0; i < m_TrafficId.size(); i++){
			id = m_TrafficId.at(i);
			delete id;
		}
		m_TrafficId.clear();

		return;
	}

	void TrafficMgr::setEnable(bool enable){
		this->enable = enable;
	}
	
	bool TrafficMgr::getEnable(){
		return this->enable;
	}
	
	int TrafficMgr::getMaxBridgeID()
	{
		return this->max_bridge_id;
	}

	int TrafficMgr::getDefaultIdbyInterface(std::string inter){
		std::vector<Node *> traffic_list = getList();
		for(unsigned int i = 0; i <	traffic_list.size(); i++){
			TrafficChannel * channel = (TrafficChannel *) traffic_list.at(i);
			if(channel->getInterface().compare(inter) == 0){
				if(channel->isDefault()){
					return channel->getClassId();
				}
			}
			
		}
		return -1;
	}

	int TrafficMgr::getrootmaxrate(std::string inter){
		std::vector<Node *> traffic_list = getList();
		for(unsigned int i = 0; i <	traffic_list.size(); i++){
			TrafficChannel * channel = (TrafficChannel *) traffic_list.at(i);
			if(channel->getInterface().compare(inter) == 0){
				//if(!(channel->getClassType() == 0)){
					return channel->getRate();
				//}
			}
			
		}
		return -1;
	}

	void TrafficMgr::add(std::string str_bridgeid,std::string interface,std::string class_name, std::string parent_name,std::string share_name,std::string class_type ,
				std::string rate,bool is_default,std::string priority, std::string limit, std::string weight,std::string attribute,std::string comment){
		unsigned int drate = 0;
		unsigned int  dpriority = 0;
		unsigned int  dlimit = 0;
		unsigned int  dweight = 0;
		unsigned int  dattribute = 0;
		unsigned int  dtype = 0;
	
		TrafficUtil::getRate(&drate, rate.c_str());
		//TrafficUtil::getRate(&dbandwidth, bandwidth.c_str());
		//TrafficUtil::getRate(&per_drate, per_rate.c_str());
		//TrafficUtil::getRate(&per_dceil, per_ceil.c_str());

		int bridge_id;

		//std::stringstream ss0(rate);
		//ss0 >> drate;

		std::stringstream ss1(priority);
		ss1 >> dpriority;

		std::stringstream ss2(limit);
		ss2 >> dlimit;

		std::stringstream ss3(weight);
		ss3 >> dweight;

		std::stringstream ss4(attribute);
		ss4 >> dattribute;
		
		std::stringstream ss5(class_type);
		ss5 >> dtype;
				
		std::stringstream ss6(str_bridgeid);
		ss6 >> bridge_id;

		if(bridge_id > this->max_bridge_id)
		{
			this->max_bridge_id = bridge_id;
		}		
		
		this->addchannel(interface, class_name, parent_name, share_name, dtype,rate,drate ,is_default, dpriority, 
				dlimit, dweight , dattribute, comment);
	}
	
	TrafficChannel * TrafficMgr::addchannel(std::string interface,
				std::string class_name, std::string parent_name,
				std::string share_name,unsigned int class_type ,
				std::string ratestring , unsigned int rate,
				bool is_default,unsigned int priority, 
				unsigned int limit, unsigned int weight,
				unsigned int attribute,std::string comment){
		char buff[128];
		int	class_id = 0;
		int   parent_id = 0;
		int   share_id = 0;
		std::string share_comment;

		TrafficChannel * channel;
		if(parent_name.compare("root") != 0 && parent_name.compare("") != 0){
			TrafficChannel * channel = getChannelbyName(interface, class_name);
			if(channel != NULL){
				std::cerr << "#channel name \"" << class_name << "\" is exist" << std::endl;
				return NULL;
			}
			channel = getChannelbyName(interface, parent_name);
			if(channel == NULL){
				std::cerr << "#parent channel \"" << parent_name << "\" interface \"" << interface << "\" is not exist" << std::endl;
				return NULL;
			}

			memset(buff,0,128);
			snprintf(buff, 127, "%s", interface.c_str());

			if(class_type == VC){      //if(class_type == 3){//modfied 2009.11.5		
				parent_id = channel->getClassId();
			}

			class_id = getNextId(buff,parent_id,class_type);
			if(class_id == DEVICE_ERR){
				return NULL;
			}
	
			if((share_name.size()>0)&&(share_name.compare("none"))){
				TrafficChannel * channel_share;
				channel_share  = getChannelbyName(interface, share_name);
				if(channel_share != NULL){
					share_id = channel_share->getClassId();
					share_comment = channel_share->getComment();
				}
				else{
					return NULL;
				}				
			}
			channel->setChild(true);
		}
		else{
			class_id   = 0;
			parent_id = 0;
			share_id  = 0;
		}

		channel = new TrafficChannel();
		channel->setName(class_name);
		channel->setGroup(false);
		channel->setInterface(interface);
		channel->setParentName(parent_name);
		channel->setShareName(share_name);
		channel->setClassName( class_name);
		channel->setShareComment(share_comment);
		channel->setRateString(ratestring);
		channel->setRate(rate);
		channel->setClassId(class_id);
		channel->setParentId(parent_id);
		channel->setShareId(share_id);
		channel->setClassType(class_type);
		channel->setDefault(is_default);
		channel->setLimit(limit);
		channel->setPriority(priority);
		channel->setWeight(weight);
		channel->setAttribute(attribute);
		channel->setComment(comment);
		channel->setId(count);

		addNode(channel);
		count++;
		return channel;
	}

	int	TrafficMgr::getNextId(std::string strInterface,int parent_id,unsigned int class_type){
		TrafficId * id;
		std::string temp;
		int status = 0;

		for(unsigned int i=0; i < m_TrafficId.size(); i++){
			id = m_TrafficId.at(i);
			temp = id->getInterface();
			if(strcmp(strInterface.c_str(), temp.c_str()) == 0){
				if(((check_type == PIPE)&&(class_type == VC))){
						status = 1;
				}
				switch(class_type){
					case SHARE:
						check_type = SHARE;
						return id->getNextId_share();
						
					case PIPE:
						check_type = PIPE;
						return id->getNextId_pipe();
						
					case VC:
						check_type = VC;
						return id->getNextId_vc(parent_id,status);
						
					default:
						return DEVICE_ERR;
				}
				//return id->getNextId();
			}
		}
		
		id = new TrafficId(256, strInterface);
		m_TrafficId.push_back(id);
		switch(class_type){
			case SHARE:
				check_type = SHARE;	
				return id->getNextId_share();
				
			case PIPE:
				check_type = PIPE;	
				return id->getNextId_pipe();
				
			case VC:
				check_type = VC;	
				return id->getNextId_vc(parent_id, status);
				
			default:
				return DEVICE_ERR;
		}
		//return id->getNextId();
	}


	TrafficChannel * TrafficMgr::getChannelbyName(std::string interface, std::string name){
		std::vector<Node *> node_list = getList();
		TrafficChannel * channel;
		for(unsigned int i=0; i < node_list.size(); i++){
			channel = (TrafficChannel *)node_list.at(i);
			if(name.compare(channel->getName()) == 0 && interface.compare(channel->getInterface()) == 0){
				return channel;	
			}
		}
		return NULL;	
		
//		TrafficChannel * channel;
//		channel = (TrafficChannel * )find(name);
//		while(channel){
//			if(channel->getInterface().compare(interface) == 0){
//				return channel;
//			}
//			channel = (TrafficChannel *)this->next();
//		}
//
//		return NULL;
	}

	TrafficChannel * TrafficMgr::getParentChannelbyName(std::string interface, std::string name){
		TrafficChannel * channel;
		channel = getChannelbyName(interface, name);
		if(channel == NULL) return NULL;
		return getChannelbyName(interface, channel->getParentName());
	}

	void TrafficMgr::output(){
		//TrafficChannel * channel = NULL;
		std::cout << "# traffic channels" << std::endl;

		//for(unsigned int i=0; i < node_list.size(); i++){
		//	channel = (TrafficChannel *)node_list.at(i);
		//	channel->output();
		//}		
		return;
	}
}

