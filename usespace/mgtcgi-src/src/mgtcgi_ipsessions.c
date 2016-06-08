#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern void ipsessions_move_save(void);
extern void ipsessions_add_edit_list(
		struct mgtcgi_xml_networks *networks,
		struct mgtcgi_xml_group_array *networkgroups,
		struct mgtcgi_xml_session_limits *session_limits);
extern int get_bridge_num(void);

void ipsessions_list_show(void)
{
	int size=0,index=0,index_end=0,flag=0;
	int page_start=0,page_index=0,iserror=0;
	char page_str[DIGITAL_LENGTH]={"1"};
	char pagesize_str[DIGITAL_LENGTH]={"10"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	int total=0,page=0,pagesize=0,bridge=0,bridge_num=0;
	struct mgtcgi_xml_ip_session_limits * ipsession = NULL;	

	cgiFormString("page",page_str,DIGITAL_LENGTH);
	cgiFormString("pagesize",pagesize_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	
	if ((strlen(page_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(pagesize_str) < STRING_LITTLE_LENGTH)
		||(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
	if ((bridge < BRIDGE_LITTLE_VALUE) ||
		(page < PAGE_LITTLE_VALUE) || 
		(pagesize < PAGE_LITTLE_VALUE)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);
	if ( NULL == ipsession){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	page_start = ((page * pagesize) - pagesize);
	if (page_start > ipsession->global_limit_num){
		page_start = ipsession->global_limit_num;
	}
	index_end = ipsession->global_limit_num;

	flag = 0;
	total = 0;
	page_index =0;
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		
		if (bridge == ipsession->p_globalinfo[index].bridge){
			total++;
			if ((total > page_start) && (page_index < pagesize)){ 
				page_index++;
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\",\
\"per_ip_session_limit\":\"%s\",\"total_session_limit\":\"%s\",\"httplog\":%d,\
\"sesslog\":%d,\"httpdirpolicy\":%d,\"dnspolicy\":%d,\"action\":\"%s\",\
\"bridge\":%d,\"comment\":\"%s\"}",\
					ipsession->p_globalinfo[index].name,ipsession->p_globalinfo[index].addr,\
					ipsession->p_globalinfo[index].dst,\
					ipsession->p_globalinfo[index].per_ip_session_limit,\
					ipsession->p_globalinfo[index].total_session_limit,\
					ipsession->p_globalinfo[index].httplog,ipsession->p_globalinfo[index].sesslog,\
					ipsession->p_globalinfo[index].httpdirpolicy,\
					ipsession->p_globalinfo[index].dnspolicy,ipsession->p_globalinfo[index].action,\
					ipsession->p_globalinfo[index].bridge,ipsession->p_globalinfo[index].comment);
			}
		}
	}
		bridge_num = get_bridge_num();
		printf("],\"total\":%d,\"bridge_num\":%d,\"iserror\":0,\"msg\":\"\"}",total,bridge_num);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&ipsession);
		return;
}

void ipsessions_add_show(void)
{
	int size=0,iserror=0;
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_session_limits * session_limits = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;	

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

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
	if ( NULL == session_limits){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印网络层策略默认数据
	printf("{\"data\":[{\"src\":\"any\",\"dst\":\"any\",\"action\":\"trusted\",\
\"per_ip_session_limit\":\"0\",\"total_session_limit\":\"0\",\"sesslog\":0,\
\"httpdirpolicy\":0,\"dnspolicy\":0}],");

	//json格式打印所有列表数组
	ipsessions_add_edit_list(networks, networkgroups, session_limits);

/*********************************释放分配的内存********************************/	
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&session_limits);
		return;
}

void ipsessions_edit_show(void)
{
	int size=0,index=0,name=0,bridge=0,iserror=0;
	char name_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	struct mgtcgi_xml_ip_session_limits *ipsession = NULL; 
	struct mgtcgi_xml_networks *networks = NULL;
	struct mgtcgi_xml_session_limits *session_limits = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;	

	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	if ((strlen(name_str) < 1) || 
		(strlen(bridge_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	name = atoi(name_str);
	bridge = atoi(bridge_str);
	if (!((0 <= name && name <= 65535) ||
		 (0 <= bridge && bridge <= 255))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);
	if ( NULL == ipsession){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (; index < ipsession->global_limit_num; index++){
		if ((bridge == ipsession->p_globalinfo[index].bridge)
			&& (name == ipsession->p_globalinfo[index].name)){
			printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\",\
\"per_ip_session_limit\":\"%s\",\"total_session_limit\":\"%s\",\"httplog\":%d,\
\"sesslog\":%d,\"httpdirpolicy\":%d,\"dnspolicy\":%d,\"action\":\"%s\",\
\"bridge\":%d,\"comment\":\"%s\"}",\
				ipsession->p_globalinfo[index].name,ipsession->p_globalinfo[index].addr,\
				ipsession->p_globalinfo[index].dst,\
				ipsession->p_globalinfo[index].per_ip_session_limit,\
				ipsession->p_globalinfo[index].total_session_limit,\
				ipsession->p_globalinfo[index].httplog,ipsession->p_globalinfo[index].sesslog,\
				ipsession->p_globalinfo[index].httpdirpolicy,\
				ipsession->p_globalinfo[index].dnspolicy,ipsession->p_globalinfo[index].action,\
				ipsession->p_globalinfo[index].bridge,ipsession->p_globalinfo[index].comment);

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

	//会话数节点
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
	if ( NULL == session_limits){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//json格式打印所有列表数组
	ipsessions_add_edit_list(networks, networkgroups, session_limits);

/*********************************释放分配的内存********************************/	
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&ipsession);
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&session_limits);
		return;
}

void ipsessions_add_save(void)
{
	int index,old_size,new_size,size,iserror=0,result=0;
	int old_total_num=0,old_global_num=0,
		new_total_num=0,new_global_num=0;
	int name,httplog,sesslog,httpdirpolicy,dnspolicy,bridge;
	int max_id,min_id;
	unsigned long int perip_limit=0,total_limit=0;
	char addr[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char per_ip_session_limit[STRING_LENGTH]={"0"};
	char total_session_limit[STRING_LENGTH]={"0"};
	char httplog_str[DIGITAL_LENGTH];
	char sesslog_str[DIGITAL_LENGTH];
	char httpdirpolicy_str[DIGITAL_LENGTH];
	char dnspolicy_str[DIGITAL_LENGTH];
	char action[STRING_LENGTH]={"trusted"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char comment[STRING_LENGTH]={"test"};
	struct mgtcgi_xml_session_limits * session_limits = NULL;
	struct mgtcgi_xml_ip_session_limits * old_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limits * new_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limit_total_info *p_totalinfo_start = NULL;
	struct mgtcgi_xml_ip_session_limit_global_info *p_globalinfo_start = NULL;

	cgiFormString("addr",addr,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("per_ip_session_limit",per_ip_session_limit,STRING_LENGTH);
	cgiFormString("total_session_limit",total_session_limit,STRING_LENGTH);
	cgiFormString("httplog",httplog_str,DIGITAL_LENGTH);
	cgiFormString("sesslog",sesslog_str,DIGITAL_LENGTH);
	cgiFormString("httpdirpolicy",httpdirpolicy_str,DIGITAL_LENGTH);
	cgiFormString("dnspolicy",dnspolicy_str,DIGITAL_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(per_ip_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(total_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(per_ip_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(httplog_str) < STRING_LITTLE_LENGTH)
		|| (strlen(sesslog_str) < STRING_LITTLE_LENGTH)
		|| (strlen(dnspolicy_str) < STRING_LITTLE_LENGTH)
		|| (strlen(action) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	httplog = atoi(httplog_str);
	sesslog = atoi(sesslog_str);
	httpdirpolicy = atoi(httpdirpolicy_str);
	dnspolicy = atoi(dnspolicy_str);
	bridge = atoi(bridge_str);
	if (!((0 <= httplog && httplog <= 1) && 
		  (0 <= sesslog && sesslog <= 1) && 
		  (0 <= httpdirpolicy && httpdirpolicy <= 1) &&
		  (0 <= dnspolicy && dnspolicy <= 1) &&
		  (0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//单个会话数不能大于总会话数检查
	if (strcmp(per_ip_session_limit, "0") == 0){ //单会话数不限制，总会话数也不能限制
		if (strcmp(total_session_limit, "0") != 0){
			iserror = MGTCGI_PER_SESSION_ERR;
			goto ERROR_EXIT;
		}
		
	} else if (strcmp(total_session_limit, "0") != 0){//单会话数限制了，总会话数也选择了限制，检查
		get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
		if ( NULL == session_limits){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}	

		for (index=0; index < session_limits->num; index++){
			if (strcmp(session_limits->pinfo[index].name, per_ip_session_limit) == 0)	
				perip_limit = strtoul(session_limits->pinfo[index].value, 0, 10);
			
			if (strcmp(session_limits->pinfo[index].name, total_session_limit) == 0)	
				total_limit = strtoul(session_limits->pinfo[index].value, 0, 10);				
		}
		if (perip_limit > total_limit){
			iserror = MGTCGI_PER_SESSION_ERR;
			goto ERROR_EXIT;
		}
	}
	
	/******************************无重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
		
	old_total_num = old_nodes->total_limit_num;
	old_global_num = old_nodes->global_limit_num;	//网络层
	new_total_num = old_total_num;
	new_global_num = old_global_num + 1;
	
	old_size = sizeof(struct mgtcgi_xml_ip_session_limits) + 
			old_total_num * 
				sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
			old_global_num * 
				sizeof(struct mgtcgi_xml_ip_session_limit_global_info);
	new_size = sizeof(struct mgtcgi_xml_ip_session_limits) + 
		new_total_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
		new_global_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_global_info);

	new_nodes = (struct mgtcgi_xml_ip_session_limits *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//name计算
	max_id = -1;
	min_id = -1;
	for (index=0; index < old_nodes->global_limit_num; index++){
		if (old_nodes->p_globalinfo[index].bridge == bridge){
			max_id = old_nodes->p_globalinfo[index].name;
			if (max_id < min_id)
				max_id = min_id;
			else
				min_id = max_id;
		}	
	}

	memcpy(new_nodes, old_nodes, old_size);
	p_totalinfo_start = (struct mgtcgi_xml_ip_session_limit_total_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits));
	p_globalinfo_start = (struct mgtcgi_xml_ip_session_limit_global_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits) + 
						old_total_num * 
						sizeof(struct mgtcgi_xml_ip_session_limit_total_info));

	//修改gloabl数量，指针
	new_nodes->global_limit_num = new_global_num;
	new_nodes->p_totalinfo = p_totalinfo_start;
	new_nodes->p_globalinfo = p_globalinfo_start;
	
	name = max_id + 1;
	new_nodes->p_globalinfo[old_global_num].name = name;
	strcpy(new_nodes->p_globalinfo[old_global_num].addr, addr);
	strcpy(new_nodes->p_globalinfo[old_global_num].dst, dst);
	strcpy(new_nodes->p_globalinfo[old_global_num].per_ip_session_limit, per_ip_session_limit);
	strcpy(new_nodes->p_globalinfo[old_global_num].total_session_limit, total_session_limit);
	new_nodes->p_globalinfo[old_global_num].httplog = httplog;
	new_nodes->p_globalinfo[old_global_num].sesslog = sesslog;
	new_nodes->p_globalinfo[old_global_num].httpdirpolicy = httpdirpolicy;
	new_nodes->p_globalinfo[old_global_num].dnspolicy = dnspolicy;
	new_nodes->p_globalinfo[old_global_num].bridge = bridge;
	strcpy(new_nodes->p_globalinfo[old_global_num].action, action);
	strcpy(new_nodes->p_globalinfo[old_global_num].comment, comment);

	
#if _MGTCGI_DEBUG_GET_
	for (index=0; index < new_nodes->global_limit_num; index++){
		printf("new_nodes->p_globalinfo[%d].name:%d\n",index,new_nodes->p_globalinfo[index].name);
		printf("new_nodes->p_globalinfo[%d].addr:%s\n",index,new_nodes->p_globalinfo[index].addr);
		printf("new_nodes->p_globalinfo[%d].dst:%s\n",index,new_nodes->p_globalinfo[index].dst);
		printf("new_nodes->p_globalinfo[%d].per_ip_session_limit:%s\n",index,new_nodes->p_globalinfo[index].per_ip_session_limit);
		printf("new_nodes->p_globalinfo[%d].total_session_limit:%s\n",index,new_nodes->p_globalinfo[index].total_session_limit);
		printf("new_nodes->p_globalinfo[%d].httplog:%d\n",index,new_nodes->p_globalinfo[index].httplog);
		printf("new_nodes->p_globalinfo[%d].sesslog:%d\n",index,new_nodes->p_globalinfo[index].sesslog);
		printf("new_nodes->p_globalinfo[%d].httpdirpolicy:%d\n",index,new_nodes->p_globalinfo[index].httpdirpolicy);
		printf("new_nodes->p_globalinfo[%d].dnspolicy:%d\n",index,new_nodes->p_globalinfo[index].dnspolicy);		
		printf("new_nodes->p_globalinfo[%d].bridge:%d\n",index,new_nodes->p_globalinfo[index].bridge);		
		printf("new_nodes->p_globalinfo[%d].action:%s\n",index,new_nodes->p_globalinfo[index].action);		
		printf("new_nodes->p_globalinfo[%d].comment:%s\n",index,new_nodes->p_globalinfo[index].comment);			
	}
#endif

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void*)new_nodes, new_size);
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
		free_xml_node((void*)&session_limits);
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		return;
}

void ipsessions_edit_save(void)
{
	int index,old_size,size,iserror=0,result=0;
	int name,httplog,sesslog,httpdirpolicy,dnspolicy,bridge;
	unsigned long int perip_limit=0,total_limit=0;
	char name_str[DIGITAL_LENGTH]={"1"};
	char addr[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char per_ip_session_limit[STRING_LENGTH]={"0"};
	char total_session_limit[STRING_LENGTH]={"0"};
	char httplog_str[DIGITAL_LENGTH];
	char sesslog_str[DIGITAL_LENGTH];
	char httpdirpolicy_str[DIGITAL_LENGTH];
	char dnspolicy_str[DIGITAL_LENGTH];
	char action[STRING_LENGTH]={"bypass"};
	char bridge_str[DIGITAL_LENGTH];
	char comment[STRING_LENGTH]={"test"};

	struct mgtcgi_xml_session_limits * session_limits = NULL;
	struct mgtcgi_xml_ip_session_limits *old_nodes = NULL;	

	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("addr",addr,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("per_ip_session_limit",per_ip_session_limit,STRING_LENGTH);
	cgiFormString("total_session_limit",total_session_limit,STRING_LENGTH);
	cgiFormString("httplog",httplog_str,DIGITAL_LENGTH);
	cgiFormString("sesslog",sesslog_str,DIGITAL_LENGTH);
	cgiFormString("httpdirpolicy",httpdirpolicy_str,DIGITAL_LENGTH);
	cgiFormString("dnspolicy",dnspolicy_str,DIGITAL_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(per_ip_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(name_str) < STRING_LITTLE_LENGTH)
		|| (strlen(total_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(per_ip_session_limit) < STRING_LITTLE_LENGTH)
		|| (strlen(httplog_str) < STRING_LITTLE_LENGTH)
		|| (strlen(sesslog_str) < STRING_LITTLE_LENGTH)
		|| (strlen(dnspolicy_str) < STRING_LITTLE_LENGTH)
		|| (strlen(action) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	name = atoi(name_str);
	httplog = atoi(httplog_str);
	sesslog = atoi(sesslog_str);
	httpdirpolicy = atoi(httpdirpolicy_str);
	dnspolicy = atoi(dnspolicy_str);
	bridge = atoi(bridge_str);

	if (!((0 <= httplog && httplog <= 1) && 
		  (0 <= sesslog && sesslog <= 1) && 
		  (0 <= httpdirpolicy && httpdirpolicy <= 1) &&
		  (0 <= dnspolicy && dnspolicy <= 1) &&
		  (0 <= bridge && bridge <= 256)) &&
		  (0 <= name && name <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//单个会话数不能大于总会话数检查
	if (strcmp(per_ip_session_limit, "0") == 0){ //单会话数不限制，总会话数也不能限制
		if (strcmp(total_session_limit, "0") != 0){
			iserror = MGTCGI_PER_SESSION_ERR;
			goto ERROR_EXIT;
		}
		
	} else if (strcmp(total_session_limit, "0") != 0){//单会话数限制了，总会话数也选择了限制，检查
		get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SESSION_LIMITS,(void**)&session_limits,&size);
		if ( NULL == session_limits){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}	

		for (index=0; index < session_limits->num; index++){
			if (strcmp(session_limits->pinfo[index].name, per_ip_session_limit) == 0)	
				perip_limit = strtoul(session_limits->pinfo[index].value, 0, 10);
			
			if (strcmp(session_limits->pinfo[index].name, total_session_limit) == 0)	
				total_limit = strtoul(session_limits->pinfo[index].value, 0, 10);				
		}
		if (perip_limit > total_limit){
			iserror = MGTCGI_PER_SESSION_ERR;
			goto ERROR_EXIT;
		}
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//修改gloabl数量，指针
	iserror = -1;
	for (index=0; index < old_nodes->global_limit_num; index++){
		if ((old_nodes->p_globalinfo[index].name == name)&&
			(old_nodes->p_globalinfo[index].bridge == bridge)){
			strcpy(old_nodes->p_globalinfo[index].addr, addr);
			strcpy(old_nodes->p_globalinfo[index].dst, dst);
			strcpy(old_nodes->p_globalinfo[index].per_ip_session_limit, per_ip_session_limit);
			strcpy(old_nodes->p_globalinfo[index].total_session_limit, total_session_limit);
			old_nodes->p_globalinfo[index].httplog = httplog;
			old_nodes->p_globalinfo[index].sesslog = sesslog;
			old_nodes->p_globalinfo[index].httpdirpolicy = httpdirpolicy;
			old_nodes->p_globalinfo[index].dnspolicy = dnspolicy;
			old_nodes->p_globalinfo[index].bridge = bridge;
			strcpy(old_nodes->p_globalinfo[index].action, action);
			strcpy(old_nodes->p_globalinfo[index].comment, comment);

			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void*)old_nodes, old_size);
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
	for (index=0; index < old_nodes->global_limit_num; index++){
		printf("new_nodes->p_globalinfo[%d].name:%d\n",index,old_nodes->p_globalinfo[index].name);
		printf("new_nodes->p_globalinfo[%d].addr:%s\n",index,old_nodes->p_globalinfo[index].addr);
		printf("new_nodes->p_globalinfo[%d].dst:%s\n",index,old_nodes->p_globalinfo[index].dst);
		printf("new_nodes->p_globalinfo[%d].per_ip_session_limit:%s\n",index,old_nodes->p_globalinfo[index].per_ip_session_limit);
		printf("new_nodes->p_globalinfo[%d].total_session_limit:%s\n",index,old_nodes->p_globalinfo[index].total_session_limit);
		printf("new_nodes->p_globalinfo[%d].httplog:%d\n",index,old_nodes->p_globalinfo[index].httplog);
		printf("new_nodes->p_globalinfo[%d].sesslog:%d\n",index,old_nodes->p_globalinfo[index].sesslog);
		printf("new_nodes->p_globalinfo[%d].httpdirpolicy:%d\n",index,old_nodes->p_globalinfo[index].httpdirpolicy);
		printf("new_nodes->p_globalinfo[%d].dnspolicy:%d\n",index,old_nodes->p_globalinfo[index].dnspolicy);		
		printf("new_nodes->p_globalinfo[%d].bridge:%d\n",index,old_nodes->p_globalinfo[index].bridge);		
		printf("new_nodes->p_globalinfo[%d].action:%s\n",index,old_nodes->p_globalinfo[index].action);		
		printf("new_nodes->p_globalinfo[%d].comment:%s\n",index,old_nodes->p_globalinfo[index].comment);			
	}
#endif

	/**************************成功，释放内存***************************/
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&session_limits);
		free_xml_node((void*)&old_nodes);
		return;
}

void ipsessions_remove_save(void)
{
	int index,old_size,new_size,result=0;
	int old_total_num=0,old_global_num=0,
		new_total_num=0,new_global_num=0;
	int name,bridge;
	int iserror,remove_num,new_index,name_id;
	char name_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_ip_session_limits *old_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limits *new_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limit_total_info *p_totalinfo_start = NULL;
	struct mgtcgi_xml_ip_session_limit_global_info *p_globalinfo_start = NULL;

	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name_str) < STRING_LITTLE_LENGTH)||
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	name = atoi(name_str);
	bridge = atoi(bridge_str);

	if (!((0 <= bridge && bridge <= 256) &&
		  (0 <= name && name <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->global_limit_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	remove_num = -1;
	for (index=0; index < old_nodes->global_limit_num; index++){
		if ((old_nodes->p_globalinfo[index].name == name) &&
			(old_nodes->p_globalinfo[index].bridge == bridge)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->global_limit_num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/***********************可以删除，构建新结构***************************************/
	old_total_num = old_nodes->total_limit_num;
	old_global_num = old_nodes->global_limit_num;	//网络层
	new_total_num = old_total_num;
	new_global_num = old_global_num - 1;
	if (new_global_num < 0)
		new_global_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_ip_session_limits) + 
			old_total_num * 
				sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
			old_global_num * 
				sizeof(struct mgtcgi_xml_ip_session_limit_global_info);
	new_size = sizeof(struct mgtcgi_xml_ip_session_limits) + 
		new_total_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
		new_global_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_global_info);

	new_nodes = (struct mgtcgi_xml_ip_session_limits *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	p_totalinfo_start = (struct mgtcgi_xml_ip_session_limit_total_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits));
	p_globalinfo_start = (struct mgtcgi_xml_ip_session_limit_global_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits) + 
						old_total_num * 
						sizeof(struct mgtcgi_xml_ip_session_limit_total_info));

	//修改数据，指针
	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->total_limit_num = new_total_num;
	new_nodes->global_limit_num = new_global_num;
	new_nodes->p_totalinfo = p_totalinfo_start;
	new_nodes->p_globalinfo = p_globalinfo_start;

	//拷贝 总会话数限制 数据
	for (index=0; index < new_nodes->total_limit_num; index++){
		new_nodes->p_totalinfo[index].bridge = old_nodes->p_totalinfo[index].bridge;
		new_nodes->p_totalinfo[index].limit = old_nodes->p_totalinfo[index].limit;
		strcpy(new_nodes->p_totalinfo[index].overhead, old_nodes->p_totalinfo[index].overhead);
	}

	//拷贝 网络层策略 数据
	name_id = -1;
	for (index=0,new_index=0; index < old_nodes->global_limit_num; index++){
		if (old_nodes->p_globalinfo[index].bridge == bridge){
			
			if (old_nodes->p_globalinfo[index].name == name){
				continue;
			} else {
				if (-1 == name_id)
					name_id = 0;
				else 
					name_id++;	
			}
			new_nodes->p_globalinfo[new_index].name = name_id;
			
		} else {
			new_nodes->p_globalinfo[new_index].name = old_nodes->p_globalinfo[index].name;
		}
			
		strcpy(new_nodes->p_globalinfo[new_index].addr, old_nodes->p_globalinfo[index].addr);
		strcpy(new_nodes->p_globalinfo[new_index].dst, old_nodes->p_globalinfo[index].dst);
		strcpy(new_nodes->p_globalinfo[new_index].per_ip_session_limit, old_nodes->p_globalinfo[index].per_ip_session_limit);
		strcpy(new_nodes->p_globalinfo[new_index].total_session_limit, old_nodes->p_globalinfo[index].total_session_limit);
		new_nodes->p_globalinfo[new_index].httplog = old_nodes->p_globalinfo[index].httplog;
		new_nodes->p_globalinfo[new_index].sesslog = old_nodes->p_globalinfo[index].sesslog;
		new_nodes->p_globalinfo[new_index].httpdirpolicy = old_nodes->p_globalinfo[index].httpdirpolicy;
		new_nodes->p_globalinfo[new_index].dnspolicy = old_nodes->p_globalinfo[index].dnspolicy;
		new_nodes->p_globalinfo[new_index].bridge = old_nodes->p_globalinfo[index].bridge;
		strcpy(new_nodes->p_globalinfo[new_index].action, old_nodes->p_globalinfo[index].action);
		strcpy(new_nodes->p_globalinfo[new_index].comment, old_nodes->p_globalinfo[index].comment);

		new_index++;
	}

	if (new_index == new_nodes->global_limit_num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_
	for (index=0; index < old_nodes->global_limit_num; index++){
		printf("new_nodes->p_globalinfo[%d].name:%d\n",index,old_nodes->p_globalinfo[index].name);
		printf("new_nodes->p_globalinfo[%d].addr:%s\n",index,old_nodes->p_globalinfo[index].addr);
		printf("new_nodes->p_globalinfo[%d].dst:%s\n",index,old_nodes->p_globalinfo[index].dst);
		printf("new_nodes->p_globalinfo[%d].per_ip_session_limit:%s\n",index,old_nodes->p_globalinfo[index].per_ip_session_limit);
		printf("new_nodes->p_globalinfo[%d].total_session_limit:%s\n",index,old_nodes->p_globalinfo[index].total_session_limit);
		printf("new_nodes->p_globalinfo[%d].httplog:%d\n",index,old_nodes->p_globalinfo[index].httplog);
		printf("new_nodes->p_globalinfo[%d].sesslog:%d\n",index,old_nodes->p_globalinfo[index].sesslog);
		printf("new_nodes->p_globalinfo[%d].httpdirpolicy:%d\n",index,old_nodes->p_globalinfo[index].httpdirpolicy);
		printf("new_nodes->p_globalinfo[%d].dnspolicy:%d\n",index,old_nodes->p_globalinfo[index].dnspolicy);		
		printf("new_nodes->p_globalinfo[%d].bridge:%d\n",index,old_nodes->p_globalinfo[index].bridge);		
		printf("new_nodes->p_globalinfo[%d].action:%s\n",index,old_nodes->p_globalinfo[index].action);		
		printf("new_nodes->p_globalinfo[%d].comment:%s\n",index,old_nodes->p_globalinfo[index].comment);			
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
		ipsessions_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		ipsessions_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		ipsessions_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		ipsessions_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		ipsessions_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		ipsessions_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		ipsessions_move_save();
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

