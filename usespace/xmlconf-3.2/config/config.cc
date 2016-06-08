/*
 * Copyright (c) 2001 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <getopt.h>
#include <opt.h>
#include "config.h"

#define kb * 1024
#define mb * 1024 kb
#define sec * 1


namespace maxnet
{

	int							Configuration::read		= 0;
	int							Configuration::write	= 0;
	int							Configuration::parse	= 1;
	int							Configuration::check	= 0;
	int							Configuration::print_fw = 0;
	int							Configuration::print_action = 0;
	int							Configuration::print_macbind = 0;
	int							Configuration::print_macauth = 0;
	int							Configuration::print_quota = 0;	
	int							Configuration::print_globalip = 0;
	int							Configuration::print_port_mirror = 0;
	int							Configuration::print_dnshelper= 0;
	int							Configuration::print_httpdirpolicy= 0;
	
	std::string					Configuration::key		= "";
	std::string					Configuration::value	= "";

	std::string					Configuration::netaddr_config_filename = "/etc/reserved-networks.xml";
	std::string					Configuration::rfc_proto_config_filename = "/etc/rfc-protocols.xml";
	std::string					Configuration::l7_proto_config_filename = "/etc/protoinfo.xml";
    std::string					Configuration::main_config_filename	= "";

    /*2012-09-28 add prot_scan */
    int                         Configuration::print_portscan = 0;
        
    

    int							Configuration::debug	= 0;
	int							Configuration::syslog	= 0;
	int							Configuration::console	= 0;
	
	int							Configuration::delete_node	= 0;
	int							Configuration::replace_node	= 0;
	int							Configuration::get_node_string	= 0;

	/* Date:2013-8-12 11:47:52 Add by chenhao  */
	int							Configuration::zero_ping = 0;
	Configuration::Configuration(int *argc, char ***argv)
    {

		int			 version = 0;
		int			 usage = 0;
		
		char * netaddr_conf_file;
		char * rfc_proto_conf_file;
		char * l7_proto_conf_file;
		char * main_conf_file;
		char * xmlkey;
		char * xmlvalue;

		char tempbuff[1024];
		snprintf(tempbuff, 1023, "%s - %s , version %s\n\n", PACKAGE_LONG_NAME, PACKAGE_DESCRIPTION, PACKAGE_VERSION);

		optTitle(tempbuff);

		optProgName(PACKAGE_NAME);

		optrega(&this->debug,					OPT_INT,				'd', "debug",			" debug level #");
		optrega(&main_conf_file,				OPT_STRING,				'f', "configfile",		" configure file name");
		optrega(&netaddr_conf_file,				OPT_STRING,				'n', "netaddr",			" network address configure file name");
		optrega(&rfc_proto_conf_file,			OPT_STRING,				'P', "rfcproto",		" rfc protocols configure file name");
		optrega(&l7_proto_conf_file,			OPT_STRING,				'l', "l7proto",			" l7 protocols configure file name");
		optrega(&xmlkey,						OPT_STRING,				'k', "xmlkey",			" xml key name to read or write");
		optrega(&xmlvalue,						OPT_STRING,				'v', "xmlvalue",		" xml value to write");
		optrega(&this->syslog,					OPT_FLAG,				's', "syslog",			" log to syslog");
		optrega(&this->console,					OPT_FLAG,				'o', "console",			" log to console");
		optrega(&this->read,					OPT_FLAG,				'r', "read",			" read a key value from xml configure file");
		optrega(&this->write,					OPT_FLAG,				'w', "write",			" write a key value to xml configure file");
		optrega(&this->parse,					OPT_FLAG,				'p', "parse",			" parse xml configure file");
		optrega(&this->check,					OPT_FLAG,				'c', "check",			" check xml configure file");
		optrega(&this->print_fw,				OPT_FLAG,				'F', "printfw",			" print firewall command");
		optrega(&this->print_action,			OPT_FLAG,				'A', "printaction",			" print firewall action command");
		optrega(&this->print_macbind,			OPT_FLAG,				'b', "printmacbind",			" print  mac bind command");
		optrega(&this->print_macauth,			OPT_FLAG,				'a', "printmacauth",			" print  mac auth command");
		optrega(&this->print_quota,				OPT_FLAG,				'q', "printquota",			" print  quotacommand");
		optrega(&this->print_globalip,			OPT_FLAG,				'G', "printgip",		" print global ip command");
		optrega(&this->print_port_mirror,		OPT_FLAG,				'm', "printmirror",		" print port mirror command");		
		optrega(&this->print_dnshelper,			OPT_FLAG,				'D', "printdnshelper",		" print dnshelper command");	
		optrega(&this->print_httpdirpolicy,		OPT_FLAG,				'H', "printhttpdirpolicy",		" print http directory policy command");	

        /*2012-09-28 add port_scan*/
        optrega(&this->print_portscan,			    OPT_FLAG,			'S', "portscan",			" portscan xml configure file");

		optrega(&this->delete_node,				OPT_FLAG,				'\0', "deletenode",		" delete node a key value from xml configure file");	
		optrega(&this->replace_node,			OPT_FLAG,				'\0', "replacenode",		" replace a node for a key value from xml configure file");	
		//optrega(&this->replace_node,			OPT_FLAG,				'\0', "getnode",		" get a node string for a key value from xml configure file");	

		optrega(&version,						OPT_FLAG,				'V', "version",			" version info");
		optrega(&usage,							OPT_FLAG,				'h', "help",			" display usage info");
		
		optAdditionalUsage(this->additionalUsage);

		opt(argc, argv);
		// parse command line and/or read config file
		//optDefaultFile(CONF_FILE);
		//opt(argc, argv);


		if (version)
		{
			printf("%s - %s , version %s\n", PACKAGE_LONG_NAME, PACKAGE_DESCRIPTION, PACKAGE_VERSION);
			exit(-1);
		}
		if (usage)
		{
			optPrintUsage();
			exit(-1);
		}
        
		if (!optinvoked(&main_conf_file))
		{
			optPrintUsage();
			std::cerr  << std::endl << "error: no xml configure file name provided." << std::endl;
			exit(-1);
		}

		if ((read > 0 || write > 0)&& !optinvoked(&xmlkey))
		{
			optPrintUsage();
			std::cerr  << std::endl << "error: no xml key name provided." << std::endl;
			exit(-1);
		}

		if (write > 0 && !optinvoked(&xmlvalue))
		{
			optPrintUsage();
			std::cerr  << std::endl << "error: no xml value provided." << std::endl;
			exit(-1);
		}

		if (optinvoked(&xmlvalue))
		{
			value = xmlvalue;
		}
		if (optinvoked(&xmlkey))
		{
			key = xmlkey;
		}
		if (optinvoked(&netaddr_conf_file))
		{
			netaddr_config_filename = netaddr_conf_file;
		}
		if (optinvoked(&l7_proto_conf_file))
		{
			l7_proto_config_filename = l7_proto_conf_file;
		}
		if (optinvoked(&l7_proto_conf_file))
		{
			l7_proto_config_filename = l7_proto_conf_file;
		}
		if (optinvoked(&main_conf_file))
		{
			main_config_filename = main_conf_file;
		}
	
		opt_free();
	}

	Configuration::~Configuration()
    {

    }


}
