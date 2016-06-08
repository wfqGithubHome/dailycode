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
#define DEBUG_PRIMARY
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

static int guide_primary_show(void)
{
	struct guide_if_wan         *wans = NULL;
    struct guide_if_wan_info    *wan_info = NULL;
    struct guide_adsl           *adsls = NULL;
    struct guide_adsl_info      *adsl_info = NULL;
    int                         size = 0, i;
    int                         ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
	if (wans == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    for (i = 0; i < wans->num; i++)
    {
        if (wans->pinfo[i].line_id != GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        wan_info = &(wans->pinfo[i]);
        goto output_wan;
    }

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void**)&adsls, &size);
    for (i = 0; i < adsls->num; i++)
    {
        if (adsls->pinfo[i].line_id != GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        adsl_info = &(adsls->pinfo[i]);
        goto output_adsl;
    }

    ret = GUIDE_OK;
    goto exit;
    
output_wan:
	printf("{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"ipaddr\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\
\"mac\":\"%s\"}",\
		wan_info->ifname, wan_info->isp, wan_info->quality, wan_info->line_id, 
		wan_info->upload, wan_info->download, wan_info->ipaddr, wan_info->netmask,
		wan_info->gateway, wan_info->mac);
    ret = GUIDE_OK;
	goto exit;
    
output_adsl:
	printf("{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"username\":\"%s\",\"password\":\"%s\"}",\
		adsl_info->ifname, adsl_info->isp, adsl_info->quality, adsl_info->line_id, 
		adsl_info->upload, adsl_info->download, adsl_info->username, adsl_info->password);
    ret = GUIDE_OK;
	goto exit;
        
exit:
    if (wans != NULL)
    {
	    free_guide_xml_node((void*)&wans);
    }

    if (adsls != NULL)
    {
        free_guide_xml_node((void*)&adsls);
    }
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static void clean_if_wan_info(struct guide_if_wan_info *wan_info)
{
    wan_info->line_id = 0;
    wan_info->isp = 0;
    wan_info->quality = 0;
    wan_info->upload = 0;
    wan_info->download= 0;
    wan_info->conntype = CONN_TYPE_NONE;
    wan_info->ipaddr[0] = '\0';
    wan_info->netmask[0] = '\0';
    wan_info->gateway[0] = '\0';
    wan_info->mac[0] = '\0';
}

static void set_if_wan_info(struct guide_if_wan_info *wan_info, int isp, int quality, int line_id, 
    int upload, int download, int conntype, char *ipaddr, char *netmask, char *gateway, char *mac)
{
    wan_info->line_id = line_id;
    wan_info->isp = isp;
    wan_info->quality = quality;
    wan_info->upload = upload;
    wan_info->download = download;
    wan_info->conntype = CONN_TYPE_PRIMARY;
    strncpy(wan_info->ipaddr, ipaddr, STRING_LENGTH);
    strncpy(wan_info->netmask, netmask, STRING_LENGTH);
    strncpy(wan_info->gateway, gateway, STRING_LENGTH);
    strncpy(wan_info->mac, mac, STRING_LENGTH);
}

static void set_adsl_info(struct guide_adsl_info *adsl_info, char *ifname, int isp, int quality, 
    int line_id, int upload, int download, char *username, char *password)
{
    strncpy(adsl_info->ifname, ifname, STRING_LENGTH);
    adsl_info->isp = isp;
    adsl_info->quality = quality;
    adsl_info->line_id = line_id;
    adsl_info->upload = upload;
    adsl_info->download = download;
    strncpy(adsl_info->username, username, STRING_LENGTH);
    strncpy(adsl_info->password, password, STRING_LENGTH);
}

static void adsl_info_copy(struct guide_adsl_info *dst, struct guide_adsl_info *src)
{
    memcpy(dst, src, sizeof(struct guide_adsl_info));
}

static int guide_reset_wans(struct guide_if_wan *wans, struct guide_split_elem *elems, int elems_num)
{
    int i;
    int quality = elems ? atoi(elems[CABLE_PARAMETER_QUALITY].val) : 0;
    int ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);

    if (quality == 0)
    {
        for (i = 0; i < wans->num; i++)
        {
            if (wans->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
            {
                clean_if_wan_info(&wans->pinfo[i]);
            }

            if (wans->pinfo[i].conntype == CONN_TYPE_PRIMARY)
            {
                wans->pinfo[i].conntype = CONN_TYPE_NONE;
            }
        }

        goto exit;
    }
    
    if (quality == GUIDE_LINE_QUALITY_ADSL)
    {
        for (i = 0; i < wans->num; i++)
        {
            if (wans->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
            {
                clean_if_wan_info(&wans->pinfo[i]);
            }

            if (wans->pinfo[i].conntype == CONN_TYPE_PRIMARY)
            {
                wans->pinfo[i].conntype = CONN_TYPE_NONE;
            }

            if (strcmp(wans->pinfo[i].ifname, elems[CABLE_PARAMETER_IFNAME].val) == 0)
            {
                wans->pinfo[i].conntype = CONN_TYPE_PRIMARY;
            }
        }

        goto exit;
    }

    if (elems_num != CABLE_FIBER_PARAMETER_MAX - 1)
    {
        ret = GUIDE_TOO_FEW_PARAMETER;
        goto exit;
    }
    
    for (i = 0; i < wans->num; i++)
    {
        if (wans->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
        {
            clean_if_wan_info(&wans->pinfo[i]);
        }

        if (wans->pinfo[i].conntype == CONN_TYPE_PRIMARY)
        {
            wans->pinfo[i].conntype = CONN_TYPE_NONE;
        }

        if (strcmp(elems[CABLE_FIBER_PARAMETER_MAC].val, "null") == 0)
        {
            elems[CABLE_FIBER_PARAMETER_MAC].val[0] = '\0';
        }
        
        if (strcmp(wans->pinfo[i].ifname, elems[CABLE_PARAMETER_IFNAME].val) == 0)
        {
            set_if_wan_info(&wans->pinfo[i], 
                            atoi(elems[CABLE_PARAMETER_ISP].val),
                            atoi(elems[CABLE_PARAMETER_QUALITY].val),
                            atoi(elems[CABLE_PARAMETER_LINE_ID].val),
                            atoi(elems[CABLE_PARAMETER_UPLOAD].val),
                            atoi(elems[CABLE_PARAMETER_DOWNLOAD].val),
                            CONN_TYPE_PRIMARY,
                            elems[CABLE_FIBER_PARAMETER_IPADDR].val,
                            elems[CABLE_FIBER_PARAMETER_NETMASK].val,
                            elems[CABLE_FIBER_PARAMETER_GATEWAY].val,
                            elems[CABLE_FIBER_PARAMETER_MAC].val);
        }
    }
    
exit:
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
    
}

static int guide_reset_adsls(struct guide_adsl **adsls, struct guide_split_elem *elems, int elems_num)
{
    struct guide_adsl   *old_adsls = *adsls;
    struct guide_adsl   *new_adsls = NULL;
    int                 size = 0;
    int                 quality = elems ? atoi(elems[CABLE_PARAMETER_QUALITY].val) : 0;
    int                 idx = 0, i;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);

    size = sizeof(struct guide_adsl) + sizeof(struct guide_adsl_info) * (old_adsls->num + 1);
    new_adsls = malloc(size);
    if (new_adsls == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }       

    memset(new_adsls, 0, size);
    
    if (quality == 0 || quality == GUIDE_LINE_QUALITY_FIBER)
    {
        for (i = 0; i < old_adsls->num; i++)
        {
            if (old_adsls->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
            {
                continue;
            }

            adsl_info_copy(&new_adsls->pinfo[idx], &old_adsls->pinfo[i]);
            new_adsls->num += 1;
            idx++;
        }

        *adsls = new_adsls;
        free(old_adsls);
        goto exit;
    }

    if (elems_num != CABLE_ADSL_PARAMETER_MAX)
    {
        ret = GUIDE_TOO_FEW_PARAMETER;
        goto exit;
    }
    
    set_adsl_info(&new_adsls->pinfo[idx], 
                   elems[CABLE_PARAMETER_IFNAME].val, 
                   atoi(elems[CABLE_PARAMETER_ISP].val), 
                   atoi(elems[CABLE_PARAMETER_QUALITY].val),
                   atoi(elems[CABLE_PARAMETER_LINE_ID].val), 
                   atoi(elems[CABLE_PARAMETER_UPLOAD].val), 
                   atoi(elems[CABLE_PARAMETER_DOWNLOAD].val),
                   elems[CABLE_ADSL_PARAMETER_USERNAME].val,
                   elems[CABLE_ADSL_PARAMETER_PASSWORD].val);
    new_adsls->num +=1;
    idx++;

    for (i = 0; i < old_adsls->num; i++)
    {
        if (old_adsls->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        adsl_info_copy(&new_adsls->pinfo[idx], &old_adsls->pinfo[i]);
        new_adsls->num += 1;
        idx++;
    }   
    
    *adsls = new_adsls;
    free(old_adsls);

exit:
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static void guide_reset_tcs(struct guide_tc *tcs, struct guide_split_elem *elems, int elems_num)
{
    int i;

    for (i = 0; i < tcs->num; i++)
    {
        if (tcs->pinfo[i].line_id != GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        if (elems)
        {
            tcs->pinfo[i].isp = atoi(elems[CABLE_PARAMETER_ISP].val);
            tcs->pinfo[i].quality = atoi(elems[CABLE_PARAMETER_QUALITY].val);
            tcs->pinfo[i].upload = atoi(elems[CABLE_PARAMETER_UPLOAD].val)*GUIDE_REAL_BANDWIDTH_RATIO;
			switch (atoi(elems[CABLE_PARAMETER_UPLOAD].val)){
				case 10000:
					tcs->pinfo[i].upload = 7500;
					break;
				case 15000:
					tcs->pinfo[i].upload = 13500;
					break;
				case 20000:
					tcs->pinfo[i].upload = 17500;
					break;
				case 25000:
					tcs->pinfo[i].upload = 23500;
					break;
				case 30000:
					tcs->pinfo[i].upload = 27500;
					break;
				case 40000:
					tcs->pinfo[i].upload = 37500;
					break;
				case 50000:
					tcs->pinfo[i].upload = 47500;
					break;
				case 100000:
					tcs->pinfo[i].upload = 80000;
					break;
				default:
					break;
			}
			tcs->pinfo[i].download = atoi(elems[CABLE_PARAMETER_DOWNLOAD].val)*GUIDE_REAL_BANDWIDTH_RATIO;
			switch (atoi(elems[CABLE_PARAMETER_DOWNLOAD].val)){
				case 10000:
					tcs->pinfo[i].download = 7500;
					break;
				case 15000:
					tcs->pinfo[i].download = 13500;
					break;
				case 20000:
					tcs->pinfo[i].download = 17500;
					break;
				case 25000:
					tcs->pinfo[i].download = 23500;
					break;
				case 30000:
					tcs->pinfo[i].download = 27500;
					break;
				case 40000:
					tcs->pinfo[i].download = 37500;
					break;
				case 50000:
					tcs->pinfo[i].download = 47500;
					break;
				case 100000:
					tcs->pinfo[i].download = 80000;
					break;
				default:
					break;
			}
			tcs->pinfo[i].enable = 1;
        }
        else
        {
            tcs->pinfo[i].isp = 0;
            tcs->pinfo[i].quality = 0;
            tcs->pinfo[i].upload = 0;
            tcs->pinfo[i].download = 0;
            tcs->pinfo[i].enable = 0;
        }
    }

    return;
}

static int __guide_primary_update(char *info, int info_num)
{
    struct guide_split_elem *elems = NULL;
    int                     elems_num = 0;
    struct guide_if_wan     *wans = NULL;
    struct guide_adsl       *adsls = NULL;
    struct guide_tc         *tcs = NULL;
    int                     size = 0;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_PRIMARY
    int                     j;
#endif
    
    DEBUGP("%s: begin, info: %s, info_num: %d\n", __FUNCTION__, info, info_num);

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void**)&tcs, &size);
    if (tcs == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
        goto exit;
    }
    
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
    if (wans == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
        goto exit;
    }

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void**)&adsls, &size);

    if (info)
    {
        ret = guide_split(info, "__", &elems, &elems_num);
        if (ret != 0)
        {
            ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }
    }

#ifdef DEBUG_PRIMARY
    DEBUGP("%s: first parameter parse\n", __FUNCTION__);
    for (j = 0; j < elems_num; j++)
    {
        DEBUGP("%s\n", elems[j].val);
    }
#endif
    
    ret = guide_reset_wans(wans, elems, elems_num);
    if (ret != 0)
    {
        ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
        goto exit;
    }

    ret = guide_reset_adsls(&adsls, elems, elems_num);
    if (ret != 0)
    {
        ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
        goto exit;
    }

    guide_reset_tcs(tcs, elems, elems_num);

    DEBUGP("%s: wans->num: %d, adsls->num: %d, tcs->num: %d\n", 
        __FUNCTION__, wans->num, adsls->num, tcs->num);
    size = sizeof(struct guide_adsl) + sizeof(struct guide_adsl_info) * adsls->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void*)adsls, size);
    size = sizeof(struct guide_if_wan) + sizeof(struct guide_if_wan_info) * wans->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void*)wans, size);
    size = sizeof(struct guide_tc) + sizeof(struct guide_tc_info) * tcs->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void*)tcs, size);
    
exit:
    if (elems)
    {
        free(elems);
    }

    if (tcs)
    {
        free(tcs);
    }

    if (wans)
    {
        free(wans);
    }

    if (adsls)
    {
        free(adsls);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

static int guide_primary_update(void)
{
    char info_num_str[STRING_LENGTH] = {0};
    int  info_num = 0;
    char *info = NULL;
    int ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("info_num", info_num_str, sizeof(info_num_str));
    info_num = atoi(info_num_str);

    if (info_num > 0)
    {
        info = malloc(MAX_SEGMENT_LENGTH * info_num);
        if (info == NULL)
        {
            ret = GUIDE_MEMORY_ALLOC_FAILED;
            goto exit;
        }
        
        memset(info, 0, MAX_SEGMENT_LENGTH * info_num);
        cgiFormString("info", info, MAX_SEGMENT_LENGTH * info_num);
    }

    ret = __guide_primary_update(info, info_num);
    
exit:
    if (info)
    {
        free(info);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_primary(void)
{
    char action[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        printf("{\"data\":[");
        ret = guide_primary_show();
        printf("],");
    }
    else if (strcmp(action, "reset") == 0)
    {
        ret = guide_primary_update();
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

