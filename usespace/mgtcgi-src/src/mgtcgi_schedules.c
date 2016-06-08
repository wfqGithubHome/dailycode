#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end);
extern int check_weeks(const char *weeks, const char *day);

extern int group_name_check(struct mgtcgi_xml_group_array *group, const char *name, const char *old_name);
extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,const char *name, const char *old_name, int check_id);

void schedules_list_show(void)
{
	int size=0,index=0,index_end;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	int total=0,iserror=0,result=0;
	int Mon,Tue,Wed,Thu,Fri,Sat,Sun;
	struct mgtcgi_xml_schedules * schedules = NULL;

	cgiFormString("page",page_str,STRING_LENGTH);
	cgiFormString("pagesize",pagesize_str,STRING_LENGTH);

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = schedules->num;
	result = get_page_turn_index(page_str, pagesize_str, total, &index, &index_end);
	if (result != 0){
		iserror = result;
		goto ERROR_EXIT;
	}


	printf("{\"data\":[");
	for (; index < index_end ;){
		Mon=check_weeks(schedules->pinfo[index].days,"Mon");
		Tue=check_weeks(schedules->pinfo[index].days,"Tue");
		Wed=check_weeks(schedules->pinfo[index].days,"Wed");
		Thu=check_weeks(schedules->pinfo[index].days,"Thu");
		Fri=check_weeks(schedules->pinfo[index].days,"Fri");
		Sat=check_weeks(schedules->pinfo[index].days,"Sat");
		Sun=check_weeks(schedules->pinfo[index].days,"Sun");
		
		printf("{\"name\":\"%s\",\"start_time\":\"%s\",\"stop_time\":\"%s\",\"mon\":%d,\"tue\":%d,\"wed\":%d,\"thu\":%d,\"fri\":%d,\"sat\":%d,\"sun\":%d,\"comment\":\"%s\"}",\
			schedules->pinfo[index].name,schedules->pinfo[index].start_time,schedules->pinfo[index].stop_time,\
			Mon,Tue,Wed,Thu,Fri,Sat,Sun,schedules->pinfo[index].comment);

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
		free_xml_node((void*)&schedules);
		return;
}

void schedules_add_show(void)
{

}

void schedules_edit_show(void)
{
	int size=0,index=0,iserror=0;
	char name[STRING_LENGTH];
	int Mon,Tue,Wed,Thu,Fri,Sat,Sun;
	struct mgtcgi_xml_schedules * schedules = NULL;

	cgiFormString("name",name,STRING_LENGTH);

	if(strlen(name) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < schedules->num; index++){
		if (strcmp(name,schedules->pinfo[index].name) == 0){
			Mon=check_weeks(schedules->pinfo[index].days,"Mon");
			Tue=check_weeks(schedules->pinfo[index].days,"Tue");
			Wed=check_weeks(schedules->pinfo[index].days,"Wed");
			Thu=check_weeks(schedules->pinfo[index].days,"Thu");
			Fri=check_weeks(schedules->pinfo[index].days,"Fri");
			Sat=check_weeks(schedules->pinfo[index].days,"Sat");
			Sun=check_weeks(schedules->pinfo[index].days,"Sun");
			
			printf("{\"name\":\"%s\",\"start_time\":\"%s\",\"stop_time\":\"%s\",\"mon\":%d,\"tue\":%d,\"wed\":%d,\"thu\":%d,\"fri\":%d,\"sat\":%d,\"sun\":%d,\"comment\":\"%s\"}",\
				schedules->pinfo[index].name,schedules->pinfo[index].start_time,schedules->pinfo[index].stop_time,\
				Mon,Tue,Wed,Thu,Fri,Sat,Sun,schedules->pinfo[index].comment);

			break;
		}
	}	
		printf("],\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&schedules);
		return;
}


void schedules_add_save(void)
{
	int old_size,new_size,size,index=0,len=0,iserror=0,result=0;
	char name[STRING_LENGTH]={"1"};
	char start_time[STRING_LENGTH]={"00:00"};
	char stop_time[STRING_LENGTH]={"00:00"};
	char days[STRING_LENGTH];
	char mon_str[DIGITAL_LENGTH]={"1"},tue_str[DIGITAL_LENGTH]={"0"},wed_str[DIGITAL_LENGTH]={"0"},
		 thu_str[DIGITAL_LENGTH]={"0"},fri_str[DIGITAL_LENGTH]={"0"},sat_str[DIGITAL_LENGTH]={"0"},
		 sun_str[DIGITAL_LENGTH]={"1"};
	char comment[STRING_LENGTH]={"1"};

	struct mgtcgi_xml_schedules *old_nodes = NULL;
	struct mgtcgi_xml_schedules *new_nodes = NULL;
	struct mgtcgi_xml_group_array * group = NULL;

	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("start_time",start_time,STRING_LENGTH);
	cgiFormString("stop_time",stop_time,STRING_LENGTH);
	cgiFormString("mon",mon_str,STRING_LENGTH);
	cgiFormString("tue",tue_str,STRING_LENGTH);
	cgiFormString("wed",wed_str,STRING_LENGTH);
	cgiFormString("thu",thu_str,STRING_LENGTH);
	cgiFormString("fri",fri_str,STRING_LENGTH);
	cgiFormString("sat",sat_str,STRING_LENGTH);
	cgiFormString("sun",sun_str,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(start_time) < STRING_LITTLE_LENGTH)
		|| (strlen(stop_time) < STRING_LITTLE_LENGTH) 
		|| (strlen(mon_str) < STRING_LITTLE_LENGTH)
		|| (strlen(tue_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(wed_str) < STRING_LITTLE_LENGTH)
		|| (strlen(thu_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(fri_str) < STRING_LITTLE_LENGTH)
		|| (strlen(sat_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(sun_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

		memset(days, 0, sizeof(days));
		if (0 == strcmp(mon_str,"1"))	
			strcat(days,"Mon,");
		if (0 == strcmp(tue_str,"1"))	
			strcat(days,"Tue,");		
		if (0 == strcmp(wed_str,"1"))	
			strcat(days,"Wed,");
		if (0 == strcmp(thu_str,"1"))	
			strcat(days,"Thu,");
		if (0 == strcmp(fri_str,"1"))	
			strcat(days,"Fri,");
		if (0 == strcmp(sat_str,"1"))	
			strcat(days,"Sat,");
		if (0 == strcmp(sun_str,"1"))	
			strcat(days,"Sun,");
		
		len = strlen(days);
		if (len < 4){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		days[len-1] = '\0';
			
	/******************************重复性检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].name, name)){
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
	old_size = sizeof(struct mgtcgi_xml_schedules) + 
				old_nodes->num *
				sizeof(struct mgtcgi_xml_schedule_info);
	new_size = sizeof(struct mgtcgi_xml_schedules) + 
				(old_nodes->num + 1) *
				sizeof(struct mgtcgi_xml_schedule_info);

	new_nodes = (struct mgtcgi_xml_schedules *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = old_nodes->num + 1;
	strcpy(new_nodes->pinfo[old_nodes->num].name, name);
	strcpy(new_nodes->pinfo[old_nodes->num].start_time, start_time);
	strcpy(new_nodes->pinfo[old_nodes->num].stop_time, stop_time);
	strcpy(new_nodes->pinfo[old_nodes->num].days, days);
	strcpy(new_nodes->pinfo[old_nodes->num].comment, name);

#if _MGTCGI_DEBUG_GET_	
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].start_time:%s\n\n",index,new_nodes->pinfo[index].start_time);
		printf("new_nodes->pinfo[%d].stop_time:%s\n\n",index,new_nodes->pinfo[index].stop_time);
		printf("new_nodes->pinfo[%d].days:%s\n\n",index,new_nodes->pinfo[index].days);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,new_nodes->pinfo[index].comment);
	}
#endif
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES,(void*)new_nodes, new_size);
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
		free_xml_node((void*)&group);
		free_malloc_buf((void*)&old_nodes);
		return;
}


void schedules_edit_save(void)
{
	int old_size,size,index=0,len=0;
	int diff_name=0,iserror=0,result=0,ret_value=0;
	char old_name[STRING_LENGTH]={"aa"};
	char name[STRING_LENGTH]={"aa1"};
	char start_time[STRING_LENGTH]={"00:00"};
	char stop_time[STRING_LENGTH]={"00:00"};
	char days[STRING_LENGTH];
	char mon_str[DIGITAL_LENGTH]={"1"},tue_str[DIGITAL_LENGTH]={"0"},wed_str[DIGITAL_LENGTH]={"0"},
		 thu_str[DIGITAL_LENGTH]={"1"},fri_str[DIGITAL_LENGTH]={"0"},sat_str[DIGITAL_LENGTH]={"0"},
		 sun_str[DIGITAL_LENGTH]={"1"};
	char comment[STRING_LENGTH]={"test"};

	struct mgtcgi_xml_schedules *old_nodes = NULL;

	//时间对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; 
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("old_name",old_name,STRING_LENGTH);
	cgiFormString("name",name,STRING_LENGTH);
	cgiFormString("start_time",start_time,STRING_LENGTH);
	cgiFormString("stop_time",stop_time,STRING_LENGTH);
	cgiFormString("mon",mon_str,STRING_LENGTH);
	cgiFormString("tue",tue_str,STRING_LENGTH);
	cgiFormString("wed",wed_str,STRING_LENGTH);
	cgiFormString("thu",thu_str,STRING_LENGTH);
	cgiFormString("fri",fri_str,STRING_LENGTH);
	cgiFormString("sat",sat_str,STRING_LENGTH);
	cgiFormString("sun",sun_str,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH) 
		|| (strlen(old_name) < STRING_LITTLE_LENGTH)
		|| (strlen(start_time) < STRING_LITTLE_LENGTH)
		|| (strlen(stop_time) < STRING_LITTLE_LENGTH) 
		|| (strlen(mon_str) < STRING_LITTLE_LENGTH)
		|| (strlen(tue_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(wed_str) < STRING_LITTLE_LENGTH)
		|| (strlen(thu_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(fri_str) < STRING_LITTLE_LENGTH)
		|| (strlen(sat_str) < STRING_LITTLE_LENGTH) 
		|| (strlen(sun_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

		memset(days, 0, sizeof(days));
		if (0 == strcmp(mon_str,"1"))	
			strcat(days,"Mon,");
		if (0 == strcmp(tue_str,"1"))	
			strcat(days,"Tue,");		
		if (0 == strcmp(wed_str,"1"))	
			strcat(days,"Wed,");
		if (0 == strcmp(thu_str,"1"))	
			strcat(days,"Thu,");
		if (0 == strcmp(fri_str,"1"))	
			strcat(days,"Fri,");
		if (0 == strcmp(sat_str,"1"))	
			strcat(days,"Sat,");
		if (0 == strcmp(sun_str,"1"))	
			strcat(days,"Sun,");
		
		len = strlen(days);
		if (len < 4){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		days[len-1] = '\0';
			
	/******************************修改数据检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&old_nodes,&old_size);
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
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);
		if ( NULL == group){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
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

		//应用层策略检查
		ret_value = firewall_name_check(firewalls, name, old_name, 2);
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
			strcpy(old_nodes->pinfo[index].start_time, start_time);
			strcpy(old_nodes->pinfo[index].stop_time, stop_time);
			strcpy(old_nodes->pinfo[index].days, days);
			strcpy(old_nodes->pinfo[index].comment, name);
			
			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES,(void*)old_nodes, old_size);
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
		printf("old_nodes->pinfo[%d].name:%s\n\n",index,old_nodes->pinfo[index].name);
		printf("old_nodes->pinfo[%d].start_time:%s\n\n",index,old_nodes->pinfo[index].start_time);
		printf("old_nodes->pinfo[%d].stop_time:%s\n\n",index,old_nodes->pinfo[index].stop_time);
		printf("old_nodes->pinfo[%d].days:%s\n\n",index,old_nodes->pinfo[index].days);
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
		free_xml_node((void*)&group);
		free_xml_node((void*)&firewalls);
		return;
}


void schedules_remove_save(void)
{
	int old_size,size,index=0;
	int ret_value=0,iserror=0,result=0;
	int remove_num,old_num,new_num,new_size,new_index;
	char name[STRING_LENGTH]={"aa1"};

	struct mgtcgi_xml_schedules *old_nodes = NULL;
	struct mgtcgi_xml_schedules *new_nodes = NULL;

	//时间对象关联的结构定义
	struct mgtcgi_xml_group_array *group = NULL; 
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("name",name,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

			
	/******************************修改数据检查*****************************/
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&old_nodes,&old_size);
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
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);
	if ( NULL == group){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_PARAM_ERR;
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

	//应用层策略检查
	ret_value = firewall_name_check(firewalls, name, name, 2);
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
		
	old_size = sizeof(struct mgtcgi_xml_schedules) + 
				old_num *
				sizeof(struct mgtcgi_xml_schedule_info);
	new_size = sizeof(struct mgtcgi_xml_schedules) + 
				new_num *
				sizeof(struct mgtcgi_xml_schedule_info);
	
	new_nodes = (struct mgtcgi_xml_schedules *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->num = new_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			strcpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name);
			strcpy(new_nodes->pinfo[new_index].start_time, old_nodes->pinfo[index].start_time);
			strcpy(new_nodes->pinfo[new_index].stop_time, old_nodes->pinfo[index].stop_time);
			strcpy(new_nodes->pinfo[new_index].days, old_nodes->pinfo[index].days);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			new_index++;
		}
	}

	if (new_index == new_nodes->num){	
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES,(void*)new_nodes, new_size);
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
		printf("new_nodes->pinfo[%d].name:%s\n\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].start_time:%s\n\n",index,new_nodes->pinfo[index].start_time);
		printf("new_nodes->pinfo[%d].stop_time:%s\n\n",index,new_nodes->pinfo[index].stop_time);
		printf("new_nodes->pinfo[%d].days:%s\n\n",index,new_nodes->pinfo[index].days);
		printf("new_nodes->pinfo[%d].comment:%s\n\n",index,new_nodes->pinfo[index].comment);
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
		free_xml_node((void*)&group);
		free_xml_node((void*)&firewalls);
		return;
}

void schedules_move(void)
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
		schedules_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		schedules_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		schedules_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		schedules_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//修改"确定"
		schedules_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		schedules_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		schedules_move();
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

