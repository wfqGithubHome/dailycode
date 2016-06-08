#ifndef _ZERO_DNSPOLICY_H
#define _ZERO_DNSPOLICY_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

#define DPI_DNS_REDIRECT 1
#define DPI_DNS_DROP     2
#define DPI_DNS_HIJACK   3

namespace maxnet{


	class DnsPolicy : public Node{

		public:

			DnsPolicy();
			~DnsPolicy();
			bool isVaild();
			void output();

			void setBridgeId(std::string id);
			void setSrc(std::string src);
			void setDst(std::string dst);
			void setDomainName(std::string domainname);
			void setAction(std::string action);
			void setTargetip(std::string targetip);
			void setRawName(std::string rawname);

			int			getBridgeId();
           	std::string getBridgeIdString();
			std::string getSrc();
			std::string getDst();
			std::string getDomainName();
			int			getAction();
			std::string getTargetip();
			std::string getRawName();
			
			bool getEnable();
			void setEnable(bool enable);

		private:
			std::string raw_name;	//文件内原始名称
			int			bridge_id;
			std::string src;
			std::string dst;
			std::string domain_name;
			std::string action;
			std::string targetip;
			bool 		policy_enable;
	};

}

#endif // _ZERO_DNSPOLICY_H
