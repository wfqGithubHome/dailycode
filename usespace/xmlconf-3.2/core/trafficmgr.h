#ifndef _ZERO_TRAFFICMGR_H
#define _ZERO_TRAFFICMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "traffic_channel.h"
#include "traffic_id.h"

namespace maxnet{


	class TrafficMgr : public NodeMgr  {
		public:

			TrafficMgr();
			~TrafficMgr();
			void output();
			bool getEnable();
			void setEnable(bool enable);
			int	 getMaxBridgeID();


			int	getDefaultIdbyInterface(std::string interface);
			int   getrootmaxrate(std::string interface);//add donald steven 2009.11.5
			
			TrafficChannel * getChannelbyName(std::string interface, std::string name);
			void add(std::string str_bridgeid,std::string interface,std::string class_name, 
				std::string parent_name,std::string share_name,
				std::string class_type ,std::string rate, 
				bool is_default,std::string priority, 
				std::string limit, std::string weight,
				std::string attribute,std::string comment);

		private:
			bool enable;

			int count;
			TrafficChannel *  addchannel(std::string interface,
				std::string class_name, std::string parent_name,
				std::string share_name,unsigned int class_type ,				
				std::string ratestring ,unsigned int rate,
				bool is_default,unsigned int priority, 
				unsigned int limit, unsigned int weight,
				unsigned int attribute,std::string comment);

			TrafficChannel * getParentChannelbyName(std::string interface, std::string name);
			int	getNextId(std::string strMask,int parent_id,unsigned int class_type);
			std::vector<TrafficId *> m_TrafficId;
			static int	max_bridge_id;


	};

}

#define SHARE        0 
#define PIPE           1
#define VC              2

#define DEVICE_OK 0
#define DEVICE_ERR -1

#endif // _ZERO_TRAFFICMGR_H
