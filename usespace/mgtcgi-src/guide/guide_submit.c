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

static int server_configure_check(struct guide_server *servers, struct guide_if_lan *lans)
{
    struct in_addr inaddr;
    char network_segment[STRING_LENGTH] = {0};
    char *network = NULL;
    char *netmask = NULL;
    uint32_t network_addr, mask_addr, lan_addr;
    int mask_len;
    int find_internal_network_segment = 0;
    int i, ret = GUIDE_OK;

    for (i = 0; i < servers->num; i++)
    {
        if (servers->pinfo[i].parentid == 1)
        {
            find_internal_network_segment = 1;
            strncpy(network_segment, servers->pinfo[i].value, STRING_LENGTH);
            break;
        }
    }

    DEBUGP("%s: network_segment: %s\n", __FUNCTION__, network_segment);
    
    if (find_internal_network_segment == 0)
    {
        DEBUGP("%s: can not find network segment info\n", __FUNCTION__);
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
        goto exit;
    }

    network = network_segment;
    netmask = strchr(network, '/');
    if (netmask == NULL)
    {
        DEBUGP("%s: can not find /in network_segment\n", __FUNCTION__);
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
        goto exit;
    }

    *netmask = '\0';
    netmask += 1;

    DEBUGP("%s: network: %s, netmask: %s\n", __FUNCTION__, network, netmask);
    
    if (inet_aton(network, &inaddr) == 0)
    {
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
        goto exit;
    }

    network_addr = inaddr.s_addr;
    if (netmask[0] == '\0')
    {
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
        goto exit;
    }

    mask_len = atoi(netmask);
    DEBUGP("%s: mask_len: %d\n", __FUNCTION__, mask_len);
    
    if (mask_len <= 0 || mask_len >= 32)
    {
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
        goto exit;
    }

    memset(&mask_addr, 0xff, sizeof(uint32_t));
    mask_addr = mask_addr << (32 - mask_len);
    mask_addr = htonl(mask_addr);

    DEBUGP("%s: lan addr: %x\n", __FUNCTION__, lan_addr);
    if (inet_aton(lans->pinfo[0].ipaddr, &inaddr) == 0)
    {
        ret = GUIDE_SUBMIT_LAN_IPADDR_ERROR;
        goto exit;
    }
    
    lan_addr = inaddr.s_addr;
    DEBUGP("%s: lan addr: %u, mask_addr: %u, network: %u\n",
        __FUNCTION__, lan_addr, mask_addr, network_addr);
    
    if ((lan_addr & mask_addr) != (network_addr & mask_addr))
    {
        ret = GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR;
    }
    
exit:
    return ret;
}

static int primary_configure_check(struct guide_if_wan *wans, 
    struct guide_adsl *adsls, struct guide_tc *tcs)
{
    return GUIDE_OK;
}

static int slave_configure_check(struct guide_if_wan *wans, 
    struct guide_adsl *adsls, struct guide_tc *tcs)
{
    return GUIDE_OK;
}

static int appbind_configure_check(struct guide_appbind *appbinds, struct guide_tc *tcs)
{
    int tc_enable[GUIDE_LINE_ID_MAX] = {0, 0, 0, 0};
    int line_id, bind_line_id, i;
    int ret = GUIDE_OK;

    for (i = 0; i < tcs->num; i++)
    {
        line_id = tcs->pinfo[i].line_id;
        tc_enable[line_id] = tcs->pinfo[i].enable;
    }

    for (i = 0; i < appbinds->num; i++)
    {
        if (appbinds->pinfo[i].bind_line_id == 0)
        {
            continue;
        }

        bind_line_id = appbinds->pinfo[i].bind_line_id;
        if (tc_enable[bind_line_id] != 1)
        {
            ret = GUIDE_SUBMIT_BIND_ERROR;
            break;
        }
    }
    
    return ret;
}

static int device_zone_configure_check(struct guide_sel_zone *zones)
{
    int i; 
    int ret = GUIDE_SUBMIT_NO_ZONE;

    for (i= 0; i < zones->num; i++)
    {
        if (zones->pinfo[i].type == 1)
        {   
            ret = GUIDE_OK;
            break;
        }
    }

    return ret;
}

int guide_module_submit(void)
{
    struct guide_if_lan     *lans = NULL;
    struct guide_if_wan     *wans = NULL;
    struct guide_adsl       *adsls = NULL;
    struct guide_server     *servers = NULL;
    struct guide_appbind    *appbinds = NULL;
    struct guide_tc         *tcs = NULL;
    struct guide_sel_zone   *zones = NULL;

    char                    cmd[CMD_LENGTH] = {0};
    int                     size = 0;
	int                     ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_LAN, (void**)&lans, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void**)&adsls, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void**)&servers, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_APPBIND, (void**)&appbinds, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void**)&tcs, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ZONE, (void**)&zones, &size);

    ret = server_configure_check(servers, lans);
    if (ret != GUIDE_OK)
    {
        DEBUGP("%s: network segment configure error\n", __FUNCTION__);
        goto exit;
    }
    
    ret = primary_configure_check(wans, adsls, tcs);
    if (ret != GUIDE_OK)
    {
        DEBUGP("%s: primary configure error\n", __FUNCTION__);
        goto exit;
    }

    ret = slave_configure_check(wans, adsls, tcs);
    if (ret != GUIDE_OK)
    {
        DEBUGP("%s: slave configure error\n", __FUNCTION__);
        goto exit;
    }

    ret = appbind_configure_check(appbinds, tcs);
    if (ret != GUIDE_OK)
    {
        DEBUGP("%s: appbind configure error\n", __FUNCTION__);
        goto exit;
    }

    ret = device_zone_configure_check(zones);
    if (ret != GUIDE_OK)
    {
        DEBUGP("%s: zone configure error\n", __FUNCTION__);
        goto exit;
    }

    snprintf(cmd, CMD_LENGTH, "%s/guide_script.sh submit", GUIDE_SCRIPT_PATH);
    system(cmd);
    
exit:
    if (lans)
    {
        free_guide_xml_node((void*)&lans);
    }
    
    if (wans)
    {
	    free_guide_xml_node((void*)&wans);
    }

    if (adsls)
    {
	    free_guide_xml_node((void*)&adsls);
    }

    if (servers)
    {
	    free_guide_xml_node((void*)&servers);
    }

    if (appbinds)
    {
	    free_guide_xml_node((void*)&appbinds);
    }

    if (tcs)
    {
	    free_guide_xml_node((void*)&tcs);
    }

    if (zones)
    {
	    free_guide_xml_node((void*)&zones);
    }
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
    
}
