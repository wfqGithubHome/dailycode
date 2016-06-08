#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end);

extern int group_name_check(struct mgtcgi_xml_group_array *group, const char *name, const char *old_name);
extern int dnspolicy_name_check(struct mgtcgi_xml_dns_policys *dnspolicy,const char *name, const char *old_name, int check_id);

void domain_list_show(void)
{
	int size=0,index=0,index_end=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,result=0,iserror=0;
	struct mgtcgi_xml_domains * domains = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&domains,&size);
	if ( NULL == domains){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = domains->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (; index < index_end ;){
		printf("{\"name\":\"%s\",\"value\":\"%s\",\"comment\":\"%s\"}",\
			domains->pinfo[index].name,domains->pinfo[index].value,domains->pinfo[index].comment);
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
		free_xml_node((void*)&domains);
		return;
}

void domain_add_show(void)
{
	;
}

void domain_edit_show(void)
{

	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_domains * domains = NULL;

	cgiFormString("name",name,STRING_LENGTH);

	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&domains,&size);
	if ( NULL == domains){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"data\":[");
	for (index = 0; index < domains->num ; index++){
		if (strcmp(name,domains->pinfo[index].name) == 0){
			printf("{\"name\":\"%s\",\"value\":\"%s\",\"comment\":\"%s\"}",\
				domains->pinfo[index].name,domains->pinfo[index].value,domains->pinfo[index].comment);
			break;
		}

	}
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&domains);
		return;
}

void domain_add_save(void)
{
	int old_size,new_size,index=0,size,iserror=0,result=0;
	char name[STRING_LENGTH]={"1"};
	char value[STRING_LENGTH]={"1"};
	char comment[STRING_LENGTH]={"1"};

	struct mgtcgi_xml_domains *old_nodes = NULL;
	struct mgtcgi_xml_domains *new_nodes = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}	
	}
	for (index=0; index < group->num; index++){
		if (0 == strcmp(group->pinfo[index].group_name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}	
	}


	/****************************重新构建新结构***************************/
	old_size = sizeof(struct mgtcgi_xml_domains) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_domain_info);
	new_size = sizeof(struct mgtcgi_xml_domains) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_domain_info);

	new_nodes = (struct mgtcgi_xml_domains *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = old_nodes->num + 1;
	strcpy(new_nodes->pinfo[old_nodes->num].name, name);
	strcpy(new_nodes->pinfo[old_nodes->num].value, value);
	strcpy(new_nodes->pinfo[old_nodes->num].comment, comment);

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n\n",index,new_nodes->pinfo[index].value);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,new_nodes->pinfo[index].comment);
	}
#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAINS,(void*)new_nodes, new_size);
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
		free_xml_node((void*)&group);
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		return;
}

void domain_edit_save(void)
{
	int old_size,size,index=0,result=0;
	int diff_name=0,ret_value=0,iserror=0;
	char old_name[STRING_LENGTH]={"aa"};
	char name[STRING_LENGTH]={"aa1"};
	char value[STRING_LENGTH]={"1"};
	char comment[STRING_LENGTH]={"1"};

	struct mgtcgi_xml_domains *old_nodes = NULL;
	//与对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //对象组
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS管控

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************参数检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&old_nodes,&old_size);
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
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&group,&size);
		if ( NULL == group){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
	
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
		if ( NULL == dnspolicys){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		//对象组检查
		ret_value = group_name_check(group, name, old_name);
		if (0 != ret_value){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
			else if (2 == ret_value){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}

		//DNS策略检查
		ret_value = dnspolicy_name_check(dnspolicys, name, old_name, 2);
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
			strcpy(old_nodes->pinfo[index].comment, comment);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAINS,(void*)old_nodes, old_size);
			if (result < 0){
				iserror = MGTCGI_WRITE_FILE_ERR;
				goto ERROR_EXIT;
			}
			iserror = 0;
			break;
		}	
	}
	if (iserror !=0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("old_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].value:%s\n\n",index,old_nodes->pinfo[index].value);
		printf("old_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
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
		free_xml_node((void*)&group);
		free_xml_node((void*)&dnspolicys);
		return;
}

void domain_remove_save(void)
{
	int old_size,size,index=0,result=0;
	int ret_value=0,iserror=0;
	int remove_num,old_num,new_num,new_size,new_index;
	char name[STRING_LENGTH]={"aa1"};

	struct mgtcgi_xml_domains *old_nodes = NULL;
	struct mgtcgi_xml_domains *new_nodes = NULL;
	
	//与对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //对象组
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS管控

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************参数检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&old_nodes,&old_size);
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


	//如果名称修改了，检查是否重复和检查是否正在使用
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
	if ( NULL == dnspolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//对象组检查
	ret_value = group_name_check(group, name, name);
	if (0 != ret_value){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		}
		else if (2 == ret_value){
			;
		}
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}

	//DNS策略检查
	ret_value = dnspolicy_name_check(dnspolicys, name, name, 2);
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

	old_size = sizeof(struct mgtcgi_xml_domains) + 
				old_num *
				sizeof(struct mgtcgi_xml_domain_info);
	new_size = sizeof(struct mgtcgi_xml_domains) + 
				new_num *
				sizeof(struct mgtcgi_xml_domain_info);

	new_nodes = (struct mgtcgi_xml_domains *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->num = new_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			strcpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name);
			strcpy(new_nodes->pinfo[new_index].value, old_nodes->pinfo[index].value);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			new_index++;
		}
	}

	if (new_index == new_nodes->num){	
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAINS,(void*)new_nodes, new_size);
		if (result  < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
		
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("old_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].value:%s\n\n",index,old_nodes->pinfo[index].value);
		printf("old_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
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
		free_xml_node((void*)&group);
		free_xml_node((void*)&dnspolicys);
		return;
}

void domain_move(void)
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
		domain_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		domain_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		domain_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		domain_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		domain_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		domain_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		domain_move();
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

