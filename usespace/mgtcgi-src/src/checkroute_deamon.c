#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include <arpa/inet.h>


#include "../include/route_xmlwrapper4c.h"

const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

void init_daemon(void);

void delay(int i)//delay function for io ports operation taking effect
{
        int j=0;
        for(j=0;j<i;j++)
        {
             usleep(1000000);                 //here, 100 ms is enough
        }
        
}

int get_running_status(const char *name)
{
	char 		buf[128];
	FILE 		*pp;
	char cmd[CMD_LENGTH] = {0};
	int	 runs = 0;

	snprintf(cmd, sizeof(cmd), "ps -w | grep %s | grep -v grep | wc -l", name);
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buf, sizeof(buf), pp);
		runs = atoi(buf);
	}
	pclose(pp);
	if (runs > 1) {
		printf ("%s is running, exit!\n", name);
		exit (1);
		
	} else {
		return 0;
	}
}

int __adsl_client_dial(const char *type, const char *name, const char *ifname)
{
	int ret_value = -1;
	char cmd[CMD_LENGTH]={0};
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh adsl-dial %s %s %s", script_path, type, name, ifname);
	system(cmd);
	return ret_value;
}


int __check_ppp_ifname(const char *ifname, char *ifdev)
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
	//free_xml_node((void*)&vlan);
	if(vlan)
	    free(vlan);
	return retval;
}

/********************************* command **********************************/
void check_adsl_dial(void)
{
	int iserror=0,index=0,size=0;
	char ifdev[STRING_LENGTH]={0};
	char ifname[STRING_LENGTH]={0};
	struct mgtcgi_xml_adsl_clients *adsl = NULL;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
    
	if (NULL == adsl){
		iserror = MGTCGI_READ_FILE_ERR;
		goto FREE_EXIT;
	}

	for (index=0; index<adsl->num; index++){
        if(adsl->pinfo[index].enable == 0)
            continue;

        strcpy(ifname, adsl->pinfo[index].ifname);
        if (__check_ppp_ifname(ifname, ifdev) != 0){
            continue;
		}
        __adsl_client_dial("check", adsl->pinfo[index].name, ifdev);
        delay(2);
	}

FREE_EXIT:
	//free_xml_node((void*)&adsl);
	if(adsl)
        free(adsl);
	return; 
}

int check_dhcp_dial(void)
{
	struct mgtcgi_xml_interface_wans 	*wan = NULL;
	char								cmd[CMD_LENGTH] = {0};
	int								size,dhcp = 0,i,defaultroute;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		dhcp = MGTCGI_READ_FILE_ERR;
		goto FREE_EXIT;
	}

	for(i = 0; i < wan->num; i++)
	{
		defaultroute = wan->pinfo[i].defaultroute;
		if(strcmp(wan->pinfo[i].access,"dhcp") == 0)
		{
			//get_link_status(wan->pinfo[i].ifname,status);
			snprintf(cmd,sizeof(cmd),"%s/route_script.sh dhcp_dial %s %s %d",script_path,wan->pinfo[i].ifname,"yes",defaultroute);
			system(cmd);
		}
	}

FREE_EXIT:
	if(wan)
	{
		free(wan);
	}
	return dhcp;
}

int check_xl2tpd_dial(void)
{
	char cmd[CMD_LENGTH]={0};
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	int retval=0,index=0,size=0;
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		retval = -1;
		goto FREE_EXIT;
	}
	
	for (index=0; index < l2tpvpn->num; index++){
		if (l2tpvpn->pinfo[index].enable == 0)
			snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh xl2tpd_check_dail down %s", l2tpvpn->pinfo[index].name);
		else
			snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh xl2tpd_check_dail up %s", l2tpvpn->pinfo[index].name);

		system(cmd);
		delay(2);
	}
	
FREE_EXIT:
    if(l2tpvpn)
        free(l2tpvpn);
	//free_xml_node((void *)&l2tpvpn);
	return retval;
}

