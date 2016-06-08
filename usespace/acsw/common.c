#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <link.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/utsname.h>

#include "include/common.h"
#include "include/ac_report.h"

#if 1
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif


void ten_second_delay(void)
{
    int i;
    
    for(i = 0; i < 10; i++)
    {
         usleep(1000000);
    }
}

int device_hwid_read(unsigned char *hwid)
{
    int ret = -1;
    FILE *fd = NULL;
    char buf[DEVICE_ID_LENGTH];

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    memset(buf, 0, DEVICE_ID_LENGTH); 
    fd = fopen("/etc/HWID", "r");
    if (fd == NULL)
    {
        goto exit;
    }

    if (fgets(buf, DEVICE_ID_LENGTH, fd))
    {
        strncpy((char *)hwid, buf, 32);
        ret = 0;
    }
    
exit:
    if (fd)
    {
        fclose(fd);
    }

    DEBUGP("%s: finish, ret: %d, hwid: %s\n", __FUNCTION__, ret, hwid);
    return ret;
}

void get_timestamp(time_t *timestamp)
{
    time_t tmp;
	tmp = time(NULL);

    *timestamp = tmp - 8*60*60;
}


