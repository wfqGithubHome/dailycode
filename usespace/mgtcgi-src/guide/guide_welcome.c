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

int guide_module_welcome(void)
{
	char    cmd[CMD_LENGTH] = {0};
    
    snprintf(cmd, CMD_LENGTH, "%s/guide_script.sh welcome", GUIDE_SCRIPT_PATH);
    system(cmd);
    return GUIDE_OK;
    
}

