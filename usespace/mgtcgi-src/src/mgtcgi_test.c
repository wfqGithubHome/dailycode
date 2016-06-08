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

void ipsessions_list_show(void)
{
	int size=0,index=0,index_end=0,type=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	int total=0,page=0,pagesize=0,bridge=0;
	struct mgtcgi_xml_ip_session_limits * ipsession = NULL;	

	cgiFormString("page",page_str,DIGITAL_LENGTH);
	cgiFormString("pagesize",pagesize_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif	

	if ((strlen(page_str) < 1) || (strlen(pagesize_str) < 1)\
								|| (strlen(bridge_str) < 1))
		goto param_error;

	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	bridge	= atoi(bridge_str);
	if ((page < 0) || (pagesize < 0) || (bridge < 0))
		goto param_error;
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);
	if ( NULL == ipsession)
	{
		free_xml_node((void*)&ipsession);
		goto memory_failure;
	}
	
	index = ((page * pagesize) - pagesize);
	index_end = (page * pagesize);
	if (index > ipsession->global_limit_num){
		index = ipsession->global_limit_num;;
	}
	if (index_end > ipsession->global_limit_num){
		index_end = ipsession->global_limit_num;
	}
	total = 0;
	
	printf("{\"data\":[");
	for (; index < index_end ;){
		index++;
		if (bridge == ipsession->p_globalinfo[index].bridge){
			total++;
			printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\"\
\"per_ip_session_limit\":%d,\"total_session_limit\":%d,\"httplog\":%d,\
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

			if (index < index_end)
				printf(",");
			else
				break;
		}

	}
	free_xml_node((void*)&ipsession);
	if (NULL == ipsession)
		printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
	else
		printf("],\"total\":%d,\"iserror\":4,\"msg\":\"\"}",total);
	
	return;

	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;

	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;
}

void ipsessions_add_show(void)
{
	int size=0,index=0;

	
#if _MGTCGI_DEBUG_GET_
		printf("content-type:text/html\n\n");		
#else
		printf("content-type:application/json\n\n");	
#endif	


	printf("{\"data\":[{\"src\":\"ros1\",\"dst\":\"ros2\"}],\
\"iplist\":[\"ip1\",\"ip2\",\"ip3\",\"ip4\"],\
\"iserror\":0,\"msg\":\"\"}");
	
		
	return;

	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;

	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;
}



void ipsessions_edit_show(void)
{
	int size=0,index=0,name=0,bridge=0;
	int total = 0;
	char name_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	struct mgtcgi_xml_ip_session_limits *ipsession = NULL; 
	struct mgtcgi_xml_networks *networks = NULL;
	struct mgtcgi_xml_session_limits *session_limits = NULL;

	cgiFormString("name",name_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");	
#else
	printf("content-type:application/json\n\n");	
#endif

	if ((strlen(name_str) < 1) || (strlen(bridge_str) < 1))
		goto param_error;
	else{
		name = atoi(name_str);
		bridge = atoi(bridge_str);
	}
	if ((name < 0) || (bridge < 0))
		goto param_error;

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS, (void**)&ipsession, &size); ;
	if ( NULL == ipsession)
	{
		free_xml_node((void*)&ipsession);
		goto memory_failure;
	}
	
	total =  ipsession->global_limit_num;
	printf("{\"data\":[");
	for (index=0; index < total; index++){
		if ((bridge == ipsession->p_globalinfo[index].bridge) &&\
			(name == ipsession->p_globalinfo[index].name)){
			printf("{\"name\":%d,\"src\":\"%s\",\"dst\":\"%s\"\
\"per_ip_session_limit\":%d,\"total_session_limit\":%d,\"httplog\":%d,\
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
	
	free_xml_node((void*)&ipsession);
	if (NULL == ipsession)
		printf("],\"iserror\":0,\"msg\":\"\"}");
	else
		printf("],\"iserror\":4,\"msg\":\"\"}");
	
	return;

	param_error:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;
		
	memory_failure:
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;

}

void ipsessions_add_save(void)
{
	char name[STRING_LENGTH];
	char ip_type[STRING_LENGTH];
	char ip_addr[STRING_LENGTH];
	int  len=0;
	
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("ip_type",ip_type,STRING_LENGTH);
	cgiFormString("ip_addr",ip_addr,STRING_LENGTH);

	if (strlen(name) < 1)	//名称检查
		goto param_error;
	if (check_ipaddr(ip_addr) != 0)
		goto param_error;


	param_error:
		printf("content-type:application/json\n\n");
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		return;

	memory_failure:
		printf("content-type:application/json\n\n");
		printf("{\"iserror\":3,\"msg\":\"memory error\"}");
		return;

}

void ipsessions_edit_save(void)
{

}

void ipsessions_remove(void)
{

}

void ipsessions_move(void)
{

}

int cgiMain()
{
	char  	submit_type[STRING_LENGTH];
	char 	cmd[CMD_LENGTH];
	int		result;

	ipsessions_add_show();
	return 0;

	snprintf(cmd, sizeof(cmd), CREATE_XMLTMP_CMD);
	system(cmd);

	cgiFormString("submit",submit_type,STRING_LENGTH);

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
		ipsessions_remove();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		ipsessions_move();
	}
	else{
		printf("content-type:application/json\n\n");
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	}

	return (0);
}

