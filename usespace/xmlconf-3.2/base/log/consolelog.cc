#include "consolelog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>

namespace maxnet{

	Consolelog::Consolelog(std::string ident){
		sLogIdent = ident;
		setLogLevel(LOG_INFO);
	}

	Consolelog::~Consolelog(){
		
	}

	int Consolelog::ReOpen(){
		return 0;
	}

	void Consolelog::log(int level, char *pbuff, int len){
		if(nLevel >= level){
			time_t currtime;	
			time(&currtime);
			char timebuff[128];
			char *ptime = ctime(&currtime);
			strncpy(&timebuff[0], ptime, 128);
			timebuff[strlen(timebuff) - 1] = '\0';
			snprintf(&szBuffer[0], 2047, "%s %s: %s\n", &timebuff[0], sLogIdent.c_str(), pbuff);
			printf(&szBuffer[0]);

		
		}
	}

	int Consolelog::setLogLevel(int level){
		if(level >= LOG_EMERG && level <= LOG_DEBUG){
			nLevel = level;
				return nLevel;
		}
		return -1;	
	}
}
