#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

int do_sys_command(char *cmd);
int copy_xmlconf_file(const char * srcfile, const char * dstfile);

const char *log_file_path = "/tmp/myclient/third_client.txt";

void get_series_id(char *series)
{
    char tmp[STRING_LENGTH] = {0};
    const char *file_path = "/etc/file_id";
    FILE *ff = NULL;

    if(!series){
        return;
    }

    ff = fopen(file_path,"r");
    if(!ff){
        return;
    }

    fread(tmp,1,sizeof(tmp),ff);
    fclose(ff);
    tmp[strlen(tmp)-1] = 0;

    strcpy(series,tmp);
    return;
}

void get_process_status(int *status)
{
    int num = 0;
    char buf[STRING_LENGTH] = {0};
    FILE *pp = NULL;
    const char *cmd = "ps |grep \"myclient\" |grep -v grep |wc -l";
    
    if(!status){
        return;
    }

    pp = popen(cmd,"r");
    if(!pp){
        return;
    }
    fgets(buf,sizeof(buf),pp);
    pclose(pp);

    num = atoi(buf);
    if(num > 0){
        *status = 1;
    }
    else{
        *status = 0;
    }

    return;
}

void routeserver_list_show(void)
{
	int iserror=0,size;
    int service_switch = 1, status = 0;
    char file_name[STRING_LENGTH] = {0};
    char up_time[STRING_LENGTH] = {0};
    char series[STRING_LENGTH] = {0};
    char username[STRING_LENGTH] = {0};
    const char *default_usname = "rsystem";
	struct mgtcgi_xml_routeserver *mgt = NULL;
    struct stat buf;
    struct tm *timer = NULL;

    strncpy(username,getenv("REMOTE_USER"),STRING_LENGTH-1);
    if((strcmp(username,default_usname) != 0) && (strcmp(username,"root") != 0)){
        iserror = 2;
        goto ERROR_EXIT;
    }
    
	get_xml_node(MGTCGI_XMLTMP, 
        MGTCGI_TYPE_ROUTESERVICE,(void**)&mgt,&size);
    if(NULL == mgt){
        goto ERROR_EXIT;
    }
    service_switch = mgt->enable;

    if(mgt->passwd_enable == 0){
        iserror = 2;
        goto ERROR_EXIT;
    }

    get_series_id(series);
    get_process_status(&status);

    memset(&buf,0,sizeof(struct stat));
    if(access("/etc/routeserver.tar.bz2",0) == 0){
        strcpy(file_name,"routeserver.tar.bz2");

        if(stat("/etc/routeserver.tar.bz2",&buf) != 0){
            goto ERROR_EXIT;
        }

        timer = gmtime(&(buf.st_mtime));
        snprintf(up_time,sizeof(up_time),"%d,%d,%d,%d,%d,%d",
                timer->tm_year+1900,
                timer->tm_mon+1,
                timer->tm_mday,
                timer->tm_hour,
                timer->tm_min,
                timer->tm_sec);
        iserror = 0;
    }
    
	printf("{\"iserror\":%d,\"swith\":%d,\"series\":\"%s\",\"file_name\":\"%s\",\"running_status\":%d,\"up_time\":\"%s\"}",
            iserror,service_switch,series,file_name,status,up_time);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;

FREE_EXIT:
	free_xml_node((void*)&mgt);
	return;
}

void routeserver_series_update(void)
{
    int iserror = 0,enable,size,result;
    char series[STRING_LENGTH] = {0};
    char enable_str[STRING_LENGTH] = {0};
    char cmd[CMD_LENGTH] = {0};
    struct mgtcgi_xml_routeserver *mgt = NULL;
    const char *flash_xmlconf = "/flash/etc/config.xml";
    const char *script = "/usr/local/httpd/htdocs/cgi-bin/sh/routeserver_script.sh";
    
    cgiFormString("series", series, sizeof(series));
    cgiFormString("enable", enable_str, sizeof(enable_str));

    if(strlen(series) < 1){
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

    enable = atoi(enable_str);
    get_xml_node(MGTCGI_XMLTMP, 
        MGTCGI_TYPE_ROUTESERVICE,(void**)&mgt,&size);
    if(NULL == mgt){
        goto ERROR_EXIT;
    }
    mgt->enable = enable?1:0;

    result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_ROUTESERVICE,(void*)mgt, size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
    result = copy_xmlconf_file(MGTCGI_XMLTMP,flash_xmlconf);

    snprintf(cmd,sizeof(cmd),"%s series_store %s",script,series);
    result = do_sys_command(cmd);

    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;

ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);

FREE_EXIT:
    return;
}

