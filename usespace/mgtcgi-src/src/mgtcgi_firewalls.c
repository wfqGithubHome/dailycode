#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void *malloc_buf(int size);
extern void free_malloc_buf(void **buf);
extern void firewalls_move_save(void);
extern int get_bridge_num(void);
extern void get_traffic_comment(int direct, const char *name, char *comment);
extern void firewalls_add_edit_list(
				struct mgtcgi_xml_schedules *schedules,
				struct mgtcgi_xml_group_array *schedulegroups,
				struct mgtcgi_xml_vlans *vlans,
				struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups,
				struct mgtcgi_xml_session_limits *session_limits,
				struct mgtcgi_xml_traffics *channels,
				char bridge);


void firewalls_list_show(void)
{
	int size=0,index=0,index_end=0,flag=0;
	int total=0,page=1,pagesize=10,bridge=0,bridge_num=0,application_firewall,scan_detect,dos_detect;
	int page_start=0,page_index=0,iserror=0;
	char page_str[DIGITAL_LENGTH];
	char pagesize_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
    char in_traffic_comment[STRING_LENGTH]={"0"};
    char out_traffics_comment[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_firewalls *firewalls = NULL;	

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
	if ((page < PAGE_LITTLE_VALUE) 
		|| (pagesize < PAGE_LITTLE_VALUE) 
		|| (bridge < BRIDGE_LITTLE_VALUE)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	page_start = ((page * pagesize) - pagesize);
	if (page_start > firewalls->num){
		page_start = firewalls->num;
	}
	index_end = firewalls->num;

	flag = 0;
	total = 0;
	page_index =0;
	bridge_num = 2;	// 桥数量要根据数据接口设置的桥信息确定

	application_firewall = firewalls->application_firewall;
	scan_detect = firewalls->scan_detect;
	dos_detect = firewalls->dos_detect;

	
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		if (bridge == firewalls->pinfo[index].bridge){
			total++;
			if ((total > page_start) && (page_index < pagesize)){
				page_index++;
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				get_traffic_comment(1,firewalls->pinfo[index].in_traffic,in_traffic_comment);
                get_traffic_comment(0,firewalls->pinfo[index].out_traffic,out_traffics_comment);
				printf("{\"name\":%d,\"schedule\":\"%s\",\"vlan\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\
	\"proto\":\"%s\",\"action\":\"%s\",\"in_traffic\":\"%s\",\"in_traffic_comment\":\"%s\",\"out_traffic\":\"%s\",\"out_traffic_comment\":\"%s\",\"dscp\":%d,\"learn_change\":%d,\"session_limit\":\"%s\",\
	\"log\":%d,\"disabled\":%d,\"bridge\":%d,\"comment\":\"%s\"}",\
					firewalls->pinfo[index].name,firewalls->pinfo[index].schedule,\
					firewalls->pinfo[index].vlan,firewalls->pinfo[index].src,
					firewalls->pinfo[index].dst,firewalls->pinfo[index].proto,
					firewalls->pinfo[index].action,firewalls->pinfo[index].in_traffic,in_traffic_comment,
					firewalls->pinfo[index].out_traffic,out_traffics_comment,firewalls->pinfo[index].dscp,
					firewalls->pinfo[index].learn_change,firewalls->pinfo[index].session_limit,
					firewalls->pinfo[index].log,firewalls->pinfo[index].disabled,
					firewalls->pinfo[index].bridge,firewalls->pinfo[index].comment);
			}
		}
	}
	printf("],");

		bridge_num = get_bridge_num();
		printf("\"total\":%d,\"bridge_num\":%d,\"application_firewall\":%d,\
\"scan_detect\":%d,\"dos_detect\":%d,\"iserror\":0,\"msg\":\"\"}",\
total,bridge_num,application_firewall,scan_detect,dos_detect);
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&firewalls);
		return;
}

