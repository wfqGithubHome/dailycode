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
//#include "../include/mgtcgi_debug.h"
//const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

#define CGI_FORM_STRING		1

char * flash_xmlconf = "/flash/etc/config.xml";
char * working_xmlconf = "/etc/config.xml";

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
void log_debug(const char* fmt, ...);

int change_zeroping_status(int enable)
{
	int ret = -1;
	FILE *pp = NULL;
	char *zeroping_path = "/proc/sys/net/dpifilter/state/ipv4_state_icmp_0_rep";
	char cmd[64];
	memset(cmd,'\0',64);

	snprintf(cmd,64,"%s %d %s %s","echo",enable,">",zeroping_path);

	pp = popen(cmd,"r");
	if(pp != NULL)
	{
		ret = 0;
	}

	return ret;
}


void networkdetection_list_show(void)
{
	int iserror = 0,isize = 0;

	struct mgtcgi_xml_networkdetection *netdect = NULL;

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKDETECTION,(void**)&netdect,&isize);
	if (  netdect == NULL)
	{
       	iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[],");
	printf("\"zeroping_enble\":%d,\"iserror\":0,\"msg\":\"\"}",netdect->zeropingenable);

	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"memory error\"}",iserror);
	return;
FREE_EXIT:
	free_xml_node((void **)&netdect);
	return;
}

void networkdetection_status_save(void)
{
	int enable=1,new_size,ret_value,iserror=0;
	char enable_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_networkdetection *netdect = NULL;
	
	cgiFormString("enable",enable_str,DIGITAL_LENGTH);

	if (strlen(enable_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKDETECTION,(void**)&netdect,&new_size);
	if ( NULL == netdect){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	netdect->zeropingenable = enable;

	ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKDETECTION,(void*)netdect, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	if(change_zeroping_status(enable) != 0)
	{
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&netdect);
	return;
}

