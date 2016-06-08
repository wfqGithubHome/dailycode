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

extern int group_name_check(struct mgtcgi_xml_group_array *group, const char *name, const char *old_name);
extern int httpdir_policy_name_check(struct mgtcgi_xml_http_dir_policys *httpdir_policy,
					const char *name, const char *old_name, int check_id);

void extensions_list_show(void)
{
	int size=0,index=0,index_end=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,iserror=0,result=0;
	struct mgtcgi_xml_extensions *extensions = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);

	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&extensions,&size);
 	if ( NULL == extensions){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = extensions->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}
	
	printf("{\"data\":[");
	for (; index < index_end ;){
		printf("{\"name\":\"%s\",\"value\":\"%s\",\"comment\":\"%s\"}",\
			extensions->pinfo[index].name,extensions->pinfo[index].value,extensions->pinfo[index].comment);
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
		free_xml_node((void*)&extensions);
		return;
}

void extensions_add_show(void)
{
	;
}

void extensions_edit_show(void)
{

	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_extensions *extensions = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	
	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&extensions,&size);
	if ( NULL == extensions){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index = 0; index < extensions->num ; index++){
		if (strcmp(name,extensions->pinfo[index].name) == 0){
			printf("{\"name\":\"%s\",\"comment\":\"%s\"}",\
				extensions->pinfo[index].name,extensions->pinfo[index].comment);
			break;
		}

	}
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&extensions);
		return;
}

void extensions_add_save(void)
{
	int old_size,new_size,index=0,size,iserror=0,result=0;
	char name[STRING_LENGTH]={"1"};
	char value[STRING_LENGTH]={"10000"};
	char comment[STRING_LENGTH];

	struct mgtcgi_xml_extensions *old_nodes = NULL;
	struct mgtcgi_xml_extensions *new_nodes = NULL;
	struct mgtcgi_xml_group_array * group = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,DIGITAL_LENGTH);
	cgiFormString("comment",comment,DIGITAL_LENGTH);

	/******************************�������*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
		
	/******************************�ظ��Լ��*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
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

	/****************************���¹����½ṹ***************************/
	old_size = sizeof(struct mgtcgi_xml_extensions) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_extension_info);
	new_size = sizeof(struct mgtcgi_xml_extensions) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_extension_info);

	new_nodes = (struct mgtcgi_xml_extensions *)malloc_buf(new_size);
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
		printf("new_nodes->pinfo[%d].name:%s\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n",index,new_nodes->pinfo[index].value);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSIONS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	/**************************�ɹ����ͷ��ڴ�***************************/

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

void extensions_edit_save(void)
{
	int old_size,size,index=0,result=0;
	int diff_name=0,ret_value=0,iserror=0;
	char old_name[STRING_LENGTH]={"aa"};
	char name[STRING_LENGTH]={"aa1"};
	char value[STRING_LENGTH]={"rar"};
	char comment[STRING_LENGTH]={"tt"};

	struct mgtcgi_xml_extensions *old_nodes = NULL;
	//���������Ľṹ����
	struct mgtcgi_xml_group_array *group = NULL; //������
	struct mgtcgi_xml_http_dir_policys *httpdirpolicys = NULL;	
	
	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,DIGITAL_LENGTH);
	cgiFormString("comment",comment,DIGITAL_LENGTH);

	/******************************�������*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//����û�б��޸�
	} else { //�����޸��ˣ�����Ƿ��ظ�
		diff_name = 1;
		for (index=0; index < old_nodes->num; index++){
			if (0 == strcmp(old_nodes->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (1 == diff_name){//��������޸��ˣ�����Ƿ��ظ��ͼ���Ƿ�����ʹ��
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
		if ( NULL == group){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
		if ( NULL == httpdirpolicys){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		//��������
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

		//HTTPS_DIR���Լ��
		ret_value = httpdir_policy_name_check(httpdirpolicys, name, old_name, 1);
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
	
	/*********************************�޸�����***********************************/	
	iserror = -1;
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, old_name)){	
			strcpy(old_nodes->pinfo[index].name, name);
			strcpy(old_nodes->pinfo[index].value, value);
			strcpy(old_nodes->pinfo[index].comment, comment);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSIONS,(void*)old_nodes, old_size);
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
		printf("new_nodes->pinfo[%d].name:%s\n",index,old_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n",index,old_nodes->pinfo[index].value);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,old_nodes->pinfo[index].comment);
	}
#endif
	
	/**************************�ɹ����ͷ��ڴ�***************************/

		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&group);
		free_xml_node((void*)&httpdirpolicys);
		return;
}

void extensions_remove_save(void)
{
	int old_size,size,index=0,result=0;
	int ret_value=0,iserror=0;
	int remove_num,old_num,new_num,new_size,new_index;
	char name[STRING_LENGTH]={"aa1"};

	struct mgtcgi_xml_extensions *old_nodes = NULL;
	struct mgtcgi_xml_extensions *new_nodes = NULL;
	//���������Ľṹ����
	struct mgtcgi_xml_group_array *group = NULL; //������
	struct mgtcgi_xml_http_dir_policys *httpdirpolicys = NULL;	
	
	cgiFormString("name",name,STRING_LENGTH);

	/******************************�������*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&old_nodes,&old_size);
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
	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//û���ҵ�Ҫɾ������Ŀ
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//��������޸��ˣ�����Ƿ��ظ��ͼ���Ƿ�����ʹ��
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//��������
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

	//HTTPS_DIR���Լ��
	ret_value = httpdir_policy_name_check(httpdirpolicys, name, name, 1);
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

	/***********************����ɾ���������½ṹ***************************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;

	old_size = sizeof(struct mgtcgi_xml_extensions) + 
				old_num *
				sizeof(struct mgtcgi_xml_extension_info);
	new_size = sizeof(struct mgtcgi_xml_extensions) + 
				new_num *
				sizeof(struct mgtcgi_xml_extension_info);

	new_nodes = (struct mgtcgi_xml_extensions *)malloc_buf(new_size);
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
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSIONS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n",index,old_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].value:%s\n",index,old_nodes->pinfo[index].value);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,old_nodes->pinfo[index].comment);
	}
#endif
	
	/**************************�ɹ����ͷ��ڴ�***************************/

		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&group);
		free_xml_node((void*)&httpdirpolicys);
		return;
}

void extensions_move(void)
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

	if (strcmp(submit_type,"list") == 0 ){//�б���ʾ����
		extensions_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//�������
		extensions_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		extensions_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//����"ȷ��"
		extensions_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		extensions_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		extensions_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//����
		extensions_move();
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
