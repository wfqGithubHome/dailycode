#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h> 
#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/file.h>


#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
#include "../include/route_xmlwrapper4c.h"

extern int do_log(char *log);
extern void log_debug(const char* fmt, ...);
extern void flush_timer(void);
extern void start_timer(void);
extern int check_flash_mount(void);
int mask_to_maskbit(char *mask_str, uint8_t *maskbit);
int print_lan_dhcp_conf(struct mgtcgi_xml_interface_lan_dhcp_info *dhcpd, const char *ifname, const int fd);

const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

#if 0
//Add by chenhao 2015-12-3 09:16:15
const struct dev_firmark_info wans[MAX_WAN_NUM] = {
    {
        .dev_name = "WAN0",
        .firmark  = 0x100,
        .table    = 100,
    },
    {
        .dev_name = "WAN1",
        .firmark  = 0x101,
        .table    = 101,
    },
    {
        .dev_name = "WAN2",
        .firmark  = 0x102,
        .table    = 102,
    },
    {
        .dev_name = "WAN3",
        .firmark  = 0x103,
        .table    = 103,
    },
    {
        .dev_name = "WAN4",
        .firmark  = 0x104,
        .table    = 104,
    }
};

const struct dev_firmark_info ppps[MAX_PPP_NUM] = {
    {
        .dev_name = "ppp0",
        .firmark  = 0x200,
        .table    = 200,
    },
    {
        .dev_name = "ppp1",
        .firmark  = 0x201,
        .table    = 201,
    },
    {
        .dev_name = "ppp2",
        .firmark  = 0x202,
        .table    = 202,
    },
    {
        .dev_name = "ppp3",
        .firmark  = 0x203,
        .table    = 203,
    },
    {
        .dev_name = "ppp4",
        .firmark  = 0x204,
        .table    = 204,
    },
    {
        .dev_name = "ppp5",
        .firmark  = 0x205,
        .table    = 205,
    },
    {
        .dev_name = "ppp6",
        .firmark  = 0x206,
        .table    = 206,
    },
    {
        .dev_name = "ppp7",
        .firmark  = 0x207,
        .table    = 207,
    },
    {
        .dev_name = "ppp8",
        .firmark  = 0x208,
        .table    = 208,
    },
    {
        .dev_name = "ppp9",
        .firmark  = 0x209,
        .table    = 209,
    },
    {
        .dev_name = "ppp10",
        .firmark  = 0x210,
        .table    = 210,
    },
    {
        .dev_name = "ppp11",
        .firmark  = 0x211,
        .table    = 211,
    },
    {
        .dev_name = "ppp12",
        .firmark  = 0x212,
        .table    = 212,
    },
    {
        .dev_name = "ppp13",
        .firmark  = 0x213,
        .table    = 213,
    },
    {
        .dev_name = "ppp14",
        .firmark  = 0x214,
        .table    = 214,
    }
};

const struct dev_firmark_info lans[MAX_PPP_NUM] = {
    {
        .dev_name = "LAN",
        .firmark  = 0x300,
        .table    = 300,
    }
};
#endif

const struct dev_firmark_info wans[MAX_WAN_NUM] = {
        {"WAN0",0x100,100},
        {"WAN1",0x101,101},
        {"WAN2",0x102,102},
        {"WAN3",0x103,103},
        {"WAN4",0x104,104},
};

const struct dev_firmark_info ppps[MAX_PPP_NUM] = {
        {"ppp0",0x200,200},
        {"ppp1",0x201,201},
        {"ppp2",0x202,202},
        {"ppp3",0x203,203},
        {"ppp4",0x204,204},
        {"ppp5",0x205,205},
        {"ppp6",0x206,206},
        {"ppp7",0x207,207},
        {"ppp8",0x208,208},
        {"ppp9",0x209,209},
        {"ppp10",0x210,210},
        {"ppp11",0x211,211},
        {"ppp12",0x212,212},
        {"ppp13",0x213,213},
        {"ppp14",0x214,214},
};

const struct dev_firmark_info lans[MAX_LAN_NUM] = {
        {"LAN",0x300,300},
};


void set_reuid()
{
	uid_t uid ,euid; 
	uid = getuid(); 
	euid = geteuid(); 
	setreuid(euid, uid);
}

int do_get_command(const char *cmd, char *buffer, int lenght)
{
	FILE *pp = NULL;
	
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, lenght, pp);
		buffer[strlen(buffer)-1] = 0;
	}
	pclose(pp);
	return 0;
}

// ·µ»ØÖµ0-³É¹¦£¬-1Ê§°Ü
int do_command(const char *cmd)
{
	int value=1;
	char buffer[DIGITAL_LENGTH]={0};
	FILE *pp = NULL;
	
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);        
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
			value = 0;
		else
			value = -1;
	}
	pclose(pp);
	return value;
}

void do_system_command(const char *cmd)
{
	set_reuid();
	system(cmd);
}

int uptate_sys_route_config_sh(void)
{
    char tmp_file[STRING_LENGTH] = {0};
    char cmd[512] = {0};

    snprintf(tmp_file,sizeof(tmp_file),"/tmp/route_config.sh.%d",getpid());
    snprintf(cmd,sizeof(cmd),"/usr/local/sbin/route-xmlconf -f %s > %s 2>/dev/null;/usr/local/sbin/filelock -s -f /tmp/route_config.sh -d %s >/dev/null 2>&1;rm -rf %s >/dev/null 2>&1",
            ROUTE_CONFIG_FILE,tmp_file,tmp_file,tmp_file);
    do_system_command(cmd);

    return 0;
}

int get_route_save_lock(void)
{
    char path[]="/var/lock/http_router.lock";
    int fd;
    fd=open(path,O_WRONLY|O_CREAT);
	if(fd < 0){
		return -1;
	}
    if(flock(fd,LOCK_EX)!=0){
		close(fd);
		return -1;
    }
    return fd;
}

void put_route_save_lock(int fd)
{
    flock(fd,LOCK_UN);
	close(fd);	
}

/*
int do_sys_command(char *cmd)
{
	int value=1;
	char buffer[DIGITAL_LENGTH]={0};
	FILE *pp = NULL;
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
	
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
			value = 0;
		else
			value = -1;
	}
	pclose(pp);
	return value;
}*/



int copy_mac_bind_xmlconfig_file(const char * srcfile, const char * dstfile)
{
    int ret_value=0;
    char cmd[512];

    flush_timer();
    if(check_flash_mount() != 0){
        ret_value = -1;
        goto EXIT;
    }

    snprintf(cmd, sizeof(cmd), "/bin/chmod +w /flash/etc/mac_bind.xml;/bin/cp %s %s ;sync;sync;/bin/chmod -w /flash/etc/mac_bind.xml",srcfile,dstfile);
	do_command(cmd);

    start_timer();

EXIT:
    return ret_value;
}
/*
int copy_iface_xmlconfig_file(const char * srcfile, const char * dstfile)
{
    int ret_value=0;
    char cmd[512];
    
    flush_timer();
    if(check_flash_mount() != 0){
        ret_value = -1;
        goto EXIT;
    }
    
    snprintf(cmd, sizeof(cmd), "/bin/chmod +w /flash/etc/iface.xml;/bin/cp %s %s ;sync;sync;/bin/chmod -w /flash/etc/iface.xml",srcfile,dstfile);
	do_command(cmd);

    start_timer();
EXIT:
    return ret_value;
}
*/

int copy_route_xmlconf_file(const char * srcfile, const char * dstfile)
{
    int ret_value=0;
    char cmd[512];

    flush_timer();
    if(check_flash_mount() != 0){
        ret_value = -1;
        goto EXIT;
    }

    snprintf(cmd, sizeof(cmd), "/bin/chmod +w /flash/etc/route_config.xml;/bin/cp %s %s ;sync;sync;/bin/chmod -w /flash/etc/route_config.xml",srcfile,dstfile);
	do_command(cmd);

    start_timer();

EXIT:
	return ret_value;
}

int get_system_date(char *buffer, int lenght)
{
	char cmd[CMD_LITTER_LENGTH]={0};
	int  ret = 0;

	strncpy(cmd, "/bin/date +%D", CMD_LENGTH);
	ret = do_get_command(cmd, buffer, lenght);

	return ret;
}

int get_interface_ip(const char *ifname,char *ip,int len)
{
    int real = 0;
    char cmd[CMD_LITTER_LENGTH] = {0};
    char ip_str[STRING_LENGTH] = {0};
    const char *cmd_str = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_ip_by_interface";
    
    if(!ifname){
        return -1;
    }

    snprintf(cmd,sizeof(cmd),"%s %s",cmd_str,ifname);
    do_get_command(cmd, ip_str, sizeof(ip_str));

    strncpy(ip,ip_str,len);
    return real;
}

static int check_policy_iface(const int id, const char *iface)
{
    int index=0,j=0,if_num=0;
    char if_str[STRING_LENGTH] = {0};
    char str[LONG_STRING_LENGTH] = {0};
    
    if(!(id > 0 && id < 32)){
        return -1;
    }

    if(!iface){
        return -1;
    }

    strcpy(str,iface);

    j = 0;
    if_num = 0;
    for(index = 0; index < strlen(str); index++){
        if(str[index] == '|'){
            if_num = 0;
            j = 0;
            continue;
        }
        
        if(str[index] == ':'){
            if_num++;
            if(if_num == 2){
                if(id == atoi(if_str)){
                    return -1;
                }
                if_num = 0;
                j = 0;
                memset(if_str,0,sizeof(if_str));
            }
            continue;
        }

        if(if_num == 1){
            if_str[j] = str[index];
            j++;
        }
    }

    return 0;
}

int check_ifwan_used(const int id){
    int iserror = -1,size = 0,index;
    char iface[LONG_STRING_LENGTH] = {0};

    struct mgtcgi_xml_firewalls_policy *policy = NULL;
    struct mgtcgi_xml_nat_dnats *dnat = NULL;
    struct mgtcgi_xml_nat_snats *snat = NULL;
    
    if(!(id > 0 && id < 32)){
        goto FREE_EXIT;
    }

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&policy, &size);
    if (NULL == policy){
        goto FREE_EXIT;
    }
    for(index = 0; index < policy->num; index++){
        strncpy(iface, policy->pinfo[index].iface, LONG_STRING_LENGTH - 1);
        if(check_policy_iface(id,iface) != 0){
            goto FREE_EXIT;
        }
        memset(iface, 0 ,sizeof(iface));
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&dnat, &size);
	if (NULL == dnat){
		goto FREE_EXIT;
	}
    for(index = 0; index < dnat->num; index++){
        if(id == atoi(dnat->pinfo[index].ifname)){
            goto FREE_EXIT;
        }
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&snat, &size);
	if (NULL == snat){
		goto FREE_EXIT;
	}
    for(index = 0; index < snat->num; index++){
        if(id == atoi(snat->pinfo[index].ifname)){
            goto FREE_EXIT;
        }
    }

    iserror = 0;
FREE_EXIT:
    free_xml_node((void*)&policy);
    free_xml_node((void*)&dnat);
    free_xml_node((void*)&snat);
    return iserror;
}

#if 0
int adsl_client_dial(const char *type, const int id)
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};

    if(!type)
        return (-1);

    if(strcmp(type,"down") == 0){
	    snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove %d", script_path, id);
    }
    else if(strcmp(type,"up") == 0){
        snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply %d", script_path, id);
    }
    else{
        return (-1);
    }

	ret_value = do_command(cmd);
	return ret_value;
}

#endif

int ifwan_dial(char * type,const int id,  int oldstate)
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};

    if(!type)
        return (-1);
    
