/*
 * Copyright (c) 2001 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef CONFIG_HH
#define CONFIG_HH

#include <iostream>
#include <map>
#include <string>
#include <sys/types.h>
#include <opt.h>
#include "../global.h"

#define PACKAGE_NAME "xmlconf"
#define PACKAGE_LONG_NAME "xmlconf"
#define PACKAGE_DESCRIPTION "Yunbs XML Configuration File Manager"
#define PACKAGE_VERSION "1.2.00"

#define CONF_FILE "/etc/xmlconf.conf"

namespace maxnet {
class Configuration
    {
	  public:
		// Construction and Destruction.
		explicit Configuration(int*, char***);
		~Configuration();

		static int additionalUsage() 
		{ 
			std::cout <<
			"xmlconf is Copyright (c) by Maxnet System Inc. 2005.\n";
			return 0;
		};

		// Run-time stuff.
		static int							read;
		static int							write;
		static int							parse;
		static int							check;
		static int							print_fw;
		static int							print_action;
		static int							print_globalip;
        static int                          print_port_mirror;
		static int                          print_macbind;

		static int 							print_macauth;
		static int 							print_quota;
		static int 							print_dnshelper;
		static int 							print_httpdirpolicy;
		static std::string					key;
		static std::string					value;
		static std::string					netaddr_config_filename;
		static std::string					rfc_proto_config_filename;
		static std::string					l7_proto_config_filename;
		static std::string					main_config_filename;
        
         /*2012-09-28 add prot_scan */
        static int                          print_portscan;

        
		static int							debug;
		static int							syslog;
		static int							console;

		static int							delete_node;
		static int							replace_node;
		static int							get_node_string;
		
		/* Date:2013-8-12 11:44:51 Add by chenhao */
		static int							zero_ping;
		// The error class throw in case version or usage information has
		// been requested and we're supposed to terminate.
		struct no_error { };

	  private:
		Configuration(const Configuration&);
		Configuration& operator= (const Configuration&);
    };
}

//extern const maxnet::Configuration* config;

#endif
