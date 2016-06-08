#ifndef _ZERO_GUIDE_WAN_H
#define _ZERO_GUIDE_WAN_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Wan : public Node
    {
		public:
			Wan();
			~Wan();
			bool isVaild();
	
			void setIfname(std::string ifname);
			void setISP(std::string isp);	
			void setQuality(std::string quality);
			void setLineid(std::string lineid);
			void setUpload(std::string upload);
			void setDownload(std::string download);
			void setMAC(std::string mac);
            void setIPaddr(std::string ipaddr);
            void setNetmask(std::string netmask);
            void setGateway(std::string gateway);
            void setConntype(std::string conntype);
            
			std::string getIfname();
			std::string getISP();
			std::string getQuality();
			std::string getLineid();
			std::string getUpload();
			std::string getDownload();
			std::string getMAC();
			std::string getIPaddr();
			std::string getNetmask();
			std::string getGateway();
            std::string getConntype();
            
		private:
			std::string ifname;
			std::string isp;
			std::string quality;
			std::string lineid;
			std::string upload;
			std::string download;
			std::string mac;
			std::string ipaddr;
			std::string netmask;
            std::string gateway;
            std::string conntype;
           
	};

}

#endif 
