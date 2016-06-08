#include <stdio.h>
#include <stdlib.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

#define	GET_STRING_LENG	20
#define	CMD_LENG		256


/*
	Make
	gcc  -o management.cgi mgtcgi_management.c mgtcgi_filed_check.c -lcgic  -lxmlwrapper4c
*/



int cgiMain()
{
	char mac_string[GET_STRING_LENG];
	int iserror=0;
    char cmd[CMD_LENGTH];
    FILE    *pp;
    char    mac_num[GET_STRING_LENG];
    
	cgiFormString("mac", mac_string, sizeof(mac_string));
	
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif
    printf("{\"auth\":");

    snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-auth --getid --mac %s ",mac_string);
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(mac_num, GET_STRING_LENG, pp);
	}
	pclose(pp);	
    
	printf("%s}",mac_num);
	return 0;
}

