#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H


/**
  *@author xxbin
  */
#include <string>
#include "log/syslog.h"
#include "log/filelog.h"
#include "log/consolelog.h"
#include <syslog.h>

#include "../global.h"

namespace maxnet {

	class Log {
	public: 
		Log(const char *ident, bool bsyslog, bool bfilelog, bool bconsolelog);
		~Log();

	  void log(int level, char *buff, int len);

	  int setLogLevel(int level);

	  int ReOpen();

	  int getLogLevel();

	  void log(int level, char *buff);

		void debug(const char* fmt, ...);
		void info(const char* fmt, ...);
		void notice(const char* fmt, ...);
		void warning(const char* fmt, ...);
		void alert(const char* fmt, ...);
		void error(const char* fmt, ...);
		void crit(const char* fmt, ...);


	  int Close();

	  int Open();	
	protected:
		std::string	sLogIdent;
		bool		bSyslog;
		bool		bFilelog;
		bool		bConsolelog;
		int			nLevel;

		Syslog		*pSyslog;
		Filelog		*pFilelog;
		Consolelog	*pConsolelog;

	};

}

#endif
