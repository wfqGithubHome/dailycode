#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>


#define ERROR_MAX_ID 20

#define PARAMTER_ERROR  1

void error_msg(int errorid);
void log_debug(const char* fmt, ...);

int channel_show_stat_all(int ifindex, uint8_t dir);

#endif

