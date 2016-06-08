#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

extern int get_bridge_num(void);
extern void print_ipaddrs_list(struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups);
extern void portmirrors_move_save();
extern inline void *malloc_buf(int size);
extern inline void free_malloc_buf(void **buf);

/*
	Make
	
*/
void portmirrors_list_show(void)
{
	int size=0,index=0,index_end=0,flag=0;
	int page_start=0,page_index=0;
	char page_str[DIGITAL_LENGTH]={"1"};
	char pagesize_str[DIGITAL_LENGTH]={"10"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	int total=0,page=0,pagesize=0,bridge=0,bridge_num=0,enable=0;
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL;	

	cgiFormString("page",page_str,DIGITAL_LENGTH);
	cgiFormString("pagesize",pagesize_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif	

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
	if ((strlen(page_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(pagesize_str) < STRING_LITTLE_LENGTH)
		||(strlen(bridge_str) < STRING_LITTLE_LENGTH))
		goto param_error;

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
	if ((page < PAGE_LITTLE_VALUE) 
		|| (pagesize < PAGE_LITTLE_VALUE) 
		|| (bridge < BRIDGE_LITTLE_VALUE))
		goto param_error;

	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&portmirrors,&size);
	if ( NULL == portmirrors){
		free_xml_node((void*)&portmirrors);
		goto memory_failure;
	}
	
	page_start = ((page * pagesize) - pagesize);
	if (page_start > portmirrors->num){
		page_start = portmirrors->num;
	}
	index_end = portmirrors->num;

	flag = 0;
	total = 0;
	page_index =0;
	bridge_num = 2;	// 桥数量要根据数据接口设置的桥信息确定
	
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		if (bridge == portmirrors->pinfo[index].bridge){
			total++;
			if ((total > page_start) && (page_index < pagesize)){
				page_index++;
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\",\"proto\":\"%s\",\
	\"action\":\"%s\",\"disabled\":%d,\"bridge\":%d,\"comment\":\"%s\"}",\
					portmirrors->pinfo[index].name,portmirrors->pinfo[index].src,\
					portmirrors->pinfo[index].dst,portmirrors->pinfo[index].proto,\
					portmirrors->pinfo[index].action,portmirrors->pinfo[index].disabled,\
					portmirrors->pinfo[index].bridge,portmirrors->pinfo[index].comment);
			}
		}
	}
	printf("],");

	for (index=0; index < MAX_BRIDGE_NUM; index++)
	{
		if (bridge == portmirrors->pstatus[index].bridge){
			enable = portmirrors->pstatus[index].enable;
			break;
		}
	}
	
	bridge_num = get_bridge_num();
	printf("\"total\":%d,\"enable\":%d,\"bridge_num\":%d,\"iserror\":0,\"msg\":\"\"}",total,enable,bridge_num);
	goto free_buf;

	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto free_buf;
	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		goto free_buf;
	free_buf:
		free_xml_node((void*)&portmirrors);
		return;
}

void portmirrors_add_show(void)
{
	int size=0;
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;	
	
#if _MGTCGI_DEBUG_GET_
		printf("content-type:text/html\n\n");		
#else
		printf("content-type:application/json\n\n");	
#endif	

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks)
	{
		free_xml_node((void*)&networks);
		goto memory_failure;
	}	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&networkgroups,&size);
	if ( NULL == networkgroups){
		free_xml_node((void*)&networkgroups);
		goto memory_failure;
	}	

	//打印网络层策略默认数据
	printf("{\"data\":[{\"proto\":\"any\",\"dst\":\"any\",\
\"action\":\"all\",\"disabled\":0}],");

	//json格式打印所有列表数组
	print_ipaddrs_list(networks, networkgroups);
	printf(",");

/******************************释放分配的内存********************************/	
	free_xml_node((void*)&networks);
	free_xml_node((void*)&networkgroups);
	if ((NULL == networkgroups) && (NULL == networks))
		printf("\"iserror\":0,\"msg\":\"\"}");
	else
		printf("\"iserror\":4,\"msg\":\"\"}");
	return;
memory_failure:
	printf("{\"iserror\":3,\"msg\":\"memory error\"}");
	return;
}

