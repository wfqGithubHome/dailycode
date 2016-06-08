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
        case MGTCGI_INTERFACE_ID:			// 2,�ӿ���Ϣ
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
		case MGTCGI_TOTALSESSIONS_ID:			//30,�����������
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,��������
			break;

		case MGTCGI_FIREWALLS_ID:				//32,Ӧ�ò����
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID: 		//33,����ͨ��
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,����ͨ��
			break;
			
		case MGTCGI_PORTMIRRORS_ID: 			//35,�������
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS�ܿز���
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTPĿ¼����
			break;

		case MGTCGI_URLFORMATS_ID:				//38,����URL��ʽ
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
		case MGTCGI_INTERFACE_ID:				// 3, ����ӿ�
			interface_edit_save();
			break;
			
		case MGTCGI_TOTALSESSIONS_ID:			//30,�����������
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,��������
			break;

		case MGTCGI_FIREWALLS_ID:				//32,Ӧ�ò����
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID: 		//33,����ͨ��
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,����ͨ��
			break;
			
		case MGTCGI_PORTMIRRORS_ID: 			//35,�������
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS�ܿز���
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTPĿ¼����
			break;

		case MGTCGI_URLFORMATS_ID:				//38,����URL��ʽ
			urlformats_edit_save();
			break;
        /************************ϵͳά������****************************/		
		case MGTCGI_SYS_DATE_ID:				//53,ϵͳ���ں�ʱ��
			sys_date_setting();
			break;
            
		case MGTCGI_SYS_REBOOT_ID:				//54,ϵͳ����
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
		case MGTCGI_TOTALSESSIONS_ID:			//30,�����������
			goto PARAM_ERROR;
			break;

		case MGTCGI_IPSESSIONS_ID:				//31,��������
			ipsessions_move_save();
			break;

		case MGTCGI_FIREWALLS_ID:				//32,Ӧ�ò����
			firewalls_move_save();
			break;
			
		case MGTCGI_TRAFFICCHANNELS_ID:			//33,����ͨ��
			break;
			
		case MGTCGI_SHARECHANNELS_ID:			//34,����ͨ��
			break;
			
		case MGTCGI_PORTMIRRORS_ID:				//35,�������
			portmirrors_move_save();
			break;
			
		case MGTCGI_DNSPOLICYS_ID:				//36,DNS�ܿز���
			dnspolicys_move_save();
			break;
			
		case MGTCGI_HTTPDIRPOLICYS_ID:			//37,HTTPĿ¼����
			httpdirpolicys_move_save();
			break;
        case MGTCGI_FIREWALLPOLICT_ID:			//40,Ӧ�÷���
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
	
		case MGTCGI_BRIDGE_NUM_ID:			// 4 ������
			get_sys_bridge_num();
			break;

		case MGTCGI_DEVICE_VERSION_ID:		//5 �豸�汾��Ϣ
			device_version_show();
			break;
		case MGTCGI_INTERFACE_NAME_ID:		//6 ��ȡ�ӿ�����
			get_sys_interface_name();
			break;

		case MGTCGI_SYS_PROTOINFO_ID:		//50 ������汾��Ϣ
			l7proto_version_show();
			break;
		case MGTCGI_SYS_LICENSE_ID:			//51 licese
			license_version_show();
			break;
		case MGTCGI_SYS_FIRMWARE_ID:		//56 firmware�汾��Ϣ
			firmware_version_show();
			break;
			
		case MGTCGI_SYS_DATE_ID:			//53,ϵͳʱ��
			sys_date_info_show();
			break;

              case MGTCGI_SYS_ONLINE_DATE_ID:			//57,����ʱ��
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
	/************************ϵͳά������****************************/
	switch(id)
	{
		case MGTCGI_SYS_DATE_ID:				//53,ϵͳ���ں�ʱ��
			sys_date_setting();
			break;
		case MGTCGI_SYS_REBOOT_ID:				//54,ϵͳ����
			sys_reboot_setting();
			break;
		case MGTCGI_SYS_PASSWD_ID:				//54,ϵͳ����
			sys_change_admin_passwd();
			break;
		case MGTCGI_SYS_BACK_UP_ID:              //59,���浱ǰ���������ļ�
		    sys_config_back_up();
            break;
        case MGTCGI_SYS_RESTORE_ID:             //60, ��ԭ�����ļ�
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
		case MGTCGI_SYS_PROTOINFO_ID:			//50,������		
			sys_protoinfo_import();
			break;
		case MGTCGI_SYS_FIRMWARE_ID:
			sys_firmware_import();				//56,�̼�
			break;
		case MGTCGI_SYS_LICENSE_ID:				//51,Licenseע��
			sys_license_import();
			break;
		case MGTCGI_SYS_XMLCONFIG_ID:			//52,�����ļ����뵼��
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
		//printf("Location:%s?iserror:1",url);//��������URL
		goto EXIT;
	EXIT:
		return;
}

void mgtcgi_export_file(int id)
{
	switch(id)
	{

		case MGTCGI_SYS_XMLCONFIG_ID:			//52,�����ļ���
			sys_xmconfig_export();
			break;

		case MGTCGI_SYS_URL_FILTER_ID:
			url_filter_export();					//60,url�����ļ�����
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
		case MGTCGI_SYS_PROTOINFO_ID:			//52,�����ļ���
			signature_remote_update();
			break;
        case MGTCGI_SYS_FIRMWARE_ID:			//52,�����ļ���
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

	if (strcmp(submit_type,"list") == 0 ){//�б���ʾ����
		mgtcgi_list_show(id);
	}	
	else if(strcmp(submit_type,"add") == 0 ){//������
		mgtcgi_add_show(id);
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		mgtcgi_edit_show(id);
	}
	else if(strcmp(submit_type,"addsave") == 0){//���"ȷ��"
		mgtcgi_add_save(id);
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		mgtcgi_edit_save(id);
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		mgtcgi_remove_save(id);
	}
	else if(strcmp(submit_type,"move") == 0){//����
		mgtcgi_move_save(id);
	}
	else if(strcmp(submit_type,"info") == 0){//������ʾ��
		mgtcgi_info_show(id);
	}
	else if(strcmp(submit_type,"setting") == 0){//����ϵͳ
		mgtcgi_sys_setting(id);
	}
	else if(strcmp(submit_type,"import") == 0){//����
		mgtcgi_import_file(id);
	}
	else if(strcmp(submit_type,"export") == 0){//����
		mgtcgi_export_file(id);
	}
    else if(strcmp(submit_type,"update") == 0){//�Զ�����
		mgtcgi_remote_update(id);
	}
    else if(strcmp(submit_type,"cmd") == 0){//�Զ�����
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


