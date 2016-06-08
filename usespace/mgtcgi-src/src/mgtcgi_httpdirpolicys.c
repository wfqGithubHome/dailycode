#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"


/*
	Make
	
*/
extern int get_bridge_num(void);
extern void print_extensions_list(struct mgtcgi_xml_http_dir_policys *httpdir_policy,
				struct mgtcgi_xml_group_array *extensiongroups,int bridge,const char *edit_extgroup);
extern void print_httpservers_list(struct mgtcgi_xml_group_array *httpservergroups);
extern inline void *malloc_buf(int size);
extern inline void free_malloc_buf(void **buf);

void httpdirpolicys_list_show(void)
{
	int size=0,index=0,index_end=0,flag=0;
	int page_start=0,page_index=0,iserror=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	int total=0,page=0,pagesize=0,bridge=0,bridge_num=0;
	struct mgtcgi_xml_http_dir_policys * httpdirpolicys = NULL;	

	cgiFormString("page",page_str,DIGITAL_LENGTH);
	cgiFormString("pagesize",pagesize_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);


	if ((strlen(page_str) < 1) || 
		(strlen(pagesize_str) < 1)||
		(strlen(bridge_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if ((strlen(page_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(pagesize_str) < STRING_LITTLE_LENGTH)
		||(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
	if ((page < PAGE_LITTLE_VALUE) 
		|| (pagesize < PAGE_LITTLE_VALUE) 
		|| (bridge < BRIDGE_LITTLE_VALUE)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	page_start = ((page * pagesize) - pagesize);
	if (page_start > httpdirpolicys->policy_num){
		page_start = httpdirpolicys->policy_num;
	}
	index_end = httpdirpolicys->policy_num;

	flag = 0;
	total = 0;
	page_index =0;
	bridge_num = 2;	// 桥数量要根据数据接口设置的桥信息确定

	printf("{\"data\":[");
	for (; index < index_end; index++){
		
		if (bridge == httpdirpolicys->p_policyinfo[index].bridge){
			total++;
			if ((total > page_start) && (page_index < pagesize)){
				page_index++;
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("{\"name\":%d,\"extgroup\":\"%s\",\"bridge\":%d,\
	\"disabled\":%d,\"servergroup\":\"%s\",\"comment\":\"%s\"}",\
					httpdirpolicys->p_policyinfo[index].name,\
					httpdirpolicys->p_policyinfo[index].extgroup,\
					httpdirpolicys->p_policyinfo[index].bridge,\
					httpdirpolicys->p_policyinfo[index].disabled,\
					httpdirpolicys->p_policyinfo[index].servergroup,\
					httpdirpolicys->p_policyinfo[index].comment);
			}
		}
	}
	printf("],");

	bridge_num = get_bridge_num();
	printf("\"total\":%d,\"bridge_num\":%d,\"iserror\":0,\"msg\":\"\"}",total,bridge_num);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&httpdirpolicys);
	return;
}

void httpdirpolicys_add_show(void)
{
	int size=0,bridge=0,iserror=0;
	char bridge_str[DIGITAL_LENGTH];
	struct mgtcgi_xml_group_array *extensiongroups = NULL;
	struct mgtcgi_xml_httpservers *httpservers = NULL;
	struct mgtcgi_xml_group_array *httpservergroups = NULL;
	struct mgtcgi_xml_http_dir_policys * httpdirpolicys = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	
	bridge	= atoi(bridge_str);
	if (bridge < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&extensiongroups,&size);
	if ( NULL == extensiongroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_HTTPSERVERS,(void**)&httpservers,&size);
 	if ( NULL == httpservers){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTPSERVER_GROUP,(void**)&httpservergroups,&size);
	if ( NULL == httpservergroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印默认值
	printf("{\"data\":[{\"extgroup\":\"\",\"servergroup\":\"\",\"disabled\":0}],");
	
	//打印扩展名组，不能重复使用
	print_extensions_list(httpdirpolicys, extensiongroups, bridge, NULL);
	printf(",");
	//打印服务器组，对象不能使用
	print_httpservers_list(httpservergroups);
	printf(",");

/******************************释放分配的内存********************************/	
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&extensiongroups);
		free_xml_node((void*)&extensiongroups);
		free_xml_node((void*)&httpservers);
		free_xml_node((void*)&httpservergroups);
		return;
}

void httpdirpolicys_edit_show(void)
{
	int size=0,index=0,iserror=0;
	char bridge_str[DIGITAL_LENGTH];
	char name_str[DIGITAL_LENGTH];
	char extgroup[STRING_LENGTH]={0};
	int bridge=0,name=0;
	struct mgtcgi_xml_http_dir_policys * httpdirpolicys = NULL;	
	struct mgtcgi_xml_group_array *extensiongroups = NULL;
	struct mgtcgi_xml_httpservers *httpservers = NULL;
	struct mgtcgi_xml_group_array *httpservergroups = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("name",name_str,DIGITAL_LENGTH);

	if ((strlen(bridge_str) < 1) || (strlen(name_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	bridge	= atoi(bridge_str);
	name	= atoi(name_str);
	if ((bridge < 0) || (name < 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < httpdirpolicys->policy_num; index++){
		if ((bridge == httpdirpolicys->p_policyinfo[index].bridge)
			&& (name == httpdirpolicys->p_policyinfo[index].name)){	
			printf("{\"name\":%d,\"extgroup\":\"%s\",\"bridge\":%d,\
\"disabled\":%d,\"servergroup\":\"%s\",\"comment\":\"%s\"}",\
				httpdirpolicys->p_policyinfo[index].name,\
				httpdirpolicys->p_policyinfo[index].extgroup,\
				httpdirpolicys->p_policyinfo[index].bridge,\
				httpdirpolicys->p_policyinfo[index].disabled,\
				httpdirpolicys->p_policyinfo[index].servergroup,\
				httpdirpolicys->p_policyinfo[index].comment);

			strcpy(extgroup, httpdirpolicys->p_policyinfo[index].extgroup);
			break;
		}
	}
	printf("],");


	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&extensiongroups,&size);
	if ( NULL == extensiongroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_HTTPSERVERS,(void**)&httpservers,&size);
	if ( NULL == httpservers){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTPSERVER_GROUP,(void**)&httpservergroups,&size);
	if ( NULL == httpservergroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//json格式打印所有列表数组
	print_extensions_list(httpdirpolicys, extensiongroups, bridge, extgroup);
	printf(",");	
	//打印服务器组，对象不能使用
	print_httpservers_list(httpservergroups);
	printf(",");

/******************************释放分配的内存********************************/	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&httpdirpolicys);
	free_xml_node((void*)&extensiongroups);
	free_xml_node((void*)&httpservers);
	free_xml_node((void*)&httpservergroups);
	return;
}

void httpdirpolicys_add_save(void)
{
	int index,old_size,new_size,iserror=0,result=0;
	int old_policy_num=0,old_format_num=0,
		new_policy_num=0,new_format_num=0;
	int disabled,bridge,name;
	int max_id,min_id;

	char bridge_str[DIGITAL_LENGTH]={"0"};
	char extgroup[STRING_LENGTH]={"fs4"};
	char disabled_str[DIGITAL_LENGTH]={"0"};
	char servergroup[STRING_LENGTH]={"dfa"};
	char comment[STRING_LENGTH]={"tt"};

	struct mgtcgi_xml_http_dir_policys *old_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policys *new_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policy_urlformat_info *p_urlinfo_start = NULL;
	struct mgtcgi_xml_http_dir_policy_info *p_policyinfo_start = NULL;

	//注释的部分使用默认值
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("extgroup",extgroup,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,DIGITAL_LENGTH);
	cgiFormString("servergroup",servergroup,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(bridge_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(extgroup) < STRING_LITTLE_LENGTH)
		|| (strlen(servergroup) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);

	if (!((0 <= disabled && disabled <= 1) &&
		(0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	old_format_num = old_nodes->format_num;
	old_policy_num = old_nodes->policy_num;
	new_format_num = old_format_num;
	new_policy_num = old_policy_num + 1;
	
	old_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
		old_format_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
		old_policy_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_info);

	new_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
		new_format_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
		new_policy_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_info);

	new_nodes = (struct mgtcgi_xml_http_dir_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//name计算
	max_id = -1;
	min_id = -1;
	for (index=0; index < old_nodes->policy_num; index++){
		if (old_nodes->p_policyinfo[index].bridge == bridge){
			max_id = old_nodes->p_policyinfo[index].name;
			if (max_id < min_id)
				max_id = min_id;
			else
				min_id = max_id;
		}	
	}

	//增加一条记录

	p_urlinfo_start = (struct mgtcgi_xml_http_dir_policy_urlformat_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys));
	p_policyinfo_start = (struct mgtcgi_xml_http_dir_policy_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys) + 
						new_format_num * 
						sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));


	memset(new_nodes, 0, sizeof(new_nodes));
	memcpy(new_nodes, old_nodes, old_size);

	//修改dir_policy数量，指针
	new_nodes->policy_num = new_policy_num;
	new_nodes->p_urlinfo = p_urlinfo_start;
	new_nodes->p_policyinfo = p_policyinfo_start;
	
	name = max_id + 1;
	new_nodes->p_policyinfo[old_policy_num].name = name;
	new_nodes->p_policyinfo[old_policy_num].disabled = disabled;
	new_nodes->p_policyinfo[old_policy_num].bridge = bridge;
	strcpy(new_nodes->p_policyinfo[old_policy_num].extgroup, extgroup);
	strcpy(new_nodes->p_policyinfo[old_policy_num].servergroup, servergroup);
	strcpy(new_nodes->p_policyinfo[old_policy_num].comment, comment);

#if _MGTCGI_DEBUG_GET_
	printf("old_nodes->policy_num:%d\n",old_nodes->policy_num);
	for (index=0; index < old_nodes->policy_num; index++){
		printf("old_nodes->p_policyinfo[%d].name:%d\n",index,old_nodes->p_policyinfo[index].name);
		printf("old_nodes->p_policyinfo[%d].disabled:%d\n",index,old_nodes->p_policyinfo[index].disabled);
		printf("old_nodes->p_policyinfo[%d].bridge:%d\n",index,old_nodes->p_policyinfo[index].bridge);

		printf("old_nodes->p_policyinfo[%d].extgroup:%s\n",index,old_nodes->p_policyinfo[index].extgroup);
		printf("old_nodes->p_policyinfo[%d].servergroup:%s\n",index,old_nodes->p_policyinfo[index].servergroup);
		printf("old_nodes->p_policyinfo[%d].comment:%s\n",index,old_nodes->p_policyinfo[index].comment);
	}
	printf("\n\n");
	

	printf("new_nodes->policy_num:%d\n",new_nodes->policy_num);
	for (index=0; index < new_nodes->policy_num; index++){
		printf("new_nodes->p_policyinfo[%d].name:%d\n",index,new_nodes->p_policyinfo[index].name);
		printf("new_nodes->p_policyinfo[%d].disabled:%d\n",index,new_nodes->p_policyinfo[index].disabled);
		printf("new_nodes->p_policyinfo[%d].bridge:%d\n",index,new_nodes->p_policyinfo[index].bridge);

		printf("new_nodes->p_policyinfo[%d].extgroup:%s\n",index,new_nodes->p_policyinfo[index].extgroup);
		printf("new_nodes->p_policyinfo[%d].servergroup:%s\n",index,new_nodes->p_policyinfo[index].servergroup);
		printf("new_nodes->p_policyinfo[%d].comment:%s\n",index,new_nodes->p_policyinfo[index].comment);
	}
#endif

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&new_nodes);
		return;
}

void httpdirpolicys_edit_save(void)
{
	int index,new_size,iserror=0,result=0;
	int disabled,bridge,name;

	char name_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char extgroup[STRING_LENGTH]={"fs4"};
	char disabled_str[DIGITAL_LENGTH]={"0"};
	char servergroup[STRING_LENGTH]={"dfa"};
	char comment[STRING_LENGTH]={"tt"};

	struct mgtcgi_xml_http_dir_policys *new_nodes = NULL; 

	//注释的部分使用默认值
	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("extgroup",extgroup,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,DIGITAL_LENGTH);
	cgiFormString("servergroup",servergroup,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(bridge_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(name_str) < STRING_LITTLE_LENGTH)
		|| (strlen(extgroup) < STRING_LITTLE_LENGTH)
		|| (strlen(servergroup) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	name = atoi(name_str);

	if (!((0 <= disabled && disabled <= 1) &&
		(0 <= bridge && bridge <= 256) &&
		(0 <= name && name <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	iserror = -1;
	for (index=0; index < new_nodes->policy_num; index++){
		if ((new_nodes->p_policyinfo[index].name == name) &&
			(new_nodes->p_policyinfo[index].bridge == bridge)){
			
			new_nodes->p_policyinfo[index].disabled = disabled;
			new_nodes->p_policyinfo[index].bridge = bridge;
			strcpy(new_nodes->p_policyinfo[index].extgroup, extgroup);
			strcpy(new_nodes->p_policyinfo[index].servergroup, servergroup);
			strcpy(new_nodes->p_policyinfo[index].comment, comment);

			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)new_nodes, new_size);
			if (result < 0){
				iserror =  MGTCGI_WRITE_FILE_ERR;
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
	printf("new_nodes->policy_num:%d\n",new_nodes->policy_num);
	for (index=0; index < new_nodes->policy_num; index++){
		printf("new_nodes->p_policyinfo[%d].name:%d\n",index,new_nodes->p_policyinfo[index].name);
		printf("new_nodes->p_policyinfo[%d].disabled:%d\n",index,new_nodes->p_policyinfo[index].disabled);
		printf("new_nodes->p_policyinfo[%d].bridge:%d\n",index,new_nodes->p_policyinfo[index].bridge);

		printf("new_nodes->p_policyinfo[%d].extgroup:%s\n",index,new_nodes->p_policyinfo[index].extgroup);
		printf("new_nodes->p_policyinfo[%d].servergroup:%s\n",index,new_nodes->p_policyinfo[index].servergroup);
		printf("new_nodes->p_policyinfo[%d].comment:%s\n",index,new_nodes->p_policyinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&new_nodes);
		return;
}

void httpdirpolicys_remove_save(void)
{
	int index,old_size,new_size,result=0;
	int old_policy_num=0,old_format_num=0,
		new_policy_num=0,new_format_num=0;
	int bridge,name;
	int iserror,remove_num,new_index,name_id=-1;

	char name_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_http_dir_policys *old_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policys *new_nodes = NULL;
	struct mgtcgi_xml_http_dir_policy_urlformat_info *p_urlinfo_start = NULL;
	struct mgtcgi_xml_http_dir_policy_info *p_policyinfo_start = NULL;

	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(bridge_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(name_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	bridge = atoi(bridge_str);
	name = atoi(name_str);

	if (!((0 <= bridge && bridge <= 256) &&
		(0 <= name && name <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->policy_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	remove_num = -1;
	for (index=0; index < old_nodes->policy_num; index++){
		if ((old_nodes->p_policyinfo[index].name == name) &&
			(old_nodes->p_policyinfo[index].bridge == bridge)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->policy_num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	old_format_num = old_nodes->format_num;
	old_policy_num = old_nodes->policy_num;
	new_format_num = old_format_num;
	new_policy_num = old_policy_num - 1;
	if (new_policy_num < 0)
		new_policy_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
		old_format_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
		old_policy_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_info);

	new_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
		new_format_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
		new_policy_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_info);

	new_nodes = (struct mgtcgi_xml_http_dir_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}


	p_urlinfo_start = (struct mgtcgi_xml_http_dir_policy_urlformat_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys));
	p_policyinfo_start = (struct mgtcgi_xml_http_dir_policy_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys) + 
						new_format_num * 
						sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));

	//修改dir_policy数量，指针
	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->format_num = new_format_num;
	new_nodes->policy_num = new_policy_num;
	new_nodes->p_urlinfo = p_urlinfo_start;
	new_nodes->p_policyinfo = p_policyinfo_start;

	for (index=0; index < new_nodes->format_num; index++){
		new_nodes->p_urlinfo[index].bridge = old_nodes->p_urlinfo[index].bridge;
		strcpy(new_nodes->p_urlinfo[index].format, new_nodes->p_urlinfo[index].format);
		strcpy(new_nodes->p_urlinfo[index].comment, new_nodes->p_urlinfo[index].comment);
	}

	name_id = -1;
	for (index=0,new_index=0; index < old_nodes->policy_num; index++){
		if (old_nodes->p_policyinfo[index].bridge == bridge){
			if (old_nodes->p_policyinfo[index].name == name){
				continue;
			} else {
				if (-1 == name_id)
					name_id = 0;
				else 
					name_id++;	
			}
			new_nodes->p_policyinfo[new_index].name = name_id;
		
		} else {
			new_nodes->p_policyinfo[new_index].name = old_nodes->p_policyinfo[index].name;
		}
		
		new_nodes->p_policyinfo[new_index].disabled = old_nodes->p_policyinfo[index].disabled;
		new_nodes->p_policyinfo[new_index].bridge = old_nodes->p_policyinfo[index].bridge;
		strcpy(new_nodes->p_policyinfo[new_index].extgroup, old_nodes->p_policyinfo[index].extgroup);
		strcpy(new_nodes->p_policyinfo[new_index].servergroup, old_nodes->p_policyinfo[index].servergroup);
		strcpy(new_nodes->p_policyinfo[new_index].comment, old_nodes->p_policyinfo[index].comment);

		new_index++;
	}


	if (new_index == new_nodes->policy_num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->policy_num:%d\n",new_nodes->policy_num);
	for (index=0; index < new_nodes->policy_num; index++){
		printf("new_nodes->p_policyinfo[%d].name:%d\n",index,new_nodes->p_policyinfo[index].name);
		printf("new_nodes->p_policyinfo[%d].disabled:%d\n",index,new_nodes->p_policyinfo[index].disabled);
		printf("new_nodes->p_policyinfo[%d].bridge:%d\n",index,new_nodes->p_policyinfo[index].bridge);

		printf("new_nodes->p_policyinfo[%d].extgroup:%s\n",index,new_nodes->p_policyinfo[index].extgroup);
		printf("new_nodes->p_policyinfo[%d].servergroup:%s\n",index,new_nodes->p_policyinfo[index].servergroup);
		printf("new_nodes->p_policyinfo[%d].comment:%s\n",index,new_nodes->p_policyinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&new_nodes);
		return;
}

void httpdirpolicys_move(void)
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
		httpdirpolicys_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		httpdirpolicys_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		httpdirpolicys_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		httpdirpolicys_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		httpdirpolicys_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		httpdirpolicys_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		httpdirpolicys_move();
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

