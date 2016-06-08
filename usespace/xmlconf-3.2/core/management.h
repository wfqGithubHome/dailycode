#ifndef _ZERO_MANAGEMENT_H
#define _ZERO_MANAGEMENT_H

#include <iostream>
#include <string>
#include <vector>



namespace maxnet{


	class Management {

		public:

			Management();
			~Management();
			bool isVaild();

			void	setIP(const std::string	ip);
			void	setNetmask(const std::string netmask);
			void	setBroadcast(const std::string	broadcast);
			void	setGateway(const std::string gateway);
			void	setServer(const std::string server);
			void	setDNSPrimary(const std::string dns_primary);
			void	setDNSSecondary(const std::string dns_secondary);

			std::string 	getIP();
			std::string 	getNetmask();
			std::string 	getBroadcast();
			std::string 	getGateway();
			std::string 	getServer();
			std::string 	getDNSPrimary();
			std::string 	getDNSSecondary();
			
			void	output();

		private:
			std::string	ip;
			std::string netmask;
			std::string	broadcast;
			std::string gateway;
			std::string server;
			std::string dns_primary;
			std::string dns_secondary;

	};

}

#endif // _ZERO_MANAGEMENT_H
