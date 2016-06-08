#include "logwraper.h"
#include "../global.h"

namespace maxnet{

	LogWraper::LogWraper(){
		m_Log = NULL;
	}

	LogWraper::~LogWraper(){

	}

	Log *LogWraper::getLog(){
		return m_Log;
	}

	void LogWraper::setLog(Log * log){
		m_Log = log;
	}

	void LogWraper::setLogHeader(std::string header){
		m_strLogHeader = header;
	}

	void LogWraper::log(int level, char *buff, int len){
		if(m_Log != NULL){
			char *logbuff = new char[2048];
			snprintf(logbuff, 2047, "%s%s", m_strLogHeader.c_str(), buff);
			m_Log->log(level, logbuff, strlen(logbuff));
			delete logbuff;
		}
	}

	void LogWraper::log(int level, char *buff){
		log(level, buff, strlen(buff));
	}


	void LogWraper::log(char *buff){
		log(LOG_INFO, buff, strlen(buff));
	}

	void LogWraper::debug(const char* fmt, ...)
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

	void LogWraper::info(const char* fmt, ...)
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

	void LogWraper::notice(const char* fmt, ...)
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

	void LogWraper::warning(const char* fmt, ...)
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

	void LogWraper::error(const char* fmt, ...)
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


	void LogWraper::alert(const char* fmt, ...)
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

	void LogWraper::crit(const char* fmt, ...)
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
}

