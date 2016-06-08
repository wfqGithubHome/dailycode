#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/route_xmlwrapper4c.h"
#include "../include/xmlwrapper4c.h"

int get_default_route_devname()
{
	int index=0,size=0,ret_value;
	char default_ifname[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_interface_vlans *vlan = NULL;
	
	//return: -1 error; 1 LAN; 2 WAN; 3 adsl; 4 l2tpvpn; 5 balance;
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

	memset(default_ifname, 0, sizeof(default_ifname));
	if (wan->num <= 0)
		goto FREE_EXIT;

	for (index=0; index<wan->num; index++){
		if (wan->pinfo[index].defaultroute == 1){
			strcpy(default_ifname, wan->pinfo[index].ifname);
			goto FREE_EXIT;
		}
	}
	
	for (index=0; index<adsl->num; index++){
		if (adsl->pinfo[index].defaultroute == 1){
			strcpy(default_ifname, adsl->pinfo[index].name);
			goto FREE_EXIT;
		}
	}
	
	for (index=0; index<wan->num; index++){
		if (l2tpvpn->pinfo[index].defaultroute == 1){
			strcpy(default_ifname, l2tpvpn->pinfo[index].name);
			goto FREE_EXIT;
		}
	}

	for (index=0; index<vlan->num; index++){
		if (vlan->pinfo[index].defaultroute == 1){
			strcpy(default_ifname, vlan->pinfo[index].name);
			goto FREE_EXIT;
		}
	}
	
FREE_EXIT:
	free_xml_node((void*)&wan);
	free_xml_node((void*)&adsl);
	free_xml_node((void*)&l2tpvpn);
	free_xml_node((void*)&vlan);
	printf("%s\n", default_ifname);
	return 0;
}


void get_all_wan_interface_name(void)
{
	int index=0,size=0,ret_value;
	struct mgtcgi_xml_interface_wans *wan = NULL;

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		ret_value = -1;
		goto FREE_EXIT;
	}

	for (index=0; index<wan->num; index++){
		printf("%s ", wan->pinfo[index].ifname);
		
	}
	
FREE_EXIT:
	free_xml_node((void*)&wan);
    return;
}


void get_default_route_gateway(const char *ifname){
	int index=0,size=0,ret_value;
	char gateway[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_interface_wans *wan = NULL;
	
	memset(gateway, 0, sizeof(gateway));
	if (ifname == NULL){
		goto FREE_EXIT;
	}
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		ret_value = -1;
		goto FREE_EXIT;
	}

	for (index=0; index<wan->num; index++){
		if (strcmp(wan->pinfo[index].ifname, ifname) == 0){
			strcpy(gateway, wan->pinfo[index].gateway);
			goto FREE_EXIT;
		}
	}

FREE_EXIT:
	free_xml_node((void*)&wan);
	printf("%s\n", gateway);
    return;
}

void print_usage(const char *name)
{
	printf("Usage: %s [options]\n", name);
	printf("The options are:\n");
	printf("-d, --def		<TOGGLE>	Get default route interface name.\n");
	printf("-w, --wan		<TOGGLE>	print all WAN interface name.\n");
	printf("-g, --gateway		<STRING>    Get	default gateway ip.\n");
}

int main(int argc, char **argv)
{
	int retval=0;
	
	if (argc < 2){
		print_usage(argv[0]);
		retval = -1;
		goto EXIT;
	}
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
		print_usage(argv[0]);
		retval = -1;
		goto EXIT;
	}
	if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--def") == 0){
		get_default_route_devname();
	}
	else if (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "--wan") == 0){
		get_all_wan_interface_name();
	}
	else if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--gateway") == 0){
		if (argc != 3){
			print_usage(argv[0]);
			retval = -1;
			goto EXIT;
		}
		else{
			get_default_route_gateway(argv[2]);
		}
	}

	EXIT:
		return retval;
}


