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

static int guide_tc_show(void)
{
	struct guide_tc     *tcs = NULL;
    int                 size = 0, i;
    int                 first_line = 1;
    int                 ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
	get_guide_xml_node(GUIDE_XML_FILE, GUIDE_TYPE_TC, (void**)&tcs, &size);
	if (tcs == NULL)
    {
        ret = GUIDE_READ_FILE_ERROR;
		goto exit;
	}

    printf("{\"data\":[");
	for (i = 0; i < tcs->num; i++)
    {
        if (first_line)
        {
    		printf("{\"name\":\"%s\",\"lineid\":\"%d\",\"isp\":\"%d\",\"quality\":\"%d\",\"balance\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"enable\":\"%d\"}",\
    			tcs->pinfo[i].name, tcs->pinfo[i].line_id, tcs->pinfo[i].isp, tcs->pinfo[i].quality, 
    			tcs->pinfo[i].balance, tcs->pinfo[i].upload, tcs->pinfo[i].download, 
    			tcs->pinfo[i].enable);
            first_line = 0;
        }
        else
        {
            printf(",{\"name\":\"%s\",\"lineid\":\"%d\",\"isp\":\"%d\",\"quality\":\"%d\",\"balance\":\"%d\",\
\"upload\":\"%d\",\"download\":\"%d\",\"enable\":\"%d\"}",\
    			tcs->pinfo[i].name, tcs->pinfo[i].line_id, tcs->pinfo[i].isp, tcs->pinfo[i].quality, 
    			tcs->pinfo[i].balance, tcs->pinfo[i].upload, tcs->pinfo[i].download, 
    			tcs->pinfo[i].enable);
        }
	}
	printf("],");    
    
exit:
    if (tcs != NULL)
    {
	    free_guide_xml_node((void*)&tcs);
    }
    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
    
}

int guide_module_tc(void)
{
    char action[STRING_LENGTH] = {0};
    int  ret = GUIDE_OK;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    cgiFormString("action", action, sizeof(action));
    if (strcmp(action, "list") == 0)
    {
        ret = guide_tc_show();
    }
    else
    {
        ret = GUIDE_NO_SUCH_ACTION;
    }   

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
    return ret;
}
