#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"


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
		if (strcmp(buffer, "DC") == 0){
			retval = 1;
		}
        else if(strcmp(buffer, "V5") == 0)
        {
            retval = 1;
        }
        else if(strcmp(buffer,"FR") == 0)
        {
            retval = 1;
        }
    	else if (strcmp(buffer,"WX") == 0){
            retval = 1;
    	}
		else if (strcmp(buffer, "DCP") == 0){
			retval = 2;
		}
		else {
			retval = 3;
		}
	}
	pclose(pp);
	return retval;
}

/*
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
			printf("{\"name\":\"%s\",\"name2\":\"%s\",\"type\":\"%s\",\
\"type_name\":\"%s\",\"selected\":%d,\"comment\":\"%s\"}",\
			l7protocol->pinfo[index].name,
			l7protocol->pinfo[index].match,
			l7protocol->pinfo[index].type,
			l7protocol->pinfo[index].type_name,
			l7protocol->pinfo[index].selected,
			l7protocol->pinfo[index].comment);
			flag = 1;
		}else{
			printf(",{\"name\":\"%s\",\"name2\":\"%s\",\"type\":\"%s\",\
\"type_name\":\"%s\",\"selected\":%d,\"comment\":\"%s\"}",\
			l7protocol->pinfo[index].name,
			l7protocol->pinfo[index].match,
			l7protocol->pinfo[index].type,
			l7protocol->pinfo[index].type_name,
			l7protocol->pinfo[index].selected,
			l7protocol->pinfo[index].comment);
		}
		total++;

	}
	printf("],");

	SUCCESS_EXIT:
		printf("\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void**)&l7protocol);
		return;	
}





void l7protocol_edit_save(void)
{
	int old_size,new_size,new_l7log_num,old_l7log_num,index;
	int l7log_include_num,ret_value,iserror;
	char selected_str[DIGITAL_LENGTH];
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
	
	SUCCESS_EXIT:
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
*/
void print_usage(const char *name)
{
	printf("Usage: %s [options]\n", name);
	printf("The options are:\n");
	printf(" 0, --disable	<INT>		disable all protoinfo\n");
	printf(" 1, --enable	<INT>		enable all protoinfo\n");
	printf(" -h,--help	<TOGGLE>	display usage info\n");
}

int main(int argc, char **argv)
{
	int retval = 0;
	int index=0,new_index=0,size=0,old_size=0,new_size=0,series=0;
	FILE *fp = NULL;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
	struct mgtcgi_xml_loggers *old_nodes = NULL;
	struct mgtcgi_xml_loggers *new_nodes = NULL;
	
	if (argc != 2){
		print_usage(argv[0]);
		retval = -1;
		goto EXIT;
	}
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
		print_usage(argv[0]);
		retval = -1;
		goto EXIT;
	}

	if((fp = fopen(L7_XMLFILE,"r")) == NULL){
		goto EXIT;	
	}
	fclose(fp);
	printf("dddddd\n");
	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE, MGTCGI_TYPE_LAYER7_SHOWS,
						"chinese",(void**)&l7protocol,&size);
	if (l7protocol == NULL){
		retval = -2;
		goto EXIT;
	}
    printf("aaaaaaaaaaa\n");
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_LOGGER, (void**)&old_nodes, &old_size);
	if (old_nodes == NULL){
		retval = -3;
		goto EXIT;
	}
    printf("bbbbbbb\n");
	series = get_device_series();
	if (strcmp(argv[1], "1") == 0 || strcmp(argv[1], "--enable") == 0){
		/*for (index=0; index<l7protocol->num; index++){
			printf("l7protocol.pinfo[%d].name:%s\n", index, l7protocol->pinfo[index].name);
			printf("l7protocol.pinfo[%d].match:%s\n", index, l7protocol->pinfo[index].match);
			printf("l7protocol.pinfo[%d].type:%s\n", index, l7protocol->pinfo[index].type);
		}*/

		new_size = sizeof(struct mgtcgi_xml_loggers) + 
				l7protocol->num *
				sizeof(struct mgtcgi_xml_session_layer7_item);
	
		new_nodes = (struct mgtcgi_xml_loggers *)malloc(new_size);
		if (new_nodes == NULL){
			retval = -4;
			goto EXIT;
		}
        printf("cccccc l7protocol->num=%d\n",l7protocol->num);
		memset(new_nodes, 0, new_size);
		new_nodes->block_log_enable = old_nodes->block_log_enable ;
		new_nodes->http_log_enable = old_nodes->http_log_enable;
		new_nodes->session_log_enable = old_nodes->session_log_enable;
		new_nodes->scan_detect_log_enable = old_nodes->scan_detect_log_enable;
		new_nodes->dos_detect_log_enable = old_nodes->dos_detect_log_enable;

		for (new_index=0,index=0; index < l7protocol->num; index++){
			if (series == 1){//NS系列
				if (strcmp(l7protocol->pinfo[index].type, "Voip") == 0 ||
					strcmp(l7protocol->pinfo[index].type, "Rfc") ==0){
					continue;
				}
			}
			strcpy(new_nodes->layer7log[new_index].name, l7protocol->pinfo[index].name);
			new_index++;
		}
		new_nodes->num = new_index;
		/*for(index=0; index<new_nodes->num; index++){
			printf("Name = %s\n", new_nodes->layer7log[index].name);
		}*/
		save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_LOGGER,(void*)new_nodes,new_size);
  	}
	else if (strcmp(argv[1], "0") == 0 || strcmp(argv[1], "--disable") == 0){
		new_size = sizeof(struct mgtcgi_xml_loggers) + 
				0 *
				sizeof(struct mgtcgi_xml_session_layer7_item);
	
		new_nodes = (struct mgtcgi_xml_loggers *)malloc(new_size);
		if (new_nodes == NULL){
			retval = -5;
			goto EXIT;
		}
		memset(new_nodes, 0, new_size);
		new_nodes->block_log_enable = old_nodes->block_log_enable ;
		new_nodes->http_log_enable = old_nodes->http_log_enable;
		new_nodes->session_log_enable = old_nodes->session_log_enable;
		new_nodes->scan_detect_log_enable = old_nodes->scan_detect_log_enable;
		new_nodes->dos_detect_log_enable = old_nodes->dos_detect_log_enable;
		new_nodes->num = 0;
		save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_LOGGER,(void*)new_nodes,new_size);
	}
	else {
		print_usage(argv[0]);
		retval = -1;
		goto EXIT;
	}

EXIT:
	free_xml_node((void **)&l7protocol);
	free_xml_node((void **)&old_nodes);
	free_xml_node((void **)&new_nodes);
	return retval;
}
