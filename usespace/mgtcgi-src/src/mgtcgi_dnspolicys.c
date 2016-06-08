#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
extern int get_bridge_num(void);

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern void print_ipaddrs_list(
				struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups);
extern void print_domains_list(
				struct mgtcgi_xml_domains * domains,
				struct mgtcgi_xml_group_array * domaingroups);
extern int check_ipaddr_type(const char *ipaddr);



void dnspolicys_list_show(void)
{
	int size=0,index=0,index_end=0,flag=0,iserror=0;
	int page_start=0,page_index=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	int total=0,page=0,pagesize=0,bridge=0,bridge_num=0,enable=0;
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;	

	cgiFormString("page",page_str,DIGITAL_LENGTH);
	cgiFormString("pagesize",pagesize_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
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

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
	if ( NULL == dnspolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	page_start = ((page * pagesize) - pagesize);
	if (page_start > dnspolicys->num){
		page_start = dnspolicys->num;
	}
	index_end = dnspolicys->num;

	flag = 0;
	total = 0;
	page_index =0;
	bridge_num = 2;	// 桥数量要根据数据接口设置的桥信息确定
	
	printf("{\"data\":[");
	for (; index < index_end; index++){
		
		if (bridge == dnspolicys->pinfo[index].bridge){
			total++;
			if ((total > page_start) && (page_index < pagesize)){
				page_index++;
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("{\"name\":%d,\"domain_name\":\"%s\",\"bridge\":%d,\
	\"src\":\"%s\",\"dst\":\"%s\",\"action\":%d,\"targetip\":\"%s\",\
	\"disabled\":%d,\"comment\":\"%s\"}",\
					dnspolicys->pinfo[index].name,dnspolicys->pinfo[index].domain_name,\
					dnspolicys->pinfo[index].bridge,dnspolicys->pinfo[index].src,\
					dnspolicys->pinfo[index].dst,dnspolicys->pinfo[index].action,\
					dnspolicys->pinfo[index].targetip,dnspolicys->pinfo[index].disabled,\
					dnspolicys->pinfo[index].comment);
			}
		}
	}

	enable = dnspolicys->application_policy;
		bridge_num = get_bridge_num();
		printf("],\"total\":%d,\"enable\":%d,\"bridge_num\":%d,\"iserror\":0,\"msg\":\"\"}",total,enable,bridge_num);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&dnspolicys);
		return;
}

void dnspolicys_add_show(void)
{
	int size=0,index=0,flag=0,iptype,iserror=0;
	struct mgtcgi_xml_networks *networks = NULL;
	struct mgtcgi_xml_group_array *networkgroups = NULL;	
	struct mgtcgi_xml_domains *domains = NULL;
	struct mgtcgi_xml_group_array *domaingroups = NULL;	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&networkgroups,&size);
	if ( NULL == networkgroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&domains,&size);
	if ( NULL == domains){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&domaingroups,&size);
	if ( NULL == domaingroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印DNS管控策略默认值
	printf("{\"data\":[{\"domain_name\":\"any\",\"src\":\"any\",\"dst\":\"any\",\
\"action\":0,\"targetip\":\"\",\"disabled\":0}],");


	//json格式打印所有列表数组
	print_ipaddrs_list(networks, networkgroups);
	printf(",");
	print_domains_list(domains, domaingroups);
	printf(",");

	//host类型的ip地址
	flag = 0;
	printf("\"hostiplist\":[");
	for (index=0; index < networks->num; index++){
		iptype=check_ipaddr_type(networks->pinfo[index].value);	//返回ip地址类型
		if( 1 == iptype){
			if (0 == flag){
				printf("\"%s\"",networks->pinfo[index].name);
				flag = 1;
			}else{
				printf(",\"%s\"",networks->pinfo[index].name);
			}
		}	
		
	}
	printf("],");

/******************************释放分配的内存********************************/	
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&domains);
		free_xml_node((void*)&domaingroups);
		return;
}

