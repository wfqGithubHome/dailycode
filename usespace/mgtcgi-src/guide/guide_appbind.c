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
#define DEBUG_APPBIND
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

static int guide_appbind_show(void)
{
	struct guide_appbind       *appbinds = NULL;
    int                         first_appbind = 1;
    int                         size = 0, i;
    int                         ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_APPBIND, (void**)&appbinds, &size);
    if (appbinds == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
        goto exit;
    }
    
    printf("{\"data\":[");

    for (i = 0; i < appbinds->num; i++)
    {
    	if (first_appbind == 1)
        {
    		printf("{\"name\":\"%s\",\"app_type\":\"%d\",\"bind_type\":\"%d\",\"bind_line\":\"%d\",\
\"bypass\":\"%d\",\"max_upload\":\"%d\",\"max_download\":\"%d\",\"weight\":\"%d\"}",\
    		appbinds->pinfo[i].name,
            appbinds->pinfo[i].app_type,
            appbinds->pinfo[i].bind_type,
            appbinds->pinfo[i].bind_line_id,
            appbinds->pinfo[i].bypass,
            appbinds->pinfo[i].maxupload,
            appbinds->pinfo[i].maxdownload,
            appbinds->pinfo[i].weight);
    		first_appbind = 0;
    	}
        else
        {
    		printf(",{\"name\":\"%s\",\"app_type\":\"%d\",\"bind_type\":\"%d\",\"bind_line\":\"%d\",\
\"bypass\":\"%d\",\"max_upload\":\"%d\",\"max_download\":\"%d\",\"weight\":\"%d\"}",\
    		appbinds->pinfo[i].name,
            appbinds->pinfo[i].app_type,
            appbinds->pinfo[i].bind_type,
            appbinds->pinfo[i].bind_line_id,
            appbinds->pinfo[i].bypass,
            appbinds->pinfo[i].maxupload,
            appbinds->pinfo[i].maxdownload,
            appbinds->pinfo[i].weight);
    	}
    }
    
	printf("],");
    
exit:
    if (appbinds != NULL)
    {
	    free_guide_xml_node((void*)&appbinds);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static int guide_appbind_update(char *info, int info_num)
{
    struct guide_split_elem *fst_elems = NULL;
    int                     fst_elems_num = 0;
    struct guide_split_elem *snd_elems = NULL;
    int                     snd_elems_num = 0;
    struct guide_appbind   *appbinds = NULL;
    int                     size = 0, idx = 0, i;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_APPBIND
    int                     j;
#endif


    DEBUGP("%s: begin, info: %s, info_num: %d\n", __FUNCTION__, info, info_num);

    size = sizeof(struct guide_appbind) + sizeof(struct guide_appbind_info) * info_num;
    appbinds = malloc(size);
    if (appbinds == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }

    memset(appbinds, 0, size);
    appbinds->num = info_num;
    
    ret = guide_split(info, "--", &fst_elems, &fst_elems_num);
    if (ret != 0 || fst_elems_num != info_num)
    {
        ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
        goto exit;
    }

#ifdef DEBUG_APPBIND
    DEBUGP("%s: first layer parameter parse\n", __FUNCTION__);
    for (j = 0; j < fst_elems_num; j++)
    {
        DEBUGP("%s\n", fst_elems[j].val);
    }
#endif

    for (i = 0; i < info_num; i++)
    {
        ret = guide_split(fst_elems[i].val, "__", &snd_elems, &snd_elems_num);
        if (ret != 0 || snd_elems_num != 8)
        {
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

#ifdef DEBUG_APPBIND
        DEBUGP("%s: second layer parameter parse\n", __FUNCTION__);
        for (j = 0; j < snd_elems_num; j++)
        {
            DEBUGP("%s ", snd_elems[j].val);
        }

        DEBUGP("\n");
#endif
        strncpy(appbinds->pinfo[idx].name, snd_elems[APPBIND_PARAMETER_APP_NAME].val, STRING_LENGTH);
        appbinds->pinfo[idx].app_type = atoi(snd_elems[APPBIND_PARAMETER_APP_TYPE].val);
        appbinds->pinfo[idx].bind_type = atoi(snd_elems[APPBIND_PARAMETER_BIND_TYPE].val);
        appbinds->pinfo[idx].bind_line_id = atoi(snd_elems[APPBIND_PARAMETER_BIND_LINE].val);
        appbinds->pinfo[idx].bypass = atoi(snd_elems[APPBIND_PARAMETER_BYPASS].val);
        appbinds->pinfo[idx].maxupload = atoi(snd_elems[APPBIND_PARAMETER_UPLOAD].val);
        appbinds->pinfo[idx].maxdownload = atoi(snd_elems[APPBIND_PARAMETER_DOWNLOAD].val);
        appbinds->pinfo[idx].weight = atoi(snd_elems[APPBIND_PARAMETER_WEIGHT].val);
        idx++;

        DEBUGP("name: %s, app_type: %s, bind_type: %s, bind_line_id: %s, bypass: %s,\
            maxupload: %s, maxdownload: %s, weight: %s\n", 
            snd_elems[APPBIND_PARAMETER_APP_NAME].val,
            snd_elems[APPBIND_PARAMETER_APP_TYPE].val,
            snd_elems[APPBIND_PARAMETER_BIND_TYPE].val,
            snd_elems[APPBIND_PARAMETER_BIND_LINE].val,
            snd_elems[APPBIND_PARAMETER_BYPASS].val,
            snd_elems[APPBIND_PARAMETER_UPLOAD].val,
            snd_elems[APPBIND_PARAMETER_DOWNLOAD].val,
            snd_elems[APPBIND_PARAMETER_WEIGHT].val);

        free(snd_elems);
        snd_elems = NULL;
        snd_elems_num = 0;
    }

    
    size = sizeof(struct guide_appbind) + sizeof(struct guide_appbind_info) * appbinds->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_APPBIND, (void*)appbinds, size);

exit:
    if (fst_elems)
    {
        free(fst_elems);
    }

    if (snd_elems)
    {
        free(snd_elems);
    }

    if (appbinds)
    {
        free(appbinds);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_appbind(void)
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
        ret = guide_appbind_show();
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
        ret = guide_appbind_update(info, info_num);
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

