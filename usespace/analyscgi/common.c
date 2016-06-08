#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <syslog.h>
#include <limits.h>

#include "common.h"
#include "cgic.h"

#define LOG_LINE_MAX         10240
static char log_buff[LOG_LINE_MAX];
//static char error[ERROR_MAX_ID][];

void do_log(char *log)
{
	int fd = 0;
	fd = open("/home/mgtcgi_log.txt", O_CREAT | O_APPEND | O_WRONLY, 00644);
	
	write(fd, log, strlen(log));
	close(fd);	
}

void log_debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buff, LOG_LINE_MAX, fmt, args);
    do_log(log_buff);
    va_end(args);
}

void error_msg(int errorid)
{
    if(errorid == 0)
        return;
    printf("{\"iserror\":%d,\"msg\":\"parameter error\"}",errorid);

    return;
}
    