#if 0
    if(strcmp(type,"down") == 0){
	    snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 2", script_path, id);
    }
    else if(strcmp(type,"up") == 0){
        if(oldstate == 0){
            snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 1", script_path, id);
        }
        else{
            snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 2", script_path, id);
        }
    }
    else{
        return (-1);
    }
#endif

    snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 2", script_path, id);
	ret_value = do_command(cmd);
	return ret_value;
}

int xl2tpd_restart()
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh xl2tpd-restart", script_path);

	//printf("cmd:%s\n",cmd);
	ret_value = do_command(cmd);
	return ret_value;
}

int xl2tpd_stop()
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh xl2tpd-stop", script_path);

	//printf("cmd:%s\n",cmd);
	ret_value = do_command(cmd);
	return ret_value;
}

int init_waninfo_to_kernel(const U8 id, const char *access, const char *ifname, const char *sx, const char *xx)
{
    int ret_value=-1;
    char cmd[CMD_LENGTH] = {0};
    const char *ecbq_path = "/usr/local/sbin/ecbq";
    
    if(id == 0 || id > 31)
        return (ret_value);

    if(!ifname || !sx || !xx)
        return (ret_value);

    snprintf(cmd,sizeof(cmd),"%s --init --root --index %u --name %s --mark %u --maxrate %sKbps --downrate %sKbps >/dev/null 2>&1",ecbq_path,id,ifname,id,sx,xx);
    system(cmd);
    
    return (0);
}

int apply_waninfo_to_kernel(const U8 id, const char *access, const char *ifname,const char *ip, 
        const char *mask, const char *gateway, const char *isp)
{
    int ret_value=-1;
 //   char cmd[CMD_LENGTH] = {0};
    struct in_addr if_ip,if_mask,if_gateway;
    unsigned long int if_subnet1,if_subnet2;
//    const char *ecbq_path = "/usr/local/sbin/ecbq";

    if(id == 0 || id > 31)
        return (ret_value);

    if(!ifname || !access)
        return (ret_value);

    if(strcmp(access,"static") == 0){
        if(!ip || !mask || !gateway)
            return (ret_value);
        
        if((inet_aton(ip,&if_ip) == 0) ||
        (inet_aton(mask,&if_mask) == 0) || 
        (inet_aton(gateway,&if_gateway) == 0)){
            return (ret_value);
        }
        if_subnet1 = (ntohl(if_ip.s_addr) & ntohl(if_mask.s_addr));
        if_subnet2 = (ntohl(if_gateway.s_addr) & ntohl(if_mask.s_addr));
        if(if_subnet1 != if_subnet2){
            return (ret_value);
        }

    //    snprintf(cmd,sizeof(cmd),"%s --dev %s --root --name %s --index %u --start >/dev/null 2>&1", ecbq_path,ifname,ifname,id);
   //     system(cmd);
    }

    return 0;
}

int unlease_waninfo_to_kernel(const U8 id, const char *access, const char *ifname,const char *ip,
        const char *mask, const char *gateway, const char *isp)
{
    int ret_value=-1;
    char cmd[CMD_LENGTH] = {0};
    struct in_addr if_ip,if_mask,if_gateway;
    unsigned long int if_subnet1,if_subnet2;
    const char *ecbq_path = "/usr/local/sbin/ecbq";
    
    if(id == 0 || id > 31)
        return (ret_value);

    if(!ifname || !access)
        return (ret_value);

    if(strcmp(access,"static") == 0){
        if(!ip || !mask || !gateway)
        return (ret_value);

        if((inet_aton(ip,&if_ip) == 0) ||
            (inet_aton(mask,&if_mask) == 0) || 
            (inet_aton(gateway,&if_gateway) == 0)){
            return (ret_value);
        }
        if_subnet1 = (ntohl(if_ip.s_addr) & ntohl(if_mask.s_addr));
        if_subnet2 = (ntohl(if_gateway.s_addr) & ntohl(if_mask.s_addr));
        if(if_subnet1 != if_subnet2){
            return (ret_value);
        }

        snprintf(cmd,sizeof(cmd),"%s --root --name %s --index %d --stop >/dev/null 2>&1",ecbq_path,ifname,id);
        system(cmd);
        snprintf(cmd,sizeof(cmd),"%s --root --name %s --index %d --del >/dev/null 2>&1",ecbq_path,ifname,id);
        system(cmd);
    }
    else if(strcmp(access,"pppoe") == 0){

    }
    else if(strcmp(access,"l2tpclient") == 0){

    }

    snprintf(cmd,sizeof(cmd),"%s --root --name %s --index %d --stop >/dev/null 2>&1",ecbq_path,ifname,id);
    system(cmd);
    snprintf(cmd,sizeof(cmd),"%s --root --name %s --index %d --del >/dev/null 2>&1",ecbq_path,ifname,id);
    system(cmd);
    
    return (0);
}

int remove_wan(const U8 id,const char *ifname, const char *access, const char *ip, const char *mask, const char *gateway,
        const char *username,const char *passwd,const char *service,const char *mtu,const char *mac,const char *dns,
        const char *sx,const char *xx,const char *isp,const U8 defaultroute,const char *timed,const char *wday,const char *min2)
{
    int ret_value=-1;
	char cmd[CMD_LENGTH]={0};
    char mac_str[LITTER_LENGTH] = {0};

    if((!ifname) || (!access) || (!mac) || (!sx) || (!xx) || (!isp) || (!timed))
        return (ret_value);
    if((strlen(ifname) == 0) || (strlen(access) == 0))
        return (ret_value);

    if(id == 0 || id > 31)
        return (ret_value);

    if(strcmp(access,"static") == 0){
        if((!ip) || (!mask) || (!gateway))
            return (ret_value);
        if((strlen(ip) == 0) ||
            (strlen(mask) == 0) ||
            (strlen(gateway) == 0)){
            return (ret_value);
        }
    }
    else if(strcmp(access,"pppoe") == 0){
        if((!username) || (!passwd) || (!service))
            return (ret_value);
        if((strlen(username) == 0) || (strlen(passwd) == 0))
            return (ret_value);
    }
    else if(strcmp(access,"l2tpclient") == 0){
        if((!username) || (!passwd) || (!service))
            return (ret_value);
        if((strlen(username) == 0) || 
            (strlen(passwd) == 0) ||
            (strlen(service) == 0))
            return (ret_value);
    }

	if (strlen(mac) != 17)
		strcpy(mac_str, "00:00:00:00:00:00");
    else
        strcpy(mac_str, mac);
/*
    ret_value=unlease_waninfo_to_kernel(id,access,ifname,ip,mask,gateway,isp);
    if(ret_value != 0)
        return (ret_value);
*/
    snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove wan %d",
		script_path, id);
    ret_value=do_command(cmd);
	return ret_value;
}


int remove_vlan(const int index)
{
	int retval = -1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove vlan %d",
				script_path,index);

	retval = do_command(cmd);
	return retval;
}

int remove_macvlan(const int index)
{
	int retval = -1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove macvlan %d",
				script_path,index);

	retval = do_command(cmd);
	return retval;
}

int remove_adsl_link_and_conf(char *name, char *username, char *password)
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove adsl \"name=%s,username=%s,password=%s\"",
				script_path, name, username, password);

	ret_value = do_command(cmd);
	return ret_value;
}

int remove_l2tpvpn_conf(char *name)
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};
	
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh remove l2tpvpn %s",script_path, name);

	ret_value = do_command(cmd);
	return ret_value;
}

int apply_pptpd_account(char *username, char *passwd, char *ip)
{
    char cmd[CMD_LENGTH] = {0};
    const char *path = "/etc/ppp/chap-secrets";

    snprintf(cmd, sizeof(cmd), "echo \"'%s'    pptpd    '%s\'    %s\" >> %s",
        username, passwd, ip, path);
    system(cmd);

    return 0;
}

int apply_dns(char *dns_primary, char *dns_secondary, char *dns_thirdary)
{
	int ret_value=-1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply dns %s %s %s",
			script_path, dns_primary, dns_secondary, dns_thirdary);

	ret_value = do_command(cmd);
	return ret_value;
}

int apply_ddns()
{
	char 	cmd[CMD_LENGTH] = {0};
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply ddns", script_path);
	system(cmd);
	return MGTCGI_SUCCESS;
}

