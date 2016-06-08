#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

char *crond_file_path = "/etc/crontabs/root";

int do_sys_command(char *cmd)
{
	int value=1;
	char buffer[DIGITAL_LENGTH]={0};
	FILE *pp = NULL;
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
	
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
			value = 0;
		else
			value = -1;
	}
	pclose(pp);
	return value;
}

int main(void)
{
    int size,index,i,j;
    int iserror = 0;
    struct mgtcgi_xml_adsl_timings * timing_adsls = NULL;

    char cmd[CMD_LENGTH] = {0};
    char *pppoe_restart = "/usr/local/sbin/pppoe-restart";

    char iface_str[STRING_LENGTH] = {0};
    char minute_str[STRING_LENGTH]={0};
    char hour_str[STRING_LENGTH]={0};
    char day_str[STRING_LENGTH]={0};
    char month_str[STRING_LENGTH]={0};
    char week_str[STRING_LENGTH]={0};
    char crond_str[CMD_LENGTH] = {0};
    char tmp[STRING_LENGTH] = {0};

    get_xml_node(MGTCGI_XMLTMP,MGTCGI_SYS_TIMING_ADSL_REDIAL,(void**)&timing_adsls,&size);
    if ( NULL == timing_adsls){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    for (index =0; index < timing_adsls->num; index++){
        strcpy(iface_str,timing_adsls->pinfo[index].iface);
        strcpy(minute_str,timing_adsls->pinfo[index].minute);
        strcpy(hour_str,timing_adsls->pinfo[index].hour);
        strcpy(day_str,timing_adsls->pinfo[index].day);
        strcpy(month_str,timing_adsls->pinfo[index].month);
        strcpy(week_str,timing_adsls->pinfo[index].week);
        
        if(strlen(minute_str) == 0){
            minute_str[0] = '*';
        }

        if(strlen(hour_str) == 0){
            hour_str[0] = '*';
        }

        if(strlen(day_str) == 0){
            day_str[0] = '*';
        }

        if(strlen(month_str) == 0){
            month_str[0] = '*';
        }

        if(strlen(week_str) == 0){
            week_str[0] = '*';
        }

        for(i = 0; i <= strlen(iface_str); i++){
            if((iface_str[i] == ',') || (i == strlen(iface_str))){
                snprintf(crond_str,sizeof(crond_str),"%s %s %s %s %s %s /etc/ppp/%s.conf",
                                            minute_str,hour_str,day_str,month_str,week_str,pppoe_restart,tmp);
                snprintf(cmd, sizeof(cmd), "echo \"%s\" >> %s", crond_str, crond_file_path);
                do_sys_command(cmd);

                memset(cmd,0,sizeof(cmd));
                memset(crond_str,0,sizeof(crond_str));
                j = 0;
            }else{
                tmp[j] = iface_str[i];
                j++;
            }
        }

        memset(iface_str,0,sizeof(iface_str));
        memset(minute_str,0,sizeof(minute_str));
        memset(hour_str,0,sizeof(hour_str));
        memset(day_str,0,sizeof(day_str));
        memset(month_str,0,sizeof(month_str));
        memset(week_str,0,sizeof(week_str));
    }

ERROR_EXIT:
    free_xml_node((void*)&timing_adsls);
    return 0;
}

