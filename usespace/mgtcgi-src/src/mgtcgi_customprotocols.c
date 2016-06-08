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
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);

void customproto_list_show(void)
{
	int size=0,index=0,index_end=0,iserror=0,result=0;
	int total=0,flag=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	struct mgtcgi_xml_protocols *custom_protocols = NULL;
	
	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&custom_protocols,&size);
	if ( NULL == custom_protocols){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = custom_protocols->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}
	
	flag = 0;
	printf("{\"data\":[");
	for (; index < index_end; index++){
		if (flag == 0)
			flag = 1;
		else
			printf(",");
		
		printf("{\"name\":\"%s\",\"proto\":\"%s\",\"dport\":\"%s\",\"sport\":\"%s\",\
\"match\":\"%s\",\"option\":\"%s\",\"comment\":\"%s\"}",\
			custom_protocols->pinfo[index].name,\
			custom_protocols->pinfo[index].proto,\
			custom_protocols->pinfo[index].dport,\
			custom_protocols->pinfo[index].sport,
			custom_protocols->pinfo[index].match,\
			custom_protocols->pinfo[index].option,\
			custom_protocols->pinfo[index].comment);
	}

		printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&custom_protocols);
		return;
}

void customproto_add_show(void)
{
	;
}

void customproto_edit_show(void)
{

	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH]={"123456"};
	struct mgtcgi_xml_protocols *custom_protocols = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&custom_protocols,&size);
	if ( NULL == custom_protocols){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index = 0; index < custom_protocols->num ; index++){
		if (strcmp(name,custom_protocols->pinfo[index].name) == 0){
			printf("{\"name\":\"%s\",\"proto\":\"%s\",\"dport\":\"%s\",\"sport\":\"%s\",\
\"match\":\"%s\",\"option\":\"%s\",\"comment\":\"%s\"}",\
			custom_protocols->pinfo[index].name,\
			custom_protocols->pinfo[index].proto,\
			custom_protocols->pinfo[index].dport,\
			custom_protocols->pinfo[index].sport,
			custom_protocols->pinfo[index].match,\
			custom_protocols->pinfo[index].option,\
			custom_protocols->pinfo[index].comment);
		break;
		}
	}

		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&custom_protocols);
		return;
}

void customproto_add_save(void)
{
	int old_size,new_size,size,index=0,iserror=0,result=0;
	int dport=0,sport=0,ret_value;
	char name[STRING_LENGTH]={"test"};
	char proto[STRING_LENGTH]={"TCP"};
	char dport_str[STRING_LENGTH]={"80"};
	char sport_str[STRING_LENGTH];
	char match[DIGITAL_LENGTH];
	char option[DIGITAL_LENGTH];
	char comment[STRING_LENGTH];

	struct mgtcgi_xml_protocols *old_nodes = NULL;
	struct mgtcgi_xml_protocols *new_nodes = NULL;
	struct mgtcgi_xml_group_array *group = NULL; 
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("proto",proto,STRING_LENGTH);
	cgiFormString("dport",dport_str,STRING_LENGTH);
	cgiFormString("sport",sport_str,STRING_LENGTH);
	cgiFormString("match",match,STRING_LENGTH);
	cgiFormString("option",option,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(proto) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	dport = atoi(dport_str);
	sport = atoi(sport_str);
	if (!((0 < dport && dport < 65536) || 
		   (0 < sport && sport < 65536))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
					"chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
						"chinese", (void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//自定义协议组不能重名
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}	
	}

	//自定义协议不能与L7列表重名, 将名称转化成大写比较
	//for (i=0; i<sizeof(name); i++){
	//	toupper_name[i] = toupper(name[i]);
	//}
	for (index=0; index < l7protocol->num; index++){
		if (0 == strcmp(l7protocol->pinfo[index].name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}		
	}	
	ret_value = group_name_check(group, name, name);
	if (2 == ret_value){
		iserror = MGTCGI_DUPLICATE_ERR;
		goto ERROR_EXIT;
	}	
	/****************************重新构建新结构***************************/
	old_size = sizeof(struct mgtcgi_xml_protocols) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_protocol_info);
	
	new_size = sizeof(struct mgtcgi_xml_protocols) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_protocol_info);

	new_nodes = (struct mgtcgi_xml_protocols *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);
	
	new_nodes->num = old_nodes->num + 1;
	strcpy(new_nodes->pinfo[old_nodes->num].name, name);
	strcpy(new_nodes->pinfo[old_nodes->num].proto, proto);
	strcpy(new_nodes->pinfo[old_nodes->num].dport, dport_str);
	strcpy(new_nodes->pinfo[old_nodes->num].sport, sport_str);
	strcpy(new_nodes->pinfo[old_nodes->num].match, match);
	strcpy(new_nodes->pinfo[old_nodes->num].option, option);
	strcpy(new_nodes->pinfo[old_nodes->num].comment, name);

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].proto:%s\n\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].dport:%s\n\n",index,new_nodes->pinfo[index].dport);
		printf("new_nodes->pinfo[%d].sport:%s\n\n",index,new_nodes->pinfo[index].sport);
		printf("new_nodes->pinfo[%d].match:%s\n\n",index,new_nodes->pinfo[index].match);
		printf("new_nodes->pinfo[%d].option:%s\n\n",index,new_nodes->pinfo[index].option);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,new_nodes->pinfo[index].comment);
	}