int apply_inteface_link(void)
{
    char    cmd[CMD_LENGTH] = {0};
	int     ret_value;

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply link",
			script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_lan(void)
{
	char 	cmd[CMD_LENGTH] = {0};
	int ret_value;

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply lan",
			script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

#if 0
int apply_lan(struct mgtcgi_xml_route_interface_lans *lan)
{
    int iserror = -1,retval,i;
    uint8_t mask_len = 32;
    int dhcp_fd,lan_command_fd;
    char buffer[LONG_STRING_LENGTH] = {0};
    struct mgtcgi_xml_interface_lan_dhcp_info *dhcp = NULL;
    
	if(NULL == lan)
        return 0;
    mask_to_maskbit(lan->mask, (uint8_t *)&mask_len);

    dhcp_fd = open(DHCP_CONFIG_PATH, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if(dhcp_fd < 0)
        goto EXIT;

    lan_command_fd = open(ROUTE_LAN_COMMAND, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if(lan_command_fd < 0)
        goto EXIT;

    dhcp = &(lan->dhcp_info);
    retval = print_lan_dhcp_conf(dhcp,lan->ifname,dhcp_fd);
    close(dhcp_fd);

    write(lan_command_fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"#!/bin/sh\n");
    write(lan_command_fd,buffer,strlen(buffer));

    if(strlen(lan->mac) > 0){
        snprintf(buffer,sizeof(buffer),"/usr/local/sbin/ip link set dev %s address %s >/dev/null 2>&1\n", lan->ifname,lan->mac);
        write(lan_command_fd,buffer,strlen(buffer));
    }
    snprintf(buffer,sizeof(buffer),"/usr/local/sbin/ip addr flush dev %s >/dev/null 2>&1\n", lan->ifname);
    write(lan_command_fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"/usr/local/sbin/ip addr add %s/%u dev %s >/dev/null 2>&1\n",lan->ip,mask_len,lan->ifname);
    write(lan_command_fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"/usr/local/sbin/ip link set dev %s address %s >/dev/null 2>&1\n",lan->ifname,lan->mac);
    write(lan_command_fd,buffer,strlen(buffer));
    for(i = 0; i < lan->subnet_num; i++){
        mask_to_maskbit(lan->pinfo[i].mask, (uint8_t *)&mask_len);
        snprintf(buffer,sizeof(buffer),"/usr/local/sbin/ip addr add %s/%u dev %s >/dev/null 2>&1\n",lan->pinfo[i].ip,mask_len,lan->ifname);
        write(lan_command_fd,buffer,strlen(buffer));
    }

    if(strcmp(lan->dhcp_type,"off") == 0){
        snprintf(buffer,sizeof(buffer),"killall dnsmasq >/dev/null 2>&1\n");
        write(lan_command_fd,buffer,strlen(buffer));
    }else{
        snprintf(buffer,sizeof(buffer),"killall dnsmasq >/dev/null 2>&1\n");
        write(lan_command_fd,buffer,strlen(buffer));
        snprintf(buffer,sizeof(buffer),"/usr/local/sbin/dnsmasq >/dev/null 2>&1\n");
        write(lan_command_fd,buffer,strlen(buffer));
    }
    close(lan_command_fd);

    snprintf(buffer,sizeof(buffer),"chmod +x %s;%s",ROUTE_LAN_COMMAND,ROUTE_LAN_COMMAND);
    system(buffer);
    iserror = 0;
EXIT:
	return iserror;
}
#endif

//ifname,access,ipaddr,netmask,gateway,username,passwd,service,mtu,mac,dns_str,sx,xx,isp,timed,wday,min2
int apply_wan(const char *type,const int id,const char *ifname, const char *access, const char *ip, const char *mask, const char *gateway,
        const char *username,const char *passwd,const char *service,const char *mtu,const char *mac,const char *dns,
        const char *sx,const char *xx,const char *isp,const int defaultroute,const char *timed,const char *wday,const char *min2)
{
	int ret_value=-1;
	char cmd[CMD_LENGTH]={0};
    char mac_str[LITTER_LENGTH] = {0};

    if(!type){
        return (ret_value);
    }

    if(strlen(type) == 0){
        return (ret_value);
    }

    if((!ifname) || (!access) || (!mac) || (!sx) || (!xx) || (!isp) || (!timed))
        return (ret_value);
    if((strlen(ifname) == 0) || (strlen(access) == 0))
        return (ret_value);

    if(strcmp(access,"static") == 0){
        if((!ip) || (!mask) || (!gateway))
            return (ret_value);
        if((strlen(ip) == 0) ||
            (strlen(mask) == 0) ||
            (strlen(gateway) == 0)){
            return (ret_value);
        }
    }
    else if(strcmp(access,"pppoe") == 0){
        if((!username) || (!passwd) || (!service))
            return (ret_value);
        if((strlen(username) == 0) || (strlen(passwd) == 0))
            return (ret_value);
    }

	if (strlen(mac) != 17)
		strcpy(mac_str, "00:00:00:00:00:00");
    else
        strcpy(mac_str, mac);

/*
    ret_value = init_waninfo_to_kernel(id,access,ifname,sx,xx);
    if(ret_value != 0)
        return ret_value;
*/
    if(strcmp(type,"add") == 0){
        snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 1",script_path, id);
    }
    else if(strcmp(type,"edit") == 0){
        snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply wan %d 2",script_path, id);
    }
    else{
        return (ret_value);
    }
    
    ret_value=do_command(cmd);
    if(ret_value != 0)
        return ret_value;

 //   ret_value = apply_waninfo_to_kernel(id,access,ifname,ip,mask,gateway,isp);
	return 0;
}

int apply_dhcp_wan(const char *ifname,const int defaultroute)
{
	char		cmd[CMD_LENGTH] = {0};
	int			ret_value = -1;

	snprintf(cmd,sizeof(cmd),"%s/route_script.sh dhcp %s %d",script_path,ifname,defaultroute);
	ret_value = do_command(cmd);

	return ret_value;
}

int apply_vlan(const int index)
{
	int retval=-1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply vlan %d",
			script_path, index);

	retval = do_command(cmd);
	return retval;
}

int apply_macvlan(const int index)
{
	int retval=-1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply macvlan %d",
			script_path,index);

	retval = do_command(cmd);
	return retval;
}


int apply_pppd_auth()
{
	int ret_value=-1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply pppdauth",script_path);

	ret_value = do_command(cmd);
	return ret_value;
}

int apply_adsl(int type, char *olduser, char *oldpasswd, char *name, char *ifname, char *username, char *password, char *servername, int mtu, int defaultroute, int enable, int peerdns)
{
	int ret_value=-1;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply adsl \"type=%d,olduser=%s,oldpasswd=%s,name=%s,ifname=%s,username=%s,password=%s,servername=%s,mtu=%d,defaultroute=%d,enable=%d,peerdns=%d\"", 
				script_path, type, olduser, oldpasswd, name, ifname, username, password, servername, mtu, defaultroute, enable, peerdns);

	ret_value = do_command(cmd);
	return ret_value;
}

int apply_l2tpvpn(char *oldvpn, char *name, char *ifname, char  *username, char *password, char *servername, int mtu, int defaultroute, int enable, int peerdns)
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply l2tpvpn oldvpn=%s,name=%s,ifname=%s,username=%s,password=%s,servername=%s,mtu=%d,defaultroute=%d,enable=%d,peerdns=%d", 
					script_path, oldvpn, name, ifname, username, password, servername, mtu, defaultroute, enable, peerdns);

	ret_value = do_command(cmd);
	return ret_value;
}

int update_list_routing()
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "/usr/local/sbin/route-xmlconf -R -f /etc/route_config.xml >/dev/null 2>&1");

	ret_value = do_command(cmd);
	return 0;	
}

int apply_routing()
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply routing", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_channel(void)
{
    int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply channel", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_delete_dnat(int dnat_delete,int rule_delete,const char *ifname, const char *protocol,
    const char *wan_ip, const char *wan_port,const char *lan_ip, const char *lan_port, int fwmark, int table)
{
#if 1
    int ret_value = -1;
    char dev[STRING_LENGTH] = {0};
    char rule_cmd[CMD_LITTER_LENGTH] = {0};
 //   char nat_cmd[CMD_LENGTH] = {0};
  //  char mark_cmd[CMD_LENGTH] = {0};
    char route_cmd[CMD_LITTER_LENGTH] = {0};
    char print_dev_str[CMD_LITTER_LENGTH] = {0};
    char ip_str[STRING_LENGTH] = {0};

    char *nat_cmd = NULL;
    char *mark_cmd = NULL;

    int nat_cmd_len = sizeof(char)*CMD_LENGTH;
    int mark_cmd_len = sizeof(char)*CMD_LENGTH;
    
    const char *rule_flush_cmd = "/usr/local/sbin/ip rule delete";
    const char *route_flush_cmd = "/usr/local/sbin/ip route flush";
    const char *mark_delete_cmd = "/usr/local/sbin/iptables -t mangle -D PREROUTING";
    const char *dnat_delete_cmd = "/usr/local/sbin/iptables -t nat -D PREROUTING ";
    const char *print_dev_cmd = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh print_dev";

    if(NULL == ifname){
        return -1;
    }

    nat_cmd = (char *)malloc(nat_cmd_len);
    if(NULL == nat_cmd){
        goto EXIT;
    }
    memset(nat_cmd,0,nat_cmd_len);

    mark_cmd = (char *)malloc(mark_cmd_len);
    if(NULL == mark_cmd){
        goto EXIT;
    }
    memset(mark_cmd,0,mark_cmd_len);

    snprintf(print_dev_str,sizeof(print_dev_str),"%s %s",print_dev_cmd,ifname);
    do_get_command(print_dev_str,dev,sizeof(dev));

    snprintf(nat_cmd,nat_cmd_len,"%s -i %s",dnat_delete_cmd,dev);
    snprintf(mark_cmd,mark_cmd_len,"%s -i %s",mark_delete_cmd,dev);
    if(strcmp(protocol,"tcp") == 0){
        strcat(nat_cmd," -p tcp");
    }
    else if(strcmp(protocol,"udp")== 0){
        strcat(nat_cmd," -p udp");
    }

    if(strlen(wan_ip) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," --dst %s",wan_ip);
    }
    else{
        get_interface_ip(ifname, ip_str, sizeof(ip_str));
        if(strlen(ip_str) == 0){
            goto EXIT;
        }
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," --dst %s",ip_str);
    }

    if(strlen(wan_port) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," -m multiport --dport %s",wan_port);
    }

    if((strlen(lan_ip) > 0) && strlen(lan_port) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," -j DNAT --to-destination %s:%s 2>/dev/null",lan_ip,lan_port);
    }

    snprintf(mark_cmd+strlen(mark_cmd),mark_cmd_len-strlen(mark_cmd),
        " -m state --state NEW -j CONNMARK --set-mark %d 2>/dev/null",fwmark);
    snprintf(rule_cmd,sizeof(rule_cmd),"%s fwmark %d table %d 2>/dev/null",rule_flush_cmd,fwmark,table);
    snprintf(route_cmd,sizeof(route_cmd),"%s table %d 2>/dev/null",route_flush_cmd,table);

    if(dnat_delete == 1){
        do_command(nat_cmd);
    }

    if(rule_delete == 1 && 
        (fwmark != 0) &&
        (table != 0) &&
        (strcmp(dev,"LAN") != 0)){
        do_command(rule_cmd);
        do_command(mark_cmd);
        do_command(route_cmd);
    }
#endif
    ret_value = 0;
EXIT:
    if(nat_cmd){
        free(nat_cmd);
        nat_cmd = NULL;
    }
     if(mark_cmd){
        free(mark_cmd);
        mark_cmd = NULL;
    }
	return ret_value;
}


int apply_add_dnat(int dnat_enable,int fwmark_enable,const char *ifname, const char *protocol,
    const char *wan_ip, const char *wan_port,const char *lan_ip, const char *lan_port,int fwmark,int table)
{
    int ret_value=-1;
    char dev[STRING_LENGTH] = {0};
    char gateway[STRING_LENGTH] = {0};
    char rule_cmd[CMD_LITTER_LENGTH] = {0};
//    char nat_cmd[CMD_LENGTH] = {0};
//    char mark_cmd[CMD_LENGTH] = {0};
    char route_cmd[CMD_LITTER_LENGTH] = {0};
    char print_dev_str[CMD_LITTER_LENGTH] = {0};
    char ip_str[STRING_LENGTH] = {0};

    char *nat_cmd = NULL;
    char *mark_cmd = NULL;

    int nat_cmd_len = sizeof(char)*CMD_LENGTH;
    int mark_cmd_len = sizeof(char)*CMD_LENGTH;
    
    const char *rule_add_cmd = "/usr/local/sbin/ip rule add";
    const char *route_add_cmd = "/usr/local/sbin/ip route add";
    const char *mark_add_cmd = "/usr/local/sbin/iptables -t mangle -A PREROUTING";
    const char *dnat_add_cmd = "/usr/local/sbin/iptables -t nat -A PREROUTING ";
    const char *print_dev_cmd = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh print_dev";
    const char *get_gateway_str = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_gateway_by_interface";

    if(NULL == ifname){
        goto EXIT;
    }

    nat_cmd = (char *)malloc(nat_cmd_len);
    if(NULL == nat_cmd){
        goto EXIT;
    }
    memset(nat_cmd,0,nat_cmd_len);

    mark_cmd = (char *)malloc(mark_cmd_len);
    if(NULL == mark_cmd){
        goto EXIT;
    }
    memset(mark_cmd,0,mark_cmd_len);

    snprintf(route_cmd,sizeof(route_cmd),"%s %s",get_gateway_str,ifname);
    do_get_command(route_cmd,gateway,sizeof(gateway));
    memset(route_cmd,0,sizeof(route_cmd));

    snprintf(print_dev_str,sizeof(print_dev_str),"%s %s",print_dev_cmd,ifname);
    do_get_command(print_dev_str,dev,sizeof(dev));

    snprintf(nat_cmd,nat_cmd_len,"%s -i %s",dnat_add_cmd,dev);
    snprintf(mark_cmd,mark_cmd_len,"%s -i %s",mark_add_cmd,dev);
    if(strcmp(protocol,"tcp") == 0){
        strcat(nat_cmd," -p tcp");
    }
    else if(strcmp(protocol,"udp")== 0){
        strcat(nat_cmd," -p udp");
    }

    if(strlen(wan_ip) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," --dst %s",wan_ip);
    }
    else{
        get_interface_ip(ifname, ip_str, sizeof(ip_str));
        if(strlen(ip_str) == 0){
            return -1;
        }
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," --dst %s",ip_str);
    }

    if(strlen(wan_port) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," -m multiport --dport %s",wan_port);
    }

    if((strlen(lan_ip) > 0) && strlen(lan_port) > 0){
        snprintf(nat_cmd+strlen(nat_cmd),nat_cmd_len-strlen(nat_cmd)," -j DNAT --to-destination %s:%s",lan_ip,lan_port);
    }

    snprintf(mark_cmd+strlen(mark_cmd),mark_cmd_len-strlen(mark_cmd),
        " -m state --state NEW -j CONNMARK --set-mark %d",fwmark);
    snprintf(rule_cmd,sizeof(rule_cmd),"%s fwmark %d table %d",rule_add_cmd,fwmark,table);
    snprintf(route_cmd,sizeof(route_cmd),"%s default via %s dev %s table %d",route_add_cmd,gateway,dev,table);

    if(dnat_enable == 1){
        do_command(nat_cmd);
    }

    if((fwmark_enable == 1) &&
        (fwmark != 0) &&
        (table != 0) &&
        (strcmp(dev,"LAN") != 0)){
        do_command(rule_cmd);
        do_command(mark_cmd);
        do_command(route_cmd);
    }

    ret_value = 0;