void dnspolicys_edit_show(void)
{
	int size=0,index=0,flag=0,iptype,iserror=0;
	char bridge_str[DIGITAL_LENGTH];
	char name_str[DIGITAL_LENGTH];
	int bridge=0,name=0;
	struct mgtcgi_xml_dns_policys *dnspolicys = NULL;
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;	
	struct mgtcgi_xml_domains * domains = NULL;
	struct mgtcgi_xml_group_array * domaingroups = NULL;	

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
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);
	if ( NULL == dnspolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < dnspolicys->num; index++){
		if ((bridge == dnspolicys->pinfo[index].bridge)
			&& (name == dnspolicys->pinfo[index].name)){	
			printf("{\"name\":%d,\"domain_name\":\"%s\",\"bridge\":%d,\
\"src\":\"%s\",\"dst\":\"%s\",\"action\":%d,\"targetip\":\"%s\",\
\"disabled\":%d,\"comment\":\"%s\"}",\
				dnspolicys->pinfo[index].name,dnspolicys->pinfo[index].domain_name,\
				dnspolicys->pinfo[index].bridge,dnspolicys->pinfo[index].src,\
				dnspolicys->pinfo[index].dst,dnspolicys->pinfo[index].action,\
				dnspolicys->pinfo[index].targetip,dnspolicys->pinfo[index].disabled,\
				dnspolicys->pinfo[index].comment);
			
			break;
		}
	}
	printf("],");

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&networkgroups,&size);
	if ( NULL == networkgroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_DOMAINS,(void**)&domains,&size);
	if ( NULL == domains){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&domaingroups,&size);
	if ( NULL == domaingroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}


	//json格式打印所有列表数组
	print_ipaddrs_list(networks, networkgroups);
	printf(",");
	print_domains_list(domains, domaingroups);
	printf(",");

	//host类型的ip地址
	flag = 0;
	printf("\"hostiplist\":[");
	for (index=0; index < networks->num; index++){
		iptype=check_ipaddr_type(networks->pinfo[index].value);	//返回ip地址类型
		if( 1 == iptype){
			if (0 == flag){
				printf("\"%s\"",networks->pinfo[index].name);
				flag = 1;
			}else{
				printf(",\"%s\"",networks->pinfo[index].name);
			}
		}	
		
	}
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&dnspolicys);
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&domains);
		free_xml_node((void*)&domaingroups);
		return;
}

