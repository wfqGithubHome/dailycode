#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

extern void interface_list_show(void);
extern void urlformats_edit_show(void);
extern void interface_edit_save(void);
extern void urlformats_edit_save(void);
extern void sys_date_setting(void);
extern void sys_reboot_setting(void);
extern void ipsessions_move_save(void);
extern void firewalls_move_save(void);
extern void portmirrors_move_save();
extern void dnspolicys_move_save();
extern void httpdirpolicys_move_save(void);
extern void get_sys_bridge_num(void);
extern void device_version_show(void);
extern void get_sys_interface_name(void);
extern void l7proto_version_show(void);
extern void license_version_show(void);
extern void firmware_version_show(void);
extern void sys_date_info_show(void);
extern void sys_online_date_info_show(void);
extern void signature_auto_update_show(void);
extern void sys_change_admin_passwd(void);
extern void sys_protoinfo_import();
extern void sys_firmware_import();
extern void sys_license_import(void);
extern void sys_xmconfig_import(void);
extern void sys_xmconfig_export(void);
extern void signature_remote_update(void);
extern void firmware_remote_update(void);
extern void signature_signature_autoupdate_cmd(void);
extern int check_cgifrom_digit(int value, int min, int max);
extern void sys_config_back_up(void);
extern void sys_config_restore(void);
extern void networkdetection_list_show(void);
extern void networkdetection_status_save(void);
extern void sys_update_url(void);
extern void url_filter_move_save(void);
extern void sys_url_filter_import(void);
extern void url_filter_export(void);
extern void url_filter_addsave(void);
extern void urlfilter_list_show(void);
extern void ac_server_info_show(void);
extern void sys_reset_setting(void);
extern void adsl_log_show(void);
extern void adsl_status_log_empty(void);
extern int check_string_digit(char *str);
extern void sys_service_update_inport(void);

//add by chenhao,2016-3-29
void firewall_policy_list_show(void);
void layer7protocol_type_list_show();
void firewall_policy_add_show(void);
void firewall_policy_edit_save(void);
void firewall_policy_add_save(void);
void firewall_policy_move_save(void);
void firewall_policy_remove_save(void);
void firewall_policy_cmd(void);

void mgtcgi_list_show(int id)
{
    switch(id)
    {
        case MGTCGI_INTERFACE_ID:			// 2,接口信息
        	interface_list_show();
        	break;
        case MGTCGI_NETWORKDETECTION_ID:
        	networkdetection_list_show();
        	break;	    
        case MGTCGI_SYS_URL_FILTER_ID:
            urlfilter_list_show();
            break;
        case MGTCGI_LOG_ADSL_ID:
            adsl_log_show();
            break;
        case MGTCGI_FIREWALLPOLICT_ID:
			firewall_policy_list_show();
			break;
        case MGTCGI_LAYER7PROTOCOLSTYPE_ID:
            layer7protocol_type_list_show();
            break;
        default:
        	goto PARAM_ERROR;
        	break;
    }
    goto EXIT;

    PARAM_ERROR:
    printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
    goto EXIT;
    EXIT:
    return;

}

void mgtcgi_add_show(int id)
{
	switch(id)
	{
		case MGTCGI_FIREWALLPOLICT_ID:
            firewall_policy_add_show();
			break;
			
		default:
			goto PARAM_ERROR;
			break;
    }
    goto EXIT;

PARAM_ERROR:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;			
}

void mgtcgi_edit_show(int id)
{
	switch(id)
	{
		case MGTCGI_TOTALSESSIONS_ID:			//30,设置总体参数
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,网络层策略
			break;

		case MGTCGI_FIREWALLS_ID:				//32,应用层策略
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID: 		//33,流量通道
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,共享通道
			break;
			
		case MGTCGI_PORTMIRRORS_ID: 			//35,镜像策略
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS管控策略
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTP目录策略
			break;

		case MGTCGI_URLFORMATS_ID:				//38,设置URL格式
			urlformats_edit_show();
			break;
			
		default:
			goto PARAM_ERROR;
			break;
    }
    goto EXIT;

PARAM_ERROR:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}

void mgtcgi_add_save(int id)
{
	switch(id)
	{
		case MGTCGI_SYS_URL_FILTER_ID:
			url_filter_addsave();
			break;
        case MGTCGI_FIREWALLPOLICT_ID:
			firewall_policy_add_save();
			break;
    	default:
    		goto ERROR_PARAM;
	}

	goto EXIT;
ERROR_PARAM:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}

void mgtcgi_status_save(int id)
{
	switch(id)
	{
		case MGTCGI_NETWORKDETECTION_ID:
			networkdetection_status_save();
			break;
    	default:
    		goto ERROR_PARAM;
	}

	goto EXIT;
ERROR_PARAM:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}