void portmirrors_edit_show(void)
{
	int size=0,index=0;
	char bridge_str[DIGITAL_LENGTH];
	char name_str[DIGITAL_LENGTH];
	int bridge=0,name=0;
	struct mgtcgi_xml_port_mirrors *portmirrors = NULL; 
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;	

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("name",name_str,DIGITAL_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif	

	if ((strlen(bridge_str) < 1) || (strlen(name_str) < 1))
		goto param_error;

	bridge	= atoi(bridge_str);
	name	= atoi(name_str);
	if ((bridge < 0) || (name < 0))
		goto param_error;
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS, (void**)&portmirrors, &size);
	if ( NULL == portmirrors){
		free_xml_node((void*)&portmirrors);
		goto memory_failure;
	}

	printf("{\"data\":[");
	for (index=0; index < portmirrors->num; index++){
		if ((bridge == portmirrors->pinfo[index].bridge)
			&& (portmirrors->pinfo[index].name == name)){	
			printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\",\"proto\":\"%s\",\
\"action\":\"%s\",\"disabled\":%d,\"bridge\":%d,\"comment\":\"%s\"}",\
				portmirrors->pinfo[index].name,portmirrors->pinfo[index].src,\
				portmirrors->pinfo[index].dst,portmirrors->pinfo[index].proto,\
				portmirrors->pinfo[index].action,portmirrors->pinfo[index].disabled,\
				portmirrors->pinfo[index].bridge,portmirrors->pinfo[index].comment);
			
			break;
		}
	}
	printf("],");


	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	if ( NULL == networks)
	{
		free_xml_node((void*)&networks);
		goto memory_failure;
	}	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&networkgroups,&size);
	if ( NULL == networkgroups){
		free_xml_node((void*)&networkgroups);
		goto memory_failure;
	}	

	//json格式打印所有列表数组
	print_ipaddrs_list(networks, networkgroups);
	printf(",");
	
	free_xml_node((void*)&portmirrors);
	free_xml_node((void*)&networks);
	free_xml_node((void*)&networkgroups);
	if ((NULL == portmirrors) && (NULL == networks) && (NULL == networkgroups))
		printf("\"iserror\":0,\"msg\":\"\"}");
	else
		printf("\"iserror\":4,\"msg\":\"\"}");
	return;

	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;
	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;

}

void portmirrors_add_save(void)
{
	int index,old_size,new_size;
	int old_num=0,new_num=0;
	int min_id,max_id;
	int disabled,bridge,name;

	char name_str[DIGITAL_LENGTH];
	char proto[STRING_LENGTH]={"any"};
	char action[STRING_LENGTH]={"all"};
	char src[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char disabled_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char comment[STRING_LENGTH];

	struct mgtcgi_xml_port_mirrors *old_nodes = NULL; 
	struct mgtcgi_xml_port_mirrors *new_nodes = NULL; 

	//注释的部分使用默认值
	cgiFormString("proto",proto,STRING_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	/******************************参数检查*****************************/
	if((strlen(proto) < STRING_LITTLE_LENGTH)	
		|| (strlen(action) < STRING_LITTLE_LENGTH)	
		|| (strlen(src) < STRING_LITTLE_LENGTH)
		|| (strlen(dst) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)) 	
		goto param_error;
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);

	if (!(0 <= disabled && disabled <= 1))
		goto param_error;
	if (!(0 <= bridge && bridge <= 256))
		goto param_error;

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		goto memory_failure;
	}
	
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_port_mirrors) + 
			MAX_BRIDGE_NUM * 
				sizeof(struct mgtcgi_xml_port_mirror_status) +
			old_num * 
				sizeof(struct mgtcgi_xml_port_mirror_info);
	new_size = sizeof(struct mgtcgi_xml_port_mirrors) + 
			MAX_BRIDGE_NUM * 
				sizeof(struct mgtcgi_xml_port_mirror_status) +
			new_num * 
				sizeof(struct mgtcgi_xml_port_mirror_info);

	new_nodes = (struct mgtcgi_xml_port_mirrors *)malloc_buf(new_size);
	if (NULL == new_nodes){
		goto memory_failure;
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
	snprintf(name_str, sizeof(name_str), "%d", name);
	
	new_nodes->pinfo[old_num].disabled = disabled;
	new_nodes->pinfo[old_num].bridge = bridge;
	new_nodes->pinfo[old_num].name = name;
	strcpy(new_nodes->pinfo[old_num].proto, proto);
	strcpy(new_nodes->pinfo[old_num].action, action);
	strcpy(new_nodes->pinfo[old_num].src, src);
	strcpy(new_nodes->pinfo[old_num].dst, dst);
	strcpy(new_nodes->pinfo[old_num].comment, comment);

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif
	save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void*)new_nodes, new_size);

	/**************************成功，释放内存***************************/
	free_xml_node((void*)&old_nodes);
	free_xml_node((void*)&new_nodes);
	if ((NULL == new_nodes) && (NULL == old_nodes))
		printf("{\"iserror\":0,\"msg\":\"\"}");
	else
		printf("{\"iserror\":4,\"msg\":\"memory error\"}");
	return;
	
