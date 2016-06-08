#include "syslog.h"
#include <syslog.h>
#include <string.h>

namespace maxnet{

	Syslog::Syslog(const char *ident, int option, int  facility){
		strncpy(&szLogIdent[0], ident, 128);
		nOption = option;
		nFacility = facility;
		Open();
		setLogLevel(LOG_INFO);
	}

	Syslog::~Syslog(){
		Close();
	}

	int Syslog::Open(){
		openlog(&szLogIdent[0], nOption, nFacility);
		return 0;
	}

	int Syslog::Close(){
		closelog();
		return 0;
	}

	int Syslog::setLogLevel(int level){
		if(level >= LOG_EMERG && level <= LOG_DEBUG){
			nLevel = level;
				return nLevel;
		}
		return -1;		
	}

	void Syslog::log(int level, char *pbuff, int len){
		if(nLevel >= level){
				syslog(level | nFacility, pbuff, len);
		}
	}

	int Syslog::ReOpen(){
		Close();
		Open();
		return 0;
	}
}
