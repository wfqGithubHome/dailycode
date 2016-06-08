/*
 * Copyright (c) 2001 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef ROUTE_CONFIG_HH
#define ROUTE_CONFIG_HH

#include <iostream>
#include <map>
#include <string>
#include <sys/types.h>
#include <opt.h>
#include "../global.h"


#define ROUTE_PACKAGE_NAME "route-xmlconf"
#define ROUTE_PACKAGE_LONG_NAME "route-xmlconf"
#define ROUTE_PACKAGE_DESCRIPTION "Maxnet XML Route Configuration File Manager"
#define ROUTE_PACKAGE_VERSION "0.1.00"


namespace maxnet {

class RouteConfiguration
    {
	  public:
		// Construction and Destruction.
		explicit RouteConfiguration(int*, char***);
		~RouteConfiguration();

		static int RouteAdditionalUsage() 
		{ 
			std::cout <<
			"xmlconf is Copyright (c) by Maxnet System Inc. 2011.\n";
			return 0;
		};

		static int							read;
		static int							write;
		static int							check;
		static int							parse;

		static int							print_dns;
		static int							print_ddns;
		
		static int							print_lan;
		static int							print_wan;
        static int                          print_adsl_client;
		static int                          print_l2tpvpn_client;

		static int                          print_loadb;
		static int                          print_routing;
		static int                          print_route_table;
		static int                          print_route_rule;
		
		static int                          print_dnat;
		static int                          print_snat;
		static int 							print_macbind;
		static int 							print_adsl_client_passwd;
        static int                          print_routing_command;

		static int							debug;
		static int							syslog;
		static int							console;
		
		static std::string					key;
		static std::string					value;
		static std::string					main_config_filename;

	  private:
		RouteConfiguration(const RouteConfiguration&);
		RouteConfiguration& operator= (const RouteConfiguration&);
    };
}

//extern const maxnet::Configuration* config;

#endif
