#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

unsigned long int get_total_session_limit(void);

void totalsessions_list_show(void)
{
	int size=0,index=0,index_end=0;
	int total=0,iserror=0;
	unsigned long int max_session = 0;

	struct mgtcgi_xml_ip_session_limits * ipsession = NULL;	

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);
	if ( NULL == ipsession){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	index_end = ipsession->total_limit_num;
	total = ipsession->total_limit_num;
	
	printf("{\"data\":[");
	for (index=0; index < index_end;){
		printf("{\"bridge\":%d,\"limit\":%d,\"overhead\":\"%s\"}",\
			ipsession->p_totalinfo[index].bridge,
			ipsession->p_totalinfo[index].limit,
			ipsession->p_totalinfo[index].overhead);

		index++;
		if (index < index_end)
			printf(",");
		else
			break;
	}
	
	max_session = get_total_session_limit();
		printf("],\"total\":%d,\"max_session\":%lu,\"iserror\":0,\"msg\":\"\"}",total,max_session);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&ipsession);
		return;
}

void totalsessions_add_show(void)
{
	;
}

void totalsessions_edit_show(void)
{
	int size=0,index=0,bridge=0,index_end,iserror=0;
	char bridge_str[DIGITAL_LENGTH]={"0"};
	unsigned long int max_session = 0;
	struct mgtcgi_xml_ip_session_limits *ipsession = NULL; 

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	if (strlen(bridge_str) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	bridge = atoi(bridge_str);
	if (bridge < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);
	if ( NULL == ipsession){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	index_end = ipsession->total_limit_num;
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		if (bridge == ipsession->p_totalinfo[index].bridge){
			printf("{\"bridge\":%d,\"limit\":%d,\"overhead\":\"%s\"}",\
				ipsession->p_totalinfo[index].bridge,
				ipsession->p_totalinfo[index].limit,
				ipsession->p_totalinfo[index].overhead);
			break;
		}
	
	}

	max_session = get_total_session_limit();
		printf("],\"max_session\":%lu,\"iserror\":0,\"msg\":\"\"}",max_session);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&ipsession);
		return;
}

void totalsessions_add_save(void)
{
	;
}

void totalsessions_edit_save(void)
{
	int index,old_size;
	int iserror=0,result=0;
	int bridge,limit;
	unsigned long int max_session = 0,total_limit=0;
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char limit_str[DIGITAL_LENGTH]={"111"};
	char overhead[STRING_LENGTH]={"block"};

	struct mgtcgi_xml_ip_session_limits *old_nodes = NULL;	

	cgiFormString("limit",limit_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("overhead",overhead,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(limit_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)
		|| (strlen(overhead) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	limit = atoi(limit_str);
	bridge = atoi(bridge_str);
	max_session = get_total_session_limit();
	
	if (!((1 <= limit && limit <= max_session) && 
		(0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}


	total_limit = limit;
	for (index=0; index < old_nodes->total_limit_num; index++){
		if (old_nodes->p_totalinfo[index].bridge != bridge)
			total_limit += old_nodes->p_totalinfo[index].limit;
	}
	if (!(1 <= total_limit && total_limit <= max_session)){
		iserror = MGTCGI_TOTAL_SESSION_ERR;
		goto ERROR_EXIT;
	}

	//修改gloabl数量，指针
	iserror = -1;
	for (index=0; index < old_nodes->total_limit_num; index++){
		if (old_nodes->p_totalinfo[index].bridge == bridge){
			old_nodes->p_totalinfo[index].limit = limit;
			strcpy(old_nodes->p_totalinfo[index].overhead, overhead);
			
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
	for (index=0; index < old_nodes->total_limit_num; index++){
		printf("new_nodes->p_totalinfo[%d].bridge:%d\n",index,old_nodes->p_totalinfo[index].bridge);
		printf("new_nodes->p_totalinfo[%d].limit:%d\n",index,old_nodes->p_totalinfo[index].limit);
		printf("new_nodes->p_totalinfo[%d].overhead:%s\n",index,old_nodes->p_totalinfo[index].overhead);
			
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
		return;
}

void totalsessions_remove(void)
{

}

void totalsessions_move(void)
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
		totalsessions_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		totalsessions_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		totalsessions_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		totalsessions_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		totalsessions_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		totalsessions_remove();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		totalsessions_move();
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

