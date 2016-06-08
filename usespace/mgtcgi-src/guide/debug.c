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

#include "include/debug.h"

#define LOG_LINE_MAX         1024

static char log_buff[LOG_LINE_MAX];

void do_log(char *log)
{
	int fd = 0;
	fd = open("/home/guide.txt", O_CREAT | O_APPEND | O_WRONLY, 00644);
	
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

