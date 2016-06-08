#ifndef __WAN_CHANNEL_H__
#define __WAN_CHANNEL_H__

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <sys/types.h>
#include <opt.h>

#include "node.h"
#include "channel.h"

namespace maxnet{

	class WanChannel : public Node{

		public:
			WanChannel();
			~WanChannel();

			void			output();
            bool            isValid();
            void            release();
            
			void			setId(std::string id);
			void			setEnable(bool enable);

            std::string     getId();
			bool	        getEnable();

            void            addChannel(std::string classname,unsigned int classtype,unsigned int classid,unsigned int dir,
                    unsigned int parentid,unsigned int shareid,std::string rate,unsigned int attr,unsigned int prio,
                    unsigned int limit,unsigned int weight,std::string comment);

            std::vector<Channel *> getchannelList();
            unsigned int getDataNum();
		private:
			std::string id;
            bool            enable;
            unsigned int    channel_num;
            std::vector<Channel *> channel_list;
	};
}

#endif // __WAN_CHANNEL_H__
