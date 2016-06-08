#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

char * flash_route_xmlconf = "/flash/etc/route_config.xml";
char * working_route_xmlconf = "/etc/route_config.xml";

extern int get_route_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size);
extern int remove_adsl_link_and_conf(char *name, char *username, char *password);
extern int save_route_xml_node(const char *filename, const int nodetype, const void * buf, const int buflen);
extern int free_xml_node(void ** buf);
extern int remove_vlan(const char *name, const char *ifname, const int *id, const char *type);
extern int copy_route_xmlconf_file(const char * srcfile, const char * dstfile);

void clear_routing(void){
    return;
}

void clear_balancing(void){
    return;
}

void clear_adsl(void){
    int size = 0, index = 0, new_size = 0, ret_value = 0;
    struct mgtcgi_xml_adsl_clients *old_nodes = NULL;
    struct mgtcgi_xml_adsl_clients *new_nodes = NULL;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&old_nodes, &size);
    if (NULL == old_nodes){
        goto ERROR_EXIT;
    }

    new_size = sizeof(struct mgtcgi_xml_adsl_clients);
    new_nodes = malloc(new_size);
    if (NULL == new_nodes){
		goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    new_nodes->num = 0;

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)new_nodes, new_size);
    if(ret_value < 0){
        goto ERROR_EXIT;
    }

    for(; index < old_nodes->num; index++){
        remove_adsl_link_and_conf(old_nodes->pinfo[index].name, old_nodes->pinfo[index].username, old_nodes->pinfo[index].password);
    }

ERROR_EXIT:
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

void clear_vlan(void){
    int size = 0, index = 0, new_size = 0, ret_value = 0, id;
    struct mgtcgi_xml_interface_vlans *old_nodes = NULL;
    struct mgtcgi_xml_interface_vlans *new_nodes = NULL;

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void**)&old_nodes, &size);
    if (NULL == old_nodes){
        goto ERROR_EXIT;
    }

    new_size = sizeof(struct mgtcgi_xml_interface_vlans);
    new_nodes = malloc(new_size);
    if (NULL == new_nodes){
		goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    new_nodes->num = 0;

    ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_VLANS, (void*)new_nodes, new_size);
    if(ret_value < 0){
        goto ERROR_EXIT;
    }

    for(; index < old_nodes->num; index++){
        id = old_nodes->pinfo[index].id;
        remove_vlan(old_nodes->pinfo[index].name, old_nodes->pinfo[index].ifname, &id, old_nodes->pinfo[index].type);
    }

ERROR_EXIT:
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

void clear_vpn(void){
    return;
}

void clear_port_mapping(void){
    return;
}


int main(void)
{
    clear_routing();
    clear_balancing();
    clear_adsl();
    clear_vlan();
    clear_vpn();
    clear_port_mapping();

    copy_route_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);

    return 0;
}
