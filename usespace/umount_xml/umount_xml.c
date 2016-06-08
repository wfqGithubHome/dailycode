#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


#define DEFAULT_TIMER               10
#define CMD_LENGTH                  256
#define STRING_LENGTH               32
#define UMOUNT_XML_PROCESS_ID_FILE  "/var/run/umount_xml.pid"
#define UMOUNT_CMD                  "/bin/umount"
#define MOUNT_CMD                   "/bin/mount"
#define CFDEV_FILE_PATH             "/etc/CFDEV"

void init_daemon(void);
void get_process_id(void);

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

void do_umount_flash(void)
{
    char cmd[CMD_LENGTH] = {0};
    char cfdev[STRING_LENGTH] = {0};
    if(get_cfdev(cfdev,sizeof(cfdev)) != 0){
        return;
    }

    snprintf(cmd,sizeof(cmd),"%s %s",UMOUNT_CMD,cfdev);
    system(cmd);
    return;
}

void sig_func()
{
    do_umount_flash();
    return;
}

void del_timer()
{
    int t;
    t = alarm(0);
    if(0 != t){
        alarm(0);
    }
    return;
}

void add_timer()
{
    if(0 == alarm(0)){
        alarm(DEFAULT_TIMER);
    }
    return;
}

int get_running_status(const char *name)
{
	char 		buf[128];
	FILE 		*pp;
	char cmd[CMD_LENGTH] = {0};
	int	 runs = 0;

	snprintf(cmd, sizeof(cmd), "ps -w | grep %s | grep -v grep | wc -l", name);
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buf, sizeof(buf), pp);
		runs = atoi(buf);
	}
	pclose(pp);
	if (runs > 1) {
		printf ("%s is running, exit!\n", name);
		exit (1);
		
	} else {
		return 0;
	}
}

int main(int argc, char **argv)
{
    get_running_status(argv[0]);
	init_daemon();
  //  get_process_id();
    signal(SIGALRM,sig_func);
    signal(SIGUSR1,del_timer);
    signal(SIGUSR2,add_timer);

    alarm(DEFAULT_TIMER);
    while(1) pause();
    return 0;
}
