#ifndef _ZERO_ROUTE_ADSL_H
#define _ZERO_ROUTE_ADSL_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteADSLClient : public Node{

		public:

			RouteADSLClient();
			~RouteADSLClient();
			bool isVaild();
			std::string getCommand();
			void output();

			//void setName(const std::string name);
			void setIfname(const std::string ifname);
			void setUserName(const std::string username);	
			void setPassword(const std::string password);
			void setServerName(const std::string servername);
			void setMTU(const std::string mtu);
			void setWeight(const std::string weight);
			void setDefaultRoute(bool defaultroute);
			void setPeerDNS(bool peerdns);
			void setEnable(bool enable);
			

			//std::string getName();
			std::string getIfname();
			std::string getUserName();
			std::string getPassword();
			std::string getServerName();
			std::string getMTU();
			std::string getWeight();
			bool		getDefaultRoute();
			bool		getPeerDNS();
			bool		getEnable();

		private:
			//std::string name;
			std::string ifname;
			std::string username;
			std::string password;
			std::string servername;
			std::string mtu;
			std::string weight;
			bool		defaultroute;
			bool		peerdns;
			bool		enable;
	};

}

#endif // _ZERO_VLAN_H
