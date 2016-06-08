#ifndef _ZERO_PROTOCOLMGR_H
#define _ZERO_PROTOCOLMGR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "protocol.h"
#include "nodemgr.h"

namespace maxnet{


	class ProtocolMgr : public NodeMgr  {
		public:

			ProtocolMgr(bool addDefault = true);
			~ProtocolMgr();
			void output();

			void add(int id, std::string name, std::string proto, std::string sport, std::string dport, std::string match,std::string type_, std::string option, std::string comment);

			void Clear();
		private:

			void addPatFile(Protocol * proto);
			int  customProtocolId;

	};

}

#endif // _ZERO_PROTOCOLMGR_H