#endif

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PROTOCOLS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	/**************************成功，释放内存***************************/

		system("/usr/local/sbin/l7-protoctl 1");
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&new_nodes);
		free_xml_node((void*)&l7protocol);
		free_xml_node((void*)&group);
		return;
}

void customproto_edit_save(void)
{
	int old_size,size,index=0;
	int dport=0,sport=0,diff_name,ret_value,iserror,result;
	char old_name[STRING_LENGTH]={"222"};
	char name[STRING_LENGTH]={"22"};
	char proto[STRING_LENGTH]={"TCP"};
	char dport_str[STRING_LENGTH]={"80"};
	char sport_str[STRING_LENGTH]={"33"};
	char match[DIGITAL_LENGTH];
	char option[DIGITAL_LENGTH];
	char comment[STRING_LENGTH];

	struct mgtcgi_xml_protocols *old_nodes = NULL;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;

	//ip地址对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //自定义对象组
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("proto",proto,STRING_LENGTH);
	cgiFormString("dport",dport_str,STRING_LENGTH);
	cgiFormString("sport",sport_str,STRING_LENGTH);
	cgiFormString("match",match,STRING_LENGTH);
	cgiFormString("option",option,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(proto) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	dport = atoi(dport_str);
	sport = atoi(sport_str);
    if (dport <= 0 || dport > 65535)
    {
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT; 
    }

    if (sport <= 0 || sport > 65535)
    {
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT; 
    }

	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7_SHOWS,
					"chinese",(void**)&l7protocol,&size);
	if ( NULL == l7protocol){
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

		//自定义协议不能与L7列表重名
		for (index=0; index < l7protocol->num; index++){
			if (0 == strcmp(l7protocol->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}		
		}
	}

	if (1 == diff_name){
		get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
							"chinese", (void**)&group,&size);
		if ( NULL == group){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		//对象组检查
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

		//应用层策略检查
		ret_value = firewall_name_check(firewalls, name, old_name, 5);
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
			strcpy(old_nodes->pinfo[index].proto, proto);
			strcpy(old_nodes->pinfo[index].dport, dport_str);
			strcpy(old_nodes->pinfo[index].sport, sport_str);
			strcpy(old_nodes->pinfo[index].match, match);
			strcpy(old_nodes->pinfo[index].option, option);
			strcpy(old_nodes->pinfo[index].comment, name);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PROTOCOLS,(void*)old_nodes, old_size);
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
	for (index=0; index < old_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].proto:%s\n\n",index,old_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].dport:%s\n\n",index,old_nodes->pinfo[index].dport);
		printf("new_nodes->pinfo[%d].sport:%s\n\n",index,old_nodes->pinfo[index].sport);
		printf("new_nodes->pinfo[%d].match:%s\n\n",index,old_nodes->pinfo[index].match);
		printf("new_nodes->pinfo[%d].option:%s\n\n",index,old_nodes->pinfo[index].option);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/

		system("/usr/local/sbin/l7-protoctl 1");
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&l7protocol);
		free_xml_node((void*)&group);
		free_xml_node((void*)&firewalls);
		return;
}

void customproto_remove_save(void)
{
	int old_size,new_size,size,index=0;
	int old_num,new_num,new_index,result=0;
	int iserror=6,ret_value=0,remove_num=0;
	char name[STRING_LENGTH]={"22"};

	struct mgtcgi_xml_protocols *old_nodes = NULL;
	struct mgtcgi_xml_protocols *new_nodes = NULL;
	struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;

	//ip地址对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; //自定义对象组
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_PROTOCOLS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
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


	get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
						"chinese", (void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size);
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//对象组检查
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

	//应用层策略检查
	ret_value = firewall_name_check(firewalls, name, name, 5);
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
	new_num = old_num -1;
	if (new_num < 0)
		new_num = 0;
	old_size = sizeof(struct mgtcgi_xml_protocols) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_protocol_info);
	
	new_size = sizeof(struct mgtcgi_xml_protocols) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_protocol_info);

	new_nodes = (struct mgtcgi_xml_protocols *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->num = new_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			strcpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name);
			strcpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[index].proto);
			strcpy(new_nodes->pinfo[new_index].dport, old_nodes->pinfo[index].dport);
			strcpy(new_nodes->pinfo[new_index].sport, old_nodes->pinfo[index].sport);
			strcpy(new_nodes->pinfo[new_index].match, old_nodes->pinfo[index].match);
			strcpy(new_nodes->pinfo[new_index].option, old_nodes->pinfo[index].option);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			new_index++;
		}
	}

	if (new_index == new_nodes->num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PROTOCOLS,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < old_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].proto:%s\n\n",index,old_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].dport:%s\n\n",index,old_nodes->pinfo[index].dport);
		printf("new_nodes->pinfo[%d].sport:%s\n\n",index,old_nodes->pinfo[index].sport);
		printf("new_nodes->pinfo[%d].match:%s\n\n",index,old_nodes->pinfo[index].match);
		printf("new_nodes->pinfo[%d].option:%s\n\n",index,old_nodes->pinfo[index].option);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,old_nodes->pinfo[index].comment);
	}
#endif

	/**************************成功，释放内存***************************/

		system("/usr/local/sbin/l7-protoctl 1");
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_malloc_buf((void*)&new_nodes);
		free_xml_node((void*)&old_nodes);
		free_xml_node((void*)&l7protocol);
		free_xml_node((void*)&group);
		free_xml_node((void*)&firewalls);
		return;
}

void customproto_move(void)
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
		customproto_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		customproto_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		customproto_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		customproto_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		customproto_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		customproto_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		customproto_move();
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

