#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, 
            int total, int *index, int *index_end);
extern void print_groups_list(struct mgtcgi_xml_group_array * group);
extern void print_groups_name_list(struct mgtcgi_xml_group_array * group);
extern int check_list_string_dot(const char *string, const char *dot);
extern int groups_add_save(int mgtcgi_group_type, int add_includ_num,
						const char *name, const char *value, const char *comment);
extern int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
							const char *name, const char *old_name, const char *value, const char *comment);
extern int groups_remove_save(int mgtcgi_group_type, const char *name);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);


void custom_proto_groups_list_show(void)
{
	int size=0,index=0,iserror=0;
	int total=0,i=0,flag=0;
	struct mgtcgi_xml_group_array * group = NULL;	

	get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP_CN,
							"chinese", (void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印组对象
	flag = 0;
	total = group->num;
	printf("{\"data\":[");
	for (index=0; index < group->num; index++){
		if (flag == 0)
			flag++;
		else
			printf(",");
		
		printf("{\"name\":\"%s\",\"objlist\":\"",group->pinfo[index].group_name);
		for(i=0; i< group->pinfo[index].num;){
			printf("%s ",group->pinfo[index].pinfo[i].name);
			i++;			
		}
		printf("\"}");
	}
	printf("],");

	//print_groups_list(group);

		printf("\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&group);
		return;
}


void custom_proto_groups_name_list_show(void)
{
	int size=0;
	int total = 0;
	struct mgtcgi_xml_group_array * group = NULL;	

	get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
							"english", (void**)&group,&size);
	if ( NULL == group){
		goto memory_failure;
	}

	//打印组对象
	total = group->num;
	print_groups_name_list(group);
	printf("\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
	goto free_buf;

	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		goto free_buf;
	free_buf:
		free_xml_node((void**)&group);
		return;
}


void custom_proto_groups_add_show(void)
{
	int size=0,index=0,index_end=0,flag=0,iserror=0;
	int total = 0;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;

	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
					"chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	flag = 0;
	index_end = l7protocol->num;
	for (index=0; index < index_end; index++){	
		if (0 == l7protocol->pinfo[index].selected)
			continue;
		else
			l7protocol->pinfo[index].selected=0;
		
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
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void**)&l7protocol);
		return;
}

void custom_proto_groups_edit_show(void)
{
	int size=0,index=0,index_end=0,flag=0,i=0;
	int total = 0,edit_index=0,iserror=0;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	
	char name_str[STRING_LENGTH]={"xxx"};
	
	cgiFormString("name",name_str,STRING_LENGTH);
	if (strlen(name_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
					"chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
							"english", (void**)&group, &size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//要编辑的列表
	edit_index = -1;
	for(index=0; index < group->num; index++ ){
		if (strcmp(group->pinfo[index].group_name, name_str) == 0){
			edit_index = index;
			break;
		}
	}
	if (-1 == edit_index){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	

	printf("{\"data\":[");
	flag = 0;
	index_end = l7protocol->num;
	for (index=0; index < index_end; index++){	
		if (0 == l7protocol->pinfo[index].selected){
			continue;
		}else{
			l7protocol->pinfo[index].selected=0;
			for (i=0; i<group->pinfo[edit_index].num; i++){
				if(strcmp(group->pinfo[edit_index].pinfo[i].name, l7protocol->pinfo[index].name) == 0){
					l7protocol->pinfo[index].selected=1;
					break;	
				}
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
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void**)&l7protocol);
		free_xml_node((void**)&group);
		return;
}

void custom_proto_groups_add_save(void)
{
	int size,index=0,add_includ_num=0,iserror=0;
	char name[STRING_LENGTH]={""};
	char *value = NULL;
	char *dot=",";
	struct mgtcgi_xml_protocols	 *obj_nodes = NULL;

	value = (char *)malloc(STRING_LIST_LENGTH * sizeof(char));
	if (NULL == value){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,(STRING_LIST_LENGTH * sizeof(char)));
	
	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	add_includ_num = check_list_string_dot(value, dot);
	if (add_includ_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
			
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//不能与对象名重名
	for (index=0; index < obj_nodes->num; index++){
		if (0 == strcmp(obj_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}		
	}

	//不能使用协议类型名称
	if (strcmp(name, "Game") == 0 ||
		strcmp(name, "Websp") == 0 ||
		strcmp(name, "P2Psp") == 0 ||
		strcmp(name, "P2P") == 0 ||
		strcmp(name, "Webtransfer") == 0 ||
		strcmp(name, "Web") == 0 ||
		strcmp(name, "Chat") == 0 ||
		strcmp(name, "NetworkIST") == 0 ||
		strcmp(name, "FileTransfer") == 0 ||
		strcmp(name, "Security") == 0 ||
		strcmp(name, "Database") == 0 ||
		strcmp(name, "Mail") == 0 ||
		strcmp(name, "Stock") == 0 ||
		strcmp(name, "Terminals") == 0 ||
		strcmp(name, "Voip") == 0 ||
		strcmp(name, "Rfc") == 0 ||
		strcmp(name, "Others") == 0){
		iserror = MGTCGI_DUPLICATE_ERR;
		goto ERROR_EXIT;
	}

	/************************释放内存，调用修改xml文件函数*************************/
	groups_add_save(MGTCGI_TYPE_PROTOCOLS_GROUP,
						add_includ_num, name, value, "");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void *)&obj_nodes);
		free_malloc_buf((void *)&value);
		return;
}

void custom_proto_groups_edit_save(void)
{
	int size,index=0,ret_value=0,add_includ_num=0;
	int diff_name,iserror=0;
	char old_name[STRING_LENGTH]={"aa1"};
	char name[STRING_LENGTH]={"AA1"};
	//char value[STRING_LIST_LENGTH]={"FTP"};
	char * value = NULL;
	char *dot=",";
	struct mgtcgi_xml_protocols	 *obj_nodes = NULL;
	//关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

    value = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!value)
        goto ERROR_EXIT;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH*sizeof(char));

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH)
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	add_includ_num = check_list_string_dot(value, dot);
	if (add_includ_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (strcmp(name, old_name) == 0){
		diff_name = 0;//名称没有被修改
	} else { 
		diff_name = 1; //名称修改了，检测是否重复
		//不能与对象名重名
		for (index=0; index < obj_nodes->num; index++){
			if (0 == strcmp(obj_nodes->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}		
		}
		//不能使用协议类型名称
		if (strcmp(name, "Game") == 0 ||
			strcmp(name, "Websp") == 0 ||
			strcmp(name, "P2Psp") == 0 ||
			strcmp(name, "P2P") == 0 ||
			strcmp(name, "Webtransfer") == 0 ||
			strcmp(name, "Web") == 0 ||
			strcmp(name, "Chat") == 0 ||
			strcmp(name, "NetworkIST") == 0 ||
			strcmp(name, "FileTransfer") == 0 ||
			strcmp(name, "Security") == 0 ||
			strcmp(name, "Database") == 0 ||
			strcmp(name, "Mail") == 0 ||
			strcmp(name, "Stock") == 0 ||
			strcmp(name, "Terminals") == 0 ||
			strcmp(name, "Voip") == 0 ||
			strcmp(name, "Rfc") == 0 ||
			strcmp(name, "Others") == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}

	if (1 == diff_name){
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		//应用层策略检查
		ret_value = firewall_name_check(firewalls, name, old_name, 5);
		if (0 != ret_value){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}

	}

	/*************************释放内存，调用保存函数*************************/
	ret_value = groups_edit_save(MGTCGI_TYPE_PROTOCOLS_GROUP,
						add_includ_num, diff_name, name, old_name, value, "");

	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);
	free_xml_node((void*)&firewalls);
    if(value)
        free(value);
	return;
}

void custom_proto_groups_remove(void)
{
	int size, ret_value=0, iserror=0;
	char name[STRING_LENGTH]={"ddd"};

	//关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size);
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//应用层策略检查
	ret_value = firewall_name_check(firewalls, name, name, 5);
	if (0 != ret_value){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		}
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}

	/*************************释放内存，调用保存函数*************************/
	ret_value = groups_remove_save(MGTCGI_TYPE_PROTOCOLS_GROUP, name);

		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&firewalls);
		return;
}

void custom_proto_groups_move(void)
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
		custom_proto_groups_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		custom_proto_groups_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		custom_proto_groups_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		custom_proto_groups_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		custom_proto_groups_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		custom_proto_groups_remove();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		custom_proto_groups_move();
	}
	else if(strcmp(submit_type,"namelist") == 0){//现在自定义组 组名列表
		custom_proto_groups_name_list_show();
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

