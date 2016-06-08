#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int check_list_string_dot(const char *string, const char *dot);

int get_device_series()
{
	char buffer[STRING_LENGTH] = {0};
	char cmd[CMD_LENGTH] = {0};
	FILE *pp = NULL;
	int retval = 0;
	
	snprintf(cmd, sizeof(cmd), "cat /etc/version | grep SERIES= | awk -F '\"' '{print $2}'");
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buffer, sizeof(buffer), pp);
		buffer[strlen(buffer)-1] = '\0';
		if (strcmp(buffer, "NS") == 0){
			retval = 1;
		}
        else if (strcmp(buffer, "DC") == 0){
			retval = 1;
		}
        else if(strcmp(buffer, "V5") == 0){
            retval = 1;
        }
        else if(strcmp(buffer, "FR") == 0){
            retval = 1;
        }
    	else if (strcmp(buffer,"WX") == 0){
            retval = 1;
    	}
		else if (strcmp(buffer, "NP") == 0){
			retval = 2;
		}
		else {
			retval = 3;
		}
	}
	pclose(pp);
	return retval;
}


void l7protocol_list_show(void)
{
	int size=0,index=0,index_end=0,selected=0,total,flag,iserror;
	char selected_str[DIGITAL_LENGTH];
	int series = 0;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;

	cgiFormString("selected",selected_str,STRING_LENGTH);
		
	if (strlen(selected_str) < STRING_LITTLE_LENGTH) {
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	selected = atoi(selected_str);
	if (!(0 <= selected && selected <=1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
						"chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	series = get_device_series();
		
	total=0;
	index_end=l7protocol->num;

	printf("{\"data\":[");
	flag = 0;
	
	for (index=0; index < index_end; index++){
		if ((1 == selected) && (0 == l7protocol->pinfo[index].selected)){
			l7protocol->num--; 
			continue;
		}

		if (series == 1){//NS系列
			if (strcmp(l7protocol->pinfo[index].type, "Voip") == 0 ||
				strcmp(l7protocol->pinfo[index].type, "Rfc") ==0){
				l7protocol->num--; 
				continue;
			}
		}
	
		if (0 == flag){
			printf("{\"id\":\"%u\",\"gid\":\"%u\",\"name\":\"%s\",\"name2\":\"%s\",\"type\":\"%s\",\
\"type_name\":\"%s\",\"selected\":%u,\"show\":%u}\n",\
            l7protocol->pinfo[index].id,
            l7protocol->pinfo[index].gid,
			l7protocol->pinfo[index].name,
			l7protocol->pinfo[index].match,
			l7protocol->pinfo[index].type,
			l7protocol->pinfo[index].type_name,
			l7protocol->pinfo[index].selected,
			l7protocol->pinfo[index].show);
			flag = 1;
		}else{
			printf(",{\"id\":\"%u\",\"gid\":\"%u\",\"name\":\"%s\",\"name2\":\"%s\",\"type\":\"%s\",\
\"type_name\":\"%s\",\"selected\":%u,\"show\":%u}\n",\
            l7protocol->pinfo[index].id,
            l7protocol->pinfo[index].gid,
			l7protocol->pinfo[index].name,
			l7protocol->pinfo[index].match,
			l7protocol->pinfo[index].type,
			l7protocol->pinfo[index].type_name,
			l7protocol->pinfo[index].selected,
			l7protocol->pinfo[index].show);
		}
		total++;

	}

		printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void**)&l7protocol);
		return;	
}

void l7protocol_add_show(void)
{
	;
}

void l7protocol_edit_show(void)
{

}

void l7protocol_add_save(void)
{

}

void l7protocol_edit_save(void)
{
	int old_size,new_size,new_l7log_num,index;
	int l7log_include_num,ret_value,iserror;
	char *value = NULL;
	char *ptr = NULL;
	char *dot=",";

	struct mgtcgi_xml_loggers *old_nodes = NULL;
	struct mgtcgi_xml_loggers *new_nodes = NULL;
	

	value = (char *)malloc(STRING_LIST_LENGTH * sizeof(char));
	if (NULL == value){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	cgiFormString("value",value,(STRING_LIST_LENGTH * sizeof(char)));
	
	l7log_include_num = check_list_string_dot(value, dot);
	if (l7log_include_num < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_LOGGER,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_l7log_num = l7log_include_num;	
	new_size = sizeof(struct mgtcgi_xml_loggers) + 
				new_l7log_num *
				sizeof(struct mgtcgi_xml_session_layer7_item);
	
	new_nodes = (struct mgtcgi_xml_loggers *)malloc_buf(new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	memset(new_nodes, 0, new_size);
	new_nodes->block_log_enable = old_nodes->block_log_enable ;
	new_nodes->http_log_enable = old_nodes->http_log_enable;
	new_nodes->session_log_enable = old_nodes->session_log_enable;
	new_nodes->scan_detect_log_enable = old_nodes->scan_detect_log_enable;
	new_nodes->dos_detect_log_enable = old_nodes->dos_detect_log_enable;
	new_nodes->num = new_l7log_num;

	//l7log 的 inclue赋值
	if (l7log_include_num > 0){
		index = 0;
		ptr=strtok(value,dot);
		if ((NULL != ptr) && (index < new_nodes->num)){
				strcpy(new_nodes->layer7log[index].name, ptr);
				index++;
		}
		while((ptr=strtok(NULL,dot))){
			if ((NULL != ptr) && (index < new_nodes->num)){
				strcpy(new_nodes->layer7log[index].name, ptr);
				index++;
			} else {
				break;
			}
		}
	}

	ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_LOGGER,(void*)new_nodes,new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		free_malloc_buf((void*)&value);
		return;	                                                                                                                             
}

void l7protocol_remove(void)
{

}

void l7protocol_move(void)
{

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

	if (strcmp(submit_type,"list") == 0 ){//列表显示所有
		l7protocol_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		l7protocol_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		l7protocol_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		l7protocol_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		l7protocol_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		l7protocol_remove();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		l7protocol_move();
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

