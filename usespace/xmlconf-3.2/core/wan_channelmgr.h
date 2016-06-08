#ifndef __WAN_CHANNELMGR_H__
#define __WAN_CHANNELMGR_H__

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <sys/types.h>
#include <opt.h>

#include "nodemgr.h"
#include "wan_channel.h"

namespace maxnet{

	class WanChannelMgr : public NodeMgr{

		public:
			WanChannelMgr();
			~WanChannelMgr();

			void    output();
            void    add(std::string id,bool enable);
            void    wanChannelAddbyId(std::string id,std::string classname,unsigned int classtype,unsigned int classid,unsigned int dir,
                    unsigned int parentid,unsigned int shareid,std::string rate,unsigned int attr,unsigned int prio,
                    unsigned int limit,unsigned int weight,std::string comment);
            
            std::vector<WanChannel *> getWanChannelList();
            unsigned int getDataNum();
            unsigned int getWanNum();
		private:
            unsigned int wan_num;
            std::vector<WanChannel *> wanchannel_list;
	};
}

#endif // __WAN_CHANNELMGR_H__
