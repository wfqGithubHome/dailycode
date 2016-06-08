#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end);

extern int ipsession_name_check(struct mgtcgi_xml_ip_session_limits *ipsession,const char *name, const char *old_name, int check_id);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);
extern int get_bridge_num(void);

void session_list_show(void)
{
	int size=0,index=0,index_end=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,iserror=0,result=0;
	struct mgtcgi_xml_session_limits * session_limits = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
	if ( NULL == session_limits){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = session_limits->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (; index < index_end ;){
		printf("{\"name\":\"%s\",\"value\":\"%s\"}",\
			session_limits->pinfo[index].name,session_limits->pinfo[index].value);
		index++;
		if (index < index_end)
			printf(",");
		else
			break;
	}
		printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&session_limits);
		return;
}

void session_add_show(void)
{
	;
}

void session_edit_show(void)
{

	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_session_limits * session_limits = NULL;

	cgiFormString("name",name,STRING_LENGTH);

	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto  ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
	if ( NULL == session_limits){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"data\":[");
	for (index = 0; index < session_limits->num ; index++){
		if (strcmp(name,session_limits->pinfo[index].name) == 0){
			printf("{\"name\":\"%s\",\"value\":\"%s\"}",\
				session_limits->pinfo[index].name,session_limits->pinfo[index].value);
			break;
		}

	}	
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&session_limits);
		return;
}

void session_add_save(void)
{
	int old_size,new_size,index=0,iserror=0,result=0;
	char name[STRING_LENGTH]={"1"};
	char value[STRING_LENGTH]={"10000"};

	struct mgtcgi_xml_session_limits *old_nodes = NULL;
	struct mgtcgi_xml_session_limits *new_nodes = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}	
	}

	/****************************重新构建新结构***************************/
	old_size = sizeof(struct mgtcgi_xml_session_limits) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_session_limit_info);
	new_size = sizeof(struct mgtcgi_xml_session_limits) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_session_limit_info);

	new_nodes = (struct mgtcgi_xml_session_limits *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = old_nodes->num + 1;
	strcpy(new_nodes->pinfo[old_nodes->num].name, name);
	strcpy(new_nodes->pinfo[old_nodes->num].value, value);

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n",index,new_nodes->pinfo[index].value);
	}
#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SESSION_LIMITS,(void*)new_nodes, new_size);
	if (result < 0){
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
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&old_nodes);
		return;
}

void session_edit_save(void)
{
	int old_size,size,index=0;
	int diff_name=0,ret_value=0,iserror=0,result=0;
	char old_name[STRING_LENGTH]={"aa"};
	char name[STRING_LENGTH]={"aa1"};
	char value[STRING_LENGTH]={"10000"};

	struct mgtcgi_xml_session_limits *old_nodes = NULL;
	
	//会话数对象关联的结构定义
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//网络层策略
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH)
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
			
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < old_nodes->num; index++){
			if (0 == strcmp(old_nodes->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (1 == diff_name){//如果名称修改了，检查是否重复和检查是否正在使用
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsessions,&size);
		if ( NULL == ipsessions){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		//网络层策略检查
		ret_value = ipsession_name_check(ipsessions, name, old_name, 2); // 2会话数
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

		//应用层策略检查
		ret_value = firewall_name_check(firewalls, name, old_name, 4);// 4会话数
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
	}//end if(1 == diff_name)
	
	
	/*********************************修改数据***********************************/	
	iserror = -1;
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, old_name)){	
			strcpy(old_nodes->pinfo[index].name, name);
			strcpy(old_nodes->pinfo[index].value, value);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SESSION_LIMITS,(void*)old_nodes, old_size);
			if (result < 0){
				iserror = MGTCGI_WRITE_FILE_ERR;
				goto ERROR_EXIT;
			}
			iserror = 0;
			break;
		}	
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("old_nodes->pinfo[%d].name:%s\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].value:%s\n",index,old_nodes->pinfo[index].value);
	}
#endif
	
	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&ipsessions);
		free_xml_node((void*)&firewalls);
		return;
}

void session_remove_save(void)
{
	int old_size,size,index=0;
	int ret_value=0,iserror=0,result=0;
	int remove_num,old_num,new_num,new_size,new_index;
	char name[STRING_LENGTH]={"aa1"};

	struct mgtcgi_xml_session_limits *old_nodes = NULL;
	struct mgtcgi_xml_session_limits *new_nodes = NULL;
	
	//会话数对象关联的结构定义
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//网络层策略
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	if (old_nodes->num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	remove_num = -1;
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsessions,&size);
	if ( NULL == ipsessions){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//网络层策略检查
	ret_value = ipsession_name_check(ipsessions, name, name, 2); // 2会话数
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

	//应用层策略检查
	ret_value = firewall_name_check(firewalls, name, name, 4);// 4会话数
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


	/***********************可以删除，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;
	old_size = sizeof(struct mgtcgi_xml_session_limits) + 
				old_num *
				sizeof(struct mgtcgi_xml_session_limit_info);
	new_size = sizeof(struct mgtcgi_xml_session_limits) + 
				new_num *
				sizeof(struct mgtcgi_xml_session_limit_info);

	new_nodes = (struct mgtcgi_xml_session_limits *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->num = new_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			strcpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name);
			strcpy(new_nodes->pinfo[new_index].value, old_nodes->pinfo[index].value);
			new_index++;
		}
	}

	if (new_index == new_nodes->num){	
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SESSION_LIMITS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
		
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < old_nodes->num; index++){
		printf("old_nodes->pinfo[%d].name:%s\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].value:%s\n",index,old_nodes->pinfo[index].value);
	}
	printf("\n\n");
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n",index,new_nodes->pinfo[index].value);
	}
#endif
	

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&ipsessions);
		free_xml_node((void*)&firewalls);
		return;
}

void session_move(void)
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
		session_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		session_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		session_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		session_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		session_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		session_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		session_move();
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

