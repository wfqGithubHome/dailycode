#ifndef _ZERO_PORT_MIRROR_MGR_H
#define _ZERO_PORT_MIRROR_MGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
namespace maxnet{


	class PortMirrorMgr : public NodeMgr  {
		public:

			PortMirrorMgr();
			~PortMirrorMgr();

			void output();
			int	 getMaxBridgeID();
			void setEnable(std::string bridge_id, int enable);
            int  getEnable(int bridge_id);
            
			void add(std::string name, std::string bridge_id, std::string src_addr, 
				std::string dst_addr, std::string proto, std::string action, 
				std::string comment,bool disabled);

		private:
			int  getBridgeIDbyName(std::string bridge_id_string);
			int enable[MAX_BRIDGE_ID];
			static int	max_bridge_id;
	};

}

#endif // _ZERO_PORT_MIRROR_MGR_H

