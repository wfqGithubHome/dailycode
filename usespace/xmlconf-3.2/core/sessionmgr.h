#ifndef _ZERO_SESSIONMGR_H
#define _ZERO_SESSIONMGR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "session.h"
#include "nodemgr.h"

namespace maxnet{


	class SessionMgr : public NodeMgr  {
		public:

			SessionMgr();
			~SessionMgr();
			void output();

			void add(std::string name, std::string session, std::string comment);
		private:


	};

}

#endif // _ZERO_SESSIONMGR_H