void routeserver_proc_restart(void)
{ 
    char *proc_restart = "/usr/local/httpd/htdocs/cgi-bin/sh/routeserver_script.sh restart";
    do_sys_command(proc_restart);

    printf("{\"iserror\":0,\"msg\":\"\"}");
    return;
}

void routeserver_proc_stop(void)
{
    char *proc_stop = "/usr/sbin/third_stop.sh";
    do_sys_command(proc_stop);

    printf("{\"iserror\":0,\"msg\":\"\"}");
    return;
}

void routeserver_proc_printlog(void)
{
    size_t size = 0;
    char *buf = NULL;
    FILE *fp = NULL;

    buf = (char *)malloc(BUF_LENGTH_4K);
    if(NULL == buf){
        goto ERROR_EXIT;
    }

    fp = fopen(log_file_path,"r");
    if(NULL == fp){
        goto ERROR_EXIT;
    }

    do{
        size = fread(buf,1,BUF_LENGTH_4K-1,fp);
        if(size > 0){
            printf("%s",buf);
            memset(buf,0,BUF_LENGTH_4K);
        }
        else{
            break;
        }
    }while(1);

    goto EXIT;
ERROR_EXIT:
    printf("%s",strerror(errno));
    goto EXIT;
EXIT:
    if(buf){
        free(buf);
        buf = NULL;
    }
    if(fp){
        fclose(fp);
        fp = NULL;
    }
    return;
}

void routeserver_proc_clearlog(void)
{
    char cmd[CMD_LENGTH] = {0};

    snprintf(cmd,sizeof(cmd),"cat /dev/null > %s 2>/dev/null",log_file_path);
    do_sys_command(cmd);

    return;
}

void routeserver_edit_save(void)
{
#if 0
	char		conntype[GET_STRING_LENG] = {"a"};
	char		newconn[GET_STRING_LENG] = {"1"};
	char		doubtconn[GET_STRING_LENG] = {"1"};
	char		loginterval[GET_STRING_LENG] = {"1"};

    int newconn_val = 0, doubtconn_val = 0, loginterval_val = 0;
	
	char 		cmd[CMD_LENGTH];

	struct mgtcgi_xml_portscan *mgt = NULL;
	int result, size=0, iserror=0;
    int i;

	cgiFormString("conntype", conntype, STRING_LENGTH);
	cgiFormString("newconn", newconn, STRING_LENGTH);
	cgiFormString("doubtconn", doubtconn, STRING_LENGTH);
    cgiFormString("loginterval", loginterval, STRING_LENGTH);

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORTSCAN,(void**)&mgt,&size);
	if ( mgt == NULL){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    for (i = 0; i < mgt->num; i++)
    {
        if (strcmp(mgt->pinfo[i].type, conntype) != 0)
        {
            continue;
        }
        
    	strcpy(mgt->pinfo[i].newer, newconn);
    	strcpy(mgt->pinfo[i].pending, doubtconn);
    	strcpy(mgt->pinfo[i].interval, loginterval);
    }
    
	//Ð´ÎÄ¼þ
	save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORTSCAN,(void*)mgt,size);

	result = -1;
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh xmlconf_update");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&mgt);
    #endif
    printf("{\"iserror\":0,\"msg\":\"\"}");
	return;
}


int cgiMain()
{  
	char submit_type[STRING_LENGTH];
	int iserror=0;

	cgiFormString("submit", submit_type, sizeof(submit_type));

#if 0 
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:text/plain\n\n");	
#endif
#endif

    if(strcmp(submit_type,"printlog") == 0){
        printf("content-type:text/plain\n\n");
    }
    else{
        printf("content-type:application/json\n\n");
    }
    
	if (strcmp(submit_type,"list") == 0 ){
		routeserver_list_show();
	}	
    else if(strcmp(submit_type,"update") == 0 ){
        routeserver_series_update();
    }
    else if(strcmp(submit_type,"restart") == 0){
        routeserver_proc_restart();
    }
    else if(strcmp(submit_type,"stop") == 0){
        routeserver_proc_stop();
    }
    else if(strcmp(submit_type,"printlog") == 0){
        routeserver_proc_printlog();
    }
    else if(strcmp(submit_type,"clearlog") == 0){
        routeserver_proc_clearlog();
    }
	else if(strcmp(submit_type,"editsave") == 0 ){
		routeserver_edit_save();
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	goto CGIC_EXIT;

ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto CGIC_EXIT;
CGIC_EXIT:
	return 0;
}

