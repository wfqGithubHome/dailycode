#ifndef FILELOGSYSTEM_H
#define FILELOGSYSTEM_H

#include <string>
#include "../../global.h"
/**
  *@author xxbin
  */
#include <sys/types.h>

namespace maxnet{

	class Filelog {
	public: 
		Filelog(std::string ident,std::string filename);
		~Filelog();

	  int	ReOpen();

	  int	setLogLevel(int level);

	  void	log(int level, char *pbuff, int len);
	  int	Close();
	  int	Open();
      void RawLog(const char *pbuff, int len);
	protected:
		std::string sLogIdent;
		std::string sFileName;
		char szBuffer[2048];
		int  nLevel;
	protected: 
		int hFile;
		


	};
}
#endif
