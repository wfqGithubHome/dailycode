#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int apply_timing_adslredial(const char *iface, const char *minute, const char *hour,
                                const char *day, const char *month, const char *week);
extern int remove_timing_adsl(const char *iface, const char *minute, const char *hour,
                                const char *day, const char *month, const char *week);
extern int copy_xmlconf_file(const char * srcfile, const char * dstfile);

char * flash_xmlconf = "/flash/etc/config.xml";
char * working_xmlconf = "/etc/config.xml";

void adsl_timing_redial_list_show(void)
{
	int size=0,index=0;
	int iserror=0,result=0;
	struct mgtcgi_xml_adsl_timings * timing_adsls = NULL;
    
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_SYS_TIMING_ADSL_REDIAL,(void**)&timing_adsls,&size);
	if ( NULL == timing_adsls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	printf("{\"data\":[");

       for(;index < timing_adsls->num; index++){
           if(result){
               printf(",");
           }else{
               result = 1;
           }

           printf("{\"name\":\"%s\",\"interface\":\"%s\",\"minute\":\"%s\",\"hour\":\"%s\",\"day\":\"%s\",\"month\":\"%s\",\"week\":\"%s\",\"comment\":\"%s\"}",
                        timing_adsls->pinfo[index].name,timing_adsls->pinfo[index].iface,
                        timing_adsls->pinfo[index].minute, timing_adsls->pinfo[index].hour, timing_adsls->pinfo[index].day,
                        timing_adsls->pinfo[index].month, timing_adsls->pinfo[index].week, timing_adsls->pinfo[index].comment);
       }
	
	printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",timing_adsls->num);
	goto FREE_EXIT;
        
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&timing_adsls);
		return;
}

void adsl_timing_redial_add_save(void)
{
    int new_size,old_size, index;
    int iserror = 0, result = 0;
    char name[STRING_LENGTH]={0};
    char iface[STRING_LENGTH]={0};
    char minute[STRING_LENGTH]={0};
    char hour[STRING_LENGTH]={0};
    char day[STRING_LENGTH]={0};
    char month[STRING_LENGTH]={0};
    char week[STRING_LENGTH]={0};
    char comment[STRING_LENGTH]={0};

    struct mgtcgi_xml_adsl_timings * old_timing_adsl = NULL;
    struct mgtcgi_xml_adsl_timings * new_timing_adsl = NULL;

    cgiFormString("name",name,STRING_LENGTH);
    cgiFormString("iface",iface,STRING_LENGTH);
    cgiFormString("minute",minute,STRING_LENGTH);
    cgiFormString("hour",hour,STRING_LENGTH);
    cgiFormString("day",day,STRING_LENGTH);
    cgiFormString("month",month,STRING_LENGTH);
    cgiFormString("week",week,STRING_LENGTH);
    cgiFormString("comment",comment,STRING_LENGTH);

    get_xml_node(MGTCGI_XMLTMP,MGTCGI_SYS_TIMING_ADSL_REDIAL,(void**)&old_timing_adsl,&old_size);
    if ( NULL == old_timing_adsl){
	    iserror = MGTCGI_READ_FILE_ERR;
	    goto ERROR_EXIT;
    }

    /********************参数检查**********************/
    for (index = 0; index < old_timing_adsl->num; index++){
        if (strcmp(name, old_timing_adsl->pinfo[index].name) == 0){
            iserror = MGTCGI_DUPLICATE_ERR;
	     goto ERROR_EXIT;
        }
    }

    old_size = sizeof(struct mgtcgi_xml_adsl_timings) + 
				old_timing_adsl->num *
				sizeof(struct mgtcgi_xml_adsl_timing_redial);
   new_size = sizeof(struct mgtcgi_xml_adsl_timings) + 
				(old_timing_adsl->num + 1) *
				sizeof(struct mgtcgi_xml_adsl_timing_redial);

    new_timing_adsl = (struct mgtcgi_xml_adsl_timings *)malloc_buf(new_size);
    if (NULL == new_timing_adsl){
	    iserror = MGTCGI_READ_FILE_ERR;
	    goto ERROR_EXIT;
    }
    memset(new_timing_adsl, 0, new_size);
    memcpy(new_timing_adsl, old_timing_adsl, old_size);
    
    new_timing_adsl->num = old_timing_adsl->num + 1;
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].name, name);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].iface, iface);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].minute, minute);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].hour, hour);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].day, day);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].month, month);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].week, week);
    strcpy(new_timing_adsl->pinfo[old_timing_adsl->num].comment, comment);

    result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_SYS_TIMING_ADSL_REDIAL,(void*)new_timing_adsl, new_size);
    if (result < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	 goto ERROR_EXIT;
    }else{
        result = apply_timing_adslredial(iface,minute,hour,day,month,week);
        if( result != 0){
            iserror = MGTCGI_APPY_ERR;
            goto ERROR_EXIT;
        }
    }

    result = copy_xmlconf_file(working_xmlconf,flash_xmlconf);
    if (iserror != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }
  
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&old_timing_adsl);
    free_xml_node((void*)&new_timing_adsl);
    return;
}

