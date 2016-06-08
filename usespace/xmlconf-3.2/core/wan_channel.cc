#include "wan_channel.h"

namespace maxnet
{

	WanChannel::WanChannel(){
		id = "0";
		enable = false;
		channel_num = 0;
	}

	WanChannel::~WanChannel(){
#if 1
		for(unsigned int i = 0; i < channel_list.size(); i++){
			Channel *channel_info = (Channel *)channel_list.at(i);
			if(channel_info){
				delete channel_info;
			}
		}
		channel_list.clear();
#endif
	}

	void	WanChannel::output(){	
		std::cout << "# id=\"" << getId() << "\", enable=\"" << (getEnable()?1:0)  <<  "\"\t\t// " << std::endl;

		for(unsigned int i = 0; i < channel_list.size(); i++){
			Channel *channel_info = (Channel *)channel_list.at(i);
			if(channel_info){
				channel_info->output();
			}
		}
	}

	bool   WanChannel::isValid(){
		if(id.size() <= 0) return false;
		return true;
	}

	void   WanChannel::release(){
		for(unsigned int i = 0; i < channel_list.size(); i++){
			Channel *channel_info = (Channel *)channel_list.at(i);
			if(channel_info){
				delete channel_info;
			}
		}
		channel_list.clear();
	}

	void	WanChannel::setId(std::string id){
		this->id = id;
	}
	void	WanChannel::setEnable(bool enable){
		this->enable = enable;
	}

	std::string     WanChannel::getId(){
		return id;
	}
	bool	        WanChannel::getEnable(){
		return enable;
	}

	void  WanChannel::addChannel(std::string classname,unsigned int classtype,unsigned int classid,unsigned int dir,
                    unsigned int parentid,unsigned int shareid,std::string rate,unsigned int attr,unsigned int prio,
                    unsigned int limit,unsigned int weight,std::string comment){
		Channel *channel_info = new Channel;
		channel_info->setClassName(classname);
		channel_info->setClassType(classtype);
		channel_info->setClassId(classid);
		channel_info->setDir(dir);
		channel_info->setParentId(parentid);
		channel_info->setShareId(shareid);
		channel_info->setRate(rate);
		channel_info->setAttribute(attr);
		channel_info->setPriority(prio);
		channel_info->setLimit(limit);
		channel_info->setWeight(weight);
		channel_info->setComment(comment);

		if(channel_info->isValid()){
			channel_list.push_back(channel_info);
			channel_num++;
		}
		else{
			delete channel_info;
		}
	}

	std::vector<Channel *> WanChannel::getchannelList(){
		return channel_list;
	}

	unsigned int WanChannel::getDataNum(){
		return channel_num;
	}


}