void firewalls_add_show(void)
{
	int size=0;	
	char bridge=0,iserror=0;
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_schedules * schedules = NULL;
	struct mgtcgi_xml_group_array * schedulegroups = NULL;
	struct mgtcgi_xml_vlans * vlans = NULL;	
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;
	struct mgtcgi_xml_session_limits * session_limits = NULL;
	struct mgtcgi_xml_traffics *channels = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	
	if(strlen(bridge_str) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	bridge = atoi(bridge_str);	
	if (bridge < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//时间计划节点
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&schedulegroups,&size);
	if ( NULL == schedulegroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//vlan节点
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&vlans,&size);
	if ( NULL == vlans){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//源目标ip节点
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

	//流量通道节点
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size); ;
	if ( NULL == channels){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//打印默认值
	printf("{\"data\":[{\"schedule\":\"always\",\"vlan\":\"\",\"src\":\"any\",\
\"dst\":\"any\",\"session_limit\":\"Unlimit\",\"proto\":\"any\",\
\"action\":\"allow\",\"in_traffic\":\"N/A\",\"out_traffic\":\"N/A\",\
\"dscp\":0,\"learn_change\":0,\"log\":0,\"disabled\":0}],");

	//json格式打印所有列表数组
	firewalls_add_edit_list(schedules, schedulegroups, vlans, networks, networkgroups,
							session_limits, channels, bridge);

/*********************************释放分配的内存********************************/		
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&schedules);
		free_xml_node((void*)&schedulegroups);
		free_xml_node((void*)&vlans);
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&session_limits);
		free_xml_node((void*)&channels);
		return;
}

