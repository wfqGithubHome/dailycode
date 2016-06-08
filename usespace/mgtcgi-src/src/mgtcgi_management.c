#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

#define	GET_STRING_LENG	20
#define	CMD_LENG		1024

extern int check_ipaddr(const char *ipaddr);

void management_list_show(void)
{
	int size=0,iserror=0;
	struct mgtcgi_xml_management *mgt = NULL;

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_MANAGEMENT,(void**)&mgt,&size);
	if ( NULL == mgt){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf( "{\"data\":[{\"ip_addr\":\"%s\",\"netmask\":\"%s\",\
\"gateway\":\"%s\",\"dns_primary\":\"%s\",\"dns_secondary\":\"%s\"}],",\
	mgt->ip,mgt->netmask,mgt->gateway,mgt->dns_primary,mgt->dns_secondary);

	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"memory error\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&mgt);
	return;
}

void management_edit_save(void)
{
	char		ip_addr[GET_STRING_LENG]={"192.168.0.254"};
	char		netmask[GET_STRING_LENG]={"255.255.255.0"};
	char		broadcast[GET_STRING_LENG];
	char		gateway[GET_STRING_LENG]={"192.168.0.1"};
	char		dns_primary[GET_STRING_LENG]={"202.98.96.68"};
	char		dns_secondary[GET_STRING_LENG]={"61.139.2.69"};
	char		result_str[GET_STRING_LENG];
	char 		cmd[CMD_LENGTH];
	FILE 		*pp = NULL;

	int result,size=0,iserror=0;
	struct mgtcgi_xml_management *mgt = NULL;

	cgiFormString("ip_addr",ip_addr,STRING_LENGTH);
	cgiFormString("netmask",netmask,STRING_LENGTH);
	cgiFormString("gateway",gateway,STRING_LENGTH);
	cgiFormString("dns_primary",dns_primary,STRING_LENGTH);
	cgiFormString("dns_secondary",dns_secondary,STRING_LENGTH);

	if (check_ipaddr(ip_addr) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_ipaddr(netmask) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_ipaddr(gateway) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_ipaddr(dns_primary) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_ipaddr(dns_secondary) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	snprintf(cmd, sizeof(cmd),"/bin/ipcalc --broadcast  %s %s | /usr/bin/awk -F \'=\' \'{print $2}\'",ip_addr,netmask);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(broadcast, GET_STRING_LENG, pp);
		broadcast[strlen(broadcast)-1] = 0;
		if (check_ipaddr(broadcast) != 0){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		strcpy(broadcast, mgt->broadcast);
	}
	pclose(pp);
	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_MANAGEMENT,(void**)&mgt,&size);
	if ( NULL == mgt){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	strcpy(mgt->ip,ip_addr);
	strcpy(mgt->netmask,netmask);
	strcpy(mgt->broadcast,broadcast);
	strcpy(mgt->gateway,gateway);
	strcpy(mgt->dns_primary,dns_primary);
	strcpy(mgt->dns_secondary,dns_secondary);

	//Ð´ÎÄ¼þ
	save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_MANAGEMENT,(void*)mgt,size);

	result = -1;
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh xmlconf_update");
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(result_str, sizeof(result_str), pp);
		result = atoi(result_str);
	}
	pclose(pp);	
	if (result == 0){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd),". /tmp/config.sh;\
/bin/ip addr flush dev ${DEVICE_MANAGE_PORT} >/dev/null 2>&1;\
/bin/ip addr add %s/`/bin/ipcalc -p %s %s| /usr/bin/awk -F \'=\' \'{print $2}\'` broadcast %s dev ${DEVICE_MANAGE_PORT} >/dev/null 2>&1;\
/bin/ip link set ${DEVICE_MANAGE_PORT} up >/dev/null 2>&1;\
/bin/ip route add 0/0 via %s dev ${DEVICE_MANAGE_PORT}>/dev/null 2>&1;\
/etc/init.d/loadconfig restart >/dev/null 2>&1;\
/etc/init.d/syslog restart >/dev/null 2>&1;\
", ip_addr, ip_addr, netmask, broadcast, gateway);
		system(cmd);
		
		iserror = 0;
	} else {
		iserror = 4; 
		goto ERROR_EXIT;
	}
	
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
	char cmd[CMD_LENGTH];
	int iserror=0;

	snprintf(cmd, sizeof(cmd), BACKUP_XMLTMP_CMD);
	system(cmd);

	cgiFormString("submit", submit_type, sizeof(submit_type));
	
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	if (strcmp(submit_type,"list") == 0 ){
		management_list_show();
	}	
	else if(strcmp(submit_type,"editsave") == 0 ){
		management_edit_save();
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

