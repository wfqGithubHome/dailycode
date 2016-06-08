#include "filelog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>

namespace maxnet{
	Filelog::Filelog(std::string ident, std::string filename){
		sLogIdent =  ident;
		sFileName =  filename;
		Open();	
		setLogLevel(LOG_INFO);
	}

	Filelog::~Filelog(){
		Close();
	}

	int Filelog::Open(){
#if 1
	    hFile = open(sFileName.c_str(), O_CREAT | O_APPEND | O_WRONLY, 00644);
		return hFile;
#endif
	//	char len_buff[9] = {0};
	//	hFile = fopen(sFileName.c_str(),"w+");
		

	}


	int Filelog::Close(){
		return close(hFile);
	}

	int Filelog::ReOpen(){
		Close();
		return Open();
	}

	int Filelog::setLogLevel(int level){
		if(level >= LOG_EMERG && level <= LOG_DEBUG){
			nLevel = level;
				return nLevel;
		}
		return -1;	
	}

	void Filelog::log(int level, char *pbuff, int len){

		//fseek(pMyfile, -lOffset,SEEK_END);

		if(nLevel >= level){
			time_t currtime;	
			time(&currtime);
			char timebuff[128];
			char *ptime = ctime(&currtime);
			strncpy(&timebuff[0], ptime, 128);
			timebuff[strlen(timebuff) - 1] = '\0';
			snprintf(&szBuffer[0], 2047, "%s %s: %s\n", &timebuff[0], sLogIdent.c_str(), pbuff);
			int len = strlen(szBuffer);
			RawLog(&szBuffer[0], len);
		}
	}

	void Filelog::RawLog(const char *pbuff, int len){
			write(hFile, pbuff, len);
	}
}

#if 0
#include <stdio.h>

void main()
{
  FILE* pMyfile,pFileBak;
  char list[30];
  char buf[30];
  char insert[10];
  int i,j;
  long lOffset;

  for(i=0;i<20;i++){
    list[i]='a'+i;
  }

  for(j=0;j<10;j++){
    insert[j]='A'+j;
  }

  lOffset=10;
  pMyfile=fopen("txt1.txt","w+");
  fwrite(list,sizeof(char),20,pMyfile);
 
  fseek(pMyfile, -lOffset,SEEK_END);
  fread(buf,sizeof(char),lOffset,pMyfile);

  fseek(pMyfile,-lOffset,SEEK_END);
  fwrite(insert,sizeof(char),10,pMyfile);

  fseek(pMyfile,0,SEEK_END);
  fwrite(buf,sizeof(char),10,pMyfile);
  fclose(pMyfile);
}
#endif
