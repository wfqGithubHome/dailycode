#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#define STRING_LENGTH  32
#define UMOUNT_XML_PROCESS_ID_FILE  "/var/run/umount_xml.pid"

void get_process_id(void)
{
    int pp,pid,pid_lenth;
    char pid_str[STRING_LENGTH] = {0};
    pid = getpid();
    snprintf(pid_str,sizeof(pid_str),"%d",pid);
    pid_lenth = strlen(pid_str);
    
    pp = open(UMOUNT_XML_PROCESS_ID_FILE,O_RDWR|O_CREAT);
    if(write(pp,pid_str,pid_lenth) != pid_lenth){
        printf("Write pid file error!\n");
        close(pp);
        exit(1);
    }
    
    close(pp);
    return;
}


void init_daemon(void) 
{ 
	int pid, fd, fdtablesize; 
	int i;

	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGHUP ,SIG_IGN);

	pid=fork();
	if(pid > 0) 
		exit(0);		// close father process
	else if(pid< 0) 
		exit(1);		// client 1 error
	/* frist client process */
	if (setsid() < 0) exit (1);
	/* leave the TTY */
	pid=fork();
	if(pid > 0) 
		exit(0);	// close frist client process
	else if(pid< 0) 
		exit(1);	// client 2 error.
	
	/* close optend fd */
	for(i = 0; i < NOFILE; ++i)//
		close(i);

	for (fd = 0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++)
		close(fd);

    /* get process pid file */
    get_process_id();

	/* chang work directory */	
	chdir("/tmp");
	
	/* set umask */
	umask(0);
	return; 
} 
