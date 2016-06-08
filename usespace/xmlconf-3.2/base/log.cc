#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>


namespace maxnet{

	Log::Log(const char *ident, bool bsyslog, bool bfilelog, bool bconsolelog){
		sLogIdent = ident;
		bSyslog = bsyslog;
		bFilelog = bfilelog;
		bConsolelog = bconsolelog;
		
		pSyslog = NULL;
		pFilelog = NULL;
		pConsolelog = NULL;
		setLogLevel(LOG_DEBUG);	
		Open();

	}

	Log::~Log(){
		Close();
	}

	int Log::Open(){
		if(bSyslog){
			if(pSyslog) delete pSyslog;
			pSyslog = NULL;
			pSyslog = new Syslog(sLogIdent.c_str(),LOG_NDELAY,LOG_USER);
		}
		if(bFilelog){
			if(pFilelog) delete pFilelog;
			pFilelog = NULL;
			std::string sFileName;
			sFileName = sLogIdent;
			pFilelog = new Filelog(sLogIdent, sFileName);

		}
		if(bConsolelog){
			if(pConsolelog) delete pConsolelog;
			pConsolelog = NULL;
			pConsolelog = new Consolelog(sLogIdent.c_str());
		}
		setLogLevel(getLogLevel());
		return 0;	
	}


	int Log::Close(){
		if(bSyslog && pSyslog != NULL){
		    delete pSyslog;
			pSyslog = NULL;
		}
		if(bFilelog && pFilelog != NULL){
			delete pFilelog;
			pFilelog = NULL;
		}
		if(bConsolelog && pConsolelog != NULL){
		    delete pConsolelog;
			pConsolelog = NULL;
		}
		return 0;
	}

	int Log::ReOpen(){
		if(bSyslog && pSyslog){
		pSyslog->ReOpen();
		}
		if(bFilelog && pFilelog){
			pFilelog->ReOpen();
		}
		if(bConsolelog && pConsolelog){
		pConsolelog->ReOpen();
		}
		return 0;
	}


	int Log::setLogLevel(int level){
		if(bSyslog && pSyslog){
		pSyslog->setLogLevel(level);
		}
		if(bFilelog && pFilelog){
			pFilelog->setLogLevel(level);
		}
		if(bConsolelog && pConsolelog){
		pConsolelog->setLogLevel(level);
		}
		if(level >= LOG_EMERG && level <= LOG_DEBUG){
			nLevel = level;
				return level;
		}
		return -1;		
	}

	void Log::debug(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_DEBUG, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::info(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_INFO, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::notice(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_NOTICE, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::warning(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_WARNING, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::alert(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_ALERT, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::error(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_ERR, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::crit(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char *buf = new char[2048];
		vsnprintf(buf, 2047, fmt, args);
		log(LOG_CRIT, buf);
		delete buf;
		buf = NULL;
		va_end(args);
	}

	void Log::log(int level, char *buff, int len){

		if(bSyslog && pSyslog){
		pSyslog->log(level, buff, len);
		}
		if(bFilelog && pFilelog){
			pFilelog->log(level, buff, len);
		}
		if(bConsolelog && pConsolelog){
		pConsolelog->log(level, buff, len);
		}

	}

	void Log::log(int level, char *buff){
		log(level, buff, strlen(buff));
	}

	int Log::getLogLevel(){
		return nLevel;
	}

}
