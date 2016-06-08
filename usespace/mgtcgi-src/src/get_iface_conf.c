#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/xmlwrapper4c.h"
#include "../include/route_xmlwrapper4c.h"

#define IFACE_XML_BAK "/etc/iface.xml.bak"
#define ROUTE_XML_CONF "/etc/route_config.xml.bak"

#define DEV_ERR -1
#define DEV_OK   0

void get_lan_conf(char *dst_xconf, char *src_xconf, int enable)
{
    struct mgtcgi_xml_interface_lans *iface_lan = NULL;
    struct mgtcgi_xml_interface_lans *route_lan = NULL;
    int src_size,dst_size,ret_value;
    int index;
    
    if(enable == 0)
        return;

    get_route_xml_node(src_xconf, MGTCGI_TYPE_INTERFACE_LANS, (void**)&route_lan, &src_size);
    if(NULL == route_lan){
        goto ERROREXIT;
    }

    get_route_xml_node(dst_xconf, MGTCGI_TYPE_INTERFACE_LANS, (void**)&iface_lan, &dst_size);
    if(NULL == iface_lan){
        goto ERROREXIT;
    }

    for(index = 0; index < route_lan->num; index++){
        strncpy(iface_lan->pinfo[index].ip,route_lan->pinfo[index].ip,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].ifname,route_lan->pinfo[index].ifname,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].mac,route_lan->pinfo[index].mac,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].newmac,route_lan->pinfo[index].newmac,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].mask,route_lan->pinfo[index].mask,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].comment,route_lan->pinfo[index].comment,STRING_LENGTH);
        strncpy(iface_lan->pinfo[index].aliases,route_lan->pinfo[index].aliases,STRING_LENGTH);
    }

    ret_value = save_route_xml_node(dst_xconf, MGTCGI_TYPE_INTERFACE_LANS, (void*)iface_lan, dst_size);
	if (ret_value < 0){
		goto ERROREXIT;
	}

ERROREXIT:
    if(iface_lan){
	    free_xml_node((void *)&iface_lan);
        iface_lan = NULL;
    }
    if(route_lan){
	    free_xml_node((void *)&route_lan);
        route_lan = NULL;
    }
    return;

}

void get_wan_conf(char *dst_xconf, char *src_xconf, int enable)
{
    struct mgtcgi_xml_interface_wans *iface_wan = NULL;
    struct mgtcgi_xml_interface_wans *route_wan = NULL;
    int src_size,dst_size,ret_value;
    int index;
    
    if(enable == 0)
        return;

    get_route_xml_node(src_xconf, MGTCGI_TYPE_INTERFACE_WANS, (void**)&route_wan, &src_size);
    if(NULL == route_wan){
        goto ERROREXIT;
    }

    get_route_xml_node(dst_xconf, MGTCGI_TYPE_INTERFACE_WANS, (void**)&iface_wan, &dst_size);
    if(NULL == iface_wan){
        goto ERROREXIT;
    }

    for(index = 0; index < route_wan->num; index++){
        strncpy(iface_wan->pinfo[index].ip,route_wan->pinfo[index].ip,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].ifname,route_wan->pinfo[index].ifname,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].mac,route_wan->pinfo[index].mac,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].newmac,route_wan->pinfo[index].newmac,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].mask,route_wan->pinfo[index].mask,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].comment,route_wan->pinfo[index].comment,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].access,route_wan->pinfo[index].access,STRING_LENGTH);
        strncpy(iface_wan->pinfo[index].gateway,route_wan->pinfo[index].gateway,STRING_LENGTH);
        iface_wan->pinfo[index].weight = route_wan->pinfo[index].weight;
        iface_wan->pinfo[index].defaultroute = route_wan->pinfo[index].defaultroute;
    }

    ret_value = save_route_xml_node(dst_xconf, MGTCGI_TYPE_INTERFACE_WANS, (void*)iface_wan, dst_size);
	if (ret_value < 0){
		goto ERROREXIT;
	}

ERROREXIT:
    if(route_wan){
	    free_xml_node((void *)&route_wan);
        route_wan = NULL;
    }
    if(iface_wan){
	    free_xml_node((void *)&iface_wan);
        iface_wan = NULL;
    }
    return;

}


int main(void)
{
    if((access(ROUTE_XML_CONF,F_OK) != 0) || (access(IFACE_XML_BAK,F_OK) != 0)){
        return DEV_ERR;
    }

    get_lan_conf(IFACE_XML_BAK,ROUTE_XML_CONF,1);
    get_wan_conf(IFACE_XML_BAK,ROUTE_XML_CONF,1);

    return 0;   
}

