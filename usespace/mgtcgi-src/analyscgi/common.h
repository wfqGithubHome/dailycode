#ifndef _COMMON_H_
#define _COMMON_H_

#define ERROR_MAX_ID 20

#define PARAMTER_ERROR  1

void error_msg(int errorid);
void log_debug(const char* fmt, ...);

int channel_show_stat_all(char *intername);
#endif

