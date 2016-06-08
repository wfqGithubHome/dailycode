#include "auth.h"
#include "../global.h"
#include <sstream>

namespace maxnet{

	Auth::Auth(){
		enable = false;
	}

	Auth::~Auth(){
		
	}
	
	bool Auth::isVaild(){
		
		return true;
	}

	void Auth::output(){
		int      i ;
		char *buf;
		char *sep=":";
		char *temp,*ptr;
		
		std::cout << std::endl << "# auth" << std::endl;
		
		if(this->getEnable()){
			std::cout << "AUTH_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "AUTH_ENABLE='0'" << std::endl;
		}

		std::cout << "SAM_NUM=\"" << this->getSamNum() << "\"" << std::endl;

		std::cout << "SAM_AUTH_SERVER=\"" << this->getSamServer() << "\"" << std::endl;

		int samnum = atoi(this->getSamNum().c_str());

		buf = (char*)this->getSamServerPrivateAddr().c_str();
		temp=strtok_r(buf,sep,&ptr);
		for(i=0;i<samnum;i++){
			std::cout << "SAM_SERVER_PRIVATE_ADDR"<<i<<"=\"" << temp<< "\"" << std::endl;
			temp=strtok_r(NULL,sep,&ptr);
			if(temp == NULL){
				break;
			}
		}
		
		std::cout << "AUTH_TIMEOUT_MODE='" << this->getTimeoutMode() << "'" << std::endl;
		std::cout << "AUTH_TIMEOUT_SECONDS='" << this->getTimeoutSeconds() << "'" << std::endl;

		if(this->getEnableMutilUser()){
			std::cout << "AUTH_ENABLE_MUTIL_USER='1'" << std::endl;
		}
		else{
			std::cout << "AUTH_ENABLE_MUTIL_USER='0'" << std::endl;
		}

		std::cout << "OVERTIME_USERS_SEND_RATE_LIMIT='" << this->getOvertimeUsersSendRate() << "'" << std::endl;
		std::cout << "USERFLOW_SEND_RATE_LIMIT='" << this->getUserflowSendRate()<< "'" << std::endl;

		std::cout << "AUTH_REDIRECT_ADV_URL=\"" << this->getDenyGuestAccess()<< "\"" << std::endl;

		if(this->getRedirectHTTPRequest()){
			std::cout << "AUTH_REDIRECT_HTTP_REQUEST='1'" << std::endl;
		}
		else{
			std::cout << "AUTH_REDIRECT_HTTP_REQUEST='0'" << std::endl;
		}

		std::cout << "SAM_REDIRECT_HTTP_RATE_LIMIT=\"" << this->getRedirectHTTPRate()<< "\"" << std::endl;

		std::cout << "AUTH_REDIRECT_HTTP_URL=\"" << this->getRedirectHTTPURL() << "\"" << std::endl;
		return;

	}

	void Auth::setEnable(bool enable)
	{
		this->enable = enable;
	}
	
	void Auth::setTimeoutMode(std::string mode)
	{
		this->timeoutMode = mode;
	}

	void Auth::setTimeoutSeconds(std::string secs)
	{
		this->timeoutSeconds = secs;
	}

	void Auth::setEnableMutilUser(bool enable)
	{
		this->enableMutilUser = enable;
	}


	void Auth::setDenyGuestAccess(std::string url)
	{
		this->denyGuestAccess = url;
	}
	
	void Auth::setRedirectHTTPRequest(bool enable)
	{
		this->enableRedirectHTTP = enable;
	}
	
	void Auth::setRedirectHTTPURL(std::string url)
	{
		this->redirectURL = url;
	}

	void Auth::setSamNum(std::string num)
	{
		this->SamNum = num;
	}
	
	void Auth::setSamServer(std::string sam)
	{
		this->SamServer = sam;
	}

	void Auth::setSamServerPrivateAddr(std::string addr)
	{
		this->SamServerPrivateAddr = addr;
	}
	
	void Auth::setOvertimeUsersSendRate(std::string rate)
	{
		this->OvertimeUsersSendRate = rate;
	}
	void Auth::setUserflowSendRate(std::string rate)
	{
		this->UserflowSendRate = rate;
	}

	void Auth::setRedirectHTTPRate(std::string rate)
	{
		this->RedirectHttpRate = rate;
	}

	bool Auth::getEnable()
	{
		return this->enable;
	}
	
	int Auth::getTimeoutMode()
	{	
		std::stringstream streamMode;
		streamMode << timeoutMode;
		int  nMode = 0;
		streamMode >> nMode;	
		return nMode;
	}
	
	int Auth::getTimeoutSeconds()
	{	
		std::stringstream streamSecs;
		streamSecs << timeoutSeconds;
		int  nSecs = 0;
		streamSecs >> nSecs;	
		return nSecs;
	}

	bool Auth::getEnableMutilUser()
	{
		return this->enableMutilUser;
	}
	
	std::string Auth::getDenyGuestAccess()
	{
		return this->denyGuestAccess;
	}
	
	bool Auth::getRedirectHTTPRequest()
	{
		return this->enableRedirectHTTP;
	}
	
	std::string Auth::getRedirectHTTPURL()
	{
		return this->redirectURL;
	}

	std::string Auth::getSamServer()
	{
		return this->SamServer;
	}

	std::string Auth::getSamNum()
	{
		return this->SamNum;
	}

	std::string Auth::getSamServerPrivateAddr()
	{
		return this->SamServerPrivateAddr;
	}

	std::string Auth::getRedirectHTTPRate()
	{
		return this->RedirectHttpRate;
	}
	
	std::string Auth::getOvertimeUsersSendRate()
	{
		return this->OvertimeUsersSendRate;
	}
	
	std::string Auth::getUserflowSendRate()
	{
		return this->UserflowSendRate;
	}
}

