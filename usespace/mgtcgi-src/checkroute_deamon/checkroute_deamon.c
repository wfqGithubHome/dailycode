#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../include/route_xmlwrapper4c.h"
#include "../include/xmlwrapper4c.h"


#define ROUTE_APPLYING     "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh"

unsigned int route_apply_is_running = 0;
unsigned int route_apply_ready_running = 0;
unsigned int route_apply_is_ok = 0;
const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format,args...)
#endif

void init_daemon(void);
void set_reuid();

void delay(int i)//delay function for io ports operation taking effect
{
    int j=0;
    for(j=0;j<i;j++)
    {
         usleep(1000000);                 //here, 100 ms is enough
    }
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

int check_route_apply(void)
{
    char cmd[CMD_LENGTH] = {0};
    char buffer[STRING_LENGTH] = {0};
    FILE *pp = NULL;
    int  result = -1;

    DEBUGP("%s: begin,route_apply_is_running:%d, route_apply_ready_running:%d, route_apply_is_ok:%d\n",
                __FUNCTION__, route_apply_is_running, route_apply_ready_running, route_apply_is_ok);
    
    if(route_apply_is_running == 1){
        return 0;
    }else{
        route_apply_is_running = 1;
    }

    if(route_apply_ready_running == 0 && route_apply_is_ok == 0){
        route_apply_is_running = 0;
        return 0;
    }

    route_apply_ready_running = 0;
    route_apply_is_ok = 1;
    snprintf(cmd,sizeof(cmd),"%s apply routing 2>/dev/null",ROUTE_APPLYING);
    DEBUGP("%s, cmd: %s\n", __FUNCTION__, cmd);

    set_reuid();
    pp = popen(cmd,"r");
    if(pp != NULL){
        fgets(buffer,sizeof(buffer),pp);
        buffer[strlen(buffer)-1] = 0;
        
        result = atoi(buffer);
        if(result == 0){
            route_apply_is_ok = 0;
        }
    }

    if(pp){
        pclose(pp);
    }
    route_apply_is_running = 0;

    DEBUGP("%s: finish,route_apply_is_running:%d, route_apply_ready_running:%d, route_apply_is_ok:%d\n",
                __FUNCTION__, route_apply_is_running, route_apply_ready_running, route_apply_is_ok);
    return 0;
}


int main(int argc, char **argv ) 
{ 
	unsigned int second = 20;
 //   int policy_status = -1;
 //   char policy_down_rate[STRING_LENGTH] = {0};  //单位:kbps
	char cmd[CMD_LENGTH] = {0};
    //int lock;
#if 1
	//char buffer[CMD_LENGTH] = {0};
	FILE *pp;
#endif	
	if ( argc != 2 ){
		//printf ("Uage:%s SleepTime\n",argv[0]);
		second = 20;		//默认20s检测一次路由
	}
	else{
		second = atoi(argv[1]);
		if (second > 300)
			second = 300;
		else if (second < 10)
			second=10;
	}

    printf("%s: begin\n", __FUNCTION__);
 //   get_policy_status(&policy_status, policy_down_rate);
	
	get_running_status(argv[0]);
    printf("get_running_status\n");
	init_daemon();
    printf("init_daemon\n");
#if 0
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh apply routing >/dev/null 2>&1");
	system(cmd);
    
    snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute");
    pp = popen(cmd, "r");
	if (pp != NULL){
		memset(buffer, 0 ,sizeof(buffer));
		fgets(buffer, sizeof(buffer), pp);
	}
	pclose(pp);
#endif
    
    //delay(2);
    //route_apply_dhcpd();
   // check_dhcp_dial();

	while (1)
	{
        //delay(second);
        
        //xl2tpd掉线重拨
        //check_xl2tpd_dial();
        //check_adsl_dial();
        //check_route_apply();
        //check_pptp_server_status(policy_status, policy_down_rate);
        //printf("route_apply_ready_running:%d\n",route_apply_ready_running);
        //如果路由发生变化，刷新路由表

        //lock = get_route_save_lock();
        //if(lock < 0)
        //    continue;
        printf("checkroute\n");
        snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute");
        //snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute status=1");
        
        pp = popen(cmd, "r");
        pclose(pp);
        
        delay(5);
        //put_route_save_lock(lock);
	}
}

