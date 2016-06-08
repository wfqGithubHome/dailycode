#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#define DEFAULT_TIMER               300
#define CMD_LENGTH                  256
#define STRING_LENGTH               32
#define UMOUNT_CMD                  "/bin/umount"
#define MOUNT_CMD                   "/bin/mount"

extern unsigned int route_apply_ready_running;

int get_cfdev(char *cfdev,int lenth);
void get_process_id(void);
void set_reuid(void);

extern int check_route_apply(void);

#ifndef SIGROUTEAPPLY
#define SIGROUTEAPPLY               38
#endif

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
    set_reuid();
    alarm(0);
    return;
}

void add_timer()
{
    set_reuid();
    alarm(DEFAULT_TIMER);
    return;
}

void sig_route_applying()
{
    route_apply_ready_running++;

    check_route_apply();
}

#if 0
void sig_function(int sig)
{
    
}
#endif

void init_daemon(void) 
{ 
	int pid, fd, fdtablesize; 
	int i;

	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGHUP ,SIG_IGN);
    signal(SIGALRM,sig_func);
    signal(SIGUSR1,del_timer);
    signal(SIGUSR2,add_timer);
    signal(SIGROUTEAPPLY,sig_route_applying);
#if 1
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
    
    /* start timer */
    add_timer();

	/* chang work directory */	
	chdir("/tmp");
	
	/* set umask */
	umask(0);
#endif
	return; 
} 
