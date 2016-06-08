#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define	GET_STRING_LENG	20
#define	CMD_LENG		1024

void portscan_list_show(void)
{
	int size=0,iserror=0,index=0;
    int index_end = 3;
	struct mgtcgi_xml_portscan *mgt = NULL;

 

	get_xml_node(MGTCGI_XMLTMP, 
        MGTCGI_TYPE_PORTSCAN,(void**)&mgt,&size);
    
    
	if (  mgt == NULL){
        iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    printf("{\"data\":[");
	for (; index < index_end ;){
		printf("{\"conntype\":\"%s\",\"newconn\":\"%s\",\"doubtconn\":\"%s\",\"loginterval\":\"%s\"}",\
			mgt->pinfo[index].type,mgt->pinfo[index].newer,mgt->pinfo[index].pending,mgt->pinfo[index].interval);

		index++;
		if (index < index_end)
			printf(",");
		else
			break;
	}
	printf("],");

	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"memory error\"}",iserror);

FREE_EXIT:

	free_xml_node((void*)&mgt);
	return;
}

void portscan_edit_save(void)
{
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

    /******************************参数检查*****************************/
	if(strcmp(conntype, "tcp") != 0 &&
         strcmp(conntype, "udp") != 0 &&
         strcmp(conntype, "icmp") != 0)
    {
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	newconn_val = atoi(newconn);
    doubtconn_val = atoi(doubtconn);
    loginterval_val = atoi(loginterval);
    
	if (newconn_val < 1000 ||
        doubtconn_val < 1000)
    {
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if (strcmp(conntype, "tcp") == 0 && loginterval_val < 600)
    {
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

    if (strcmp(conntype, "udp") == 0 && loginterval_val < 300)
    {
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

    if (strcmp(conntype, "icmp") == 0 && loginterval_val < 300)
    {
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

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
    
	//写文件
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
	return;
}


int cgiMain()
{  
	char submit_type[STRING_LENGTH];
	int iserror=0;

	cgiFormString("submit", submit_type, sizeof(submit_type));
	
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	if (strcmp(submit_type,"list") == 0 ){
		portscan_list_show();
	}	
	else if(strcmp(submit_type,"editsave") == 0 ){
		portscan_edit_save();
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