EXIT:
    if(nat_cmd){
        free(nat_cmd);
        nat_cmd = NULL;
    }
     if(mark_cmd){
        free(mark_cmd);
        mark_cmd = NULL;
    }
	return ret_value;
}

int apply_dnat()
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply dnat", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_snat()
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply snat", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_init_wans()
{
    int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply init_wan", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}


int apply_tables()
{
    int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply tables", script_path);
	ret_value = do_command(cmd);
	return ret_value;
}

int apply_macbind(void)
{
	int ret_value=0;
	char cmd[CMD_LITTER_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh apply macbind");
	ret_value = do_command(cmd);
	return ret_value;
}

int change_macbind(char *ip,char *mac,int action)
{
    int ret_value=0;
	 char cmd[CMD_LENGTH]={0};
        if(action == 1){
             snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud perm; /usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action drop", ip, mac,ip,mac);
        }else if(action == 0){
                    snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud reachable;/usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action accept", ip, mac,ip,mac);
        }else{
            ret_value = -1;
        }
        do_system_command(cmd);
	return ret_value;
}

int del_macbind(const int type, const char *ip, const char *mac)
{
	int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	if (type == 1){
	//    snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --del --ip %s --mac %s",ip,mac);
		snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh del %s dev LAN lladdr %s; /usr/local/sbin/mac-bind -b 0 --del --ip %s --mac %s", ip, mac,ip,mac);
	}else if (type == 2){
	//    snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -f");
		snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -f; /usr/local/sbin/ip neigh flush dev LAN nud perm");
	}else{
		ret_value=-1;
       }
       do_system_command(cmd);
	return ret_value;
}

int add_macbind(const char *ip, const char *mac, const int action)
{
	char cmd[CMD_LENGTH]={0};
	if (action == 0)
        snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud reachable;/usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action accept", ip, mac,ip,mac);
        //snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud perm; ", ip, mac);
		//snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action accept", ip, mac);
	else
        snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud perm; /usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action drop", ip, mac,ip,mac);
        //snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh replace %s dev LAN lladdr %s nud reachable", ip, mac);
		//snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --add --ip %s --mac %s --action drop", ip, mac);
	do_system_command(cmd);
	return 0;
}


void macbind_enbale_learnt_cmd(int learnt)
{
	char cmd[CMD_LITTER_LENGTH]={0};
	
	if (learnt == 1)
		snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --set --action accept");
	else
		snprintf(cmd, sizeof(cmd), "/usr/local/sbin/mac-bind -b 0 --set --action drop");

	do_system_command(cmd);
}

int print_host_conf(struct mgtcgi_xml_dhcpd_hosts *host, 
							const char *dhcpname, const int fd)
{
//	char buf[1024] = {0};
    char *buf = NULL;
	int index,retval=-1;
    int len = sizeof(char)*1024;
    
	if (host == NULL){
		goto EXIT;
	}

    buf = (char *)malloc(len);
    if(NULL == buf){
        goto EXIT;
    }
    memset(buf,0,len);
	
	for (index=0; index < host->num; index++){
		if (strcmp(dhcpname, host->pinfo[index].dhcpname) == 0){
			if (host->pinfo[index].enable == 0)
				continue;

			snprintf(buf, len, "\thost %s {\n", host->pinfo[index].name);
			write(fd, buf, strlen(buf));
			snprintf(buf, len, "\t\thardware ethernet %s;\n", host->pinfo[index].mac);
			write(fd, buf, strlen(buf));
			snprintf(buf, len, "\t\tfixed-address %s;\n", host->pinfo[index].ip);
			write(fd, buf, strlen(buf));
			if (strlen(host->pinfo[index].mask) >= 7){
				snprintf(buf, len, "\t\toption subnet-mask %s;\n", host->pinfo[index].mask);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].gateway) >= 7){
				snprintf(buf, len, "\t\toption routers %s;\n", host->pinfo[index].gateway);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].dnsname) >= 1){
				snprintf(buf, len, "\t\toption domain-name \"%s\";\n", host->pinfo[index].dnsname);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].dns) >= 7){
				snprintf(buf, len, "\t\toption routers %s;\n", host->pinfo[index].dns);
				write(fd, buf, strlen(buf));
			}
			if (host->pinfo[index].lease_time >= 3600){
				snprintf(buf, len, "\t\tdefault-lease-time %u;\n", host->pinfo[index].lease_time);
				write(fd, buf, strlen(buf));
			}
			if (host->pinfo[index].max_lease_time >= 3600){
				snprintf(buf, len, "\t\tmax-lease-time %u;\n", host->pinfo[index].max_lease_time);
				write(fd, buf, strlen(buf));
			}
			snprintf(buf, len, "\t}\n");
			write(fd, buf, strlen(buf));
		}
	}
    retval = 0;
    
EXIT:
    if(buf){
        free(buf);
        buf = NULL;
    }
	return retval;
}

int print_dhcpd_conf(struct mgtcgi_xml_dhcpd_servers *dhcpd, struct mgtcgi_xml_dhcpd_hosts*host,
								struct mgtcgi_xml_interface_lans *lan, const int fd)
{
	int index,i,j,flag,len,retval=-1;
	char if_ip[STRING_LENGTH]={0};
	char if_mask[STRING_LENGTH]={0};
	char subnet_addr[STRING_LENGTH]={0};
	char range_head[STRING_LENGTH]={0};
	char range_tail[STRING_LENGTH]={0};
//	char buf[1024] = {0};
    char *buf = NULL;
    int buf_len = sizeof(char)*1024;
    int fd_sys;
    char sys_info[100]={0};
    
	struct in_addr if_in_ip, if_in_mask, if_in_network;

	if ((dhcpd == NULL) || (host == NULL) || (lan == NULL)){
		goto EXIT;
	}

    buf = (char *)malloc(buf_len);
    if(NULL == buf){
        goto EXIT;
    }
    memset(buf,0,buf_len);

    
	for (index=0; index < dhcpd->num; index++){
		retval = -1;
		for (i=0; i < lan->num; i++){
			//»ñÈ¡Íø¿¨×ÓÍø¡¢ÑÚÂëµØÖ·
			if (strcmp(lan->pinfo[i].ifname, dhcpd->pinfo[index].ifname) == 0){
				strncpy(if_ip, lan->pinfo[i].ip, STRING_LENGTH);
				strncpy(if_mask, lan->pinfo[i].mask, STRING_LENGTH);

				if (inet_aton(if_ip, &if_in_ip) == 0){//´íÎó·µ»Ø0
					goto EXIT;
				}
				if (inet_aton(if_mask, &if_in_mask) == 0){
					goto EXIT;
				}
				if_in_network.s_addr = (unsigned long int)(if_in_ip.s_addr & if_in_mask.s_addr);
				len = strlen((char *)inet_ntoa(if_in_network));
				memcpy(subnet_addr, (char *)inet_ntoa(if_in_network), len);
				subnet_addr[len] = '\0';
				
				retval = 0;
				break;
			}
		}
		if (retval != 0){
			goto EXIT;
		}
        
        snprintf(sys_info,sizeof(sys_info),"%s ",dhcpd->pinfo[index].ifname);
        
		snprintf(buf, buf_len, "##%s\n", dhcpd->pinfo[index].name);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "subnet %s netmask %s {\n", subnet_addr, if_mask);
		write(fd, buf, strlen(buf));

		memset(range_head, 0, sizeof(range_head));
		memset(range_tail, 0, sizeof(range_tail));
		for (i=0,j=0,flag=0; i < strlen(dhcpd->pinfo[index].range); i++){
			if (dhcpd->pinfo[index].range[i] == '-'){
				flag = 1;
				continue;
			}
			if (flag == 0)
				range_head[i] = dhcpd->pinfo[index].range[i];
			else{
				range_tail[j] = dhcpd->pinfo[index].range[i];
				j++;
			}
		}
		
		snprintf(buf, buf_len, "\trange %s %s;\n", range_head, range_tail);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\toption subnet-mask %s;\n", dhcpd->pinfo[index].mask);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\toption routers %s;\n", dhcpd->pinfo[index].gateway);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\toption domain-name \"%s\";\n", dhcpd->pinfo[index].dnsname);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\toption domain-name-servers %s;\n", dhcpd->pinfo[index].dns);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\tdefault-lease-time %u;\n", dhcpd->pinfo[index].lease_time);
		write(fd, buf, strlen(buf));
		snprintf(buf, buf_len, "\tmax-lease-time %u;\n", dhcpd->pinfo[index].max_lease_time);
		write(fd, buf, strlen(buf));

		//¾²Ì¬DHCP
		retval = print_host_conf(host, dhcpd->pinfo[index].name, fd);
		
		snprintf(buf, buf_len, "}\n");
		write(fd, buf, strlen(buf));

		if (retval != 0){
			retval = -2;
			goto EXIT;
		}
	}

    sys_info[strlen(sys_info)-1] = '"';
    
    fd_sys = open("/etc/sysconfig/dhcpd", O_CREAT | O_TRUNC | O_WRONLY, 0755);
    write(fd_sys, "DHCPDARGS=\"",strlen("DHCPDARGS=\""));
    
    write(fd_sys, sys_info, strlen(sys_info));
    close(fd_sys);

    retval = 0;
EXIT:
    if(buf){
        free(buf);
        buf = NULL;
    }
	return retval;
}

int print_dnsmasq_resolv_conf(struct mgtcgi_xml_dhcpd_servers *dhcpd, const int fd)
{
    int retval = -1,i;
    char buffer[STRING_LENGTH] = {0};
    char *p = NULL;
    char *point = NULL;
    struct in_addr iton;
    
    if(!dhcpd){
        goto EXIT;
    }

    for(i = 0; i< dhcpd->num;i++){
        p = dhcpd->pinfo[i].dns;

        while(p){
            while((point = strsep(&p,",")) != NULL){
                if(inet_aton(point,&iton) != 0){
                    snprintf(buffer,sizeof(buffer),"nameserver %s\n",point);
                    write(fd,buffer,strlen(buffer));
                }
            }
        }
    }

    snprintf(buffer,sizeof(buffer),"nameserver 127.0.0.1\n");
    write(fd,buffer,strlen(buffer));

    retval = 0;
EXIT:
    return retval;
}

