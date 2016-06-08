#ifndef _IP_MAC_MGR_H
#define _IP_MAC_MGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{


	class MacBindMgr : public NodeMgr{

		public:

			MacBindMgr();
			~MacBindMgr();
			bool isVaild();
			void output();
			void setMacBindControl(bool status);
			int   getMacBindMaxBridge();
			bool getMacBindAction(int bridge);
			bool getMacBindControl();
			void add(std::string name, std::string name,std::string ipaddr, std::string macaddr);
			int   getBridgeIDbyName(std::string bridge_id_string);
			void setMacBindAction(std::string bridge_id_string, std::string action);

		private:
			bool		  MacBindAction[5]; 
			bool		  MacBindControl;
			std::string bridge;
			static int	max_bridge;
	};

}

#endif