void adsl_timing_redial_remove_save(void)
{
    int new_size,old_size,old_num,new_num,index,new_index;
    int iserror = 0, result = 0, sign = 0;
    char name[STRING_LENGTH]={0};
    struct mgtcgi_xml_adsl_timings * old_timing_adsl = NULL;
    struct mgtcgi_xml_adsl_timings * new_timing_adsl = NULL;

    cgiFormString("name",name,STRING_LENGTH);

    get_xml_node(MGTCGI_XMLTMP,MGTCGI_SYS_TIMING_ADSL_REDIAL,(void**)&old_timing_adsl,&old_size);
    if ( NULL == old_timing_adsl){
	    iserror = MGTCGI_READ_FILE_ERR;
	    goto ERROR_EXIT;
    }

    old_num = old_timing_adsl->num;
    new_num = old_num - 1;

    new_size = sizeof(struct mgtcgi_xml_adsl_timings) + 
				new_num *
				sizeof(struct mgtcgi_xml_adsl_timing_redial);
    new_timing_adsl = (struct mgtcgi_xml_adsl_timings *)malloc_buf(new_size);
    if (NULL == new_timing_adsl){
	    iserror = MGTCGI_READ_FILE_ERR;
	    goto ERROR_EXIT;
    }

    new_timing_adsl->num = new_num;
    for(index = 0,new_index = 0; index < old_timing_adsl->num; index++){
        if(strcmp(name, old_timing_adsl->pinfo[index].name) != 0){
            strcpy(new_timing_adsl->pinfo[new_index].name,old_timing_adsl->pinfo[index].name);
            strcpy(new_timing_adsl->pinfo[new_index].iface,old_timing_adsl->pinfo[index].iface);
            strcpy(new_timing_adsl->pinfo[new_index].minute,old_timing_adsl->pinfo[index].minute);
            strcpy(new_timing_adsl->pinfo[new_index].hour,old_timing_adsl->pinfo[index].hour);
            strcpy(new_timing_adsl->pinfo[new_index].day,old_timing_adsl->pinfo[index].day);
            strcpy(new_timing_adsl->pinfo[new_index].month,old_timing_adsl->pinfo[index].month);
            strcpy(new_timing_adsl->pinfo[new_index].week,old_timing_adsl->pinfo[index].week);
            strcpy(new_timing_adsl->pinfo[new_index].comment,old_timing_adsl->pinfo[index].comment);
            new_index++;
        }else{
            sign = index;
        }
    }

    result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_SYS_TIMING_ADSL_REDIAL,(void*)new_timing_adsl, new_size);
    if (result  < 0){
	 iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }else{
        result = remove_timing_adsl(old_timing_adsl->pinfo[sign].iface,
                                        old_timing_adsl->pinfo[sign].minute,
                                        old_timing_adsl->pinfo[sign].hour,
                                        old_timing_adsl->pinfo[sign].day,
                                        old_timing_adsl->pinfo[sign].month,
                                        old_timing_adsl->pinfo[sign].week);
        if( result != 0){
            iserror = MGTCGI_APPY_ERR;
            goto ERROR_EXIT;
        }
    }

    result = copy_xmlconf_file(working_xmlconf,flash_xmlconf);
    if (iserror != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }
    
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&old_timing_adsl);
    free_xml_node((void*)&new_timing_adsl);
    return;
}

void timing_list_show(int id)
{
    switch(id)
    {
        case MGTCGI_SYS_TIMING_ADSL_REDIAL:   //ADSL定时信息
            adsl_timing_redial_list_show();
            break;
        default:
            goto PARAM_ERROR;
	     break;
    }
    goto EXIT;

    PARAM_ERROR:
              printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto EXIT;
    EXIT:
		return;
}

void timing_add_save(int id)
{
    switch(id)
    {
        case MGTCGI_SYS_TIMING_ADSL_REDIAL:
            adsl_timing_redial_add_save();
            break;
        default:
            goto PARAM_ERROR;
	     break;
    }
    goto EXIT;

    PARAM_ERROR:
              printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto EXIT;
    EXIT:
		return;
}

void timing_remove_save(id)
{
    switch(id)
    {
        case MGTCGI_SYS_TIMING_ADSL_REDIAL:
            adsl_timing_redial_remove_save();
            break;
        default:
            goto PARAM_ERROR;
	     break;
    }
    goto EXIT;

    PARAM_ERROR:
              printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto EXIT;
    EXIT:
		return;
}

int cgiMain(void)
{
	char submit_type[STRING_LENGTH];
       char id_str[STRING_LENGTH];
	int iserror=0,id;

	cgiFormString("submit", submit_type, sizeof(submit_type));
       cgiFormString("id", id_str, sizeof(id_str));
	
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

       id = atoi(id_str);
	if (strcmp(submit_type,"list") == 0 ){//列表显示所有
		timing_list_show(id);
	}	
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		timing_add_save(id);
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		timing_remove_save(id);
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
