#include "wan_channelmgr.h"

namespace maxnet
{

	WanChannelMgr::WanChannelMgr(){
		wan_num = 0;
	}

	WanChannelMgr::~WanChannelMgr(){
#if 1
		for(unsigned int i = 0; i < wanchannel_list.size(); i++){
			WanChannel *wanchannel = (WanChannel *)wanchannel_list.at(i);
			if(wanchannel){
				wanchannel->release();
				delete wanchannel;
			}
		}
		wanchannel_list.clear();
#endif
	}

	void	WanChannelMgr::output(){	
		std::cout << "# wan  channels" << std::endl;
		for(unsigned int i = 0; i < wanchannel_list.size(); i++){
			WanChannel *wanchannel = (WanChannel *)wanchannel_list.at(i);
			if(wanchannel){
				wanchannel->output();
			}
		}
	}

	void  WanChannelMgr::add(std::string id,bool enable){
		WanChannel *wanchannel = new WanChannel;
		wanchannel->setId(id);
		wanchannel->setEnable(enable);

		if(wanchannel->isValid()){
			wanchannel_list.push_back(wanchannel);
			wan_num++;
		}
		else{
			delete wanchannel;
		}
	}

	void WanChannelMgr::wanChannelAddbyId(std::string id,std::string classname,unsigned int classtype,unsigned int classid,unsigned int dir,
                    unsigned int parentid,unsigned int shareid,std::string rate,unsigned int attr,unsigned int prio,
                    unsigned int limit,unsigned int weight,std::string comment){
		WanChannel *wanchannel = NULL;
		for(unsigned int i = 0; i < wanchannel_list.size(); i++){
			WanChannel *tmpchannel = wanchannel_list.at(i);
			if(tmpchannel->getId().compare(id) == 0){
				wanchannel = tmpchannel;
				break;
			}
		}

		if(wanchannel){
			wanchannel->addChannel(classname,classtype,classid,dir,parentid,shareid,rate,
				attr,prio, limit,weight,comment);
		}
	}

	std::vector<WanChannel *> WanChannelMgr::getWanChannelList(){
		return wanchannel_list;
	}

	unsigned int WanChannelMgr::getDataNum(){
		unsigned int  count = 0;
		for(unsigned int i = 0; i < wanchannel_list.size(); i++){
			WanChannel *wanchannel = wanchannel_list.at(i);
			count += wanchannel->getDataNum();
		}

		return count;
	}

	unsigned int WanChannelMgr::getWanNum(){
		return wan_num;
	}

}
