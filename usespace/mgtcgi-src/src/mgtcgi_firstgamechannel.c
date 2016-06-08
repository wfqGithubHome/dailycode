#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

void get_game_count(char *name,int *count)
{
    int num = 0;
    char *p = name;
    
    if(!name || !count)
        return;

    while(*p){
        if(*p == ',')
            num++;
        p++;
    }

    *count=num+1;
    return;
}

int do_fgc_kernel_command(char *ids)
{
    char *p = NULL;
    char *point = NULL;
    char *item = ",";
    char cmd[256] = {0};
    char s[1024] = {0};
    char *add_lay7id_cmd = "/usr/local/sbin/ip-detect --fgc_set -p %s >/dev/null 2>&1";
    char *flush_command = "/usr/local/sbin/ip-detect --fgc_flush >/dev/null 2>&1";


    system(flush_command);
    if(ids == NULL){
        return 0;
    }

    strncpy(s,ids,1024);

    p = s;
    while(p){
        while((point = strsep(&p,item))){
            snprintf(cmd,sizeof(cmd),add_lay7id_cmd,point);
            system(cmd);
        }
    }

    return 0;
}

void first_game_channel_list(void)
{
    int         size,i,j,iserror = 0,t = 0;
    struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
    struct mgtcgi_xml_first_game_channels   *group = NULL;
    
    get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
        "chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIRST_GAME_CHANNEL,(void**)&group,&size);
    if(NULL == group){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    for(i = 0; i < l7protocol->num; i++){     
        if(l7protocol->pinfo[i].selected == 0){
            continue;
        }
        else{
            l7protocol->pinfo[i].selected = 0;
        }
        
        for(j = 0; j < group->num; j++){
            if(strcmp(l7protocol->pinfo[i].name,group->pinfo[j].name) == 0){
                l7protocol->pinfo[i].selected = 1;
            }
        }
    }

    printf("{\"data\":[");
    for(i = 0; i < l7protocol->num; i++){
        if(strcmp(l7protocol->pinfo[i].type,"Game") != 0)
            continue;
        
        if(t == 1)
            printf(",");
        
        printf("{\"id\":%d,\"name\":\"%s\",\"name2\":\"%s\",\"type\":\"%s\",\"type_name\":\"%s\",\"selected\":%d,\"comment\":\"%s\"}",\
            l7protocol->pinfo[i].id,
			l7protocol->pinfo[i].name,
			l7protocol->pinfo[i].match,
			l7protocol->pinfo[i].type,
			l7protocol->pinfo[i].type_name,
			l7protocol->pinfo[i].selected,
			l7protocol->pinfo[i].comment);
        t = 1;
    }
    printf("],\"iserror\":%d,\"errormsg\":\"\"}",iserror);
    
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror:\":%d,\"errormsg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void**)&l7protocol);
    free_xml_node((void**)&group);
    return;
}

void first_game_channel_editsave(void)
{
    int                                     id,iserror = -1,num = 0,count = 0,index = 0,calsize,result;
    char                                    id_str[1024] = {0};
    char                                    id_str_tmp[1024] = {0};
    char                                    *channel_name = NULL;
    char                                    *id_p = NULL,*name_p = NULL;
    char                                    *id_p_t = NULL,*name_p_t = NULL;
    char                                    *item = ",";
    struct mgtcgi_xml_first_game_channels   *group = NULL;

    channel_name = (char *)malloc(1024 * 4);
    if(NULL == channel_name){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto EXIT;
    }

    cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("name", channel_name, 1024 * 4);

    strncpy(id_str_tmp,id_str,1024);
    get_game_count(channel_name,&num);
    if(num == 0){
        iserror = 0;
        goto EXIT;
    }

    calsize = sizeof(struct mgtcgi_xml_first_game_channels) + 
            num * sizeof(struct mgtcgi_xml_first_game_channel_info);

    group = (struct mgtcgi_xml_first_game_channels *)malloc(calsize);
    if(NULL == group){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto EXIT;
    }

    id_p = id_str;
    name_p = channel_name;

    while(id_p && name_p){
        while((id_p_t = strsep(&id_p,item)) && (name_p_t = strsep(&name_p,item))){
            id = atoi(id_p_t);

            group->pinfo[index].id = id;
            strncpy(group->pinfo[index].name, name_p_t, STRING_LENGTH - 1);
            index++;
            count++;
        }
    }

    group->num = count;

    result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIRST_GAME_CHANNEL,(void *)group, calsize);
    if(result < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto EXIT;
    }

    do_fgc_kernel_command(id_str_tmp);

    iserror = 0;
EXIT:
    printf("{\"iserror\":%d,\"num\":%d,\"errormsg\":\"\"}",iserror,num);
    goto FREE_EXIT;
FREE_EXIT:
    if(channel_name){
        free(channel_name);
        channel_name = NULL;
    }
    free_xml_node((void**)&group);
    return;
}

int cgiMain(void)
{
    char        submit_type[STRING_LENGTH] = {0};
    int         iserror = 0;

    cgiFormString("submit", submit_type, sizeof(submit_type));

#if _MGTCGI_DEBUG_GET_
        printf("content-type:text/html\n\n");       
#else
        printf("content-type:application/json\n\n");    
#endif


    if(strcmp(submit_type, "list") == 0){
        first_game_channel_list();
    }
    else if(strcmp(submit_type, "editsave") == 0){
        first_game_channel_editsave();
    }
    else{
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    goto CGI_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"errormsg\":\"\"}",iserror);
    goto CGI_EXIT;
CGI_EXIT:
    return 0;
}

