/*
 * Copyright (c) 2001 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <getopt.h>
#include "route_config.h"

#define kb * 1024
#define mb * 1024 kb
#define sec * 1


namespace maxnet
{
	int					RouteConfiguration::read						= 0;
	int					RouteConfiguration::write						= 0;
	int					RouteConfiguration::check						= 0;
	int					RouteConfiguration::parse						= 1;
	
	int					RouteConfiguration::print_dns					= 0;
	int					RouteConfiguration::print_ddns					= 0;
	
	int					RouteConfiguration::print_lan					= 0;
	int					RouteConfiguration::print_wan					= 0;
	int					RouteConfiguration::print_adsl_client			= 0;
	int					RouteConfiguration::print_l2tpvpn_client		= 0;

	int					RouteConfiguration::print_loadb					= 0;
	int					RouteConfiguration::print_routing				= 0;
	int					RouteConfiguration::print_route_table			= 0;
	int					RouteConfiguration::print_route_rule			= 0;

	int					RouteConfiguration::print_routing_command		= 0;
	
	int					RouteConfiguration::print_dnat					= 0;
	int					RouteConfiguration::print_snat					= 0;
	int					RouteConfiguration::print_macbind				= 0;
	int					RouteConfiguration::print_adsl_client_passwd	= 0;
	
    int					RouteConfiguration::debug						= 0;		
	int					RouteConfiguration::syslog						= 0;
	int					RouteConfiguration::console						= 0;

	std::string			RouteConfiguration::key							= "";
	std::string			RouteConfiguration::value						= "";
	std::string			RouteConfiguration::main_config_filename		= "";

	RouteConfiguration::RouteConfiguration(int *argc, char ***argv)
    {

		int	version = 0;
		int	usage	= 0;
		char * main_conf_file;
		char * xmlkey;
		char * xmlvalue;

		char tempbuff[1024];
		snprintf(tempbuff, 1023, "%s - %s , version %s\n\n", ROUTE_PACKAGE_LONG_NAME, ROUTE_PACKAGE_DESCRIPTION, ROUTE_PACKAGE_VERSION);

		optTitle(tempbuff);
		optProgName(ROUTE_PACKAGE_NAME);

		optrega(&this->debug,					OPT_INT,		'd', "debug",			" debug level #");
		optrega(&main_conf_file,				OPT_STRING,		'f', "configfile",		" route configure file name");
		optrega(&xmlkey,						OPT_STRING,		'k', "xmlkey",			" xml key name to read or write");
		optrega(&xmlvalue,						OPT_STRING,		'v', "xmlvalue",		" xml value to write");
		optrega(&this->syslog,					OPT_FLAG,		's', "syslog",			" log to syslog");
		optrega(&this->console,					OPT_FLAG,		'o', "console",			" log to console");

		optrega(&this->read,					OPT_FLAG,		'r', "read",			" read a key value from xml route configure file");
		optrega(&this->write,					OPT_FLAG,		'w', "write",			" write a key value to xml route configure file");
		optrega(&this->parse,					OPT_FLAG,		'p', "parse",			" parse xml route configure file");
		optrega(&this->check,					OPT_FLAG,		'c', "check",			" check xml route configure file");

		optrega(&this->print_dns,				OPT_FLAG,		'N', "printdns",		" print dns info");
		optrega(&this->print_ddns,				OPT_FLAG,		'm', "printddns",		" print ddns info");
		
		optrega(&this->print_lan,				OPT_FLAG,		'l', "printlan",			" print interface lan info");
		optrega(&this->print_wan,				OPT_FLAG,		'W', "printwan",			" print interface wan info");
		optrega(&this->print_adsl_client,		OPT_FLAG,		'A', "printadsl",			" print adsl client info");
		optrega(&this->print_l2tpvpn_client,	OPT_FLAG,		'L', "printl2tpvpn",		" print l2tpvpn client info");
		optrega(&this->print_loadb,				OPT_FLAG,		'B', "printloadb",			" print load balancing command");
		optrega(&this->print_routing,			OPT_FLAG,		'R', "printrouting",		" print routing command");
		optrega(&this->print_route_table,		OPT_FLAG,		'T', "printroutetable",		" print route table info");		
		optrega(&this->print_route_rule,		OPT_FLAG,		'U', "printrouterule",		" print route rule info");	
		optrega(&this->print_dnat,				OPT_FLAG,		'D', "printdnat",			" print dnat command");	
		optrega(&this->print_snat, 				OPT_FLAG,		'S', "printsnat",			" print snat command");	
		optrega(&this->print_macbind, 			OPT_FLAG,		'M', "printmacbind",		" print static mac-ip bind command");	
		optrega(&this->print_adsl_client_passwd,OPT_FLAG,		'P', "printadslpasswd",		" print adsl client password command");	
		optrega(&this->print_routing_command,	OPT_FLAG,		'Y', "printroutingcommand",	" print all routing command");
		
		optrega(&version,						OPT_FLAG,		'V', "version",				" version info");
		optrega(&usage,							OPT_FLAG,		'h', "help",				" display usage info");
		
		optAdditionalUsage(this->RouteAdditionalUsage);

		opt(argc, argv);
		

		if (version)
		{
			printf("%s - %s , version %s\n", ROUTE_PACKAGE_LONG_NAME, ROUTE_PACKAGE_DESCRIPTION, ROUTE_PACKAGE_VERSION);
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

		if (optinvoked(&xmlkey))
		{
			key = xmlkey;
		}

		//optPrintUsage();
		opt_free();
	}

	RouteConfiguration::~RouteConfiguration()
    {

    }


}
