#ifndef ZERO_ROUTE_DNS_H
#define ZERO_ROUTE_DNS_H

#include <iostream>
#include <string>
#include <vector>

#define MAX_MTU 5000

namespace maxnet{


	class RouteDNS {

		public:

			RouteDNS();
			~RouteDNS();
			bool isVaild();
			void output();

			void setDNSPrimary(const std::string dns_primary);
			void setDNSSecondary(const std::string dns_secondary);
			void setDNSThirdary(const std::string dns_thirdary);
			
			std::string getDNSPrimary();
			std::string getDNSSecondary();
			std::string	getDNSThirdary();


		private:
			std::string dns_primary;
			std::string dns_secondary;
			std::string dns_thirdary;
	};

}

#endif // _ZERO_SYSTEM_H
