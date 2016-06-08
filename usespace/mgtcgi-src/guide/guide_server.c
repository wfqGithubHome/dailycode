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
#define DEBUG_SERVER
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

static int guide_server_show(void)
{
	struct guide_server         *servers = NULL;
    int                         first_server = 1;
    int                         size = 0, i;
    int                         ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);

    get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void**)&servers, &size);
    if (servers == NULL)
    {
        DEBUGP("%s: can not find server info\n", __FUNCTION__);
        ret = GUIDE_READ_FILE_ERROR;
        goto exit;
    }
    
    printf("{\"data\":[");

    for (i = 0; i < servers->num; i++)
    {
    	if (first_server == 1)
        {
    		printf("{\"parent_id\":\"%d\",\"value\":\"%s\"}",\
                    servers->pinfo[i].parentid,
                    servers->pinfo[i].value);
    		first_server = 0;
    	}
        else
        {
    		printf(",{\"parent_id\":\"%d\",\"value\":\"%s\"}",\
                    servers->pinfo[i].parentid,
                    servers->pinfo[i].value);
    	}
    }
    
	printf("],");
    
exit:
    if (servers != NULL)
    {
	    free_guide_xml_node((void*)&servers);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static int __guide_server_update(char *info, int info_num)
{
    struct guide_split_elem *fst_elems = NULL;
    int                     fst_elems_num = 0;
    struct guide_split_elem *snd_elems = NULL;
    int                     snd_elems_num = 0;
    struct guide_server   *servers = NULL;
    int                     size = 0, idx = 0, i;
    int                     ret = GUIDE_OK;
#ifdef DEBUG_SERVER
    int                     j;
#endif

    DEBUGP("%s: begin, info: %s, info_num: %d\n", __FUNCTION__, info, info_num);

    size = sizeof(struct guide_server) + sizeof(struct guide_server_info) * info_num;
    servers = malloc(size);
    if (servers == NULL)
    {
        ret = GUIDE_MEMORY_ALLOC_FAILED;
        goto exit;
    }

    memset(servers, 0, size);
    servers->num = info_num;
    
    ret = guide_split(info, "--", &fst_elems, &fst_elems_num);
    if (ret != 0 || fst_elems_num != info_num)
    {
        ret = GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR;
        goto exit;
    }

#ifdef DEBUG_SERVER
    DEBUGP("%s: first layer parameter parse\n", __FUNCTION__);
    for (j = 0; j < fst_elems_num; j++)
    {
        DEBUGP("%s\n", fst_elems[j].val);
    }
#endif

    for (i = 0; i < info_num; i++)
    {
        ret = guide_split(fst_elems[i].val, "__", &snd_elems, &snd_elems_num);
        if (ret != 0 || snd_elems_num != ERVER_PARAMETER_MAX)
        {
            ret = GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR;
            goto exit;
        }

#ifdef DEBUG_SERVER
        DEBUGP("%s: second layer parameter parse\n", __FUNCTION__);
        for (j = 0; j < snd_elems_num; j++)
        {
            DEBUGP("%s\n", snd_elems[j].val);
        }
#endif

        servers->pinfo[idx].parentid = atoi(snd_elems[SERVER_PARAMETER_PARENT_ID].val);
        strncpy(servers->pinfo[idx].value, snd_elems[SERVER_PARAMETER_VALUE].val, STRING_LENGTH);
        idx++;

        free(snd_elems);
        snd_elems = NULL;
        snd_elems_num = 0;
    }

    
    size = sizeof(struct guide_server) + sizeof(struct guide_server_info) * servers->num;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_SERVER, (void*)servers, size);

exit:
    if (fst_elems)
    {
        free(fst_elems);
    }

    if (snd_elems)
    {
        free(snd_elems);
    }

    if (servers)
    {
        free(servers);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

static int guide_server_update(void)
{
    char *info = NULL;
    char info_num_str[STRING_LENGTH] = {0};
    int  info_num = 0;
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
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

    ret = __guide_server_update(info, info_num);
exit:
    if (info)
    {
        free(info);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
   
}

int guide_module_server(void)
{
    char action[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_server_show();
    }
    else if (strcmp(action, "reset") == 0)
    {
        ret = guide_server_update();
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}