void mgtcgi_edit_save(int id)
{
	switch(id)
	{
		case MGTCGI_INTERFACE_ID:				// 3, 网络接口
			interface_edit_save();
			break;
			
		case MGTCGI_TOTALSESSIONS_ID:			//30,设置总体参数
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,网络层策略
			break;

		case MGTCGI_FIREWALLS_ID:				//32,应用层策略
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID: 		//33,流量通道
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,共享通道
			break;
			
		case MGTCGI_PORTMIRRORS_ID: 			//35,镜像策略
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS管控策略
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTP目录策略
			break;

		case MGTCGI_URLFORMATS_ID:				//38,设置URL格式
			urlformats_edit_save();
			break;
        /************************系统维护部分****************************/		
		case MGTCGI_SYS_DATE_ID:				//53,系统日期和时间
			sys_date_setting();
			break;
            
		case MGTCGI_SYS_REBOOT_ID:				//54,系统重启
			sys_reboot_setting();
			break;
        case MGTCGI_FIREWALLPOLICT_ID:
			firewall_policy_edit_save();
			break;
			
		default:
			goto PARAM_ERROR;
			break;
	}
    
    goto EXIT;
PARAM_ERROR:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}

void mgtcgi_remove_save(int id)
{
    switch(id)
    {
        case MGTCGI_SYS_URL_FILTER_ID:
            url_filter_move_save();
            break;
        case MGTCGI_LOG_ADSL_ID:
            adsl_status_log_empty();
            break;
        case MGTCGI_FIREWALLPOLICT_ID:
			firewall_policy_remove_save();
			break;
        default:
            goto PARAM_ERROR;
        break;
    }
    goto EXIT;

PARAM_ERROR:
    printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
    goto EXIT;
EXIT:
    return;
}

void mgtcgi_move_save(int id)
{
	switch(id)
	{
		case MGTCGI_TOTALSESSIONS_ID:			//30,设置总体参数
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,网络层策略
			ipsessions_move_save();
			break;

		case MGTCGI_FIREWALLS_ID:				//32,应用层策略
			firewalls_move_save();
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID:			//33,流量通道
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,共享通道
			break;
			
		case MGTCGI_PORTMIRRORS_ID:				//35,镜像策略
			portmirrors_move_save();
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS管控策略
			dnspolicys_move_save();
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTP目录策略
			httpdirpolicys_move_save();
			break;
        case MGTCGI_FIREWALLPOLICT_ID:			//40,应用分流
			firewall_policy_move_save();
			break;
			
		default:
			goto PARAM_ERROR;
			break;
	}
	goto EXIT;

PARAM_ERROR:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}


void mgtcgi_info_show(int id)
{
	switch(id)
	{
	
		case MGTCGI_BRIDGE_NUM_ID:			// 4 桥数量
			get_sys_bridge_num();
			break;

		case MGTCGI_DEVICE_VERSION_ID:		//5 设备版本信息
			device_version_show();
			break;
		case MGTCGI_INTERFACE_NAME_ID:		//6 获取接口名称
			get_sys_interface_name();
			break;

		case MGTCGI_SYS_PROTOINFO_ID:		//50 特征库版本信息
			l7proto_version_show();
			break;
		case MGTCGI_SYS_LICENSE_ID:			//51 licese
			license_version_show();
			break;
		case MGTCGI_SYS_FIRMWARE_ID:		//56 firmware版本信息
			firmware_version_show();
			break;
			
		case MGTCGI_SYS_DATE_ID:			//53,系统时间
			sys_date_info_show();
			break;

              case MGTCGI_SYS_ONLINE_DATE_ID:			//57,在线时间
			sys_online_date_info_show();
			break;

              case MGTCGI_AUTOUPDATE_ENABLE_ID:
                     signature_auto_update_show();
                     break;
                     
              case MGTCGI_SYS_UPDATE_URL_ID:
                    ac_server_info_show();
                    break;
			
		default:
			goto PARAM_ERROR;
			break;
	}
	goto EXIT;

	PARAM_ERROR:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto EXIT;
	EXIT:
		return;
}

void mgtcgi_sys_setting(int id)
{
	/************************系统维护部分****************************/
	switch(id)
	{
		case MGTCGI_SYS_DATE_ID:				//53,系统日期和时间
			sys_date_setting();
			break;
		case MGTCGI_SYS_REBOOT_ID:				//54,系统重启
			sys_reboot_setting();
			break;
		case MGTCGI_SYS_PASSWD_ID:				//54,系统重启
			sys_change_admin_passwd();
			break;
		case MGTCGI_SYS_BACK_UP_ID:              //59,保存当前所有配置文件
		    sys_config_back_up();
            break;
        case MGTCGI_SYS_RESTORE_ID:             //60, 还原配置文件
            sys_config_restore();
            break;
		case MGTCGI_SYS_UPDATE_URL_ID:
			sys_update_url();
			break;
        case MGTCGI_SYS_RESET_ID:
            sys_reset_setting();
            break;
            
		default:
			goto PARAM_ERROR;
			break;
	}
	goto EXIT;

	PARAM_ERROR:
		printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
		goto EXIT;
	EXIT:
		return;
}

