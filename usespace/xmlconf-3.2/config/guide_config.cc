#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <getopt.h>
#include "guide_config.h"

#define kb * 1024
#define mb * 1024 kb
#define sec * 1

namespace maxnet
{
	int					GuideConfiguration::read						= 0;
	int					GuideConfiguration::write						= 0;
	int					GuideConfiguration::check						= 0;
	int					GuideConfiguration::parse						= 1;
	
	
    int				    GuideConfiguration::debug						= 0;		
	int					GuideConfiguration::syslog						= 0;
	int					GuideConfiguration::console				        = 0;

	std::string			GuideConfiguration::main_config_filename		= "";

	GuideConfiguration::GuideConfiguration(int *argc, char ***argv)
      {

		int	version = 0;
		int	usage	= 0;
		char * main_conf_file;

		char tempbuff[1024];
		snprintf(tempbuff, 1023, "%s - %s , version %s\n\n", GUIDE_PACKAGE_LONG_NAME, GUIDE_PACKAGE_DESCRIPTION, GUIDE_PACKAGE_VERSION);

		optTitle(tempbuff);
		optProgName(GUIDE_PACKAGE_NAME);

		optrega(&this->debug,					OPT_INT,		'd', "debug",			" debug level #");
		optrega(&main_conf_file,				OPT_STRING,		'f', "configfile",		" Guide configure file name");
		optrega(&this->syslog,					OPT_FLAG,		's', "syslog",			" log to syslog");
		optrega(&this->console,					OPT_FLAG,		'o', "console",			" log to console");

		optrega(&this->read,					OPT_FLAG,		'r', "read",			" read a key value from xml Guide configure file");
		optrega(&this->write,					OPT_FLAG,		'w', "write",			" write a key value to xml Guide configure file");
		optrega(&this->parse,					OPT_FLAG,		'p', "parse",			" parse xml Guide configure file");
		optrega(&this->check,					OPT_FLAG,		'c', "check",			" check xml Guide configure file");

		optrega(&version,						OPT_FLAG,		'V', "version",			" version info");
		optrega(&usage,							OPT_FLAG,		'h', "help",			" display usage info");
		
		optAdditionalUsage(this->GuideAdditionalUsage);
		opt(argc, argv);

		if (version)
		{
			printf("%s - %s , version %s\n", GUIDE_PACKAGE_LONG_NAME, GUIDE_PACKAGE_DESCRIPTION, GUIDE_PACKAGE_VERSION);
			exit(-1);
		}
		if (usage)
		{
			optPrintUsage();
			exit(-1);
		}

		if (optinvoked(&main_conf_file))
		{
			main_config_filename = main_conf_file;
		}
		else{
			optPrintUsage();
			exit(-1);
		}

		opt_free();
	}

	GuideConfiguration::~GuideConfiguration()
    {

    }

}

