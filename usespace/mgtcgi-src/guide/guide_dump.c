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

#define ROWS_BUFF_SIZE 409600
#define ROW_BUFF_SIZE  4096
 
typedef struct guide_balance_info
{
    int  line_id;
    int  quota;
    char ifname[STRING_LENGTH];
} guide_balance_info_t;

static void get_channel_if_name(struct guide_tc *tcs, 
    struct guide_if_wan *wans, struct guide_adsl *adsls)
{
    int i, j;
    int line_id;
    int find_in_wans = 0;

    DEBUGP("%s begin\n", __FUNCTION__);
    
    for (i = 0; i < tcs->num; i++)
    {
        if (tcs->pinfo[i].enable == 0)
        {
            continue;
        }
        
        memset(tcs->pinfo[i].name, 0, STRING_LENGTH);
        line_id = tcs->pinfo[i].line_id;
        
        for (j = 0; j < wans->num; j++)
        {
            if (wans->pinfo[j].line_id != line_id)
            {
                continue;
            }

            strncpy(tcs->pinfo[i].name, wans->pinfo[j].ifname, STRING_LENGTH);
            find_in_wans = 1;
            break;
        }

        if (find_in_wans)
        {
            find_in_wans = 0;
            continue;
        }

        for (j = 0; j < adsls->num; j++)
        {
            if (adsls->pinfo[j].line_id != line_id)
            {
                continue;
            }

            snprintf(tcs->pinfo[i].name, STRING_LENGTH, "adsl-%d", j+1);
            break;
        }
    }
}

