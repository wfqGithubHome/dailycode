#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/route_xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
#include "../include/mgtcgi_debug.h"
#include "mgtcgi_route_common.c"
#include "mgtcgi_route_xmlparser.c"

void mgtcgi_list_show(int id)
{
	switch(id)
	{
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_list_show();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_list_show();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_list_show();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_list_show();
			break;
        case MGTCGI_ROUTE_MACVLAN_ID:
			route_macvlan_list_show();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_list_show();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_list_show();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
			route_vpnclient_list_show();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_list_show();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_list_show();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_list_show();
			break;
		case MGTCGI_ROUTE_DNAT_ID:
			route_dnat_list_show();
			break;
		case MGTCGI_ROUTE_SNAT_ID:	
			route_snat_list_show();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_list_show();
			break;
		case MGTCGI_ROUTE_MACBIND_ID:
			route_macbind_list_show();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_list_show();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_list_show();
			break;	
		case MGTCGI_ROUTE_DDNS_ID:
			route_ddns_list_show();
            break;
        case MGTCGI_ROUTE_PPTPDVPN_ID:
            route_pptpd_server_list_show();
			break;	
        case MGTCGI_ROUTE_PPTPD_ACCOUNT_ID:
            route_pptpd_account_list_show();
            break;
        case MGTCGI_WAN_CHANNEL_ID:
            wan_channel_list_show();
			break;
        case MGTCGI_ROUTING_TABLES_ID:
            route_tables_list_show();
			break;
        case MGTCGI_ROUTING_INTERFACELINK_ID:
            route_interfacelink_list_show();
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
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_add_show();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_add_show();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_add_show();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_add_show();
			break;
        case MGTCGI_ROUTE_MACVLAN_ID:
			route_macvlan_add_show();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_add_show();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_add_show();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
			route_vpnclient_add_show();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_add_show();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_add_show();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_add_show();
			break;
        case MGTCGI_ROUTE_DNAT_ID:
            route_dnat_add_show();
            break;
		case MGTCGI_ROUTE_SNAT_ID:
			route_snat_add_show();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_add_show();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_add_show();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_add_show();
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
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_edit_show();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_edit_show();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_edit_show();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_edit_show();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_edit_show();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_edit_show();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
			route_vpnclient_edit_show();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_edit_show();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_edit_show();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_edit_show();
			break;
		case MGTCGI_ROUTE_DNAT_ID:
		//	route_dnat_edit_show();
			break;
		case MGTCGI_ROUTE_SNAT_ID:	
		//	route_snat_edit_show();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_edit_show();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_edit_show();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_edit_show();
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
    int lock;
    lock = get_route_save_lock();
    if(lock < 0)
        goto EXIT;
	switch(id)
	{
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_add_save();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_add_save();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_add_save();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_add_save();
			break;
        case MGTCGI_ROUTE_MACVLAN_ID:
			route_macvlan_add_save();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_add_save();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_add_save();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
		//	route_vpnclient_add_save();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_add_save();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_add_save();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_add_save();
			break;
        case MGTCGI_ROUTE_DNAT_ID:
            route_dnat_add_save();
            break;
		case MGTCGI_ROUTE_SNAT_ID:	
			route_snat_add_save();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_add_save();
			break;
		case MGTCGI_ROUTE_MACBIND_ID:
			route_macbind_add_save();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_add_save();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_add_save();
			break;
        case MGTCGI_ROUTE_PPTPD_ACCOUNT_ID:
            route_pptpdvpn_account_add_save();
            break;

		default:
			goto PARAM_ERROR;
			break;
	}
	goto EXIT;


	PARAM_ERROR:
		printf("{\"iserror\":2,\"msg\":\"parameter error\"}");
		goto EXIT;
	EXIT:
        if(lock >= 0)
            put_route_save_lock(lock);
		return;
}

void mgtcgi_edit_save(int id)
{
    int lock;
    lock = get_route_save_lock();
    if(lock < 0)
        goto EXIT;
    
	switch(id)
	{
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_edit_save();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_edit_save();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_edit_save();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_edit_save();
			break;
        case MGTCGI_ROUTE_MACVLAN_ID:
			route_macvlan_edit_save();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_edit_save();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_edit_save();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
	//		route_vpnclient_edit_save();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_edit_save();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_edit_save();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_edit_save();
			break;
        case MGTCGI_ROUTE_DNAT_ID:
            route_dnat_edit_save();
            break;
		case MGTCGI_ROUTE_SNAT_ID:	
			route_snat_edit_save();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_edit_save();
			break;
		case MGTCGI_ROUTE_MACBIND_ID:
			route_macbind_edit_save(NULL);
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_edit_save();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_edit_save();
			break;
		case MGTCGI_ROUTE_DDNS_ID:
			route_ddns_edit_save();
            break;
        case MGTCGI_ROUTE_PPTPDVPN_ID:
            route_pptpvpn_edit_save();
		    break;	
        case MGTCGI_WAN_CHANNEL_ID:
            route_wan_channel_type_edit_save();
            break;
        case MGTCGI_ROUTING_INTERFACELINK_ID:
            route_interfacelink_edit_save();
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
        if(lock >= 0)
            put_route_save_lock(lock);
		return;

}

void mgtcgi_remove_save(int id)
{    
    int lock;
    lock = get_route_save_lock();
    if(lock < 0)
        goto EXIT;
	switch(id)
	{
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_remove_save();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_remove_save();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_remove_save();
			break;
		case MGTCGI_ROUTE_VLAN_ID:
			route_vlan_remove_save();
			break;
        case MGTCGI_ROUTE_MACVLAN_ID:
			route_macvlan_remove_save();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_remove_save();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_remove_save();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
		//	route_vpnclient_remove_save();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_remove_save();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_remove_save();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_remove_save();
			break;
        case MGTCGI_ROUTE_DNAT_ID:
            route_dnat_remove_save();
            break;
		case MGTCGI_ROUTE_SNAT_ID:	
			route_snat_remove_save();
			break;
		case MGTCGI_ROUTE_LOAD_BALANCING_ID:
			route_loadb_remove_save();
			break;
		case MGTCGI_ROUTE_MACBIND_ID:
			route_macbind_remove_save();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_remove_save();
			break;
		case MGTCGI_ROUTE_DHCPD_HOST_ID:
			route_dhcpd_host_remove_save();
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
        if(lock >= 0)
            put_route_save_lock(lock);
		return;
}

void mgtcgi_move_save(int id)
{
    int lock;
    lock = get_route_save_lock();
    if(lock < 0)
        goto EXIT;
	switch(id)
	{
		case MGTCGI_ROUTE_DNS_ID:
			route_dns_move_save();
			break;
		case MGTCGI_ROUTE_LAN_ID:
			route_iflan_move_save();
			break;
		case MGTCGI_ROUTE_WAN_ID:
			route_ifwan_move_save();
			break;
		case MGTCGI_PPPD_AUTH_ID:
			route_pppdauth_move_save();
			break;
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adslclient_move_save();
			break;
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
			route_vpnclient_move_save();
			break;
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_move_save();
			break;
		case MGTCGI_ROUTE_RTABLE_ID:
			route_rtable_move_save();
			break;
		case MGTCGI_ROUTE_RRULE_ID:
			route_rrule_move_save();
			break;
		case MGTCGI_ROUTE_DNAT_ID:
		//	route_dnat_move_save();
			break;
		case MGTCGI_ROUTE_SNAT_ID:	
		//	route_snat_move_save();
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
        if(lock >= 0)
            put_route_save_lock(lock);
		return;
}

void mgtcgi_apply(int id)
{
	switch(id)
	{
		case MGTCGI_ROUTE_ROUTING_ID:
			route_routing_apply();
			break;
        case MGTCGI_WAN_CHANNEL_ID:
			ifwan_channel_apply();
			break;  
        case MGTCGI_ROUTE_DNAT_ID:
			dnat_apply();
			break;
        case MGTCGI_ROUTE_SNAT_ID:
			snat_apply();
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

void mgtcgi_cmd(int id)
{
    int lock;
    lock = get_route_save_lock();
    if(lock < 0)
        goto EXIT;
	switch(id)
	{
		case MGTCGI_ROUTE_ADSL_CLIENT_ID:
			route_adsl_cmd();
			break;	
		case MGTCGI_ROUTE_L2TPVPN_CLIENT_ID:
			route_l2tpvpn_cmd();
			break;	
		case MGTCGI_ROUTE_MACBIND_ID:
			route_macbind_learnt_cmd();
			break;
		case MGTCGI_ROUTE_DHCPD_ID:
			route_dhcpd_enable_cmd();
			break;
        case MGTCGI_ROUTE_PPTPDVPN_ID:
            route_pptpd_enable_cmd();
            break;
        case MGTCGI_WAN_CHANNEL_ID:
            ifwan_channel_enable_cmd();
            break;
        case MGTCGI_ROUTING_TABLES_ID:
            route_tables_enable_cmd();
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
    if(lock >= 0)
        put_route_save_lock(lock);
	return;

}


void mgtcgi_info_show(int id)
{
	switch(id)
	{
	
		case MGTCGI_TOOLS_PRING:
			route_tools_ping();
			break;
		case MGTCGI_TOOLS_TRACEROUTE:
			route_tools_traceroute();
			break;

		case MGTCGI_CHART_NET_DEV:
			route_chart_netdev();
			break;

        case MGTCGI_DHCP_LEASE_INFO_ID:
            route_dhcp_lease_info();
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

}

void mgtcgi_import_file(int id)
{
	char url[URL_LENGTH]={"0"};
	cgiFormString("url", url, sizeof(url));
	switch(id)
	{
		case MGTCGI_ROUTE_MACBIND_ID:
			sys_macbind_import();
			break;
        case MGTCGI_ROUTE_XMLCONFIG_ID:
			sys_route_xmlconfig_import();
			break;
        case MGTCGI_ROUTING_TABLES_ID:
			route_tables_xmlconfig_import();
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
		case MGTCGI_ROUTE_MACBIND_ID:
			sys_macbind_export();
			break;
       case MGTCGI_ROUTE_XMLCONFIG_ID:
			sys_route_xmlconfig_export();
			break;
			
		default:
			break;
	}

	return;
}

extern int check_cgifrom_digit(int value, int min, int max);

int cgiMain()
{
	char submit_type[STRING_LENGTH]={"edit"};
	char id_str[DIGITAL_LENGTH]={"81"};
	char url[URL_LENGTH]={0};
	int	 id,ret_value=0,iserror=0;

	//snprintf(cmd, sizeof(cmd), ROUTE_BACKUP_XMLTMP_CMD);
	//system(cmd);

	cgiFormString("url", url, sizeof(url));
	cgiFormString("submit",submit_type,STRING_LENGTH);
	cgiFormString("ID",id_str,DIGITAL_LENGTH);
    
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
	else if(strcmp(submit_type,"apply") == 0){ //应用
		mgtcgi_apply(id);
	}
	else if(strcmp(submit_type,"cmd") == 0){ //应用
		mgtcgi_cmd(id);
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
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	goto CGIC_EXIT;

	ERROR_EXIT:
		if (strlen(url) < 1)
			printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		else
			printf("<script>window.location='%s?iserror=1';</script>",url); 
		goto CGIC_EXIT;

	CGIC_EXIT:
		return (0);
}


