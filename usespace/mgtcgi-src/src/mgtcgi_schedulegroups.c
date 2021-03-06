#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void print_groups_list(struct mgtcgi_xml_group_array * group);
extern int check_weeks(const char *weeks, const char *day);
extern int groups_add_save(int mgtcgi_group_type, int add_includ_num,
            const char *name, const char *value, const char *comment);
extern int check_list_string_dot(const char *string, const char *dot);
extern int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
			const char *name, const char *old_name, const char *value, const char *comment);
extern int groups_remove_save(int mgtcgi_group_type, const char *name);

extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);


void schedulegroups_list_show(void)
{
	int size=0,total=0,iserror=0;
	struct mgtcgi_xml_group_array * group = NULL;	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror  = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印组对象
	total = group->num;
	print_groups_list(group);

		printf("\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void**)&group);
		return;
}

void schedulegroups_add_show(void)
{
	int size=0,index=0,iserror=0,total=0;
	struct mgtcgi_xml_schedules * schedules = NULL;


	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = schedules->num;
	printf("{\"left_list\":[");
	for (index=0; index < total;){
		printf("\"%s\"",schedules->pinfo[index].name);
		index++;
		if (index < total)
			printf(",");
		else
			break;
	}
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&schedules);
		return;
}

void schedulegroups_edit_show(void)
{

	int size=0,index=0,left=0,i=0;
	int flag,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_schedules * schedules = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	

	cgiFormString("name",name,STRING_LENGTH);

	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	flag = 0;
	printf("{\"data\":[{\"name\":\"%s\"}],\"left_list\":[",name);
	for (index=0; index < group->num; index++){
		if (strcmp(name,group->pinfo[index].group_name) == 0){
			for (left=0; left < schedules->num; left++){
				for(i=0; i < group->pinfo[index].num;){
					if (0 == strcmp(schedules->pinfo[left].name, group->pinfo[index].pinfo[i].name))
						break;
					else
						i++;
				} 
				if (0 == flag){
					if (i == group->pinfo[index].num){
						printf("\"%s\"",schedules->pinfo[left].name);
						flag = 1;
					}
				}
				else{
					if (i == group->pinfo[index].num){
						printf(",\"%s\"",schedules->pinfo[left].name);
						flag = 1;
					}
				}
		
			}
			printf("],");

			printf("\"right_list\":[");
			for(i=0; i < group->pinfo[index].num;){
				printf("\"%s\"",group->pinfo[index].pinfo[i].name);
				i++;
				if (i < group->pinfo[index].num)
					printf(",");
				else
					break;
			}
			printf("],");
			
			break;
		}
	}

		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&schedules);
		free_xml_node((void*)&group);
		return;
}

void schedulegroups_add_save(void)
{
	int size,index=0,add_includ_num=0,iserror=0;
	char name[STRING_LENGTH]={"test"};
	//char value[STRING_LIST_LENGTH]={"time1"};
	char * value=NULL;
	char *dot=",";
	//char str[STRING_LIST_LENGTH];
	char * str = NULL;
	char *ptr;
	struct mgtcgi_xml_schedules	 *obj_nodes = NULL;

    value = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!value)
        goto ERROR_EXIT;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH * sizeof(char));

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
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&obj_nodes,&size);
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

	str = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!str)
        goto ERROR_EXIT;

	//value检查，必须是对象存在的名称。
	strcpy(str, value);
	ptr=strtok(str,dot);
	if (NULL != ptr){
		for (index=0; index < obj_nodes->num; index++){
			if (0 == strcmp(obj_nodes->pinfo[index].name, ptr))
				break;	
		}
		if (index == obj_nodes->num){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			for (index=0; index < obj_nodes->num; index++){
				if (0 == strcmp(obj_nodes->pinfo[index].name, ptr))
					break;		
			}
			if (index == obj_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	free(str);
    str = NULL;
	groups_add_save(MGTCGI_TYPE_SCHEDULES_GROUP, add_includ_num, name, value, "");
	
/*************************释放内存，调用保存函数*************************/
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);    
    if(str)
        free(str);
    if(value)
        free(value);
	return;
}

void schedulegroups_edit_save(void)
{
	int size,index=0,ret_value=0,add_includ_num=0;
	int diff_name=0,iserror=0;
	char old_name[STRING_LENGTH]={"bb"};
	char name[STRING_LENGTH]={"bb1"};
	//char value[STRING_LIST_LENGTH]={"time1"};
	char * value = NULL;
	char *dot=",";
	//char str[STRING_LIST_LENGTH];
    char * str = NULL;
	char *ptr;
	struct mgtcgi_xml_schedules	 *obj_nodes = NULL;
	//时间对象关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

    value = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!value)
        goto ERROR_EXIT;
    
	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH * sizeof(char));

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
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	str = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!str)
        goto ERROR_EXIT;
	//value检查，必须是对象存在的名称。
	strcpy(str, value);
	ptr=strtok(str,dot);
	if (NULL != ptr){
		for (index=0; index < obj_nodes->num; index++){
			if (0 == strcmp(obj_nodes->pinfo[index].name, ptr))
				break;	
		}
		if (index == obj_nodes->num){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			for (index=0; index < obj_nodes->num; index++){
				if (0 == strcmp(obj_nodes->pinfo[index].name, ptr))
					break;		
			}
			if (index == obj_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
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
	}
	
	if (1 == diff_name){
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		//应用层策略检查
		ret_value = firewall_name_check(firewalls, name, old_name, 2);
		if (0 != ret_value){
			if (1 == ret_value){
				iserror= MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}//end if(1 == diff_name)


	/*************************释放内存，调用保存函数*************************/
	free(str);
    str = NULL;
	groups_edit_save(MGTCGI_TYPE_SCHEDULES_GROUP,
					add_includ_num, diff_name, name, old_name, value, "");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);
	free_xml_node((void*)&firewalls);
    if(str)
        free(str);
    if(value)
        free(value);
	return;
}

void schedulegroups_remove_save(void)
{
	int size,ret_value=0,iserror=0;
	char name[STRING_LENGTH]={"bb1"};

	//时间对象关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//应用层策略检查
	ret_value = firewall_name_check(firewalls, name, name, 2);
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
	groups_remove_save(MGTCGI_TYPE_SCHEDULES_GROUP, name);
	
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&firewalls);
		return;
}

void schedulegroups_move(void)
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
		schedulegroups_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		schedulegroups_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		schedulegroups_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		schedulegroups_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		schedulegroups_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		schedulegroups_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		schedulegroups_move();
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

