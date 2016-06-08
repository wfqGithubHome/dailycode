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

static int guide_dnat_show(void)
{
	struct guide_dnat   *dnat = NULL;
    int                 size = 0;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_DNAT, (void**)&dnat, &size);
	if (dnat == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    printf("{\"data\":[");
	printf("{\"enable\":\"%d\"}", dnat->enable);
	printf("],");
    
exit:
    if (dnat != NULL)
    {
	    free_guide_xml_node((void*)&dnat);
    }
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

static int guide_dnat_update(int enable)
{
    struct guide_dnat   *dnat = NULL;
    int                 size = 0;
    int                 ret = GUIDE_OK;
    
    DEBUGP("%s: begin, enable: %d\n", __FUNCTION__, enable);
    
    if (enable != 0 && enable != 1)
    {
        ret = GUIDE_DANT_ENABLE_ERROR;
        goto exit;
    }
    
    size = sizeof(struct guide_dnat);
    dnat = malloc(size);
    if (dnat == NULL)
    {
        ret = GUIDE_DANT_ENABLE_ERROR;
        goto exit;
    }

    dnat->enable = enable;
    save_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_DNAT, (void*)dnat, size);
    
exit:
    if (dnat)
    {
        free(dnat);
    }
    
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

int guide_module_dnat(void)
{
    char action[STRING_LENGTH] = {0};
    char enable_str[STRING_LENGTH] = {0};
    int  enable;
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_dnat_show();
    }
    else if (strcmp(action, "reset") == 0)
    {
        cgiFormString("enable", enable_str, sizeof(enable_str));
        enable = atoi(enable_str);
        ret = guide_dnat_update(enable);
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

