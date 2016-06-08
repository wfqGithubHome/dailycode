#ifndef _ZERO_GUIDE_APPBIND_H
#define _ZERO_GUIDE_APPBIND_H

#include <iostream>
#include <string>
#include <vector>

#include "../node.h"


namespace maxnet
{
	class Appbind : public Node
    {
		public:
			Appbind();
			~Appbind();
			bool isVaild();

			void setName(std::string name);
			void setAppType(std::string apptype);
			void setBindType(std::string bindtype);
			void setBindLineid(std::string bindlineid);
			void setBypass(std::string bypass);
            void setMaxUpload(std::string maxupload);
            void setMaxDownload(std::string maxdownload);
            void setWeight(std::string weight);

			std::string getName();
			std::string getAppType();
			std::string getBindType();
			std::string	getBindLineid();
			std::string	getBypass();
            std::string	getMaxUpload();
            std::string	getMaxDownload();
            std::string	getWeight();

		private:
			std::string name;
			std::string apptype;
			std::string bindtype;
			std::string bindlineid;
			std::string bypass;
            std::string maxupload;
            std::string maxdownload;
            std::string weight;
	};
}

#endif 

