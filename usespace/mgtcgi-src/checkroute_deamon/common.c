#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#define CMD_LENGTH                  256
#define STRING_LENGTH               32
#define UMOUNT_XML_PROCESS_ID_FILE  "/var/run/umount_xml.pid"
#define CFDEV_FILE_PATH             "/etc/CFDEV"

void set_reuid()
{
	uid_t uid ,euid; 
	uid = getuid(); 
	euid = geteuid(); 
	setreuid(euid, uid);
}

int get_cfdev(char *cfdev,int lenth)
{
    int travl = 0;
    char cmd[CMD_LENGTH] = {0};
    char buffer[STRING_LENGTH] = {0};
    FILE *fp = NULL;
    FILE *pp = NULL;
    
    fp = fopen(CFDEV_FILE_PATH,"r");
    if(NULL == fp){
        travl = -1;   
        goto ERROREXIT;
    }
    
    if(fread(buffer,1,sizeof(buffer),fp) == 0){
        travl = -1;
        goto ERROREXIT;
    }
    fclose(fp);

    snprintf(cmd,sizeof(cmd),"/bin/mount -w |grep %s |awk -F ' ' '{print $3}'",buffer);
    pp = popen(cmd,"r");
    if(NULL == pp){
        travl = -1;
        goto ERROREXIT;
    }
    fgets(cfdev,lenth - 1,pp);
    pclose(pp);
    goto EXIT;
    
ERROREXIT:
    if(fp){
        fclose(fp);
    }
    if(pp){
        pclose(pp);
    }
    goto EXIT;
EXIT:
    return travl;
}

void get_process_id(void)
{
    int pp,pid,pid_lenth;
    char pid_str[STRING_LENGTH] = {0};
    pid = getpid();
    snprintf(pid_str,sizeof(pid_str),"%d",pid);
    pid_lenth = strlen(pid_str);
    
    pp = open(UMOUNT_XML_PROCESS_ID_FILE,O_RDWR|O_CREAT|O_TRUNC);
    if(write(pp,pid_str,pid_lenth) != pid_lenth){
        printf("Write pid file error!\n");
        close(pp);
        exit(1);
    }
    
    close(pp);
    return;
}


