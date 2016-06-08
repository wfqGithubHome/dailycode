#ifndef SYSLOGSYSTEM_H
#define SYSLOGSYSTEM_H

#include "../../global.h"
/**
  *@author xxbin
  */
namespace maxnet{
	class Syslog {
	public: 
		Syslog(const char *ident, int option, int  facility);
		~Syslog();

	  void log(int level, char *pbuff, int len);

	  int setLogLevel(int level);

	  int ReOpen();

	  int  Close();

	  int  Open();
	protected: 
		char szLogIdent[128];
		int  nOption;
		int  nFacility;
		int  nLevel;

	};
}
#endif
