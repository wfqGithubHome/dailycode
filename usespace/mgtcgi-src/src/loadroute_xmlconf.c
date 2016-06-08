#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/xmlwrapper4c.h"
#include "../include/route_xmlwrapper4c.h"

extern int apply_dns();
extern int check_ppp_ifname(const char *ifname, char *ifdev);
//extern int apply_adsl(int type, char *olduser, char *oldpasswd, char *name, char *ifname, char *username, char *password, char *servername, int mtu, int defaultroute, int enable, int peerdns);
//extern int apply_l2tpvpn(char *oldvpn, char *name, char *ifname, char  *username, char *password, char *servername, int mtu, int defaultroute, int enable, int peerdns);
//extern int apply_l2tpvpn(char *oldvpn, char *name, char *ifname, char  *username, char *password, char *servername, int mtu, int defaultroute, int enable, int peerdns);
//extern int l2tpvpn_client_dial(char * type, char * name);
extern int apply_routing();
extern int apply_dnat();
extern int apply_routing();
extern int apply_macbind(void);
//extern int apply_dhcpd();
//extern int apply_dnsmasq_dhcpd(void);
//extern int copy_route_xmlconf_file(const char * srcfile, const char * dstfile);
extern int apply_ddns();
extern int apply_vlan();
extern int apply_wan();
extern int apply_lan();
extern int xl2tpd_restart();
extern int apply_snat();
extern int apply_tables();
extern int apply_init_wans();
int apply_macvlan(const int index);


#if 0
/******************************** route xml config init *****************************/
int route_dns_init(int init)
{
	int  ret_value=0,size;
	struct mgtcgi_xml_dns *dns = NULL;

	if (init == 0)
		return 0;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void**)&dns, &size);
	if (NULL == dns){
		ret_value = -1;
		goto FREE_EXIT;
	}
	ret_value = apply_dns(dns->dns_primary, dns->dns_secondary, dns->dns_thirdary);
	
	FREE_EXIT:
		free_xml_node((void*)&dns);
		return ret_value;
}
#endif

int route_ddns_init(int init)
{
	int  ret_value=0;
	if (init == 0)
		return 0;


	ret_value = apply_ddns();
	return ret_value;
}


int route_iflan_init(int init)
{
	int	ret_value=0;
	if (init == 0)
		return 0;

	ret_value = apply_lan();
	return ret_value;
}

int route_ifwan_init(int init)
{
	int ret_value=0;
	if (init == 0)
		return 0;
	
    ret_value = apply_init_wans();
	return ret_value;
}


int route_vlan_init(int init)
{
	int ret_value=0;
    ret_value = apply_vlan(0);
	return ret_value;
}

int route_macvlan_init(int init)
{
	int ret_value=0;
    ret_value = apply_macvlan(0);
	return ret_value;
}

extern int apply_pppd_auth();

int route_pppdauth_init(int init)
{
	int ret_value=0;
	ret_value = apply_pppd_auth();
	return ret_value;
}


#if 0
int route_adslclient_init(int init)
{
	int index=0,size=0,ret_value=0,type=0;
	char ifdev[STRING_LENGTH]={0};
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	
	if (init == 0)
		return 0;
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		ret_value = -1;
		goto FREE_EXIT;
	}

	type = 0;
	for (index=0; index < adsl->num; index++){
		if (check_ppp_ifname(adsl->pinfo[index].ifname, ifdev) != 0){
			continue;
		}
		ret_value = apply_adsl(type, adsl->pinfo[index].username, adsl->pinfo[index].password, adsl->pinfo[index].name, 
					ifdev, adsl->pinfo[index].username, adsl->pinfo[index].password, adsl->pinfo[index].servername, 
					adsl->pinfo[index].mtu, adsl->pinfo[index].defaultroute, adsl->pinfo[index].enable, adsl->pinfo[index].peerdns);
		type=1;
	}

FREE_EXIT:
	free_xml_node((void*)&adsl);
	return ret_value;
}

int route_vpnclient_init(int init)
{
	int index=0,size=0,ret_value=0;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;

	if (init == 0)
		return 0;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	if (NULL == l2tpvpn){
		ret_value = -1;
		goto FREE_EXIT;
	}
	
	for (index=0; index < l2tpvpn->num; index++){
		ret_value = apply_l2tpvpn(l2tpvpn->pinfo[index].name, l2tpvpn->pinfo[index].name, l2tpvpn->pinfo[index].ifname,
				l2tpvpn->pinfo[index].username, l2tpvpn->pinfo[index].password, l2tpvpn->pinfo[index].servername,
				l2tpvpn->pinfo[index].mtu, l2tpvpn->pinfo[index].defaultroute, l2tpvpn->pinfo[index].enable, l2tpvpn->pinfo[index].peerdns);
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

FREE_EXIT:
	free_xml_node((void*)&l2tpvpn);
	return ret_value;
}
#endif

int route_routing_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;
	
	ret_value = apply_routing();
	return ret_value;
}

int route_dnat_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;
	
	ret_value = apply_dnat();
	return ret_value;
}

int route_tables_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;
    ret_value = apply_tables();
    return ret_value;
}

int route_snat_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;

	ret_value = apply_snat();
	return ret_value;
}

int route_macbind_init(int init)
{

	int ret_value=0;

	if (init == 0)
		return 0;

	ret_value = apply_macbind();
	return ret_value;
}

#if 0
int route_dhcpd_dnsmasq_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;
    
	system("mkdir /var/lib 2>/dev/null;touch /var/lib/dnsmasq.leases 2>/dev/null");
	
	ret_value = apply_dnsmasq_dhcpd();
	return ret_value;

}
#endif

#if 0
int route_dhcpd_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;
    
	system("mkdir /var/db 2>/dev/null;touch /var/db/dhcpd.leases 2>/dev/null");
	
	ret_value = apply_dhcpd();
	return ret_value;
}
#endif

int route_ipup_init(int init)
{
	int ret_value=0;
	int wfd=0;
	char *filename = "/etc/ppp/ip-up";
	char buffer[]={"#!/bin/sh\n/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh apply routing\nexit 0\n"};

	if (init == 0)
		return 0;
	
	wfd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0755);
	if (wfd < 0){
		ret_value = -1;
		goto EXIT;
	}
	if (write(wfd, buffer, sizeof(buffer))<0){
		ret_value = -2;
		goto EXIT;
	}

	EXIT:
		close(wfd);
		return ret_value;
}

int route_ipdown_init(int init)
{
	int ret_value=0;

	if (init == 0)
		return 0;

	return ret_value;
}

int main(void)
{
//	int result=0;
//	char * flash_route_xmlconf = "/flash/etc/route_config.xml";
//	char * working_route_xmlconf = "/etc/route_config.xml";
	
//	result = copy_route_xmlconf_file(flash_route_xmlconf, working_route_xmlconf);
//	if (result != 0){
//		printf("copy route_config.xml file error.\n");
//		return -1;
//	}
	//route_dns_init(1);

    route_vlan_init(1);
    route_macvlan_init(1);
    route_pppdauth_init(1);
	
    route_iflan_init(1);
	route_ifwan_init(1);

	route_dnat_init(1);
	route_snat_init(1);
	route_tables_init(1);
	//route_adslclient_init(1);
	//route_vpnclient_init(1);
	route_routing_init(1);
	route_macbind_init(1);
	//route_dhcpd_init(1);
    //route_dhcpd_dnsmasq_init(1);
    xl2tpd_restart();
//	route_ipup_init(1);
	route_ipdown_init(1);
	route_ddns_init(1);
    return 0;
}

