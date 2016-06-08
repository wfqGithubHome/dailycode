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

/*
 * {"data":[{"conntype":"tcp","newconn":"1000","doubtconn":"1000","loginterval":"600"},{"conntype":"udp","newconn":"1000","doubtconn":"1000","loginterval":"300"},{"conntype":"icmp","newconn":"1000","doubtconn":"1000","loginterval":"300"}],"iserror":0,"msg":""}
 * 
 * {"iserror":1,"msg":""}
 */
int cgiMain()
{  
    char    module[STRING_LENGTH] = {0};
    char    action[STRING_LENGTH] = {0};
	int     ret = GUIDE_OK;

    cgiFormString("module", module, sizeof(module));
    cgiFormString("action", action, sizeof(action));
	
#if 0
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	if (strcmp(module,"welcome") == 0)
    {
		ret = guide_module_welcome();
	}	
	else if(strcmp(module, "lan") == 0)
    {
		ret = guide_module_lan();
	}
    else if(strcmp(module, "wan") == 0)
    {
		ret = guide_module_wan();
	}
    else if (strcmp(module, "server") == 0)
    {
        ret = guide_module_server();
    }
    else if (strcmp(module, "primary") == 0)
    {
        ret = guide_module_primary();
    }
    else if (strcmp(module, "slave") == 0)
    {
        ret = guide_module_slave();
    }
    else if (strcmp(module, "appbind") == 0)
    {
        ret = guide_module_appbind();
    }
    else if(strcmp(module, "zone") == 0)
    {
		ret = guide_module_zone();
	}
    else if(strcmp(module, "dnat") == 0)
    {
		ret = guide_module_dnat();
	}
    else if(strcmp(module, "tc") == 0)
    {
		ret = guide_module_tc();
	}
    else if (strcmp(module, "submit") == 0)
    {
        ret = guide_module_submit();
    }
	else
    {
		ret = GUIDE_NO_SUCH_MODULE;
	}

    if (ret == GUIDE_OK)
    {
        goto exit;
    }

    printf("{\"iserror\":\"%d\",\"msg\":\"\"}", ret);
    return ret;
    
exit:
    if (strcmp(module, "submit") == 0 || 
        strcmp(action, "reset") == 0)
    {
        printf("{\"iserror\":\"0\",\"msg\":\"\"}");
    }
    else
    {
        printf("\"iserror\":\"0\",\"msg\":\"\"}");
    }
    
	return ret;
}




