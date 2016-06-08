#ifndef __REPORT_CORE_H__
#define __REPORT_CORE_H__

#define REPORT_LONG_NAME        	"V-REPORT"
#define REPORT_NAME                 "ROUTE-REPORT"
#define REPORT_DESCRIPTION      	"ROUTE-REPORT"
#define REPORT_VERSION             	"1.0.0"
#define DEVICE_ID_LENGTH            64

#define LONG_STRING_LENGTH          512

#define REPORT_COMMAND_LENGTH       256
#define BASE_POST_DATA_LENGTH 	    128
#define BASE_WAN_INFO_LENGTH		128
#define BASE_ADSL_INFO_LENGTH		128

#define ROUTE_REPORT_PORTO_NUM      19
#define MD5KEY "1234~!@#$%^&*()QWERTP{}!@#$%^$@"

#define PROTO_POST_URL          "http://www.znwba.com/routeservice/routeprotocolcounts.do"
#define BASE_POST_URL           "http://www.znwba.com/routeservice/routeinfo.do"
#define FLOW_POST_URL           "http://www.znwba.com/routeservice/routespeeds.do"
#define SECUTY_LOG_POST_URL     "http://www.znwba.com/routeservice/routesafes.do"


#define FLOW_FACE_INFO_LENGTH		255
#define FLOW_POST_DATA_LENGTH 	    512
#define PROTO_POST_DATA_LENGTH      1024
#define FLOW_FILE               "/proc/net/dev"
#define FLOW_POST_DATA_TEMP     "{\"data\":[%s,%s,%s,%s],\"deviceid\":\"%s\",\"dtdate\":\"%d\",\"key\":\"%s\"}"
#define FLOW_POST_IFNAME_TEMP   "{\"ifname\":\"%s\",\"downspeed\":\"%lld\",\"upspeed\":\"%lld\",\"dtdate\":\"%d\"}"
#define CURL_SETOPT(x,y,z) do {if(CURLE_OK != curl_easy_setopt(x,y,z)) goto EXIT;} while(0)

#define DEFAULT_REPORT_INTERVAL_TIME        15
#define DEFAULT_REPORT_INTERVAL_MIN_TIME    10
#define DEFAULT_REPORT_INTERVAL_MAX_TIME    600

struct report_config_info
{
    int     base;
    int     flow;
    int     log;
    int     time;
    int     proto;
    int     help;
};

struct http_respone_data
{
	unsigned char* buf;
	size_t buf_len;
	size_t data_len;
};

struct route_face_byte_info
{
	long long int rx_byte;
	long long int tx_byte;

};

struct route_face_byte_infos
{
	int time;
	struct route_face_byte_info LAN0;
	struct route_face_byte_info WAN0;
	struct route_face_byte_info WAN1;
	struct route_face_byte_info WAN2;
};

//Add by chenhao,2015-6-2 11:25:20
typedef struct route_proto_speed_info
{
    char    proto_name[STRING_LENGTH];
    u_int64_t  rx_bytes;            //下行
    u_int64_t  tx_bytes;            //上行
}route_proto_speed_info_t;

#endif

