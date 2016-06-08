#ifndef _ZERO_LOGWRAPER_H
#define _ZERO_LOGWRAPER_H

#include <iostream>
#include <string>
#include <strstream> 
#include <stdarg.h>
#include "log.h"
#include "../global.h"
namespace maxnet{

class LogWraper {

  public:
    LogWraper();
	~LogWraper();

	void	debug(const char* fmt, ...);
	void	info(const char* fmt, ...);
	void	notice(const char* fmt, ...);
	void	warning(const char* fmt, ...);
	void	alert(const char* fmt, ...);
	void	error(const char* fmt, ...);
	void	crit(const char* fmt, ...);

	void	log(int level, char *buff, int len);
	void	log(int level, char *buff);
	void	log(char *buff);
	Log    *getLog();
	void    setLog(Log *log);
	void	setLogHeader(std::string header);
  protected:
	std::string m_strLogHeader;
    	Log	*m_Log;

};

}

#endif // _ZERO_LOGWRAPER_H
