#ifndef _CONSOLELOGSYSTEM_H
#define _CONSOLELOGSYSTEM_H
#include <string>
#include "../../global.h"

/**
  *@author xxbin
  */
namespace maxnet{

	class Consolelog {
		public: 
			Consolelog(std::string ident);
			~Consolelog();
			int setLogLevel(int level);
			void log(int level, char *pbuff, int len);
			int ReOpen();
		protected:
			std::string sLogIdent;
			char		szBuffer[2048];
			int			nLevel;
	};
}
#endif
