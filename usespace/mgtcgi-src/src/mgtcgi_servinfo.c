#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

#if 0
#define log_servinfo log_debug
#else
#define log_servinfo(format, args...)
#endif

void server_info_show(void)
{
    int iserror = 0;
    int flag = 0;
    int index;
    int size = 0;
	struct mgtcgi_xml_servinfo *servinfo = NULL;
    
	get_xml_nodeforservinfo(MGTCGI_XMLTMP,SERV_XMLFILE,(void**)&servinfo,&size);
    log_servinfo("%s: servinfo = %p\n", __FUNCTION__, servinfo);
    
	if ( NULL == servinfo)
    {
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");

	for (index=0; index < servinfo->num; index++){
        log_servinfo("%s: type = %u, name = %s, en_name = %s, zonename = %s, en_zonename = %s, comment = %s, selected = %d\n", 
            __FUNCTION__, 
            servinfo->pinfo[index].type,
            servinfo->pinfo[index].name,
            servinfo->pinfo[index].en_name,
            servinfo->pinfo[index].zonename,
            servinfo->pinfo[index].en_zonename,
            servinfo->pinfo[index].comment,
            servinfo->pinfo[index].selected);
            
		if (flag == 0)
        {
			printf("{\"type\":\"%u\",\"name\":\"%s\",\"en_name\":\"%s\",\"zonename\":\"%s\",\"en_zonename\":\"%s\",\"comment\":\"%s\",\"selected\":\"%d\"}",\
			servinfo->pinfo[index].type,
            servinfo->pinfo[index].name,
            servinfo->pinfo[index].en_name,
            servinfo->pinfo[index].zonename,
            servinfo->pinfo[index].en_zonename,
            servinfo->pinfo[index].comment,
            servinfo->pinfo[index].selected);
			flag = 1;
		}else{
			printf(",{\"type\":\"%u\",\"name\":\"%s\",\"en_name\":\"%s\",\"zonename\":\"%s\",\"en_zonename\":\"%s\",\"comment\":\"%s\",\"selected\":\"%d\"}",\
			servinfo->pinfo[index].type,
            servinfo->pinfo[index].name,
            servinfo->pinfo[index].en_name,
            servinfo->pinfo[index].zonename,
            servinfo->pinfo[index].en_zonename,
            servinfo->pinfo[index].comment,
            servinfo->pinfo[index].selected);
		}

	}
    
	printf("],");

	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void**)&servinfo);
	return;	
}

extern inline void *malloc_buf(int size);
extern inline void free_malloc_buf(void **buf);

void server_info_edit_save()
{
    int old_size,new_size,iserror=0,result=0;
    int type;
	char type_str[STRING_LENGTH]={"0"};
	char en_name[STRING_LENGTH]={"0"};
	char en_zonename[STRING_LENGTH]={"0"};
    char cmd_str[128] = {"0"};
    int update = 0;
    int i;

	struct mgtcgi_xml_servconf *old_nodes = NULL;
	struct mgtcgi_xml_servconf *new_nodes = NULL;

	cgiFormString("type",type_str,STRING_LENGTH);
	cgiFormString("en_name",en_name,STRING_LENGTH);
	cgiFormString("en_zonename",en_zonename,STRING_LENGTH);
    type = atoi(type_str);

    log_servinfo("%s: type = %u, en_name = %s, en_zonename = %s\n", 
        __FUNCTION__, type, en_name, en_zonename);
			
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SERVER_CONF,(void**)&old_nodes,&old_size);
	if (old_nodes == NULL)
    {
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    for (i = 0; i < old_nodes->num; i++)
    {
        if (old_nodes->pinfo[i].type != type)
        {
            continue;
        }

        old_nodes->pinfo[i].type = type;
	    strcpy(old_nodes->pinfo[i].en_name, en_name);
	    strcpy(old_nodes->pinfo[i].en_zonename, en_zonename);
        update = 1;
    }

    if (update == 1)
    {
        new_nodes = old_nodes;
        new_size = old_size;
        goto SAVE;
    }

	new_size = sizeof(struct mgtcgi_xml_servconf) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_servconf_info);

	new_nodes = (struct mgtcgi_xml_servconf *)malloc_buf(new_size);
	if (new_nodes == NULL)
    {
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = old_nodes->num + 1;
    new_nodes->pinfo[old_nodes->num].type = type;
	strcpy(new_nodes->pinfo[old_nodes->num].en_name, en_name);
	strcpy(new_nodes->pinfo[old_nodes->num].en_zonename, en_zonename);

SAVE:
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SERVER_CONF,(void*)new_nodes, new_size);
	if (result < 0)
    {
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    snprintf(cmd_str, 127, "%s", "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh sync_remote_server_info");
    system(cmd_str);
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
FREE_EXIT:
    if (old_nodes)
    {
	    free_xml_node((void*)&old_nodes);
    }

    if (new_nodes && new_nodes != old_nodes)
    {
		free_xml_node((void*)&new_nodes);
    }

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

	if (strcmp(submit_type,"list") == 0 )
    {
		server_info_show();
	}	
    else if (strcmp(submit_type, "editsave") == 0)
    {
        server_info_edit_save();
    }
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	goto CGIC_EXIT;

	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}", iserror);
		goto CGIC_EXIT;
	CGIC_EXIT:
		return 0;
}

