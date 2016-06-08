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
extern int check_list_string_dot(const char *string, const char *dot);
extern int groups_add_save(int mgtcgi_group_type, int add_includ_num,
						const char *name, const char *value, const char *comment);
extern int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
							const char *name, const char *old_name, const char *value, const char *comment);
extern int groups_remove_save(int mgtcgi_group_type, const char *name);

extern int httpdir_policy_name_check(struct mgtcgi_xml_http_dir_policys *httpdir_policy,
					const char *name, const char *old_name, int check_id);


void extensiongroups_list_show(void)
{
	int size=0;
	int total=0,iserror=0;
	struct mgtcgi_xml_group_array * group = NULL;

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror=MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	//��ӡ�����
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

void extensiongroups_add_show(void)
{
	int size=0,index=0,i=0;
	int left,use_flag,flag,iserror=0;
	struct mgtcgi_xml_extensions *extensions = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&extensions,&size);
	if ( NULL == extensions){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"left_list\":[");
	flag = 0;
	for (left=0; left < extensions->num; left++){
			use_flag = 0;
			for (index=0; index < group->num; index++){
				for (i=0; i < group->pinfo[index].num; i++){
					if (strcmp(group->pinfo[index].pinfo[i].name, extensions->pinfo[left].name) == 0){
						use_flag = 1;
						break;
					}
				}
				if (use_flag == 1)
					break;
			}
	
			if (use_flag == 0){
				if (flag == 0){
					printf("\"%s\"",extensions->pinfo[left].name);
					flag = 1;
				} else {
					printf(",\"%s\"",extensions->pinfo[left].name);
				}
			}
		}

		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&extensions);
		free_xml_node((void*)&group);
		return;
}

void extensiongroups_edit_show(void)
{
	int size=0,index=0,left=0,i=0;
	int flag,use_flag=0,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_extensions * extensions = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	

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

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	flag = 0;
	printf("{\"data\":[{\"name\":\"%s\"}],\"left_list\":[",name);

	for (left=0; left < extensions->num; left++){
		use_flag = 0;
		for (index=0; index < group->num; index++){
			for (i=0; i < group->pinfo[index].num; i++){
				if (strcmp(group->pinfo[index].pinfo[i].name, extensions->pinfo[left].name) == 0){
					use_flag = 1;
					break;
				}
			}
			if (use_flag == 1)
				break;
		}

		if (use_flag == 0){
			if (flag == 0){
				printf("\"%s\"",extensions->pinfo[left].name);
				flag = 1;
			} else {
				printf(",\"%s\"",extensions->pinfo[left].name);
			}
		}
	}
	printf("],");

	printf("\"right_list\":[");
	for (index=0; index < group->num; index++){
		flag = 0;
		if (strcmp(group->pinfo[index].group_name, name) == 0){
			for(i=0; i < group->pinfo[index].num; i++){
				if (flag == 0) {
					printf("\"%s\"",group->pinfo[index].pinfo[i].name);
					flag = 1;
				} else {
					printf(",\"%s\"",group->pinfo[index].pinfo[i].name);
				}
			}
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
		free_xml_node((void*)&group);
		return;
}

void extensiongroups_add_save(void)
{
	int size,index=0,add_includ_num=0,iserror=0;
	char name[STRING_LENGTH]={"test"};
	//char value[STRING_LIST_LENGTH]={"aa,bb,cc"};
	char * value = NULL;
	char *dot=",";
	//char str[STRING_LIST_LENGTH];
	char *str = NULL;
	char *ptr;
	struct mgtcgi_xml_extensions *obj_nodes = NULL;
    value = (char *)malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!value)
        goto ERROR_EXIT;
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH*sizeof(char));

	/******************************�������*****************************/
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

			
	/******************************�ظ��Լ��*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//���������������
	for (index=0; index < obj_nodes->num; index++){
		if (0 == strcmp(obj_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}		
	}
    str = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!str)
        goto ERROR_EXIT;
	//value��飬�����Ƕ�����ڵ����ơ�
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

	/************************�ͷ��ڴ棬�����޸�xml�ļ�����*************************/
	groups_add_save(MGTCGI_TYPE_EXTENSION_GROUP,
					add_includ_num, name, value, "");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);
    if(value)
        free(value);
    if(str)
        free(str);
	return;
}

void extensiongroups_edit_save(void)
{

	int size,index=0,ret_value=0,add_includ_num=0;
 	int diff_name=0,iserror=0;
 	char old_name[STRING_LENGTH]={"bb"};
	char name[STRING_LENGTH]={"bb1"};
	//char value[STRING_LIST_LENGTH]={"exe,txt"};
    char * value=NULL;
    char *dot=",";
	//char str[STRING_LIST_LENGTH];
	char *str = NULL;
	char *ptr;
	struct mgtcgi_xml_extensions *obj_nodes = NULL;
	//��֮�����Ľṹ����
	struct mgtcgi_xml_http_dir_policys *httpdirpolicys = NULL;	
    value = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!value)
        goto ERROR_EXIT;
	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH*sizeof(char));

	/******************************�������*****************************/
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
			
	/******************************�ظ��Լ��*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_EXTENSIONS,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    str = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!str)
        goto ERROR_EXIT;
	//value��飬�����Ƕ�����ڵ����ơ�
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
		diff_name = 0;//����û�б��޸�
	} else { 
		diff_name = 1; //�����޸��ˣ�����Ƿ��ظ�
		//���������������
		for (index=0; index < obj_nodes->num; index++){
			if (0 == strcmp(obj_nodes->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}		
		}
	}
	
	if (1 == diff_name){
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
		if ( NULL == httpdirpolicys){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
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
    free(str);
    str = NULL;
	/*************************�ͷ��ڴ棬���ñ��溯��*************************/
	ret_value = groups_edit_save(MGTCGI_TYPE_EXTENSION_GROUP,
						add_includ_num, diff_name, name, old_name, value, "");
	
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);
	free_xml_node((void*)&httpdirpolicys);
    if(value)
        free(value);
    if(str)
        free(str);
    return;
}

void extensiongroups_remove_save(void)
{
	int size,ret_value=0,iserror=0;
	char name[STRING_LENGTH]={"bb1"};
	//��֮�����Ľṹ����
	struct mgtcgi_xml_http_dir_policys *httpdirpolicys = NULL;	

	cgiFormString("name",name,STRING_LENGTH);


	/******************************�������*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
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

	/*************************�ͷ��ڴ棬���ñ��溯��*************************/
	ret_value = groups_remove_save(MGTCGI_TYPE_EXTENSION_GROUP,name);

		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&httpdirpolicys);
		return;
}

void extensiongroups_move(void)
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
	system(cmd);

	cgiFormString("submit",submit_type,STRING_LENGTH);
	
	if (strcmp(submit_type,"list") == 0 ){//�б���ʾ����
		extensiongroups_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//������
		extensiongroups_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		extensiongroups_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//���"ȷ��"
		extensiongroups_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		extensiongroups_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		extensiongroups_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//����
		extensiongroups_move();
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