int print_pptpd_option(char *dns,char *entry,int fd)
{
    int sign = 0;
    char dot[2] = ",";
    char buf[CMD_LITTER_LENGTH] = {0};
    char *p,*ptr;
    struct in_addr ips;
    
    if(fd < 0){
        return -1;
    }

    if(!dns || !entry){
        return -1;
    }

    write(fd,buf,0);
    snprintf(buf,sizeof(buf),"name pptpd\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"proxyarp\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"lock\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"nobsdcomp\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"novj\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"novjccomp\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"nologfd\n");
    write(fd,buf,strlen(buf));

    if(strncmp(entry,"pap",3) == 0){
        snprintf(buf,sizeof(buf),"require-pap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"require-chap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"require-mschap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"require-mschap-v2\n");
        write(fd,buf,strlen(buf));
    }
    else if(strncmp(entry,"mpp",3) == 0){
        snprintf(buf,sizeof(buf),"refuse-pap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"refuse-chap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"refuse-mschap\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"require-mschap-v2\n");
        write(fd,buf,strlen(buf));
        snprintf(buf,sizeof(buf),"require-mppe-128\n");
        write(fd,buf,strlen(buf));
    }
    else{
        return -1;
    }

    p = dns;
    while(p){
        while((ptr = strsep(&p,dot)) != NULL){
            if(inet_aton(ptr,&ips) == 0){
                return -1;
            }
            snprintf(buf,sizeof(buf),"ms-dns %s\n",ptr);
            write(fd,buf,strlen(buf));
            sign = 1;
        }
    }

    if(sign == 0){
        snprintf(buf,sizeof(buf),"ms-dns 114.114.114.114\n");
        write(fd,buf,strlen(buf));
    }

    return 0;
}

int print_pptpd_conf(const char *localip,const char *startip,const char *endip,int fd)
{
    char dot = '.';
    char ip_pool[STRING_LENGTH] = {0};
    char buf[CMD_LITTER_LENGTH] = {0};
    char *ptr = NULL;
    struct in_addr ips;
    
    if(fd < 0){
        return -1;
    }

    if(!localip || !startip || !endip){
        return -1;
    }

    if((inet_aton(localip,&ips) == 0)
        || (inet_aton(startip,&ips) == 0)
        || (inet_aton(endip,&ips) == 0)){
        return -1;
    }

    if((ptr = strrchr(endip,dot)) == NULL){
        return -1;
    }

    snprintf(ip_pool,sizeof(ip_pool),"%s-%s",startip,(ptr+1));

    write(fd,buf,0);
    snprintf(buf,sizeof(buf),"ppp /usr/local/sbin/pppd\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"option /etc/ppp/options.pptpd\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"connection 100\n");
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"localip %s\n",localip);
    write(fd,buf,strlen(buf));
    snprintf(buf,sizeof(buf),"remoteip %s\n",ip_pool);
    write(fd,buf,strlen(buf));

    return 0;
}

int print_lan_dhcp_conf(struct mgtcgi_xml_interface_lan_dhcp_info *dhcpd, const char *ifname, const int fd)
{
    int retval = -1;
    char buffer[CMD_LITTER_LENGTH] = {0};
    char *name = "dhcp";
    int resolv_fd;
    
    if (dhcpd == NULL || ifname == NULL){
		goto EXIT;
	}

    resolv_fd = open(DHCP_RESOLV_CONFPATH, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if(resolv_fd < 0){
        goto EXIT;
    }

    write(fd,buffer,0);
    snprintf(buffer,sizeof(buffer),"listen-address=127.0.0.1\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"bind-interfaces\n");
    write(fd,buffer,strlen(buffer));

    snprintf(buffer,sizeof(buffer),"dhcp-range=\"%s\",%s,%s,%us\n",
        name,dhcpd->startip,dhcpd->endip,dhcpd->lease_time);
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"interface=%s\n",ifname);
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"listen-address=%s\n",dhcpd->gateway);
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-option=\"%s\",3,%s\n",name,dhcpd->gateway);
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-option=\"%s\",15,\"%s\"\n",name,name);
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"resolv-file=/etc/dnsmasq.resolv.conf\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"cache-size=10000\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-leasefile=/var/lib/dnsmasq.leases\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"no-hosts\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-lease-max=10000\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"#log-facility=/var/log/dnsmasq.log\n");
    write(fd,buffer,strlen(buffer));

    memset(buffer,0,sizeof(buffer));
    write(resolv_fd,buffer,strlen(buffer));
    if(strlen(dhcpd->first_dns) > 0){
        snprintf(buffer,sizeof(buffer),"nameserver %s\n",dhcpd->first_dns);
        write(resolv_fd,buffer,strlen(buffer));
    }
    if(strlen(dhcpd->second_dns) > 0){
        snprintf(buffer,sizeof(buffer),"nameserver %s\n",dhcpd->second_dns);
        write(resolv_fd,buffer,strlen(buffer));
    }
    snprintf(buffer,sizeof(buffer),"nameserver 127.0.0.1\n");
    write(resolv_fd,buffer,strlen(buffer));
    
    close(resolv_fd);
    retval = 0;
EXIT:
    return retval;
}

int print_dnsmasq_dhcpd_conf(struct mgtcgi_xml_dhcpd_servers *dhcpd, struct mgtcgi_xml_dhcpd_hosts*host,
								struct mgtcgi_xml_interface_lans *lan, const int fd)
{
    int retval = -1,i;
    char buffer[CMD_LITTER_LENGTH] = {0};
    char *p = NULL;
    
    if ((dhcpd == NULL) || (host == NULL) || (lan == NULL)){
		goto EXIT;
	}

