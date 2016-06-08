#ifndef _ZERO_SYSTEM_H
#define _ZERO_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>

#define MAX_MTU 5000

namespace maxnet{


	class System {

		public:

			System();
			~System();
			bool isVaild();
			void output();

			void setHostName(std::string hostname);
			void setDomainName(std::string domainname);
			void setContact(std::string contact);
			void setLocation(std::string location);
			void setPassword(std::string password);
            void setPassword2(std::string password2);

			void setDBP(bool enable);
			void setEnableSTP(bool enable);
#ifdef CONFIG_IXP
            void setEnableFAutoUpdate(bool enable);
            void setEnableMAutoUpdate(bool enable);
#else
			void setEnableAutoUpdate(bool enable);
#endif
			void setUpdateURL(std::string url);
			
			void setSNMPenable(bool enable);
			void setSNMPRO(std::string com);
			void setSNMPRW(std::string com);

            void setmax_bridgenum(std::string max_bridgenum);
            void setbridgenum(std::string bridgenum);

			std::string getHostName();
			std::string getDomainName();
			std::string	getContact();
			std::string getLocation();
			std::string getPassword();
            std::string getPassword2();
            std::string getmax_bridgenum();
            std::string getbridgenum();
            
			std::string getMTU();
			int			getMTUNum();

			bool		isSTPenable();
#ifdef CONFIG_IXP
            bool        isFAutoUpdateEnable();
            bool        isMAutoUpdateEnable();
#else
			bool		isAutoUpdateEnable();
#endif
			bool		isSNMPenable();
			std::string getSNMPRO();
			std::string getSNMPRW();

			std::string	getUpdateURL();
			std::string getTimezone();

			void setTimezone(std::string tz);
			void setMTU(std::string mtu);

			std::string getNTPServer();
			void setNTPServer(std::string server);

			bool		isDBP();
			bool		isNTPenable();
			void		setEnableNTP(bool enable);
			bool		checkLinkState(const char *interfaceName);
		private:
			std::string	hostname;
			std::string domainname;
			std::string	contact;
			std::string location;
			std::string password;
            std::string password2;
			std::string time_zone;
			std::string ntp_server;
			std::string mtu;
            std::string max_bridgenum;
            std::string bridgenum;

			bool		enableNTP;
			bool		enableSTP;
#ifdef CONFIG_IXP
            bool        enableFAutoUpdate;
            bool        enableMAutoUpdate;
#else
			bool		enableAutoUpdate;
#endif
			bool		enableDBP;
			bool		enableSNMP;
			std::string	SNMPRO;
			std::string SNMPRW;

			std::string	updateURL;

	};

}

#endif // _ZERO_SYSTEM_H
