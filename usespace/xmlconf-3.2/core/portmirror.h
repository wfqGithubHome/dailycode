#ifndef _ZERO_PORT_MIRROR_H
#define _ZERO_PORT_MIRROR_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class PortMirror : public Node{

		public:

			PortMirror();
			~PortMirror();
			bool isVaild();
			void output();

			void setBridgeID(int bridge_id);
			void setBridgeIDbyString(std::string bridge_id);
			void setSrcAddr(std::string addr);
            void setDstAddr(std::string addr);
			void setAction(std::string action);
            void setProto(std::string proto);

            std::string getProto();
			std::string getSrcAddr();
			std::string getDstAddr();            
			std::string getAction();
			int			getBridgeID();
		private:
			std::string src_addr;
            std::string dst_addr;
            std::string proto;
			std::string action;
			int			bridge_id;
	};

}

#endif // _ZERO_PORT_MIRROR_H