    write(fd,buffer,0);
    snprintf(buffer,sizeof(buffer),"listen-address=127.0.0.1\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"bind-interfaces\n");
    write(fd,buffer,strlen(buffer));

    for(i = 0; i< dhcpd->num; i++){
        p = strchr(dhcpd->pinfo[i].range,'-');
        if(NULL == p){
            continue;
        }
        *p = ',';
        snprintf(buffer,sizeof(buffer),"dhcp-range=\"%s\",%s,%s,%us\n",
            dhcpd->pinfo[i].name,dhcpd->pinfo[i].range,dhcpd->pinfo[i].mask,dhcpd->pinfo[i].max_lease_time);
        write(fd,buffer,strlen(buffer));
        snprintf(buffer,sizeof(buffer),"interface=%s\n",dhcpd->pinfo[i].ifname);
        write(fd,buffer,strlen(buffer));
        snprintf(buffer,sizeof(buffer),"listen-address=%s\n",dhcpd->pinfo[i].gateway);
        write(fd,buffer,strlen(buffer));
        snprintf(buffer,sizeof(buffer),"dhcp-option=\"%s\",3,%s\n",dhcpd->pinfo[i].name,dhcpd->pinfo[i].gateway);
        write(fd,buffer,strlen(buffer));
        if(strlen(dhcpd->pinfo[i].dnsname) > 0){
            snprintf(buffer,sizeof(buffer),"dhcp-option=\"%s\",15,\"%s\"\n",dhcpd->pinfo[i].name,dhcpd->pinfo[i].dnsname);
            write(fd,buffer,strlen(buffer));
        }
    }

    snprintf(buffer,sizeof(buffer),"resolv-file=/etc/dnsmasq.resolv.conf\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"cache-size=10000\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-leasefile=/var/lib/dnsmasq.leases\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"no-hosts\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"dhcp-lease-max=10000\n");
    write(fd,buffer,strlen(buffer));
    snprintf(buffer,sizeof(buffer),"#log-facility=/var/log/dnsmasq.log\n");
    write(fd,buffer,strlen(buffer));

    retval = 0;
EXIT:
    return retval;
}


int printwaninfo(const char *name,int flag)
{
    char link_str[STRING_LENGTH] = {0};
    char mac_str[STRING_LENGTH] = {0};
  //  char ip_str[STRING_LENGTH] = {0};
   // char mask_str[STRING_LENGTH] = {0};

    char link_cmd[CMD_LITTER_LENGTH] = {0};
    char mac_cmd[CMD_LITTER_LENGTH] = {0};
  //  char ip_cmd[CMD_LENGTH] = {0};
  //  char mask_cmd[CMD_LENGTH] = {0};
    
    if(name == NULL){
        return -1;
    }
    
    if(0 == flag)
        return 0;

    snprintf(link_cmd,sizeof(link_cmd),"/sbin/ethtool %s |grep \"Link detected\" |awk -F ':' '{print $2}' |awk '{print $1}'",name);
    snprintf(mac_cmd,sizeof(mac_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"link/ether\" |awk '{print $2}'",name);
 //   snprintf(ip_cmd,sizeof(ip_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"inet\" |awk '{print $2}' |awk -F '/' '{print $1}'",name);
 //   snprintf(mask_cmd,sizeof(mask_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"inet\" |awk '{print $2}' |awk -F '/' '{print $2}'",name);

    do_get_command(link_cmd,link_str, sizeof(link_str));
    do_get_command(mac_cmd,mac_str, sizeof(mac_str));
 //   do_get_command(ip_cmd,ip_str, sizeof(ip_str));
  //  do_get_command(mask_cmd,mask_str, sizeof(mask_str));

  //  mask_bit = atoi(mask_str);

    printf("{\"ifname\":\"%s\",\"mac\":\"%s\",\"status\":\"%s\",\"ip\":\"\",\"mask\":\"\",\"gateway\":\"\",\"weight\":\"1\",\"defaultroute\":0}",
        name,mac_str,link_str);

    return 0;
}

int editwaninfo(const char *name,int flag)
{
    char link_str[STRING_LENGTH] = {0};
    char mac_str[STRING_LENGTH] = {0};
  //  char ip_str[STRING_LENGTH] = {0};
   // char mask_str[STRING_LENGTH] = {0};

    char link_cmd[CMD_LITTER_LENGTH] = {0};
    char mac_cmd[CMD_LITTER_LENGTH] = {0};
  //  char ip_cmd[CMD_LENGTH] = {0};
  //  char mask_cmd[CMD_LENGTH] = {0};
    
    if(name == NULL){
        return -1;
    }
    
    if(0 == flag)
        return 0;

    snprintf(link_cmd,sizeof(link_cmd),"/sbin/ethtool %s |grep \"Link detected\" |awk -F ':' '{print $2}' |awk '{print $1}'",name);
    snprintf(mac_cmd,sizeof(mac_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"link/ether\" |awk '{print $2}'",name);
 //   snprintf(ip_cmd,sizeof(ip_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"inet\" |awk '{print $2}' |awk -F '/' '{print $1}'",name);
 //   snprintf(mask_cmd,sizeof(mask_cmd),"/usr/local/sbin/ip addr show dev %s |grep \"inet\" |awk '{print $2}' |awk -F '/' '{print $2}'",name);

    do_get_command(link_cmd,link_str, sizeof(link_str));
    do_get_command(mac_cmd,mac_str, sizeof(mac_str));
 //   do_get_command(ip_cmd,ip_str, sizeof(ip_str));
  //  do_get_command(mask_cmd,mask_str, sizeof(mask_str));

  //  mask_bit = atoi(mask_str);

    printf("{\"ifname\":\"%s\",\"mac\":\"%s\",\"status\":\"%s\",\"access\":\"static\",\"ip\":\"\",\"mask\":\"\",\"gateway\":\"\",\"weight\":\"1\",\"defaultroute\":0,\"comment\":\"\"}",
        name,mac_str,link_str);

    return 0;
}

int apply_pptpd_vpn(int enable, char *localip, char *startip, char *endip,
                char *dns, char *entry)
{
    int iserror = -1,confd,opfd,retal;
    char dot = '.';
    char ip_pool[STRING_LENGTH] = {0};
    char conf_path[STRING_LENGTH] = "/etc/ppp/pptpd.conf";
    char option_path[STRING_LENGTH] = "/etc/ppp/options.pptpd";
    struct in_addr ips;
    char *ptr = NULL;
    
    if(!localip || !startip || !endip || !entry || !dns){
        return -1;
    }

    if((inet_aton(localip,&ips) == 0)
        || (inet_aton(startip,&ips) == 0)
        || (inet_aton(endip,&ips) == 0)
        || (strlen(entry) == 0)){
        return -1;
    }

    if((ptr = strrchr(endip,dot)) == NULL){
        return -1;
    }

    snprintf(ip_pool,sizeof(ip_pool),"%s-%s",startip,(ptr+1));

    confd = open(conf_path, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if(confd < 0){
        goto EXIT;
    }

    opfd = open(option_path, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if(opfd < 0){
        goto EXIT;
    }

    retal = print_pptpd_conf(localip,startip,endip,confd);
    if(retal != 0){
        goto EXIT;
    }

    retal = print_pptpd_option(dns,entry,opfd);
    if(retal != 0){
        goto EXIT;
    }

    if(enable == 0){
        system("sync;sync;killall pptpd >/dev/null 2>&1");
    }
    else{
        system("sync;sync;killall pptpd >/dev/null 2>&1;/usr/local/sbin/pptpd -c /etc/ppp/pptpd.conf >/dev/null 2>&1");
    }
    iserror = 0;

EXIT:
    close(confd);
    close(opfd);
    return iserror;
}

int apply_dnsmasq_dhcpd(void)
{
    int size,iserror=-1;
    int conf_retval,resolv_retval;
    int conf_fd,resolv_fd;
    char buf[CMD_LITTER_LENGTH] = {0};
    char conf_path[STRING_LENGTH] = "/etc/dnsmasq.conf";
    char resolv_path[STRING_LENGTH] = "/etc/dnsmasq.resolv.conf";
    struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
    struct mgtcgi_xml_dhcpd_hosts *host = NULL;
    struct mgtcgi_xml_interface_lans *lan = NULL;

    set_reuid();
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
    if (NULL == dhcpd){
        iserror = MGTCGI_READ_FILE_ERR;
        goto EXIT;
    }
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
    if (NULL == host){
        iserror = MGTCGI_READ_FILE_ERR;
        goto EXIT;
    }
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
    if (NULL == lan){
        goto EXIT;
    }

    conf_fd = open(conf_path, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if (conf_fd < 0){
        goto EXIT;
    }

    resolv_fd = open(resolv_path, O_CREAT | O_TRUNC | O_WRONLY, 666);
    if (resolv_fd < 0){
        goto EXIT;
    }
    
    conf_retval = print_dnsmasq_dhcpd_conf( dhcpd, host, lan, conf_fd);
    close(conf_fd);

    resolv_retval = print_dnsmasq_resolv_conf(dhcpd, resolv_fd);
    close(resolv_fd);
    
    if (conf_retval == 0 && resolv_retval == 0){
        memset(buf, 0, sizeof(buf));
        snprintf(buf,sizeof(buf),"sync;sync;chmod 666 %s >/dev/null 2>&1;chmod 666 %s >/dev/null 2>&1",conf_path,resolv_path);
        system(buf);
        
        memset(buf, 0, sizeof(buf));
        if (dhcpd->dhcpd_enable == 1 && dhcpd->num > 0)
            snprintf(buf, sizeof(buf), "sync;sync;killall -9 dnsmasq >/dev/null 2>&1;/usr/local/sbin/dnsmasq >/dev/null 2>&1 &");
        else
            snprintf(buf, sizeof(buf), "killall -9 dnsmasq >/dev/null 2>&1");
        system(buf);
    }

    iserror = 0;
EXIT:
    if(dhcpd){
        free_xml_node((void*)&dhcpd);
        dhcpd = NULL;
    }
    if(host){
        free_xml_node((void *)&host);
        host = NULL;
    }
    if(lan){
        free_xml_node((void*)&lan);
        lan = NULL;
    }

    return iserror;

}

#if 1
int apply_dhcpd()
{
	int size,retval=0;
	int fd;
	char buf[CMD_LITTER_LENGTH] = {0};
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;

	set_reuid();
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
	if (NULL == dhcpd){
		retval = MGTCGI_READ_FILE_ERR;
		goto EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_HOSTS, (void**)&host, &size);
	if (NULL == host){
		retval = MGTCGI_READ_FILE_ERR;
		goto EXIT;
	}
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	if (NULL == lan){
		retval = -1;
		goto EXIT;
	}

	fd = open("/etc/dhcpd.conf", O_CREAT | O_TRUNC | O_WRONLY, 0755);
	if (fd < 0){
		retval = -1;
		goto EXIT;
	}
	
	snprintf(buf, sizeof(buf), "ddns-update-style none;\n");
	write(fd, buf, strlen(buf));
	
	retval = print_dhcpd_conf( dhcpd, host, lan, fd);
	
	//if (retval == 0){
		memset(buf, 0, sizeof(buf));
		if (dhcpd->dhcpd_enable == 1)
			snprintf(buf, sizeof(buf), "sync;sync;killall -9 dhcpd >/dev/null 2>&1;/usr/local/sbin/dhcpd >/dev/null 2>&1");
		else
			snprintf(buf, sizeof(buf), "killall -9 dhcpd >/dev/null 2>&1");
		system(buf);
	//}
	
EXIT:
	close(fd);
    if(dhcpd){
	    free_xml_node((void*)&dhcpd);
        dhcpd = NULL;
    }
    if(host){
	    free_xml_node((void *)&host);
        host = NULL;
    }
    if(lan){
	    free_xml_node((void*)&lan);
        lan = NULL;
    }

	return retval;
}
#endif

struct allocate_rate{
    int minrate;
    int maxrate;
    int prec;
};

struct allocate_rate allocall[32][2] ={{{0,0,0},{0,0,0}},{{500,5000,50},{3000,5000,50}},{{300,1000,40},{2500,7000,30}},
{{200,600,10},{1000,8000,20}},{{200,600,10},{1000,40000,30}},{{200,800,10},{1000,60000,30}},{{250,600,10},{1000,7000,30}},
{{400,1800,20},{1000,5000,30}},{{400,1800,20},{1000,5000,30}},{{300,3000,30},{1000,7000,20}},{{300,3000,20},{1000,7000,20}}};

int init_allocat_rate()
{
    allocall[18][0].minrate = 300;
    allocall[18][0].maxrate = 1500;
    allocall[18][0].prec = 20;
    allocall[18][1].minrate = 1000;
    allocall[18][1].maxrate = 7000;
    allocall[18][1].prec = 20;
    return 0;
}


static int print_rate_by_catid(const int dir,const int classtype,const unsigned int id,const char *rate_str)
{
    int rate = 0;
    int pipe_rate = 1000000;
    
    if(!rate_str)
        return 0;

    if(!(id > 0 && id <= 30))
        return 50;
    
    init_allocat_rate();
    rate = atoi(rate_str);
    if(classtype == 0){
        return rate;
    }
    if(classtype == 1){
        return pipe_rate;
    }
    rate = rate * allocall[id][dir].prec /100;
    if(rate < allocall[id][dir].minrate)
        rate = allocall[id][dir].minrate;
    if(rate > allocall[id][dir].maxrate)
        rate = allocall[id][dir].maxrate;
    return rate;
}


#if 0
static int print_rate_by_catid(const int dir_t,const int classtype,const unsigned int id,const char *rate_str)
{
    int rate = 0, dir = 1;
    int pipe_rate = 1000000,rate_tmp = 0;
    
    if(!rate_str)
        return 0;

    if(!(id > 0 && id <= 30))
        return 50;

    rate = atoi(rate_str);
    if(dir_t == 0){
        dir = 0;
    }
    
    if(classtype == 0){
        return rate;
    }
    if(classtype == 1){
        return pipe_rate;
    }
    if(classtype == 2){
        if(id == 1){
            return (dir?5000:3000);
        }
        else if(id == 2){
            return (dir?5000:800);
        }
        else if(id == 3){
            return (dir?5000:600);
        }
        else if(id == 4){
            if(dir == 1){
                if((rate/3000) > 0){
                    rate_tmp = (rate/3000)*1000+500;
                }
                else{
                    rate_tmp = 500;
                }
            }
            return (dir?rate_tmp:500);
        }
        else if(id == 5){
            if(dir == 1){
                if((rate/2000) > 0){
                    rate_tmp = (rate/2000)*1000+500;
                }
                else{
                    rate_tmp = 500;
                }
            }
            return (dir?rate_tmp:600);
        }
        else if(id == 6){
            return (dir?6000:600);
        }
        else if(id == 7){
            if(dir == 1){
                if((rate/3000) > 0){
                    rate_tmp = (rate/3000)*1000+500;
                }
                else{
                    rate_tmp = 500;
                }
            }
            return (dir?rate_tmp:4000);
        }
        else if(id == 8){
            return (dir?1500:1200);
        }
        else if(id == 9){
            
            return (dir?5000:800);
        }
        else if(id == 18){
            if(dir == 1){
                if((rate/3000) > 0){
                    rate_tmp = (rate/3000)*1000+500;
                }
                else{
                    rate_tmp = 500;
                }
            }
            return (dir?rate_tmp:1800);
        }
    }

    return 50;
}
#endif
static int print_weight_and_prio_by_catid(const unsigned int id, int *weight, int *prio)
{
    if(!(id > 0 && id <= 30))
        return -1;
#if 0    
    if(!classname)
        return -1;
  
    strcpy(str,classname);
    for(i=0;i<strlen(str);i++){
        c = str[i];
        if(c >= 'A' && c <= 'Z'){
            str[i] = c+32;
        }
    }
#endif

    if(id == 1){
        *weight = 100;
        *prio = 1;
    }
    else if(id == 2){
        *weight = 80;
        *prio = 2;
    }
    else if(id == 3){
        *weight = 15;
        *prio = 9;
    }
    else if(id == 4){
        *weight = 12;
        *prio = 9;
    }
    else if(id == 5){
        *weight = 15;
        *prio = 7;
    }
    else if(id == 6){
        *weight = 15;
        *prio = 7;
    }
    else if(id == 7){
        *weight = 20;
        *prio = 6;
    }
    else if(id == 8){
        *weight = 20;
        *prio = 5;
    }
    else if(id == 9){
        *weight = 20;
        *prio = 5;
    }
    else if(id == 18){
        *weight = 20;
        *prio = 7;
    }

    return 0;
}

int init_wan_channel(int index, const char *up, const char *down)
{
    int retal=-1,iserror=-1,old_channelsize=0,size,i,iIndex=0;
    int default_num = 0,old_num=0,new_num=0,lcalsize = 0;
    int weight = 0, prio = 0;
    char share_name[LITTER_LENGTH] = {0};
    char pipe_name[LITTER_LENGTH] = {0};
    char classname[LITTER_LENGTH] = {0};
    const char *pipe_rate = "1000Mbps";
    const char *default_name = "default";
    struct mgtcgi_xml_wans_channels *old_wanchannel = NULL;
    struct mgtcgi_xml_wans_channels *new_wanchannel = NULL;
    struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
    
    if(!((index > 0) && (index < 31)))
        return retal;
    
    if(!up || !down)
        return retal;

    sprintf(share_name,"share%d",index);
    sprintf(pipe_name,"pipe%d",index);
    
    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&old_wanchannel, &old_channelsize);
    if (NULL == old_wanchannel){
        retal = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    retal = -1;
    for(i=0;i<old_wanchannel->num;i++){
        if(index == old_wanchannel->pinfo[i].id){
            retal = 0;
            break;
        }
    }
    if(retal == 0){
        iserror = MGTCGI_USED_ERR;
        goto ERROR_EXIT;
    }

    get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7TYPE_SHOWS,
						"chinese",(void**)&l7protocol,&size);
    if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    
    default_num = (l7protocol->num + 3)*2;
    old_num = old_wanchannel->num;

    new_num = old_num + default_num;
    lcalsize = sizeof(struct mgtcgi_xml_wans_channels) +
            new_num * (sizeof(struct mgtcgi_xml_wans_channel_info));
    
    new_wanchannel = (struct mgtcgi_xml_wans_channels *)malloc(lcalsize);
	if (NULL == new_wanchannel){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    memset(new_wanchannel, 0, lcalsize);
    memcpy(new_wanchannel, old_wanchannel, old_channelsize);
    new_wanchannel->num = new_num;
    
    //share channel
    iIndex = old_num;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 0;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 0;
    new_wanchannel->pinfo[iIndex].classid = index;
    new_wanchannel->pinfo[iIndex].parentid = 0;
    new_wanchannel->pinfo[iIndex].shareid = 0;
    new_wanchannel->pinfo[iIndex].priority = 0;
    new_wanchannel->pinfo[iIndex].limit = 0;
    new_wanchannel->pinfo[iIndex].attribute = 0;
    new_wanchannel->pinfo[iIndex].weight = 0;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",share_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%sKbps",up);
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,share_name);

    iIndex++;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 1;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 0;
    new_wanchannel->pinfo[iIndex].classid = index;
    new_wanchannel->pinfo[iIndex].parentid = 0;
    new_wanchannel->pinfo[iIndex].shareid = 0;
    new_wanchannel->pinfo[iIndex].priority = 0;
    new_wanchannel->pinfo[iIndex].limit = 0;
    new_wanchannel->pinfo[iIndex].attribute = 0;
    new_wanchannel->pinfo[iIndex].weight = 0;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",share_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%sKbps",down);
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,share_name);

    //pipe channel
    iIndex++;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 0;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 1;
    new_wanchannel->pinfo[iIndex].classid = index * 64;
    new_wanchannel->pinfo[iIndex].parentid = 0;
    new_wanchannel->pinfo[iIndex].shareid = 0;
    new_wanchannel->pinfo[iIndex].priority = 0;
    new_wanchannel->pinfo[iIndex].limit = 0;
    new_wanchannel->pinfo[iIndex].attribute = 0;
    new_wanchannel->pinfo[iIndex].weight = 0;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",pipe_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,pipe_rate);
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,pipe_name);

    iIndex++;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 1;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 1;
    new_wanchannel->pinfo[iIndex].classid = index * 64;
    new_wanchannel->pinfo[iIndex].parentid = 0;
    new_wanchannel->pinfo[iIndex].shareid = 0;
    new_wanchannel->pinfo[iIndex].priority = 0;
    new_wanchannel->pinfo[iIndex].limit = 0;
    new_wanchannel->pinfo[iIndex].attribute = 0;
    new_wanchannel->pinfo[iIndex].weight = 0;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",pipe_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,pipe_rate);
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,pipe_name);
    
    //default channel
    iIndex++;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 0;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 2;
    new_wanchannel->pinfo[iIndex].classid = (index*64+10);
    new_wanchannel->pinfo[iIndex].parentid = index*64;
    new_wanchannel->pinfo[iIndex].shareid = index;
    new_wanchannel->pinfo[iIndex].priority = 7;
    new_wanchannel->pinfo[iIndex].limit = 1000;
    new_wanchannel->pinfo[iIndex].attribute = 1;
    new_wanchannel->pinfo[iIndex].weight = 20;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",default_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%dKbps",print_rate_by_catid(0,2,10,up));
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,default_name);

    iIndex++;
    new_wanchannel->pinfo[iIndex].id = index;
    new_wanchannel->pinfo[iIndex].direct = 1;
    new_wanchannel->pinfo[iIndex].enable = 1;
    new_wanchannel->pinfo[iIndex].classtype = 2;
    new_wanchannel->pinfo[iIndex].classid = (index*64+10);
    new_wanchannel->pinfo[iIndex].parentid = index*64;
    new_wanchannel->pinfo[iIndex].shareid = index;
    new_wanchannel->pinfo[iIndex].priority = 7;
    new_wanchannel->pinfo[iIndex].limit = 1000;
    new_wanchannel->pinfo[iIndex].attribute = 1;
    new_wanchannel->pinfo[iIndex].weight = 20;
    snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",default_name);
    snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%dKbps",print_rate_by_catid(1,2,10,down));
    snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,default_name);

    iIndex++;
    for(i = 0; (iIndex < new_num) && (i < l7protocol->num); i++){
        strncpy(classname,l7protocol->pinfo[i].name,LITTER_LENGTH-1);
        print_weight_and_prio_by_catid(l7protocol->pinfo[i].gid,&weight,&prio);
        
        new_wanchannel->pinfo[iIndex].id = index;
        new_wanchannel->pinfo[iIndex].direct = 0;
        new_wanchannel->pinfo[iIndex].enable = 1;
        new_wanchannel->pinfo[iIndex].classtype = 2;
        new_wanchannel->pinfo[iIndex].classid = (index*64+l7protocol->pinfo[i].gid);
        new_wanchannel->pinfo[iIndex].parentid = index*64;
        new_wanchannel->pinfo[iIndex].shareid = index;
        new_wanchannel->pinfo[iIndex].priority = prio;
        new_wanchannel->pinfo[iIndex].limit = 1000;
        new_wanchannel->pinfo[iIndex].attribute = 1;
        new_wanchannel->pinfo[iIndex].weight = weight;
        snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",classname);
        snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%dKbps",
                print_rate_by_catid(0,2,l7protocol->pinfo[i].gid,up));
        snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,l7protocol->pinfo[i].match);
        iIndex++;
        
        new_wanchannel->pinfo[iIndex].id = index;
        new_wanchannel->pinfo[iIndex].direct = 1;
        new_wanchannel->pinfo[iIndex].enable = 1;
        new_wanchannel->pinfo[iIndex].classtype = 2;
        new_wanchannel->pinfo[iIndex].classid = (index*64+l7protocol->pinfo[i].gid);
        new_wanchannel->pinfo[iIndex].parentid = index*64;
        new_wanchannel->pinfo[iIndex].shareid = index;
        new_wanchannel->pinfo[iIndex].priority = prio;
        new_wanchannel->pinfo[iIndex].limit = 1000;
        new_wanchannel->pinfo[iIndex].attribute = 1;
        new_wanchannel->pinfo[iIndex].weight = weight;
        snprintf(new_wanchannel->pinfo[iIndex].classname,LITTER_STRING-1,"%s",classname);
        snprintf(new_wanchannel->pinfo[iIndex].rate,LITTER_STRING-1,"%dKbps",
                print_rate_by_catid(1,2,l7protocol->pinfo[i].gid,down));
        snprintf(new_wanchannel->pinfo[iIndex].comment,LITTER_LENGTH-1,l7protocol->pinfo[i].match);
        iIndex++;

        memset(classname,0,sizeof(classname));
        weight = 0;
        prio = 0;
        
    }

    retal = save_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void*)new_wanchannel, lcalsize);
    if (retal < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 

    iserror = 0;
ERROR_EXIT:
    if(old_wanchannel){
        free_xml_node((void*)&old_wanchannel);
    }
    if(new_wanchannel){
        free_xml_node((void*)&new_wanchannel);
    }
    if(l7protocol){
        free_xml_node((void*)&l7protocol);
    }
    return iserror;
}