void mgtcgi_import_file(int id)
{
	char url[URL_LENGTH]={"0"};
	cgiFormString("url", url, sizeof(url));
	switch(id)
	{
		case MGTCGI_SYS_PROTOINFO_ID:			//50,特征库		
			sys_protoinfo_import();
			break;
		case MGTCGI_SYS_FIRMWARE_ID:
			sys_firmware_import();				//56,固件
			break;
		case MGTCGI_SYS_LICENSE_ID:				//51,License注册
			sys_license_import();
			break;
		case MGTCGI_SYS_XMLCONFIG_ID:			//52,配置文件导入导出
			sys_xmconfig_import();
			break;
        case MGTCGI_SYS_URL_FILTER_ID:
            sys_url_filter_import();
            break;
        case MGTCGI_SYS_SERVICE_UPDATE_ID:
            sys_service_update_inport();
            break;
		default:
			goto PARAM_ERROR;
			break;
	}
	goto EXIT;

	PARAM_ERROR:
		printf("<script>window.location='%s?iserror=1';</script>",url); 
		//printf("Location:%s?iserror:1",url);//返回引用URL
		goto EXIT;
	EXIT:
		return;
}

void mgtcgi_export_file(int id)
{
	switch(id)
	{

		case MGTCGI_SYS_XMLCONFIG_ID:			//52,配置文件出
			sys_xmconfig_export();
			break;

		case MGTCGI_SYS_URL_FILTER_ID:
			url_filter_export();					//60,url过滤文件导出
			break;
			
		default:
			break;
	}

	return;
}

void mgtcgi_remote_update(int id)
{
    switch(id)
	{
		case MGTCGI_SYS_PROTOINFO_ID:			//52,配置文件出
			signature_remote_update();
			break;
        case MGTCGI_SYS_FIRMWARE_ID:			//52,配置文件出
			firmware_remote_update();
			break;  
		default:
			break;
	}

	return;
}

void mgtcgi_command(int id)
{
    switch(id)
	{

		case MGTCGI_AUTOUPDATE_ENABLE_ID:			
			signature_signature_autoupdate_cmd();
			break;
        case MGTCGI_FIREWALLPOLICT_ID:
			firewall_policy_cmd();
			break;
		default:
			break;
	}

	return;
}

int cgiMain()
{
	char submit_type[STRING_LENGTH]={"editsave"};
	char id_str[DIGITAL_LENGTH]={"3"};
	char url[URL_LENGTH]={0};
	int	 id,ret_value=0,iserror=0;
	char cmd[CMD_LENGTH];

	snprintf(cmd, sizeof(cmd), BACKUP_XMLTMP_CMD);
	system(cmd);

	cgiFormString("url", url, sizeof(url));
	cgiFormString("submit",submit_type,STRING_LENGTH);
	cgiFormString("id",id_str,DIGITAL_LENGTH);

	if (strcmp(submit_type, "import") == 0){
		printf("content-type:text/html\n\n");
	}
	else if (strcmp(submit_type, "export") == 0){
		;
	}
	else {
#if _MGTCGI_DEBUG_GET_
		printf("content-type:text/html\n\n");		
#else
		printf("content-type:application/json\n\n");	
#endif	
	}

	id = atoi(id_str);
	ret_value = check_cgifrom_digit(id, 1, 256);
	if (ret_value != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strcmp(submit_type,"list") == 0 ){//列表显示所有
		mgtcgi_list_show(id);
	}	
	else if(strcmp(submit_type,"add") == 0 ){//点击添加
		mgtcgi_add_show(id);
	}
	else if(strcmp(submit_type,"edit") == 0){//点击编辑
		mgtcgi_edit_show(id);
	}
	else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
		mgtcgi_add_save(id);
	}
	else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
		mgtcgi_edit_save(id);
	}
	else if(strcmp(submit_type,"remove") == 0){//点击删除
		mgtcgi_remove_save(id);
	}
	else if(strcmp(submit_type,"move") == 0){//排序
		mgtcgi_move_save(id);
	}
	else if(strcmp(submit_type,"info") == 0){//单个显示的
		mgtcgi_info_show(id);
	}
	else if(strcmp(submit_type,"setting") == 0){//设置系统
		mgtcgi_sys_setting(id);
	}
	else if(strcmp(submit_type,"import") == 0){//导入
		mgtcgi_import_file(id);
	}
	else if(strcmp(submit_type,"export") == 0){//导出
		mgtcgi_export_file(id);
	}
    else if(strcmp(submit_type,"update") == 0){//自动更新
		mgtcgi_remote_update(id);
	}
    else if(strcmp(submit_type,"cmd") == 0){//自动更新
		mgtcgi_command(id);
	}
	else if(strcmp(submit_type,"status") == 0){
		mgtcgi_status_save(id);
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	goto CGIC_EXIT;

	ERROR_EXIT:
		if (strlen(url) < 1)
			printf("{\"iserror\":%d,\"msg\":\"\",\"url\":\"%s\"}",iserror,url);
		else
			printf("<script>window.location='%s?iserror=1';</script>",url); 
		goto CGIC_EXIT;

	CGIC_EXIT:
		return (0);
}


