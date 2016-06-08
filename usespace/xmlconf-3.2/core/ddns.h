#ifndef __DYNAMIC_DNS__
#define __DYNAMIC_DNS__

#include <iostream>
#include <string>
#include <vector>

namespace maxnet
{

	class RouteDDns 
	{

		public:

			RouteDDns();
			~RouteDDns();
            bool isVaild();
			void output();

	    void setDDnsEnable(bool  DDnsEnable);
          
            void setUsername(std::string username);
            void setPassword(std::string password);
            void setDDnsDns(std::string DDnsDns);
	     	void setDDnsProvider(std::string provider);

            bool isDDnsEnable();       
            std::string getUsername();
            std::string getPassword();
            std::string getDDnsDns();
	     	std::string  getDDnsProvider();		
            
        private:

            bool DDnsEnable;
            std::string username;
            std::string password;
            std::string DDnsDns;
	     	std::string provider;	
	};
}
#endif