int remove_wan_channel(const int id)
{
    int iserror=-1,retal=-1,size,index,new_index=0;
    int old_num,new_num,lClasize,count=0;
    struct mgtcgi_xml_wans_channels *old_wanchannel = NULL;
    struct mgtcgi_xml_wans_channels *new_wanchannel = NULL;
    
    if(!(id > 0 && id < 31))
        return -1;

    get_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void**)&old_wanchannel, &size);
    if (NULL == old_wanchannel){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    old_num = old_wanchannel->num;
    for(index = 0; index < old_num; index++){
        if(id == old_wanchannel->pinfo[index].id)
            count++;
    }

    if(count > old_num){
        new_num = 0;
    }
    else{
        new_num = old_num - count;
    }

    lClasize = sizeof(struct mgtcgi_xml_wans_channels) +
            new_num * (sizeof(struct mgtcgi_xml_wans_channel_info));
    new_wanchannel = (struct mgtcgi_xml_wans_channels *)malloc(lClasize);
    if(new_wanchannel == NULL){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto ERROR_EXIT;
    }
    memset(new_wanchannel, 0, lClasize);

    iserror = -1;
    new_wanchannel->num = new_num;
    for(index = 0; index < old_wanchannel->num; index++){
        if(id == old_wanchannel->pinfo[index].id){
            iserror = 0;
            continue;
        }

        new_wanchannel->pinfo[new_index].id = old_wanchannel->pinfo[index].id;
        new_wanchannel->pinfo[new_index].direct = old_wanchannel->pinfo[index].direct;
        new_wanchannel->pinfo[new_index].enable = old_wanchannel->pinfo[index].enable;
        new_wanchannel->pinfo[new_index].classtype = old_wanchannel->pinfo[index].classtype;
        new_wanchannel->pinfo[new_index].classid = old_wanchannel->pinfo[index].classid;
        new_wanchannel->pinfo[new_index].parentid = old_wanchannel->pinfo[index].parentid;
        new_wanchannel->pinfo[new_index].shareid = old_wanchannel->pinfo[index].shareid;
        new_wanchannel->pinfo[new_index].priority = old_wanchannel->pinfo[index].priority;
        new_wanchannel->pinfo[new_index].limit = old_wanchannel->pinfo[index].limit;
        new_wanchannel->pinfo[new_index].attribute = old_wanchannel->pinfo[index].attribute;
        new_wanchannel->pinfo[new_index].weight = old_wanchannel->pinfo[index].weight;
        strncpy(new_wanchannel->pinfo[new_index].classname,old_wanchannel->pinfo[index].classname,LITTER_STRING-1);
        strncpy(new_wanchannel->pinfo[new_index].rate,old_wanchannel->pinfo[index].rate,LITTER_STRING-1);
        strncpy(new_wanchannel->pinfo[new_index].comment,old_wanchannel->pinfo[index].comment,LITTER_LENGTH-1);
        new_index++;
    }

    if(iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
    }

    retal = save_route_xml_node(WAN_CHANNEL_FILE, MGTCGI_TYPE_WAN_CHANNEL, (void*)new_wanchannel, lClasize);
    if (retal < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	} 
    
    iserror = 0;
ERROR_EXIT:
    if(old_wanchannel){
        free_xml_node((void*)&old_wanchannel);
    }
    if(new_wanchannel){
        free_xml_node((void*)&new_wanchannel);
    }

    return iserror;
}

int apply_wan_channel(const char *cmd, int id, const char *up, const char *down)
{
    int     iserror=-1,retal=-1;

    if(!cmd)
        return iserror;

    if(strcmp(cmd,"init") == 0){
        retal = init_wan_channel(id,up,down);
        if(retal != 0){
            goto ERROR_EXIT;
        }
    }
    else if(strcmp(cmd,"remove") == 0){
        retal = remove_wan_channel(id);
        if(retal != 0){
            goto ERROR_EXIT;
        }
    }
    else if(strcmp(cmd,"apply") == 0){
        retal = 0;
    }
    else{
        goto ERROR_EXIT;
    }

    if(retal == 0){
        retal = apply_channel();
        if(retal != 0){
            goto ERROR_EXIT;
        }
    }

    iserror = 0;
ERROR_EXIT:
    return iserror;
}

