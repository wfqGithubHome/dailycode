#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end);
extern int check_ipaddr_type(const char *ipaddr);
extern int check_ipaddr(const char *ipaddr);
extern int group_name_check(struct mgtcgi_xml_group_array *group, const char *name, const char *old_name);
extern int ipsession_name_check(struct mgtcgi_xml_ip_session_limits *ipsession,const char *name, const char *old_name, int check_id);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);
extern int portmirror_name_check(struct mgtcgi_xml_port_mirrors *portmirror,const char *name, const char *old_name, int check_id);
extern int dnspolicy_name_check(struct mgtcgi_xml_dns_policys *dnspolicy,const char *name, const char *old_name, int check_id);


void networks_list_show(void)
{
	int size=0,index=0,index_end=0,type=0;
	char ip_type[STRING_LENGTH];
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,iserror=0,result=0;
	struct mgtcgi_xml_networks * networks = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);

	if ((strlen(page_str) < STRING_LITTLE_LENGTH) || 
		 (strlen(pagesize_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	total = networks->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (; index < index_end ;){
		type=check_ipaddr_type(networks->pinfo[index].value);	//返回ip地址类型
		if( 1 == type){
			strcpy(ip_type,"host");
		}
		else if(2 == type){
			strcpy(ip_type,"subnet");
		}
		else if(3 == type){
			strcpy(ip_type,"range");
		}
		else{
			strcpy(ip_type,"undefine");
		}

		printf("{\"name\":\"%s\",\"ip_type\":\"%s\",\"value\":\"%s\"}",networks->pinfo[index].name,ip_type,networks->pinfo[index].value);

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
		free_xml_node((void*)&networks);
		return;
}

void networks_add_show(void)
{
	;
}

void networks_edit_show(void)
{

	int size=0,index=0,iptype=0,netbit=0,iserror=0;
	char ip_type[STRING_LENGTH];
	char name[STRING_LENGTH];
	char check_ipaddr[STRING_LENGTH];
	char ipaddr_head[STRING_LENGTH];
	char ipaddr_tail[STRING_LENGTH];
	struct mgtcgi_xml_networks * networks = NULL;

	cgiFormString("name",name,STRING_LENGTH);	

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

       printf("{\"data\":[");
	for (index = 0; index < networks->num ; index++){
		if (strcmp(name,networks->pinfo[index].name) == 0){
			iptype=check_ipaddr_type(networks->pinfo[index].value);	//返回ip地址类型
			if( 1 == iptype){
				strcpy(ip_type,"host");
				printf("{\"name\":\"%s\",\"ip_type\":\"%s\",\"ip_addr\":\"%s\"}",\
					networks->pinfo[index].name,ip_type,networks->pinfo[index].value);
			}
			else if(2 == iptype){
				strcpy(ip_type,"subnet");
				strcpy(check_ipaddr,networks->pinfo[index].value);

				strcpy(ipaddr_head, strtok(check_ipaddr, "/"));
				strcpy(ipaddr_tail, strtok(NULL, "/"));
				netbit=atoi(ipaddr_tail);
				
				printf("{\"name\":\"%s\",\"ip_type\":\"%s\",\"ip_addr\":\"%s\",\"netbit\":%d}",\
					networks->pinfo[index].name,ip_type,ipaddr_head,netbit);
			}
			else if(3 == iptype){
				strcpy(ip_type,"range");
				strcpy(check_ipaddr,networks->pinfo[index].value);
				strcpy(ipaddr_head, strtok(check_ipaddr, "-"));
				strcpy(ipaddr_tail, strtok(NULL, "-"));
				
				printf("{\"name\":\"%s\",\"ip_type\":\"%s\",\"start_ip\":\"%s\",\"end_ip\":\"%s\"}",
					networks->pinfo[index].name,ip_type,ipaddr_head,ipaddr_tail);
				
			}
			else{
				strcpy(ip_type,"undefine");
				printf("{\"name\":\"%s\",\"ip_type\":\"%s\",\"ip_addr\":\"%s\"}",\
					networks->pinfo[index].name,ip_type,networks->pinfo[index].value);
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
		free_xml_node((void*)&networks);
		return;
}

void networks_add_save(void)
{
	int old_size,new_size,size,index=0,iptype=0,iserror=0,result=0;
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char range[STRING_LENGTH];

	struct mgtcgi_xml_group_array *group = NULL;
	struct mgtcgi_xml_networks *old_networks = NULL;
	struct mgtcgi_xml_networks *new_networks = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) || 
		(strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if(check_ipaddr(value) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iptype=check_ipaddr_type(value); //返回ip地址类型
	if( 1 == iptype){	//host
		strcpy(range, "false");	
	}
	else if(2 == iptype){	//subnet
		strcpy(range, "false");
	}
	else if(3 == iptype){	//range
		strcpy(range, "true");	
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&old_networks,&old_size);
	if ( NULL == old_networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
		
	for (index=0; index < old_networks->num; index++){
		if (0 == strcmp(old_networks->pinfo[index].name, name)){
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
	old_size = sizeof(struct mgtcgi_xml_networks) + 
				old_networks->num *
				sizeof(struct mgtcgi_xml_network_info);
	new_size = sizeof(struct mgtcgi_xml_networks) + 
				(old_networks->num + 1) *
				sizeof(struct mgtcgi_xml_network_info);

	new_networks = (struct mgtcgi_xml_networks *)malloc_buf(new_size);
	if (NULL == new_networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_networks, old_networks, old_size);

	new_networks->num = old_networks->num + 1;
	strcpy(new_networks->pinfo[old_networks->num].name, name);
	strcpy(new_networks->pinfo[old_networks->num].value, value);
	strcpy(new_networks->pinfo[old_networks->num].range, range);
	strcpy(new_networks->pinfo[old_networks->num].comment, name);

	#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_networks->num; index++){
		printf("new_networks->pinfo[%d].name:%s\n\n",index,new_networks->pinfo[index].name);
		printf("new_networks->pinfo[%d].value:%s\n\n",index,new_networks->pinfo[index].value);
		printf("new_networks->pinfo[%d].range:%s\n\n",index,new_networks->pinfo[index].range);
		printf("new_networks->pinfo[%d].comment:%s\n\n",index,new_networks->pinfo[index].comment);
	}
	#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS,(void*)new_networks, new_size);
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
		free_malloc_buf((void*)&new_networks);
		free_xml_node((void*)&old_networks);
		free_xml_node((void*)&group);
		return;
}

void networks_edit_save(void)
{
	int old_size,size,index=0,iptype=0,iserror=0,result=0;
	int diff_name=0,ret_value=0;
	
	char old_name[STRING_LENGTH]={"zw"};
	char name[STRING_LENGTH]={"zw1"};
	char value[STRING_LENGTH]={"192.168.10.0/24"};
	char range[STRING_LENGTH];
	
	struct mgtcgi_xml_networks *old_networks = NULL;

	//ip地址对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //IP对象组
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//网络层策略
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL;//镜像策略
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS管控
	

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("value",value,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(value) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if(check_ipaddr(value) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iptype=check_ipaddr_type(value); //返回ip地址类型
	if( 1 == iptype){	//host
		strcpy(range, "false"); 
	}
	else if(2 == iptype){	//subnet
		strcpy(range, "false");
	}
	else if(3 == iptype){	//range
		strcpy(range, "true");	
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/*****************************目标位置检测*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&old_networks,&old_size);
	if ( NULL == old_networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < old_networks->num; index++){
			if (0 == strcmp(old_networks->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (1 == diff_name){
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);
		if ( NULL == group){
			iserror = MGTCGI_READ_FILE_ERR;
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

		//ip对象组检查
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

		//网络层策略检查
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

		//应用层策略检查
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

		//端口镜像策略检查
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

		//DNS策略检查
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


/*********************************修改数据***********************************/	
	iserror = -1;
	for (index=0; index < old_networks->num; index++){
		if (0 == strcmp(old_networks->pinfo[index].name, old_name)){	
			strcpy(old_networks->pinfo[index].name, name);
			strcpy(old_networks->pinfo[index].value, value);
			strcpy(old_networks->pinfo[index].range, range);
			strcpy(old_networks->pinfo[index].comment, name);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS,(void*)old_networks, old_size);
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
	for (index=0; index < old_networks->num; index++){
		printf("old_networks->pinfo[%d].name:%s\n\n",index,old_networks->pinfo[index].name);
		printf("old_networks->pinfo[%d].value:%s\n\n",index,old_networks->pinfo[index].value);
		printf("old_networks->pinfo[%d].range:%s\n\n",index,old_networks->pinfo[index].range);
		printf("old_networks->pinfo[%d].comment:%s\n\n",index,old_networks->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_networks);
		free_xml_node((void*)&group);
		free_xml_node((void*)&ipsessions);
		free_xml_node((void*)&firewalls);
		free_xml_node((void*)&portmirrors);
		free_xml_node((void*)&dnspolicys);
		return;
}

void networks_remove_save(void)
{

	int old_size,new_size,size,index=0,iserror,result=0,
		old_num,new_num,new_index;
	int ret_value=0,remove_num=0;
	
	char name[STRING_LENGTH]={"zw1"};
	
	struct mgtcgi_xml_networks *old_networks = NULL;
	struct mgtcgi_xml_networks *new_networks = NULL;

	//ip地址对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //IP对象组
	struct mgtcgi_xml_ip_session_limits *ipsessions = NULL;//网络层策略
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL;//镜像策略
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;//DNS管控
	
	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	/*****************************目标位置检测*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&old_networks,&old_size);
	if ( NULL == old_networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (old_networks->num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	remove_num = -1;
	for (index=0; index < old_networks->num; index++){
		if (0 == strcmp(old_networks->pinfo[index].name, name)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_networks->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
/*************************验证要删除的条目是否被使用*********************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
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

	//ip对象组检查
	ret_value = group_name_check(group, name, name);
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

	//网络层策略检查
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

	//应用层策略检查
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

	//端口镜像策略检查
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

	//DNS策略检查
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


/***********************可以删除，构建新结构***************************************/
	old_num = old_networks->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;
	old_size = sizeof(struct mgtcgi_xml_networks) + 
				old_num *
				sizeof(struct mgtcgi_xml_network_info);
	new_size = sizeof(struct mgtcgi_xml_networks) + 
				new_num *
				sizeof(struct mgtcgi_xml_network_info);

	new_networks = (struct mgtcgi_xml_networks *)malloc_buf(new_size);
	if (NULL == new_networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_networks->num = new_num;
	for (index=0,new_index=0; index < old_networks->num; index++){
		if ((remove_num != index) && (new_index < new_networks->num)){
			strcpy(new_networks->pinfo[new_index].name, old_networks->pinfo[index].name);
			strcpy(new_networks->pinfo[new_index].value, old_networks->pinfo[index].value);
			strcpy(new_networks->pinfo[new_index].range, old_networks->pinfo[index].range);
			strcpy(new_networks->pinfo[new_index].comment, old_networks->pinfo[index].comment);	
			new_index++;
		}
	}

	if (new_index == new_networks->num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS,(void*)new_networks, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = 7;
		goto ERROR_EXIT;
	}
	
#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < old_networks->num; index++){
		printf("old_networks->pinfo[%d].name:%s\n\n",index,old_networks->pinfo[index].name);
		printf("old_networks->pinfo[%d].value:%s\n\n",index,old_networks->pinfo[index].value);
		printf("old_networks->pinfo[%d].range:%s\n\n",index,old_networks->pinfo[index].range);
		printf("old_networks->pinfo[%d].comment:%s\n\n",index,old_networks->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_malloc_buf((void*)&new_networks);
		free_xml_node((void*)&old_networks);
		free_xml_node((void*)&group);
		free_xml_node((void*)&ipsessions);
		free_xml_node((void*)&firewalls);
		free_xml_node((void*)&portmirrors);
		free_xml_node((void*)&dnspolicys);
		return;
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
		networks_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		networks_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		networks_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		networks_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		networks_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		networks_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		;
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

