#ifndef _GUIDE_XMLCONFIG_
#define _GUIDE_XMLCONFIG_

#include<stdint.h>

#define GUIDE_XML_FILE    "/tmp/guide.xml"
#define GUIDE_SCRIPT_PATH "/usr/local/httpd/htdocs/cgi-bin/sh"

#define STRING_LENGTH       64
#define URL_LENGTH			256
#define COMMENT_LENGTH      256
#define CMD_LENGTH          512


#define GUIDE_TYPE_LAN               100
#define GUIDE_TYPE_SERVER            101
#define GUIDE_TYPE_WAN               102
#define GUIDE_TYPE_ADSL              103
#define GUIDE_TYPE_TC                104
#define GUIDE_TYPE_APPBIND           105
#define GUIDE_TYPE_ZONE              106
#define GUIDE_TYPE_DNAT              107

#define GUIDE_ISP_TYPE_TEL  1
#define GUIDE_ISP_TYPE_CNC  2

#define GUIDE_LINE_QUALITY_FIBER 1
#define GUIDE_LINE_QUALITY_ADSL  2

#define GUIDE_PRIMARY_LINE_ID 1
#define GUIDE_SLAVE1_LINE_ID  2
#define GUIDE_SLAVE2_LINE_ID  3

typedef enum guide_server_parameter
{
    SERVER_PARAMETER_PARENT_ID,
    SERVER_PARAMETER_VALUE,
    ERVER_PARAMETER_MAX,
} guide_server_parameter_t;

typedef enum guide_cable_common_parameter 
{
    CABLE_PARAMETER_LINE_ID,
    CABLE_PARAMETER_IFNAME,
    CABLE_PARAMETER_ISP,
    CABLE_PARAMETER_QUALITY,
    CABLE_PARAMETER_UPLOAD,
    CABLE_PARAMETER_DOWNLOAD,
} guide_cable_common_parameter_t;

typedef enum guide_cable_fiber_parameter
{
    CABLE_FIBER_PARAMETER_IPADDR         = 6,
    CABLE_FIBER_PARAMETER_NETMASK        = 7,
    CABLE_FIBER_PARAMETER_GATEWAY        = 8,
    CABLE_FIBER_PARAMETER_MAC            = 9,
    CABLE_FIBER_PARAMETER_CONNTYPE       = 10,
    CABLE_FIBER_PARAMETER_MAX            = 11,
} guide_cable_fiber_parameter_t;

typedef enum guide_cable_adsl_parameter
{
    CABLE_ADSL_PARAMETER_USERNAME    = 6,
    CABLE_ADSL_PARAMETER_PASSWORD    = 7,
    CABLE_ADSL_PARAMETER_MAX         = 8,
} guide_cable_adsl_parameter_t;

typedef enum guide_zone_parameter
{
    ZONE_PARAMETER_TYPE,
    ZONE_PARAMETER_OEM,
    ZONE_PARAMETER_ISP,
    ZONE_PARAMETER_MAX,
} guide_zone_parameter_t;

typedef enum guide_appbind_parameter 
{
    APPBIND_PARAMETER_APP_NAME,
    APPBIND_PARAMETER_APP_TYPE,
    APPBIND_PARAMETER_BIND_TYPE,
    APPBIND_PARAMETER_BIND_LINE,
    APPBIND_PARAMETER_BYPASS,
    APPBIND_PARAMETER_UPLOAD,
    APPBIND_PARAMETER_DOWNLOAD,
    APPBIND_PARAMETER_WEIGHT,
    APPBIND_PARAMETER_MAX,
} guide_appbind_parameter_t;

typedef enum guide_conn_type 
{
    CONN_TYPE_NONE,
    CONN_TYPE_PRIMARY,
    CONN_TYPE_SLAVE,
} guid_conn_type_t;


typedef struct guide_server_info
{
    int  parentid;
    char value[STRING_LENGTH];
} guide_server_info_t;


typedef struct guide_server 
{
    unsigned short num;
    struct guide_server_info pinfo[0];
} guide_server_t;

typedef struct guide_if_lan_info
{
    char ifname[STRING_LENGTH];
    char ipaddr[STRING_LENGTH];
    char netmask[STRING_LENGTH];
    char mac[STRING_LENGTH];
} guide_if_lan_info_t;

typedef struct guide_if_lan 
{
    unsigned short num;
    struct guide_if_lan_info pinfo[0];
} guide_if_lan_t;

typedef struct guide_if_wan_info
{
    char ifname[STRING_LENGTH];
    int  isp;
    int  quality;
    int  line_id;
    int  upload;
    int  download;
    int  conntype;
    char mac[STRING_LENGTH];
    char ipaddr[STRING_LENGTH];
    char netmask[STRING_LENGTH];
    char gateway[STRING_LENGTH];
} guide_if_wan_info_t;

typedef struct guide_if_wan
{
    unsigned short num;
    struct guide_if_wan_info pinfo[0];
} guide_if_wan_t;

typedef struct guide_adsl_info
{
    char ifname[STRING_LENGTH];
    int  isp;
    int  quality;
    int  line_id;
    int  upload;
    int  download;
    char username[STRING_LENGTH];
    char password[STRING_LENGTH];

} guide_adsl_info_t;

typedef struct guide_adsl
{
    unsigned short num;
    struct guide_adsl_info pinfo[0];    
} guide_adsl_t;

typedef struct guide_tc_info
{
    char name[STRING_LENGTH];
    int  line_id;
    int  isp;
    int  quality;
    int  balance;
    int  upload;
    int  download;
    int  enable;
} guide_tc_info_t;

typedef struct guide_tc
{
    unsigned short num;
    struct guide_tc_info pinfo[0];
} guide_tc_t;

typedef struct guide_appbind_info
{
    char name[STRING_LENGTH];
    int  app_type;
    int  bind_type;
    int  bind_line_id;
    int  bypass;
    int  maxupload;
    int  maxdownload;
    int  weight;
} guide_appbind_info_t;

typedef struct guide_appbind 
{
    unsigned short num;
    struct guide_appbind_info pinfo[0];
} guide_appbind_t;

typedef struct guide_dnat 
{
    int enable;
} guide_dnat_t;

typedef struct guide_zone_info
{
    uint8_t type;
    char    name[STRING_LENGTH];
    char    en_name[STRING_LENGTH];
    char    zonename[STRING_LENGTH];
    char    en_zonename[STRING_LENGTH];
    char    var1[URL_LENGTH];
    char    var2[URL_LENGTH];
    char    var3[URL_LENGTH];
    char    var4[URL_LENGTH];
    char    comment[COMMENT_LENGTH];
    int     selected;
} guide_zone_info_t;

typedef struct guide_zone
{
    unsigned short          num;
    struct guide_zone_info  pinfo[0];
} guide_zone_t;

typedef struct guide_sel_zone_info
{
    int   type;
    char  oem[STRING_LENGTH];
    char  isp[STRING_LENGTH];
} guide_sel_zone_info_t;

typedef struct guide_sel_zone
{
    unsigned short               num;
    struct  guide_sel_zone_info  pinfo[0];
} guide_sel_zone_t;


#endif