void get_adslclient_link(const char *name, char *status, char *ip, char *gateway, char *uptime)
{
	int i=0;
	char *dot=",";
	char *ptr;
	char strbuf[CMD_LENGTH]={0};
	char cmd[CMD_LENGTH]={0};
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

int route_print_host_conf(struct mgtcgi_xml_dhcpd_hosts *host, 
							const char *dhcpname, const int fd)
{
	char buf[1024] = {0};
	int index,retval=0;
	if (host == NULL){
		retval = -1;
		goto EXIT;
	}
	
	for (index=0; index < host->num; index++){
		if (strcmp(dhcpname, host->pinfo[index].dhcpname) == 0){
			if (host->pinfo[index].enable == 0)
				continue;

			snprintf(buf, sizeof(buf), "\thost %s {\n", host->pinfo[index].name);
			write(fd, buf, strlen(buf));
			snprintf(buf, sizeof(buf), "\t\thardware ethernet %s;\n", host->pinfo[index].mac);
			write(fd, buf, strlen(buf));
			snprintf(buf, sizeof(buf), "\t\tfixed-address %s;\n", host->pinfo[index].ip);
			write(fd, buf, strlen(buf));
			if (strlen(host->pinfo[index].mask) >= 7){
				snprintf(buf, sizeof(buf), "\t\toption subnet-mask %s;\n", host->pinfo[index].mask);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].gateway) >= 7){
				snprintf(buf, sizeof(buf), "\t\toption routers %s;\n", host->pinfo[index].gateway);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].dnsname) >= 1){
				snprintf(buf, sizeof(buf), "\t\toption domain-name \"%s\";\n", host->pinfo[index].dnsname);
				write(fd, buf, strlen(buf));
			}
			if (strlen(host->pinfo[index].dns) >= 7){
				snprintf(buf, sizeof(buf), "\t\toption routers %s;\n", host->pinfo[index].dns);
				write(fd, buf, strlen(buf));
			}
			if (host->pinfo[index].lease_time >= 3600){
				snprintf(buf, sizeof(buf), "\t\tdefault-lease-time %u;\n", host->pinfo[index].lease_time);
				write(fd, buf, strlen(buf));
			}
			if (host->pinfo[index].max_lease_time >= 3600){
				snprintf(buf, sizeof(buf), "\t\tmax-lease-time %u;\n", host->pinfo[index].max_lease_time);
				write(fd, buf, strlen(buf));
			}
			snprintf(buf, sizeof(buf), "\t}\n");
			write(fd, buf, strlen(buf));
		}
	}
EXIT:
	return retval;
}


int route_print_dhcpd_conf(struct mgtcgi_xml_dhcpd_servers *dhcpd, struct mgtcgi_xml_dhcpd_hosts*host,
								struct mgtcgi_xml_interface_lans *lan, const int fd)
{
	int index,i,j,flag,len,retval;
	char if_ip[STRING_LENGTH]={0};
	char if_mask[STRING_LENGTH]={0};
	char subnet_addr[STRING_LENGTH]={0};
	char range_head[STRING_LENGTH]={0};
	char range_tail[STRING_LENGTH]={0};
	char buf[1024] = {0};
    int fd_sys;
    char sys_info[100]={0};
	struct in_addr if_in_ip, if_in_mask, if_in_network;

	if ((dhcpd == NULL) || (host == NULL) || (lan == NULL)){
		retval = -1;
		goto EXIT;
	}
            
