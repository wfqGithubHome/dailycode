#ifndef _ZERO_DNSPOLICYMGR_H
#define _ZERO_DNSPOLICYMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{

	class DnspolicyMgr : public NodeMgr  {
		public:

			DnspolicyMgr();
			~DnspolicyMgr();
			void output();
			bool getEnable();
			void setEnable(bool enable);


			void add(std::string name, std::string bridge_id, 
						std::string src, std::string dst, 
						std::string domain_name,std::string action,std::string targetip,
						std::string comment,bool disabled);

		private:
			bool enable;
			int  policy_id[MAX_BRIDGE_ID];

	};

}

#endif // _ZERO_DNSPOLICYMGR_H
