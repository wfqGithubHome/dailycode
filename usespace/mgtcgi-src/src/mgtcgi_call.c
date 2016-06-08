#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

void set_reuid()
{
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
}

void do_system_command(const char *cmd)
{
	set_reuid();
	system(cmd);
}

void send_drop(void)
{
    char cmd[CMD_LENGTH] = {0};
    char cmd1[CMD_LENGTH] = {0};
    int ret = 0;
    //FILE	*pp;
    
	snprintf(cmd1, sizeof(cmd1), "/usr/local/sbin/nf-hipac -I GLOBALIP0 --source 0.0.0.0/0 -d 0.0.0.0/0 -j DROP 2>/dev/null");
    snprintf(cmd,sizeof(cmd), "%s \n echo \"%s \" >> /flash/etc/check.sh", cmd1, cmd1);
    
    do_system_command(cmd);

    //pp = popen(cmd, "r");
    //if (pp == NULL)
    //{
    //    return;
    //}
    //pclose(pp);
    return ;
}

void send_ip_drop(void)
{
    char cmd[CMD_LENGTH] = {0};
    int ret = 0;
    char addr[17] = {0};
    FILE	*pp;

    cgiFormString("ip",addr,sizeof(addr));
    
	snprintf(cmd, sizeof(cmd), "/usr/local/sbin/nf-hipac -I GLOBALIP0 --source %s -d 0.0.0.0/0 -j DROP 2>/dev/nul\n",addr);
    snprintf(cmd,sizeof(cmd), "echo %s >> /flash/etc/check.sh", cmd);
    
	pp = popen(cmd, "r");
    if (pp == NULL)
    {
        return;
    }
    pclose(pp);
    
    return ;
}

void send_url_drop(void)
{

    char cmd[CMD_LENGTH] = {0};
    int ret = 0;
    char domain[CMD_LENGTH] = {0};
    FILE    *pp;
    
    cgiFormString("url",domain,sizeof(domain));
    
    snprintf(cmd, sizeof(cmd), "/usr/local/sbin/dns_helper --domainadd  --domainid 1  --domain %s \n" ,domain);
    snprintf(cmd, sizeof(cmd), "%s /usr/local/sbin/nf-hipac -A DNSHELPER0 --layer7 1  -s 0.0.0.0/0  -d 0.0.0.0/0  -j ACCEPT  --set-mark=1,1\n",cmd);
    snprintf(cmd, sizeof(cmd), "%s /usr/local/sbin/dns_helper --add --actionid 1 --bridgeid 0 --ipaddr 0.0.0.0/0 --style 2\n",cmd);
    snprintf(cmd,sizeof(cmd), "echo %s >> /flash/etc/check.sh", cmd);
    
	pp = popen(cmd, "r");
    if (pp == NULL)
    {
        return;
    }
    pclose(pp);
    return ;
}


int cgiMain()
{
	char submit_type[STRING_LENGTH];
	//char cmd[CMD_LENGTH];
	int iserror=0;
    char state[STRING_LENGTH];

	//snprintf(cmd, sizeof(cmd), BACKUP_XMLTMP_CMD);
	//system(cmd);

	cgiFormString("state", state, sizeof(state));
    //if(strcmp(state,"1") == 0)
    //    return 0;
    printf("content-type:text/html\n\n");
    
    cgiFormString("cmd", submit_type, sizeof(submit_type));
	
	if (strcmp(submit_type,"1") == 0 ){//列表显示所有
		send_drop();
	}	
	else if(strcmp(submit_type,"2") == 0 ){//点击添加
		send_ip_drop();
	}
	else if(strcmp(submit_type,"3") == 0){//点击编辑
		send_url_drop();
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
	}
    
	printf("SUCCESS");
	return 0;
}