	for (index=0; index < dhcpd->num; index++){
		retval = -1;
		for (i=0; i < lan->num; i++){
			//获取网卡子网、掩码地址
			if (strcmp(lan->pinfo[i].ifname, dhcpd->pinfo[index].ifname) == 0){
				strncpy(if_ip, lan->pinfo[i].ip, STRING_LENGTH);
				strncpy(if_mask, lan->pinfo[i].mask, STRING_LENGTH);

				if (inet_aton(if_ip, &if_in_ip) == 0){//错误返回0
					retval = -1;
					goto EXIT;
				}
				if (inet_aton(if_mask, &if_in_mask) == 0){
					retval = -1;
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
			retval = -1;
			goto EXIT;
		}
        snprintf(sys_info,sizeof(sys_info),"%s ",dhcpd->pinfo[index].ifname);
		snprintf(buf, sizeof(buf), "##%s\n", dhcpd->pinfo[index].name);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "subnet %s netmask %s {\n", subnet_addr, if_mask);
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
		
		snprintf(buf, sizeof(buf), "\trange %s %s;\n", range_head, range_tail);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\toption subnet-mask %s;\n", dhcpd->pinfo[index].mask);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\toption routers %s;\n", dhcpd->pinfo[index].gateway);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\toption domain-name \"%s\";\n", dhcpd->pinfo[index].dnsname);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\toption domain-name-servers %s;\n", dhcpd->pinfo[index].dns);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\tdefault-lease-time %u;\n", dhcpd->pinfo[index].lease_time);
		write(fd, buf, strlen(buf));
		snprintf(buf, sizeof(buf), "\tmax-lease-time %u;\n", dhcpd->pinfo[index].max_lease_time);
		write(fd, buf, strlen(buf));

		//静态DHCP
		retval = route_print_host_conf(host, dhcpd->pinfo[index].name, fd);
		
		snprintf(buf, sizeof(buf), "}\n");
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
EXIT:
	return retval;
}

int route_apply_dhcpd()
{
	int size,retval=0;
	int fd;
	char buf[1024] = {0};
	struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;
	struct mgtcgi_xml_dhcpd_hosts *host = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	
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
	retval = route_print_dhcpd_conf(dhcpd, host, lan, fd);
	if (retval == 0){
		memset(buf, 0, sizeof(buf));
		//if (dhcpd->dhcpd_enable == 1)
			//snprintf(buf, sizeof(buf), "killall -9 dhcpd >/dev/null 2>&1; /usr/local/sbin/dhcpd >/dev/null 2>&1");
			snprintf(buf, sizeof(buf), "sync;sync;/usr/local/sbin/dhcpd >/dev/null 2>&1");
		//else
		//	snprintf(buf, sizeof(buf), "killall -9 dhcpd >/dev/null 2>&1");
		system(buf);
	}
	
EXIT:
	close(fd);
    if(dhcpd)
        free(dhcpd);
    if(host)
        free(host);
    if(lan)
        free(lan);
	return retval;
}

int main(int argc, char **argv ) 
{ 
	unsigned int second = 30;
	char cmd[CMD_LENGTH] = {0};
#if 1
	char buffer[CMD_LENGTH] = {0};
	FILE *pp;
#endif	
	if ( argc != 2 ){
		//printf ("Uage:%s SleepTime\n",argv[0]);
		second = 20;		//默认20s检测一次路由
	}
	else{
		second = atoi(argv[1]);
		if (second > 300)
			second = 300;
		else if (second < 10)
			second=10;
	}
	
	get_running_status(argv[0]);
	init_daemon();	
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh apply routing >/dev/null 2>&1");
	system(cmd);
    
#if 1
    snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute status=0,%s", buffer);
    pp = popen(cmd, "r");
	if (pp != NULL){
		memset(buffer, 0 ,sizeof(buffer));
		fgets(buffer, sizeof(buffer), pp);
	}
	pclose(pp);
#endif

    //delay(2);
    //route_apply_dhcpd();
    check_dhcp_dial();

	while (1)
	{
		delay(second);
		//xl2tpd掉线重拨
		check_xl2tpd_dial();
        check_adsl_dial();
        
		//如果路由发生变化，刷新路由表
#if 1
		snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute status=1,%s", buffer);
		//snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh checkroute status=1");
		pp = popen(cmd, "r");
		if (pp != NULL){
			memset(buffer, 0 ,sizeof(buffer));
			fgets(buffer, sizeof(buffer), pp);
		}
		pclose(pp);
#endif
	}
}

