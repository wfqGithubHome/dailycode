#ifndef _ZERO_GUIDE_ADSL_H
#define _ZERO_GUIDE_ADSL_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Adsl : public Node
    {
		public:
			Adsl();
			~Adsl();
			bool isVaild();
					
			void setAdslIfname(const std::string ifname);
			void setAdslISP(const std::string isp);	
			void setAdslQuality(const std::string quality);
			void setAdslLineid(const std::string lineid);
			void setAdslUpload(const std::string upload);
			void setAdslDownload(const std::string download);
			void setAdslUsername(const std::string username);
			void setAdslPassword(const std::string password);
			
			std::string getAdslIfname();
			std::string getAdslISP();
			std::string getAdslQuality();
			std::string getAdslLineid();
			std::string getAdslUpload();
			std::string getAdslDownload();
			std::string	getAdslUsername();
			std::string	getAdslPassword();

		private:
			
			std::string      ifname;
			std::string      isp;
			std::string      quality;
			std::string      lineid;
			std::string      upload;
			std::string      download;
			std::string      username;
            std::string      password;
            
	};
}

#endif 