param_error:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	return;
memory_failure:
	free_xml_node((void*)&old_nodes);
	free_xml_node((void*)&new_nodes);
	printf("{\"iserror\":3,\"msg\":\"memory error\"}");
	return;

}

void portmirrors_edit_save(void)
{
	int index,new_size;
	int disabled,bridge,name;
	int iserror = 7;

	char name_str[DIGITAL_LENGTH];
	char proto[STRING_LENGTH]={"any"};
	char action[STRING_LENGTH]={"all"};
	char src[STRING_LENGTH]={"any"};
	char dst[STRING_LENGTH]={"any"};
	char disabled_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char comment[STRING_LENGTH];
	
	struct mgtcgi_xml_port_mirrors *new_nodes = NULL; 

	//注释的部分使用默认值
	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("proto",proto,STRING_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	cgiFormString("disabled",disabled_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	/******************************参数检查*****************************/
	if((strlen(proto) < STRING_LITTLE_LENGTH)
		|| (strlen(name_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(action) < STRING_LITTLE_LENGTH)	
		|| (strlen(src) < STRING_LITTLE_LENGTH)
		|| (strlen(dst) < STRING_LITTLE_LENGTH)
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)) 	
		goto param_error;
	
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	name = atoi(name_str);

	if (!(0 <= disabled && disabled <= 1))
		goto param_error;
	if (!(0 <= bridge && bridge <= 256))
		goto param_error;
	if (!(0 <= name && name <= 65535))
		goto param_error;

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		goto memory_failure;
	}
	
	iserror = 7;
	for (index=0; index < new_nodes->num; index++){
		if ((new_nodes->pinfo[index].name == name)&&
			(new_nodes->pinfo[index].bridge == bridge)){
			
			new_nodes->pinfo[index].disabled = disabled;
			new_nodes->pinfo[index].bridge = bridge;
			new_nodes->pinfo[index].name = name;
			strcpy(new_nodes->pinfo[index].proto, proto);
			strcpy(new_nodes->pinfo[index].action, action);
			strcpy(new_nodes->pinfo[index].src, src);
			strcpy(new_nodes->pinfo[index].dst, dst);
			strcpy(new_nodes->pinfo[index].comment, comment);

			save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void*)new_nodes, new_size);
			iserror = 0;
			break;
		}
	}

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
	free_xml_node((void*)&new_nodes);
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	return;
	
param_error:
	free_xml_node((void*)&new_nodes);
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	return;
memory_failure:
	free_xml_node((void*)&new_nodes);
	printf("{\"iserror\":3,\"msg\":\"memory error\"}");
	return;
}

