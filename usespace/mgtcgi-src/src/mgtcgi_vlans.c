#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);

void vlans_list_show(void)
{
	int size=0,index=0,index_end=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,iserror=0,result=0;
	struct mgtcgi_xml_vlans * vlans = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&vlans,&size);
	if ( NULL == vlans){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = vlans->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (; index < index_end ;){
		printf("{\"name\":\"%s\",\"vlans\":\"%d\",\"comment\":\"%s\"}",\
			vlans->pinfo[index].name,vlans->pinfo[index].vlans,vlans->pinfo[index].comment);
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
		free_xml_node((void*)&vlans);
		return;
}

void vlans_add_show(void)
{
	;
}

void vlans_edit_show(void)
{

	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH];
	struct mgtcgi_xml_vlans * vlans = NULL;

	cgiFormString("name",name,STRING_LENGTH);

	if (strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&vlans,&size);
	if ( NULL == vlans){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
		
	printf("{\"data\":[");
	for (index = 0; index < vlans->num ; index++){
		if (strcmp(name,vlans->pinfo[index].name) == 0){
			printf("{\"name\":\"%s\",\"vlans\":%d,\"comment\":\"%s\"}",\
				vlans->pinfo[index].name,vlans->pinfo[index].vlans,vlans->pinfo[index].comment);
			break;
		}
	}
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&vlans);
		return;
}

void vlans_add_save(void)
{
	int old_size,new_size,index=0,vlans,iserror=0,result=0;
	char name[STRING_LENGTH]={"1"};
	char vlans_str[DIGITAL_LENGTH]={"0"};
	char comment[STRING_LENGTH]={"1"};

	struct mgtcgi_xml_vlans *old_nodes = NULL;
	struct mgtcgi_xml_vlans *new_nodes = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("vlans",vlans_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(vlans_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	vlans = atoi(vlans_str);
	if (!(0 <= vlans && vlans <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}

	/****************************重新构建新结构***************************/
	old_size = sizeof(struct mgtcgi_xml_vlans) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_vlan_info);
	new_size = sizeof(struct mgtcgi_xml_vlans) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_vlan_info);

	new_nodes = (struct mgtcgi_xml_vlans *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = old_nodes->num + 1;
	strcpy(new_nodes->pinfo[old_nodes->num].name, name);
	strcpy(new_nodes->pinfo[old_nodes->num].comment, comment);
	new_nodes->pinfo[old_nodes->num].vlans = vlans;

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].vlans:%d\n\n",index,new_nodes->pinfo[index].vlans);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,new_nodes->pinfo[index].comment);
	}
#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_VLANS,(void*)new_nodes, new_size);
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
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&old_nodes);
		return;
}

void vlans_edit_save(void)
{
	int old_size,size,index=0,vlans,iserror=0,result=0;
	int diff_name=0,ret_value=0;
	char old_name[STRING_LENGTH]={"aa"};
	char name[STRING_LENGTH]={"aa1"};
	char vlans_str[DIGITAL_LENGTH]={"0"};
	char comment[STRING_LENGTH]={"1"};

	struct mgtcgi_xml_vlans *old_nodes = NULL;

	//VLAN对象关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("vlans",vlans_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(vlans_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	vlans = atoi(vlans_str);
	if (!(0 <= vlans && vlans <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************参数检查******************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < old_nodes->num; index++){
			if (0 == strcmp(old_nodes->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (1 == diff_name){//如果名称修改了，检查是否重复和检查是否正在使用
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		//应用层策略检查 
		ret_value = firewall_name_check(firewalls, name, old_name, 3);//3        
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
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, old_name)){	
			strcpy(old_nodes->pinfo[index].name, name);
			strcpy(old_nodes->pinfo[index].comment, comment);
			old_nodes->pinfo[index].vlans = vlans;
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_VLANS,(void*)old_nodes, old_size);
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
		printf("old_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].vlans:%d\n\n",index,old_nodes->pinfo[index].vlans);
		printf("old_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
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
		free_xml_node((void*)&firewalls);
		return;
}

void vlans_remove_save(void)
{
	int old_size,size,index=0,result=0;
	int ret_value=0,iserror=0;
	int remove_num,old_num,new_num,new_size,new_index;
	char name[STRING_LENGTH]={"aa1"};

	struct mgtcgi_xml_vlans *old_nodes = NULL;
	struct mgtcgi_xml_vlans *new_nodes = NULL;
	//VLAN对象关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
			
	/******************************参数检查******************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&old_nodes,&old_size);
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
	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}	

	//检查是否重复和检查是否正在使用
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//应用层策略检查 
	ret_value = firewall_name_check(firewalls, name, name, 3);//3        
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
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_vlans) + 
				old_num *
				sizeof(struct mgtcgi_xml_vlan_info);
	new_size = sizeof(struct mgtcgi_xml_vlans) + 
				new_num *
				sizeof(struct mgtcgi_xml_vlan_info);

	new_nodes = (struct mgtcgi_xml_vlans *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->num = new_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			strcpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			new_nodes->pinfo[new_index].vlans = old_nodes->pinfo[index].vlans;
			new_index++;
		}
	}

	if (new_index == new_nodes->num){	
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_VLANS,(void*)new_nodes, new_size);
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
		printf("old_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].vlans:%d\n\n",index,old_nodes->pinfo[index].vlans);
		printf("old_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
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
		free_xml_node((void*)&new_nodes);
		free_xml_node((void*)&firewalls);
		return;
}

void vlans_move(void)
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
		vlans_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		vlans_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		vlans_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		vlans_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		vlans_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		vlans_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		vlans_move();
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

