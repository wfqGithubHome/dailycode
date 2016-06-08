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
#define DEBUG_SLAVE
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

int     first_slave                            = 1;
int     total_tc_upload[GUIDE_LINE_ID_MAX]     = {0, 0, 0, 0};
int     total_tc_download[GUIDE_LINE_ID_MAX]   = {0, 0, 0, 0};
int     total_tc_sub_lines[GUIDE_LINE_ID_MAX]  = {0, 0, 0, 0};
int     tc_isp[GUIDE_LINE_ID_MAX]              = {0, 0, 0, 0};
int     tc_quality[GUIDE_LINE_ID_MAX]          = {0, 0, 0, 0};

static void guide_slave_wan_show(struct guide_if_wan *wans)
{
    int i;

    for (i = 0; i < wans->num; i++)
    {
        if (wans->pinfo[i].line_id == 0 ||
            wans->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        if (first_slave)
        {
            printf("{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"ipaddr\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\
\"mac\":\"%s\"}",\
        		wans->pinfo[i].ifname, wans->pinfo[i].isp, wans->pinfo[i].quality, 
        		wans->pinfo[i].line_id, wans->pinfo[i].upload, wans->pinfo[i].download, 
        		wans->pinfo[i].ipaddr, wans->pinfo[i].netmask, wans->pinfo[i].gateway, 
        		wans->pinfo[i].mac);
            first_slave = 0;
        }
        else
        {
            printf(",{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"ipaddr\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\
\"mac\":\"%s\"}",\
        		wans->pinfo[i].ifname, wans->pinfo[i].isp, wans->pinfo[i].quality, 
        		wans->pinfo[i].line_id, wans->pinfo[i].upload, wans->pinfo[i].download, 
        		wans->pinfo[i].ipaddr, wans->pinfo[i].netmask, wans->pinfo[i].gateway, 
        		wans->pinfo[i].mac);
        }
    }
}

static void guide_slave_adsl_show(struct guide_adsl *adsls)
{
    int i;

    for (i = 0; i < adsls->num; i++)
    {
        if (adsls->pinfo[i].line_id == 0 ||
            adsls->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        if (first_slave)
        {
            printf("{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"username\":\"%s\",\"password\":\"%s\"}",\
        		adsls->pinfo[i].ifname, adsls->pinfo[i].isp, adsls->pinfo[i].quality, 
        		adsls->pinfo[i].line_id, adsls->pinfo[i].upload, adsls->pinfo[i].download, 
        		adsls->pinfo[i].username, adsls->pinfo[i].password);
            first_slave = 0;
        }
        else
        {
            printf(",{\"ifname\":\"%s\",\"isp\":\"%d\",\"quality\":\"%d\",\"line_id\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"username\":\"%s\",\"password\":\"%s\"}",\
        		adsls->pinfo[i].ifname, adsls->pinfo[i].isp, adsls->pinfo[i].quality, 
        		adsls->pinfo[i].line_id, adsls->pinfo[i].upload, adsls->pinfo[i].download, 
        		adsls->pinfo[i].username, adsls->pinfo[i].password);
        }
    }
}

static int guide_slave_show(void)
{
	struct guide_if_wan         *wans = NULL;
    struct guide_adsl           *adsls = NULL;
    int                         size = 0;
    int                         ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
	if (wans == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    guide_slave_wan_show(wans);

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void**)&adsls, &size);
    guide_slave_adsl_show(adsls);

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

static void adsl_info_copy(struct guide_adsl_info *dst, struct guide_adsl_info *src)
{
    memcpy(dst, src, sizeof(struct guide_adsl_info));
}

static void __guide_slave_wans_reset(struct guide_if_wan *wans, 
    struct guide_split_elem *snd_elems, int snd_elems_num)
{
    int quality;
    int line_id;
    int i;

    quality = atoi(snd_elems[CABLE_PARAMETER_QUALITY].val);
    if (quality == GUIDE_LINE_QUALITY_ADSL)
    {
        for (i = 0; i < wans->num; i++)
        {
            if (strcmp(wans->pinfo[i].ifname, snd_elems[CABLE_PARAMETER_IFNAME].val) == 0)
            {
                wans->pinfo[i].conntype = CONN_TYPE_SLAVE;
            }
        }

        return;
    }

    if (strcmp(snd_elems[CABLE_FIBER_PARAMETER_MAC].val, "null") == 0)
    {
        snd_elems[CABLE_FIBER_PARAMETER_MAC].val[0] = '\0';
    }

    for (i = 0; i < wans->num; i++)
    {
        if (strcmp(wans->pinfo[i].ifname, snd_elems[CABLE_PARAMETER_IFNAME].val) == 0)
        {
            wans->pinfo[i].line_id = atoi(snd_elems[CABLE_PARAMETER_LINE_ID].val);
            wans->pinfo[i].isp = atoi(snd_elems[CABLE_PARAMETER_ISP].val);
            wans->pinfo[i].quality = atoi(snd_elems[CABLE_PARAMETER_QUALITY].val);
            wans->pinfo[i].upload = atoi(snd_elems[CABLE_PARAMETER_UPLOAD].val);
            wans->pinfo[i].download = atoi(snd_elems[CABLE_PARAMETER_DOWNLOAD].val);
            wans->pinfo[i].conntype = CONN_TYPE_SLAVE;
            strncpy(wans->pinfo[i].ipaddr, snd_elems[CABLE_FIBER_PARAMETER_IPADDR].val, STRING_LENGTH);
            strncpy(wans->pinfo[i].netmask, snd_elems[CABLE_FIBER_PARAMETER_NETMASK].val, STRING_LENGTH);
            strncpy(wans->pinfo[i].gateway, snd_elems[CABLE_FIBER_PARAMETER_GATEWAY].val, STRING_LENGTH);
            strncpy(wans->pinfo[i].mac, snd_elems[CABLE_FIBER_PARAMETER_MAC].val, STRING_LENGTH);

            line_id = wans->pinfo[i].line_id;
            total_tc_upload[line_id] += wans->pinfo[i].upload;
            total_tc_download[line_id] += wans->pinfo[i].download;
            total_tc_sub_lines[line_id] += 1;
            tc_isp[line_id] = wans->pinfo[i].isp;
            tc_quality[line_id] = GUIDE_LINE_QUALITY_FIBER;
        }
    }
    
}

static void __guide_slave_adsl_reset(struct guide_adsl *new_adsls, 
    struct guide_split_elem *snd_elems, int snd_elems_num)
{
    int quality;
    int line_id;
    int idx = new_adsls->num;

    quality = atoi(snd_elems[CABLE_PARAMETER_QUALITY].val);
    if (quality == GUIDE_LINE_QUALITY_FIBER)
    {
        return;
    }

    new_adsls->pinfo[idx].line_id = atoi(snd_elems[CABLE_PARAMETER_LINE_ID].val);
    new_adsls->pinfo[idx].isp = atoi(snd_elems[CABLE_PARAMETER_ISP].val);
    new_adsls->pinfo[idx].quality = atoi(snd_elems[CABLE_PARAMETER_QUALITY].val);
    new_adsls->pinfo[idx].upload = atoi(snd_elems[CABLE_PARAMETER_UPLOAD].val);
    new_adsls->pinfo[idx].download = atoi(snd_elems[CABLE_PARAMETER_DOWNLOAD].val);
    strncpy(new_adsls->pinfo[idx].ifname, snd_elems[CABLE_PARAMETER_IFNAME].val, STRING_LENGTH);
    strncpy(new_adsls->pinfo[idx].username, snd_elems[CABLE_ADSL_PARAMETER_USERNAME].val, STRING_LENGTH);
    strncpy(new_adsls->pinfo[idx].password, snd_elems[CABLE_ADSL_PARAMETER_PASSWORD].val, STRING_LENGTH);

    line_id = new_adsls->pinfo[idx].line_id;
    total_tc_upload[line_id] += new_adsls->pinfo[idx].upload;
    total_tc_download[line_id] += new_adsls->pinfo[idx].download;
    total_tc_sub_lines[line_id] += 1;
    tc_isp[line_id] = new_adsls->pinfo[idx].isp;
    if (tc_quality[line_id] != GUIDE_LINE_QUALITY_FIBER)
    {
        tc_quality[line_id] = GUIDE_LINE_QUALITY_ADSL;
    }
    
    new_adsls->num += 1;
    idx++;

}

static int guide_slave_wans_reset(struct guide_if_wan *wans, 
    struct guide_split_elem *fst_elems, int fst_elems_num)
{    
    struct guide_split_elem *snd_elems = NULL;
    int                     snd_elems_num = 0;
    int                     i;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_SLAVE
    int                     j;
#endif

    DEBUGP("%s: begin\n", __FUNCTION__);

    /* clean all slave fiber */
    for (i = 0; i < wans->num; i++)
    {
        if (wans->pinfo[i].line_id == GUIDE_PRIMARY_LINE_ID ||
            wans->pinfo[i].conntype == CONN_TYPE_PRIMARY)
        {
            continue;
        }

        wans->pinfo[i].line_id = 0;
        wans->pinfo[i].isp = 0;
        wans->pinfo[i].quality = 0;
        wans->pinfo[i].upload = 0;
        wans->pinfo[i].download = 0;
        wans->pinfo[i].conntype = CONN_TYPE_NONE;
        wans->pinfo[i].ipaddr[0] = '\0';
        wans->pinfo[i].netmask[0] = '\0';
        wans->pinfo[i].gateway[0] = '\0';
        wans->pinfo[i].mac[0] = '\0';
    }

    if (fst_elems == NULL)
    {
        goto exit;
    }
    
    for (i = 0; i < fst_elems_num; i++)
    {
        ret = guide_split(fst_elems[i].val, "__", &snd_elems, &snd_elems_num);
        if (ret != 0)
        {
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

#ifdef DEBUG_SLAVE
        DEBUGP("%s: second layer parameter parse, snd_elems_num: %d\n", __FUNCTION__, snd_elems_num);
        for (j = 0; j < snd_elems_num; j++)
        {
            DEBUGP("%s ", snd_elems[j].val);
        }
        DEBUGP("\n");
#endif

        if (snd_elems_num != (CABLE_FIBER_PARAMETER_MAX-1) &&
            snd_elems_num != CABLE_ADSL_PARAMETER_MAX)
        {
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

        __guide_slave_wans_reset(wans, snd_elems, snd_elems_num);

        free(snd_elems);
        snd_elems = NULL;
        snd_elems_num = 0;
    }

exit:
    if (snd_elems)
    {
        free(snd_elems);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int guide_slave_adsls_reset(struct guide_adsl **adsls, 
    struct guide_split_elem *fst_elems, int fst_elems_num)
{
    struct guide_split_elem     *snd_elems = NULL;
    int                         snd_elems_num = 0;
    struct guide_adsl           *new_adsls = NULL;
    struct guide_adsl           *old_adsls = *adsls;
    int                         size = 0, idx = 0, i;
    int                         ret = GUIDE_OK;
#ifdef DEBUG_SLAVE
    int                         j;
#endif

    DEBUGP("%s: begin, adsls->num: %d, fst_elems_num: %d\n", 
    __FUNCTION__, old_adsls->num, fst_elems_num);

    size = sizeof(struct guide_adsl) + sizeof(struct guide_adsl_info) * (fst_elems_num + 1);
    new_adsls = malloc(size);
    if (new_adsls == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }
    memset(new_adsls, 0, size);

    /* clean all slave adsl */
    for (i = 0; i < old_adsls->num; i++)
    {
        if (old_adsls->pinfo[i].line_id != GUIDE_PRIMARY_LINE_ID)
        {
            continue;
        }

        adsl_info_copy(&new_adsls->pinfo[idx], &old_adsls->pinfo[i]);
        new_adsls->num += 1;
        idx += 1;
    }

    if (fst_elems == NULL)
    {
        *adsls = new_adsls;
        free(old_adsls);
        goto exit;
    }
    
    for (i = 0; i < fst_elems_num; i++)
    {
        DEBUGP("%s: fst_elems[%d].val: %s\n", __FUNCTION__, i, fst_elems[i].val);
        ret = guide_split(fst_elems[i].val, "__", &snd_elems, &snd_elems_num);
        if (ret != 0)
        {
            free(new_adsls);
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }
        
#ifdef DEBUG_SLAVE
        DEBUGP("%s: second layer parameter parse, snd_elems_num: %d\n", __FUNCTION__, snd_elems_num);
        for (j = 0; j < snd_elems_num; j++)
        {
            DEBUGP("%s ", snd_elems[j].val);
        }
        DEBUGP("\n");
#endif

        if (snd_elems_num != (CABLE_FIBER_PARAMETER_MAX-1) &&
            snd_elems_num != CABLE_ADSL_PARAMETER_MAX)
        {
            DEBUGP("%s: elems_num error: %d\n", __FUNCTION__, snd_elems_num);
            free(new_adsls);
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

        __guide_slave_adsl_reset(new_adsls, snd_elems, snd_elems_num);

        free(snd_elems);
        snd_elems = NULL;
        snd_elems_num = 0;
    }

    *adsls = new_adsls;
    free(old_adsls);
    
exit:
    if (snd_elems)
    {
        free(snd_elems);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static void guide_tcs_reset(struct guide_tc *tcs)
{
    int line_id;
    int i;
    

    for (i= 1; i < tcs->num; i++)
    {
        line_id = tcs->pinfo[i].line_id;
        
        if (total_tc_sub_lines[line_id] == 0)
        {
            tcs->pinfo[i].isp = 0;
            tcs->pinfo[i].quality = 0;
            tcs->pinfo[i].balance = 0;
            tcs->pinfo[i].upload = 0;
            tcs->pinfo[i].download = 0;
            tcs->pinfo[i].enable = 0;
        }
        else if (total_tc_sub_lines[line_id] == 1)
        {
            tcs->pinfo[i].isp = tc_isp[line_id];
            tcs->pinfo[i].quality = tc_quality[line_id];
            tcs->pinfo[i].balance = 0;
            tcs->pinfo[i].upload = total_tc_upload[line_id]*GUIDE_REAL_BANDWIDTH_RATIO;
            tcs->pinfo[i].download = total_tc_download[line_id]*GUIDE_REAL_BANDWIDTH_RATIO;
            tcs->pinfo[i].enable = 1;
        }
        else
        {
            tcs->pinfo[i].isp = tc_isp[line_id];
            tcs->pinfo[i].quality = tc_quality[line_id];
            tcs->pinfo[i].balance = 1;
            tcs->pinfo[i].upload = total_tc_upload[line_id]*GUIDE_REAL_BANDWIDTH_RATIO;
            tcs->pinfo[i].download = total_tc_download[line_id]*GUIDE_REAL_BANDWIDTH_RATIO;
            tcs->pinfo[i].enable = 1;
        }
    }  

    return;
}

static int __guide_slave_update(char *info, int info_num)
{
    struct guide_split_elem *fst_elems = NULL;
    int                     fst_elems_num = 0;
    
    struct guide_if_wan     *wans = NULL;
    struct guide_adsl       *adsls = NULL;
    struct guide_tc         *tcs = NULL;
    
    int                     size = 0;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_SLAVE
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

    if (info != NULL)
    {
        ret = guide_split(info, "--", &fst_elems, &fst_elems_num);
        if (ret != 0)
        {
            ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }
    }

#ifdef DEBUG_SLAVE
    DEBUGP("%s: first parameter parse\n", __FUNCTION__);
    for (j = 0; j < fst_elems_num; j++)
    {
        DEBUGP("%s\n", fst_elems[j].val);
    }
#endif

    ret = guide_slave_wans_reset(wans, fst_elems, fst_elems_num);
    if (ret != GUIDE_OK)
    {
        goto exit;
    }

    ret = guide_slave_adsls_reset(&adsls, fst_elems, fst_elems_num);
    if (ret != GUIDE_OK)
    {
        goto exit;
    }

    guide_tcs_reset(tcs);

    size = sizeof(struct guide_adsl) + sizeof(struct guide_adsl_info) * adsls->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void*)adsls, size);
    size = sizeof(struct guide_if_wan) + sizeof(struct guide_if_wan_info) * wans->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void*)wans, size);
    size = sizeof(struct guide_tc) + sizeof(struct guide_tc_info) * tcs->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void*)tcs, size);
    ret = GUIDE_OK;
    
exit:
    if (fst_elems)
    {
        free(fst_elems);
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

static int guide_slave_update(void)
{
    char info_num_str[STRING_LENGTH] = {0};
    int  info_num = 0;
    char *info = NULL;
    int  ret = GUIDE_OK;

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
        
    ret = __guide_slave_update(info, info_num);

exit:
    if (info)
    {
        free(info);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_slave(void)
{
    char action[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        printf("{\"data\":[");
        ret = guide_slave_show();
        printf("],");  
        
    }
    else if (strcmp(action, "reset") == 0)
    {
        ret = guide_slave_update();
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

