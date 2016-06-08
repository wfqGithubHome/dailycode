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

static int guide_wan_show(void)
{
	struct guide_if_wan *wans = NULL;
    int                 size = 0, i;
    int                 first_line = 1;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_WAN, (void**)&wans, &size);
	if (wans == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    printf("{\"data\":[");
	for (i = 0; i < wans->num; i++)
    {
        if (first_line)
        {
    		printf("{\"ifname\":\"%s\",\"conntype\":\"%d\"}",\
		        wans->pinfo[i].ifname, wans->pinfo[i].conntype);
            first_line = 0;
        }
        else
        {
            printf(",{\"ifname\":\"%s\",\"conntype\":\"%d\"}",\
		        wans->pinfo[i].ifname, wans->pinfo[i].conntype);
        }
	}
	printf("],");
        
exit:
    if (wans != NULL)
    {
	    free_guide_xml_node((void*)&wans);
    }
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

int guide_module_wan(void)
{
    char action[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_wan_show();
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}

