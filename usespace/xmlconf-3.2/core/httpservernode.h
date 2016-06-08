#ifndef _ZERO_HTTPSERVERNODE_H
#define _ZERO_HTTPSERVERNODE_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class HttpServerNode : public Node{

		public:

			HttpServerNode();
			~HttpServerNode();
			bool isVaild();
			void output();

			void setBridgeId(std::string id);

			void setServerType(std::string servertype);
			void setIPAddr(std::string ipaddr);
			void setPrefix(std::string prefix);
			void setSuffix(std::string suffix);
			
			int			getBridgeId();
           	std::string getBridgeIdString();
			
			std::string	getServerType();
			std::string getIPAddr();
			std::string getPrefix();
			std::string getSuffix();
			
		private:
			//void setGroupID(int grpid);
			//int	getGroupID();
			//int groupid;
			int			bridge_id;
			std::string ip_addr;
			std::string servertype;
			std::string prefix;
			std::string suffix;
			
	};

}

#endif // _ZERO_HTTPSERVERNODE_H