static void guide_lans_dump(char *rows, char *row, struct guide_if_lan *lans)
{
    int i;
    int first_lan = 1;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_LANS=");
    for (i = 0; i < lans->num; i++)
    {            
        snprintf(row, ROW_BUFF_SIZE, "%s__%s__%s", 
            lans->pinfo[i].ipaddr,
            lans->pinfo[i].netmask,
            strlen(lans->pinfo[i].mac) > 0 ? lans->pinfo[i].mac : "null");

        if (first_lan)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_lan = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    return;
}

static void guide_wans_dump(char *rows, char *row, struct guide_if_wan *wans)
{
    int i;
    int first_wan = 1;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_WANS=");
    for (i = 0; i < wans->num; i++)
    {            
        snprintf(row, ROW_BUFF_SIZE, "%d__%s__%s__%s__%s__%s", 
            wans->pinfo[i].line_id,
            wans->pinfo[i].ifname,
            strlen(wans->pinfo[i].ipaddr) > 0 ? wans->pinfo[i].ipaddr : "null",
            strlen(wans->pinfo[i].netmask) > 0 ? wans->pinfo[i].netmask : "null",
            strlen(wans->pinfo[i].gateway) > 0 ? wans->pinfo[i].gateway : "null",
            strlen(wans->pinfo[i].mac) > 0 ? wans->pinfo[i].mac : "null");

        if (first_wan)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_wan = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    return;
}

static void guide_servers_dump(char *rows, char *row, struct guide_server *servers)
{
    int index = 1, i;
    int first_server = 1;
    int group_id;
    int group_empty[5] ={1, 1, 1, 1, 1};

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_SERVERS=");
    for (i = 0; i < servers->num; i++)
    {            
        group_id = servers->pinfo[i].parentid;
        group_empty[group_id] = 0;
        
        snprintf(row, ROW_BUFF_SIZE, "IP%d__%d__%s", 
            index, 
            servers->pinfo[i].parentid,
            servers->pinfo[i].value);
        index++;

        if (first_server)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_server = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    for (i = 2; i < 5; i++)
    {
        if (group_empty[i] == 0)
        {
            continue;
        }

        snprintf(row, ROW_BUFF_SIZE, "IP%d__%d__%s", 
            index, 
            i,
            "0.0.0.0");
        index++;

        snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
    }

    printf("%s\n", rows);
    return;
}

static void guide_adsls_dump(char *rows, char *row, struct guide_adsl *adsls)
{
    int index = 1, i;
    int first_adsl = 1;

    DEBUGP("%s: begin, adsls->num: %d\n", __FUNCTION__, adsls->num);

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_ADSLS=");
    for (i = 0; i < adsls->num; i++)
    {            
        snprintf(row, ROW_BUFF_SIZE, "%d__%s__%d__%s__%s",
            index, 
            adsls->pinfo[i].ifname,
            adsls->pinfo[i].line_id,
            strlen(adsls->pinfo[i].username) > 0 ? adsls->pinfo[i].username : "null",
            strlen(adsls->pinfo[i].password) > 0 ? adsls->pinfo[i].password : "null");
        index++;

        if (first_adsl)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_adsl = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    DEBUGP("%s: finish\n", __FUNCTION__);
    printf("%s\n", rows);
    return;
}

static void guide_dnat_dump(char *rows, char *row, struct guide_dnat *dnat)
{
    snprintf(rows, ROWS_BUFF_SIZE, "GUIDE_DEFAULT_DNAT=%d", dnat->enable);
    printf("%s\n", rows);
    return;
}

static void guide_zones_dump(char *rows, char *row, struct guide_sel_zone *sel_zones)
{
    int i;
    int first_zone = 1;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_UPDATE_SERVER_INFO=");
    for (i = 0; i < sel_zones->num; i++)
    {            
        snprintf(row, ROW_BUFF_SIZE, "%d__%s__%s",
            sel_zones->pinfo[i].type,
            sel_zones->pinfo[i].oem,
            sel_zones->pinfo[i].isp);

        if (first_zone)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_zone = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    return;
}

static void guide_appbind_dump(char *rows, char *row, struct guide_appbind *appbinds)
{
    char maxupload_str[STRING_LENGTH] = {0};
    char maxdownload_str[STRING_LENGTH] = {0};
    int remainder;
    int  i;
    int  first_appbind = 1;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_APPBINDS=");
    for (i = 0; i < appbinds->num; i++)
    {       
        if (appbinds->pinfo[i].bind_line_id == 0)
        {
            continue;
        }
        
        remainder = appbinds->pinfo[i].maxupload % 1000;
        if (remainder == 0)
        {
            snprintf(maxupload_str, STRING_LENGTH, "%dMbps", appbinds->pinfo[i].maxupload/1000);
        }
        else
        {
            snprintf(maxupload_str, STRING_LENGTH, "%dKbps", appbinds->pinfo[i].maxupload);
        }

        remainder = appbinds->pinfo[i].maxdownload % 1000;
        if (remainder == 0)
        {
            snprintf(maxdownload_str, STRING_LENGTH, "%dMbps", appbinds->pinfo[i].maxdownload/1000);
        }
        else
        {
            snprintf(maxdownload_str, STRING_LENGTH, "%dKbps", appbinds->pinfo[i].maxdownload);
        }
        
        snprintf(row, ROW_BUFF_SIZE, "%d__%d__%d__%d__%s__%s__%d",
            i,
            appbinds->pinfo[i].bind_line_id,
            appbinds->pinfo[i].app_type,
            appbinds->pinfo[i].bypass,
            maxupload_str,
            maxdownload_str,
            appbinds->pinfo[i].weight);

        if (first_appbind)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_appbind = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    return;
}


static void guide_channels_dump(char *rows, char *row, struct guide_tc *tcs, 
    struct guide_if_wan *wans, struct guide_adsl *adsls)
{
    int i;
    int first_zone = 1;
    char upload_str[STRING_LENGTH] = {0};
    char download_str[STRING_LENGTH] = {0};
    int  remainder = 0;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_TC_LINES=");
    get_channel_if_name(tcs, wans, adsls);
    for (i = 0; i < tcs->num; i++)
    {           
        if (tcs->pinfo[i].enable == 0)
        {
            continue;
        }
        
        remainder = tcs->pinfo[i].upload % 1000;
        if (remainder == 0)
        {
            snprintf(upload_str, STRING_LENGTH, "%dMbps", tcs->pinfo[i].upload/1000);
        }
        else
        {
            snprintf(upload_str, STRING_LENGTH, "%dKbps", tcs->pinfo[i].upload);
        }

        remainder = tcs->pinfo[i].download % 1000;
        if (remainder == 0)
        {
            snprintf(download_str, STRING_LENGTH, "%dMbps", tcs->pinfo[i].download/1000);
        }
        else
        {
            snprintf(download_str, STRING_LENGTH, "%dKbps", tcs->pinfo[i].download);
        }
        
        snprintf(row, ROW_BUFF_SIZE, "%d__%d__%d__%d__%d__%s__%s__%s",
            tcs->pinfo[i].line_id,
            tcs->pinfo[i].isp,
            tcs->pinfo[i].quality,
            tcs->pinfo[i].balance,
            0,
            upload_str, 
            download_str,
            tcs->pinfo[i].name);

        if (first_zone)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_zone = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    return;
}

static void guide_firewalls_dump(char *rows, char *row, struct guide_appbind *appbinds)
{
    int  first_fw = 1;
    int index = 0, i;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_APP_FIREWALLS=");
    for (i = 0; i < appbinds->num; i++)
    {
        if (appbinds->pinfo[i].bind_line_id == 0)
        {
            continue;
        }

        snprintf(row, ROW_BUFF_SIZE, "%d__%d__%d__%d",
            index,
            i,
            appbinds->pinfo[i].app_type,
            appbinds->pinfo[i].bypass);
        index++;

        if (first_fw)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_fw = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    
    printf("%s\n", rows);
    return;
}


static void __guide_balances_dump(struct guide_balance_info *balance_info,   
    int *end, int line_id, struct guide_if_wan *wans, struct guide_adsl *adsls)
{
    int idx = *end;
    int index = 0, i;
    int *bandwidth = NULL;
    int bandwidth_len = wans->num + adsls->num;

    DEBUGP("%s: begin, line_id: %d, bandwidth_len: %d\n", __FUNCTION__, line_id, bandwidth_len);
    
    bandwidth = (int *)malloc(sizeof(int)*bandwidth_len);
    if (bandwidth == NULL)
    {
        goto exit;
    }
    
    for (i = 0; i < wans->num; i++)
    {
        if (wans->pinfo[i].line_id != line_id)
        {
            continue;
        }

        balance_info[idx].line_id = line_id;
        strncpy(balance_info[idx].ifname, wans->pinfo[i].ifname, STRING_LENGTH);
        bandwidth[index] = wans->pinfo[i].download;
        idx++;
        index++;
    }

    for (i = 0; i < adsls->num; i++)
    {
        if (adsls->pinfo[i].line_id != line_id)
        {
            continue;
        }

        balance_info[idx].line_id = line_id;
        snprintf(balance_info[idx].ifname, STRING_LENGTH, "adsl-%d", i+1);
        bandwidth[index] = adsls->pinfo[i].download;
        idx++;
        index++;
    }
    
    guide_min_ratio(bandwidth, index);

    idx = *end;
    for (i = 0; i < index; i++)
    {
        balance_info[idx].quota = bandwidth[i];
        idx++;
    }

    *end = *end + index;
    
exit:
    if (bandwidth)
    {
        free(bandwidth);
    }

    return;
}

static void guide_balances_dump(char *rows, char *row, struct guide_tc *tcs, 
    struct guide_if_wan *wans, struct guide_adsl *adsls)
{
    struct guide_balance_info  *balance_info = NULL;
    int                         balance_info_num = (wans->num + adsls->num)*2;
    int                         first_balance = 1;
    int                         count = 0, i;

    rows[0] = '\0';
    snprintf(rows, ROWS_BUFF_SIZE, "%s", "GUIDE_BALANCE_LINES=");
    if (balance_info_num == 0)
    {
        goto exit;
    }

    balance_info = (struct guide_balance_info *)malloc(sizeof(struct guide_balance_info)* balance_info_num);
    if (balance_info == NULL)
    {
        goto exit;
    }
    memset(balance_info, 0, sizeof(struct guide_balance_info)* balance_info_num);
    
    for (i = 0; i < tcs->num; i++)
    {
        if (tcs->pinfo[i].enable == 0)
        {
            continue;
        }

        if (tcs->pinfo[i].balance == 0)
        {
            continue;
        }

        __guide_balances_dump(balance_info, &count, tcs->pinfo[i].line_id, wans, adsls);
    }

    for (i = 0; i < count; i++)
    {
        snprintf(row, ROW_BUFF_SIZE, "%d__%s__%d__%d",
            i+1,
            balance_info[i].ifname,
            balance_info[i].line_id,
            balance_info[i].quota);

        if (first_balance)
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "%s", row);
            first_balance = 0;
        }
        else
        {
            snprintf(rows+strlen(rows), ROWS_BUFF_SIZE-strlen(rows), "#%s", row);
        }
    }

    printf("%s\n", rows);
    
exit:
    if (balance_info)
    {
        free(balance_info);
    }

    return;
}

int main(int argc, char **argv)
{
    struct guide_if_lan     *lans = NULL;
    struct guide_if_wan     *wans = NULL;
    struct guide_server     *servers = NULL;
    struct guide_adsl       *adsls = NULL;
    struct guide_tc         *tcs = NULL;
    struct guide_appbind    *appbinds = NULL;
    struct guide_sel_zone   *sel_zones = NULL;
    struct guide_dnat       *dnat = NULL;

    char                    *row = NULL;
    char                    *rows = NULL;

    int                     size = 0;
    int                     ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    rows = (char *)malloc(ROWS_BUFF_SIZE);
    if (rows == NULL)
    {
        goto exit;
    }
    memset(rows, 0, ROWS_BUFF_SIZE);
    
    row = (char *)malloc(ROW_BUFF_SIZE);
    if (row == NULL)
    {
        goto exit;
    }
    memset(row, 0, ROW_BUFF_SIZE);

	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_LAN, (void**)&lans, &size);
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void**)&servers, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ADSL, (void**)&adsls, &size);
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void**)&tcs, &size);    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_APPBIND, (void**)&appbinds, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ZONE, (void**)&sel_zones, &size);
    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_DNAT, (void**)&dnat, &size);
    
    guide_lans_dump(rows, row, lans);
    guide_servers_dump(rows, row, servers);
    guide_wans_dump(rows, row, wans);
    guide_adsls_dump(rows, row, adsls);
    guide_dnat_dump(rows, row, dnat);
    guide_zones_dump(rows, row, sel_zones);
    guide_appbind_dump(rows, row, appbinds);
    guide_channels_dump(rows, row, tcs, wans, adsls);
    guide_firewalls_dump(rows, row, appbinds);
    guide_balances_dump(rows, row, tcs, wans, adsls);
    
    ret = 0;
exit:
    if (rows)
    {
        free(rows);
    }

    if (row)
    {
        free(row);
    }

    if (lans)
    {
        free_guide_xml_node((void*)&lans);
    }

    if (wans)
    {
        free_guide_xml_node((void*)&wans);
    }

    if (servers)
    {
        free_guide_xml_node((void*)&servers);
    }

    if (adsls)
    {
        free_guide_xml_node((void*)&adsls);
    }

    if (tcs)
    {
        free_guide_xml_node((void*)&tcs);
    }

    if (appbinds)
    {
        free_guide_xml_node((void*)&appbinds);
    }

    if (sel_zones)
    {
        free_guide_xml_node((void*)&sel_zones);
    }

    if (dnat)
    {
        free_guide_xml_node((void*)&dnat);
    }

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__);
    return ret;
}

