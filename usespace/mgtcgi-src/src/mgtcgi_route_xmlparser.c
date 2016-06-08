#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

#define CGI_FORM_STRING		1
char * flash_route_xmlconf = "/flash/etc/route_config.xml";
char * flash_mac_bind_xmlconf = "/flash/etc/mac_bind.xml";
char * flash_iface_xmlconfig = "/flash/etc/iface.xml";
char * working_route_xmlconf = "/etc/route_config.xml";
char * working_mac_bind_xmlconf = "/etc/mac_bind.xml";
char * working_iface_xmlconfig = "/etc/iface.xml";
char *working_wanchannel_xmlconf = "/etc/channel.xml";
char *flash_wanchannel_xmlconf = "/flash/etc/channel.xml";

char *iface_change_report = "/usr/local/sbin/route_report -b >/dev/null 2>&1";

const struct ptotocol_id proto_ids[10] = {
    {PROTOCOL_ID_BEGIN,"null"},
    {PROTOCOL_ID_TCP,"tcp"},
    {PROTOCOL_ID_UDP,"udp"},
    {PROTOCOL_ID_ICMP,"icmp"},
    {PROTOCOL_ID_ALL,"all"},
    {PROTOCOL_ID_END,"null"},
};
int check_ipaddr_dot(const char *ipaddr, const char *dot);
int get_link_detected(const char *ifname);
int get_link_speed(const char *ifname);
int check_interface_list(const char *string);

inline int check_cgiform_string(char *string)
{
	if (strlen(string) < STRING_LITTLE_LENGTH){
		return -1;
	}
	return 0;
}

inline int check_vlan_name(const char *checkname, const char *head){	
	int retval=0,i=0,sign=1;
	char buff[STRING_LENGTH]={0};
	
	if (NULL == checkname){
		return -1;
	} 
	for (i=0; i < strlen(checkname); i++){
		if (i >= strlen(head)){
            if(!((checkname[i] >= '0') && (checkname[i] <= '9'))){
                sign=0;
                break;
            }
		}
		else {
			buff[i] = checkname[i];
		}
	}
	
	if ((strcmp(buff, head) == 0) && sign)
		retval = 0;
	else
		retval = -1;
	
	return retval;
}

inline int check_adsl_name(const char *name){	
	int ret_value=0,i=0;
	char buffer[20]={0};
	if (NULL == name){
		return -1;
	} 
	
	for (i=0; i < strlen(name); i++){
		if (i >= 4)
			break;
		buffer[i] = name[i];
	}
	buffer[i] = '\0';
	if (strcmp(buffer, "adsl") == 0)
		ret_value = 0;
	else
		ret_value = -1;
	return ret_value;
}

inline int check_passwd(const char *passwd){
    const char *tmp = passwd;
    char    c;
    
    if(NULL == passwd){
        return -1;
    }

    while((c = *tmp++)){        
        if(!isalnum(c)){
            return -1;
        }
    }

    return 0;
}

inline int check_l2tpvpn_name(const char *name){	
	int ret_value=0,i=0;
	char buffer[20]={0};
	if (NULL == name){
		return -1;
	} 
	
	for (i=0; i < strlen(name); i++){
		if (i >= 7)
			break;
		buffer[i] = name[i];
	}
	buffer[i] = '\0';
	if (strcmp(buffer, "l2tpvpn") == 0)
		ret_value = 0;
	else
		ret_value = -1;
    
	return ret_value;
}

inline int check_loadb_name(const char *name){	
	int ret_value=0,i=0;
	char buffer[20]={0};
	if (NULL == name){
		return -1;
	} 
		
	for (i=0; i < strlen(name); i++){
		if (i >= 7)
			break;
		buffer[i] = name[i];
	}
	buffer[i] = '\0';
	if (strcmp(buffer, "balance") == 0)
		ret_value = 0;
	else
		ret_value = -1;

	return ret_value;
}


//return: -1 error; 1 LAN; 2 WAN; 3 adsl; 4 l2tpvpn; 5 balance; 6 vlan
inline int check_interface_type(const char const *name){
	int ret_value=0,i=0;
	char buffer[20]={0};

	if (name == NULL){
		return -1;
	}	
	for (i=0; i < strlen(name); i++){
		if (i >= 3)
			break;
		buffer[i] = name[i];
	}
	
	buffer[i] = '\0';
	if (strcmp(buffer, "LAN") == 0)	//LAN口
		ret_value = 1;
	else if (strcmp(buffer, "WAN") == 0)//WAN口
		ret_value = 2;
	else if (strcmp(buffer, "ads") == 0)//adsl客服端
		ret_value = 3;
	else if (strcmp(buffer, "l2t") == 0)//l2tpvpn客服端
		ret_value = 4;
	else if (strcmp(buffer, "bal") == 0)//balance负载均衡接口
		ret_value = 5;
	else if(strcmp(buffer,"vla") == 0)//vlan接口
		ret_value = 6;
	else 
		ret_value = -1;
	
	return ret_value;
}

inline int check_pptpd_username(const char *username)
{
    if(!username)
        return -1;

    if(!(strlen(username) > 0 && strlen(username) < 17))
        return -1;
    
    return 0;
}

inline int check_pptpd_passwd(const char *passwd)
{
    if(!passwd)
        return -1;

    if(!(strlen(passwd) > 0 && strlen(passwd) < 17))
        return -1;

    return 0;
}

extern int check_ipaddr(const char *ipaddr);
extern int check_mask(char* subnet) ;

int check_table_dstip_list(const char *string, const char *dot)
{
	char ipaddr[STRING_LENGTH] = {0};
	//char str[STRING_LIST_LENGTH]={0};
	char * str = NULL;
	int  dot_num=0,i=0,j=0;

    str = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!str)
        return -1;
    memset(str,0,STRING_LIST_LENGTH*sizeof(char));
    strncpy(str, string, strlen(string));
	for (i=0,j=0; i < strlen(str); i++){
		if (str[i] == ','){
			if (j > 0){
				//printf("ipaddr:%s\n", ipaddr);
				if (check_ipaddr(ipaddr) != 0){
                    free(str);
					return -1;
				}
				
				memset(ipaddr, 0, sizeof(ipaddr));
				dot_num++;
			}
			
			j = 0;
			continue;
		}

		if (j < sizeof(ipaddr))
			ipaddr[j] = str[i];
		else{
            free(str);
            str = NULL;
			return -1;
		}

		j++;
	}

    if(str){
	    free(str);
        str = NULL;
    }
	return (dot_num);
}

int dhcp_ip_pool_check(const char *ip, const char *mask, const char *startip, const char *endip,
        const char *dhcp_gateway, const char *dhcp_mask)
{
    int iserror = -1;
    unsigned long int if_subnet,startip_subnet,endip_subnet,gatway_subnet;
    struct in_addr rang_ip,start_ip1,end_ip2,mask_ip3,gateway,dhcp_mask_ip4;
    
    if(!ip || !mask || !startip || !endip || !dhcp_gateway || !dhcp_mask)
        return iserror;

    if (inet_aton(ip, &rang_ip) == 0){
		goto FREE_EXIT;
	}
	if (inet_aton(startip, &start_ip1) == 0){
		goto FREE_EXIT;
	}
	if (inet_aton(endip, &end_ip2) == 0){
		goto FREE_EXIT;
	}
    if (inet_aton(mask, &mask_ip3) == 0){
		goto FREE_EXIT;
	}
    if (inet_aton(dhcp_gateway, &gateway) == 0){
		goto FREE_EXIT;
	}
    if (inet_aton(dhcp_mask, &dhcp_mask_ip4) == 0){
		goto FREE_EXIT;
	}

    if(ntohl(start_ip1.s_addr) > ntohl(end_ip2.s_addr)){
        goto FREE_EXIT;
    }

    if_subnet = (ntohl(rang_ip.s_addr) & ntohl(mask_ip3.s_addr));
    startip_subnet = (ntohl(start_ip1.s_addr) & ntohl(mask_ip3.s_addr));
    if(startip_subnet != if_subnet){
        goto FREE_EXIT;
    }
    
    endip_subnet = (ntohl(end_ip2.s_addr) & ntohl(mask_ip3.s_addr));
    if(endip_subnet != if_subnet){
        goto FREE_EXIT;
    }
    
    gatway_subnet = (ntohl(gateway.s_addr) & ntohl(dhcp_mask_ip4.s_addr));
    if(gatway_subnet != if_subnet){
        goto FREE_EXIT;
    }

    iserror = 0;
FREE_EXIT:
    return iserror;
}


int check_mac_bind_list(const char *string)
{
	int count=0,i=0;
	for (i=0; i < strlen(string); i++){
		if (string[i] == ';')
			count++;
	}
	return (count);
}

/*

static int ip_rang_check(unsigned long int ip_rang,char *alias,int length)
{
    int re_value = 0,ip_index = 0,mask_index = 0,sign = 1,i;
    char ip[STRING_LENGTH] = {0};
    char mask[STRING_LENGTH] = {0};
    unsigned long int if_rang;
    struct in_addr if_ip, if_mask;

    for (i = 0;i <= length;i++){
        if((alias[i] == '/') || (i == length)){
            sign++;
            continue;
        }

        if(sign == 1){
            ip[ip_index] = alias[i];
            ip_index++;
        }
        else if(sign == 2){
            mask[mask_index] = alias[i];
            mask_index++;
        }
    }
   log_debug("ip:%s,mask:%s\n",ip,mask);

    if (inet_aton(ip, &if_ip) == 0){
        re_value = -1;
        goto EXIT;
    }
    if (inet_aton(mask, &if_mask) == 0){
        re_value = -1;
        goto EXIT;
    }
    if_rang = (ntohl(if_ip.s_addr) & (0xffffff << (32 - atoi(mask))));
    log_debug("ip_rang:%ul,if_rang:%ul\n",ip_rang,if_rang);

    if(ip_rang != if_rang){
        re_value = -1;
        goto EXIT;
    }
    

EXIT:
    return re_value;
}

static int ip_alias_rang_check(unsigned long int ip_rang, char *ip_alias, int length)
{
    int ret_value = -1,index = 0,i;
    char alias[STRING_LENGTH] = {0};
    unsigned long int tmp_rang = ip_rang;
    log_debug("ip_rang:%ul\n",ip_rang);
    log_debug("1<<1:%d\n",1<<1);
    log_debug("0xff<<8:%x,0xffff<<8:%x\n",0xff<<8,0xffff<<8);
    if(ip_alias == NULL){
        goto EXIT;
    }
    log_debug("ip_alias:%s\n",ip_alias);

    for (i = 0;i <= length;i++){
        if((ip_alias[i] == ',') || (i == length)){
            log_debug("alias:%s\n",alias);
            if(ip_rang_check(tmp_rang,alias,strlen(alias)) == 0){
                ret_value = 0;
                goto EXIT;
            }
            
            index = 0;
            memset(alias,0,sizeof(alias));
            continue;
        }

        alias[index] = ip_alias[i];
        index++;
    }


EXIT:
    return ret_value;
}*/

int adsl_use_name_check(struct mgtcgi_xml_adsl_clients *adsl,
					const char *name, const char *old_name, int check_id)
{
	//返回值:0 没有被使用; 1 被使用; -1 参数错误		 
	int index=0,ret_value=0;
	
	if (check_id == 1){ //adsl-client, vpn-client
		for (index=0; index < adsl->num; index++){
			if (strcmp(adsl->pinfo[index].ifname, old_name) == 0){
				ret_value = 1;
				break;
			}
		}
	} 
	else {
		ret_value = -1;
	}
	return (ret_value);
}

int wans_ifname_check(struct mgtcgi_xml_interface_wans *wan_info, const char *name)
{
    int ret = 0,i;

    if(wan_info == NULL || name == NULL)
        return -1;

    for(i = 0; i < wan_info->num; i++){
        if(strcmp(wan_info->pinfo[i].ifname, name)== 0){
            ret = 1;
            break;
        }
    }

    return (ret);
}


int loadb_use_name_check(struct mgtcgi_xml_load_balancings *loadb,
					const char *name, const char *old_name, int check_id)
{
	//返回值:0 没有被使用; 1 被使用; -1 参数错误		 
	int index=0,ret_value=0;
	
	if (check_id == 1){ //adsl-client, vpn-client
		for (index=0; index < loadb->num; index++){
			if (strcmp(loadb->pinfo[index].ifname, old_name) == 0){
				ret_value = 1;
				break;
			}
		}
	} 
	else {
		ret_value = -1;
	}
	return (ret_value);
}

int routing_use_name_check(struct mgtcgi_xml_routings *routing,
					const char *name, const char *old_name, int check_id)
{
	//返回值:0 没有被使用; 1 被使用; -1 参数错误		 
	int index=0,ret_value=0;
	
	if (check_id == 1){	//adsl-client, vpn-client
		for (index=0; index < routing->num; index++){
			if (strcmp(routing->pinfo[index].ifname, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if(2 == check_id){	//table
		for (index=0; index < routing->num; index++){
			if (strcmp(routing->pinfo[index].table, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} 
	else if(3 == check_id){ //rule
		for (index=0; index < routing->num; index++){
			if (strcmp(routing->pinfo[index].rule, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	}
	else {
		ret_value = -1;
	}
	
	return (ret_value);
}

int route_rule_apply(int check,char *src,char *src_port,char *dst,char *dst_port,char *tos,char *protocol,char *action)
{
    int res = -1;
    char cmd[256] = {0};
    char cmd_str[256] = {0};
    const char *iptables_cmd = "/usr/local/sbin/iptables -t mangle -I zone_lan_prerouting";

    if(check == 0){
        return 0;
    }

    if(!src || !src_port || !dst || !dst_port || !tos || !protocol || !action){
        return res;
    }

    if(strcmp(action,"ACCEPT") == 0){
        return 0;
    }

    if(strlen(src) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -s %s ",src);
    }
    if(strlen(src_port) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m multiport --sport %s ",src_port);
    }
    if(strlen(dst) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -d %s ",dst);
    }
    if(strlen(dst_port) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m multiport --dport %s ",dst_port);
    }
    if((strlen(tos) != 0) && (strcmp(tos,"null") != 0)){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m dscp --dscp %s ",tos);
    }

    if(strcmp(protocol,"null") == 0){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
    }
    else if((strcmp(protocol,"tcp") == 0) || (strcmp(protocol,"udp") == 0)){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p %s %s -j DROP",iptables_cmd,protocol,cmd_str);
        system(cmd);
    }
    else if(strcmp(protocol,"all") == 0){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p tcp %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p udp %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
    }

    return 0;
}

int route_rule_remove(int check,char *src,char *src_port,char *dst,char *dst_port,char *tos, char *protocol,char *action)
{
    int res = -1;
    char cmd[256] = {0};
    char cmd_str[256] = {0};
    const char *iptables_cmd = "/usr/local/sbin/iptables -t mangle -D zone_lan_prerouting ";

    if(check == 0){
        return 0;
    }

    if(!src || !src_port || !dst || !dst_port || !tos || !protocol || !action){
        return res;
    }

    if(strcmp(action,"ACCEPT") == 0){
        return 0;
    }

    if(strlen(src) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -s %s ",src);
    }
    if(strlen(src_port) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m multiport --sport %s ",src_port);
    }
    if(strlen(dst) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -d %s ",dst);
    }
    if(strlen(dst_port) != 0){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m multiport --dport %s ",dst_port);
    }
    if((strlen(tos) != 0) && (strcmp(tos,"null") != 0)){
        snprintf(cmd_str+strlen(cmd_str),sizeof(cmd_str)-strlen(cmd_str)," -m dscp --dscp %s ",tos);
    }

    if(strcmp(protocol,"null") == 0){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
    }
    else if((strcmp(protocol,"tcp") == 0) || (strcmp(protocol,"udp") == 0)){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p %s %s -j DROP",iptables_cmd,protocol,cmd_str);
        system(cmd);
    }
    else if(strcmp(protocol,"all") == 0){
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p tcp %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd+strlen(cmd),sizeof(cmd)-strlen(cmd)," %s -p udp %s -j DROP",iptables_cmd,cmd_str);
        system(cmd);
    }

    return 0;
}


void print_ifname_list(struct mgtcgi_xml_interface_wans *wan, 
								struct mgtcgi_xml_interface_vlans *vlan)
{
	int index=0,flag=0;
	printf("\"wan_list\":[");
	if (NULL != wan){
		for (index=0; index < wan->num; index++){
#if 1
            if((strcmp(wan->pinfo[index].ifname,"WAN3") == 0) ||
                (strcmp(wan->pinfo[index].ifname,"WAN4") == 0)){
                continue;
            }
#endif		
			if (flag == 1)
				printf(",");
			else
				flag = 1;
			
			printf("\"%s\"",wan->pinfo[index].ifname);
		}
#if 1
        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN3\"");

        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN4\"");
#endif
    }


	
	if (NULL != vlan){
		for (index=0; index < vlan->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;

			printf("\"%s\"",vlan->pinfo[index].name);
		}
	}

	printf("]");
}


void print_ifwan_list(struct mgtcgi_xml_interface_wans *wan)
{
	int index=0,flag=0;
	printf("\"wan_list\":[");
	if (NULL != wan){
		for (index=0; index < wan->num; index++){
#if 1
            if((strcmp(wan->pinfo[index].ifname,"WAN3") == 0) ||
                (strcmp(wan->pinfo[index].ifname,"WAN4") == 0)){
                continue;
            }
#endif		
			if (flag == 1)
				printf(",");
			else
				flag = 1;
			
			printf("\"%s\"",wan->pinfo[index].ifname);
		}
#if 1
        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN3\"");
            
        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN4\"");
#endif
	}


	printf("]");
}

void printf_dhcpd_list(void *arg){
	int index=0;
	static int flag=0;
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;

	dhcpd = (struct mgtcgi_xml_dhcpd_servers *)arg;
	
	printf("\"dhpcd_list\":[");
	if (NULL != dhcpd){
		for (index=0; index < dhcpd->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;
			
			printf("\"%s\"",dhcpd->pinfo[index].name);
		}
	}
	printf("]");
    return;
}

void print_alias_info(char *alias)
{
    int dot;
    char ip[STRING_LENGTH] = {0};
    char dot_str[STRING_LENGTH] = {0};
    char mask[STRING_LENGTH] = {0};
    char *p = NULL;
    char *point = NULL;
    
    if(alias == NULL){
        return;
    }

    if(strlen(alias) == 0){
        return;
    }

    p = alias;
    while(p){
        while((point = strsep(&p,","))){
            sscanf(point,"%[0-9|.]/%[0-9]",ip,dot_str);
            dot = atoi(dot_str);
            maskbit_to_mask(dot,mask,sizeof(mask));
            printf(",[\"%s\",\"%s\"]",ip,mask);
        }
    }
}

void printf_lan_info(struct mgtcgi_xml_interface_lans *lan)
{
    int i,sep=0;
    
    if(NULL == lan){
        return;
    }

    printf("\"lan_info\":[");
    for(i = 0; i < lan->num; i++){
        printf("%s",(sep?",":""));
        sep=1;
        
        printf("[\"%s\",\"%s\"]",lan->pinfo[i].ip,lan->pinfo[i].mask);
        print_alias_info(lan->pinfo[i].aliases);
    }
    printf("]");
}

void print_interface_list(struct mgtcgi_xml_interface_wans *wan,
									 struct mgtcgi_xml_interface_lans *lan,
									 struct mgtcgi_xml_adsl_clients *adsl,
								     struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn,
								     struct mgtcgi_xml_load_balancings *loadb,
								     struct mgtcgi_xml_interface_vlans *vlan)
{
	int index=0;
	static int flag=0;

	printf("\"if_list\":[");
	
	if (NULL != wan){
		for (index=0; index < wan->num; index++){
#if 1
            if((strcmp(wan->pinfo[index].ifname,"WAN3") == 0) ||
                (strcmp(wan->pinfo[index].ifname,"WAN4") == 0)){
                continue;
            }
#endif		
			if (flag == 1)
				printf(",");
			else
				flag = 1;
			
			printf("\"%s\"",wan->pinfo[index].ifname);
		}
#if 1
        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN3\"");

        if (flag == 1)
			printf(",");
		else
			flag = 1;
        printf("\"WAN4\"");

#endif
	}



	if (NULL != lan){
		for (index=0; index < lan->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;

			printf("\"%s\"",lan->pinfo[index].ifname);
		}
	}

	if (NULL != adsl){
		for (index=0; index < adsl->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;

			printf("\"%s\"",adsl->pinfo[index].name);
		}
	}

	if (NULL != l2tpvpn){
		for (index=0; index < l2tpvpn->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;

			printf("\"%s\"",l2tpvpn->pinfo[index].name);
		}
	}

	if (NULL != loadb){
			for (index=0; index < loadb->num; index++){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
	
				printf("\"%s\"",loadb->pinfo[index].name);
			}
		}

	if (NULL != vlan){
			for (index=0; index < vlan->num; index++){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
	
				printf("\"%s\"",vlan->pinfo[index].name);
			}
		}

	printf("]");
}

void print_loadb_iflist(const struct mgtcgi_xml_interface_wans const *wan,
									const struct mgtcgi_xml_adsl_clients const *adsl,
									const struct mgtcgi_xml_l2tpvpn_clients const *l2tpvpn,
									const struct mgtcgi_xml_load_balancings const *loadb,
									const struct mgtcgi_xml_interface_vlans *vlan,
									const char *ifname)
{
	int index=0,i=0;
	int ifname_flag = 0;
	static int flag=0;
    int wan3_flag = 0,wan4_flag = 0;
	if (loadb == NULL){
		printf("\"if_list\":[]");
		return;
	}

	printf("\"if_list\":[");

	if (ifname != NULL){
		printf("\"%s\"", ifname);
		flag = 1;
	}
    for (i=0; i < loadb->num; i++){
        if (strcmp(loadb->pinfo[i].ifname, "WAN3") == 0){
            wan3_flag = 1;
        }
        else if(strcmp(loadb->pinfo[i].ifname, "WAN4") == 0){
            wan4_flag = 1;
        }
    }
	
	if (NULL != wan){
		for (index=0; index < wan->num; index++){	
            if(strcmp(wan->pinfo[index].ifname,"WAN3") == 0){
                continue;
            }
            if(strcmp(wan->pinfo[index].ifname,"WAN4") == 0){
                continue;
            }

			ifname_flag = 0;
			for (i=0; i < loadb->num; i++){
				if (strcmp(wan->pinfo[index].ifname, loadb->pinfo[i].ifname) == 0){
					ifname_flag = 1;
					break;
				}
			}

			if (ifname_flag == 0){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
				
				printf("\"%s\"",wan->pinfo[index].ifname);
			}
		}

#if 1
        if(wan3_flag == 0){
            if (flag == 1)
    			printf(",");
    		else
    			flag = 1;
            printf("\"WAN3\"");
        }
        if(wan4_flag == 0){
            if (flag == 1)
    			printf(",");
    		else
    			flag = 1;
            printf("\"WAN4\"");
        }
#endif
	}

	if (NULL != adsl){
		for (index=0; index < adsl->num; index++){
			ifname_flag = 0;
			for (i=0; i < loadb->num; i++){
				if (strcmp(adsl->pinfo[index].name, loadb->pinfo[i].ifname) == 0){
					ifname_flag = 1;
					break;
				}
			}

			if (ifname_flag == 0){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
				
				printf("\"%s\"",adsl->pinfo[index].name);
			}
		}
	}

	if (NULL != l2tpvpn){
		for (index=0; index < l2tpvpn->num; index++){
			ifname_flag = 0;
			for (i=0; i < loadb->num; i++){
				if (strcmp(l2tpvpn->pinfo[index].name, loadb->pinfo[i].ifname) == 0){
					ifname_flag = 1;
					break;
				}
			}

			if (ifname_flag == 0){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
				
				printf("\"%s\"",l2tpvpn->pinfo[index].name);
			}
		}
	}

	if (NULL != vlan){
		for (index=0; index < vlan->num; index++){
			ifname_flag = 0;
			for (i=0; i < loadb->num; i++){
				if (strcmp(vlan->pinfo[index].name, loadb->pinfo[i].ifname) == 0){
					ifname_flag = 1;
					break;
				}
			}

			if (ifname_flag == 0){
				if (flag == 1)
					printf(",");
				else
					flag = 1;
				
				printf("\"%s\"",vlan->pinfo[index].name);
			}
		}
	}
	
	printf("]");
}





void print_table_list(struct mgtcgi_xml_group_array *table)
{
	int index=0,flag=0;
	printf("\"table_list\":[");
	if (table != NULL){
		for (index=0; index < table->num; index++){
			
			if (flag == 1)
				printf(",");
			else
				flag = 1;
			
			printf("\"%s\"",table->pinfo[index].group_name);
		}
	}
	printf("]");
}

void print_rule_list(struct mgtcgi_xml_route_rules *rule)
{
	int index=0,flag=0;
	printf("\"rule_list\":[");
	if (rule != NULL){
		for (index=0; index < rule->num; index++){
			if (flag == 1)
				printf(",");
			else
				flag = 1;

			printf("\"%s\"",rule->pinfo[index].name);
		}
	}
	printf("]");
}

void print_routemark_list(struct mgtcgi_xml_group_array *table,
									struct mgtcgi_xml_route_rules *rule)
{
	int index=0,flag=0;
	printf("\"routemark_list\":[");
	for (index=0; index < table->num; index++){
		
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("\"%s\"",table->pinfo[index].group_name);
	}

	for (index=0; index < rule->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;

		printf("\"%s\"",rule->pinfo[index].name);
	}
	printf("]");
}

int dhcpd_ip_pool_legal_check(const char *ifname, const char *range, const char *mask, const char *gateway)
{
    int iserror=-1,size,index=0,retval;
    int i,j,flag;
    char range_head[STRING_LENGTH]={0};
	char range_tail[STRING_LENGTH]={0};
    struct in_addr range_in_ip1, range_in_ip2, range_in_gateway;
    struct mgtcgi_xml_interface_lans *lan = NULL;

    if(!ifname || !range || !mask || !gateway){
        goto EXIT;
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		goto FREE_EXIT;
	}

    retval = -1;
    for (index=0; index < lan->num; index++){
		if (strcmp(lan->pinfo[index].ifname, ifname) == 0){
			retval = 0;
			break;
		}
	}

    if (retval != 0){
		goto FREE_EXIT;
	}

    for (i=0,j=0,flag=0; i < strlen(range); i++){
		if (range[i] == '-'){
			flag = 1;
			continue;
		}
		if (flag == 0)
			range_head[i] = range[i];
		else{
			range_tail[j] = range[i];
			j++;
		}
	}

	if (inet_aton(range_head, &range_in_ip1) == 0){
		goto FREE_EXIT;
	}
	if (inet_aton(range_tail, &range_in_ip2) == 0){
		goto FREE_EXIT;
	}
	if (inet_aton(gateway, &range_in_gateway) == 0){
		goto FREE_EXIT;
	}

	if (ntohl(range_in_ip1.s_addr) > ntohl(range_in_ip2.s_addr)){
		goto FREE_EXIT;
	}

    iserror = 0;
FREE_EXIT:
    free_xml_node((void*)&lan);
    goto EXIT;
EXIT:
    return iserror;
}

int dhcpd_ip_pool_check(const char *ifname, const char *range, const char *mask, const char *gateway)
{
	int index,size,retval=0,i,j,flag;
	char if_ip[STRING_LENGTH]={0};
	char if_mask[STRING_LENGTH]={0};
	char range_head[STRING_LENGTH]={0};
	char range_tail[STRING_LENGTH]={0};
    char ip_alias[LONG_STRING_LENGTH] = {0};
	unsigned long int if_subnet, range_subnet1,range_subnet2,range_subnet3;
	struct in_addr if_in_ip, if_in_mask, range_in_ip1, range_in_ip2, 
					range_in_mask, range_in_gateway;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		retval = -1;
		goto FREE_EXIT;
	}

	retval = -1;
	for (index=0; index < lan->num; index++){
		if (strcmp(lan->pinfo[index].ifname, ifname) == 0){
			strncpy(if_ip, lan->pinfo[index].ip, STRING_LENGTH);
			strncpy(if_mask, lan->pinfo[index].mask, STRING_LENGTH);
            strncpy(ip_alias, lan->pinfo[index].aliases, LONG_STRING_LENGTH);
			retval = 0;
			break;
		}
	}
	if (retval != 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(if_ip, &if_in_ip) == 0){//错误返回0
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(if_mask, &if_in_mask) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if_subnet = (ntohl(if_in_ip.s_addr) & ntohl(if_in_mask.s_addr));
	//printf("if_subnet=%ul\n", if_subnet);

	// 检查掩码是否相同
	if (inet_aton(mask, &range_in_mask) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (ntohl(if_in_mask.s_addr) != ntohl(range_in_mask.s_addr)){
		retval = -1;
		goto FREE_EXIT;
	}

	//dhcp 地址池检查
	for (i=0,j=0,flag=0; i < strlen(range); i++){
		if (range[i] == '-'){
			flag = 1;
			continue;
		}
		if (flag == 0)
			range_head[i] = range[i];
		else{
			range_tail[j] = range[i];
			j++;
		}
	}

	if (inet_aton(range_head, &range_in_ip1) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(range_tail, &range_in_ip2) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(gateway, &range_in_gateway) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (ntohl(range_in_ip1.s_addr) > ntohl(range_in_ip2.s_addr)){
		retval = -1;
		goto FREE_EXIT;
	}
	
	range_subnet1 = (ntohl(range_in_ip1.s_addr) & ntohl(range_in_mask.s_addr));
	//printf("ip1 range_subnet=%ul\n", range_subnet);
	if (range_subnet1 != if_subnet){
		retval = -1;
		goto FREE_EXIT;
	}
	range_subnet2 = (ntohl(range_in_ip2.s_addr) & ntohl(range_in_mask.s_addr));
	//printf("ip2 range_subnet=%ul\n", range_subnet);
	if (range_subnet2 != if_subnet){
		retval = -1;
		goto FREE_EXIT;
	}

	range_subnet3 = (ntohl(range_in_gateway.s_addr) & ntohl(range_in_mask.s_addr));
	//printf("gateway range_subnet=%ul\n", range_subnet);
	if (range_subnet3 != if_subnet){
		retval = -1;
		goto FREE_EXIT;
	}
	retval = 0;
FREE_EXIT:
    free_xml_node((void*)&lan);
	return retval;
}

int dhcpd_host_ip_pool_check(const char *dhcpname, const char *hostip,
										const char *mask, const char *gateway)
{
	int index,size,retval=0;
	char ifname[STRING_LENGTH]={0};
	char if_ip[STRING_LENGTH]={0};
	char if_mask[STRING_LENGTH]={0};
	unsigned long int if_subnet, range_subnet;
	struct in_addr if_in_ip, if_in_mask, range_in_hostip, range_in_mask;
	
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;

	if (strlen(hostip) < 7){
		retval = -1;
		goto FREE_EXIT;
	}

	/*printf("dhcpname = %s\n", dhcpname);
	printf("hostip = %s\n", hostip);
	printf("mask = %s\n", mask);
	printf("gateway = %s\n", gateway);*/
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		retval = -1;
		goto FREE_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		retval = -1;
		goto FREE_EXIT;
	}
	retval = -1;
	for (index=0; index < dhcpd->num; index++){
		if (strcmp(dhcpd->pinfo[index].name, dhcpname) == 0){
			strncpy(ifname, dhcpd->pinfo[index].ifname, STRING_LENGTH);
			retval = 0;
			break;
		}
	}
	
	if (retval != 0){
		retval = -1;
		goto FREE_EXIT;
	}
	retval = -1;
	for (index=0; index < lan->num; index++){
		if (strcmp(lan->pinfo[index].ifname, ifname) == 0){
			strncpy(if_ip, lan->pinfo[index].ip, STRING_LENGTH);
			strncpy(if_mask, lan->pinfo[index].mask, STRING_LENGTH);
			retval = 0;
			break;
		}
	}
	if (retval != 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(if_ip, &if_in_ip) == 0){//错误返回0
		retval = -1;
		goto FREE_EXIT;
	}
	if (inet_aton(if_mask, &if_in_mask) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	if_subnet = (ntohl(if_in_ip.s_addr) & ntohl(if_in_mask.s_addr));
		
	//检查子网掩码是否相同
	if (strlen(mask) < 7){
		if (inet_aton(if_mask, &range_in_mask) == 0){
			retval = -1;
			goto FREE_EXIT;
		}
	}
	else{
		if (inet_aton(mask, &range_in_mask) == 0){
			retval = -1;
			goto FREE_EXIT;
		}
		if (ntohl(if_in_mask.s_addr) != ntohl(range_in_mask.s_addr)){
			retval = -1;
			goto FREE_EXIT;
		}
	}
	
	if (inet_aton(hostip, &range_in_hostip) == 0){
		retval = -1;
		goto FREE_EXIT;
	}
	range_subnet = (ntohl(range_in_hostip.s_addr) & ntohl(range_in_mask.s_addr));
	if (range_subnet != if_subnet){
		retval = -1;
		goto FREE_EXIT;
	}

	if (strlen(gateway) >= 7){
		if (inet_aton(gateway, &range_in_hostip) == 0){
			retval = -1;
			goto FREE_EXIT;
		}
		range_subnet = (ntohl(range_in_hostip.s_addr) & ntohl(range_in_mask.s_addr));
		if (range_subnet != if_subnet){
			retval = -1;
			goto FREE_EXIT;
		}
	}

	retval = 0;
FREE_EXIT:
	free_xml_node((void*)&lan);
	free_xml_node((void*)&dhcpd);
    if(lan){
	    free_xml_node((void*)&lan);
        lan = NULL;
    }
    if(dhcpd){
	    free_xml_node((void *)&dhcpd);
        dhcpd = NULL;
    }
	return retval;
}

int pptpd_server_ip_pool_check(const char *startip, const char *endip)
{
    int retal = -1;
    struct in_addr ip1,ip2,if_mask;
    unsigned long int if_subnet1,if_subnet2;
    char mask[STRING_LENGTH] = "255.255.255.0";

    if(!startip || !endip)
        return retal;

    if((inet_aton(startip, &ip1) == 0)
        || (inet_aton(endip, &ip2) == 0)
        || (inet_aton(mask, &if_mask) == 0)){
        goto ERROR;
    }

    if(ntohl(ip1.s_addr) > ntohl(ip2.s_addr)){
        goto ERROR;
    }
    
    if_subnet1 = (ntohl(ip1.s_addr) & ntohl(if_mask.s_addr));
    if_subnet2 = (ntohl(ip2.s_addr) & ntohl(if_mask.s_addr));
    if(if_subnet1 != if_subnet2){
        goto ERROR;
    }

    retal = 0;
ERROR:
    return retal;
}


/****************************** config list show ***************************/
void route_dns_list_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_dns *dns = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void**)&dns, &size);
	if (NULL == dns){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf( "{\"data\":[{\"dns_primary\":\"%s\",\"dns_secondary\":\"%s\",\"dns_thirdary\":\"%s\"}],",\
			dns->dns_primary, dns->dns_secondary, dns->dns_thirdary);
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dns);
	return;
}

void route_ddns_list_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_ddns *ddns = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DDNS, (void**)&ddns, &size);

	if (NULL == ddns){
		iserror = MGTCGI_READ_FILE_ERR;
		
		goto ERROR_EXIT;
	}

	

	printf( "{\"data\":[{\"enable_ddns\":\"%u\",\"provider\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"dns\":\"%s\"}],",\
			ddns->enable_ddns, ddns->provider,ddns->username, ddns->password,ddns->dns);
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&ddns);
	return;
}