void get_link_status(const char *ifname, char *link_status)
{
	int  ret_value = -1;
	char cmd[CMD_LITTER_LENGTH]={0};
	
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list link %s", script_path, ifname);

	ret_value = do_command(cmd);
	if (ret_value == 0)
		strcpy(link_status, "yes");
	else
		strcpy(link_status, "no");
	
	return;
}

void get_vlan_status(const char *ifname, char *link_status)
{
	char	value[STRING_LENGTH] = {0};
	char cmd[CMD_LITTER_LENGTH] = {0};
	int	ret_value;
	
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list vlan %s", script_path, ifname);

	ret_value = do_get_command(cmd,value,STRING_LENGTH);
	if (ret_value == 0)
	{
		if(strncmp(value,"yes",3) == 0)
		{
			strcpy(link_status, "yes");
		}
		else
		{
			strcpy(link_status, "no");
		}
	}
		
	return;
}
void get_link_macaddr(const char *ifname, char *macaddr)
{
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;
	
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list macaddr %s", script_path, ifname);
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(macaddr, LITTER_LENGTH - 1, pp);
		macaddr[strlen(macaddr)-1] = '\0';
	}
	pclose(pp);
	return;
}

void get_dhcp_addr(const char *ifname, char *ip, char *mask, char *gateway)
{
	int i=0;
	char *dot=",";
	char *ptr;
	char strbuf[CMD_LENGTH]={0};
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list dhcp %s 2>/dev/null", script_path, ifname);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, sizeof(strbuf), pp);
		strbuf[strlen(strbuf)-1] = 0;
	}
	pclose(pp);

	ptr=strtok(strbuf, dot);
	if (NULL != ptr){
		strcpy(ip, ptr);
		i++;
	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			if (i == 1){
				strcpy(mask, ptr);
			}
			else if (i == 2){
				strcpy(gateway, ptr);
			} 
			else{
				break;
			}
			i++;
		} else {
			break;
		}
	}
}

void get_adslclient_status(const char *name,int *status)
{
      char cmd[STRING_LENGTH]={""};
      char ststua_str[STRING_LENGTH]={""};
      char pppoe_status_file_path[STRING_LENGTH]={"0"};

      snprintf(pppoe_status_file_path,sizeof(pppoe_status_file_path),"/var/run/%s.conf-pppoe.pid.status",name);
      snprintf(cmd,sizeof(cmd),"cat %s 2>/dev/null",pppoe_status_file_path);

      do_get_command(cmd, ststua_str, sizeof(ststua_str));
      *status = atoi(ststua_str);

      return;
}

void get_adslclient_link(const char *name, char *status, char *ip, char *gateway, char *uptime)
{
	int i=0;
	char *dot=",";
	char *ptr;
	char strbuf[CMD_LENGTH]={0};
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list adsl %s", script_path, name);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, sizeof(strbuf), pp);
		strbuf[strlen(strbuf)-1] = 0;
	}
	pclose(pp);

	ptr=strtok(strbuf, dot);
	if (NULL != ptr){
		strcpy(status, ptr);
		i++;
	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			if (i == 1){
				strcpy(ip, ptr);
			}
			else if (i == 2){
				strcpy(gateway, ptr);
			} 
			else if (i == 3){
				strcpy(uptime, ptr);
			}
			i++;
		} else {
			break;
		}
	}

}


void get_l2tpvpnclient_link(const char *name, char *status, char *ip, char *gateway, char *uptime)
{
	int i=0;
	char *dot=",";
	char *ptr;
	char strbuf[CMD_LENGTH]={0};
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list l2tpvpn %s", script_path, name);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, sizeof(strbuf), pp);
		strbuf[strlen(strbuf)-1] = 0;
	}
	pclose(pp);

	ptr=strtok(strbuf, dot);
	if (NULL != ptr){
		strcpy(status, ptr);
		i++;
	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			if (i == 1){
				strcpy(ip, ptr);
			}
			else if (i == 2){
				strcpy(gateway, ptr);
			} 
			else if (i == 3){
				strcpy(uptime, ptr);
			}
			i++;
		} else {
			break;
		}
	}

}

int get_dhcpd_status(void)
{
	int  retval = -1;
	char buffer[DIGITAL_LENGTH]={0};
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;
	
	snprintf(cmd, sizeof(cmd), "ps -w 2>/dev/null | grep dnsmasq | grep -v grep | wc -l");
	
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		retval = atoi(buffer);
	}
	pclose(pp);
	return retval;
}

int get_pptpd_status(void)
{
	int  retval = -1;
	char buffer[DIGITAL_LENGTH]={0};
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;
	
	snprintf(cmd, sizeof(cmd), "ps -w 2>/dev/null | grep pptpd | grep -v grep | wc -l");
	
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		retval = atoi(buffer);
	}
	pclose(pp);
	return retval;
}


int change_defaultroute(void)
{
#if 0
	struct 		mgtcgi_xml_interface_wans *wan = NULL;
	struct 		mgtcgi_xml_adsl_clients *adsl = NULL;
	struct 		mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct 		mgtcgi_xml_interface_vlans *vlan = NULL;
	char			ifname[STRING_LENGTH] = {0};
	char			cmd[CMD_LITTER_LENGTH] = {0};
	int			iserror = -1,size,index,ret_value;

	snprintf(cmd,sizeof(cmd),"/usr/local/sbin/defroutectl -d");
	do_get_command(cmd,ifname,sizeof(ifname));

	if(strcmp(ifname,"") == 0)
	{
		iserror = 0;
		return iserror;
	}

	if(strncmp(ifname,"WAN",3)== 0)
	{
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
		if (NULL == wan)
		{
			iserror = MGTCGI_READ_FILE_ERR;
			goto FREE_EXIT;
		}

		for (index=0; index<wan->num; index++)
		{
			if (wan->pinfo[index].defaultroute == 1)
			{
				wan->pinfo[index].defaultroute = 0;
				break;
			}
		}

		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wan, size);
		if (ret_value < 0)
		{
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto FREE_EXIT;
		}
	}
	else if(strncmp(ifname,"ads",3) == 0)
	{
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
		if (NULL == adsl)
		{
			iserror = MGTCGI_READ_FILE_ERR;
			goto FREE_EXIT;
		}

		for (index=0; index<adsl->num; index++)
		{
			if (adsl->pinfo[index].defaultroute == 1)
			{
				adsl->pinfo[index].defaultroute = 0;
				break;
			}
		}

		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)adsl, size);
		if (ret_value < 0)
		{
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto FREE_EXIT;
		}
	}
	else if(strncmp(ifname,"l2t",3) == 0)
	{
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
		if (NULL == l2tpvpn)
		{
			iserror = MGTCGI_READ_FILE_ERR;
			goto FREE_EXIT;
		}

		for (index=0; index<l2tpvpn->num; index++)
		{
			if (l2tpvpn->pinfo[index].defaultroute == 1)
			{
				l2tpvpn->pinfo[index].defaultroute = 0;
				break;
			}
		}

		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)l2tpvpn, size);
		if (ret_value < 0)
		{
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto FREE_EXIT;
		}
	}
	else if(strncmp(ifname,"vla",3) == 0 || strncmp(ifname,"vet",3) == 0)
	{
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
		if (NULL == vlan)
		{
			iserror = MGTCGI_READ_FILE_ERR;
			goto FREE_EXIT;
		}
		
		for (index=0; index<vlan->num; index++)
		{
			if (vlan->pinfo[index].defaultroute == 1)
			{
				vlan->pinfo[index].defaultroute = 0;

				ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void*)vlan, size);
				if (ret_value < 0)
				{
					iserror = MGTCGI_WRITE_FILE_ERR;
					goto FREE_EXIT;
				}
				break;
			}
		}
	}

	iserror = 0;
FREE_EXIT:
	free_xml_node((void*)&wan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&vlan);
	return iserror;
#endif
    return 0;
}

int check_ppp_ifname(const char *ifname, char *ifdev)
{
	int i,index,size,retval=0;
	char buff[STRING_LENGTH]={0};
	struct mgtcgi_xml_interface_vlans *vlan = NULL;

	memset(ifdev, 0, STRING_LENGTH);
	memset(buff, 0, sizeof(buff));
	for (i=0; i < strlen(ifname); i++){
		if (i >= 3){
			buff[i] = '\0';
			break;
		}
		else{
			buff[i] = ifname[i];
		}
	}
	if (strcmp(buff, "vla") == 0 || strcmp(buff, "VLA") == 0){
		get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&vlan, &size);
		if (NULL == vlan){
			retval = 1;
			goto EXIT;
		}
		for(index=0; index < vlan->num; index++){
			if (strcmp(vlan->pinfo[index].name, ifname) == 0){
				snprintf(ifdev, STRING_LENGTH, "%s.%d", vlan->pinfo[index].ifname, vlan->pinfo[index].id);
				break;
			}
		}
		if (index >= vlan->num)
			retval = 2;
		else
			retval = 0;
	}
	else{
		strncpy(ifdev, ifname, STRING_LENGTH);
	}
EXIT:
	free_xml_node((void **)&vlan);
	return retval;
}

void print_routing_valid()
{
	//char strbuf[BUF_LENGTH]={0};
	char *strbuf =NULL;
    
	char cmd[CMD_LITTER_LENGTH]={0};
	FILE *pp = NULL;
    strbuf = (char *) malloc(BUF_LENGTH*sizeof(char));
    if(!strbuf)
        return;
    memset(strbuf,0,BUF_LENGTH*sizeof(char));
    
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list routing", script_path);
    pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, BUF_LENGTH*sizeof(char), pp);
		strbuf[strlen(strbuf) - 1] = 0;
	}
	pclose(pp);
	printf("%s",strbuf);

    if(strbuf){
        free(strbuf);
        strbuf = NULL;
    }
}

int divid_ip(char *ipstr, char *ip, char *mask)
{
    char *maskp = NULL;

    maskp = strchr(ipstr, '/');
    if (maskp == NULL)
    {
        return (-1);
    }

    *maskp = '\0';
    maskp++;
    strncpy(ip, ipstr, STRING_LENGTH);
    strncpy(mask, maskp, STRING_LENGTH);
    return 0;
}

int mask_to_maskbit(char *mask_str, uint8_t *maskbit)
{
    struct in_addr inaddr;
    uint32_t mask = 0;
    unsigned char *maskp = NULL;
    uint8_t my_maskbit = 0, mask_part = 0;
    int i;

    if (inet_aton(mask_str, &inaddr) == 0)
    {
        return (-1);
    }

    mask = inaddr.s_addr;
    maskp = (unsigned char *)&mask;
    
    for (i=0; i<4; i++)
    {        
        if (maskp[i] != 255)
        {
            mask_part = maskp[i];
            break;
        }

        my_maskbit += 8;
    }

    if (i < 4)
    {
        while(1)
        {
            if (mask_part != 0)
            {
                my_maskbit += 1;
                mask_part = mask_part << 1;
            }
            else
            {
                break;
            }
        }
    }

    *maskbit = my_maskbit;
    return 0;
}

int maskbit_to_mask(uint8_t bit, char *mask_str, int mask_str_len)
{
    uint32_t mask = 0xffffffff;
    unsigned char *maskp = NULL;

    mask = mask << (32-bit);
    mask = htonl(mask);
    maskp = (unsigned char *)&mask;
    snprintf(mask_str, mask_str_len, "%u.%u.%u.%u", 
        maskp[0], maskp[1], maskp[2], maskp[3]);
    return 0;    
}

