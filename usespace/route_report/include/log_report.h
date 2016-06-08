#ifndef __LOG_REPORT_H__
#define __LOG_REPORT_H__

#define IDS_LOG_MSG_BUFF_MAX        1024
#define MSG_TEM_LEN	128
#define LOG_TEM_LEN	 512
#define POST_DATA_LEN 2048
#define IDS_LOG_LEN	2048
#define NETLINK_IDS_LOG             40
#define IDS_LOG_NETLINK_BASE             0x10
#define IDS_LOG_SHOW            	     (IDS_LOG_NETLINK_BASE+1)
#define IDS_LOG_FLUSH            	     (IDS_LOG_NETLINK_BASE+2)
#define IDS_LOG_NETLINK_MAX              (IDS_LOG_NETLINK_BASE+3)

#define IDS_LOG_DONE           	         0
#define IDS_LOG_ERROR          	         1
#define IDS_LOG_MEMERR        	         2
#define IDS_LOG_SHOW_DONE 	             3
#define LOG_POST_OPT_TEMP "{\"level\": \"%d\",\"caption\":\"%s\", \"style\": \"%s\",\"dtdate\": \"%d\",\"ip\": \"%s\"}"
#define LOG_POST_DATA_TEMP "{\"data\": [%s],\"deviceid\":\"%s\",\"dtdate\":\"%d\",\"key\":\"%s\"}"

typedef struct ids_log_msg
{
	uint16_t         msg_len;
    char             message[IDS_LOG_MSG_BUFF_MAX];
} ids_log_msg_t;

typedef struct ids_log_respond_info
{
	struct nlmsghdr     nlhdr;
	struct ids_log_msg  msg;
} ids_log_respond_info_t;

extern int display_ids_log(char *);
extern char *strptime(const char *buf,const char *format, struct tm *tm);
extern int get_curr_unixtime(void) ;

#endif