void route_iflan_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	char link_status[STRING_LENGTH]={0};
	struct mgtcgi_xml_route_interface_lans *lan = NULL;
    
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
        
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    printf("{\"data\":{");
	get_link_status(lan->ifname, link_status);
	printf("\"status\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\",\"mac\":\"%s\",\"ifname\":\"%s\",\"dhcp_type\":\"%s\","  \
           "\"startip\":\"%s\",\"endip\":\"%s\",\"lease_time\":\"%u\",\"gateway\":\"%s\",\"dhcp_mask\":\"%s\",\"first_dns\":\"%s\",\"second_dns\":\"%s\",\"mutilan_enable\":\"%u\",",
            link_status,lan->ip,lan->mask,lan->mac,lan->ifname,lan->dhcp_type,
			lan->dhcp_info.startip,lan->dhcp_info.endip,lan->dhcp_info.lease_time,lan->dhcp_info.gateway,
			lan->dhcp_info.dhcp_mask,lan->dhcp_info.first_dns,lan->dhcp_info.second_dns,lan->sunbet_enable);

    printf("\"mutilan\":[");
    for(index=0;index<lan->subnet_num;index++){
        if(flag == 1)
            printf(",");
        else
            flag = 1;
        printf("{\"ip\":\"%s\",\"mask\":\"%s\"}",lan->pinfo[index].ip,lan->pinfo[index].mask);
        total++;
    }
	printf("]},");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(lan){
	    free_xml_node((void *)&lan);
        lan = NULL;
    }
	return;
}
void route_ifwan_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	char macaddr[LITTER_LENGTH]={0};
	char ip[LITTER_LENGTH]={0};
	char gateway[LITTER_LENGTH]={0};
	char link_status[LITTER_LENGTH]={0};
    char uptime[LITTER_LENGTH];
    char mask[LITTER_LENGTH];
	struct mgtcgi_xml_interface_wans *wan = NULL;  

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < wan->num; index++){
		memset(link_status, 0, sizeof(link_status));
		memset(macaddr, 0, sizeof(macaddr));
		get_link_macaddr(wan->pinfo[index].ifname, macaddr);
		
		memset(ip, 0, sizeof(ip));
		memset(gateway, 0, sizeof(gateway));
        memset(uptime, 0, sizeof(uptime));

		if (strcmp(wan->pinfo[index].access, "static") == 0){
			strcpy(ip, wan->pinfo[index].ip);
			strcpy(gateway, wan->pinfo[index].gateway);
            strcpy(mask, wan->pinfo[index].mask);
            get_link_status(wan->pinfo[index].ifname, link_status);
		}
		else if (strcmp(wan->pinfo[index].access, "dhcp") == 0){
			get_dhcp_addr(wan->pinfo[index].ifname, ip, mask, gateway);
            get_link_status(wan->pinfo[index].ifname, link_status);
		}
        else if(strcmp(wan->pinfo[index].access, "pppoe") == 0){
            get_adslclient_link(wan->pinfo[index].ifname, link_status, ip,gateway, uptime);
        }
        else if(strcmp(wan->pinfo[index].access, "l2tpclient") == 0){
            get_l2tpvpnclient_link(wan->pinfo[index].ifname, link_status, ip,gateway, uptime);
        }


        if (flag == 1)
		    printf(",");
	    else
		    flag = 1;
        
	    printf("{\"id\":\"%u\",\"enable\":\"%u\",\"access\":\"%s\",\"status\":\"%s\",\"ifname\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\","  \
               "\"gateway\":\"%s\",\"username\":\"%s\",\"passwd\":\"%s\",\"sname\":\"%s\",\"type\":\"%s\","  \
               "\"mtu\":\"%s\",\"mac\":\"%s\",\"dns\":\"%s\",\"up\":\"%s\",\"down\":\"%s\",\"isp\":\"%s\","  \
               "\"time\":\"%s\",\"week\":\"%s\",\"day\":\"%s\",\"uptime\":\"%s\",\"defaultroute\":\"%u\"}",
			wan->pinfo[index].id, wan->pinfo[index].enable,wan->pinfo[index].access,link_status, wan->pinfo[index].ifname,
			ip,mask,gateway,wan->pinfo[index].username,wan->pinfo[index].passwd,wan->pinfo[index].servername,
			wan->pinfo[index].type,wan->pinfo[index].mtu,wan->pinfo[index].mac,wan->pinfo[index].dns,
			wan->pinfo[index].up,wan->pinfo[index].down,wan->pinfo[index].isp,wan->pinfo[index].time,
			wan->pinfo[index].week,wan->pinfo[index].day,uptime,wan->pinfo[index].defaultroute);

		total++;
	}

	printf("],");
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wan){
	    free_xml_node((void*)&wan);
        wan = NULL;
    }
	return;
}

void wan_channel_list_show()
{
    int iserror=0,total=0,size=0,index=0,flag=0;
    struct mgtcgi_xml_wans_channels *wanchannel = NULL;
    
    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&wanchannel, &size);
    if (NULL == wanchannel){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    total = 0;
    flag = 0;
    printf("{\"data\":[");
    for (index=0; index < wanchannel->num; index++){

        if (flag == 1)
            printf(",");
        else
            flag = 1;

        printf("{\"id\":\"%u\",\"enable\":\"%u\",\"classname\":\"%s\",\"rate\":\"%s\",\"classtype\":\"%u\",\"classid\":\"%u\",\"parentid\":\"%u\",\"shareid\":\"%u\",\"priority\":\"%u\",\"limit\":\"%u\",\"attribute\":\"%u\",\"weight\":\"%u\",\"dir\":\"%u\",\"comment\":\"%s\"}",
                wanchannel->pinfo[index].id,wanchannel->pinfo[index].enable,wanchannel->pinfo[index].classname,
                wanchannel->pinfo[index].rate,wanchannel->pinfo[index].classtype,wanchannel->pinfo[index].classid,
                wanchannel->pinfo[index].parentid,wanchannel->pinfo[index].shareid,wanchannel->pinfo[index].priority,
                wanchannel->pinfo[index].limit,wanchannel->pinfo[index].attribute,wanchannel->pinfo[index].weight,
                wanchannel->pinfo[index].direct,wanchannel->pinfo[index].comment);
        total++;
    }
    printf("],");
    
    printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(wanchannel){
        free_xml_node((void*)&wanchannel);
    }
    return;

}

void route_vlan_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
    char macaddr[LITTER_LENGTH] = {0};
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < vlan->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;

        get_link_macaddr(vlan->pinfo[index].name, macaddr);
		printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"id\":\"%d\",\"mac\":\"%s\",\"comment\":\"%s\"}",
				vlan->pinfo[index].name, vlan->pinfo[index].ifname,
				vlan->pinfo[index].id, macaddr, vlan->pinfo[index].comment);
        memset(macaddr,0,sizeof(macaddr));
        total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(vlan){
	    free_xml_node((void*)&vlan);
    }
	return;
}

void route_macvlan_list_show(void)
{
    int iserror=0,total=0,size=0,index=0,flag=0;
    char macaddr[LITTER_LENGTH] = {0};
    struct mgtcgi_xml_interface_macvlans *macvlan = NULL;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void**)&macvlan, &size);
    if (NULL == macvlan){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    total = 0;
    flag = 0;
    printf("{\"data\":[");
    for (index=0; index < macvlan->num; index++){

        if (flag == 1)
            printf(",");
        else
            flag = 1;

        get_link_macaddr(macvlan->pinfo[index].name, macaddr);
        printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"mac\":\"%s\",\"comment\":\"%s\"}",
                macvlan->pinfo[index].name, macvlan->pinfo[index].ifname,
                macaddr, macvlan->pinfo[index].comment);
        memset(macaddr,0,sizeof(macaddr));
        total++;
    }
    printf("],");
    
    printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(macvlan){
        free_xml_node((void*)&macvlan);
    }
    return;
}

void route_tables_list_show()
{
    int iserror=0,total=0,size=0,index=0,flag=0;
    struct mgtcgi_xml_route_tables *tables = NULL;
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTING_TABLES, (void**)&tables, &size);
    if (NULL == tables){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    total = 0;
    flag = 0;
    printf("{\"data\":[");
    for (index=0; index < tables->num; index++){

        if (flag == 1)
            printf(",");
        else
            flag = 1;

        printf("{\"id\":\"%u\",\"enable\":\"%u\",\"value\":\"%s\"}",
                tables->pinfo[index].id, tables->pinfo[index].enable,tables->pinfo[index].value);
        total++;
    }
    printf("],");
    
    printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(tables){
        free_xml_node((void*)&tables);
    }
    return;
}

void route_interfacelink_list_show(void)
{
    int iserror=0,total=0,size=0,index=0,flag=0;
    int link_detected=0,link_speed=0;
    struct mgtcgi_xml_interface_links *links = NULL;
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LINKS, (void**)&links, &size);
    if (NULL == links){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    total = 0;
    flag = 0;
    printf("{\"data\":[");
    for (index=0; index < links->num; index++){

        if (flag == 1)
            printf(",");
        else
            flag = 1;

        link_detected = get_link_detected(links->pinfo[index].ifname);
        link_speed = get_link_speed(links->pinfo[index].ifname);
        printf("{\"ifname\":\"%s\",\"linkstatus\":%d,\"type\":\"%s\",\"mode\":\"%s\",\"duplex\":\"%s\",\"speed\":%d,\"linkmode\":\"%u\"}",
                links->pinfo[index].ifname,link_detected,links->pinfo[index].type,links->pinfo[index].mode,
                links->pinfo[index].duplex,link_speed,links->pinfo[index].linkmode);
                        
        total++;
    }
    printf("],");
    
    printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(links){
        free_xml_node((void*)&links);
    }
    return;
}

void route_pppdauth_list_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_pppd_authenticate *pppdauth = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPPD_AUTHENTICATE, (void**)&pppdauth, &size);
	if (NULL == pppdauth){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf( "{\"data\":[{\"pap\":%d,\"chap\":%d,\"mschap\":%d,\"mschapv2\":%d,\"eap\":%d}],",\
			pppdauth->pap, pppdauth->chap, pppdauth->mschap, pppdauth->mschapv2, pppdauth->eap);
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:

    if(pppdauth){
	    free_xml_node((void*)&pppdauth);
        pppdauth = NULL;
    }
	return;
}

void route_adslclient_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0,adsl_status=-1;
	char ip[STRING_LENGTH]={0};
	char gateway[STRING_LENGTH]={0};
	char uptime[STRING_LENGTH]={0};
	char status[STRING_LENGTH]={0};
	struct mgtcgi_xml_adsl_clients *adsl = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < adsl->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		memset(status, 0, sizeof(status));
		memset(ip, 0, sizeof(ip));
		memset(uptime, 0, sizeof(uptime));
		memset(gateway, 0, sizeof(gateway));
		get_adslclient_link(adsl->pinfo[index].name, status, ip, gateway, uptime);
		get_adslclient_status(adsl->pinfo[index].name,&adsl_status);
		printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"username\":\"%s\",\"status\":\"%s\",\"ip\":\"%s\",\"gateway\":\"%s\",\"uptime\":\"%s\",\"weight\":%d,\"enable\":%d,\"defaultroute\":%d,\"ppp_status\":%d}",
				adsl->pinfo[index].name, adsl->pinfo[index].ifname, adsl->pinfo[index].username, status, ip, gateway, uptime,
				adsl->pinfo[index].weight, adsl->pinfo[index].enable,adsl->pinfo[index].defaultroute,adsl_status);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(adsl){
	    free_xml_node((void *)&adsl);
        adsl = NULL;
    }
	return;
}
void route_vpnclient_list_show()	
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	char ip[STRING_LENGTH]={0};
	char uptime[STRING_LENGTH]={0};
	char gateway[STRING_LENGTH]={0};
	char status[STRING_LENGTH]={0};
	
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < l2tpvpn->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		memset(status, 0, sizeof(status));
		memset(ip, 0, sizeof(ip));
		memset(uptime, 0, sizeof(uptime));
		memset(gateway, 0, sizeof(gateway));
		get_l2tpvpnclient_link(l2tpvpn->pinfo[index].name, status, ip, gateway, uptime);
		
		printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"username\":\"%s\",\"status\":\"%s\",\"ip\":\"%s\",\"gateway\":\"%s\",\"uptime\":\"%s\",\"weight\":%d,\"enable\":%d,\"defaultroute\":%d}",
				l2tpvpn->pinfo[index].name, l2tpvpn->pinfo[index].ifname, l2tpvpn->pinfo[index].username, status, ip, gateway, uptime, 
				l2tpvpn->pinfo[index].weight, l2tpvpn->pinfo[index].enable,l2tpvpn->pinfo[index].defaultroute);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:

    if(l2tpvpn){
	    free_xml_node((void*)&l2tpvpn);
        l2tpvpn = NULL;
    }
	return;
}

void route_loadb_list_show()	
{
#if 0
	int iserror=0,total=0,size=0,index=0,flag=0,
		iftype=0,i=0,total_weight=0;
	char ip[STRING_LENGTH]={0};
	char mask[STRING_LENGTH]={0};
	char uptime[STRING_LENGTH]={0};
	char gateway[STRING_LENGTH]={0};
	char status[STRING_LENGTH]={0};
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total_weight = 0;
	for (index=0; index < loadb->num; index++){
		total_weight += loadb->pinfo[index].weight;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < loadb->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;
        
		memset(status, 0, sizeof(status));
		memset(ip, 0, sizeof(ip));
		memset(mask, 0, sizeof(mask));
		memset(gateway, 0, sizeof(gateway));
		iftype = check_interface_type(loadb->pinfo[index].ifname);
		if (iftype == 2){ //WAN
			for (i=0; i<wan->num; i++){
				if (strcmp(wan->pinfo[i].ifname, loadb->pinfo[index].ifname) == 0){
					get_link_status(wan->pinfo[i].ifname, status);
					if (strcmp(wan->pinfo[i].access, "static") == 0){
						strcpy(ip, wan->pinfo[i].ip);
						strcpy(mask, wan->pinfo[i].mask);
						strcpy(gateway, wan->pinfo[i].gateway);
					}
					else if (strcmp(wan->pinfo[i].access, "dhcp") == 0){
						get_dhcp_addr(wan->pinfo[i].ifname, ip, mask, gateway);
					}
					break; 
				}
			}
		}
		else if (iftype == 3){ //adsl
			get_adslclient_link(loadb->pinfo[index].ifname, status, ip, gateway, uptime);
		}
		else if (iftype == 4){ //l2tpvpn
			get_l2tpvpnclient_link(loadb->pinfo[index].ifname, status, ip, gateway, uptime);
		}
		else if(iftype == 6){
			for (i=0; i<vlan->num; i++){
				if (strcmp(vlan->pinfo[i].name, loadb->pinfo[index].ifname) == 0){
					get_vlan_status(vlan->pinfo[i].name, status);
					strcpy(ip, vlan->pinfo[i].ip);
					strcpy(mask, vlan->pinfo[i].mask);
					strcpy(gateway,vlan->pinfo[i].gateway);
					break; 
				}
			}
		}
		
		printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"status\":\"%s\",\"ip\":\"%s\",\"gateway\":\"%s\",\"rule\":\"%s\",\"total_weight\":%d,\"weight\":%d}",
				loadb->pinfo[index].name, loadb->pinfo[index].ifname, status, ip, gateway, loadb->pinfo[index].rulename, total_weight, loadb->pinfo[index].weight);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:


    if(loadb){
	    free_xml_node((void*)&loadb);
        loadb = NULL;
    }
    if(wan){
	    free_xml_node((void *)&wan);
        wan = NULL;
    }
    if(vlan){
	    free_xml_node((void*)&vlan);
        vlan = NULL;
    }
#endif
	return;
}


void route_routing_list_show()
{
	int total=0;
	printf("{\"data\":[");
	print_routing_valid();
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	return;
}
void route_rtable_list_show()
{
	int iserror=0,size=0,index=0,total=0,flag=0;
	struct mgtcgi_xml_group_array *table = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if (NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < table->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;
			
		printf( "{\"name\":\"%s\",\"number\":%d,\"comment\":\"%s\"}",
			table->pinfo[index].group_name, table->pinfo[index].num,table->pinfo[index].comment);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}",total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(table){
	    free_xml_node((void *)&table);
        table = NULL;
    }
	return;
}
void route_rrule_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;

	struct mgtcgi_xml_route_rules *rule = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < rule->num; index++){

		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("{\"name\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\"protocol\":\"%s\",\"src_port\":\"%s\",\"dst_port\":\"%s\",\"tos\":\"%s\",\"action\":\"%s\",\"comment\":\"%s\"}",
				rule->pinfo[index].name, rule->pinfo[index].src, rule->pinfo[index].dst, rule->pinfo[index].protocol,rule->pinfo[index].src_port,
				rule->pinfo[index].dst_port, rule->pinfo[index].tos, strlen(rule->pinfo[index].action)?(rule->pinfo[index].action):"ACCEPT",rule->pinfo[index].comment);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(rule){
	    free_xml_node((void *)&rule);
        rule = NULL;
    }
	return;
}
void route_dnat_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	
	struct mgtcgi_xml_nat_dnats *dnat = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&dnat, &size);
	if (NULL == dnat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < dnat->num; index++){
	
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("{\"id\":\"%u\",\"protocol\":\"%s\",\"enable\":\"%d\",\"comment\":\"%s\",\"wan_ip\":\"%s\",\"wan_port_before\":\"%s\",\"wan_port_after\":\"%s\",\"lan_port_before\":\"%s\",\"lan_port_after\":\"%s\",\"lan_ip\":\"%s\",\"ifname\":\"%s\",\"loop_enable\":\"%u\"}",
                dnat->pinfo[index].id,
                dnat->pinfo[index].protocol, 
				dnat->pinfo[index].enable, 
				dnat->pinfo[index].comment, 
				dnat->pinfo[index].wan_ip, 
				dnat->pinfo[index].wan_port_before,
				dnat->pinfo[index].wan_port_after, 
				dnat->pinfo[index].lan_port_before, 
				dnat->pinfo[index].lan_port_after,
				dnat->pinfo[index].lan_ip,
				dnat->pinfo[index].ifname,
				dnat->pinfo[index].loop_enable);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(dnat){
	    free_xml_node((void *)&dnat);
        dnat = NULL;
    }
	return;
}

void route_snat_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	
	struct mgtcgi_xml_nat_snats *snat = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&snat, &size);
	if (NULL == snat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < snat->num; index++){
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("{\"id\":%d,\"enable\":%d,\"ifname\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\"protocol\":\"%s\",\"action\":\"%s\",\"dstip\":\"%s\",\"dstport\":\"%s\",\"comment\":\"%s\"}",
				snat->pinfo[index].id, snat->pinfo[index].enable, snat->pinfo[index].ifname, 
				snat->pinfo[index].src, snat->pinfo[index].dst, snat->pinfo[index].protocol,
				snat->pinfo[index].action, snat->pinfo[index].dstip,snat->pinfo[index].dstport,
				snat->pinfo[index].comment);
		total++;
	}
	printf("],");
	
	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:

    if(snat){
	    free_xml_node((void *)&snat);
        snat = NULL;
    }
	return;
}


void route_macbind_list_show()
{
	int iserror=0,buff_size=0;
	char cmd[CMD_LITTER_LENGTH]={"0"};
   char page_str[DIGITAL_LENGTH]={"0"};
   char pagesize_str[DIGITAL_LENGTH]={"0"};
	char *buffer = NULL;
	FILE *pp;
    
#if CGI_FORM_STRING
    cgiFormString("page", page_str, sizeof(page_str));
	cgiFormString("pagesize",pagesize_str,sizeof(pagesize_str));
#endif

	buff_size = 5 * STRING_LIST_LENGTH * sizeof(char);
	buffer = (char *)malloc(buff_size);//500k,大约5000条
	if (NULL == buffer){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	memset(buffer, 0, buff_size);
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh list macbind %s %s",page_str,pagesize_str);
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buffer, buff_size, pp);
              if(buffer[strlen(buffer) - 1] == '\n'){
		    buffer[strlen(buffer) - 1]='\0';
              }
	}
	pclose(pp);
    
	printf("%s",buffer);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(buffer!=NULL){
        free(buffer);
        buffer = NULL;
    }
	return;
}

void route_dhcpd_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	int dhcpd_status=0;
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = 0;
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < dhcpd->num; index++){
	
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"range\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"dnsname\":\"%s\",\"dns\":\"%s\",\"lease_time\":\"%d\",\"max_lease_time\":\"%d\",\"enable\":%d}",
				dhcpd->pinfo[index].name, dhcpd->pinfo[index].ifname, dhcpd->pinfo[index].range, 
				dhcpd->pinfo[index].mask, dhcpd->pinfo[index].gateway, dhcpd->pinfo[index].dnsname,
				dhcpd->pinfo[index].dns, dhcpd->pinfo[index].lease_time,dhcpd->pinfo[index].max_lease_time,
				dhcpd->pinfo[index].enable);
		total++;
	}
	printf("],");

	//log_debug("111\n");
	//log_debug("dhcpd_status1 = %d\n", dhcpd_status);
	dhcpd_status = get_dhcpd_status();
	//log_debug("dhcpd_status2 = %d\n", dhcpd_status);

	printf("\"iserror\":0,\"total\":%d,\"dhcpd_enable\":%d,\"dhcpd_status\":%d,\"msg\":\"\"}", total, dhcpd->dhcpd_enable, dhcpd_status);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:

    if(dhcpd){
	    free_xml_node((void *)&dhcpd);
        dhcpd = NULL;
    }
	return;
}

void route_pptpd_server_list_show()
{
    int iserror=0,size=0;
    struct mgtcgi_xml_pptpd_server *pptpd = NULL;
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_SERVERS, (void**)&pptpd, &size);
    if (NULL == pptpd){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"enable\":%d,\"startip\":\"%s\",\"endip\":\"%s\",\"localip\":\"%s\",\"dns\":\"%s\",\"entry\":\"%s\",",
        pptpd->enable,
        pptpd->startip,
        pptpd->endip,
        pptpd->localip,
        pptpd->dns,
        pptpd->entry_mode);

    printf("\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(pptpd){
        free_xml_node((void *)&pptpd);
        pptpd = NULL;
    }
    return;

}

void route_pptpd_account_list_show()
{
    int iserror=0,size=0,total=0,index=0,flag=0;
    struct mgtcgi_xml_pptpd_accounts_info *account = NULL;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_ACCOUNT, (void**)&account, &size);
    if (NULL == account){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"data\":[");
    for(index=0; index < account->num; index++){
        if(flag==1){
            printf(",");
        }
        else{
            flag=1;
        }

        printf("{\"username\":\"%s\",\"passwd\":\"%s\",\"ip\":\"%s\"}",
                account->pinfo[index].username,
                account->pinfo[index].passwd,
                account->pinfo[index].ip);
        total++;
    }
    printf("],\"total\":%d,\"iserror\":%d,\"msg\":\"\"}",total,iserror);
    goto FREE_EXIT;

ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(account){
        free_xml_node((void *)&account);
        account = NULL;
    }
    return;

}

void route_dhcpd_host_list_show()
{
	int iserror=0,total=0,size=0,index=0,flag=0;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
	if (NULL == host){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	total = 0;
	flag = 0;

	printf("{\"data\":[");
	for (index=0; index < host->num; index++){
	
		if (flag == 1)
			printf(",");
		else
			flag = 1;
		
		printf("{\"name\":\"%s\",\"dhcpname\":\"%s\",\"mac\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"dnsname\":\"%s\",\"dns\":\"%s\",\"lease_time\":\"%d\",\"max_lease_time\":\"%d\",\"enable\":%d}",
				host->pinfo[index].name, host->pinfo[index].dhcpname, host->pinfo[index].mac, 
				host->pinfo[index].ip, host->pinfo[index].mask, host->pinfo[index].gateway,
				host->pinfo[index].dnsname, host->pinfo[index].dns, host->pinfo[index].lease_time,
				host->pinfo[index].max_lease_time, host->pinfo[index].enable);
		total++;
	}
	printf("],");

	printf("\"iserror\":0,\"total\":%d,\"msg\":\"\"}", total);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(host){
	    free_xml_node((void *)&host);
        host = NULL;
    }
	return;
}

/******************************** config add show *****************************/
void route_dns_add_show()
{
	return; //无
}
void route_iflan_add_show()
{
	return;//
}
void route_ifwan_add_show()
{
	int iserror=0,size=0,flag=0,dot=0,index,i;
    char mac[LITTER_LENGTH] = {0};
	struct mgtcgi_xml_interface_links *wan_links = NULL;
    struct mgtcgi_xml_interface_vlans *vlan = NULL;
    struct mgtcgi_xml_interface_macvlans *macvlan = NULL;

    struct mgtcgi_xml_interface_wans *wans_info = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
	if (NULL == wans_info){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LINKS, (void**)&wan_links, &size);
	if (NULL == wan_links){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void**)&macvlan, &size);
	if (NULL == macvlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    printf("{\"if_list\":[");

    flag = 0;
    for(index = 0; index < wan_links->num; index++){
        if(strcmp(wan_links->pinfo[index].type,"wan") != 0)
            continue;
        for(i = 0; i < wans_info->num; i++){
            if(strcmp(wans_info->pinfo[i].ifname,wan_links->pinfo[index].ifname) == 0){
                flag = 1;
                break;
            }
        }

        if(flag == 0){
            if(dot == 1){
                printf(",");
            }
            else
                dot = 1;

            get_link_macaddr(wan_links->pinfo[index].ifname,mac);
            printf("[\"%s\",\"%s\"]",wan_links->pinfo[index].ifname,mac);
        }
        flag = 0;
    }

    flag = 0;
    for(index = 0; index < vlan->num; index++){
        for(i = 0; i < wans_info->num; i++){
            if(strcmp(wans_info->pinfo[i].ifname,vlan->pinfo[index].name) == 0){
                flag = 1;
                break;
            }
        }

        if(flag == 0){
            if(dot == 1){
                printf(",");
            }
            else
                dot = 1;
            get_link_macaddr(vlan->pinfo[index].name,mac);
            printf("[\"%s\",\"%s\"]",vlan->pinfo[index].name,mac);
        }
        flag = 0;
    }

    flag = 0;
    for(index = 0; index < macvlan->num; index++){
        for(i = 0; i < wans_info->num; i++){
            if(strcmp(wans_info->pinfo[i].ifname,macvlan->pinfo[index].name) == 0){
                flag = 1;
                break;
            }
        }

        if(flag == 0){
            if(dot == 1){
                printf(",");
            }
            else
                dot = 1;
            get_link_macaddr(macvlan->pinfo[index].name,mac);
            printf("[\"%s\",\"%s\"]",macvlan->pinfo[index].name,mac);
        }
        flag = 0;
    }
    printf("],\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&wan_links);
    free_xml_node((void*)&vlan);
    free_xml_node((void*)&macvlan);
    free_xml_node((void*)&wans_info);
	return;
}

void route_vlan_add_show()
{
	int iserror=0,size=0,flag=0,index;
    char macaddr[LITTER_LENGTH] = {0};
	struct mgtcgi_xml_interface_links *wan_links = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LINKS, (void**)&wan_links, &size);
	if (NULL == wan_links){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
#if 0
	printf("{\"data\":[");
	printf("{\"name\":\"vlan%d\",\"macvlan_name\":\"veth%d\",\"type\":\"vlan\",\"ifname\":\"WAN0\",\"id\":\"\",\"mac\":\"\",\"ip\":\"\",\"mask\":\"\",\"comment\":\"\"}",(vlan->num+2),(vlan->num+2));
	printf("],");
	print_interface_list(wan, lan, NULL, NULL, NULL,NULL);
	printf(",");
#endif	
    printf("{\"if_list\":[");
    for(index = 0; index < wan_links->num; index++){
        if(strcmp(wan_links->pinfo[index].type,"wan") != 0)
            continue;

        if(flag == 1){
            printf(",");
        }
        else
            flag = 1;

        get_link_macaddr(wan_links->pinfo[index].ifname,macaddr);
        printf("[\"%s\",\"%s\"]",wan_links->pinfo[index].ifname,macaddr);
        memset(macaddr,0,sizeof(macaddr));
    }
	printf("],\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wan_links){
	    free_xml_node((void*)&wan_links);
        wan_links = NULL;
    }
	return;
}

void route_macvlan_add_show()
{
	int iserror=0,size=0,flag=0,index;
	struct mgtcgi_xml_interface_links *wan_links = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LINKS, (void**)&wan_links, &size);
	if (NULL == wan_links){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
#if 0
	printf("{\"data\":[");
	printf("{\"name\":\"vlan%d\",\"macvlan_name\":\"veth%d\",\"type\":\"vlan\",\"ifname\":\"WAN0\",\"id\":\"\",\"mac\":\"\",\"ip\":\"\",\"mask\":\"\",\"comment\":\"\"}",(vlan->num+2),(vlan->num+2));
	printf("],");
	print_interface_list(wan, lan, NULL, NULL, NULL,NULL);
	printf(",");
#endif	
    printf("{\"if_list\":[");
    for(index = 0; index < wan_links->num; index++){
        if(strcmp(wan_links->pinfo[index].type,"wan") != 0)
            continue;

        if(flag == 1){
            printf(",");
        }
        else
            flag = 1;
        printf("\"%s\"",wan_links->pinfo[index].ifname);
    }
	printf("],\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wan_links){
	    free_xml_node((void*)&wan_links);
        wan_links = NULL;
    }
	return;
}



void route_pppdauth_add_show()
{
	return;//
}
void route_adslclient_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"data\":[");
	printf("{\"name\":\"adsl-%d\",\"ifname\":\"WAN0\",\"username\":\"\",\"password\":\"\",\"servername\":\"\",\"mtu\":1480,\"defaultroute\":0,\"peerdns\":0,\"enable\":1,\"comment\":\"\"}",(adsl->num+1));
	printf("],");
	print_ifname_list(wan, vlan);
	printf(",");

	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:



    if(vlan){
	    free_xml_node((void*)&vlan);
        vlan = NULL;
    }
    if(wan){
	    free_xml_node((void *)&wan);
        wan = NULL;
    }
    if(adsl){
	    free_xml_node((void *)&adsl);
        adsl = NULL;
    }
	return;
}
void route_vpnclient_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	printf("{\"name\":\"l2tpvpn-%d\",\"ifname\":\"WAN0\",\"username\":\"\",\"password\":\"\",\"servername\":\"\",\"mtu\":1480,\"defaultroute\":0,\"peerdns\":0,\"enable\":1,\"comment\":\"\"}",(l2tpvpn->num+1));
	printf("],");
	print_ifwan_list(wan);
	printf(",");

	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wan){
	    free_xml_node((void*)&wan);
        wan = NULL;
    }
    if(l2tpvpn){
	    free_xml_node((void *)&l2tpvpn);
        l2tpvpn = NULL;
    }
	return;
}


