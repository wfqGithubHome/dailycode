#include <stdio.h>
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
#define DEBUG_ZONE
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

static int guide_zone_show(void)
{
	struct guide_zone       *zones = NULL;
    struct guide_sel_zone   *sel_zones = NULL;
    int                     first_zone = 1;
    int                     size = 0, i, j;
    int                     ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_xml_nodeforservinfo(MGTCGI_XMLTMP, SERV_XMLFILE,(void**)&zones, &size);
	if (zones == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ZONE, (void**)&sel_zones, &size);
    for (i = 0; i < zones->num; i++)
    {        
        DEBUGP("%s: zones[%d]: type: %u, enname: %s, en_zonename: %s\n", 
            __FUNCTION__, i, 
            zones->pinfo[i].type, 
            zones->pinfo[i].en_name,
            zones->pinfo[i].en_zonename);

        zones->pinfo[i].selected = 0;
        for (j = 0; j < sel_zones->num; j++)
        {
            DEBUGP("%s: sel_zones[%d]: type: %u, oem: %s, isp: %s\n", 
                __FUNCTION__, j, 
                sel_zones->pinfo[j].type, 
                sel_zones->pinfo[j].oem,
                sel_zones->pinfo[j].isp);
            
            if (zones->pinfo[i].type == sel_zones->pinfo[j].type && 
                strcmp(zones->pinfo[i].en_name, sel_zones->pinfo[j].oem) == 0 &&
                strcmp(zones->pinfo[i].en_zonename, sel_zones->pinfo[j].isp) == 0)
            {
                zones->pinfo[i].selected = 1;
            }
        }
    }
    
    printf("{\"data\":[");

    for (i = 0; i < zones->num; i++)
    {
    	if (first_zone == 1)
        {
    		printf("{\"type\":\"%u\",\"name\":\"%s\",\"en_name\":\"%s\",\"zonename\":\"%s\",\"en_zonename\":\"%s\",\"comment\":\"%s\",\"selected\":\"%d\"}",\
    		zones->pinfo[i].type,
            zones->pinfo[i].name,
            zones->pinfo[i].en_name,
            zones->pinfo[i].zonename,
            zones->pinfo[i].en_zonename,
            zones->pinfo[i].comment,
            zones->pinfo[i].selected);
    		first_zone = 0;
    	}
        else
        {
    		printf(",{\"type\":\"%u\",\"name\":\"%s\",\"en_name\":\"%s\",\"zonename\":\"%s\",\"en_zonename\":\"%s\",\"comment\":\"%s\",\"selected\":\"%d\"}",\
    		zones->pinfo[i].type,
            zones->pinfo[i].name,
            zones->pinfo[i].en_name,
            zones->pinfo[i].zonename,
            zones->pinfo[i].en_zonename,
            zones->pinfo[i].comment,
            zones->pinfo[i].selected);
    	}
    }
    
	printf("],");
        
exit:
    if (zones != NULL)
    {
	    free_guide_xml_node((void*)&zones);
    }

    if (sel_zones)
    {
        free_guide_xml_node((void*)&sel_zones);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static int guide_zone_update(char *info, int info_num)
{
    struct guide_split_elem *fst_elems = NULL;
    int                     fst_elems_num = 0;
    struct guide_split_elem *snd_elems = NULL;
    int                     snd_elems_num = 0;
    struct guide_sel_zone   *sel_zones = NULL;
    int                     size = 0, idx = 0, i;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_ZONE
    int                     j;
#endif

    DEBUGP("%s: begin, info: %s, info_num: %d\n", __FUNCTION__, info, info_num);

    size = sizeof(struct guide_sel_zone) + sizeof(struct guide_sel_zone_info) * info_num;
    sel_zones = malloc(size);
    if (sel_zones == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }

    memset(sel_zones, 0, size);
    
    ret = guide_split(info, "--", &fst_elems, &fst_elems_num);
    if (ret != 0 || fst_elems_num != info_num)
    {
        ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
        goto exit;
    }

#ifdef DEBUG_ZONE
    DEBUGP("%s: first layer parameter parse\n", __FUNCTION__);
    for (j = 0; j < fst_elems_num; j++)
    {
        DEBUGP("%s\n", fst_elems[j].val);
    }
#endif

    for (i = 0; i < info_num; i++)
    {
        ret = guide_split(fst_elems[i].val, "__", &snd_elems, &snd_elems_num);
        if (ret != 0 || snd_elems_num != ZONE_PARAMETER_MAX)
        {
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

#ifdef DEBUG_ZONE
        DEBUGP("%s: second layer parameter parse\n", __FUNCTION__);
        for (j = 0; j < snd_elems_num; j++)
        {
            DEBUGP("%s\n", snd_elems[j].val);
        }
#endif

        sel_zones->pinfo[idx].type = atoi(snd_elems[ZONE_PARAMETER_TYPE].val);
        strncpy(sel_zones->pinfo[idx].oem, snd_elems[ZONE_PARAMETER_OEM].val, STRING_LENGTH);
        strncpy(sel_zones->pinfo[idx].isp, snd_elems[ZONE_PARAMETER_ISP].val, STRING_LENGTH);
        sel_zones->num += 1;
        idx++;

        free(snd_elems);
        snd_elems = NULL;
        snd_elems_num = 0;
    }

    
    size = sizeof(struct guide_sel_zone) + sizeof(struct guide_sel_zone_info) * sel_zones->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_ZONE, (void*)sel_zones, size);

exit:
    if (fst_elems)
    {
        free(fst_elems);
    }

    if (snd_elems)
    {
        free(snd_elems);
    }

    if (sel_zones)
    {
        free(sel_zones);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_zone(void)
{
    char action[STRING_LENGTH] = {0};
    char *info = NULL;
    char info_num_str[STRING_LENGTH] = {0};
    int  info_num = 0;
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_zone_show();
    }
    else if (strcmp(action, "reset") == 0)
    {
        cgiFormString("info_num", info_num_str, sizeof(info_num_str));
        info_num = atoi(info_num_str);
        if (info_num <= 0)
        {   
            ret = GUIDE_TOO_FEW_PARAMETER;
            goto exit;
        }

        info = malloc(MAX_SEGMENT_LENGTH * info_num);
        if (info == NULL)
        {
            ret = GUIDE_MEMORY_ALLOC_FAILED;
            goto exit;
        }

        memset(info, 0, MAX_SEGMENT_LENGTH * info_num);
        cgiFormString("info", info, MAX_SEGMENT_LENGTH * info_num);
        ret = guide_zone_update(info, info_num);
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

exit:
    if (info)
    {
        free(info);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