void portmirrors_remove_save(void)
{
	int index,old_size,new_size;
	int bridge,name;
	int iserror,remove_num,old_num,new_num,new_index,name_id=-1;

	char name_str[DIGITAL_LENGTH]={"1"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	
	struct mgtcgi_xml_port_mirrors *old_nodes = NULL; 
	struct mgtcgi_xml_port_mirrors *new_nodes = NULL; 

	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	/******************************参数检查*****************************/
	if((strlen(name_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)) 	
		goto param_error;
	
	bridge = atoi(bridge_str);
	name = atoi(name_str);

	if (!(0 <= bridge && bridge <= 256))
		goto param_error;
	if (!(0 <= name && name <= 65535))
		goto param_error;

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		goto memory_failure;
	}

	if (old_nodes->num < 1)
		goto param_error;

	remove_num = -1;
	for (index=0; index < old_nodes->num; index++){
		if ((old_nodes->pinfo[index].name == name) &&
			(old_nodes->pinfo[index].bridge == bridge)){
			remove_num = index;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->num)))//没有找到要删除的条目
		goto param_error;


	/***********************可以删除，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;

	old_size = sizeof(struct mgtcgi_xml_port_mirrors) + 
			MAX_BRIDGE_NUM * 
				sizeof(struct mgtcgi_xml_port_mirror_status) +
			old_num * 
				sizeof(struct mgtcgi_xml_port_mirror_info);
	new_size = sizeof(struct mgtcgi_xml_port_mirrors) + 
			MAX_BRIDGE_NUM * 
				sizeof(struct mgtcgi_xml_port_mirror_status) +
			new_num * 
				sizeof(struct mgtcgi_xml_port_mirror_info);

	new_nodes = (struct mgtcgi_xml_port_mirrors *)malloc_buf(new_size);
	if (NULL == new_nodes){
		goto memory_failure;
	}


	new_nodes->num = new_num;
	for (index=0; index < MAX_BRIDGE_NUM; index++){
		new_nodes->pstatus[index].bridge = old_nodes->pstatus[index].bridge;
		new_nodes->pstatus[index].enable = old_nodes->pstatus[index].enable;
	}

	name_id=-1;
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
		strcpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[index].proto);
		strcpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[index].action);
		strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src);
		strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst);
		strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);

		new_index++;
	}


	if (new_index == new_nodes->num){
		iserror = 0;
		save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void*)new_nodes, new_size);
	} else {
		iserror = 7;
	}

#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/
	free_malloc_buf((void*)&new_nodes);
	free_xml_node((void*)&old_nodes);
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	return;
	
	param_error:
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&old_nodes);
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;
	memory_failure:
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&old_nodes);
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;

}

void portmirrors_status_save(void)
{
	int enable,bridge,ret_value,new_size,index;
	char enable_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_port_mirrors *new_nodes = NULL; 

	cgiFormString("enable",enable_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	/******************************参数检查*****************************/
	if((strlen(enable_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)) 	
		goto param_error;
	
	bridge = atoi(bridge_str);
	enable = atoi(enable_str);

	if (!(0 <= bridge && bridge <= 256))
		goto param_error;
	if (!(0 <= enable && enable <= 1))
		goto param_error;

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		goto memory_failure;
	}

	for (index=0; index < MAX_BRIDGE_NUM; index++){
		if (new_nodes->pstatus[index].bridge == bridge){
			new_nodes->pstatus[index].enable = enable;
			break;
		}
	}

	ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void*)new_nodes, new_size);

	/**************************成功，释放内存***************************/
	if (-1 != ret_value)
		printf("{\"iserror\":0,\"msg\":\"\"}");
	else
		printf("{\"iserror\":4,\"msg\":\"\"}");
	return;
	
	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto free_buf;
	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		goto free_buf;
	free_buf:
		free_xml_node((void*)&new_nodes);
		return;
}
void portmirrors_move(void)
{

}

int cgiMain()
{
	char  	submit_type[STRING_LENGTH];
	char 	cmd[CMD_LENGTH];
	snprintf(cmd, sizeof(cmd), BACKUP_XMLTMP_CMD);
	system(cmd);

	cgiFormString("submit",submit_type,STRING_LENGTH);

	if (strcmp(submit_type,"list") == 0 ){//列表显示所有
		portmirrors_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		portmirrors_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		portmirrors_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		portmirrors_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		portmirrors_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		portmirrors_remove_save();
	}
	else if(strcmp(submit_type,"status") == 0){//启用，禁用
		portmirrors_status_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		portmirrors_move_save();
	}
	else{
		
	#if _MGTCGI_DEBUG_GET_
		printf("content-type:text/html\n\n");	
	#else
		printf("content-type:application/json\n\n");	
	#endif	
	
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	}

	return (0);
}