void route_loadb_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	struct mgtcgi_xml_route_rules * rule = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	printf("{\"name\":\"balance-%d\",\"ifname\":\"\",\"weight\":1,\"comment\":\"\"}", loadb->num+1);
	printf("],");
	print_loadb_iflist(wan, adsl, l2tpvpn, loadb, vlan, NULL);
	printf(",");
	print_rule_list(rule);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(rule){
	    free_xml_node((void*)&rule);
        rule = NULL;
    }
    if(loadb){
	    free_xml_node((void*)&loadb);
        loadb = NULL;
    }
    if(wan){
	    free_xml_node((void *)&wan);
        wan = NULL;
    }

    if(adsl){
	    free_xml_node((void*)&adsl);
        adsl = NULL;
    }
    if(l2tpvpn){
	    free_xml_node((void *)&l2tpvpn);
        l2tpvpn = NULL;
    }
    if(vlan){
	    free_xml_node((void*)&vlan);
        vlan = NULL;
    }
	return;
}


void route_routing_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_group_array * table = NULL;
	struct mgtcgi_xml_route_rules * rule = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if (NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"data\":[");
	printf("{\"enbale\":1,\"dst\":\"\",\"gateway\":\"0.0.0.0\",\"ifname\":\"\",\"priority\":1,\"table\":\"\",\"rule\":\"\",\"comment\":\"\"}");
	printf("],");
	print_interface_list(wan, lan, adsl, l2tpvpn, loadb,vlan);
	printf(",");
	print_table_list(table);
	printf(",");
	print_rule_list(rule);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&wan);
	free_xml_node((void*)&lan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&table);
	free_xml_node((void*)&rule);
	free_xml_node((void*)&loadb);
	free_xml_node((void *)&vlan);
	return;
}
void route_rtable_add_show()
{
	return;//
}
void route_rrule_add_show()
{
	return;//
}
void route_dnat_add_show()
{
	int iserror=0,size=0,i,t=0;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	printf("{\"wan_list\":[");
	for(i = 0; i < wan->num; i++){
        if(wan->pinfo[i].enable == 0)
            continue;
        
        if(t == 1){
            printf(",");
        }
        printf("\"%u\"",wan->pinfo[i].id);
        t = 1;
    }
	
	printf("],\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&wan);
	return;
}

void route_snat_add_show()
{
    int iserror=0,size=0,i,t=0;
    struct mgtcgi_xml_interface_wans *wan = NULL;
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
    if (NULL == wan){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }
    
    printf("{\"wan_list\":[");
    for(i = 0; i < wan->num; i++){
        if(wan->pinfo[i].enable == 0)
            continue;
        
        if(t == 1){
            printf(",");
        }
        printf("\"%u\"",wan->pinfo[i].id);
        t = 1;
    }
    
    printf("],\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&wan);
    return;
}

void route_dhcpd_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	printf("{\"name\":\"\",\"ifname\":\"\",\"range\":\"\",\"mask\":\"\",\"gateway\":\"\",\"dnsname\":\"\",\"dns\":\"\",\"lease_time\":86400,\"max_lease_time\":86400,\"enable\":1,\"comment\":\"\"}");
	printf("],");
	print_interface_list(NULL, lan, NULL, NULL, NULL,NULL);
    printf(",");
    printf_lan_info(lan);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&lan);
	return;

}

void route_dhcpd_host_add_show()
{
	int iserror=0,size=0;
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	printf("{\"name\":\"\",\"dhcpname\":\"\",\"mac\":\"\",\"ip\":\"\",\"mask\":\"\",\"gateway\":\"\",\"dnsname\":\"\",\"dns\":\"\",\"lease_time\":0,\"max_lease_time\":0,\"enable\":1,\"comment\":\"\"}");
	printf("],");
	printf_dhcpd_list((void *)dhcpd);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dhcpd);
	return;
}

/******************************** config edit show *****************************/

void route_dns_edit_show()
{
	return;//无
}
void route_iflan_edit_show()
{
	int iserror=0,size=0,index=0;
	char ifname[STRING_LENGTH]={"LAN"};
	char link_status[STRING_LENGTH]={0};
	char macaddr[STRING_LENGTH]={0};
    uint8_t maskbit = 0;
	int ipstr_len = 0;
	struct mgtcgi_xml_interface_lans *lan = NULL;

#if CGI_FORM_STRING
	cgiFormString("ifname", ifname, STRING_LENGTH);
#endif

	if (strlen(ifname) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < lan->num; index++){
		if (strcmp(lan->pinfo[index].ifname, ifname) == 0){
		
			memset(link_status, 0, sizeof(link_status));
			memset(macaddr, 0, sizeof(macaddr));
			get_link_status(lan->pinfo[index].ifname, link_status);
			get_link_macaddr(lan->pinfo[index].ifname, macaddr);
		    mask_to_maskbit(lan->pinfo[index].mask, &maskbit);
			ipstr_len = strlen(lan->pinfo[index].ip);
			snprintf(lan->pinfo[index].ip+ipstr_len, STRING_LENGTH-ipstr_len, 
				"/%u", maskbit);
			printf("{\"ifname\":\"%s\",\"mac\":\"%s\",\"status\":\"%s\",\"ip\":\"%s\",\"aliases\":\"%s\",\"mask\":\"%s\",\"comment\":\"%s\"}",
					lan->pinfo[index].ifname, macaddr, link_status, lan->pinfo[index].ip, lan->pinfo[index].aliases, lan->pinfo[index].mask,
					lan->pinfo[index].comment);

			break;
		}
	}
	printf("],");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&lan);
	return;
}
void route_ifwan_edit_show()
{
#if 0
	int iserror=0,size=0,index=0,flag=0;
	char macaddr[STRING_LENGTH]={0};
	char ifname[STRING_LENGTH]={"WAN1"};
	char link_status[STRING_LENGTH]={0};
	struct mgtcgi_xml_interface_wans *wan = NULL;

#if CGI_FORM_STRING
	cgiFormString("ifname", ifname, STRING_LENGTH);
#endif

	if (strlen(ifname) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < wan->num; index++){
		if (strcmp(wan->pinfo[index].ifname, ifname) == 0){	
			memset(link_status, 0, sizeof(link_status));
			get_link_status(wan->pinfo[index].ifname, link_status);
			get_link_macaddr(wan->pinfo[index].ifname, macaddr);
			
			printf("{\"ifname\":\"%s\",\"mac\":\"%s\",\"status\":\"%s\",\"access\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"weight\":\"%d\",\"defaultroute\":%d,\"comment\":\"%s\"}",
					wan->pinfo[index].ifname, macaddr, link_status, wan->pinfo[index].access, wan->pinfo[index].ip, 
					wan->pinfo[index].mask, wan->pinfo[index].gateway, wan->pinfo[index].weight, wan->pinfo[index].defaultroute,
					wan->pinfo[index].comment);
            flag=1;
			break;
		}
	}

    if(flag == 0){
        editwaninfo(ifname,1);
    }
	printf("],");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&wan);
#endif
	return;
}


void route_vlan_edit_show()
{
#if 0
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"vlan1"};
    char macaddr[STRING_LENGTH] = {0};
    char type[STRING_LENGTH] = {"vlan"};
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    
	printf("{\"data\":[");
	for (index=0; index < vlan->num; index++){
		if (strcmp(vlan->pinfo[index].name, name) == 0){
            if(strlen(vlan->pinfo[index].mac) > 0){
                snprintf(macaddr, sizeof(macaddr),vlan->pinfo[index].mac);
            }
            else{
                get_link_macaddr(vlan->pinfo[index].name, macaddr);
            }

            if(strlen(vlan->pinfo[index].type) > 0){
                strncpy(type,vlan->pinfo[index].type,sizeof(type));
            }
		
			printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"type\":\"%s\",\"id\":%d,\"mac\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"defaultroute\":\"%d\",\"comment\":\"%s\"}",
					vlan->pinfo[index].name, vlan->pinfo[index].ifname, type, vlan->pinfo[index].id, macaddr, 
					vlan->pinfo[index].ip, vlan->pinfo[index].mask, vlan->pinfo[index].gateway, vlan->pinfo[index].defaultroute,vlan->pinfo[index].comment);

			break;
		}
	}
	printf("],");
	print_interface_list(wan, lan, NULL, NULL, NULL,NULL);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&vlan);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&lan);
#endif
	return;
}

void route_pppdauth_edit_show()
{
	return;//
}
void route_adslclient_edit_show()
{
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"adsl1"};
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < adsl->num; index++){
		if (strcmp(adsl->pinfo[index].name, name) == 0){
			printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"servername\":\"%s\",\"mtu\":%d,\"weight\":%d,\"defaultroute\":%d,\"peerdns\":%d,\"enable\":%d,\"comment\":\"%s\"}",
					adsl->pinfo[index].name, adsl->pinfo[index].ifname, adsl->pinfo[index].username, 
					adsl->pinfo[index].password, adsl->pinfo[index].servername, adsl->pinfo[index].mtu,
					adsl->pinfo[index].weight, adsl->pinfo[index].defaultroute, adsl->pinfo[index].peerdns, 
					adsl->pinfo[index].enable, adsl->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	print_ifname_list(wan, vlan);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&vlan);
	return;
}
void route_vpnclient_edit_show()
{
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"vpn1"};
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < l2tpvpn->num; index++){
		if (strcmp(l2tpvpn->pinfo[index].name, name) == 0){
			
			printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"servername\":\"%s\",\"mtu\":%d,\"weight\":%d,\"defaultroute\":%d,\"peerdns\":%d,\"enable\":%d,\"comment\":\"%s\"}",
					l2tpvpn->pinfo[index].name, l2tpvpn->pinfo[index].ifname, l2tpvpn->pinfo[index].username, 
					l2tpvpn->pinfo[index].password, l2tpvpn->pinfo[index].servername, l2tpvpn->pinfo[index].mtu,
					l2tpvpn->pinfo[index].weight, l2tpvpn->pinfo[index].defaultroute, l2tpvpn->pinfo[index].peerdns, 
					l2tpvpn->pinfo[index].enable, l2tpvpn->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	print_ifwan_list(wan);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&wan);
	return;
}

void route_loadb_edit_show()
{
	int iserror=0,size=0,index=0;
	char * pifname = NULL;
	char name[STRING_LENGTH]={"balancing-1"};
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	struct mgtcgi_xml_route_rules * rule = NULL;
 

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif 
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	} 
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");

	for (index=0; index < loadb->num; index++){
		if (strcmp(loadb->pinfo[index].name, name) == 0){
			printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"rule\":\"%s\",\"weight\":%d,\"comment\":\"%s\"}",
			loadb->pinfo[index].name, loadb->pinfo[index].ifname, loadb->pinfo[index].rulename, loadb->pinfo[index].weight, loadb->pinfo[index].comment);
			pifname = loadb->pinfo[index].ifname;
			break;
		}
	}
	printf("],");
	print_loadb_iflist(wan, adsl, l2tpvpn, loadb, vlan, pifname);
	printf(",");
	print_rule_list(rule);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&loadb);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&rule);
	free_xml_node((void*)&vlan);
	return;
}

void route_routing_edit_show()
{
	int iserror=0,size=0,index=0;
	int id=0;
	char id_str[DIGITAL_LENGTH]={"1"};
	struct mgtcgi_xml_routings *routing = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_group_array * table = NULL;
	struct mgtcgi_xml_route_rules * rule = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

#if CGI_FORM_STRING
	cgiFormString("id", id_str, DIGITAL_LENGTH);
#endif

	if (strlen(id_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (id != -1){
		if (!(0 <= id && id <= 65535)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if (NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	if (id == -1){ //默认路由
		printf("{\"data\":[{\"id\":-1,\"dst\":\"\",\"gateway\":\"\",\"ifname\":\"\",\"priority\":1,\"table\":\"\",\"rule\":\"\",\"enable\":1,\"comment\":\"\"}],");
		print_interface_list(wan, lan, adsl, l2tpvpn, NULL,vlan);
		printf(",");
		print_table_list(NULL);
		printf(",");
		print_rule_list(NULL);
		printf(",");
		goto SUCCESS_EXIT;
	}
	
	printf("{\"data\":[");
	for (index=0; index < routing->num; index++){
		if (routing->pinfo[index].id == id){
			printf("{\"id\":%d,\"dst\":\"%s\",\"gateway\":\"%s\",\"ifname\":\"%s\",\"priority\":%d,\"table\":\"%s\",\"rule\":\"%s\",\"enable\":%d,\"comment\":\"%s\"}",
					routing->pinfo[index].id, routing->pinfo[index].dst, routing->pinfo[index].gateway, 
					routing->pinfo[index].ifname, routing->pinfo[index].priority, routing->pinfo[index].table, 
					routing->pinfo[index].rule, routing->pinfo[index].enable, routing->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	print_interface_list(wan, lan, adsl, l2tpvpn, loadb,vlan);
	printf(",");
	print_table_list(table);
	printf(",");
	print_rule_list(rule);
	printf(",");
    
SUCCESS_EXIT:	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&routing);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&lan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&table);
	free_xml_node((void*)&rule);
	free_xml_node((void*)&loadb);
	free_xml_node((void *)&vlan);
	return;
}
void route_rtable_edit_show()
{
	int iserror=0,size=0,index=0,i=0,include_flag=-1;
	char name[STRING_LENGTH]={"TEL"};
	struct mgtcgi_xml_group_array *table = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if (NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	include_flag = -1;
	printf("{\"data\":[");
	for (index=0; index < table->num; index++){
		if (strcmp(table->pinfo[index].group_name, name) == 0){ 		
			printf("{\"name\":\"%s\",\"comment\":\"%s\"}",
					table->pinfo[index].group_name, table->pinfo[index].comment);
			include_flag = 0;
			break;
		}
	}
	printf("],");

	printf("\"iplist\":[");	
	if (include_flag == 0){
		for(i=0; i < table->pinfo[index].num;){
			printf("\"%s\"",table->pinfo[index].pinfo[i].name);
			i++;
			if (i < table->pinfo[index].num)
				printf(",");
			else
				break;
		}		
	}
	printf("],");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&table);
	return;
}
void route_rrule_edit_show()
{
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"web"};
	struct mgtcgi_xml_route_rules *rule = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
#endif

	if (strlen(name) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < rule->num; index++){
		if (strcmp(rule->pinfo[index].name, name) == 0){	
			printf("{\"name\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\"protocol\":\"%s\",\"src_port\":\"%s\",\"dst_port\":\"%s\",\"tos\":\"%s\",\"action\":\"%s\",\"comment\":\"%s\"}",
					rule->pinfo[index].name, rule->pinfo[index].src, rule->pinfo[index].dst,  rule->pinfo[index].protocol,rule->pinfo[index].src_port,
					rule->pinfo[index].dst_port, rule->pinfo[index].tos, strlen(rule->pinfo[index].action)?(rule->pinfo[index].action):"ACCEPT", rule->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&rule);
	return;
}
void route_dnat_edit_show()
{
#if 0
	int iserror=0,size=0,index=0;
	int id=0;
	char id_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_nat_dnats *dnat = NULL;	
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

#if CGI_FORM_STRING
	cgiFormString("id", id_str, DIGITAL_LENGTH);
#endif

	if (strlen(id_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (!(0 <= id && id <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&dnat, &size);
	if (NULL == dnat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < dnat->num; index++){
		if (dnat->pinfo[index].id == id){
			printf("{\"id\":%d,\"ifname\":\"%s\",\"protocol\":\"%s\",\"wan_ip\":\"%s\",\"wan_port\":\"%s\",\"lan_ip\":\"%s\",\"lan_port\":\"%s\",\"enable\":%d,\"comment\":\"%s\"}",
					dnat->pinfo[index].id, dnat->pinfo[index].ifname, dnat->pinfo[index].protocol, 
					dnat->pinfo[index].wan_ip, dnat->pinfo[index].wan_port, dnat->pinfo[index].lan_ip, dnat->pinfo[index].lan_port,
					dnat->pinfo[index].enable, dnat->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	
	print_interface_list(wan, lan, adsl, l2tpvpn, NULL,vlan);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dnat);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&lan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void *)&vlan);
#endif
	return;
}
void route_snat_edit_show()
{
#if 0 
	int iserror=0,size=0,index=0;
	int id=0;
	char id_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_nat_snats *snat = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;


#if CGI_FORM_STRING
	cgiFormString("id", id_str, DIGITAL_LENGTH);
#endif

	if (strlen(id_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (!(0 <= id && id <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&snat, &size);
	if (NULL == snat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}


	printf("{\"data\":[");
	for (index=0; index < snat->num; index++){
		if (snat->pinfo[index].id == id){
			printf("{\"id\":%d,\"ifname\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\",\"protocol\":\"%s\",\"action\":\"%s\",\"dstip\":\"%s\",\"dstport\":\"%s\",\"enable\":%d,\"comment\":\"%s\"}",
					snat->pinfo[index].id, snat->pinfo[index].ifname, snat->pinfo[index].src, 
					snat->pinfo[index].dst, snat->pinfo[index].protocol, snat->pinfo[index].action,
					snat->pinfo[index].dstip, snat->pinfo[index].dstport,
					snat->pinfo[index].enable, snat->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	print_interface_list(wan, lan, adsl, l2tpvpn, NULL,vlan);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&snat);
	free_xml_node((void*)&wan);
	free_xml_node((void*)&lan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void *)&vlan);
#endif
	return;
}

void route_dhcpd_edit_show()
{
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"dhcp1"};
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;

	#if CGI_FORM_STRING
		cgiFormString("name", name, STRING_LENGTH);
	#endif

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < dhcpd->num; index++){
		if (strcmp(dhcpd->pinfo[index].name, name) == 0){
			printf("{\"name\":\"%s\",\"ifname\":\"%s\",\"range\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"dnsname\":\"%s\",\"dns\":\"%s\",\"lease_time\":%u,\"max_lease_time\":%u,\"enable\":%d,\"comment\":\"%s\"}",
				dhcpd->pinfo[index].name, dhcpd->pinfo[index].ifname, dhcpd->pinfo[index].range, 
				dhcpd->pinfo[index].mask, dhcpd->pinfo[index].gateway, dhcpd->pinfo[index].dnsname,
				dhcpd->pinfo[index].dns, dhcpd->pinfo[index].lease_time, dhcpd->pinfo[index].max_lease_time,
				dhcpd->pinfo[index].enable, dhcpd->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	print_interface_list(NULL, lan, NULL, NULL, NULL,NULL);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dhcpd);
	free_xml_node((void*)&lan);
	return;
}

void route_dhcpd_host_edit_show()
{
	int iserror=0,size=0,index=0;
	char name[STRING_LENGTH]={"003"};
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;

	#if CGI_FORM_STRING
		cgiFormString("name", name, STRING_LENGTH);
	#endif

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
	if (NULL == host){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"data\":[");
	for (index=0; index < host->num; index++){
		if (strcmp(host->pinfo[index].name, name) == 0){
			printf("{\"name\":\"%s\",\"dhcpname\":\"%s\",\"mac\":\"%s\",\"ip\":\"%s\",\"mask\":\"%s\",\"gateway\":\"%s\",\"dnsname\":\"%s\",\"dns\":\"%s\",\"lease_time\":%u,\"max_lease_time\":%u,\"enable\":%d,\"comment\":\"%s\"}",
				host->pinfo[index].name, host->pinfo[index].dhcpname, host->pinfo[index].mac, 
				host->pinfo[index].ip, host->pinfo[index].mask, host->pinfo[index].gateway, 
				host->pinfo[index].dnsname, host->pinfo[index].dns, host->pinfo[index].lease_time, 
				host->pinfo[index].max_lease_time, host->pinfo[index].enable, host->pinfo[index].comment);
			break;
		}
	}
	printf("],");
	printf_dhcpd_list((void *)dhcpd);
	printf(",");
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dhcpd);
	free_xml_node((void*)&host);
	return;
}


/******************************** config add save *****************************/
void route_dns_add_save()
{
	return;//无
}
void route_iflan_add_save()
{
	return;//
}

extern int mac_repair(char *mac, const int length, const char *dot);
extern int check_mac_addr(const char *macaddr, const char *dot);

void route_ifwan_add_save()
{
    int iserror=0,index=0,old_index=0,ret_value=0;
    int old_num=0,new_num=0,old_size=0,new_size=0;
    int id=2,i=0,tmp_id=1,defaultroute=0,enable=0;

    const char *dot = ":";
    char ifname[LITTER_LENGTH]={"WAN0"};
    char access[LITTER_LENGTH]={"static"};
    char type[LITTER_LENGTH]={"0"};
    char username[LITTER_LENGTH]={"111"};
    char passwd[LITTER_LENGTH]={"111"};
    char ipaddr[LITTER_LENGTH]={0};
    char netmask[LITTER_LENGTH]={0};
    char gateway[LITTER_LENGTH]={0};
    char service[LITTER_LENGTH]={0};
    char mtu[LITTER_LENGTH]={0};
    char mac[LITTER_LENGTH]={0};
    char dns_1[LITTER_LENGTH]={0};
    char dns_2[LITTER_LENGTH]={0};
    char dns_3[LITTER_LENGTH]={0};
    char sx[LITTER_LENGTH]={0};
    char xx[LITTER_LENGTH]={0};
    char isp[LITTER_LENGTH]={0};
    char default_str[LITTER_STRING]={0};
    char enable_str[LITTER_STRING]={0};
    char timed[LITTER_LENGTH]={0};
    char wday[LITTER_LENGTH]={0};
    char min2[LITTER_LENGTH]={0};

    char sx_str[STRING_LENGTH] = {0};
    char xx_str[STRING_LENGTH] = {0};
    char dns_str[STRING_LENGTH] = {0};
    struct in_addr if_ip,if_mask,if_gateway;
    unsigned long int if_subnet1,if_subnet2;
    struct mgtcgi_xml_interface_wans *old_nodes = NULL;
    struct mgtcgi_xml_interface_wans *new_nodes = NULL;
    
#if CGI_FORM_STRING
    cgiFormString("ifname", ifname, sizeof(ifname));
    cgiFormString("access", access, sizeof(access));
    cgiFormString("username", username, sizeof(username));
    cgiFormString("passwd", passwd, sizeof(passwd));
    cgiFormString("ip", ipaddr, sizeof(ipaddr));
    cgiFormString("mask", netmask, sizeof(netmask));
    cgiFormString("gateway", gateway, sizeof(gateway));
    cgiFormString("service", service, sizeof(service));
    cgiFormString("mtu", mtu, sizeof(mtu));
    cgiFormString("mac", mac, sizeof(mac));
    cgiFormString("dns_1", dns_1, sizeof(dns_1));
    cgiFormString("dns_2", dns_2, sizeof(dns_2));
    cgiFormString("dns_3", dns_3, sizeof(dns_3));
    cgiFormString("sx", sx, sizeof(sx));
    cgiFormString("xx", xx, sizeof(xx));
    cgiFormString("isp", isp, sizeof(isp));
    cgiFormString("default", default_str, sizeof(default_str));
    cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("timed", timed, sizeof(timed));
    cgiFormString("tm_wday", wday, sizeof(wday));
    cgiFormString("tm_min2", min2, sizeof(min2));
#endif 

    mac_repair(mac, strlen(mac), dot);
    if((check_cgiform_string(ifname) != 0)    
        || (check_cgiform_string(access) != 0)
        || (check_cgiform_string(default_str) != 0)
        || (check_cgiform_string(enable_str) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    defaultroute = atoi(default_str);
    if(!((defaultroute >=0) && (defaultroute <= 1))){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    enable = atoi(enable_str);
    if(!((enable >=0) && (enable <= 1))){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    if (strlen(mac) != 17){
        get_link_macaddr(ifname, mac);
    }
    
    iserror = check_mac_addr(mac, dot);
    if (iserror != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    if(strcmp(access,"pppoe") == 0){
        if((strlen(username) == 0) ||
            (strlen(passwd) == 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }
    else if(strcmp(access,"static") == 0){
        if((inet_aton(ipaddr,&if_ip) == 0) ||
            (inet_aton(netmask,&if_mask) == 0) || 
            (inet_aton(gateway,&if_gateway) == 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
        if_subnet1 = (ntohl(if_ip.s_addr) & ntohl(if_mask.s_addr));
        if_subnet2 = (ntohl(if_gateway.s_addr) & ntohl(if_mask.s_addr));
        if(if_subnet1 != if_subnet2){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }
    else if(strcmp(access,"l2tpclient") == 0){
        if((strlen(username) == 0) ||
            (strlen(passwd) == 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }

        if(check_ipaddr(service) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_1) > 0){
        if(check_ipaddr(dns_1) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_2) > 0){
        if(check_ipaddr(dns_2) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_3) > 0){
        if(check_ipaddr(dns_3) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    snprintf(dns_str,sizeof(dns_str),"%s-%s-%s",dns_1,dns_2,dns_3);
    snprintf(sx_str,sizeof(sx_str),"%sKbps",sx);
    snprintf(xx_str,sizeof(xx_str),"%sKbps",xx);
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&old_nodes, &old_size);
    if (NULL == old_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(old_nodes->num > 30){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    //重复检查，ifname不能重复；
    for (index=0; index < old_nodes->num; index++){
        if (strcmp(old_nodes->pinfo[index].ifname, ifname) == 0){
            iserror = MGTCGI_DUPLICATE_ERR;
            goto ERROR_EXIT;
        }
    }

    if(old_nodes->num == 0){
        id = 1;
        defaultroute = 1;
    }
    else{
        for(i=0,tmp_id=1;i<old_nodes->num;i++,tmp_id++){
            if(tmp_id != old_nodes->pinfo[i].id){
                id = tmp_id;
                break;
            }
        }
        if(i == old_nodes->num){
            id = tmp_id;
        }

        if(defaultroute == 1){
            for(index = 0; index < old_nodes->num; index++){
                old_nodes->pinfo[index].defaultroute = 0;
            }
        }
    }

    //将数据添加到结构中
    old_num = old_nodes->num;
    new_num = old_num + 1;

    old_size = sizeof(struct mgtcgi_xml_interface_wans) + 
                old_num * 
                sizeof(struct mgtcgi_xml_interface_wans_info);
    new_size = sizeof(struct mgtcgi_xml_interface_wans) + 
                new_num * 
                sizeof(struct mgtcgi_xml_interface_wans_info);

    new_nodes = (struct mgtcgi_xml_interface_wans *)malloc(new_size);
    if (NULL == new_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    memcpy(new_nodes, old_nodes, old_size);

    new_nodes->num = new_num;
    for(index = 0,old_index=0; index < new_num; index++){
        if(index == (id - 1)){
            new_nodes->pinfo[index].id = id;
            new_nodes->pinfo[index].defaultroute = defaultroute;
            new_nodes->pinfo[index].enable = enable;
            strncpy(new_nodes->pinfo[index].ifname, ifname, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].access, access, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].type, type, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].ip, ipaddr, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].mask, netmask, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].gateway, gateway, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].username, username, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].passwd, passwd, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].servername, service, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].mtu, mtu, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].mac, mac, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].dns, dns_str, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[index].up, sx_str, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].down, xx_str, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].isp, isp, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].time, timed, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].week, wday, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].day, min2, LITTER_LENGTH-1);

            continue;
        }

        if(old_index < old_nodes->num){
            new_nodes->pinfo[index].id = old_nodes->pinfo[old_index].id;
            new_nodes->pinfo[index].defaultroute = old_nodes->pinfo[old_index].defaultroute;
            new_nodes->pinfo[index].enable = old_nodes->pinfo[old_index].enable;
    		strncpy(new_nodes->pinfo[index].ifname, old_nodes->pinfo[old_index].ifname, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].access, old_nodes->pinfo[old_index].access, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].type, old_nodes->pinfo[old_index].type, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].ip, old_nodes->pinfo[old_index].ip, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].mask, old_nodes->pinfo[old_index].mask, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].gateway, old_nodes->pinfo[old_index].gateway, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].username, old_nodes->pinfo[old_index].username, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].passwd, old_nodes->pinfo[old_index].passwd, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].servername, old_nodes->pinfo[old_index].servername, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].mtu, old_nodes->pinfo[old_index].mtu, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].mac, old_nodes->pinfo[old_index].mac, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].dns, old_nodes->pinfo[old_index].dns, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[index].up, old_nodes->pinfo[old_index].up, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].down, old_nodes->pinfo[old_index].down, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].isp, old_nodes->pinfo[old_index].isp, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].time, old_nodes->pinfo[old_index].time, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[index].week, old_nodes->pinfo[old_index].week, LITTER_LENGTH-1);
    		strncpy(new_nodes->pinfo[index].day, old_nodes->pinfo[old_index].day, LITTER_LENGTH-1);
            old_index++;
        }
    }

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)new_nodes, new_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    ret_value = apply_wan_channel("init",id,sx,xx);
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }
    
    ret_value = apply_wan("add",id,ifname,access,ipaddr,netmask,gateway,username,passwd,service,mtu,mac,dns_str,sx,xx,isp,defaultroute,timed,wday,min2);
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_wanchannel_xmlconf, flash_wanchannel_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"id\":%d,\"iserror\":0,\"msg\":\"\"}",id);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }

    return;
}

void route_vlan_add_save(void)
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int id=2;

	const char *dot = ":";
	char name[LITTER_LENGTH]={"vlan2"};
	char ifname[LITTER_LENGTH]={"WAN0"};
	char mac[LITTER_LENGTH]={"00:0c:29:ae:d6:00"};
	char comment[COMMENT_LENGTH]={0};
	char id_str[DIGITAL_LENGTH]={"3"};

	struct mgtcgi_xml_interface_vlans *old_nodes = NULL;
	struct mgtcgi_xml_interface_vlans *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("id", id_str, sizeof(id_str));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("comment", comment, sizeof(comment));
#endif 

	mac_repair(mac, strlen(mac), dot);
	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	id = atoi(id_str);
	if (!(1 <= id && id <= 4096)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strlen(mac) != 17){
		get_link_macaddr(ifname, mac);
	}
	iserror = check_mac_addr(mac, dot);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复检查，name不能重复；相同接口下的id不能重复。
	for (index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}

		if ((strcmp(old_nodes->pinfo[index].ifname, ifname) == 0) &&
			(old_nodes->pinfo[index].id == id)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_interface_vlans) + 
				old_num * 
				sizeof(struct mgtcgi_xml_interface_vlans_info);
	new_size = sizeof(struct mgtcgi_xml_interface_vlans) + 
				new_num * 
				sizeof(struct mgtcgi_xml_interface_vlans_info);

	new_nodes = (struct mgtcgi_xml_interface_vlans *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, sizeof(name));
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, sizeof(ifname));
    strncpy(new_nodes->pinfo[old_num].mac, mac, sizeof(mac));
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH-1);
	new_nodes->pinfo[old_num].id = id;
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
        if(uptate_sys_route_config_sh() != 0){
            iserror = MGTCGI_WRITE_FILE_ERR;
		    goto ERROR_EXIT;
        }
        
		ret_value = apply_vlan(new_num);
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}
		
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
    //    save_route_vlan_web(new_nodes,1);
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

void route_macvlan_add_save(void)
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
    unsigned int max=0,t=0;

	const char *dot = ":";
	const char *head = "veth";
	char name[LITTER_LENGTH]={"veth2"};
	char ifname[LITTER_LENGTH]={"WAN0"};
	char mac[LITTER_LENGTH]={"00:0c:29:ae:d6:00"};
	char comment[COMMENT_LENGTH]={0};
    char *p = NULL;

	struct mgtcgi_xml_interface_macvlans *old_nodes = NULL;
	struct mgtcgi_xml_interface_macvlans *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("comment", comment, sizeof(comment));
#endif 

	mac_repair(mac, strlen(mac), dot);
	if(check_cgiform_string(ifname) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if(strlen(mac) > 0){
	    iserror = check_mac_addr(mac, dot);
	    if (iserror != 0){
		    iserror = MGTCGI_PARAM_ERR;
		    goto ERROR_EXIT;
	    }
    }

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(old_nodes->num == 0){
        max = 1;
    }
    else{
        for(index=0; index < old_nodes->num; index++){
            p = old_nodes->pinfo[index].name;
            t = atoi(p+strlen(head));
            max = (t>max?t:max);
        }
        max++;
    }
    sprintf(name,"%s%u",head,max);

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_interface_macvlans) + 
				old_num * 
				sizeof(struct mgtcgi_xml_interface_macvlans_info);
	new_size = sizeof(struct mgtcgi_xml_interface_macvlans) + 
				new_num * 
				sizeof(struct mgtcgi_xml_interface_macvlans_info);

	new_nodes = (struct mgtcgi_xml_interface_macvlans *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, sizeof(name));
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, sizeof(ifname));
	strncpy(new_nodes->pinfo[old_num].mac, mac, sizeof(mac));
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
    
    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	    goto ERROR_EXIT;
    }
    
	ret_value = apply_macvlan(new_num);
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
 //   save_route_macvlan_web(new_nodes, 1);
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

