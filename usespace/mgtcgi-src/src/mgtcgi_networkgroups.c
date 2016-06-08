#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void print_groups_list(struct mgtcgi_xml_group_array * group);
extern int check_list_string_dot(const char *string, const char *dot);
extern int groups_add_save(int mgtcgi_group_type, int add_includ_num,
	const char *name, const char *value, const char *comment);
extern int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
	const char *name, const char *old_name, const char *value, const char *comment);
extern int groups_remove_save(int mgtcgi_group_type, const char *name);

extern int group_name_check(struct mgtcgi_xml_group_array *group, const char *name, const char *old_name);
extern int ipsession_name_check(struct mgtcgi_xml_ip_session_limits *ipsession,const char *name, const char *old_name, int check_id);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);
extern int portmirror_name_check(struct mgtcgi_xml_port_mirrors *portmirror,const char *name, const char *old_name, int check_id);
extern int dnspolicy_name_check(struct mgtcgi_xml_dns_policys *dnspolicy,const char *name, const char *old_name, int check_id);


void networkgroups_list_show(void)
{
	int size=0,iserror=0,total=0;
	struct mgtcgi_xml_group_array *group = NULL;	
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
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

void networkgroups_add_show(void)
{
	int size=0,index=0;
	int total=0,iserror=0;
	struct mgtcgi_xml_networks * networks = NULL;
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
		
	total = networks->num;
	printf("{\"left_list\":[");
	for (index=0; index < total;){
		printf("\"%s\"",networks->pinfo[index].name);
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
	free_xml_node((void*)&networks);
	return;
}

void networkgroups_edit_show(void)
{
	int size=0,index=0,left=0,flag=0,iserror=0,i=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * group = NULL;	

	cgiFormString("name",name,STRING_LENGTH);

	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	flag = 0;
	printf("{\"data\":[{\"name\":\"%s\"}],\"left_list\":[",name);
	for (index=0; index < group->num; index++){
		if (strcmp(name,group->pinfo[index].group_name) == 0){
			for (left=0; left < networks->num; left++){
				for(i=0; i < group->pinfo[index].num;){
					if (0 == strcmp(networks->pinfo[left].name, group->pinfo[index].pinfo[i].name))
						break;
					else
						i++;
				} 
				if (0 == flag){
					if (i == group->pinfo[index].num){
						printf("\"%s\"",networks->pinfo[left].name);
						flag = 1;
					}
				}
				else{
					if (i == group->pinfo[index].num){
						printf(",\"%s\"",networks->pinfo[left].name);
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
		free_xml_node((void*)&networks);
		free_xml_node((void*)&group);
		return;
}

void networkgroups_add_save(void)
{
	int size,index=0,ret_value=0,add_includ_num=0,iserror=0;
	char name[STRING_LENGTH]={"test"};
	//char value[STRING_LIST_LENGTH]={"ros1,ros2"};
	char * value = NULL;
	//char str[STRING_LIST_LENGTH];
	char * str = NULL;
	char *ptr;
	char *dot=",";
	struct mgtcgi_xml_networks *obj_nodes = NULL;
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
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&obj_nodes,&size);
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
    str=NULL;
	/*************************�ͷ��ڴ棬���ñ��溯��*************************/
	ret_value = groups_add_save(MGTCGI_TYPE_NETWORKS_GROUP,
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

void networkgroups_edit_save(void)
{
	int size,index=0,ret_value=0,add_includ_num=0;
	int diff_name=0,iserror=0;
	char old_name[STRING_LENGTH]={"bb"};
	char name[STRING_LENGTH]={"bb1"};
	//char value[STRING_LIST_LENGTH]={"ros1,ip"};
	char * value = NULL;
	//char str[STRING_LIST_LENGTH];
	char *str = NULL;
	char *ptr;
	char *dot=",";
	struct mgtcgi_xml_networks *obj_nodes = NULL;
	//�����������Ľṹ����
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//��������
	struct mgtcgi_xml_firewalls *firewalls = NULL; //Ӧ�ò����
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL;//�������
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS�ܿ�

    value = (char *) malloc (STRING_LIST_LENGTH * sizeof(char));
    if(!value)
        goto ERROR_EXIT;

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LIST_LENGTH * sizeof(char));

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

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&obj_nodes,&size);
	if ( NULL == obj_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    str = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!str){
        goto ERROR_EXIT;
    }

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
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&portmirrors,&size);
		if ( NULL == portmirrors){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
			if ( NULL == dnspolicys){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		//�������Լ��
		ret_value = ipsession_name_check(ipsessions, name, old_name, 1);
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

		//Ӧ�ò���Լ��
		ret_value = firewall_name_check(firewalls, name, old_name, 1);
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

		//�˿ھ�����Լ��
		ret_value = portmirror_name_check(portmirrors, name, old_name, 1);
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

		//DNS���Լ��
		ret_value = dnspolicy_name_check(dnspolicys, name, old_name, 1);
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
	
	ret_value = groups_edit_save(MGTCGI_TYPE_NETWORKS_GROUP,
						add_includ_num, diff_name, name, old_name, value, "");

	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&obj_nodes);
	free_xml_node((void*)&ipsessions);
	free_xml_node((void*)&firewalls);
	free_xml_node((void*)&portmirrors);
	free_xml_node((void*)&dnspolicys);
    if(value)
        free(value);
    if(str)
        free(str);
	return;
}

void networkgroups_remove_save(void)
{
	int size,ret_value=0;
	int iserror=0;
	char name[STRING_LENGTH]={"bb1"};

	struct mgtcgi_xml_networks *obj_nodes = NULL;
	//�����������Ľṹ����
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//��������
	struct mgtcgi_xml_firewalls *firewalls = NULL; //Ӧ�ò����
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL;//�������
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS�ܿ�

	cgiFormString("name",name,STRING_LENGTH);

	/******************************�������*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
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
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&portmirrors,&size);
	if ( NULL == portmirrors){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
	if ( NULL == dnspolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//�������Լ��
	ret_value = ipsession_name_check(ipsessions, name, name, 1);
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

	//Ӧ�ò���Լ��
	ret_value = firewall_name_check(firewalls, name, name, 1);
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

	//�˿ھ�����Լ��
	ret_value = portmirror_name_check(portmirrors, name, name, 1);
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

	//DNS���Լ��
	ret_value = dnspolicy_name_check(dnspolicys, name, name, 1);
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
	
	ret_value = groups_remove_save(MGTCGI_TYPE_NETWORKS_GROUP, name);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&obj_nodes);
		free_xml_node((void*)&ipsessions);
		free_xml_node((void*)&firewalls);
		free_xml_node((void*)&portmirrors);
		free_xml_node((void*)&dnspolicys);
		return;
}

void networks_move(void)
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
		networkgroups_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//������
		networkgroups_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		networkgroups_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//���"ȷ��"
		networkgroups_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		networkgroups_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		networkgroups_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//����
		networks_move();
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