void dnspolicys_add_save(void)
{
	int index,old_size,new_size,iserror=0,result=0;
	int old_num=0,new_num=0;
	int disabled,bridge,name,action;
	int max_id,min_id;

	char domain_name[STRING_LENGTH]={"any"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char src[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char action_str[DIGITAL_LENGTH]={"1"};
	char targetip[STRING_LENGTH]={"ros1"};
	char comment[STRING_LENGTH]={"test"};
	char disabled_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_dns_policys *old_nodes = NULL; 
	struct mgtcgi_xml_dns_policys *new_nodes = NULL; 

	//注释的部分使用默认值
	cgiFormString("domain_name",domain_name,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("action",action_str,STRING_LENGTH);
	cgiFormString("targetip",targetip,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(domain_name) < STRING_LITTLE_LENGTH)	
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(src) < STRING_LITTLE_LENGTH)
		|| (strlen(dst) < STRING_LITTLE_LENGTH)
		|| (strlen(action_str) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strcmp(action_str, "2") != 0){ // 2 丢弃
		if (strlen(targetip) < STRING_LITTLE_LENGTH){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	action = atoi(action_str);

	if (!((0 <= disabled && disabled <= 1) &&
		(0 <= bridge && bridge <= 256) &&
		(0 <= action && action <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_dns_policys) + 
			old_num * 
				sizeof(struct mgtcgi_xml_dns_policy_info);
	new_size = sizeof(struct mgtcgi_xml_dns_policys) + 
			new_num * 
				sizeof(struct mgtcgi_xml_dns_policy_info);

	new_nodes = (struct mgtcgi_xml_dns_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//name计算
	max_id = -1;
	min_id = -1;
	for (index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].bridge == bridge){
			max_id = old_nodes->pinfo[index].name;
			if (max_id < min_id)
				max_id = min_id;
			else
				min_id = max_id;
		}	
	}

	//增加一条记录
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	name = max_id + 1;

	new_nodes->pinfo[old_num].name = name;
	new_nodes->pinfo[old_num].disabled = disabled;
	new_nodes->pinfo[old_num].bridge = bridge;
	new_nodes->pinfo[old_num].action = action;
	strcpy(new_nodes->pinfo[old_num].domain_name, domain_name);
	strcpy(new_nodes->pinfo[old_num].src, src);
	strcpy(new_nodes->pinfo[old_num].dst, dst);
	strcpy(new_nodes->pinfo[old_num].targetip, targetip);
	strcpy(new_nodes->pinfo[old_num].comment, comment);

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].action:%d\n",index,new_nodes->pinfo[index].action);
		
		printf("new_nodes->pinfo[%d].domain_name:%s\n",index,new_nodes->pinfo[index].domain_name);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].targetip:%s\n",index,new_nodes->pinfo[index].targetip);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void*)new_nodes, new_size);
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
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&new_nodes);
		return;
}

void dnspolicys_edit_save(void)
{
	int index,new_size,iserror=0,result=0;
	int disabled,bridge,name,action;
	char name_str[DIGITAL_LENGTH]={"2"};
	char domain_name[STRING_LENGTH]={"any"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char src[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char action_str[DIGITAL_LENGTH]={"1"};
	char targetip[STRING_LENGTH]={"ros2"};
	char comment[STRING_LENGTH]={"test"};
	char disabled_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_dns_policys *new_nodes = NULL; 

	//注释的部分使用默认值
	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("domain_name",domain_name,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("action",action_str,DIGITAL_LENGTH);
	cgiFormString("targetip",targetip,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(domain_name) < STRING_LITTLE_LENGTH)	
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(src) < STRING_LITTLE_LENGTH)
		|| (strlen(dst) < STRING_LITTLE_LENGTH)
		|| (strlen(action_str) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strcmp(action_str, "2") != 0){ // 2 丢弃
		if (strlen(targetip) < STRING_LITTLE_LENGTH){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	action = atoi(action_str);
	name = atoi(name_str);

	if (!((0 <= disabled && disabled <= 1) &&
		(0 <= bridge && bridge <= 256) &&
		(0 <= action && action <= 256) &&
		(0 <= name && name <= 65535))){		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	iserror = -1;
	for (index=0; index < new_nodes->num; index++){
		if ((new_nodes->pinfo[index].name == name) &&
			(new_nodes->pinfo[index].bridge == bridge)){
			new_nodes->pinfo[index].name = name;
			new_nodes->pinfo[index].disabled = disabled;
			new_nodes->pinfo[index].bridge = bridge;
			new_nodes->pinfo[index].action = action;
			strcpy(new_nodes->pinfo[index].domain_name, domain_name);
			strcpy(new_nodes->pinfo[index].src, src);
			strcpy(new_nodes->pinfo[index].dst, dst);
			strcpy(new_nodes->pinfo[index].targetip, targetip);
			strcpy(new_nodes->pinfo[index].comment, comment);

			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void*)new_nodes, new_size);
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
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].action:%d\n",index,new_nodes->pinfo[index].action);
		
		printf("new_nodes->pinfo[%d].domain_name:%s\n",index,new_nodes->pinfo[index].domain_name);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].targetip:%s\n",index,new_nodes->pinfo[index].targetip);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
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

void dnspolicys_remove_save(void)
{
	int index,old_size,new_size;
	int bridge,name,iserror=0,result=0;
	int remove_num,old_num,new_num,new_index,name_id=-1;
	char name_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_dns_policys *new_nodes = NULL; 
	struct mgtcgi_xml_dns_policys *old_nodes = NULL;

	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
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
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&old_nodes,&old_size);
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
		if ((old_nodes->pinfo[index].name == name) &&
			(old_nodes->pinfo[index].bridge == bridge)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/***********************可以删除，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;

	old_size = sizeof(struct mgtcgi_xml_dns_policys) + 
			old_num * 
				sizeof(struct mgtcgi_xml_dns_policy_info);
	new_size = sizeof(struct mgtcgi_xml_dns_policys) + 
			new_num * 
				sizeof(struct mgtcgi_xml_dns_policy_info);

	new_nodes = (struct mgtcgi_xml_dns_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->num = new_num;
	new_nodes->application_policy = old_nodes->application_policy;

	for (index=0,new_index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].bridge == bridge){
			if (old_nodes->pinfo[index].name == name){
				continue;
			} else {
				if (-1 == name_id)
					name_id = 0;
				else 
					name_id++;	
			}
			new_nodes->pinfo[new_index].name = name_id;
		
		} else {
			new_nodes->pinfo[new_index].name = old_nodes->pinfo[index].name;
		}

		new_nodes->pinfo[new_index].disabled = old_nodes->pinfo[index].disabled;
		new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[index].bridge;
		new_nodes->pinfo[new_index].action = old_nodes->pinfo[index].action;
		strcpy(new_nodes->pinfo[new_index].domain_name, old_nodes->pinfo[index].domain_name);
		strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src);
		strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst);
		strcpy(new_nodes->pinfo[new_index].targetip, old_nodes->pinfo[index].targetip);
		strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);

		new_index++;
	}

	if (new_index == new_nodes->num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].action:%d\n",index,new_nodes->pinfo[index].action);
		
		printf("new_nodes->pinfo[%d].domain_name:%s\n",index,new_nodes->pinfo[index].domain_name);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].targetip:%s\n",index,new_nodes->pinfo[index].targetip);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
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

void dnspolicys_move(void)
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
		dnspolicys_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		dnspolicys_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		dnspolicys_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		dnspolicys_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		dnspolicys_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		dnspolicys_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		dnspolicys_move();
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

