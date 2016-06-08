#ifndef GUIDE_CONFIG_HH
#define GUIDE_CONFIG_HH

#include <iostream>
#include <map>
#include <string>
#include <sys/types.h>
#include <opt.h>
#include "../global.h"

#define GUIDE_PACKAGE_NAME          "guide-xmlconf"
#define GUIDE_PACKAGE_LONG_NAME     "guide-xmlconf"
#define GUIDE_PACKAGE_DESCRIPTION   "XML Guide Configuration File Manager"
#define GUIDE_PACKAGE_VERSION       "0.1.00"


namespace maxnet 
{

    class GuideConfiguration
    {
	  public:
		explicit GuideConfiguration(int*, char***);
		~GuideConfiguration();

		static int GuideAdditionalUsage() 
		{ 
			std::cout << "xmlconf is Copyright (c) by Maxnet System Inc. 2011.\n";
			return 0;
		};

		static int							read;
		static int							write;
		static int							check;
		static int							parse;


		static int							debug;
		static int							syslog;
		static int							console;
		
		static std::string					main_config_filename;

	  private:
		GuideConfiguration(const GuideConfiguration&);
		GuideConfiguration& operator= (const GuideConfiguration&);
    };
}

#endif
