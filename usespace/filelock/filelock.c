#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dlfcn.h>

#include <unistd.h>

#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/file.h>

#include "filelock.h"

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format,args...)
#endif

#define FILELOCK_LONG_NAME        		"FILELOCK"
#define FILELOCK_NAME                   "FILELOCK"
#define FILELOCK_DESCRIPTION      		"FILELOCK"
#define FILELOCK_VERSION             	"1.0.0"


struct filelock_info filelock_config;

static int filelock_command_parse(int *argc, char ***argv)
{
	char buff[1024];

	memset(&filelock_config, 0, sizeof(struct filelock_info));

	snprintf(buff, 1023, "%s - %s ,version %s\n\n", 
	FILELOCK_LONG_NAME,FILELOCK_DESCRIPTION, FILELOCK_VERSION);
	optTitle(buff);
	optProgName(FILELOCK_NAME);
	
	optrega(&filelock_config.lockfile,       	OPT_STRING,             'f', "file",        "lock file");
    optrega(&filelock_config.dfile,       	    OPT_STRING,             'd', "dst",         "action file");
    optrega(&filelock_config.get,            	OPT_FLAG,               'g', "get",         "get lock file");
	optrega(&filelock_config.set,               OPT_FLAG,               's', "set",         "set lock file");
	optrega(&filelock_config.help,          	OPT_FLAG,               'h', "help",        "usage");

	opt(argc, argv); 
    if(filelock_config.get + filelock_config.set != 1)
        return DEV_ERR;
    if(filelock_config.lockfile == NULL || filelock_config.dfile == NULL)
        return DEV_ERR;
	return DEV_OK;
}


int filelock_get_filename(char *dst, char *lock)
{
    int i=0,j=0;
    char *filename;
    for(i=0;i<strlen(dst);i++)
    {
        if(dst[i] == '/')
        {
            j = i + 1;
        }
    }
    filename = &dst[j];
    sprintf(lock,"/var/lock/%s.lock",filename);
    return 0;
}

int main(int argc, char *argv[])
{
	int ret = DEV_ERR; 
    int fd;
    char lock[64]={0};
    char cmd[256]={0};
    FILE *pp;

    if(filelock_command_parse(&argc, &argv)!=DEV_OK){
        optPrintUsage();
		return ret;
    }
    
    filelock_get_filename(filelock_config.lockfile,lock);
    fd=open(lock,O_WRONLY|O_CREAT);
    if(fd < 0){
        return ret;
    }

    if(flock(fd,LOCK_EX)!=0){
	    close(fd);
		return ret;
    }

    if(filelock_config.get)
        snprintf(cmd, sizeof(cmd), "/bin/cp %s %s", filelock_config.lockfile,filelock_config.dfile);
    else
        snprintf(cmd, sizeof(cmd), "/bin/cp %s %s", filelock_config.dfile,filelock_config.lockfile);

    pp = popen(cmd, "r");
    pclose(pp);
    
    flock(fd,LOCK_UN);
	close(fd);
    
	return 0;	
}