void route_pppdauth_add_save()
{
	return;//
}
void route_adslclient_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int mtu=0,weight=0,defaultroute=0,peerdns=0,enable=0;

	char name[STRING_LENGTH]={"adsl3"};
	char ifname[STRING_LENGTH]={"WAN3"};
	char username[STRING_LENGTH]={"test3"};
	char password[STRING_LENGTH]={"test3"};
	char servername[STRING_LENGTH]={0};
	char comment[COMMENT_LENGTH]={0};

	char mtu_str[DIGITAL_LENGTH]={"1480"};
	char weight_str[DIGITAL_LENGTH]={"0"};
	char defaultroute_str[DIGITAL_LENGTH]={"0"};
	char peerdns_str[DIGITAL_LENGTH]={"0"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char ifdev[STRING_LENGTH]={0};
	
	struct mgtcgi_xml_adsl_clients *old_nodes = NULL;
	struct mgtcgi_xml_adsl_clients *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("username", username, sizeof(username));
	cgiFormString("password", password, sizeof(password));
	cgiFormString("servername", servername, sizeof(servername));
	cgiFormString("comment", comment, sizeof(comment));
	
	cgiFormString("mtu", mtu_str, sizeof(mtu_str));
	cgiFormString("weight", weight_str, sizeof(weight_str));
	cgiFormString("defaultroute", defaultroute_str, sizeof(defaultroute_str));
	cgiFormString("peerdns", peerdns_str, sizeof(peerdns_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
#endif 

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(username) != 0)
		|| (check_cgiform_string(password) != 0)
		|| (check_cgiform_string(mtu_str) != 0)
		|| (check_cgiform_string(weight_str) != 0)
		|| (check_cgiform_string(defaultroute_str) != 0)
		|| (check_cgiform_string(peerdns_str) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_adsl_name(name) != 0){ //名称必须adsl+后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if(check_passwd(password) != 0){  //密码必须是数字和字母组成
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

	mtu = atoi(mtu_str);
	weight = atoi(weight_str);
	defaultroute = atoi(defaultroute_str);
	peerdns = atoi(peerdns_str);
	enable = atoi(enable_str);
	if (!((1200 <= mtu && mtu <= 1500) && 
		  (1 <= weight && weight <= 99) && 
		  (0 <= defaultroute && defaultroute <= 1) && 
		  (0 <= peerdns && peerdns <= 1) &&
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_ppp_ifname(ifname, ifdev) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if( defaultroute == 1)
	{
		iserror = change_defaultroute();
		if(iserror != 0)
		{
			goto ERROR_EXIT;
		}
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复检查，name不能重复
	for (index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}

		/*if ((strcmp(old_nodes->pinfo[index].username, username) == 0) &&
			(strcmp(old_nodes->pinfo[index].password, password) == 0)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}*/
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_adsl_clients) + 
				old_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
	new_size = sizeof(struct mgtcgi_xml_adsl_clients) + 
				new_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);

	new_nodes = (struct mgtcgi_xml_adsl_clients *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].username, username, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].password, password, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].servername, servername, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);

	new_nodes->pinfo[old_num].mtu = mtu;
	new_nodes->pinfo[old_num].weight = weight;
	new_nodes->pinfo[old_num].defaultroute = defaultroute;
	new_nodes->pinfo[old_num].peerdns = peerdns;
	new_nodes->pinfo[old_num].enable = enable;

    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	ret_value = apply_adsl(1, username, password, name, ifdev, username, password, servername, mtu, defaultroute, enable, peerdns);

    if (ret_value != 0){
		iserror = MGTCGI_ADSL_DIAL_ERR;
		//goto ERROR_EXIT;
	}
#if 0
    ret_value = apply_dnat();
    if (ret_value != 0){
	    iserror = MGTCGI_APPY_ERR;
	    goto ERROR_EXIT;
    }
#endif

    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
    
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

#if 0
void route_vpnclient_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int mtu=0,weight=0,defaultroute=0,peerdns=0,enable=0;

	char name[STRING_LENGTH]={"adsl3"};
	char ifname[STRING_LENGTH]={"WAN3"};
	char username[STRING_LENGTH]={"test3"};
	char password[STRING_LENGTH]={"test3"};
	char servername[STRING_LENGTH]={"10.200.15.15"};
	char comment[COMMENT_LENGTH]={0};
	
	char mtu_str[DIGITAL_LENGTH]={"1480"};
	char weight_str[DIGITAL_LENGTH]={"0"};
	char defaultroute_str[DIGITAL_LENGTH]={"0"};
	char peerdns_str[DIGITAL_LENGTH]={"0"};
	char enable_str[DIGITAL_LENGTH]={"1"};

	struct mgtcgi_xml_l2tpvpn_clients *old_nodes = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("username", username, sizeof(username));
	cgiFormString("password", password, sizeof(password));
	cgiFormString("servername", servername, sizeof(servername));
	cgiFormString("comment", comment, sizeof(comment));
	
	cgiFormString("mtu", mtu_str, sizeof(mtu_str));
	cgiFormString("weight", weight_str, sizeof(weight_str));
	cgiFormString("defaultroute", defaultroute_str, sizeof(defaultroute_str));
	cgiFormString("peerdns", peerdns_str, sizeof(peerdns_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(username) != 0)
		|| (check_cgiform_string(password) != 0)
		|| (check_cgiform_string(servername) != 0)
		|| (check_cgiform_string(mtu_str) != 0)
		|| (check_cgiform_string(weight_str) != 0)
		|| (check_cgiform_string(defaultroute_str) != 0)
		|| (check_cgiform_string(peerdns_str) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_l2tpvpn_name(name) != 0){ //名称必须l2tpvpn+后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	mtu = atoi(mtu_str);
	weight = atoi(weight_str);
	defaultroute = atoi(defaultroute_str);
	peerdns = atoi(peerdns_str);
	enable = atoi(enable_str);
	if (!((1200 <= mtu && mtu <= 1500) && 
		  (1 <= weight && weight <= 99) && 
		  (0 <= defaultroute && defaultroute <= 1) && 
		  (0 <= peerdns && peerdns <= 1) &&
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if( defaultroute == 1)
	{
		iserror = change_defaultroute();
		if(iserror != 0)
		{
			goto ERROR_EXIT;
		}
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复检查，name不能重复
	for (index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_l2tpvpn_clients) + 
				old_num * 
				sizeof(struct mgtcgi_xml_l2tpvpn_clients_info);
	new_size = sizeof(struct mgtcgi_xml_l2tpvpn_clients) + 
				new_num * 
				sizeof(struct mgtcgi_xml_l2tpvpn_clients_info);

	new_nodes = (struct mgtcgi_xml_l2tpvpn_clients *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].username, username, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].password, password, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].servername, servername, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	
	new_nodes->pinfo[old_num].mtu = mtu;
	new_nodes->pinfo[old_num].weight = weight;
	new_nodes->pinfo[old_num].defaultroute = defaultroute;
	new_nodes->pinfo[old_num].peerdns = peerdns;
	new_nodes->pinfo[old_num].enable = enable;
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = apply_l2tpvpn(name, name, ifname, username, password, servername, mtu, defaultroute, enable, peerdns);
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; //保存成功，应用失败
			//goto ERROR_EXIT;
		}
		xl2tpd_restart();
		ret_value = 0;
		for (index=0; index < new_nodes->num; index++){
			if (new_nodes->pinfo[index].enable == 0)
				continue;
			if (l2tpvpn_client_dial("up", new_nodes->pinfo[index].name) != 0){
				ret_value = -1;
			}
		}
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			//goto ERROR_EXIT;
		}
        if(old_nodes){
            free_xml_node((void*)&old_nodes);
            old_nodes = NULL;
        }

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}
#endif

void route_loadb_add_save()
{
	int iserror=0,size=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int weight=0;
	char name[STRING_LENGTH] = {"balance-3"};
	char ifname[STRING_LENGTH] = {"WAN3"};
	char rule[STRING_LENGTH] = {"WAN3"};
	char weight_str[STRING_LENGTH] = {"1"};
	char comment[STRING_LENGTH] = {0};

	struct mgtcgi_xml_load_balancings *old_nodes = NULL;
	struct mgtcgi_xml_load_balancings *new_nodes = NULL;
 
#if CGI_FORM_STRING
	cgiFormString("name", name, STRING_LENGTH);
	cgiFormString("ifname", ifname, STRING_LENGTH);
	cgiFormString("weight", weight_str, STRING_LENGTH);
	cgiFormString("rule", rule, STRING_LENGTH);
	cgiFormString("comment", comment, STRING_LENGTH);
#endif 
	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(weight_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_loadb_name(name) != 0){ //名称必须balance+后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	weight = atoi(weight_str);
	if ( 1 > weight || weight > 100){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strlen(rule) < 1)
		strncpy(rule, "null", sizeof(rule));
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	} 

	for (index=0; index < old_nodes->num; index ++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
		if (strcmp(old_nodes->pinfo[index].ifname, ifname) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_load_balancings) + 
				old_num * 
				sizeof(struct mgtcgi_xml_load_balancings_info);
	new_size = sizeof(struct mgtcgi_xml_load_balancings) + 
				new_num * 
				sizeof(struct mgtcgi_xml_load_balancings_info);

	new_nodes = (struct mgtcgi_xml_load_balancings *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].rulename, rule, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	new_nodes->pinfo[old_num].weight = weight;
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

void route_macbind_add_save()
{
	int action=0,iserror=0,ret_value=0,old_num=0,new_num=0,size=0,new_size=0,old_size=0;
	const char dot[] = ":";
	char *value = NULL;
	//char cmd[CMD_LENGTH]={0};
	char ipaddr[STRING_LENGTH]={"0"};
	char macaddr[STRING_LENGTH]={"0"};
	char action_str[STRING_LENGTH]={"0"};
	char comment_str[STRING_LENGTH] = {"0"};
	
	struct mgtcgi_xml_mac_binds *new_nodes = NULL;
	struct mgtcgi_xml_mac_binds *old_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("ipaddr", ipaddr, sizeof(ipaddr));
	cgiFormString("macaddr", macaddr, sizeof(macaddr));
	cgiFormString("action", action_str, sizeof(action_str));
	cgiFormString("comment", comment_str, sizeof(comment_str));
#endif
	mac_repair(macaddr, strlen(macaddr), dot);
	action=atoi(action_str);
	if ((check_ipaddr(ipaddr) < 0) || 
		(check_mac_addr(macaddr, dot) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	} 	

	old_num = old_nodes->num;
	new_num = old_num + 1; 
	old_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				old_num * 
				sizeof(struct mgtcgi_xml_mac_binds_info);
	new_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				new_num * 
				sizeof(struct mgtcgi_xml_mac_binds_info);

	new_nodes = (struct mgtcgi_xml_mac_binds *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//将数据添加到结构中
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].ip, ipaddr, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].mac, macaddr, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment_str, STRING_LENGTH);
	new_nodes->pinfo[old_num].action = action;
	new_nodes->pinfo[old_num].bridge= 0;
	new_nodes->pinfo[old_num].id= old_num;
    
    if(old_nodes != NULL){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	new_size = 0;
	ret_value = save_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = add_macbind(ipaddr, macaddr, action);
		ret_value = copy_mac_bind_xmlconfig_file(working_mac_bind_xmlconf, flash_mac_bind_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}	
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

    if(value!=NULL){
        free(value);
        value = NULL;
    }
	return;

}

void route_routing_add_save()
{
	int iserror=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int enable=0,priority=0,id=0;
	int iftype=0,i=0;
	char buf[STRING_LENGTH]={0};

	char dst[STRING_LENGTH]={0};
	char gateway[STRING_LENGTH]={"WAN1"};
	char ifname[STRING_LENGTH]={"WAN1"};
	char priority_str[STRING_LENGTH]={"1"};
	char table[STRING_LENGTH]={"null"};
	char rule[STRING_LENGTH]={"null"};
	char enable_str[STRING_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};

	struct mgtcgi_xml_routings *old_nodes = NULL;
	struct mgtcgi_xml_routings *new_nodes = NULL;
	//struct mgtcgi_xml_load_balancings *loadb = NULL;

#if CGI_FORM_STRING
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("priority", priority_str, sizeof(priority_str));
	cgiFormString("table", table, sizeof(table));
	cgiFormString("rule", rule, sizeof(rule));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	memset(buf, 0, sizeof(buf));
	for (i=0; i < strlen(ifname); i++){
		if (i >= 3){
			buf[i] = '\0';
			break;
		}
		else{
			buf[i] = ifname[i];
		}
	}
	if (strcmp(buf, "bal") == 0){
		memset(table, 0, sizeof(table));
		memset(rule, 0, sizeof(rule));
		memset(dst, 0, sizeof(dst));
		strncpy(table, "null", sizeof(table));
		strncpy(rule, "null", sizeof(rule));
	}
	
	if((check_cgiform_string(ifname) != 0)
		|| (check_cgiform_string(table) != 0)
		|| (check_cgiform_string(rule) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iftype = check_interface_type(ifname);
	//dst，table, rule至少填写一个。
	if (iftype != 5){
		if (!((check_cgiform_string(dst) == 0) ||
			(strcmp(table, "null") != 0) ||
			(strcmp(rule, "null") != 0))){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}

	//dst不为空，检查dst合法性
	if ((check_cgiform_string(dst) == 0) && (check_ipaddr(dst) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	priority = atoi(priority_str);
	enable = atoi(enable_str);
	if (!((1 <= priority && priority <= 250) && 
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_routings) + 
				old_num * 
				sizeof(struct mgtcgi_xml_routings_info);
	new_size = sizeof(struct mgtcgi_xml_routings) + 
				new_num * 
				sizeof(struct mgtcgi_xml_routings_info);

	new_nodes = (struct mgtcgi_xml_routings *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	id = old_num;
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].dst, dst, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].gateway, gateway, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].table, table, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].rule, rule, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	new_nodes->pinfo[old_num].id = id;
	new_nodes->pinfo[old_num].priority = priority;
	new_nodes->pinfo[old_num].enable = enable;
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = update_list_routing();
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; //成功，更新列表失败
			goto ERROR_EXIT;
		}
		
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

extern int groups_add_save(int mgtcgi_group_type, int add_includ_num,const char *name, const char *value, const char *comment);

void route_rtable_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_size=0;
	int add_includ_num=0;
	char *dot=",";

	char name[STRING_LENGTH]={"tt"};
	//char value[STRING_LIST_LENGTH]={"192.168.0.0/24,192.168.1.0/24,192.168.2.0/24,,"};
    char *value = NULL;
    char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_group_array *old_nodes = NULL;

    value = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!value)
        goto ERROR_EXIT;
    
#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("value", value, STRING_LIST_LENGTH * sizeof(char));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	if((check_cgiform_string(name) != 0) 
		|| (check_cgiform_string(value) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	add_includ_num = check_table_dstip_list(value, dot);
	if (add_includ_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&old_nodes, &old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复性检查
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].group_name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}		
	}

	ret_value = groups_add_save(MGTCGI_TYPE_ROUTE_TABLES,
								add_includ_num, name, value, comment);

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&old_nodes);

    if(value){
        free(value);
        value = NULL;
    }
	return;
}
void route_rrule_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	
	char name[STRING_LENGTH]={"web"};
	char src[STRING_LENGTH]={0};
	char dst[STRING_LENGTH]={"2.4.5.6"};
	char protocol[STRING_LENGTH]={0};
    char src_port[STRING_LENGTH]={0};
	char dst_port[STRING_LENGTH]={0};
	char tos[STRING_LENGTH]={0};
    char action[STRING_LENGTH] = {0};
	char comment[COMMENT_LENGTH]={0};

	struct mgtcgi_xml_route_rules *old_nodes = NULL;
	struct mgtcgi_xml_route_rules *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("src", src, sizeof(src));
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("protocol", protocol, sizeof(protocol));
    cgiFormString("src_port", src_port, sizeof(src_port));
	cgiFormString("dst_port", dst_port, sizeof(dst_port));
	cgiFormString("tos", tos, sizeof(tos));
    cgiFormString("action", action, sizeof(action));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	if((check_cgiform_string(name) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if((check_cgiform_string(action) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

	//src,dst,protocol,tos至少输入一个。
/*	if(!((check_cgiform_string(src) == 0) 
		|| (check_cgiform_string(dst) == 0)
		|| (strcmp(protocol, "null") != 0)
		|| (strcmp(tos, "null") != 0))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if ((strcmp(protocol, "null") != 0) && 
		(check_cgiform_string(dst_port) != 0) && (check_cgiform_string(src_port) != 0) ){		//协议不为null，端口必须输入
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
*/
	if ((check_cgiform_string(src) == 0) && (check_ipaddr(src) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	if ((check_cgiform_string(dst) == 0) && (check_ipaddr(dst) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重名检查
	for (index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_route_rules) + 
				old_num * 
				sizeof(struct mgtcgi_xml_route_rules_info);
	new_size = sizeof(struct mgtcgi_xml_route_rules) + 
				new_num * 
				sizeof(struct mgtcgi_xml_route_rules_info);

	new_nodes = (struct mgtcgi_xml_route_rules *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);

	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].src, src, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dst, dst, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].protocol, protocol, STRING_LENGTH);
    strncpy(new_nodes->pinfo[old_num].src_port, src_port, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dst_port, dst_port, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].tos, tos, STRING_LENGTH);
    strncpy(new_nodes->pinfo[old_num].action, action, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
        ret_value = route_rule_apply(1,src,src_port,dst,dst_port,tos,protocol,action);
    
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&old_nodes);
	free(new_nodes);
	return;
}
void route_dnat_add_save()
{
	int iserror=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int enable=0,loop_enable=0,index=0;
    int w_before,w_after,l_before,l_after;

    char enable_str[DIGITAL_LENGTH]={"0"};
    char comment[STRING_LENGTH]={0};
    char protocol[LITTER_STRING]={"TCP"};
    char src[STRING_LENGTH]={""};
    char wanport_before[DIGITAL_LENGTH]={""};
    char wanport_after[DIGITAL_LENGTH]={""};
    char lanport_before[DIGITAL_LENGTH]={""};
    char lanport_aftere[DIGITAL_LENGTH]={""};
    char dst[STRING_LENGTH]={""};
	char ifname[STRING_LENGTH]={"1"};
    char loop_enable_str[DIGITAL_LENGTH] = {"0"};
	
	struct mgtcgi_xml_nat_dnats *old_nodes = NULL;
	struct mgtcgi_xml_nat_dnats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("comment", comment, sizeof(comment));
    cgiFormString("protocol", protocol, sizeof(protocol));
    cgiFormString("src", src, sizeof(src));
    cgiFormString("wanport_before", wanport_before, sizeof(wanport_before));
    cgiFormString("wanport_after", wanport_after, sizeof(wanport_after));
    cgiFormString("lanport_before", lanport_before, sizeof(lanport_before));
    cgiFormString("lanport_aftere", lanport_aftere, sizeof(lanport_aftere));
    cgiFormString("dst", dst, sizeof(dst));
    cgiFormString("ifname", ifname, sizeof(ifname));
    cgiFormString("loop_enable", loop_enable_str, sizeof(loop_enable_str));
#endif
	
	if((check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(protocol) != 0)
		|| (check_cgiform_string(enable_str) != 0)
		|| (check_cgiform_string(wanport_before) != 0)
		|| (check_cgiform_string(dst) != 0)
		|| (check_cgiform_string(comment) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	w_before = atoi(wanport_before);
	if (!(1 <= w_before && w_before <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    //端口检查
    if(check_cgiform_string(wanport_after) == 0){
        w_after = atoi(wanport_after);
        if (!(1 <= w_after && w_after <= 65535)){
    		iserror = MGTCGI_PARAM_ERR;
    		goto ERROR_EXIT;
	    }
        if(w_before >= w_after){
            iserror = MGTCGI_PARAM_ERR;
		    goto ERROR_EXIT;
        }

        if((check_cgiform_string(lanport_before) != 0)
            || (check_cgiform_string(lanport_aftere) != 0)){
            strncpy(lanport_before,wanport_before,DIGITAL_LENGTH-1);
            strncpy(lanport_aftere,wanport_after,DIGITAL_LENGTH-1);
        }
        else{
            l_before = atoi(lanport_before);
    		if (!(1 <= l_before && l_before <= 65535)){
    			iserror = MGTCGI_PARAM_ERR;
    			goto ERROR_EXIT;
    		}
            l_after = atoi(lanport_aftere);
    		if (!(1 <= l_after && l_after <= 65535)){
    			iserror = MGTCGI_PARAM_ERR;
    			goto ERROR_EXIT;
    		}
            if(l_before >= l_after){
                iserror = MGTCGI_PARAM_ERR;
    			goto ERROR_EXIT;
            }
        } 
    }
    else{
        memset(lanport_aftere,0,sizeof(lanport_aftere));
        if(check_cgiform_string(lanport_before) != 0){
            strncpy(lanport_before,wanport_before,DIGITAL_LENGTH-1);
        }
        l_before = atoi(lanport_before);
		if (!(1 <= l_before && l_before <= 65535)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
    }

    if(check_cgiform_string(src) == 0){
		if (check_ipaddr(src) != 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
    
	if (check_ipaddr(dst) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
    
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    loop_enable = atoi(loop_enable_str);
	if (!(0 <= loop_enable && loop_enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    //名称检查
    for(index = 0; index < old_nodes->num; index++){
        if(strcmp(old_nodes->pinfo[index].comment,comment) == 0){
            iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;    
        }
    }

	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	
	
	old_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
				old_num * 
				sizeof(struct mgtcgi_xml_nat_dnats_info);
	new_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
				new_num * 
				sizeof(struct mgtcgi_xml_nat_dnats_info);
	
	new_nodes = (struct mgtcgi_xml_nat_dnats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	
	new_nodes->num = new_num;
    new_nodes->pinfo[old_num].id = new_num;
    new_nodes->pinfo[old_num].enable = enable;
    new_nodes->pinfo[old_num].loop_enable = loop_enable;
    strncpy(new_nodes->pinfo[old_num].protocol, protocol, LITTER_STRING-1);
    strncpy(new_nodes->pinfo[old_num].comment, comment, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].wan_ip, src, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].wan_port_before, wanport_before, DIGITAL_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].wan_port_after, wanport_after, DIGITAL_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].lan_port_before, lanport_before, DIGITAL_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].lan_port_after, lanport_aftere, DIGITAL_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].lan_ip, dst, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH-1);
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
	    goto ERROR_EXIT;
    }

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
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

void route_snat_add_save()
{
	int iserror=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	int id=0,enable=0;

	char ifname[STRING_LENGTH]={"WAN1"};
	char protocol[STRING_LENGTH]={"all"};
	char src[STRING_LENGTH]={0};
	char dst[STRING_LENGTH]={0};
	char action[STRING_LENGTH]={"MASQUERADE"};
	char dstip[STRING_LENGTH]={0};
	char dstport[STRING_LENGTH]={0};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_nat_snats *old_nodes = NULL;
	struct mgtcgi_xml_nat_snats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("protocol", protocol, sizeof(protocol));
	cgiFormString("src", src, sizeof(src));
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("action", action, sizeof(action));
	cgiFormString("dstip", dstip, sizeof(dstip));
	cgiFormString("dstport", dstport, sizeof(dstport));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	
	if((check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(protocol) != 0)
		|| (check_cgiform_string(action) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//action选择为SNAT，dstip不能为空。
	if ((strcmp(action, "SNAT") == 0) &&
		(check_cgiform_string(dstip) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//协议选择为tcp或udp，action为SNAT, dstport不能为空
	if ((strcmp(protocol, "all") != 0) && 
		(strcmp(action, "SNAT") == 0) &&
		(check_cgiform_string(dstport) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	 
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//action为MASQUERADE， dstip，dstport无效
	if (strcmp(action, "MASQUERADE") == 0){
		dstip[0] = 0;
		dstport[0] = 0;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	
	
	old_size = sizeof(struct mgtcgi_xml_nat_snats) + 
				old_num * 
				sizeof(struct mgtcgi_xml_nat_snats_info);
	new_size = sizeof(struct mgtcgi_xml_nat_snats) + 
				new_num * 
				sizeof(struct mgtcgi_xml_nat_snats_info);
	
	new_nodes = (struct mgtcgi_xml_nat_snats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	id = old_num;
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	
	new_nodes->num = new_num;
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].src, src, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dst, dst, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].protocol, protocol, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].action, action, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dstip, dstip, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dstport, dstport, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	new_nodes->pinfo[old_num].enable = enable;
	new_nodes->pinfo[old_num].id = id;
	
    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void*)new_nodes, new_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }
    
    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    
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

void route_dhcpd_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	unsigned int lease_time,max_lease_time;
	int enable=1;

	char name[STRING_LENGTH]={"dhcp1"};
	char ifname[STRING_LENGTH]={"LAN"};
	char range[STRING_LENGTH]={"192.168.0.100-192.168.0.101"};
	char mask[STRING_LENGTH]={"255.255.254.0"};
	char gateway[STRING_LENGTH]={"192.168.0.1"};
	char dnsname[STRING_LENGTH]={""};
	char dns[STRING_LENGTH]={"61.139.2.69,8.8.8.8"};
	char lease_time_str[DIGITAL_LENGTH]={"86400"};
	char max_lease_time_str[DIGITAL_LENGTH]={"86400"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_dhcpd_servers *old_nodes = NULL;
	struct mgtcgi_xml_dhcpd_servers *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("range", range, sizeof(range));
	cgiFormString("mask", mask, sizeof(mask));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("dnsname", dnsname, sizeof(dnsname));
	cgiFormString("dns", dns, sizeof(dns));
	cgiFormString("lease_time", lease_time_str, sizeof(lease_time_str));
	cgiFormString("max_lease_time", max_lease_time_str, sizeof(max_lease_time_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(ifname) != 0)
		|| (check_cgiform_string(range) != 0)
		|| (check_cgiform_string(mask) != 0)
		|| (check_cgiform_string(gateway) != 0)
		|| (check_cgiform_string(dns) != 0)
		|| (check_cgiform_string(lease_time_str) != 0)
		|| (check_cgiform_string(max_lease_time_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
    
	//iserror = dhcpd_ip_pool_check(ifname, range, mask, gateway);
    iserror = dhcpd_ip_pool_legal_check(ifname, range, mask, gateway);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iserror = check_table_dstip_list(dns, NULL);
	if (iserror < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	lease_time = strtoul(lease_time_str, NULL, 10);
	if (!((unsigned int)3600 <= lease_time && lease_time <= (unsigned int)2592000)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	max_lease_time = strtoul(max_lease_time_str, NULL, 10);
	if (!((unsigned int)3600 <= max_lease_time && max_lease_time <= (unsigned int)2592000)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复检查
	for(index=0; index<old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}
	
	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	
	
	old_size = sizeof(struct mgtcgi_xml_dhcpd_servers) + 
				old_num * 
				sizeof(struct mgtcgi_xml_dhcpd_servers_info);
	new_size = sizeof(struct mgtcgi_xml_dhcpd_servers) + 
				new_num * 
				sizeof(struct mgtcgi_xml_dhcpd_servers_info);
	
	new_nodes = (struct mgtcgi_xml_dhcpd_servers *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	new_nodes->dhcpd_enable = old_nodes->dhcpd_enable;
	
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ifname, ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].range, range, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].mask, mask, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].gateway, gateway, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dnsname, dnsname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dns, dns, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	new_nodes->pinfo[old_num].lease_time = lease_time;
	new_nodes->pinfo[old_num].max_lease_time =  max_lease_time;
	new_nodes->pinfo[old_num].enable =  enable;
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	//	ret_value = apply_dhcpd();
        ret_value = apply_dnsmasq_dhcpd();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&old_nodes);
	free(new_nodes);
	return;
}


void route_dhcpd_host_add_save()
{
	int iserror=0,index=0,ret_value=0;
	int old_num=0,new_num=0,old_size=0,new_size=0;
	unsigned int lease_time,max_lease_time;
	int enable;
	const char *dot=":";
	
	char name[STRING_LENGTH]={"031"};
	char dhcpname[STRING_LENGTH]={"DHCP1"};
	char mac[STRING_LENGTH]={"00:11:22:33:44:55"};
	char ip[STRING_LENGTH]={"192.168.0.200"};
	char mask[STRING_LENGTH]={""};
	char gateway[STRING_LENGTH]={""};
	char dnsname[STRING_LENGTH]={""};
	char dns[STRING_LENGTH]={"61.139.2.69,8.8.8.8"};
	char lease_time_str[DIGITAL_LENGTH]={"86400"};
	char max_lease_time_str[DIGITAL_LENGTH]={"86400"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_dhcpd_hosts *old_nodes = NULL;
	struct mgtcgi_xml_dhcpd_hosts *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
	cgiFormString("dhcpname", dhcpname, sizeof(dhcpname));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("ip", ip, sizeof(ip));
	cgiFormString("mask", mask, sizeof(mask));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("dnsname", dnsname, sizeof(dnsname));
	cgiFormString("dns", dns, sizeof(dns));
	cgiFormString("lease_time", lease_time_str, sizeof(lease_time_str));
	cgiFormString("max_lease_time", max_lease_time_str, sizeof(max_lease_time_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(dhcpname) != 0)
		|| (check_cgiform_string(mac) != 0)
		|| (check_cgiform_string(ip) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iserror = dhcpd_host_ip_pool_check(dhcpname, ip, mask, gateway);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	mac_repair(mac, strlen(mac), dot);
	iserror = check_mac_addr(mac, dot);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	if (check_cgiform_string(dns) == 0){
		iserror = check_table_dstip_list(dns, NULL);
		if (iserror < 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	lease_time = strtoul(lease_time_str, NULL, 10);
	if (lease_time != (unsigned int)0)
		if (!((unsigned int)3600 <= lease_time && lease_time <= (unsigned int)2592000)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	max_lease_time = strtoul(max_lease_time_str, NULL, 10);
	if (max_lease_time != (unsigned int)0)
		if (!((unsigned int)3600 <= max_lease_time && max_lease_time <= (unsigned int)2592000)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&old_nodes, &old_size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//重复检查
	for(index=0; index<old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}
	
	//将数据添加到结构中
	old_num = old_nodes->num;
	new_num = old_num + 1;	
	
	old_size = sizeof(struct mgtcgi_xml_dhcpd_hosts) + 
				old_num * 
				sizeof(struct mgtcgi_xml_dhcpd_hosts_info);
	new_size = sizeof(struct mgtcgi_xml_dhcpd_hosts) + 
				new_num * 
				sizeof(struct mgtcgi_xml_dhcpd_hosts_info);
	
	new_nodes = (struct mgtcgi_xml_dhcpd_hosts *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	
	strncpy(new_nodes->pinfo[old_num].name, name, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dhcpname, dhcpname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].mac, mac, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].ip, ip, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].mask, mask, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].gateway, gateway, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dnsname, dnsname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].dns, dns, STRING_LENGTH);
	strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH);
	new_nodes->pinfo[old_num].lease_time = lease_time;
	new_nodes->pinfo[old_num].max_lease_time =  max_lease_time;
	new_nodes->pinfo[old_num].enable =  enable;
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
		
		ret_value = apply_dhcpd();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&old_nodes);
	free(new_nodes);
	return;
}

void route_pptpdvpn_account_add_save()
{
    int iserror=0,retval=0,old_size=0,new_size=0;
    int old_num=0,new_num=0;
    char username[STRING_LENGTH] = {"123"};
    char passwd[STRING_LENGTH] = {"123"};
    char ip[STRING_LENGTH] = {0};
    struct mgtcgi_xml_pptpd_accounts_info *account = NULL;
    struct mgtcgi_xml_pptpd_accounts_info *new_account = NULL;

#if CGI_FORM_STRING
    cgiFormString("username", username, sizeof(username));
    cgiFormString("passwd", passwd, sizeof(passwd));
    cgiFormString("ip", ip, sizeof(ip));
#endif

    if((check_pptpd_username(username) != 0) ||
        (check_pptpd_passwd(passwd) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strlen(ip) > 0){
        if(check_ipaddr(ip) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_ACCOUNT, (void**)&account, &old_size);
    if (NULL == account){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    old_num = account->num;
    new_num = old_num + 1;

    old_size = sizeof(struct mgtcgi_xml_pptpd_accounts_info) + 
            old_num * sizeof(struct mgtcgi_xml_pptpd_account);
    new_size = sizeof(struct mgtcgi_xml_pptpd_accounts_info) + 
            new_num * sizeof(struct mgtcgi_xml_pptpd_account);

    new_account = (struct mgtcgi_xml_pptpd_accounts_info *)malloc(new_size);
    if(new_account == NULL){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto ERROR_EXIT;
    }
    memset(new_account, 0, new_size);
    memcpy(new_account, account, old_size);

    new_account->num = new_num;
    strncpy(new_account->pinfo[old_num].username,username,STRING_LENGTH-1);
    strncpy(new_account->pinfo[old_num].passwd,passwd,STRING_LENGTH-1);
    strncpy(new_account->pinfo[old_num].ip,ip,STRING_LENGTH-1);

    retval = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_ACCOUNT, (void*)new_account, new_size);
    if(retval < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    retval = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (retval != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    
    retval = apply_pptpd_account(username, passwd, ip);
    if(retval != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;

ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&account);
    return;
}

/******************************** config edit save *****************************/
void route_dns_edit_save()
{
	int  ret_value=0,iserror=0,size;
	char dns_primary[STRING_LENGTH]={"202.98.96.68"};
	char dns_secondary[STRING_LENGTH]={"61.139.2.66"};
	char dns_thirdary[STRING_LENGTH]={""};
	struct mgtcgi_xml_dns *dns = NULL;

#if CGI_FORM_STRING
	cgiFormString("dns_primary", dns_primary, sizeof(dns_primary));
	cgiFormString("dns_secondary", dns_secondary, sizeof(dns_secondary));
	cgiFormString("dns_thirdary", dns_thirdary, sizeof(dns_thirdary));
#endif

	if ((check_cgiform_string(dns_primary) == 0) && (check_ipaddr(dns_primary) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if ((check_cgiform_string(dns_secondary) == 0) && (check_ipaddr(dns_secondary) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if ((check_cgiform_string(dns_thirdary) == 0) && (check_ipaddr(dns_thirdary) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void**)&dns, &size);
	if (NULL == dns){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	strncpy(dns->dns_primary, dns_primary, STRING_LENGTH);
	strncpy(dns->dns_secondary, dns_secondary, STRING_LENGTH);
	strncpy(dns->dns_thirdary, dns_thirdary, STRING_LENGTH);

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void*)dns, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = apply_dns(dns_primary, dns_secondary, dns_thirdary);
		if (ret_value < 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dns);
	return;
}
void route_ddns_edit_save()
{
	int  	ret_value=0,iserror=0,size;
	char   	enable_ddns[STRING_LENGTH] = {0};
	char   	provider[STRING_LENGTH] = {0};
	char 	username[STRING_LENGTH]={"admin"};
	char 	password[STRING_LENGTH]={"root"};
	char 	dns[STRING_LENGTH]={"admin@f3322.org"};
	struct mgtcgi_xml_ddns *ddns = NULL;

#if CGI_FORM_STRING
	cgiFormString("enable_ddns", enable_ddns, sizeof(enable_ddns));
	cgiFormString("provider", provider, sizeof(provider));
	cgiFormString("username", username, sizeof(username));
	cgiFormString("password", password, sizeof(password));
	cgiFormString("dns", dns, sizeof(dns));
#endif

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DDNS, (void**)&ddns, &size);


	if (NULL == ddns){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	ddns->enable_ddns = (U8)atoi(enable_ddns);
	strncpy(ddns->provider, provider, STRING_LENGTH);
	strncpy(ddns->username, username, STRING_LENGTH);
	strncpy(ddns->password, password, STRING_LENGTH);
	strncpy(ddns->dns, dns, STRING_LENGTH);

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DDNS, (void*)ddns, size);
	
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	    goto ERROR_EXIT;
    }
	ret_value = apply_ddns();
	if (ret_value < 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}


	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&ddns);
	return;
}
void route_iflan_edit_save()
{
	int	iserror=0,index=0,ret_value=0;
    int len = 0,mutilan_num=0,duolan_enable=0;
	const char *dot=":";
    const char *duolan_dot = "<";
    char ip[LITTER_LENGTH]={"2.5.5.5"};
    char mask[LITTER_LENGTH]={0};
	char newmac[LITTER_LENGTH]={0};
    char dhcp_type[LITTER_LENGTH] = {0};
    char startip[LITTER_LENGTH] = {0};
    char endip[LITTER_LENGTH] = {0};
    char lease_time[LITTER_LENGTH] = {0};
    char dhcp_gateway[LITTER_LENGTH] = {0};
    char dhcp_mask[LITTER_LENGTH] = {0};
    char first_dns[LITTER_LENGTH] = {0};
    char second_dns[LITTER_LENGTH] = {0};
    char duolan_enable_str[DIGITAL_LENGTH] = {0};   
	char ifname[LITTER_LENGTH]={"LAN"};
    char mutillan_ip[LITTER_LENGTH] = {0};
    char mutillan_mask[LITTER_LENGTH] = {0};
    char *duolan_info = NULL;
    char *tmp_str = NULL;
    char *p = NULL,*point = NULL;
	struct mgtcgi_xml_route_interface_lans *lan = NULL;

    duolan_info = (char *)malloc(CMD_LENGTH);
    if(!duolan_info){
        iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
    }
    memset(duolan_info,0,CMD_LENGTH);
    tmp_str = (char *)malloc(CMD_LENGTH);
    if(!tmp_str){
        iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
    }
    memset(tmp_str,0,CMD_LENGTH);

#if CGI_FORM_STRING
	cgiFormString("ip", ip, sizeof(ip));
    cgiFormString("mask", mask, sizeof(mask));
    cgiFormString("newmac", newmac, sizeof(newmac));
    cgiFormString("dhcp_type", dhcp_type, sizeof(dhcp_type));
    cgiFormString("startip", startip, sizeof(startip));
    cgiFormString("endip", endip, sizeof(endip));
    cgiFormString("lease_time", lease_time, sizeof(lease_time));
    cgiFormString("dhcp_gateway", dhcp_gateway, sizeof(dhcp_gateway));
    cgiFormString("dhcp_mask", dhcp_mask, sizeof(dhcp_mask));
    cgiFormString("first_dns", first_dns, sizeof(first_dns));
    cgiFormString("second_dns", second_dns, sizeof(second_dns));
    cgiFormString("duolan_enable", duolan_enable_str, sizeof(duolan_enable_str));
    cgiFormString("ifname", ifname, sizeof(ifname));
    cgiFormString("duolan_info", duolan_info, CMD_LENGTH);
#endif
	mac_repair(newmac, strlen(newmac), dot);
    memcpy(tmp_str,duolan_info,strlen(duolan_info));
    
	if((check_cgiform_string(ip) != 0)
		|| (check_cgiform_string(newmac) != 0)
		|| (check_cgiform_string(mask) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	if ((check_ipaddr(ip) != 0) ||
            (check_mac_addr(newmac, dot) != 0) ||
            (check_mask(mask) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}  

    if(strcmp(dhcp_type,"off") != 0){
        iserror = dhcp_ip_pool_check(ip,mask,startip,endip,dhcp_gateway,dhcp_mask);
        if(iserror != 0){
    		iserror = MGTCGI_PARAM_ERR;
    		goto ERROR_EXIT;
    	}
    }
    
    duolan_enable = atoi(duolan_enable_str);
    if(duolan_enable != 0)
        duolan_enable = 1;

    p = duolan_info;
    if((duolan_enable == 1) && (strlen(duolan_info) > 0)){
        while(p){
            while((point = strsep(&p,duolan_dot))){
                sscanf(point,"%[^|]|%[^|]",mutillan_ip,mutillan_mask);
                if ((check_ipaddr(mutillan_ip) != 0) ||
                    (check_mask(mutillan_mask) != 0)){
                    iserror = MGTCGI_PARAM_ERR;
		            goto ERROR_EXIT;
	            }  
                mutilan_num++;
            }
        }
    }

    len = sizeof(struct mgtcgi_xml_route_interface_lans) + 
        sizeof(struct mgtcgi_xml_interface_lan_subnets_info) * mutilan_num;
    lan = (struct mgtcgi_xml_route_interface_lans *)malloc(len);
    if(lan == NULL){
        iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
    }

    lan->sunbet_enable = duolan_enable;
    lan->subnet_num = mutilan_num;
    strncpy(lan->ip, ip, LITTER_LENGTH - 1);	
    strncpy(lan->mask, mask, LITTER_LENGTH -1);
    strncpy(lan->mac, newmac, LITTER_LENGTH - 1);
    strncpy(lan->ifname, ifname, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_type, dhcp_type, LITTER_LENGTH - 1);
    lan->dhcp_info.lease_time = atoi(lease_time);
    strncpy(lan->dhcp_info.startip, startip, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_info.endip, endip, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_info.gateway, dhcp_gateway, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_info.dhcp_mask, dhcp_mask, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_info.first_dns, first_dns, LITTER_LENGTH - 1);
    strncpy(lan->dhcp_info.second_dns, second_dns, LITTER_LENGTH - 1);

    p = tmp_str;
    if((duolan_enable == 1) && (strlen(tmp_str) > 0)){
        while(p){
            while((point = strsep(&p,duolan_dot))){
                sscanf(point,"%[^|]|%[^|]",mutillan_ip,mutillan_mask);
                strncpy(lan->pinfo[index].ip, mutillan_ip, LITTER_LENGTH - 1); 
                strncpy(lan->pinfo[index].mask, mutillan_mask, LITTER_LENGTH - 1);
                index++;
            }
        }
    }
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void*)lan, len);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }

	ret_value = apply_lan();
	if (ret_value < 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}        
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&lan);
    if(duolan_info)
        free(duolan_info);
    if(tmp_str)
        free(tmp_str);
	return;
}
void route_ifwan_edit_save()
{
	int iserror=0,index=0,ret_value=0,sign=0,old_size=0;
    int old_id=2,id=2,diff_name=0,defaultroute=0,enable;

    const char *dot = ":";
    char old_id_str[LITTER_LENGTH]={0};
    char old_ifname[LITTER_LENGTH] = {0};
    char ifname[LITTER_LENGTH]={"WAN0"};
    char access[LITTER_LENGTH]={"static"};
    char type[LITTER_LENGTH]={"0"};
    char username[LITTER_LENGTH]={"111"};
    char passwd[LITTER_LENGTH]={"111"};
    char ipaddr[LITTER_LENGTH]={0};
    char netmask[LITTER_LENGTH]={0};
    char gateway[LITTER_LENGTH]={0};
    char service[LITTER_LENGTH]={0};
    char mtu[LITTER_LENGTH]={0};
    char mac[LITTER_LENGTH]={0};
    char dns_1[LITTER_LENGTH]={0};
    char dns_2[LITTER_LENGTH]={0};
    char dns_3[LITTER_LENGTH]={0};
    char sx[LITTER_LENGTH]={0};
    char xx[LITTER_LENGTH]={0};
    char isp[LITTER_LENGTH]={0};
    char defaultroute_str[LITTER_STRING]={0};
    char enable_str[LITTER_STRING]={0};
    char timed[LITTER_LENGTH]={0};
    char wday[LITTER_LENGTH]={0};
    char min2[LITTER_LENGTH]={0};

    char dns_str[STRING_LENGTH] = {0};
    struct mgtcgi_xml_interface_wans *old_nodes = NULL;

#if CGI_FORM_STRING
    cgiFormString("old_id", old_id_str, sizeof(old_id_str));
    cgiFormString("old_ifname", old_ifname, sizeof(old_ifname));
    cgiFormString("ifname", ifname, sizeof(ifname));
    cgiFormString("access", access, sizeof(access));
    cgiFormString("username", username, sizeof(username));
    cgiFormString("passwd", passwd, sizeof(passwd));
    cgiFormString("ip", ipaddr, sizeof(ipaddr));
    cgiFormString("mask", netmask, sizeof(netmask));
    cgiFormString("gateway", gateway, sizeof(gateway));
    cgiFormString("service", service, sizeof(service));
    cgiFormString("mtu", mtu, sizeof(mtu));
    cgiFormString("mac", mac, sizeof(mac));
    cgiFormString("dns_1", dns_1, sizeof(dns_1));
    cgiFormString("dns_2", dns_2, sizeof(dns_2));
    cgiFormString("dns_3", dns_3, sizeof(dns_3));
    cgiFormString("sx", sx, sizeof(sx));
    cgiFormString("xx", xx, sizeof(xx));
    cgiFormString("isp", isp, sizeof(isp));
    cgiFormString("default", defaultroute_str, sizeof(defaultroute_str));
    cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("timed", timed, sizeof(timed));
    cgiFormString("tm_wday", wday, sizeof(wday));
    cgiFormString("tm_min2", min2, sizeof(min2));
#endif 

    mac_repair(mac, strlen(mac), dot);
    if((check_cgiform_string(ifname) != 0)    
        || (check_cgiform_string(access) != 0)
        || (check_cgiform_string(enable_str) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    old_id = atoi(old_id_str);
#if 0
    if(check_ifwan_used(old_id) != 0){
        iserror = MGTCGI_USED_ERR;
		goto ERROR_EXIT;
    }
#endif
    defaultroute = atoi(defaultroute_str);
    if(!((defaultroute >= 0) && (defaultroute <= 1))){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    enable = atoi(enable_str);
    if(!((enable >= 0) && (enable <= 1))){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    if (strlen(mac) != 17){
        get_link_macaddr(ifname, mac);
    }
    
    iserror = check_mac_addr(mac, dot);
    if (iserror != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strcmp(access,"pppoe") == 0){
        if((strlen(username) == 0) ||
            (strlen(passwd) == 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }
    else if(strcmp(access,"static") == 0){
        if((check_ipaddr(ipaddr) != 0) ||
            (check_ipaddr(gateway) != 0) ||
            (check_mask(netmask) != 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_1) > 0){
        if(check_ipaddr(dns_1) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_2) > 0){
        if(check_ipaddr(dns_2) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dns_3) > 0){
        if(check_ipaddr(dns_3) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    snprintf(dns_str,sizeof(dns_str),"%s-%s-%s",dns_1,dns_2,dns_3);
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&old_nodes, &old_size);
    if (NULL == old_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    //重复性检查
	if (strcmp(old_ifname, ifname) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < old_nodes->num; index++){
			if (0 == strcmp(old_nodes->pinfo[index].ifname, ifname)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

    if(old_nodes->num == 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    else{
        id = old_id;
    }

    if((id == 0) || (id > 31)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(old_nodes->num == 1){
        defaultroute = 1;
    }
    else{
        if(defaultroute == 1){
            for(index = 0; index< old_nodes->num; index++){
                if(old_id != old_nodes->pinfo[index].id){
                    old_nodes->pinfo[index].defaultroute = 0;
                }
            }
        }
    }

    //将数据添加到结构中
    for(index = 0; index < old_nodes->num; index++){
        if((old_id == old_nodes->pinfo[index].id) &&
            (strcmp(old_nodes->pinfo[index].ifname,old_ifname) == 0)){
#if 0
            ret_value = remove_wan(old_nodes->pinfo[index].id,old_nodes->pinfo[index].ifname,old_nodes->pinfo[index].access,
            old_nodes->pinfo[index].ip,old_nodes->pinfo[index].mask,old_nodes->pinfo[index].gateway,
            old_nodes->pinfo[index].username,old_nodes->pinfo[index].passwd,old_nodes->pinfo[index].servername,
            old_nodes->pinfo[index].mtu,old_nodes->pinfo[index].mac,old_nodes->pinfo[index].dns,
            old_nodes->pinfo[index].up,old_nodes->pinfo[index].down,old_nodes->pinfo[index].isp,
            old_nodes->pinfo[index].defaultroute,old_nodes->pinfo[index].time,old_nodes->pinfo[index].week,
            old_nodes->pinfo[index].day);
#endif
        	if (ret_value < 0){
        		iserror = MGTCGI_APPLY_ERR;
        		goto ERROR_EXIT;
        	}
            
            old_nodes->pinfo[index].id = id;
            old_nodes->pinfo[index].defaultroute = defaultroute;
            old_nodes->pinfo[index].enable = enable;
            strncpy(old_nodes->pinfo[index].ifname, ifname, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].access, access, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].type, type, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].ip, ipaddr, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].mask, netmask, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].gateway, gateway, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].username, username, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].passwd, passwd, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].servername, service, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].mtu, mtu, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].mac, mac, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].dns, dns_str, STRING_LENGTH-1);
            strncpy(old_nodes->pinfo[index].up, sx, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].down, xx, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].isp, isp, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].time, timed, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].week, wday, LITTER_LENGTH-1);
            strncpy(old_nodes->pinfo[index].day, min2, LITTER_LENGTH-1);

            sign = index;
        }
    }

    if(sign > (old_nodes->num-1)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)old_nodes, old_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    ret_value = apply_wan("edit",id,ifname,access,ipaddr,netmask,gateway,username,passwd,service,mtu,mac,dns_str,sx,xx,isp,defaultroute,timed,wday,min2);
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }
    
    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
    }

    return;
}

void route_wan_channel_edit_save(void)
{
    int iserror=0,index=0,size=0,ret_value=0;
    int id=2,dir=0,attr=1,limit=1000,weight=100,classid=64;
    
    char id_str[DIGITAL_LENGTH]={"1"};
    char classid_str[DIGITAL_LENGTH]={"1"};
    char dir_str[DIGITAL_LENGTH]={"1"};
    char rate_str[LITTER_STRING]={0};
    char attr_str[DIGITAL_LENGTH]={"1"};
    char limit_str[LITTER_STRING]={"1000"};
    char weight_str[LITTER_STRING]={0};
    
    struct mgtcgi_xml_wans_channels *wan_channel = NULL;
    
#if CGI_FORM_STRING
    cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("classid", classid_str, sizeof(classid_str));
    cgiFormString("dir", dir_str, sizeof(dir_str));
    cgiFormString("rate", rate_str, sizeof(rate_str));
    cgiFormString("attr", attr_str, sizeof(attr_str));
    cgiFormString("limit", limit_str, sizeof(limit_str));
    cgiFormString("weight", weight_str, sizeof(weight_str));
#endif

    if((check_cgiform_string(id_str) != 0)	
        || (check_cgiform_string(classid_str) != 0)
		|| (check_cgiform_string(dir_str) != 0)
		|| (check_cgiform_string(rate_str) != 0)
		|| (check_cgiform_string(attr_str) != 0)
		|| (check_cgiform_string(weight_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    id = atoi(id_str);
    if(!(id > 0 && id < 32)){
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }

    classid = atoi(classid_str);
    dir = atoi(dir_str);
    attr = atoi(attr_str);
    limit = atoi(limit_str);
    weight = atoi(weight_str);
      
    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&wan_channel, &size);
    if (NULL == wan_channel){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    for(index = 0; index < wan_channel->num; index++){
        if((id == wan_channel->pinfo[index].id) &&
            (dir == wan_channel->pinfo[index].direct) &&
            (classid == wan_channel->pinfo[index].classid)){
            iserror = 0;
            wan_channel->pinfo[index].attribute = attr;
            wan_channel->pinfo[index].limit = limit;
            wan_channel->pinfo[index].weight = weight;
            strncpy(wan_channel->pinfo[index].rate, rate_str,LITTER_STRING-1);
            break;
        }
    }

    if (iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void*)wan_channel, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_wanchannel_xmlconf, flash_wanchannel_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&wan_channel);
    return;
}

void route_share_channel_edit_save(void)
{
    int iserror=0,index=0,size=0,ret_value=0,up_ok=-1,down_ok=-1;
    int id=2,classid=64,wan_size=0;
    
    char id_str[DIGITAL_LENGTH]={"1"};
    char classid_str[DIGITAL_LENGTH]={"1"};
    char up_str[LITTER_STRING]={"1000Kbps"};
    char down_str[LITTER_STRING]={"1000Kbps"};
    
    struct mgtcgi_xml_wans_channels *wan_channel = NULL;
    struct mgtcgi_xml_interface_wans *wans_info = NULL;

#if CGI_FORM_STRING
    cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("classid", classid_str, sizeof(classid_str));
    cgiFormString("up", up_str, sizeof(up_str));
    cgiFormString("down", down_str, sizeof(down_str));
#endif

    if((check_cgiform_string(id_str) != 0)  
        || (check_cgiform_string(classid_str) != 0)
        || (check_cgiform_string(up_str) != 0)
        || (check_cgiform_string(down_str) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    id = atoi(id_str);
    if(!(id > 0 && id < 32)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    classid = atoi(classid_str);
    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&wan_channel, &size);
    if (NULL == wan_channel){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &wan_size);
    if (NULL == wans_info){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    for(index = 0; index < wan_channel->num; index++){
        if((id == wan_channel->pinfo[index].id) &&
            (0 == wan_channel->pinfo[index].direct) &&
            (classid == wan_channel->pinfo[index].classid)){
            up_ok = 0;
            strncpy(wan_channel->pinfo[index].rate, up_str,LITTER_STRING-1);
        }
        if((id == wan_channel->pinfo[index].id) &&
            (1 == wan_channel->pinfo[index].direct) &&
            (classid == wan_channel->pinfo[index].classid)){
            down_ok = 0;
            strncpy(wan_channel->pinfo[index].rate, down_str,LITTER_STRING-1);
        }

        if(up_ok == 0 && down_ok == 0)
            break;
    }

    if (up_ok != 0 || down_ok != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    iserror = -1;
    for(index = 0; index < wans_info->num; index++){
        if(id == wans_info->pinfo[index].id){
            iserror = 0;

            strncpy(wans_info->pinfo[index].up, up_str, LITTER_STRING-1);
            strncpy(wans_info->pinfo[index].down, down_str, LITTER_STRING-1);
            break;
        }
    }
    if (iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void*)wan_channel, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wans_info, wan_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_wanchannel_xmlconf, flash_wanchannel_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&wan_channel);
    return;
}

void route_parent_channel_edit_save(void)
{
    printf("{\"iserror\":1,\"msg\":\"\"}");
    return;
}

void route_wan_channel_type_edit_save(void)
{
    int type = 0;
    char type_str[LITTER_STRING]={"0"};

#if CGI_FORM_STRING
    cgiFormString("type", type_str, sizeof(type_str));
#endif

    type = atoi(type_str);

    if(0 == type){
        route_share_channel_edit_save();
    }
    else if(1 == type){
        route_parent_channel_edit_save();
    }
    else if(2 == type){
        route_wan_channel_edit_save();
    }
    else{
        goto PARAM_ERROR;
    }
    goto EXIT;

PARAM_ERROR:
	printf("{\"iserror\":1,\"msg\":\"parameter error\"}");
	goto EXIT;
EXIT:
	return;
}

extern int remove_vlan(const int id);

void route_interfacelink_edit_save()
{
    int index=0,iserror=0,i,j,count=0,new_size=0,retal;
//  char value[BUF_LENGTH_4K]={"eth0,3,0,auto,100,full;eth1,2,0,auto,100,full;"};
    char tmpstr[STRING_LENGTH]={"0"};
    int speed=0,linkmode=0;
    int comma_num=0,semicolon_num=0,mgtnum=0;
    char *value = NULL;
    struct mgtcgi_xml_interface_links *links = NULL;

    value = (char *)malloc(sizeof(char)*BUF_LENGTH_4K);
    if(NULL == value){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto ERROR_EXIT;
    }
    memset(value,0,sizeof(char)*BUF_LENGTH_4K);
    
    cgiFormString("value", value, (sizeof(char)*BUF_LENGTH_4K));

    count = check_interface_list(value);
    if (count < 1){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    new_size = sizeof(struct mgtcgi_xml_interface_links) + 
            count * 
            sizeof(struct mgtcgi_xml_interface_link_info);

    links = (struct mgtcgi_xml_interface_links *)malloc(new_size);
    if (NULL == links){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    memset(links, 0, new_size);
    links->num = count;
    comma_num=0;
    semicolon_num=0;
    memset(tmpstr, 0, sizeof(tmpstr));
    for (index=0,i=0,j=0; i<strlen(value); i++){
        //eth0,1,0,auto,100
        if (value[i] == ','){
            if (comma_num == 0){    //ifname
                strcpy(links->pinfo[index].ifname, tmpstr);
            }
            else if (comma_num == 1){   //linkstate
                linkmode = atoi(tmpstr);
                links->pinfo[index].linkmode = linkmode;
            }
            else if (comma_num == 2){   //mode
                strcpy(links->pinfo[index].mode, tmpstr);   
            }
            else if (comma_num == 3){   //speed
                speed = atoi(tmpstr);
                links->pinfo[index].speed = speed;
            }
            else {
                iserror = MGTCGI_PARAM_ERR;
                goto ERROR_EXIT;
            }
            
            comma_num++;
            j = 0;
            memset(tmpstr, 0, sizeof(tmpstr));
            continue;
        }
        
        if (value[i] == ';'){
            if (comma_num == 4){    //duplex
                strcpy(links->pinfo[index].duplex, tmpstr);
                if(strncmp(links->pinfo[index].ifname,"WAN",3) == 0){
                    strcpy(links->pinfo[index].type,"wan");
                }
                else if(strncmp(links->pinfo[index].ifname,"LAN",3) == 0){
                    strcpy(links->pinfo[index].type,"lan");
                }
                else{
                    strcpy(links->pinfo[index].type,"none");
                }
                index++;
            }
            
            comma_num=0;
            j = 0;
            memset(tmpstr, 0, sizeof(tmpstr));
            continue;
        }
        
        if (j < sizeof(tmpstr))
            tmpstr[j] = value[i];
        j++;
    }

    //参数验证
    mgtnum = 0;
    for (i=0; i < links->num; i++){
        //必须有一个管理口
        if(links->pinfo[i].linkmode == 3){ 
            mgtnum++;
        }
    }
    
    if (mgtnum == 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
	retal = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LINKS, (void*)links, new_size);
	if (retal < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
    }
    
    retal = apply_inteface_link();
    if(retal != 0){
        iserror = MGTCGI_APPLY_ERR;
	    goto ERROR_EXIT;
    }

    retal = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (retal != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(links){
        free_xml_node((void *)&links);
        links = NULL;
    }
    if(value){
        free(value);
        value = NULL;
    }
    return;

}

void route_vlan_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int diff_name=0,id=2,old_id=2,remove_flag=0;
    int vlan_sign = 0;
	
	const char *dot=":";
	char old_name[LITTER_LENGTH]={"vlan1"};
    char old_ifname[LITTER_LENGTH] = {"WAN0"};
	
	char name[LITTER_LENGTH]={"vlan1"};
	char ifname[LITTER_LENGTH]={"WAN0"};
	char mac[LITTER_LENGTH]={"00:0c:29:ae:d6:01"};
	char comment[COMMENT_LENGTH]={0};
	char id_str[DIGITAL_LENGTH]={"2"};
	
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	struct mgtcgi_xml_interface_wans *wans_info = NULL;

#if CGI_FORM_STRING
	cgiFormString("oldname", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("id", id_str, sizeof(id_str));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	mac_repair(mac, strlen(mac), dot);
	if((check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(name) != 0)
		|| (check_cgiform_string(ifname) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if (strlen(mac) != 17){
        get_link_macaddr(ifname, mac);
	}
    id = atoi(id_str);
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
	if (NULL == wans_info){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    //重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < vlan->num; index++){
			if (0 == strcmp(vlan->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

    //检查旧名称是否被使用?
	ret_value = wans_ifname_check(wans_info,old_name);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
    remove_flag = 1;
	
	iserror = -1;
	for (index=0; index < vlan->num; index++){
		if (strcmp(vlan->pinfo[index].name, old_name) == 0){
            old_id = vlan->pinfo[index].id;
            strcpy(old_ifname,vlan->pinfo[index].ifname);

			strncpy(vlan->pinfo[index].name, name, LITTER_LENGTH);
			strncpy(vlan->pinfo[index].ifname, ifname, LITTER_LENGTH);
			strncpy(vlan->pinfo[index].mac, mac, LITTER_LENGTH);
			strncpy(vlan->pinfo[index].comment, comment, COMMENT_LENGTH);
			vlan->pinfo[index].id = id;

            vlan_sign = index + 1;
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void*)vlan, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (remove_flag != 0)
		remove_vlan(vlan_sign);

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }
	
	ret_value = apply_vlan(vlan_sign);
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&vlan);
	free_xml_node((void*)&wans_info);
	return;
}

void route_macvlan_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int remove_flag=0,macvlan_index=0;
	
	const char *dot=":";
	char old_name[LITTER_LENGTH]={"vlan1"};
    char old_ifname[LITTER_LENGTH] = {"WAN0"};
	
	char ifname[LITTER_LENGTH]={"WAN0"};
	char mac[LITTER_LENGTH]={"00:0c:29:ae:d6:01"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_interface_macvlans *macvlan = NULL;
	struct mgtcgi_xml_interface_wans *wans_info = NULL;

#if CGI_FORM_STRING
	cgiFormString("oldname", old_name, sizeof(old_name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	mac_repair(mac, strlen(mac), dot);
	if((check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(ifname) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if (strlen(mac) != 17){
        get_link_macaddr(ifname, mac);
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void**)&macvlan, &size);
	if (NULL == macvlan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
	if (NULL == wans_info){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    //检查旧名称是否被使用?
	ret_value = wans_ifname_check(wans_info,old_name);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
    remove_flag = 1;
	
	iserror = -1;
	for (index=0; index < macvlan->num; index++){
		if (strcmp(macvlan->pinfo[index].name, old_name) == 0){
            strncpy(old_ifname, macvlan->pinfo[index].ifname, LITTER_LENGTH-1);
            
			strncpy(macvlan->pinfo[index].ifname, ifname, LITTER_LENGTH-1);
			strncpy(macvlan->pinfo[index].mac, mac, LITTER_LENGTH-1);
			strncpy(macvlan->pinfo[index].comment, comment, COMMENT_LENGTH-1);

            macvlan_index = index + 1;
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void*)macvlan, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (remove_flag != 0)
		remove_macvlan(macvlan_index);

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	    goto ERROR_EXIT;
    }
	
	ret_value = apply_macvlan(macvlan_index);
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&macvlan);
	free_xml_node((void*)&wans_info);
	return;
}


void route_pppdauth_edit_save()
{
	int iserror=0,size=0,ret_value=0;
	int pap=0,chap=0,mschap=0,mschapv2=0,eap=0;
	char pap_str[DIGITAL_LENGTH]={"1"};
	char chap_str[DIGITAL_LENGTH]={"1"};
	char mschap_str[DIGITAL_LENGTH]={"1"};
	char mschapv2_str[DIGITAL_LENGTH]={"1"};
	char eap_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_pppd_authenticate *pppdauth = NULL;

#if CGI_FORM_STRING
	cgiFormString("pap", pap_str, sizeof(pap_str));
	cgiFormString("chap", chap_str, sizeof(chap_str));
	cgiFormString("mschap", mschap_str, sizeof(mschap_str));
	cgiFormString("mschapv2", mschapv2_str, sizeof(mschapv2_str));
	cgiFormString("eap", eap_str, sizeof(eap_str));
#endif
	
	if ((check_cgiform_string(pap_str) != 0) ||
		(check_cgiform_string(chap_str) != 0) ||
		(check_cgiform_string(mschap_str) != 0) ||
		(check_cgiform_string(mschapv2_str) != 0) ||
		(check_cgiform_string(eap_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	pap = atoi(pap_str);
	chap = atoi(chap_str);
	mschap = atoi(mschap_str);
	mschapv2 = atoi(mschapv2_str);
	eap = atoi(eap_str);
	if (!((0 <= pap && pap <= 1) &&
		(0 <= chap && chap <= 1) &&
		(0 <= mschap && mschap <= 1) &&
		(0 <= mschapv2 && mschapv2 <= 1) && 
		(0 <= eap && eap <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPPD_AUTHENTICATE, (void**)&pppdauth, &size);
	if (NULL == pppdauth){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	pppdauth->pap = pap;
	pppdauth->chap = chap;
	pppdauth->mschap = mschap;
	pppdauth->mschapv2 = mschapv2;
	pppdauth->eap = eap;
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPPD_AUTHENTICATE, (void*)pppdauth, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
    
    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }
	ret_value = apply_pppd_auth(pap, chap, mschap, mschapv2, eap);
	if (ret_value < 0){
		iserror = MGTCGI_APPY_ERR;
		goto ERROR_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&pppdauth);
	return;
}

void route_adslclient_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0,diff_name=0;
	int mtu=0,weight=0,defaultroute=0,peerdns=0,enable=0;

	char ifdev[STRING_LENGTH]={0};
	char olduser[STRING_LENGTH]={0};
	char oldpasswd[STRING_LENGTH]={0};
	char old_name[STRING_LENGTH]={"adsl-1"};
	char name[STRING_LENGTH]={"adsl-1"};
	char ifname[STRING_LENGTH]={"WAN1"};
	char username[STRING_LENGTH]={"test1"};
	char password[STRING_LENGTH]={"123456"};
	char servername[STRING_LENGTH]={0};
	char comment[COMMENT_LENGTH]={0};
	
	char mtu_str[DIGITAL_LENGTH]={"1480"};
	char weight_str[DIGITAL_LENGTH]={"0"};
	char defaultroute_str[DIGITAL_LENGTH]={"0"};
	char peerdns_str[DIGITAL_LENGTH]={"0"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_routings 	   *routing = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;
	
#if CGI_FORM_STRING	
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("username", username, sizeof(username));
	cgiFormString("password", password, sizeof(password));
	cgiFormString("servername", servername, sizeof(servername));
	cgiFormString("comment", comment, sizeof(comment));
	cgiFormString("mtu", mtu_str, sizeof(mtu_str));
	cgiFormString("weight", weight_str, sizeof(weight_str));
	cgiFormString("defaultroute", defaultroute_str, sizeof(defaultroute_str));
	cgiFormString("peerdns", peerdns_str, sizeof(peerdns_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(username) != 0)
		|| (check_cgiform_string(password) != 0)
		|| (check_cgiform_string(mtu_str) != 0)
		|| (check_cgiform_string(weight_str) != 0)
		|| (check_cgiform_string(defaultroute_str) != 0)
		|| (check_cgiform_string(peerdns_str) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_adsl_name(name) != 0){ //名称必须adsl+后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if(check_passwd(password) != 0){  //密码必须是数字和字母组成
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }
	
	mtu = atoi(mtu_str);
	weight = atoi(weight_str);
	defaultroute = atoi(defaultroute_str);
	peerdns = atoi(peerdns_str);
	enable = atoi(enable_str);
	if (!((1200 <= mtu && mtu <= 1500) && 
		  (1 <= weight && weight <= 99) && 
		  (0 <= defaultroute && defaultroute <= 1) && 
		  (0 <= peerdns && peerdns <= 1) &&
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_ppp_ifname(ifname, ifdev) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if( defaultroute == 1)
	{
		iserror = change_defaultroute();
		if(iserror != 0)
		{
			goto ERROR_EXIT;
		}
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//账号密码不能同时相同
	/*for (index=0; index < adsl->num; index++) {
		if ((strcmp(adsl->pinfo[index].name, old_name) != 0) &&
			(strcmp(adsl->pinfo[index].username, username) == 0) &&
			(strcmp(adsl->pinfo[index].password, password) == 0)){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
		}
	}*/
	
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < adsl->num; index++){
			if (0 == strcmp(adsl->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
		if (NULL == routing){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
		if (NULL == loadb){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}

		ret_value = routing_use_name_check(routing, name, old_name, 1);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
		ret_value = loadb_use_name_check(loadb, name, old_name, 1);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	iserror = -1;
	for (index=0; index < adsl->num; index++){
		if (strcmp(adsl->pinfo[index].name, old_name) == 0){		
			strncpy(olduser, adsl->pinfo[index].username, sizeof(olduser));
			strncpy(oldpasswd, adsl->pinfo[index].password, sizeof(oldpasswd));
			
			strncpy(adsl->pinfo[index].name, name, STRING_LENGTH);
			strncpy(adsl->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(adsl->pinfo[index].username, username, STRING_LENGTH);
			strncpy(adsl->pinfo[index].password, password, STRING_LENGTH);
			strncpy(adsl->pinfo[index].servername, servername, STRING_LENGTH);
			strncpy(adsl->pinfo[index].comment, comment, COMMENT_LENGTH);
	
			adsl->pinfo[index].mtu = mtu;
			adsl->pinfo[index].weight = weight;
			adsl->pinfo[index].defaultroute = defaultroute;
			adsl->pinfo[index].peerdns = peerdns;
			adsl->pinfo[index].enable = enable;
	
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)adsl, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
        ret_value = remove_adsl_link_and_conf(old_name, olduser, oldpasswd);
		if (ret_value < 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}
        
		ret_value = apply_adsl(2, olduser, oldpasswd, name, ifdev, username, password, servername, mtu, defaultroute, enable, peerdns);
              if (ret_value != 0){
			iserror = MGTCGI_ADSL_DIAL_ERR;
			//goto ERROR_EXIT;
		}

         //   ret_value = apply_dnat();
         ret_value = 0;
	     if (ret_value != 0){
		    iserror = MGTCGI_APPY_ERR;
		    goto ERROR_EXIT;
	     }

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&routing);
	free_xml_node((void*)&loadb);
	return;
}

#if 0
void route_vpnclient_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0,diff_name=0;
	int mtu=0,weight=0,defaultroute=0,peerdns=0,enable=0;

	char oldvpn[STRING_LENGTH]={0};
	char old_name[STRING_LENGTH]={"vpn1"};
	char name[STRING_LENGTH]={"vpn1"};
	char ifname[STRING_LENGTH]={"WAN1"};
	char username[STRING_LENGTH]={"test1"};
	char password[STRING_LENGTH]={"123456"};
	char servername[STRING_LENGTH]={"10.200.15.15"};
	char comment[COMMENT_LENGTH]={0};
	
	char mtu_str[DIGITAL_LENGTH]={"1460"};
	char weight_str[DIGITAL_LENGTH]={"0"};
	char defaultroute_str[DIGITAL_LENGTH]={"0"};
	char peerdns_str[DIGITAL_LENGTH]={"0"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_routings	   *routing = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;

#if CGI_FORM_STRING	
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("username", username, sizeof(username));
	cgiFormString("password", password, sizeof(password));
	cgiFormString("servername", servername, sizeof(servername));
	cgiFormString("comment", comment, sizeof(comment));
	cgiFormString("mtu", mtu_str, sizeof(mtu_str));
	cgiFormString("weight", weight_str, sizeof(weight_str));
	cgiFormString("defaultroute", defaultroute_str, sizeof(defaultroute_str));
	cgiFormString("peerdns", peerdns_str, sizeof(peerdns_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(username) != 0)
		|| (check_cgiform_string(password) != 0)
		|| (check_cgiform_string(servername) != 0)
		|| (check_cgiform_string(mtu_str) != 0)
		|| (check_cgiform_string(weight_str) != 0)
		|| (check_cgiform_string(defaultroute_str) != 0)
		|| (check_cgiform_string(peerdns_str) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_l2tpvpn_name(name) != 0){ //名称必须l2tpvpn+后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	mtu = atoi(mtu_str);
	weight = atoi(weight_str);
	defaultroute = atoi(defaultroute_str);
	peerdns = atoi(peerdns_str);
	enable = atoi(enable_str);
	if (!((1200 <= mtu && mtu <= 1500) && 
		  (1 <= weight && weight <= 99) && 
		  (0 <= defaultroute && defaultroute <= 1) && 
		  (0 <= peerdns && peerdns <= 1) &&
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if( defaultroute == 1 )
	{
		iserror = change_defaultroute();
		if(iserror != 0)
		{
			goto ERROR_EXIT;
		}
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < l2tpvpn->num; index++){
			if (0 == strcmp(l2tpvpn->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
		if (NULL == routing){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
		if (NULL == loadb){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
			
		ret_value = routing_use_name_check(routing, name, old_name, 1);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
		ret_value = loadb_use_name_check(loadb, name, old_name, 1);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	iserror = -1;
	for (index=0; index < l2tpvpn->num; index++){
		if (strcmp(l2tpvpn->pinfo[index].name, old_name) == 0){	
			strncpy(oldvpn, l2tpvpn->pinfo[index].name, sizeof(oldvpn));
		
			strncpy(l2tpvpn->pinfo[index].name, name, STRING_LENGTH);
			strncpy(l2tpvpn->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(l2tpvpn->pinfo[index].username, username, STRING_LENGTH);
			strncpy(l2tpvpn->pinfo[index].password, password, STRING_LENGTH);
			strncpy(l2tpvpn->pinfo[index].servername, servername, STRING_LENGTH);
			strncpy(l2tpvpn->pinfo[index].comment, comment, COMMENT_LENGTH);
	
			l2tpvpn->pinfo[index].mtu = mtu;
			l2tpvpn->pinfo[index].weight = weight;
			l2tpvpn->pinfo[index].defaultroute = defaultroute;
			l2tpvpn->pinfo[index].peerdns = peerdns;
			l2tpvpn->pinfo[index].enable = enable;
	
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)l2tpvpn, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = apply_l2tpvpn(oldvpn, name, ifname, username, password, servername, mtu, defaultroute, enable, peerdns);
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; //保存成功，应用失败
			//goto ERROR_EXIT;
		}
		xl2tpd_restart();
		ret_value = 0;
		for (index=0; index < l2tpvpn->num; index++){
			if (l2tpvpn->pinfo[index].enable == 0)
				continue;
			if (l2tpvpn_client_dial("up", l2tpvpn->pinfo[index].name) != 0){
				ret_value = -1;
			}
		}
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			//goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&routing);
	free_xml_node((void*)&loadb);
	return;
}
#endif

void route_loadb_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0,diff_name=0;
	int weight=0;
	char old_name[STRING_LENGTH]={"balance-3"};
	char name[STRING_LENGTH]={"balance-3"};
	char ifname[STRING_LENGTH]={"WAN2"};
	char rule[STRING_LENGTH]={""};
	char comment[COMMENT_LENGTH]={""};
	char weight_str[DIGITAL_LENGTH]={"1"};

	struct mgtcgi_xml_load_balancings *loadb = NULL;
	struct mgtcgi_xml_routings	   	  *routing = NULL;

#if CGI_FORM_STRING	
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("rule", rule, sizeof(rule));
	cgiFormString("weight", weight_str, sizeof(weight_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif

	if((check_cgiform_string(name) != 0)	
		|| (check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(weight_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_loadb_name(name) != 0){ //名称必须balance + 后缀组成
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	weight = atoi(weight_str);
	if (1 > weight || weight > 100){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (strlen(rule) < 1)
		strncpy(rule, "null", sizeof(rule));
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < loadb->num; index++){
			if (strcmp(loadb->pinfo[index].name, name) == 0){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	for (index=0; index < loadb->num; index++){
		if ((strcmp(loadb->pinfo[index].name, old_name) != 0) &&
			(strcmp(loadb->pinfo[index].ifname, ifname) == 0)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}
	}
	
	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
		if (NULL == routing){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
	
		ret_value = routing_use_name_check(routing, name, old_name, 1);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	iserror = -1;
	for (index=0; index < loadb->num; index++){
		if (strcmp(loadb->pinfo[index].name, old_name) == 0){
			strncpy(loadb->pinfo[index].name, name, STRING_LENGTH);
			strncpy(loadb->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(loadb->pinfo[index].rulename, rule, STRING_LENGTH);
			strncpy(loadb->pinfo[index].comment, comment, COMMENT_LENGTH);
			loadb->pinfo[index].weight = weight;
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void*)loadb, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&loadb);
	free_xml_node((void*)&routing);
	return;
}


int change_default_gateway(char * def_ifname, char * def_gateway)
{
#if 0
	int ret_value=0,iftype=0,index=0,size=0,iserror=0;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	if (strlen(def_ifname) < 1){
		ret_value = MGTCGI_PARAM_ERR;
		goto FREE_EXIT;
	}
	//return: -1 error; 1 LAN; 2 WAN; 3 adsl; 4 l2tpvpn; 5 balance;
	iftype = check_interface_type(def_ifname);
	
	if (1 >= iftype || iftype > 6){
		ret_value = -1;
		goto FREE_EXIT;
	}

	ret_value = set_default_route(def_ifname, def_gateway);
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR;
		goto FREE_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		ret_value = -1;
		goto FREE_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		ret_value = -1;
		goto FREE_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		ret_value = -1;
		goto FREE_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
	if (NULL == vlan){
		ret_value = -1;
		goto FREE_EXIT;
	}


	for (index=0; index<wan->num; index++){
		if ((iftype == 2) && (strcmp(wan->pinfo[index].ifname, def_ifname) == 0 )){
			wan->pinfo[index].defaultroute = 1;	
		}
		else{
			wan->pinfo[index].defaultroute = 0;
		}
	}
	
	for (index=0; index<adsl->num; index++){
		if ((iftype == 3) && (strcmp(adsl->pinfo[index].name, def_ifname) == 0 )){
			adsl->pinfo[index].defaultroute = 1;	
		}
		else{
			adsl->pinfo[index].defaultroute = 0;
		}
	}
	
	for (index=0; index<l2tpvpn->num; index++){
		if ((iftype == 4) && (strcmp(l2tpvpn->pinfo[index].name, def_ifname) == 0 )){
			l2tpvpn->pinfo[index].defaultroute = 1;	
		}
		else{
			l2tpvpn->pinfo[index].defaultroute = 0;
		}
	}

	for (index=0; index<vlan->num; index++){
		if ((iftype == 6) && (strcmp(vlan->pinfo[index].name, def_ifname) == 0 )){
			vlan->pinfo[index].defaultroute = 1;	
		}
		else{
			vlan->pinfo[index].defaultroute = 0;
		}
	}
	

	iserror = 0;
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wan, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto FREE_EXIT;
	}
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)adsl, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto FREE_EXIT;
	}
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)l2tpvpn, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto FREE_EXIT;
	}

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto FREE_EXIT;
	}
	
FREE_EXIT:
	ret_value = iserror;
	free_xml_node((void*)&wan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&vlan);
	return ret_value;
#endif
return 0;
}

void route_routing_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int enable=0,priority=0,id=0;
	int iftype=0,i=0;
	char buf[STRING_LENGTH]={0};
	
	char id_str[DIGITAL_LENGTH]={"-1"};
	char dst[STRING_LENGTH]={""};
	char gateway[STRING_LENGTH]={0};
	char ifname[STRING_LENGTH]={"adsl-1"};
	char priority_str[STRING_LENGTH]={"1"};
	char table[STRING_LENGTH]={"null"};
	char rule[STRING_LENGTH]={"null"};
	char enable_str[STRING_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_routings *routing = NULL;

#if CGI_FORM_STRING	
	cgiFormString("id", id_str, sizeof(id_str));
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("priority", priority_str, sizeof(priority_str));
	cgiFormString("table", table, sizeof(table));
	cgiFormString("rule", rule, sizeof(rule));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	memset(buf, 0, sizeof(buf));
	for (i=0; i < strlen(ifname); i++){
		if (i >= 3){
			buf[i] = '\0';
			break;
		}
		else{
			buf[i] = ifname[i];
		}
	}
	if (strcmp(buf, "bal") == 0){
		memset(table, 0, sizeof(table)); 
		memset(rule, 0, sizeof(table));
		memset(dst, 0, sizeof(dst));
		strncpy(table, "null", sizeof(table));
		strncpy(rule, "null", sizeof(rule));
	}

	if((check_cgiform_string(ifname) != 0) ||
		(check_cgiform_string(id_str) != 0) ||
		(check_cgiform_string(table) != 0)||
		(check_cgiform_string(rule) != 0)||
		(check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	priority = atoi(priority_str);
	enable = atoi(enable_str);
	id = atoi(id_str);
	if (id == -1){ //修改默认路由
		if (strcmp(ifname, "all") == 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		ret_value = change_default_gateway(ifname, gateway);

		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}
		else {
			goto SUCCESS_EXIT;
		}
	}
	
	if (!((1 <= priority && priority <= 250) && 
		  (0 <= id && id <= 65535) && 
		  (0 <= enable && enable <= 1))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	
	if (strcmp(ifname, "all") == 0){
		if (check_ipaddr(gateway) != 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		} 
	}
	//dst，table, rule至少填写一个。
	iftype = check_interface_type(ifname);
	if (iftype != 5){
		if (!((check_cgiform_string(dst) == 0) ||
			(strcmp(table, "null") != 0) ||
			(strcmp(rule, "null") != 0))){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	//dst不为空，检查dst合法性
	if ((check_cgiform_string(dst) == 0) && (check_ipaddr(dst) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	iserror = -1;
	for (index=0; index < routing->num; index++){
		if (routing->pinfo[index].id == id){
			strncpy(routing->pinfo[index].dst, dst, STRING_LENGTH);
			strncpy(routing->pinfo[index].gateway, gateway, STRING_LENGTH);
			strncpy(routing->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(routing->pinfo[index].table, table, STRING_LENGTH);
			strncpy(routing->pinfo[index].rule, rule, STRING_LENGTH);
			strncpy(routing->pinfo[index].comment, comment, COMMENT_LENGTH);
			routing->pinfo[index].priority = priority;
			routing->pinfo[index].enable = enable;
	
			iserror = 0;
			break;
		}
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void*)routing, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = update_list_routing();
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; //成功，更新列表失败
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
SUCCESS_EXIT:
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&routing);
		return;
}
extern int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
							const char *name, const char *old_name, const char *value, const char *comment);

void route_rtable_edit_save()
{
	int iserror=0,index=0,ret_value=0,size=0;
	int diff_name=0,edit_includ_num=0;
	char *dot=",";

	char buffer[STRING_LENGTH] = {0};
	
	char old_name[STRING_LENGTH]={"CNC2"};
	char name[STRING_LENGTH]={"CNC2"};
	//char value[STRING_LIST_LENGTH]={"1.1.1.1"};
	char * value = NULL;
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_routings	  *routing = NULL;
	struct mgtcgi_xml_group_array *table = NULL;

    value = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!value)
        goto ERROR_EXIT;

#if CGI_FORM_STRING	
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("value", value, STRING_LIST_LENGTH * sizeof(char));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	
	if((check_cgiform_string(name) != 0) 
		|| (check_cgiform_string(old_name) != 0)
		|| (check_cgiform_string(value) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (((strcmp(old_name, "TEL") == 0) ||
		(strcmp(old_name, "CNC") == 0))){
		if (strcmp(comment, "build") == 0) {
			get_system_date(buffer, sizeof(buffer));
			memset(name, 0, sizeof(name));
			strcpy(name, old_name);
			memset(comment, 0, sizeof(comment));
			snprintf(comment, sizeof(comment), "Build - %s", buffer);	
		}
		else {
			iserror = MGTCGI_EDIT_DEFAULT_ERR;
			goto ERROR_EXIT;
		}
	}

	edit_includ_num = check_table_dstip_list(value, dot);
	if (edit_includ_num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if ( NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < table->num; index++){
			if (0 == strcmp(table->pinfo[index].group_name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
		if (NULL == routing){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		ret_value = routing_use_name_check(routing, name, old_name, 2);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	ret_value = groups_edit_save(MGTCGI_TYPE_ROUTE_TABLES,
					edit_includ_num, diff_name, name, old_name, value, comment);

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&table);
	free_xml_node((void*)&routing);
    if(value)
        free(value);
	return;
}
void route_rrule_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int diff_name=0;

	char old_name[STRING_LENGTH]={"web"};
	char name[STRING_LENGTH]={"web"};
	char src[STRING_LENGTH]={0};
	char dst[STRING_LENGTH]={0};
	char protocol[STRING_LENGTH]={0};
    char src_port[STRING_LENGTH]={0};
	char dst_port[STRING_LENGTH]={0};
	char tos[STRING_LENGTH]={"8"};
    char action[STRING_LENGTH] = {0};
	char comment[COMMENT_LENGTH]={0};
	struct mgtcgi_xml_route_rules *rule = NULL;
	struct mgtcgi_xml_routings    *routing = NULL;

#if CGI_FORM_STRING	
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("src", src, sizeof(src));
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("protocol", protocol, sizeof(protocol));
    cgiFormString("src_port", src_port, sizeof(src_port));
	cgiFormString("dst_port", dst_port, sizeof(dst_port));
	cgiFormString("tos", tos, sizeof(tos));
    cgiFormString("action", action, sizeof(action));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	
	if(((check_cgiform_string(name) != 0) ||
		(check_cgiform_string(old_name) != 0))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    if(check_cgiform_string(name) != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
	
	//src,dst,protocol,tos至少输入一个。
/*	if(!((check_cgiform_string(src) == 0) 
		|| (check_cgiform_string(dst) == 0)
		|| (strcmp(protocol, "null") != 0)
		|| (strcmp(tos, "null") != 0))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if ((strcmp(protocol, "null") != 0) && 
		(check_cgiform_string(dst_port) != 0) && (check_cgiform_string(src_port) != 0)){		//协议不为null，端口必须输入
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	*/
	if ((check_cgiform_string(src) == 0) && (check_ipaddr(src) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if ((check_cgiform_string(dst) == 0) && (check_ipaddr(dst) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	if (NULL == rule){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < rule->num; index++){
			if (0 == strcmp(rule->pinfo[index].name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
		if (NULL == routing){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
	
		ret_value = routing_use_name_check(routing, name, old_name, 3);
		if (ret_value != 0){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			} 
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	iserror = -1;
	for (index=0; index < rule->num; index++){
		if (strcmp(rule->pinfo[index].name, old_name) == 0){
            route_rule_remove(1,
                rule->pinfo[index].src,
                rule->pinfo[index].src_port, 
                rule->pinfo[index].dst,
                rule->pinfo[index].dst_port,
                rule->pinfo[index].tos,
                rule->pinfo[index].protocol,
                rule->pinfo[index].action);
            
			strncpy(rule->pinfo[index].name, name, STRING_LENGTH);
			strncpy(rule->pinfo[index].src, src, STRING_LENGTH);
			strncpy(rule->pinfo[index].dst, dst, STRING_LENGTH);
			strncpy(rule->pinfo[index].protocol, protocol, STRING_LENGTH);
            strncpy(rule->pinfo[index].src_port, src_port, STRING_LENGTH);
			strncpy(rule->pinfo[index].dst_port, dst_port, STRING_LENGTH);
			strncpy(rule->pinfo[index].tos, tos, STRING_LENGTH);
            strncpy(rule->pinfo[index].action, action, STRING_LENGTH);
			strncpy(rule->pinfo[index].comment, comment, COMMENT_LENGTH);
            
			iserror = 0;
			break;
		}
	}
		
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void*)rule, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
        ret_value = route_rule_apply(1,src,src_port,dst,dst_port,tos,protocol,action);
        
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
		printf("{\"iserror\":0,\"msg\":\"\"}");
		goto FREE_EXIT;
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&rule);
		free_xml_node((void*)&routing);
		return;
}
void route_dnat_edit_save()
{
	int iserror=0,ret_value=0;
	int size=0,index=0,old_id=0;
	int enable=0,loop_enable=0;
    int w_before,w_after,l_before,l_after;

    char old_id_str[STRING_LENGTH]={"0"};
    char enable_str[DIGITAL_LENGTH]={"0"};
    char comment[STRING_LENGTH]={0};
    char protocol[LITTER_STRING]={"TCP"};
    char src[STRING_LENGTH]={""};
    char wanport_before[DIGITAL_LENGTH]={""};
    char wanport_after[DIGITAL_LENGTH]={""};
    char lanport_before[DIGITAL_LENGTH]={""};
    char lanport_aftere[DIGITAL_LENGTH]={""};
    char dst[STRING_LENGTH]={""};
	char ifname[STRING_LENGTH]={"1"};
    char loop_enable_str[DIGITAL_LENGTH] = {"0"};
	
	struct mgtcgi_xml_nat_dnats *dnat = NULL;

#if CGI_FORM_STRING
    cgiFormString("old_id", old_id_str, sizeof(old_id_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("comment", comment, sizeof(comment));
    cgiFormString("protocol", protocol, sizeof(protocol));
    cgiFormString("src", src, sizeof(src));
    cgiFormString("wanport_before", wanport_before, sizeof(wanport_before));
    cgiFormString("wanport_after", wanport_after, sizeof(wanport_after));
    cgiFormString("lanport_before", lanport_before, sizeof(lanport_before));
    cgiFormString("lanport_aftere", lanport_aftere, sizeof(lanport_aftere));
    cgiFormString("dst", dst, sizeof(dst));
    cgiFormString("ifname", ifname, sizeof(ifname));
    cgiFormString("loop_enable", loop_enable_str, sizeof(loop_enable_str));
#endif
    if((check_cgiform_string(ifname) != 0)  
        || (check_cgiform_string(protocol) != 0)
        || (check_cgiform_string(enable_str) != 0)
        || (check_cgiform_string(wanport_before) != 0)
        || (check_cgiform_string(dst) != 0)
        || (check_cgiform_string(comment) != 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
    }
        
    w_before = atoi(wanport_before);
    if (!(1 <= w_before && w_before <= 65535)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    //端口检查
    if(check_cgiform_string(wanport_after) == 0){
        w_after = atoi(wanport_after);
        if (!(1 <= w_after && w_after <= 65535)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
        if(w_before >= w_after){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }

        if((check_cgiform_string(lanport_before) != 0)
            || (check_cgiform_string(lanport_aftere) != 0)){
            strncpy(lanport_before,wanport_before,DIGITAL_LENGTH-1);
            strncpy(lanport_aftere,wanport_after,DIGITAL_LENGTH-1);
        }
        else{
            l_before = atoi(lanport_before);
            if (!(1 <= l_before && l_before <= 65535)){
                iserror = MGTCGI_PARAM_ERR;
                goto ERROR_EXIT;
            }
            l_after = atoi(lanport_aftere);
            if (!(1 <= l_after && l_after <= 65535)){
                iserror = MGTCGI_PARAM_ERR;
                goto ERROR_EXIT;
            }
            if(l_before >= l_after){
                iserror = MGTCGI_PARAM_ERR;
                goto ERROR_EXIT;
            }
        } 
    }
    else{
        memset(lanport_aftere,0,sizeof(lanport_aftere));
        if(check_cgiform_string(lanport_before) != 0){
            strncpy(lanport_before,wanport_before,DIGITAL_LENGTH-1);
        }
        l_before = atoi(lanport_before);
        if (!(1 <= l_before && l_before <= 65535)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }
	
	if(check_cgiform_string(src) == 0){
		if (check_ipaddr(src) != 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
    
	if (check_ipaddr(dst) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    loop_enable = atoi(loop_enable_str);
	if (!(0 <= loop_enable && loop_enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    old_id = atoi(old_id_str);
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&dnat, &size);
	if (NULL == dnat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    //名称检查
    for(index = 0; index < dnat->num; index++){
        if(old_id == dnat->pinfo[index].id)
            continue;
        if(strcmp(dnat->pinfo[index].comment,comment) == 0){
            iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT; 
        }
    }

	iserror = -1;
	for (index=0; index < dnat->num; index++){
		if (dnat->pinfo[index].id == old_id){
            dnat->pinfo[index].enable = enable;
            dnat->pinfo[index].loop_enable = loop_enable;
            strncpy(dnat->pinfo[index].protocol, protocol, LITTER_STRING-1);
            strncpy(dnat->pinfo[index].comment, comment, STRING_LENGTH-1);
            strncpy(dnat->pinfo[index].wan_ip, src, STRING_LENGTH-1);
            strncpy(dnat->pinfo[index].wan_port_before, wanport_before, DIGITAL_LENGTH-1);
            strncpy(dnat->pinfo[index].wan_port_after, wanport_after, DIGITAL_LENGTH-1);
            strncpy(dnat->pinfo[index].lan_port_before, lanport_before, DIGITAL_LENGTH-1);
            strncpy(dnat->pinfo[index].lan_port_after, lanport_aftere, DIGITAL_LENGTH-1);
            strncpy(dnat->pinfo[index].lan_ip, dst, STRING_LENGTH-1);
            strncpy(dnat->pinfo[index].ifname, ifname, STRING_LENGTH-1);

            iserror = 0;
			break;
		}
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void*)dnat, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&dnat);

	return;
}

void route_snat_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int enable=0,id=0;
	
	char id_str[DIGITAL_LENGTH]={"0"};
	char ifname[STRING_LENGTH]={"all"};
	char protocol[STRING_LENGTH]={"all"};
	char src[STRING_LENGTH]={0};
	char dst[STRING_LENGTH]={0};
	char action[STRING_LENGTH]={"MASQUERADE"};
	char dstip[STRING_LENGTH]={0};
	char dstport[STRING_LENGTH]={0};
	char enable_str[STRING_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_nat_snats *snat = NULL;

#if CGI_FORM_STRING	
	cgiFormString("id", id_str, sizeof(id_str));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("protocol", protocol, sizeof(protocol));
	cgiFormString("src", src, sizeof(src));
	cgiFormString("dst", dst, sizeof(dst));
	cgiFormString("action", action, sizeof(action));
	cgiFormString("dstip", dstip, sizeof(dstip));
	cgiFormString("dstport", dstport, sizeof(dstport));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
		
	if((check_cgiform_string(ifname) != 0)	
		|| (check_cgiform_string(id_str) != 0)
		|| (check_cgiform_string(protocol) != 0)
		|| (check_cgiform_string(action) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//action选择为SNAT，dstip不能为空。
	if ((strcmp(action, "SNAT") == 0) &&
		(check_cgiform_string(dstip) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//协议选择为tcp或udp，action为SNAT, dstport不能为空
	if ((strcmp(protocol, "all") != 0) && 
		(strcmp(action, "SNAT") == 0) &&
		(check_cgiform_string(dstport) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	 
	enable = atoi(enable_str);
	id = atoi(id_str);
	if (!((0 <= enable && enable <= 1) &&
		(0 <= id && id <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	//action为MASQUERADE， dstip，dstport无效
	if (strcmp(action, "MASQUERADE") == 0){
		dstip[0] = 0;
		dstport[0] = 0;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&snat, &size);
	if (NULL == snat){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	
	iserror = -1;
	for (index=0; index < snat->num; index++){
		if (snat->pinfo[index].id == id){
			strncpy(snat->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(snat->pinfo[index].src, src, STRING_LENGTH);
			strncpy(snat->pinfo[index].dst, dst, STRING_LENGTH);
			strncpy(snat->pinfo[index].protocol, protocol, STRING_LENGTH);
			strncpy(snat->pinfo[index].action, action, STRING_LENGTH);
			strncpy(snat->pinfo[index].dstip, dstip, STRING_LENGTH);
			strncpy(snat->pinfo[index].dstport, dstport, STRING_LENGTH);
			strncpy(snat->pinfo[index].comment, comment, COMMENT_LENGTH);
			snat->pinfo[index].enable = enable;
			
			iserror = 0;
			break;
		}
	}
		
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void*)snat, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	    goto ERROR_EXIT;
    }

/*
	ret_value = apply_snat();
	if (ret_value != 0){
		iserror = MGTCGI_APPY_ERR;
		goto ERROR_EXIT;
	}
*/
	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(snat){
	    free_xml_node((void*)&snat);
        snat = NULL;
    }
	return;
}

void route_macbind_edit_save(const char *strdata)
{
	int index=0,iserror=0,ret_value=0,i=0,j=0,count=0,type=0,total=0,learnt=0,bridge=0,new_num=0,size=0,new_size=0;
	int comma_num=0,semicolon_num=0,value_size=0;
	char *value = NULL;
	char learnt_str[STRING_LENGTH]={"0"};
	char total_str[STRING_LENGTH]={"2"};
	char tmpstr[STRING_LENGTH]={"0"};
   char type_str[STRING_LENGTH]={"0"};
   char ip[STRING_LENGTH]={"0"};
   char mac[STRING_LENGTH]={"0"};
   char comment[STRING_LENGTH]={"0"};
   char action[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_mac_binds *new_nodes = NULL;
	struct mgtcgi_xml_mac_binds *old_nodes = NULL;

	value_size = 5 * STRING_LIST_LENGTH * sizeof(char);
	value = (char *)malloc(value_size);//500k,大约5000条
	if (NULL == value){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    memset(value,0,value_size);

#if CGI_FORM_STRING	
	cgiFormString("value", value, value_size);
	cgiFormString("learnt", learnt_str, sizeof(learnt_str));
	cgiFormString("total", total_str, sizeof(total_str));
       cgiFormString("type", type_str, sizeof(type_str));
       cgiFormString("ip", ip, sizeof(ip));
       cgiFormString("mac", mac, sizeof(mac));
       cgiFormString("action", action, sizeof(action));
       cgiFormString("comment", comment, sizeof(comment));
#endif
	learnt=atoi(learnt_str);
	total=atoi(total_str);
       type=atoi(type_str);
    
#if 1	
	if (strdata != NULL){
		strcpy(value, strdata);
		get_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void**)&old_nodes, &size);
		if (NULL == old_nodes){
			goto FREE_EXIT;
		}
		learnt = old_nodes->learnt;
	}
#endif
if(type != 2){
	count = check_mac_bind_list(value);
	if (count < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
}
#if 1
	if ((strdata == NULL) && (count != total)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
#endif

      if(type == 2){
            get_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void**)&old_nodes, &size);
		if (NULL == old_nodes){
			goto FREE_EXIT;
		}
        
            for(index = 0;index < old_nodes->num;index++){
                if((strcmp(old_nodes->pinfo[index].ip,ip) == 0) && (strcmp(old_nodes->pinfo[index].mac,mac) == 0)){
                    old_nodes->pinfo[index].action = atoi(action);
                     break;
                }
            }

             ret_value = save_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void*)old_nodes, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
       }else{
	//将数据添加到结构中
	new_num = count; 
	new_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				new_num * 
				sizeof(struct mgtcgi_xml_mac_binds_info);

	new_nodes = (struct mgtcgi_xml_mac_binds *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	new_nodes->learnt = learnt;
	new_nodes->num = new_num;

	memset(tmpstr, 0, sizeof(tmpstr));
	for (index=0,i=0,j=0; i<strlen(value)  ; i++){
		//192.168.0.10,00-23-1D-2B-60-4F,comment,1;
		if (value[i] == ','){
			if (comma_num == 0){	//ip
				strcpy(new_nodes->pinfo[index].ip, tmpstr);
			}
			else if (comma_num == 1){	//mac
				strcpy(new_nodes->pinfo[index].mac, tmpstr);
			}
			else if(comma_num == 2){ //comment
				strcpy(new_nodes->pinfo[index].comment,tmpstr);	
			}
			else {
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num++;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (value[i] == ';'){
			if (comma_num == 3){	//comment
				new_nodes->pinfo[index].action = atoi(tmpstr);
				new_nodes->pinfo[index].id = index;
				new_nodes->pinfo[index].bridge = bridge;
				index++;
			}
				
			semicolon_num++;
			if (semicolon_num > new_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num=0;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (j < sizeof(tmpstr))
			tmpstr[j] = value[i];
		j++;
	}

	new_size = 0;
	ret_value = save_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
}

      
#if 0        
	if (strdata != NULL){
		snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --set --action accept;/usr/local/sbin/mac-bind -f");
		do_system_command(cmd);
	}
#endif

if(type == 2){
    ret_value = change_macbind(ip,mac,atoi(action));
}else{
    ret_value = apply_macbind();
}
	ret_value = copy_mac_bind_xmlconfig_file(working_mac_bind_xmlconf, flash_mac_bind_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}	
	
#if 0
	if (strdata != NULL)
		goto FREE_EXIT;
#endif
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
       if(new_nodes){
	       free_xml_node((void*)&new_nodes);
           new_nodes = NULL;
       }
	if(value){
	    free(value);
           value = NULL;
	}
	return;
}

void route_dhcpd_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0,diff_name=0;
	unsigned int lease_time,max_lease_time;
	int enable=1;

	char old_name[STRING_LENGTH]={"dhcp2"};
	char name[STRING_LENGTH]={"dhcp2"};
	char ifname[STRING_LENGTH]={"LAN"};
	char range[STRING_LENGTH]={"192.168.0.100-192.168.0.110"};
	char mask[STRING_LENGTH]={"255.255.255.0"};
	char gateway[STRING_LENGTH]={"192.168.0.1"};
	char dnsname[STRING_LENGTH]={""};
	char dns[STRING_LENGTH]={"8.8.8.8"};
	char lease_time_str[DIGITAL_LENGTH]={"86400"};
	char max_lease_time_str[DIGITAL_LENGTH]={"86400"};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;

#if CGI_FORM_STRING
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("ifname", ifname, sizeof(ifname));
	cgiFormString("range", range, sizeof(range));
	cgiFormString("mask", mask, sizeof(mask));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("dnsname", dnsname, sizeof(dnsname));
	cgiFormString("dns", dns, sizeof(dns));
	cgiFormString("lease_time", lease_time_str, sizeof(lease_time_str));
	cgiFormString("max_lease_time", max_lease_time_str, sizeof(max_lease_time_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	if((check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(name) != 0)
		|| (check_cgiform_string(ifname) != 0)
		|| (check_cgiform_string(range) != 0)
		|| (check_cgiform_string(mask) != 0)
		|| (check_cgiform_string(gateway) != 0)
		|| (check_cgiform_string(dns) != 0)
		|| (check_cgiform_string(lease_time_str) != 0)
		|| (check_cgiform_string(max_lease_time_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

//	iserror = dhcpd_ip_pool_check(ifname, range, mask, gateway);
    iserror = dhcpd_ip_pool_legal_check(ifname, range, mask, gateway);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iserror = check_table_dstip_list(dns, NULL);
	if (iserror < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	lease_time = strtoul(lease_time_str, NULL, 10);
	if (!((unsigned int)3600 <= lease_time && lease_time <= (unsigned int)2592000)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	max_lease_time = strtoul(max_lease_time_str, NULL, 10);
	if (!((unsigned int)3600 <= max_lease_time && max_lease_time <= (unsigned int)2592000)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < dhcpd->num; index++){
			if (strcmp(dhcpd->pinfo[index].name, name) == 0){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	if (diff_name == 1){ //名称被修改，检查旧名称是否被使用?
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
		if (NULL == host){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		for (index=0; index<host->num; index++){
			if(strcmp(host->pinfo[index].dhcpname, old_name) == 0){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	iserror = -1;
	for (index=0; index < dhcpd->num; index++){
		if (strcmp(dhcpd->pinfo[index].name, old_name) == 0){
			strncpy(dhcpd->pinfo[index].name, name, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].ifname, ifname, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].range, range, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].mask, mask, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].gateway, gateway, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].dnsname, dnsname, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].dns, dns, STRING_LENGTH);
			strncpy(dhcpd->pinfo[index].comment, comment, COMMENT_LENGTH);
			dhcpd->pinfo[index].lease_time = lease_time;
			dhcpd->pinfo[index].max_lease_time = max_lease_time;
			dhcpd->pinfo[index].enable = enable;
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void*)dhcpd, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
		ret_value = apply_dnsmasq_dhcpd();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(dhcpd){
	    free_xml_node((void*)&dhcpd);
        dhcpd = NULL;
    }
    if(host){
	    free_xml_node((void*)&host);
        host = NULL;
    }
	return;
}

void route_pptpvpn_edit_save(void)
{
    int iserror=0,size,ret_value,enable;
    char localip[STRING_LENGTH] = {0};
    char startip[STRING_LENGTH] = {0};
    char endip[STRING_LENGTH] = {0};
    char dns[STRING_LENGTH] = {0};
    char entry_mode[STRING_LENGTH] = {0};
    struct mgtcgi_xml_pptpd_server *pptpdvpn = NULL;

#if CGI_FORM_STRING
	cgiFormString("localip", localip, sizeof(localip));
    cgiFormString("startip", startip, sizeof(startip));
    cgiFormString("endip", endip, sizeof(endip));
    cgiFormString("dns", dns, sizeof(dns));
    cgiFormString("entry_mode", entry_mode, sizeof(entry_mode));
#endif

    if((check_cgiform_string(localip) != 0)    
        || (check_cgiform_string(startip) != 0)
        || (check_cgiform_string(endip) != 0)
        || (check_cgiform_string(entry_mode) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if((check_ipaddr(localip) != 0)
        || (check_ipaddr(startip) != 0)
        || (check_ipaddr(endip) != 0)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    iserror = pptpd_server_ip_pool_check(startip,endip);
    if(iserror != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_SERVERS, (void**)&pptpdvpn, &size);
    if(pptpdvpn == NULL){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }
    enable = pptpdvpn->enable;

    strncpy(pptpdvpn->localip, localip, STRING_LENGTH);
    strncpy(pptpdvpn->startip, startip, STRING_LENGTH);
    strncpy(pptpdvpn->endip, endip, STRING_LENGTH);
    strncpy(pptpdvpn->dns, dns, STRING_LENGTH);
    strncpy(pptpdvpn->entry_mode, entry_mode, STRING_LENGTH);

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_SERVERS, (void*)pptpdvpn, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
    else{
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
		ret_value = apply_pptpd_vpn(enable,localip,startip,endip,dns,entry_mode);
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}
    }
    
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(pptpdvpn){
        free_xml_node((void*)&pptpdvpn);
        pptpdvpn = NULL;
    }
    return;
}

void route_dhcpd_host_edit_save()
{
	int iserror=0,index=0,size=0,ret_value=0,diff_name=0;
	unsigned int lease_time,max_lease_time;
	int enable;
	const char *dot=":";

	char old_name[STRING_LENGTH]={"003"};
	char name[STRING_LENGTH]={"003"};
	char dhcpname[STRING_LENGTH]={"dhcp2"};
	char mac[STRING_LENGTH]={"00:11:22:33:44:66"};
	char ip[STRING_LENGTH]={"192.168.168.0.10"};
	char mask[STRING_LENGTH]={"255.255.255.0"};
	char gateway[STRING_LENGTH]={"192.168.0.1"};
	char dnsname[STRING_LENGTH]={""};
	char dns[STRING_LENGTH]={"8.8.8.8"};
	char lease_time_str[DIGITAL_LENGTH]={""};
	char max_lease_time_str[DIGITAL_LENGTH]={""};
	char enable_str[DIGITAL_LENGTH]={"1"};
	char comment[COMMENT_LENGTH]={0};
	
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;

#if CGI_FORM_STRING
	cgiFormString("old_name", old_name, sizeof(old_name));
	cgiFormString("name", name, sizeof(name));
	cgiFormString("dhcpname", dhcpname, sizeof(dhcpname));
	cgiFormString("mac", mac, sizeof(mac));
	cgiFormString("ip", ip, sizeof(ip));
	cgiFormString("mask", mask, sizeof(mask));
	cgiFormString("gateway", gateway, sizeof(gateway));
	cgiFormString("dnsname", dnsname, sizeof(dnsname));
	cgiFormString("dns", dns, sizeof(dns));
	cgiFormString("lease_time", lease_time_str, sizeof(lease_time_str));
	cgiFormString("max_lease_time", max_lease_time_str, sizeof(max_lease_time_str));
	cgiFormString("enable", enable_str, sizeof(enable_str));
	cgiFormString("comment", comment, sizeof(comment));
#endif
	if((check_cgiform_string(old_name) != 0)	
		|| (check_cgiform_string(name) != 0)
		|| (check_cgiform_string(dhcpname) != 0)
		|| (check_cgiform_string(mac) != 0)
		|| (check_cgiform_string(ip) != 0)
		|| (check_cgiform_string(enable_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	iserror = dhcpd_host_ip_pool_check(dhcpname, ip, mask, gateway);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	mac_repair(mac, strlen(mac), dot);
	iserror = check_mac_addr(mac, dot);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (check_cgiform_string(dns) == 0){
		iserror = check_table_dstip_list(dns, NULL);
		if (iserror < 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <= 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	lease_time = strtoul(lease_time_str, NULL, 10);
	if (lease_time != (unsigned int)0)
		if (!((unsigned int)3600 <= lease_time && lease_time <= (unsigned int)2592000)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	max_lease_time = strtoul(max_lease_time_str, NULL, 10);
	if (max_lease_time != (unsigned int)0)
		if (!((unsigned int)3600 <= max_lease_time && max_lease_time <= (unsigned int)2592000)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
	if (NULL == host){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//重复性检查
	if (strcmp(name, old_name) == 0){
		diff_name = 0;	//名称没有被修改
	} else { //名称修改了，检测是否重复
		diff_name = 1;
		for (index=0; index < host->num; index++){
			if (strcmp(host->pinfo[index].name, name) == 0){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	iserror = -1;
	for (index=0; index < host->num; index++){
		if (strcmp(host->pinfo[index].name, old_name) == 0){
			strncpy(host->pinfo[index].name, name, STRING_LENGTH);
			strncpy(host->pinfo[index].dhcpname, dhcpname, STRING_LENGTH);
			strncpy(host->pinfo[index].mac, mac, STRING_LENGTH);
			strncpy(host->pinfo[index].ip, ip, STRING_LENGTH);
			strncpy(host->pinfo[index].mask, mask, STRING_LENGTH);
			strncpy(host->pinfo[index].gateway, gateway, STRING_LENGTH);
			strncpy(host->pinfo[index].dnsname, dnsname, STRING_LENGTH);
			strncpy(host->pinfo[index].dns, dns, STRING_LENGTH);
			strncpy(host->pinfo[index].comment, comment, COMMENT_LENGTH);
			host->pinfo[index].lease_time = lease_time;
			host->pinfo[index].max_lease_time = max_lease_time;
			host->pinfo[index].enable = enable;
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void*)host, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
		/*ret_value = apply_dhcpd();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}*/
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(host){
        free_xml_node((void*)&host);
        host = NULL;
    }
	return;
}



/******************************** config remove save *****************************/
void route_dns_remove_save()
{
	return;//无
}
void route_iflan_remove_save()
{
	return;//
}
void route_ifwan_remove_save()
{
 	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char id_str[STRING_LENGTH]={"0"};
	char ifname[STRING_LENGTH]={0};
	int id=0,sign=0;
	struct mgtcgi_xml_interface_wans *old_nodes = NULL;
	struct mgtcgi_xml_interface_wans *new_nodes = NULL;

#if CGI_FORM_STRING	
	cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("ifname", ifname, sizeof(ifname));
#endif
	if((check_cgiform_string(ifname) != 0)){
		goto ERROR_EXIT;
	}

    id = atoi(id_str);
    if((id < 1) || (id > 31)){
        iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
    }
    
#if 1
    if(check_ifwan_used(id) != 0){
        iserror = MGTCGI_USED_ERR;
		goto ERROR_EXIT;
    }
#endif
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_interface_wans) + 
				old_num * 
				sizeof(struct mgtcgi_xml_interface_wans_info);
	new_size = sizeof(struct mgtcgi_xml_interface_wans) + 
				new_num * 
				sizeof(struct mgtcgi_xml_interface_wans_info);
	
	new_nodes = (struct mgtcgi_xml_interface_wans *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if ((strcmp(old_nodes->pinfo[index].ifname, ifname) == 0) &&
            (old_nodes->pinfo[index].id == id)){
            sign = index;
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
            new_nodes->pinfo[new_index].id = old_nodes->pinfo[index].id;
            new_nodes->pinfo[new_index].defaultroute = old_nodes->pinfo[index].defaultroute;
            new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].access, old_nodes->pinfo[index].access, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].type, old_nodes->pinfo[index].type, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].ip, old_nodes->pinfo[index].ip, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].mask, old_nodes->pinfo[index].mask, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].gateway, old_nodes->pinfo[index].gateway, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].username, old_nodes->pinfo[index].username, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].passwd, old_nodes->pinfo[index].passwd, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].servername, old_nodes->pinfo[index].servername, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].mtu, old_nodes->pinfo[index].mtu, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[index].mac, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].dns, old_nodes->pinfo[index].dns, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].up, old_nodes->pinfo[index].up, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].down, old_nodes->pinfo[index].down, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].isp, old_nodes->pinfo[index].isp, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].time, old_nodes->pinfo[index].time, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].week, old_nodes->pinfo[index].week, LITTER_LENGTH-1);
			strncpy(new_nodes->pinfo[new_index].day, old_nodes->pinfo[index].day, LITTER_LENGTH-1);
		}
	
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    ret_value = apply_wan_channel("remove",id, NULL, NULL);
    if(ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }
    
	ret_value = remove_wan(old_nodes->pinfo[sign].id,old_nodes->pinfo[sign].ifname,old_nodes->pinfo[sign].access,
        old_nodes->pinfo[sign].ip,old_nodes->pinfo[sign].mask,old_nodes->pinfo[sign].gateway,
        old_nodes->pinfo[sign].username,old_nodes->pinfo[sign].passwd,old_nodes->pinfo[sign].servername,
        old_nodes->pinfo[sign].mtu,old_nodes->pinfo[sign].mac,old_nodes->pinfo[sign].dns,
        old_nodes->pinfo[sign].up,old_nodes->pinfo[sign].down,old_nodes->pinfo[sign].isp,
        old_nodes->pinfo[sign].defaultroute,old_nodes->pinfo[sign].time,old_nodes->pinfo[sign].week,
        old_nodes->pinfo[sign].day);
    
	if (ret_value < 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
	    goto ERROR_EXIT;
    }

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

    ret_value = copy_route_xmlconf_file(working_wanchannel_xmlconf, flash_wanchannel_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    
	return;
}

void route_vlan_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"vlan1"};
	char ifname[STRING_LENGTH]={0};
	int id=0,vlan_sign = 0;
	struct mgtcgi_xml_interface_vlans *old_nodes = NULL;
	struct mgtcgi_xml_interface_vlans *new_nodes = NULL;
	struct mgtcgi_xml_interface_wans *wans_info = NULL;

#if CGI_FORM_STRING	
	cgiFormString("name", name, sizeof(name));
#endif
	if((check_cgiform_string(name) != 0)){
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
	if (NULL == wans_info){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	ret_value = wans_ifname_check(wans_info, name);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_interface_vlans) + 
				old_num * 
				sizeof(struct mgtcgi_xml_interface_vlans_info);
	new_size = sizeof(struct mgtcgi_xml_interface_vlans) + 
				new_num * 
				sizeof(struct mgtcgi_xml_interface_vlans_info);
	
	new_nodes = (struct mgtcgi_xml_interface_vlans *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			id = old_nodes->pinfo[index].id;
			strncpy(ifname, old_nodes->pinfo[index].ifname, sizeof(ifname));

            vlan_sign = index + 1;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[index].mac, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			
			new_nodes->pinfo[new_index].id = old_nodes->pinfo[index].id;
		}
	
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
        
		ret_value = remove_vlan( vlan_sign);
		if (ret_value < 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}

        if(uptate_sys_route_config_sh() != 0){
            iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
        }

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
   //     save_route_vlan_web(new_nodes,1);
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(wans_info){
	    free_xml_node((void*)&wans_info);
        wans_info = NULL;
    }

	return;
}

void route_macvlan_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0,macvlan_index=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"vlan1"};
	char ifname[STRING_LENGTH]={0};
	struct mgtcgi_xml_interface_macvlans *old_nodes = NULL;
	struct mgtcgi_xml_interface_macvlans *new_nodes = NULL;
	struct mgtcgi_xml_interface_wans *wans_info = NULL;

#if CGI_FORM_STRING	
	cgiFormString("name", name, sizeof(name));
#endif
	if((check_cgiform_string(name) != 0)){
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
	if (NULL == wans_info){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	ret_value = wans_ifname_check(wans_info, name);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_interface_macvlans) + 
				old_num * 
				sizeof(struct mgtcgi_xml_interface_macvlans_info);
	new_size = sizeof(struct mgtcgi_xml_interface_macvlans) + 
				new_num * 
				sizeof(struct mgtcgi_xml_interface_macvlans_info);
	
	new_nodes = (struct mgtcgi_xml_interface_macvlans *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			strncpy(ifname, old_nodes->pinfo[index].ifname, sizeof(ifname));
            macvlan_index = index + 1;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[index].mac, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);			
		}
	
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_MACVLANS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	ret_value = remove_macvlan(macvlan_index);
	if (ret_value < 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
	    goto ERROR_EXIT;
    }

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
 //   save_route_macvlan_web(new_nodes,1);
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(wans_info){
	    free_xml_node((void*)&wans_info);
        wans_info = NULL;
    }

	return;
}



void route_pppdauth_remove_save()
{
	return;//
}
void route_adslclient_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"adsl2"};
	char username[STRING_LENGTH]={0};
	char password[STRING_LENGTH]={0};
	struct mgtcgi_xml_adsl_clients *old_nodes = NULL;
	struct mgtcgi_xml_adsl_clients *new_nodes = NULL;
	struct mgtcgi_xml_routings	   *routing = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;

#if CGI_FORM_STRING	
	cgiFormString("name", name, sizeof(name));
#endif
	
	if((check_cgiform_string(name) != 0)){
		goto ERROR_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	
	//删除前，检查名称是否被使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = routing_use_name_check(routing, name, name, 1);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	ret_value = loadb_use_name_check(loadb, name, name, 1);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}


	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_adsl_clients) + 
				old_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
	new_size = sizeof(struct mgtcgi_xml_adsl_clients) + 
				new_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
	
	new_nodes = (struct mgtcgi_xml_adsl_clients *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			strncpy(username, old_nodes->pinfo[index].username, sizeof(username));
			strncpy(password, old_nodes->pinfo[index].password, sizeof(password));
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].username, old_nodes->pinfo[index].username, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].password, old_nodes->pinfo[index].password, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].servername, old_nodes->pinfo[index].servername, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			
			new_nodes->pinfo[new_index].mtu = old_nodes->pinfo[index].mtu;
			new_nodes->pinfo[new_index].weight = old_nodes->pinfo[index].weight;
			new_nodes->pinfo[new_index].defaultroute = old_nodes->pinfo[index].defaultroute;
			new_nodes->pinfo[new_index].peerdns = old_nodes->pinfo[index].peerdns;
			new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
		}
	
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = remove_adsl_link_and_conf(name, username, password);
		if (ret_value < 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}

             // ret_value = apply_dnat();
         ret_value = 0;
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}


		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(routing){
	    free_xml_node((void*)&routing);
        routing = NULL;
    }
    if(loadb){
	    free_xml_node((void*)&loadb);
        loadb = NULL;
    }
	return;
}

#if 0
void route_vpnclient_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"vpn2"};
	struct mgtcgi_xml_l2tpvpn_clients *old_nodes = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *new_nodes = NULL;
	struct mgtcgi_xml_routings	   *routing = NULL;
	struct mgtcgi_xml_load_balancings *loadb = NULL;

#if CGI_FORM_STRING	
	cgiFormString("name", name, sizeof(name));
#endif
	
	if((check_cgiform_string(name) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//删除前，检查名称是否被使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&loadb, &size);
	if (NULL == loadb){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = routing_use_name_check(routing, name, name, 1);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	ret_value = loadb_use_name_check(loadb, name, name, 1);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_l2tpvpn_clients) + 
				old_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
	new_size = sizeof(struct mgtcgi_xml_l2tpvpn_clients) + 
				new_num * 
				sizeof(struct mgtcgi_xml_adsl_clients_info);
	
	new_nodes = (struct mgtcgi_xml_l2tpvpn_clients *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].username, old_nodes->pinfo[index].username, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].password, old_nodes->pinfo[index].password, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].servername, old_nodes->pinfo[index].servername, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			
			new_nodes->pinfo[new_index].mtu = old_nodes->pinfo[index].mtu;
			new_nodes->pinfo[new_index].weight = old_nodes->pinfo[index].weight;
			new_nodes->pinfo[new_index].defaultroute = old_nodes->pinfo[index].defaultroute;
			new_nodes->pinfo[new_index].peerdns = old_nodes->pinfo[index].peerdns;
			new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = remove_l2tpvpn_conf(name);
		if (ret_value < 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}
		ret_value = l2tpvpn_client_dial("down", name);
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(routing){
	    free_xml_node((void*)&routing);
        routing = NULL;
    }
    if(loadb){
	    free_xml_node((void*)&loadb);
        loadb=NULL;
    }
	return;
}
#endif

void route_loadb_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"balance-3"};
	struct mgtcgi_xml_load_balancings *old_nodes = NULL;
	struct mgtcgi_xml_load_balancings *new_nodes = NULL;
	struct mgtcgi_xml_routings	  	  *routing = NULL;

#if CGI_FORM_STRING	
	cgiFormString("name", name, sizeof(name));
#endif
	
	if((check_cgiform_string(name) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//删除前，检查名称是否被使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = routing_use_name_check(routing, name, name, 1);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_load_balancings) + 
				old_num * 
				sizeof(struct mgtcgi_xml_load_balancings_info);
	new_size = sizeof(struct mgtcgi_xml_load_balancings) + 
				new_num * 
				sizeof(struct mgtcgi_xml_load_balancings_info);
	
	new_nodes = (struct mgtcgi_xml_load_balancings *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].rulename, old_nodes->pinfo[index].rulename, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			new_nodes->pinfo[new_index].weight = old_nodes->pinfo[index].weight;
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(routing){
	    free_xml_node((void*)&routing);
        routing = NULL;
    }
	return;
}




void route_routing_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0,id=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char id_str[STRING_LENGTH]={"1"};
	struct mgtcgi_xml_routings *old_nodes = NULL;
	struct mgtcgi_xml_routings *new_nodes = NULL;

#if CGI_FORM_STRING	
	cgiFormString("id", id_str, sizeof(id_str));
#endif
	
	if(check_cgiform_string(id_str) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (!(0 <= id && id <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_routings) + 
				old_num * 
				sizeof(struct mgtcgi_xml_routings_info);
	new_size = sizeof(struct mgtcgi_xml_routings) + 
				new_num * 
				sizeof(struct mgtcgi_xml_routings_info);
	
	new_nodes = (struct mgtcgi_xml_routings *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].id == id){
			iserror = 0;
			continue;
		}

		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].gateway, old_nodes->pinfo[index].gateway, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].table, old_nodes->pinfo[index].table, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].rule, old_nodes->pinfo[index].rule, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			
			new_nodes->pinfo[new_index].priority = old_nodes->pinfo[index].priority;
			new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
			new_nodes->pinfo[new_index].id = new_index; //id与new_index相同
		}

		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
 	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = update_list_routing();
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; //成功，更新列表失败
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

extern int groups_remove_save(int mgtcgi_group_type, const char *name);

void route_rtable_remove_save()
{
	int iserror=0,size=0,ret_value=0;
	char name[STRING_LENGTH]={"CNC3"};
	struct mgtcgi_xml_group_array *table = NULL;
	struct mgtcgi_xml_routings	   *routing = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
#endif
	
	if((check_cgiform_string(name) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (((strcmp(name, "TEL") == 0) ||
		(strcmp(name, "CNC") == 0))){
		iserror = MGTCGI_EDIT_DEFAULT_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&table, &size);
	if (NULL == table){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//删除前，检查名称是否被使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	ret_value = routing_use_name_check(routing, name, name, 2);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	groups_remove_save(MGTCGI_TYPE_ROUTE_TABLES, name);

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(table){
	    free_xml_node((void*)&table);
        table = NULL;
    }
    if(routing){
	    free_xml_node((void*)&routing);
        routing = NULL;
    }
	return;
}
void route_rrule_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"aa"};
	struct mgtcgi_xml_route_rules *old_nodes = NULL;
	struct mgtcgi_xml_route_rules *new_nodes = NULL;
	struct mgtcgi_xml_routings	   *routing = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
#endif
	
	if((check_cgiform_string(name) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//删除前，检查名称是否被使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	if (NULL == routing){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	ret_value = routing_use_name_check(routing, name, name, 3);
	if (ret_value != 0){
		if (1 == ret_value){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		} 
		else{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_route_rules) + 
				old_num * 
				sizeof(struct mgtcgi_xml_route_rules_info);
	new_size = sizeof(struct mgtcgi_xml_route_rules) + 
				new_num * 
				sizeof(struct mgtcgi_xml_route_rules_info);
	
	new_nodes = (struct mgtcgi_xml_route_rules *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
            route_rule_remove(1,
                old_nodes->pinfo[index].src,
                old_nodes->pinfo[index].src_port, 
                old_nodes->pinfo[index].dst,
                old_nodes->pinfo[index].dst_port,
                old_nodes->pinfo[index].tos,
                old_nodes->pinfo[index].protocol,
                old_nodes->pinfo[index].action);
			iserror = 0;
			continue;
		}
		
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].protocol, old_nodes->pinfo[index].protocol, STRING_LENGTH);
            strncpy(new_nodes->pinfo[new_index].src_port, old_nodes->pinfo[index].src_port, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dst_port, old_nodes->pinfo[index].dst_port, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].tos, old_nodes->pinfo[index].tos, STRING_LENGTH);
            strncpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[index].action, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(routing){
	    free_xml_node((void*)&routing);
        routing = NULL;
    }
	return;
}

void route_dnat_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0,id=0,start_id=1;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char id_str[STRING_LENGTH]={"0"};
    
	struct mgtcgi_xml_nat_dnats *old_nodes = NULL;
	struct mgtcgi_xml_nat_dnats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("id", id_str, sizeof(id_str));
#endif 
	
	if(check_cgiform_string(id_str) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (!(0 <= id && id <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
    if(old_num == 0){
        new_num = 0;
    }
    else{
	    new_num = old_num - 1;	
    }
	
	old_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
				old_num * 
				sizeof(struct mgtcgi_xml_nat_dnats_info);
	new_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
				new_num * 
				sizeof(struct mgtcgi_xml_nat_dnats_info);
	
	new_nodes = (struct mgtcgi_xml_nat_dnats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].id == id){
			iserror = 0;
			continue;
		}

		if (new_index < new_nodes->num){
            new_nodes->pinfo[new_index].id = start_id;
            new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
            strncpy(new_nodes->pinfo[new_index].protocol, old_nodes->pinfo[index].protocol, LITTER_STRING-1);
            strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].wan_ip, old_nodes->pinfo[index].wan_ip, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].wan_port_before, old_nodes->pinfo[index].wan_port_before, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].wan_port_after, old_nodes->pinfo[index].wan_port_after, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].lan_port_before, old_nodes->pinfo[index].lan_port_before, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].lan_port_after, old_nodes->pinfo[index].lan_port_after, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].lan_ip, old_nodes->pinfo[index].lan_ip, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH-1);
		}
		new_index++;
        start_id++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

void route_snat_remove_save()
{
	int  iserror=0,index=0,size=0,ret_value=0,id=0;
	int  old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char id_str[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_nat_snats *old_nodes = NULL;
	struct mgtcgi_xml_nat_snats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("id", id_str, sizeof(id_str));
#endif

	if(check_cgiform_string(id_str) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	id = atoi(id_str);
	if (!(0 <= id && id<= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_nat_snats) + 
				old_num * 
				sizeof(struct mgtcgi_xml_nat_snats_info);
	new_size = sizeof(struct mgtcgi_xml_nat_snats) + 
				new_num * 
				sizeof(struct mgtcgi_xml_nat_snats_info);
	
	new_nodes = (struct mgtcgi_xml_nat_snats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	
	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].id == id){
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].protocol, old_nodes->pinfo[index].protocol, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[index].action, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dstip, old_nodes->pinfo[index].dstip, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dstport, old_nodes->pinfo[index].dstport, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
		
			new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
			new_nodes->pinfo[new_index].id = new_index; //id与new_index相同
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
    }
/*
	ret_value = apply_snat();
	if (ret_value != 0){
		iserror = MGTCGI_APPY_ERR;
		goto ERROR_EXIT;
	}
*/
	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

void route_dhcpd_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"dhcp1"};

	struct mgtcgi_xml_dhcpd_servers *new_nodes = NULL;
	struct mgtcgi_xml_dhcpd_servers *old_nodes = NULL;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
#endif
	if(check_cgiform_string(name) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    
	iserror = -1;
	for(index=0; index < old_nodes->num; index++){
		if(strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//检查是否被其他对象使用?
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
	if (NULL == host){
		//iserror = MGTCGI_READ_FILE_ERR;
		goto DELETE_SERVER;
	}
	for (index=0; index<host->num; index++){
		if(strcmp(host->pinfo[index].dhcpname, name) == 0){
			iserror = MGTCGI_USED_ERR;
			goto ERROR_EXIT;
		}
	}
    
DELETE_SERVER:
	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_dhcpd_servers) + 
				old_num * 
				sizeof(struct mgtcgi_xml_dhcpd_servers_info);
	new_size = sizeof(struct mgtcgi_xml_dhcpd_servers) + 
				new_num * 
				sizeof(struct mgtcgi_xml_dhcpd_servers_info);
	
	new_nodes = (struct mgtcgi_xml_dhcpd_servers *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;
	new_nodes->dhcpd_enable = old_nodes->dhcpd_enable;

	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[index].ifname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].range, old_nodes->pinfo[index].range, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].mask, old_nodes->pinfo[index].mask, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].gateway, old_nodes->pinfo[index].gateway, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dnsname, old_nodes->pinfo[index].dnsname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dns, old_nodes->pinfo[index].dns, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			new_nodes->pinfo[new_index].lease_time =  old_nodes->pinfo[index].lease_time;
			new_nodes->pinfo[new_index].max_lease_time =  old_nodes->pinfo[index].max_lease_time;
			new_nodes->pinfo[new_index].enable =  old_nodes->pinfo[index].enable;
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	ret_value = apply_dnsmasq_dhcpd();
#if 0    
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR; 
		goto ERROR_EXIT;
	}
#endif
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(host){
	    free_xml_node((void*)&host);
        host = NULL;
    }
	return;
}

void route_dhcpd_host_remove_save()
{
	int iserror=0,index=0,size=0,ret_value=0;
	int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
	char name[STRING_LENGTH]={"001"};

	struct mgtcgi_xml_dhcpd_hosts *new_nodes = NULL;
	struct mgtcgi_xml_dhcpd_hosts *old_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("name", name, sizeof(name));
#endif
	if(check_cgiform_string(name) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&old_nodes, &size);
	if (NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	iserror = -1;
	for(index=0; index < old_nodes->num; index++){
		if(strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			break;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	old_num = old_nodes->num;
	new_num = old_num - 1;	
	
	old_size = sizeof(struct mgtcgi_xml_dhcpd_hosts) + 
				old_num * 
				sizeof(struct mgtcgi_xml_dhcpd_hosts_info);
	new_size = sizeof(struct mgtcgi_xml_dhcpd_hosts) + 
				new_num * 
				sizeof(struct mgtcgi_xml_dhcpd_hosts_info);
	
	new_nodes = (struct mgtcgi_xml_dhcpd_hosts *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	new_nodes->num = new_num;

	iserror = -1;
	for (new_index=0,index=0; index < old_nodes->num; index++){
		if (strcmp(old_nodes->pinfo[index].name, name) == 0){
			iserror = 0;
			continue;
		}
		if (new_index < new_nodes->num){
			strncpy(new_nodes->pinfo[new_index].name, old_nodes->pinfo[index].name, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dhcpname, old_nodes->pinfo[index].dhcpname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[index].mac, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].ip, old_nodes->pinfo[index].ip, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].mask, old_nodes->pinfo[index].mask, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].gateway, old_nodes->pinfo[index].gateway, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dnsname, old_nodes->pinfo[index].dnsname, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].dns, old_nodes->pinfo[index].dns, STRING_LENGTH);
			strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH);
			new_nodes->pinfo[new_index].lease_time =  old_nodes->pinfo[index].lease_time;
			new_nodes->pinfo[new_index].max_lease_time =  old_nodes->pinfo[index].max_lease_time;
			new_nodes->pinfo[new_index].enable =  old_nodes->pinfo[index].enable;
		}
		new_index++;
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
	if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 

	ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	if (ret_value != 0){
		iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		goto ERROR_EXIT;
	}

	ret_value = apply_dhcpd();
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR; 
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

/******************************** config move save *****************************/
void copy_routings_value(struct mgtcgi_xml_routings * new_nodes,
						struct mgtcgi_xml_routings * old_nodes, 
						int new_index, int old_index, int id)
{

	if (id	< 0)
		new_nodes->pinfo[new_index].id = old_nodes->pinfo[old_index].id;
	else
		new_nodes->pinfo[new_index].id = id;//重新修改编号

	strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[old_index].dst, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].gateway, old_nodes->pinfo[old_index].gateway, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[old_index].ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].table, old_nodes->pinfo[old_index].table, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].rule, old_nodes->pinfo[old_index].rule, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment, COMMENT_LENGTH);
	new_nodes->pinfo[new_index].priority = old_nodes->pinfo[old_index].priority;
	new_nodes->pinfo[new_index].enable = old_nodes->pinfo[old_index].enable;
}

#if 0
void copy_dnats_value(struct mgtcgi_xml_nat_dnats * new_nodes,
						struct mgtcgi_xml_nat_dnats * old_nodes, 
						int new_index, int old_index, int id)
{
	if (id	< 0)
		new_nodes->pinfo[new_index].id = old_nodes->pinfo[old_index].id;
	else
		new_nodes->pinfo[new_index].id = id;//重新修改编号

	strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[old_index].ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].protocol, old_nodes->pinfo[old_index].protocol, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].wan_ip, old_nodes->pinfo[old_index].wan_ip, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].wan_port, old_nodes->pinfo[old_index].wan_port, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].lan_ip, old_nodes->pinfo[old_index].lan_ip, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].lan_port, old_nodes->pinfo[old_index].lan_port, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment, COMMENT_LENGTH);
	new_nodes->pinfo[new_index].enable = old_nodes->pinfo[old_index].enable;

}
#endif

#if 0

void copy_snats_value(struct mgtcgi_xml_nat_snats * new_nodes,
						struct mgtcgi_xml_nat_snats * old_nodes, 
						int new_index, int old_index, int id)
{

	if (id	< 0)
		new_nodes->pinfo[new_index].id = old_nodes->pinfo[old_index].id;
	else
		new_nodes->pinfo[new_index].id = id;//重新修改编号

	strncpy(new_nodes->pinfo[new_index].ifname, old_nodes->pinfo[old_index].ifname, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[old_index].src, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[old_index].dst, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].protocol, old_nodes->pinfo[old_index].protocol, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[old_index].action, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].dstip, old_nodes->pinfo[old_index].dstip, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].dstport, old_nodes->pinfo[old_index].dstport, STRING_LENGTH);
	strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment, COMMENT_LENGTH);
	new_nodes->pinfo[new_index].enable = old_nodes->pinfo[old_index].enable;

}
#endif



void route_dns_move_save()
{
	return;//
}
void route_iflan_move_save()
{
	return;//
}
void route_ifwan_move_save()
{
	return;//
}
void route_pppdauth_move_save()
{
	return;//
}
void route_adslclient_move_save()
{
	return;//
}
void route_vpnclient_move_save()
{
	return;//
}
void route_routing_move_save()
{
	int  iserror=0,index=0,new_index=0,ret_value=0;
	int  old_size=0,new_size=0,old_num=0,new_num=0,id=0;
	int  new_id=0,old_id=0,new_move_index=0,old_move_index=0,max_index=0;
	char new_id_str[STRING_LENGTH]={"0"};
	char old_id_str[STRING_LENGTH]={"1"};
	struct mgtcgi_xml_routings *old_nodes = NULL;
	struct mgtcgi_xml_routings *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("new_id", new_id_str, sizeof(new_id_str));
	cgiFormString("old_id", old_id_str, sizeof(old_id_str));
#endif
	
	if((check_cgiform_string(new_id_str) != 0)||
		(check_cgiform_string(old_id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	old_id = atoi(old_id_str);
	new_id = atoi(new_id_str);
	if (!((0 <= old_id && old_id <= 65535) &&
		 (0 <= new_id && new_id <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	

	if (old_id == new_id){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&old_nodes, &old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	old_move_index = -1;
	new_move_index = -1;
	max_index = 0;
	for (index=0; index < old_nodes->num; index++){

		if (old_nodes->pinfo[index].id == old_id)
			old_move_index = index;//old_id的位置

		if (old_nodes->pinfo[index].id == new_id)
			new_move_index = index;//new_id的位置

		max_index = index;
	
	}
	if (old_move_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_move_index == -1){//没有找到要移动的条目的位置
		if (new_id > old_id)
			new_move_index = max_index;
		else
			new_move_index = 0;
	}
	if (new_move_index == old_move_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	new_size = sizeof(struct mgtcgi_xml_routings) + 
		new_num * 
			sizeof(struct mgtcgi_xml_firewall_info);

	new_nodes = (struct mgtcgi_xml_routings *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}

	memset(new_nodes, 0, new_size);

	//拷贝 数据
	new_nodes->num = new_num;
	iserror = -2;
	id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){

		if (index == old_move_index){ 
			iserror ++;
			new_index--; //跳过old_id,new_index值不变；跳过的部分在new_id位置插入
		
		}
		else if (index == new_move_index){ //插入old_id，并拷贝当前数据
			iserror ++;
			if (old_move_index < new_move_index){//往下移动
			
				copy_routings_value( new_nodes, old_nodes, new_index, index, id);
				
				id++;
				new_index++;
				
				copy_routings_value( new_nodes, old_nodes, new_index, old_move_index, id);

			}
			else {//往上移动
			
				copy_routings_value( new_nodes, old_nodes, new_index, old_move_index, id);

				id++;
				new_index++;
				
				copy_routings_value( new_nodes, old_nodes, new_index, index, id);
			}
			
			id++;

		}
		else {
			copy_routings_value( new_nodes, old_nodes, new_index, index, id);
			id++;
		}	
	}

	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS,(void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = update_list_routing();//成功，更新列表失败
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR; 
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

void route_rtable_move_save()
{
	return;//
}
void route_rrule_move_save()
{
	return;//
}
void route_dnat_move_save()
{
#if 0
	int  iserror=0,index=0,new_index=0,ret_value=0;
	int  old_size=0,new_size=0,old_num=0,new_num=0,id=0;
	int  new_id=0,old_id=0,new_move_index=0,old_move_index=0,max_index=0;
	char new_id_str[STRING_LENGTH]={"0"};
	char old_id_str[STRING_LENGTH]={"1"};
	struct mgtcgi_xml_nat_dnats *old_nodes = NULL;
	struct mgtcgi_xml_nat_dnats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("new_id", new_id_str, sizeof(new_id_str));
	cgiFormString("old_id", old_id_str, sizeof(old_id_str));
#endif
	
	if((check_cgiform_string(new_id_str) != 0)||
		(check_cgiform_string(old_id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	old_id = atoi(old_id_str);
	new_id = atoi(new_id_str);
	if (!((0 <= old_id && old_id <= 65535) &&
		 (0 <= new_id && old_id <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	if (old_id == new_id){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&old_nodes, &old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	old_move_index = -1;
	new_move_index = -1;
	max_index = 0;
	for (index=0; index < old_nodes->num; index++){
	
		if (old_nodes->pinfo[index].id == old_id)
			old_move_index = index;//old_id的位置
	
		if (old_nodes->pinfo[index].id == new_id)
			new_move_index = index;//new_id的位置
	
		max_index = index;
	
	}
	if (old_move_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_move_index == -1){//没有找到要移动的条目的位置
		if (new_id > old_id)
			new_move_index = max_index;
		else
			new_move_index = 0;
	}
	if (new_move_index == old_move_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}
	
	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	new_size = sizeof(struct mgtcgi_xml_nat_dnats) + 
		new_num * 
			sizeof(struct mgtcgi_xml_firewall_info);
	
	new_nodes = (struct mgtcgi_xml_nat_dnats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	
	//拷贝 数据
	new_nodes->num = new_num;
	iserror = -2;
	id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){
	
		if (index == old_move_index){ 
			iserror ++;
			new_index--; //跳过old_id,new_index值不变；跳过的部分在new_id位置插入
		
		}
		else if (index == new_move_index){ //插入old_id，并拷贝当前数据
			iserror ++;
			if (old_move_index < new_move_index){//往下移动
			
				copy_dnats_value( new_nodes, old_nodes, new_index, index, id);
				
				id++;
				new_index++;
				
				copy_dnats_value( new_nodes, old_nodes, new_index, old_move_index, id);
	
			}
			else {//往上移动
			
				copy_dnats_value( new_nodes, old_nodes, new_index, old_move_index, id);
	
				id++;
				new_index++;
				
				copy_dnats_value( new_nodes, old_nodes, new_index, index, id);
			}
			
			id++;
	
		}
		else {
			copy_dnats_value( new_nodes, old_nodes, new_index, index, id);
			id++;
		}	
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS,(void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
#if 0
		ret_value = apply_dnat();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}
#endif
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
#endif
	return;
}

void route_snat_move_save()
{
#if 0
	int  iserror=0,index=0,new_index=0,ret_value=0;
	int  old_size=0,new_size=0,old_num=0,new_num=0,id=0;
	int  new_id=0,old_id=0,new_move_index=0,old_move_index=0,max_index=0;
	char new_id_str[STRING_LENGTH]={"0"};
	char old_id_str[STRING_LENGTH]={"1"};
	struct mgtcgi_xml_nat_snats *old_nodes = NULL;
	struct mgtcgi_xml_nat_snats *new_nodes = NULL;

#if CGI_FORM_STRING
	cgiFormString("new_id", new_id_str, sizeof(new_id_str));
	cgiFormString("old_id", old_id_str, sizeof(old_id_str));
#endif
	
	if((check_cgiform_string(new_id_str) != 0)||
		(check_cgiform_string(old_id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	old_id = atoi(old_id_str);
	new_id = atoi(new_id_str);
	if (!((0 <= old_id && old_id <= 65535) &&
		 (0 <= new_id && new_id <= 65535))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	
	if (old_id == new_id){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&old_nodes, &old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	old_move_index = -1;
	new_move_index = -1;
	max_index = 0;
	for (index=0; index < old_nodes->num; index++){
	
		if (old_nodes->pinfo[index].id == old_id)
			old_move_index = index;//old_id的位置
	
		if (old_nodes->pinfo[index].id == new_id)
			new_move_index = index;//new_id的位置
	
		max_index = index;
	
	}
	if (old_move_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_move_index == -1){//没有找到要移动的条目的位置
		if (new_id > old_id)
			new_move_index = max_index;
		else
			new_move_index = 0;
	}
	if (new_move_index == old_move_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}
	
	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	new_size = sizeof(struct mgtcgi_xml_nat_snats) + 
		new_num * 
			sizeof(struct mgtcgi_xml_firewall_info);
	
	new_nodes = (struct mgtcgi_xml_nat_snats *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}
	
	memset(new_nodes, 0, new_size);
	
	//拷贝 数据
	new_nodes->num = new_num;
	iserror = -2;
	id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){
	
		if (index == old_move_index){ 
			iserror ++;
			new_index--; //跳过old_id,new_index值不变；跳过的部分在new_id位置插入
		
		}
		else if (index == new_move_index){ //插入old_id，并拷贝当前数据
			iserror ++;
			if (old_move_index < new_move_index){//往下移动
			
				copy_snats_value( new_nodes, old_nodes, new_index, index, id);
				
				id++;
				new_index++;
				
				copy_snats_value( new_nodes, old_nodes, new_index, old_move_index, id);
	
			}
			else {//往上移动
			
				copy_snats_value( new_nodes, old_nodes, new_index, old_move_index, id);
	
				id++;
				new_index++;
				
				copy_snats_value( new_nodes, old_nodes, new_index, index, id);
			}
			
			id++;
	
		}
		else {
			copy_snats_value( new_nodes, old_nodes, new_index, index, id);
			id++;
		}	
	}
	
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS,(void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	else {
		ret_value = apply_snat();
		if (ret_value != 0){
			iserror = MGTCGI_APPY_ERR;
			goto ERROR_EXIT;
		}

		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}
	
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
#endif
	return;
}

void route_macbind_remove_save()
{
	int index=0,iserror=0,ret_value=0,i=0,j=0,count=0,total=0,learnt=0,bridge=0,new_num=0,new_size=0;
	int comma_num=0,semicolon_num=0,type=0,value_size=0;
	const char *dot=":";
	char *value = NULL;
	char learnt_str[STRING_LENGTH]={"0"};
	char total_str[STRING_LENGTH]={"2"};
	char ipaddr[STRING_LENGTH]={"0"};
	char macaddr[STRING_LENGTH]={"0"};
	char type_str[STRING_LENGTH]={"0"};
	char tmpstr[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_mac_binds *new_nodes = NULL;

	value_size = 5 * STRING_LIST_LENGTH * sizeof(char);
	value = (char *)malloc(value_size);//500k,大约5000条
	if (NULL == value){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

#if CGI_FORM_STRING	
	cgiFormString("value", value, value_size);
	cgiFormString("learnt", learnt_str, sizeof(learnt_str));
	cgiFormString("total", total_str, sizeof(total_str));
	cgiFormString("ipaddr", ipaddr, sizeof(ipaddr));
	cgiFormString("macaddr", macaddr, sizeof(macaddr));
	cgiFormString("type", type_str, sizeof(type_str));
#endif
	learnt=atoi(learnt_str);
	type=atoi(type_str);
	if (type ==1){ //删除单条
		if ((check_ipaddr(ipaddr) != 0)||
			(check_mac_addr(macaddr, dot) != 0)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		
		count = check_mac_bind_list(value);
		if (count < 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		
		total=atoi(total_str);
		if(count != total){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	else if (type == 2){  //删除所有
		count=0;
		learnt = 1;		//删除所有，自动启用mac学习。
		memset(value, 0, sizeof(value));
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//将数据添加到结构中
	new_num = count; 
	new_size = sizeof(struct mgtcgi_xml_mac_binds) + 
				new_num * 
				sizeof(struct mgtcgi_xml_mac_binds_info);

	new_nodes = (struct mgtcgi_xml_mac_binds *)malloc(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	new_nodes->learnt = learnt;
	new_nodes->num = new_num;

	memset(tmpstr, 0, sizeof(tmpstr));
	for (index=0,i=0,j=0; i<strlen(value); i++){
		//192.168.0.10,00-23-1D-2B-60-4F,0;
		if (value[i] == ','){
			if (comma_num == 0){	//ip
				strcpy(new_nodes->pinfo[index].ip, tmpstr);
			}
			else if (comma_num == 1){	//mac
				strcpy(new_nodes->pinfo[index].mac, tmpstr);
			}
			else if(comma_num == 2){
				new_nodes->pinfo[index].action = atoi(tmpstr);
				new_nodes->pinfo[index].id = index;
				new_nodes->pinfo[index].bridge = bridge;	
			}
			else {
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num++;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (value[i] == ';'){
			if (comma_num == 3){	//comment
				strcpy(new_nodes->pinfo[index].comment,tmpstr);
				index++;
			}
				
			semicolon_num++;
			if (semicolon_num > new_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num=0;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (j < sizeof(tmpstr))
			tmpstr[j] = value[i];
		j++;
	}

	new_size = 0;
	ret_value = save_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void*)new_nodes, new_size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		if (type == 1)
			ret_value = del_macbind(type, ipaddr, macaddr);
		else
			ret_value = del_macbind(type, NULL, NULL);
		
		ret_value = copy_mac_bind_xmlconfig_file(working_mac_bind_xmlconf, flash_mac_bind_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(new_nodes){
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    if(value){
	    free(value);
        value = NULL;
    }
	return;
}

/********************************* apply **********************************/
void route_routing_apply(void)
{
	int iserror=0, ret_value=0;
	ret_value = apply_routing();
	if (ret_value != 0){
		iserror = MGTCGI_APPLY_ERR;
		goto ERROR_EXIT;
    }

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	return;	
}

void ifwan_channel_apply()
{
    int iserror=0, ret_value=0;
    ret_value = apply_wan_channel("apply",0,NULL,NULL);
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    return; 
}

void dnat_apply(void)
{
    int iserror=0, ret_value=0;
    ret_value = apply_dnat();
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    return; 
}

void snat_apply(void)
{
    int iserror=0, ret_value=0;
    ret_value = apply_snat();
    if (ret_value != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    return; 
}

/********************************* command **********************************/
void route_adsl_cmd(void)
{
	int iserror=0,ret_value=0;
	char command[STRING_LENGTH]={0};
	char type[STRING_LENGTH]={0};
	char id_str[STRING_LENGTH]={0};
    int id = 0,size,enable=0,index;
    int oldstate = 0;

    struct mgtcgi_xml_interface_wans *wans_info = NULL;
#if CGI_FORM_STRING
	cgiFormString("command", command, sizeof(command));
	cgiFormString("type", type, sizeof(type));
	cgiFormString("id", id_str, sizeof(id_str));
#endif

	if((check_cgiform_string(command) != 0)||
		(check_cgiform_string(type) != 0)||
		(check_cgiform_string(id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    id = atoi(id_str);
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
    if (NULL == wans_info){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    enable=0;
    if(strcmp(type,"up") == 0){
        enable = 1;
    }

    iserror = -1;
    for(index = 0; index < wans_info->num; index++){
        if(id == wans_info->pinfo[index].id){
            oldstate = wans_info->pinfo[index].enable;
            wans_info->pinfo[index].enable = enable;

            iserror = 0;
            break;
        }
    }

    if(iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    iserror = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wans_info, size);
    if (iserror < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

	if (strcmp(command, "dial") == 0){ //拨号
		ret_value = ifwan_dial(type,id,oldstate);
		if (ret_value != 0){
			iserror = MGTCGI_ADSL_DIAL_ERR;
			goto ERROR_EXIT;
		}
	}

    iserror = 0;
/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wans_info){
	    free_xml_node((void*)&wans_info);
        wans_info = NULL;
    }
	return; 
}

void route_l2tpvpn_cmd(void)
{
	int iserror=0,ret_value=0,size;
	char command[STRING_LENGTH]={0};
	char type[STRING_LENGTH]={0};
	char id_str[STRING_LENGTH]={0};
    int id = 0,enable=0,index;
    int oldstate = 0;

    struct mgtcgi_xml_interface_wans *wans_info = NULL;
    
#if CGI_FORM_STRING
	cgiFormString("command", command, sizeof(command));
	cgiFormString("type", type, sizeof(type));
	cgiFormString("id", id_str, sizeof(id_str));
#endif

	if((check_cgiform_string(command) != 0)||
		(check_cgiform_string(type) != 0)||
		(check_cgiform_string(id_str) != 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    id = atoi(id_str);
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans_info, &size);
    if (NULL == wans_info){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    enable=0;
    if(strcmp(type,"up") == 0){
        enable = 1;
    }

    iserror = -1;
    for(index = 0; index < wans_info->num; index++){
        if(id == wans_info->pinfo[index].id){
            oldstate = wans_info->pinfo[index].enable;
            wans_info->pinfo[index].enable = enable;

            iserror = 0;
            break;
        }
    }

    if(iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    iserror = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wans_info, size);
    if (iserror < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 

    if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    
	if (strcmp(command, "dial") == 0){ //拨号
		ret_value = ifwan_dial(type, id, oldstate);
		if (ret_value != 0){
			iserror = MGTCGI_APPLY_ERR;
			goto ERROR_EXIT;
		}
	}

/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(wans_info){
	    free_xml_node((void*)&wans_info);
        wans_info = NULL;
    }

	return; 
}

void route_macbind_learnt_cmd(void)
{
	int  iserror=1,ret_value=0,size=0;
	int learnt=0;
	char learnt_str[STRING_LENGTH]={0};
	struct mgtcgi_xml_mac_binds *macbind = NULL;
	
#if CGI_FORM_STRING
	cgiFormString("learnt", learnt_str, sizeof(learnt_str));
#endif
	learnt=atoi(learnt_str);
	if (learnt != 0)
		learnt = 1;

	get_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void**)&macbind, &size);
	if ( NULL == macbind){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	macbind->learnt = learnt;

	ret_value = save_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void*)macbind, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		macbind_enbale_learnt_cmd(learnt);
		ret_value = copy_mac_bind_xmlconfig_file(working_mac_bind_xmlconf, flash_mac_bind_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}	
	}

/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(macbind){
	    free_xml_node((void*)&macbind);
        macbind = NULL;
    }
	return;
}

void route_dhcpd_enable_cmd(void)
{
	int  iserror=1,ret_value=0,size=0;
	int dhcpd_enable=0;
	char dhcpd_enable_str[DIGITAL_LENGTH]={0};
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	
#if CGI_FORM_STRING
	cgiFormString("dhcpd_enable", dhcpd_enable_str, sizeof(dhcpd_enable_str));
#endif

    dhcpd_enable=atoi(dhcpd_enable_str);
	if (dhcpd_enable != 0)
		dhcpd_enable = 1;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if ( NULL == dhcpd){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	dhcpd->dhcpd_enable = dhcpd_enable;

	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void*)dhcpd, size);
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
	else {
		ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
		if (ret_value != 0){
			iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
			goto ERROR_EXIT;
		}	

		if (dhcpd->dhcpd_enable == 1){
			system("killall -9 dnsmasq >/dev/null 2>&1;/usr/local/sbin/dnsmasq >/dev/null 2>&1");
		}
		else{
			system("killall -9 dnsmasq >/dev/null 2>&1");
		}
	}

/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(dhcpd){
	    free_xml_node((void*)&dhcpd);
        dhcpd = NULL;
    }
	return;
}

void route_pptpd_enable_cmd(void)
{
    int  iserror=1,ret_value=0,size=0;
    int pptpd_enable=0;
    char pptpd_enable_str[DIGITAL_LENGTH]={0};
    struct mgtcgi_xml_pptpd_server *pptpdvpn = NULL;
    
#if CGI_FORM_STRING
    cgiFormString("pptpd_enable", pptpd_enable_str, sizeof(pptpd_enable_str));
#endif

    pptpd_enable=atoi(pptpd_enable_str);
    if (pptpd_enable != 0)
        pptpd_enable = 1;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_SERVERS, (void**)&pptpdvpn, &size);
    if ( NULL == pptpdvpn){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    pptpdvpn->enable = pptpd_enable;

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPTPD_SERVERS, (void*)pptpdvpn, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 
    else {
        ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
        if (ret_value != 0){
            iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
            goto ERROR_EXIT;
        }   

        if (pptpdvpn->enable == 1){
            system("killall -9 pptpd >/dev/null 2>&1;/usr/local/sbin/pptpd -c /etc/ppp/pptpd.conf>/dev/null 2>&1");
        }
        else{
            system("killall -9 pptpd >/dev/null 2>&1");
        }
    }

/**************************成功，释放内存***************************/
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(pptpdvpn){
        free_xml_node((void*)&pptpdvpn);
        pptpdvpn = NULL;
    }
    return;

}

void route_tables_enable_cmd(void)
{
    int  iserror=1,ret_value=0,size=0,index=0;
    int table_enable=0,id=1;
    char id_str[DIGITAL_LENGTH]={0};
    char table_enable_str[DIGITAL_LENGTH]={0};
    struct mgtcgi_xml_route_tables *tables = NULL;
    
#if CGI_FORM_STRING
    cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("table_enable", table_enable_str, sizeof(table_enable_str));
#endif

    table_enable=atoi(table_enable_str);
    if (table_enable != 0)
        table_enable = 1;

    id = atoi(id_str);

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTING_TABLES, (void**)&tables, &size);
    if ( NULL == tables){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    iserror = -1;
    for(index = 0; index < tables->num; index++){
        if(id == tables->pinfo[index].id){
            iserror = 0;
            tables->pinfo[index].enable = table_enable;
        }
    }

    if(iserror != 0){
        iserror= MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTING_TABLES, (void*)tables, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 
    else {
        if(uptate_sys_route_config_sh() != 0){
            iserror = MGTCGI_SAVE_FLASH_ERR; 
            goto ERROR_EXIT;
        }

        iserror = apply_tables();
        if(iserror != 0){
            iserror = MGTCGI_APPLY_ERR;
            goto ERROR_EXIT;    
        }
        
        ret_value = copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
        if (ret_value != 0){
            iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
            goto ERROR_EXIT;
        }       
    }

/**************************成功，释放内存***************************/
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(tables){
        free_xml_node((void*)&tables);
        tables = NULL;
    }
    return;
}

void ifwan_channel_enable_cmd()
{
    int  iserror=1,ret_value=0,size=0,index=0;
    int  channel_enable=0,id=1;
    char id_str[DIGITAL_LENGTH]={0};
    char channel_enable_str[DIGITAL_LENGTH]={0};
    struct mgtcgi_xml_wans_channels *channels = NULL;
    
#if CGI_FORM_STRING
    cgiFormString("id", id_str, sizeof(id_str));
    cgiFormString("channel_enable", channel_enable_str, sizeof(channel_enable_str));
#endif

    channel_enable=atoi(channel_enable_str);
    if (channel_enable != 0)
        channel_enable = 1;

    id = atoi(id_str);

    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&channels, &size);
    if ( NULL == channels){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    iserror = -1;
    for(index = 0; index < channels->num; index++){
        if(id == channels->pinfo[index].id){
            iserror = 0;
            channels->pinfo[index].enable = channel_enable;
        }
    }

    if(iserror != 0){
        iserror= MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void*)channels, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 

 /*   if(uptate_sys_route_config_sh() != 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }*/
    
    ret_value = copy_route_xmlconf_file(working_wanchannel_xmlconf, flash_wanchannel_xmlconf);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }   

/**************************成功，释放内存***************************/
    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(channels){
        free_xml_node((void*)&channels);
        channels = NULL;
    }
    return;
}

extern int import_file(const char *tmpfile_name, char file_name[]);

void sys_macbind_import(void)
{
	int iserror = 0;
	unsigned int buff_size = 0;
	char url[URL_LENGTH]={"0"};
	char cmd[CMD_LENGTH] = {0};
	char * buffer = NULL;
	char file_name[STRING_LENGTH]={0};
	const char *tmpfile_name="/tmp/tmp_macbind_import_file";
	FILE *fp = NULL;

	buff_size = 5 * STRING_LIST_LENGTH * sizeof(char);
	buffer = (char *)malloc(buff_size);	//500k,大约5000条
	if (NULL == buffer){
		iserror = MGTCGI_READ_FILE_ERR;
		goto EXIT;
	}

	cgiFormString("url", url, sizeof(url));

	iserror = import_file(tmpfile_name, file_name);
	if (iserror != 0)
		goto EXIT;
	
	memset(buffer, 0, buff_size);
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh macbind_import %s\n",tmpfile_name );

	fp=popen(cmd,"r");
	fgets(buffer, buff_size, fp);
	pclose(fp);
	
	route_macbind_edit_save(buffer);

EXIT:
	free(buffer);
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
	//printf("Location:%s?iserror=%d\r\n\r\n\r\n",url,iserror);
	//log_debug("<script>window.location='%s?iserror=%d';</script>",url,iserror);
	return;	
}
extern int get_macbind_path_filename(char *filename);
extern int export_file(const char *file_name, const char *export_name);
extern int del_macbind_path_filename(char *filename);

void sys_macbind_export(void)
{
	int retval = 0;
	const char *export_name = "mac-ip.txt";
	char file_name[CMD_LENGTH]={"0"};
	memset(file_name, 0, sizeof(file_name));
	retval = get_macbind_path_filename(file_name);
	
	if (retval == 0){
		retval=export_file(file_name, export_name);
		//log_debug("file_name = %s\n", file_name);
		del_macbind_path_filename(file_name);
	}
	return;
}

static int check_tables_ip_type(const char *src)
{
    int iserror = -1;
    char ip[STRING_LENGTH] = {0};
    FILE *fp = NULL;
    
    if(!src)
        return -1;

    fp = fopen(src,"r");
    if(fp == NULL){
        goto EXIT;
    }

    while(fgets(ip,sizeof(ip),fp) != NULL){
        if(check_ipaddr(ip) != 0){
            goto EXIT;
        }
        memset(ip,0,sizeof(ip));
    }

    iserror = 0;
EXIT:
    if(fp){
        fclose(fp);
        fp = NULL;
    }
    return (iserror);
}

void route_tables_xmlconfig_import()
{
    int iserror = 0,id = 1;
    char url[URL_LENGTH]={"0"};
    char cmd[CMD_LENGTH] = {0};
    char id_str[DIGITAL_LENGTH] = {0};
    char file_name[STRING_LENGTH]={0};
    const char *tmpfile_name="/tmp/route_xmlconf.tmp";
    
    cgiFormString("url", url, sizeof(url));
    cgiFormString("id", id_str, sizeof(id_str));

    id = atoi(id_str);
    if(!(id > 0 && id < 32))
        goto EXIT; 

    iserror = import_file(tmpfile_name, file_name);
    if (iserror != 0)
        goto EXIT;

    if(check_tables_ip_type(tmpfile_name) != 0){
        iserror = MGTCGI_IMPROT_FILE_BAD;
        goto EXIT; 
    }

    flush_timer();
    if(check_flash_mount() != 0){
        goto EXIT; 
    }
    
    snprintf(cmd,sizeof(cmd),"cp %s /etc/%d >/dev/null 2>&1;rm -rf %s >/dev/null 2>&1;cp /etc/%d /flash/etc/%d >/dev/null 2>&1",tmpfile_name,id,tmpfile_name,id,id);
    system(cmd);
    start_timer();

    memset(cmd, 0, sizeof(cmd));
    iserror = apply_tables();
    if(iserror != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto EXIT;    
    }

    iserror = 0;
EXIT:
    printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
    return;
}

void sys_route_xmlconfig_import(void)
{
	int iserror = 0;
	char url[URL_LENGTH]={"0"};
	char cmd[CMD_LENGTH];
	char buffer[DIGITAL_LENGTH];
	char file_name[STRING_LENGTH]={0};
	const char *tmpfile_name="/tmp/route_xmlconf.tmp";
	const char *apply_xmlconf_cmd="/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh check_route_config";
	FILE *fp = NULL;
	cgiFormString("url", url, sizeof(url));

	iserror = import_file(tmpfile_name, file_name);
	if (iserror != 0)
		goto EXIT;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"%s %s",apply_xmlconf_cmd, tmpfile_name);

	fp=popen(cmd,"r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);

	iserror = atoi(buffer);
	if (iserror != 0)
		iserror = MGTCGI_IMPROT_FILE_BAD;

EXIT:
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
	return;

}

void sys_route_xmlconfig_export(void)
{
    int iserror = 0;
	const char *export_name="route_config.xml";
	const char *file_name="/etc/route_config.xml";
	
	iserror = export_file(file_name, export_name);
	return;  
}

#if 0
/******************************** route tools *****************************/
static void get_tools_ping_info(const char *ifname, char *link_status)
{
	char cmd[CMD_LENGTH]={0};
	FILE *pp = NULL;

	strcpy(link_status, "no");
	//snprintf(cmd, sizeof(cmd), "%s/route_script.sh ping %s", script_path, ifname);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(link_status, STRING_LENGTH, pp);
		link_status[strlen(link_status)-1] = 0;
		if (strcmp(link_status, "yes") != 0){
			strcpy(link_status, "no");
		}
	}
	pclose(pp);
}

static void get_tools_traceroute_info(const char *ifname, char *link_status)
{
	char cmd[CMD_LENGTH]={0};
	FILE *pp = NULL;

	strcpy(link_status, "no");
	//snprintf(cmd, sizeof(cmd), "%s/route_script.sh traceroute %s", script_path, ifname);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(link_status, STRING_LENGTH, pp);
		link_status[strlen(link_status)-1] = 0;
		if (strcmp(link_status, "yes") != 0){
			strcpy(link_status, "no");
		}
	}
	pclose(pp);
}
#endif
static int get_chart_netdev_info(const char *devname, char *buffer)
{
	char cmd[CMD_LENGTH]={0};
	FILE *pp = NULL;

	//memset(buffer, 0, sizeof(buffer));
	//snprintf(cmd, sizeof(cmd), "%s/route_script.sh netdev %s", script_path, devname);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fread( buffer, sizeof(char), 4096, pp);  
		//buffer[strlen(buffer)-1] = 0;
		//fgets(buffer, 4096, pp);
		printf("%s\n",buffer);

	}
	pclose(pp);

	return 0;
}



void route_tools_ping()
{

}
void route_tools_traceroute()
{

}

void route_chart_netdev()
{
	int iserror = 0,ret_value=0;
	//char buffer[BUF_LENGTH] = {0};
	char *buffer = NULL;
	char devname[STRING_LENGTH]={"all"};
	

#if CGI_FORM_STRING
	cgiFormString("devname", devname, sizeof(devname));
#endif

	if(check_cgiform_string(devname) != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    buffer = (char *) malloc(BUF_LENGTH*sizeof(char));
    if(!buffer)
        goto ERROR_EXIT;
    memset(buffer,0,BUF_LENGTH*sizeof(char));
	ret_value = get_chart_netdev_info(devname, buffer);
	if (ret_value != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//printf("{\"netdev_info\":\"%s\",\"iserror\":0,\"msg\":\"\"}",buffer);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(buffer){
        free(buffer);
        buffer = NULL;
    }
	return;
}

void route_dhcp_lease_info(void){
    char lease_file[STRING_LENGTH] = {"/var/lib/dnsmasq.leases"};
    char buffer[LONG_STRING_LENGTH] = {0};
    char page_str[DIGITAL_LENGTH] = {0};
    char pagesize_str[DIGITAL_LENGTH] = {0};
    char ip[STRING_LENGTH] = {0};
    char mac[STRING_LENGTH] = {0};
    char host[STRING_LENGTH] = {0};
    FILE *fp = NULL;
    char *p = NULL;
    char *point = NULL;
    const char dot[4] = ":";
    int sep = 0,iserror = 0, sign = 1;
    int page, pagesize;
    int total = 0,first = 0, last = 0;
    struct in_addr inp1;


#if CGI_FORM_STRING
    cgiFormString("page", page_str, sizeof(page_str));
    cgiFormString("pagesize", pagesize_str, sizeof(pagesize_str));
#endif

    page = atoi(page_str);
    if(page <= 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    pagesize = atoi(pagesize_str);
    if(pagesize <= 0){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    first = (page - 1) * pagesize + 1;
    last = page * pagesize;
    
    fp = fopen(lease_file,"a+");
    if(NULL == fp){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"data\":[");
    while(fgets(buffer,sizeof(buffer),fp)){
        p = buffer;
        while(p){
            while((point = strsep(&p," "))){
                if(sign == 2){
                    strncpy(mac,point,sizeof(mac)-1);
                }
                else if(sign == 3){
                    strncpy(ip,point,sizeof(ip)-1);
                }
                else if(sign == 4){
                    strncpy(host,point,sizeof(host)-1);
                }
                sign++;
            }
        }
        sign = 1;

        if((inet_aton(ip,&inp1) == 0) || (check_mac_addr(mac,dot) != 0)){
            memset(ip,0,sizeof(ip));
            memset(mac,0,sizeof(mac));
            memset(buffer,0,sizeof(buffer));
            continue;
        }

        total++;
        if((total >= first) && (total <= last)){
            printf("%s",(sep?",":""));
            sep = 1;
            
            printf("[\"%s\",\"%s\",\"%s\"]",ip,mac,host);
        }
        
        p = NULL;
        point = NULL;
        memset(ip,0,sizeof(ip));
        memset(mac,0,sizeof(mac));
        memset(buffer,0,sizeof(buffer));
    }
    printf("],\"iserror\":%d,\"total\":%d,\"msg\":\"\"}",iserror,total);
    goto EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"total\":0,\"msg\":\"\"}",iserror);
    goto EXIT;
EXIT:
    return;
}


