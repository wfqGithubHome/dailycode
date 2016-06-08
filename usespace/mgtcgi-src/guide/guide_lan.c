#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <linux/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <dirent.h>

#include "include/utils.h"
#include "include/debug.h"
#include "include/cgic.h"
#include "include/guide_error.h"
#include "include/guide.h"

#if 0
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

static int guide_interface_lan_show(void)
{
	struct guide_if_lan *lans = NULL;
    int                 size = 0;
    int                 i;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_LAN, (void**)&lans, &size);
	if (lans == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    printf("{\"data\":[");
	for (i = 0; i < lans->num; i++)
    {
		printf("{\"ifname\":\"%s\",\"ipaddr\":\"%s\",\"netmask\":\"%s\",\"mac\":\"%s\"}",\
            lans->pinfo[i].ifname,
            lans->pinfo[i].ipaddr, 
			lans->pinfo[i].netmask, 
			lans->pinfo[i].mac);
	}
	printf("],");
    
exit:
    if (lans != NULL)
    {
	    free_guide_xml_node((void*)&lans);
    }
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static int guide_interface_lan_update(char *ipaddr_str, char *netmask_str, char *mac)
{
    struct guide_if_lan *lans = NULL;
    struct guide_server *servers = NULL;
    int                 size = 0;
    struct in_addr      inaddr;
    uint32_t            naddr, nmask, nwork;
    int                 mask_len = 0, result, i;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: ipaddr: %s, netmask: %s, mac: %s\n", __FUNCTION__, ipaddr_str, netmask_str, mac);

    /* check arguments */
    if (inet_aton(ipaddr_str, &inaddr) == 0)
    {
        ret = GUIDE_LAN_IPADDRESS_ERROR;
        DEBUGP("%s: ip address format error\n", __FUNCTION__);
        goto exit;
    }
    naddr = inaddr.s_addr;

    if (inet_aton(netmask_str, &inaddr) == 0)
    {
        ret = GUIDE_LAN_NETMASK_ERROR;
        DEBUGP("%s: netmask format error\n", __FUNCTION__);
        goto exit;
    }
    nmask = inaddr.s_addr;
    nwork = naddr & nmask;
    mask_len = guide_mask_len(nmask);

    mac_repair(mac, strlen(mac), ":");

    size = sizeof(struct guide_if_lan) + sizeof(struct guide_if_lan_info);
    lans = malloc(size);
    if (lans == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }

    lans->num = 1;
    strncpy(lans->pinfo[0].ifname, "LAN", STRING_LENGTH);
    strncpy(lans->pinfo[0].ipaddr, ipaddr_str, STRING_LENGTH);
    strncpy(lans->pinfo[0].netmask, netmask_str, STRING_LENGTH);
    strncpy(lans->pinfo[0].mac, mac, STRING_LENGTH);

    result = save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_LAN, (void*)lans, size);
    if (result <= 0)
    {
        ret = GUIDE_WRITE_FILE_ERROR;
    }

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void**)&servers, &size);
    if (servers == NULL || servers->num < 1)
    {
        ret = GUIDE_READ_FILE_ERROR;
        goto exit;
    }

    for (i = 0; i < servers->num; i++)
    {
        if (servers->pinfo[i].parentid != 1)
        {
            continue;
        }

        snprintf(servers->pinfo[i].value, STRING_LENGTH, NIPQUAD_FMT"/%d", NIPQUAD(nwork), mask_len);
    }

    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void*)servers, size);
exit:
    if (lans)
    {
        free(lans);
    }

    if (servers)
    {
        free_guide_xml_node((void*)&servers);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_lan(void)
{
    char action[STRING_LENGTH] = {0};
    char ipaddr[STRING_LENGTH] = {0};
    char netmask[STRING_LENGTH] = {0};
    char mac[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_interface_lan_show();
    }
    else if (strcmp(action, "reset") == 0)
    {
        cgiFormString("ipaddr", ipaddr, sizeof(ipaddr));
        cgiFormString("netmask", netmask, sizeof(netmask));
        cgiFormString("mac", mac, sizeof(mac));
        ret = guide_interface_lan_update(ipaddr, netmask, mac);
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

