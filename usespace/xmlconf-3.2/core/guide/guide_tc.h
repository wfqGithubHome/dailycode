#ifndef _ZERO_GUIDE_TC_H
#define _ZERO_GUIDE_TC_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class TC : public Node
    {
		public:
			TC();
			~TC();
			bool isVaild();
			
			void setName(std::string name);
			void setLineid(std::string lineid);
            void setISP(std::string isp);
			void setQuality(std::string quality);
            void setBalance(std::string balance);
            void setUpload(std::string upload);
            void setDownload(std::string download);
            void  setEnable(std::string enable);
			
			std::string getName();
			std::string getLineid();
            std::string getISP();
			std::string getQuality();
            std::string getBalance();
            std::string getUpload();
            std::string getDownload();
            std::string getEnable();
        
		private:
			
			std::string name;
			std::string lineid;
			std::string isp;
            std::string quality;
            std::string balance;
            std::string upload;
            std::string download;
            std::string enable;
	};

}

#endif 
