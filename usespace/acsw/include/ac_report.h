#ifndef __AC_REPORT_H__
#define __AC_REPORT_H__

#define AC_REPORT_URL               "http://www.rennishang.com" 
#define AC_REPORT_SETTING_SUB_URL   "acclient/setting"
#define AC_REPORT_PORTAL_SUB_URL    "acclient/portal"
#define AC_REPORT_STATE_SUB_URL     "acclient/acstate"
#define AC_REPORT_WARNING_SUB_URL   "acclient/warning"
#define AC_REPORT_GETCONFIG_SUB_URL "acclient/getconfig"
#define AC_REPORT_GETAPP_SUB_URL    "acclient/getapp"
#define AC_REPORT_GETWEBFILE_SUB_URL "acclient/webfile"


#define GET_WEBFILEINFO_FILENAME       "/tmp/webfileinfo.inf"
#define DEVICE_WEBFILEINFO_FILENAME    "/etc/webfileinfo.inf"
#define DEVICE_WEBFILEINFO_FLASHFILE   "/wifi/webfileinfo.inf"

#define DEVICE_WIFISEVER_ROOTPAHT      "/wifi"

#define AC_REPORT_MAGIC "ackeyUHB6a7d8aabwscdxevfrTGBZAQ"
#define AC_PORTMAP_FILEPATH		"/etc/acportmap"

#define STRING_LENGTH           64
#define MD5SUM_LENGTH           64
#define DEVICE_ID_LENGTH        64
#define MAC_ADDRESS_LENGTH      6
#define ERROR_MESSAGE_LENGTH    256
#define RETURN_SIGN_LENGTH      64
#define COMMAND_STRING_LENGTH   256

#define IP_STRING_LENGTH        16
#define APP_NAME_LENGTH         64

#define APP_URL_LENGTH          512
#define APP_VERSION_LENGTH      32

#define AC_AP_TIMEOUT_WARING_THRESHOLD_LOW  5*60
#define AC_AP_TIMEOUT_WARING_THRESHOLD_HIGH 50*60

#define WEBFILE_NAME_LENGTH     256

#define BUFFER_LEN              128
#define CMD_LEN                 256

#define PORT_MAP_BUF_LENGTH     1024

/* for request pdu */
enum ac_report_warning_type
{
    AC_REPORT_WARNING_TYPE_NONE,
    AC_REPORT_WARNING_TYPE_TIMEOUT,
    AC_REPORT_WARNING_TYPE_RESET,
    AC_REPORT_WARNING_TYPE_OVERHEAD,
    AC_REPORT_WARNING_TYPE_MAX,
};

struct ac_report_mac
{
    unsigned char mac[MAC_ADDRESS_LENGTH];
};

/* for respond pdu */
struct ac_respond_mac_time
{
    unsigned char   mac[MAC_ADDRESS_LENGTH];
    int             time;
};

struct ac_respond_getapp_unit
{
    char app_name[APP_NAME_LENGTH];
    int  app_size;
    int  app_id;
    char app_url[APP_URL_LENGTH];
    char app_version[APP_VERSION_LENGTH];
    char md5sum[MD5SUM_LENGTH];
    int  status;
};

/* request pdu data and repond pdu data */
struct ac_report_setting
{
    char    ac_id[DEVICE_ID_LENGTH];
    char    ap_id[DEVICE_ID_LENGTH];
    int     cache_toplimit;
    int     wifi_conn_toplimit;
    int     timespan;
};

struct ac_respond_setting
{
    int     error;
};

struct ac_report_portal
{
    char            ac_id[DEVICE_ID_LENGTH];
    unsigned char   app_mac[MAC_ADDRESS_LENGTH];
};

struct ac_report_state
{
    int     ac_count;
    int     wifi_client_count;
    int     up_data;
    int     down_data;
    char    ac_id[DEVICE_ID_LENGTH];
    int     mac_count;
    struct ac_report_mac mac[0];
};

struct ac_respond_state
{
    int                         error;
    int                         mac_time_count;
    struct  ac_respond_mac_time  mac_time[0];
};

struct ac_report_warning
{
    enum ac_report_warning_type warning_type;
    char                        ap_id[DEVICE_ID_LENGTH];
    char                        ac_id[DEVICE_ID_LENGTH];
};

struct ac_respond_warning
{
    int     error;
};  

struct ac_report_getconfig
{
    char ac_id[DEVICE_ID_LENGTH];
};

struct ac_report_getportmap
{
    char ac_id[DEVICE_ID_LENGTH];
    char model[STRING_LENGTH];
    char serics[STRING_LENGTH];
};


struct ac_respond_getconfig
{
    int             error;
    char            portal[APP_URL_LENGTH];
    int             cache_toplimit;
    int             timespan;
    int             wifi_Conn_toplimit;
    int             mac_time_count;
    int             ip_count;
    int             sip_count;
    int             ips_offset;
    int             sips_offset;
    unsigned char   buff[0];
};

struct ac_report_getapp
{
    char ac_id[DEVICE_ID_LENGTH];
};

struct ac_respond_getapp
{
    int     error;
    int     unit_count;
    struct ac_respond_getapp_unit units[0];
};

struct ac_respond_getportmap
{
    int     code;
    int     dport;
    int     sport;
    char    ip[STRING_LENGTH];
};
struct ac_respond_getportmaps{
	int num;
	struct ac_respond_getportmap pinfo[0];
};
struct ac_report_getwebfile
{
    char ac_id[DEVICE_ID_LENGTH];
};

struct ac_respond_webfile_unit
{
    char file_path[APP_NAME_LENGTH];
    char file_name[WEBFILE_NAME_LENGTH];
    char md5sum[MD5SUM_LENGTH];
};

struct ac_respond_webfile_info
{
    int     version;
    char    getfile_url[APP_URL_LENGTH];
};


extern int do_ac_report_setting(char *ac_url, struct ac_report_setting *report, struct ac_respond_setting **respond);
extern int do_ac_report_portal(char *ac_url, struct ac_report_portal *report);
extern int do_ac_report_state(char *ac_url, struct ac_report_state *report, struct ac_respond_state **respond);
extern int do_ac_report_warning(char *ac_url, struct ac_report_warning *report, struct ac_respond_warning **respond);
extern int do_ac_report_getconfig(char *ac_url, struct ac_report_getconfig *report, struct ac_respond_getconfig **respond);
extern int do_ac_report_getapp(char *ac_url, struct ac_report_getapp *report, struct ac_respond_getapp **respond);
extern int do_ac_report_getwebfile(char *ac_url, struct ac_report_getwebfile *report);
extern int do_ac_report_getportmap(const char *url, struct ac_report_getportmap *portmap, struct ac_respond_getportmaps **respond_portmap);

extern int ac_webfile_system_update(char *webfileinfo, char *devicewebfileinfo);
extern int do_ac_port_map(char *iface,char *sip,char *dip, unsigned int sport, unsigned int dport);
extern int get_wan_ip(char *ip,int iplen,char *iface,int ifacelen);

#endif