void firewalls_edit_show(void)
{
	int size=0,index=0;	
	char bridge=0,name=0,iserror=0;
	char bridge_str[DIGITAL_LENGTH];
	char name_str[DIGITAL_LENGTH];
	struct mgtcgi_xml_firewalls *firewalls = NULL;	
	struct mgtcgi_xml_schedules * schedules = NULL;
	struct mgtcgi_xml_group_array * schedulegroups = NULL;
	struct mgtcgi_xml_vlans * vlans = NULL; 
	struct mgtcgi_xml_networks * networks = NULL;
	struct mgtcgi_xml_group_array * networkgroups = NULL;
	struct mgtcgi_xml_session_limits * session_limits = NULL;
	struct mgtcgi_xml_traffics *channels = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("name",name_str,DIGITAL_LENGTH);

	if((strlen(bridge_str) < 1) || (strlen(name_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	bridge = atoi(bridge_str);	
	name = atoi(name_str);
	if ((bridge < 0) || (name < 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//firewall节点
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
	if ( NULL == firewalls){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < firewalls->num; index++){
		if ((bridge == firewalls->pinfo[index].bridge) && (name == firewalls->pinfo[index].name)){
			printf("{\"name\":%d,\"schedule\":\"%s\",\"vlan\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\
\"proto\":\"%s\",\"action\":\"%s\",\"in_traffic\":\"%s\",\"out_traffic\":\"%s\",\"dscp\":%d,\"learn_change\":%d,\"session_limit\":\"%s\",\
\"log\":%d,\"disabled\":%d,\"bridge\":%d,\"comment\":\"%s\"}",\
				firewalls->pinfo[index].name,firewalls->pinfo[index].schedule,\
				firewalls->pinfo[index].vlan,firewalls->pinfo[index].src,
				firewalls->pinfo[index].dst,firewalls->pinfo[index].proto,
				firewalls->pinfo[index].action,firewalls->pinfo[index].in_traffic,
				firewalls->pinfo[index].out_traffic,firewalls->pinfo[index].dscp,
				firewalls->pinfo[index].learn_change,firewalls->pinfo[index].session_limit,
				firewalls->pinfo[index].log,firewalls->pinfo[index].disabled,
				firewalls->pinfo[index].bridge,firewalls->pinfo[index].comment);

			break;
		}
	}
	printf("],");

	//时间计划节点
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);
	if ( NULL == schedules){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&schedulegroups,&size);
	if ( NULL == schedulegroups){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//vlan节点
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_VLANS,(void**)&vlans,&size);
	if ( NULL == vlans){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//源目标ip节点
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

	//流量通道节点
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size); ;
	if ( NULL == channels){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//json格式打印所有列表数组
	firewalls_add_edit_list(schedules, schedulegroups, vlans, networks, networkgroups,
							session_limits, channels, bridge);

/*********************************释放分配的内存********************************/	
		printf("\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&firewalls);
		free_xml_node((void*)&schedules);
		free_xml_node((void*)&schedulegroups);
		free_xml_node((void*)&vlans);
		free_xml_node((void*)&networks);
		free_xml_node((void*)&networkgroups);
		free_xml_node((void*)&session_limits);
		free_xml_node((void*)&channels);
		return;
}

void firewalls_add_save(void)
{
	int index,new_size,old_size, new_num, old_num;
	int iserror=0,result=0;
	int max_id,min_id;
	int name,quota_action,log,disabled,bridge,dscp,learn_change;

	char  dscp_str[DIGITAL_LENGTH];
	char  learn_change_str[DIGITAL_LENGTH];

	char proto[STRING_LENGTH];
	char auth[STRING_LENGTH]={"#GUEST#"};
	char action[STRING_LENGTH];
	char schedule[STRING_LENGTH];
	char session_limit[STRING_LENGTH];
	char vlan[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char mac[STRING_LENGTH]={"any"};
	char in_traffic[STRING_LENGTH];
	char out_traffic[STRING_LENGTH];
	char quota[STRING_LENGTH]={"any"};
	char quota_action_str[DIGITAL_LENGTH]={"0"};
	char second_in_traffic[STRING_LENGTH]={"N/A"};
	char second_out_traffic[STRING_LENGTH]={"N/A"};
	char log_str[DIGITAL_LENGTH];
	char disabled_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	char comment[STRING_LENGTH];

	struct mgtcgi_xml_firewalls * old_nodes = NULL; 
	struct mgtcgi_xml_firewalls * new_nodes = NULL; 

	//注释的部分使用默认值
	//cgiFormString("application_firewall",application_firewall_str,DIGITAL_LENGTH);
	//cgiFormString("scan_detect",scan_detect_str,DIGITAL_LENGTH);
	//cgiFormString("dos_detect",dos_detect_str,DIGITAL_LENGTH);
	cgiFormString("proto",proto,STRING_LENGTH);
	//cgiFormString("auth",auth,STRING_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("schedule",schedule,STRING_LENGTH);
	cgiFormString("session_limit",session_limit,STRING_LENGTH);
	cgiFormString("vlan",vlan,STRING_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	//cgiFormString("mac",mac,STRING_LENGTH);
	cgiFormString("in_traffic",in_traffic,STRING_LENGTH);
	cgiFormString("out_traffic",out_traffic,STRING_LENGTH);
	//cgiFormString("quota",quota,STRING_LENGTH);
	//cgiFormString("quota_action",quota_action_str,DIGITAL_LENGTH);
	//cgiFormString("second_in_traffic",second_in_traffic,STRING_LENGTH);
	//cgiFormString("second_out_traffic",second_out_traffic,STRING_LENGTH);
	cgiFormString("log",log_str,DIGITAL_LENGTH);
	cgiFormString("disabled",disabled_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("dscp",dscp_str,DIGITAL_LENGTH);
	cgiFormString("learn_change",learn_change_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(schedule) < STRING_LITTLE_LENGTH)	//时间
		|| (strlen(vlan) < STRING_LITTLE_LENGTH)	//vlan
		|| (strlen(src) < STRING_LITTLE_LENGTH)		//源地址
		|| (strlen(dst) < STRING_LITTLE_LENGTH)		//目的地址
		|| (strlen(session_limit) < STRING_LITTLE_LENGTH)//会话数限制
		|| (strlen(proto) < STRING_LITTLE_LENGTH)	//应用组
		|| (strlen(action) < STRING_LITTLE_LENGTH)	//动作
		|| (strlen(in_traffic) < STRING_LITTLE_LENGTH)	//进入流量
		|| (strlen(out_traffic) < STRING_LITTLE_LENGTH)	//外出流量
		|| (strlen(log_str) < STRING_LITTLE_LENGTH)	//阻挡日志
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)//是否禁用
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){//桥ID
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	log = atoi(log_str);
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	quota_action = atoi(quota_action_str);
	dscp = atoi(dscp_str);
	learn_change = atoi(learn_change_str);

	if (!((0 <= log && log <= 1) &&
		   (0 <= disabled && disabled <= 1) &&
		   (0 <= bridge && bridge <= 256) && 
		   (0 <= quota_action && quota_action <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_firewalls) + 
			old_num * 
				sizeof(struct mgtcgi_xml_firewall_info);
	new_size = sizeof(struct mgtcgi_xml_firewalls) + 
			new_num * 
				sizeof(struct mgtcgi_xml_firewall_info);


	new_nodes = (struct mgtcgi_xml_firewalls *)malloc_buf(new_size);
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

	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;

	/*页面可以显示的参数*/
	name = max_id + 1;
	new_nodes->pinfo[old_num].name = name;
	new_nodes->pinfo[old_num].log = log;
	new_nodes->pinfo[old_num].disabled = disabled;
	new_nodes->pinfo[old_num].bridge = bridge;
	new_nodes->pinfo[old_num].dscp = dscp;
	new_nodes->pinfo[old_num].learn_change = learn_change;
	strcpy(new_nodes->pinfo[old_num].schedule, schedule);
	strcpy(new_nodes->pinfo[old_num].vlan, vlan);
	strcpy(new_nodes->pinfo[old_num].src, src);
	strcpy(new_nodes->pinfo[old_num].dst, dst);
	strcpy(new_nodes->pinfo[old_num].session_limit, session_limit);
	strcpy(new_nodes->pinfo[old_num].proto, proto);
	strcpy(new_nodes->pinfo[old_num].action, action);
	strcpy(new_nodes->pinfo[old_num].in_traffic, in_traffic);
	strcpy(new_nodes->pinfo[old_num].out_traffic, out_traffic);
	strcpy(new_nodes->pinfo[old_num].comment, comment);
	/*页面不显示的参数*/
	strcpy(new_nodes->pinfo[old_num].auth, auth);
	strcpy(new_nodes->pinfo[old_num].mac, mac);
	strcpy(new_nodes->pinfo[old_num].quota, quota);
	strcpy(new_nodes->pinfo[old_num].second_in_traffic, second_in_traffic);
	strcpy(new_nodes->pinfo[old_num].second_out_traffic, second_out_traffic);
	new_nodes->pinfo[old_num].quota_action = quota_action;


#if _MGTCGI_DEBUG_GET_
	printf("new_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name:%d\n",index,new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].log:%d\n",index,new_nodes->pinfo[index].log);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);

		printf("new_nodes->pinfo[%d].schedule:%s\n",index,new_nodes->pinfo[index].schedule);
		printf("new_nodes->pinfo[%d].vlan:%s\n",index,new_nodes->pinfo[index].vlan);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].session_limit:%s\n",index,new_nodes->pinfo[index].session_limit);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].in_traffic:%s\n",index,new_nodes->pinfo[index].in_traffic);
		printf("new_nodes->pinfo[%d].out_traffic:%s\n",index,new_nodes->pinfo[index].out_traffic);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,new_nodes->pinfo[index].comment);
	}
#endif

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void*)new_nodes, new_size);
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

void firewalls_edit_save(void)
{
	int index,new_size;
	int name,quota_action,log,disabled,bridge,dscp,learn_change;
	int iserror=0,result=0;

	char  dscp_str[DIGITAL_LENGTH];
	char  learn_change_str[DIGITAL_LENGTH];

	char name_str[STRING_LENGTH];
	char proto[STRING_LENGTH];
	char auth[STRING_LENGTH]={"#GUEST#"};
	char action[STRING_LENGTH];
	char schedule[STRING_LENGTH];
	char session_limit[STRING_LENGTH];
	char vlan[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char mac[STRING_LENGTH]={"any"};
	char in_traffic[STRING_LENGTH];
	char out_traffic[STRING_LENGTH];
	char quota[STRING_LENGTH]={"any"};
	char quota_action_str[DIGITAL_LENGTH]={"0"};
	char second_in_traffic[STRING_LENGTH]={"N/A"};
	char second_out_traffic[STRING_LENGTH]={"N/A"};
	char log_str[DIGITAL_LENGTH];
	char disabled_str[DIGITAL_LENGTH];
	char bridge_str[DIGITAL_LENGTH];
	char comment[STRING_LENGTH];
 
	struct mgtcgi_xml_firewalls * new_nodes = NULL; 

	//注释的部分使用默认值
	//cgiFormString("application_firewall",application_firewall_str,DIGITAL_LENGTH);
	//cgiFormString("scan_detect",scan_detect_str,DIGITAL_LENGTH);
	//cgiFormString("dos_detect",dos_detect_str,DIGITAL_LENGTH);

	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("proto",proto,STRING_LENGTH);
	//cgiFormString("auth",auth,STRING_LENGTH);
	cgiFormString("action",action,STRING_LENGTH);
	cgiFormString("schedule",schedule,STRING_LENGTH);
	cgiFormString("session_limit",session_limit,STRING_LENGTH);
	cgiFormString("vlan",vlan,STRING_LENGTH);
	cgiFormString("src",src,STRING_LENGTH);
	cgiFormString("dst",dst,STRING_LENGTH);
	//cgiFormString("mac",mac,STRING_LENGTH);
	cgiFormString("in_traffic",in_traffic,STRING_LENGTH);
	cgiFormString("out_traffic",out_traffic,STRING_LENGTH);
	//cgiFormString("quota",quota,STRING_LENGTH);
	//cgiFormString("quota_action",quota_action_str,DIGITAL_LENGTH);
	//cgiFormString("second_in_traffic",second_in_traffic,STRING_LENGTH);
	//cgiFormString("second_out_traffic",second_out_traffic,STRING_LENGTH);
	cgiFormString("log",log_str,DIGITAL_LENGTH);
	cgiFormString("disabled",disabled_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("dscp",dscp_str,DIGITAL_LENGTH);
	cgiFormString("learn_change",learn_change_str,DIGITAL_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(schedule) < STRING_LITTLE_LENGTH)	//时间
		|| (strlen(name_str) < STRING_LITTLE_LENGTH)	//name
		|| (strlen(vlan) < STRING_LITTLE_LENGTH)	//vlan
		|| (strlen(src) < STRING_LITTLE_LENGTH)		//源地址
		|| (strlen(dst) < STRING_LITTLE_LENGTH)		//目的地址
		|| (strlen(session_limit) < STRING_LITTLE_LENGTH)//会话数限制
		|| (strlen(proto) < STRING_LITTLE_LENGTH)	//应用组
		|| (strlen(action) < STRING_LITTLE_LENGTH)	//动作
		|| (strlen(in_traffic) < STRING_LITTLE_LENGTH)	//进入流量
		|| (strlen(out_traffic) < STRING_LITTLE_LENGTH)	//外出流量
		|| (strlen(log_str) < STRING_LITTLE_LENGTH)	//阻挡日志
		|| (strlen(disabled_str) < STRING_LITTLE_LENGTH)//是否禁用
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){//桥ID
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	name = atoi(name_str);
	log = atoi(log_str);
	disabled = atoi(disabled_str);
	bridge = atoi(bridge_str);
	quota_action = atoi(quota_action_str);
	dscp = atoi(dscp_str);
	learn_change = atoi(learn_change_str);
	if (!((0 <= name && name <= 65535) &&
			(0 <= log && log <= 1) &&
			(0 <= disabled && disabled <= 1) &&
			(0 <= bridge && bridge <= 256) &&
			(0 <= quota_action && quota_action <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	iserror = -1;
	for (index=0; index < new_nodes->num; index++){
		if ((new_nodes->pinfo[index].bridge == bridge) &&
			(new_nodes->pinfo[index].name == name)){
			new_nodes->pinfo[index].log = log;
			new_nodes->pinfo[index].disabled = disabled;
			new_nodes->pinfo[index].bridge = bridge;
			new_nodes->pinfo[index].dscp = dscp;
			new_nodes->pinfo[index].learn_change = learn_change;
			strcpy(new_nodes->pinfo[index].schedule, schedule);
			strcpy(new_nodes->pinfo[index].vlan, vlan);
			strcpy(new_nodes->pinfo[index].src, src);
			strcpy(new_nodes->pinfo[index].dst, dst);
			strcpy(new_nodes->pinfo[index].session_limit, session_limit);
			strcpy(new_nodes->pinfo[index].proto, proto);
			strcpy(new_nodes->pinfo[index].action, action);
			strcpy(new_nodes->pinfo[index].in_traffic, in_traffic);
			strcpy(new_nodes->pinfo[index].out_traffic, out_traffic);
			strcpy(new_nodes->pinfo[index].comment, comment);
			/*页面不显示的参数*/
			strcpy(new_nodes->pinfo[index].auth, auth);
			strcpy(new_nodes->pinfo[index].mac, mac);
			strcpy(new_nodes->pinfo[index].quota, quota);
			strcpy(new_nodes->pinfo[index].second_in_traffic, second_in_traffic);
			strcpy(new_nodes->pinfo[index].second_out_traffic, second_out_traffic);
			new_nodes->pinfo[index].quota_action = quota_action;

			result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void*)new_nodes, new_size);
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
		printf("new_nodes->pinfo[%d].log:%d\n",index,new_nodes->pinfo[index].log);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);

		printf("new_nodes->pinfo[%d].schedule:%s\n",index,new_nodes->pinfo[index].schedule);
		printf("new_nodes->pinfo[%d].vlan:%s\n",index,new_nodes->pinfo[index].vlan);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].session_limit:%s\n",index,new_nodes->pinfo[index].session_limit);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].in_traffic:%s\n",index,new_nodes->pinfo[index].in_traffic);
		printf("new_nodes->pinfo[%d].out_traffic:%s\n",index,new_nodes->pinfo[index].out_traffic);
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

void firewalls_remove_save(void)
{
	int index,old_size,new_size;
	int name,bridge,result=0;
	int iserror,remove_num,old_num,new_num,new_index,name_id;

	char name_str[STRING_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};

 	struct mgtcgi_xml_firewalls * old_nodes = NULL; 
	struct mgtcgi_xml_firewalls * new_nodes = NULL; 

	cgiFormString("name",name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(name_str) < STRING_LITTLE_LENGTH)	//name
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){//桥ID
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	name = atoi(name_str);
	bridge = atoi(bridge_str);
	if (!((0 <= name && name <= 65535) &&
		   (0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void**)&old_nodes,&old_size);
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
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	/***********************可以删除，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	if (new_num < 0)
		new_num = 0;

	old_size = sizeof(struct mgtcgi_xml_firewalls) + 
			old_num * 
				sizeof(struct mgtcgi_xml_firewall_info);
	new_size = sizeof(struct mgtcgi_xml_firewalls) + 
			new_num * 
				sizeof(struct mgtcgi_xml_firewall_info);

	new_nodes = (struct mgtcgi_xml_firewalls *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->num = new_num;
	new_nodes->application_firewall = old_nodes->application_firewall;
	new_nodes->scan_detect = old_nodes->scan_detect;
	new_nodes->dos_detect = old_nodes->dos_detect;

	name_id = -1;
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

		new_nodes->pinfo[new_index].log = old_nodes->pinfo[index].log;
		new_nodes->pinfo[new_index].disabled = old_nodes->pinfo[index].disabled;
		new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[index].bridge;
		new_nodes->pinfo[new_index].dscp = old_nodes->pinfo[index].dscp;
		new_nodes->pinfo[new_index].learn_change = old_nodes->pinfo[index].learn_change;
		strcpy(new_nodes->pinfo[new_index].schedule, old_nodes->pinfo[index].schedule);
		strcpy(new_nodes->pinfo[new_index].vlan, old_nodes->pinfo[index].vlan);
		strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src);
		strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst);
		strcpy(new_nodes->pinfo[new_index].session_limit, old_nodes->pinfo[index].session_limit);
		strcpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[index].proto);
		strcpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[index].action);
		strcpy(new_nodes->pinfo[new_index].in_traffic, old_nodes->pinfo[index].in_traffic);
		strcpy(new_nodes->pinfo[new_index].out_traffic, old_nodes->pinfo[index].out_traffic);
		strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
		/*页面不显示的参数*/
		strcpy(new_nodes->pinfo[new_index].auth, old_nodes->pinfo[index].auth);
		strcpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[index].mac);
		strcpy(new_nodes->pinfo[new_index].quota, old_nodes->pinfo[index].quota);
		strcpy(new_nodes->pinfo[new_index].second_in_traffic, old_nodes->pinfo[index].second_in_traffic);
		strcpy(new_nodes->pinfo[new_index].second_out_traffic, old_nodes->pinfo[index].second_out_traffic);
		new_nodes->pinfo[new_index].quota_action = old_nodes->pinfo[index].quota_action;

		new_index++;
	}
	
	if (new_index == new_nodes->num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void*)new_nodes, new_size);
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
		printf("new_nodes->pinfo[%d].log:%d\n",index,new_nodes->pinfo[index].log);
		printf("new_nodes->pinfo[%d].disabled:%d\n",index,new_nodes->pinfo[index].disabled);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,new_nodes->pinfo[index].bridge);

		printf("new_nodes->pinfo[%d].schedule:%s\n",index,new_nodes->pinfo[index].schedule);
		printf("new_nodes->pinfo[%d].vlan:%s\n",index,new_nodes->pinfo[index].vlan);
		printf("new_nodes->pinfo[%d].src:%s\n",index,new_nodes->pinfo[index].src);
		printf("new_nodes->pinfo[%d].dst:%s\n",index,new_nodes->pinfo[index].dst);
		printf("new_nodes->pinfo[%d].session_limit:%s\n",index,new_nodes->pinfo[index].session_limit);
		printf("new_nodes->pinfo[%d].proto:%s\n",index,new_nodes->pinfo[index].proto);
		printf("new_nodes->pinfo[%d].action:%s\n",index,new_nodes->pinfo[index].action);
		printf("new_nodes->pinfo[%d].in_traffic:%s\n",index,new_nodes->pinfo[index].in_traffic);
		printf("new_nodes->pinfo[%d].out_traffic:%s\n",index,new_nodes->pinfo[index].out_traffic);
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
		free_xml_node((void*)&old_nodes);
		free_malloc_buf((void*)&new_nodes);
		return;
}



void firewalls_status_save(void){	
	int enable=1,new_size,traffic_size,ret_value,iserror=0;
	char enable_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_firewalls * new_nodes = NULL; 
    struct mgtcgi_xml_traffics * traffic_nodes = NULL; 
	
	cgiFormString("enable",enable_str,DIGITAL_LENGTH);

	if (strlen(enable_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void**)&traffic_nodes,&traffic_size);
    if ( NULL == traffic_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->application_firewall = enable;
    traffic_nodes->status = enable;

	ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void*)traffic_nodes, traffic_size);
    if (ret_value < 0){
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
        if(new_nodes){
		    free_xml_node((void*)&new_nodes);
        }
        if(traffic_nodes){
            free_xml_node((void*)&traffic_nodes);
        }
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
		firewalls_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		firewalls_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		firewalls_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		firewalls_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		firewalls_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		firewalls_remove_save();
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		firewalls_move_save();
	}
	else if(strcmp(submit_type,"status") == 0){//禁用，启用
		firewalls_status_save();
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

