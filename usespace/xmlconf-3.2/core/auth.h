#ifndef _ZERO_AUTH_H
#define _ZERO_AUTH_H

#include <iostream>
#include <string>
#include <vector>


namespace maxnet{


	class Auth {

		public:

			Auth();
			~Auth();
			bool isVaild();
			void output();

			void setEnable(bool enable);
			void setTimeoutMode(std::string mode);
			void setEnableMutilUser(bool enable);
			void setDenyGuestAccess(std::string url);
			void setRedirectHTTPRequest(bool enable);
			void setRedirectHTTPURL(std::string url);
			void setTimeoutSeconds(std::string secs);
			/*add new by Donald on 2009.12.14 */
			void setSamServer(std::string sam);
			void setOvertimeUsersSendRate(std::string rate);
			void setUserflowSendRate(std::string rate);

			void setSamNum(std::string num);
			void setSamServerPrivateAddr(std::string addr);
			void setRedirectHTTPRate(std::string rate);

			bool getEnable();
			int	 getTimeoutMode();
			bool getEnableMutilUser();
			std::string getDenyGuestAccess();
			bool getRedirectHTTPRequest();
			std::string getRedirectHTTPURL();
			int getTimeoutSeconds();
			/*add new by Donald on 2009.12.14 */
			std::string getSamServer();
			std::string getOvertimeUsersSendRate();
			std::string getUserflowSendRate();
			std::string getSamNum();
			std::string getSamServerPrivateAddr();
			std::string getRedirectHTTPRate();

	
		private:
			std::string	redirectURL;
			std::string timeoutMode;
			std::string timeoutSeconds;
			/*add new by Donald on 2009.12.14 */
			std::string SamServer;
			std::string OvertimeUsersSendRate;
			std::string UserflowSendRate;
			bool		enable;
			bool		enableMutilUser;
			std::string denyGuestAccess;
			bool		enableRedirectHTTP;
			std::string SamNum;
			std::string SamServerPrivateAddr;
			std::string RedirectHttpRate;
	};

}

#endif // _ZERO_AUTH_H
