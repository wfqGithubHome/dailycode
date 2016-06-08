#ifndef _MGTCGI_XMLCONFIG_
#define _MGTCGI_XMLCONFIG_

#define SERV_XMLFILE                        "/etc/servinfo.xml"                     
#define L7_XMLFILE							"/etc/protoinfo.xml"
#define MGTCGI_XMLTMP						"/etc/config.xml"
#define ROUTE_CONFIG_FILE					"/etc/route_config.xml"
#define MAC_BIND_FILE                                       "/etc/mac_bind.xml"
#define GUIDE_XML_FILE					    "/tmp/guide.xml"
#define BACKUP_XMLTMP_CMD					"/usr/local/httpd/htdocs/cgi-bin/sh/xmlconf.sh create"
#define ROUTE_BACKUP_XMLTMP_CMD				"/usr/local/httpd/htdocs/cgi-bin/sh/xmlconf.sh create"
#define GET_MAX_BRIDGE_NUM					"/usr/local/sbin/xmlconf -f /etc/config.xml -r -k \"xmlconfig/system/max_bridgenum\" | awk '{print $3}'"
#define GET_BRIDGE_NUM						"/usr/local/sbin/xmlconf -f /etc/config.xml -r -k \"xmlconfig/system/bridgenum\" | awk '{print $3}'"
#define GET_TOTAL_SESSION_LIMIT				"/bin/cat /proc/sys/net/dpifilter/state/ipv4_state_max 2>/dev/null"

//add by chenhao2014-11-4 23:25:06
#define PPPD_ADSL_LOG_FILE_PATH                  "/var/log/pppd.log"

#define	PAGE_LITTLE_VALUE					1
#define	STRING_LITTLE_LENGTH				1
#define BRIDGE_LITTLE_VALUE					0

#define MAX_BRIDGE_NUM						5
#define DIGITAL_LENGTH						20
#define STRING_LENGTH						64
#define URL_LENGTH							256
#define COMMENT_LENGTH						256
#define L7_COMMENT_LENGTH					512
//#define CMD_LENGTH							1024
#define CMD_LENGTH							512

#define BUF_LENGTH_4K						4096
#define BUF_LENGTH							40960
#define STRING_LIST_LENGTH					102400
#define LOG_LINE_MAX						102400
#define LICENSE_KEY_SIZE					1152


#define	GET_STRING_LENG	20

#define   GET_MAX_ADSL_LOG     20



//web错误代码
#define MGTCGI_SUCCESS				0
#define MGTCGI_PARAM_ERR			1
#define MGTCGI_DUPLICATE_ERR		2
#define MGTCGI_READ_FILE_ERR		3
#define MGTCGI_WRITE_FILE_ERR		4
#define MGTCGI_IP_DUPLICATE_ERR		5
#define MGTCGI_USED_ERR				6
#define MGTCGI_NO_TARGET_ERR		7
#define MGTCGI_XMLCONF_ERR			8
#define MGTCGI_APPY_ERR				9
#define MGTCGI_TOTAL_SESSION_ERR	10
#define MGTCGI_IMPORT_FPATH_ERR		11
#define MGTCGI_IMPORT_FOPEN_ERR		12
#define MGTCGI_IMPROT_FTYPE_ERR		13
#define MGTCGI_IMPROT_FSAVE_ERR		14
#define MGTCGI_IMPROT_FILE_BAD		15

#define MGTCGI_PER_SESSION_ERR		20
#define MGTCGI_EDIT_DEFAULT_ERR		21
#define MGTCGI_APPLY_ERR			22
#define MGTCGI_SAVE_FLASH_ERR		23
#define MGTCGI_ADSL_DIAL_ERR		24
#define MGTCGI_VLAN_USED_ERR		25

#define MGTCGI_REMOTE_UPDATE_SERVER_CLOSE 26
#define MGTCGI_REMOTE_UPDATE_ARG_ERR      27
#define MGTCGI_REMOTE_UPDATE_LAST_VER     28
#define MGTCGI_REMOTE_UPDATE_NO_ZONE      29


//与html交互的id定义
#define MGTCGI_SYSTEM_ID				1	//系统
#define MGTCGI_MANAGEMENT_ID			2	//管理接口
#define MGTCGI_INTERFACE_ID				3	//数据接口
#define MGTCGI_BRIDGE_NUM_ID			4	//获取桥的数量
#define MGTCGI_DEVICE_VERSION_ID		5	//设置版本信息
#define MGTCGI_INTERFACE_NAME_ID		6	//根据桥ID，获得接口名称
#define MGTCGI_AUTOUPDATE_ENABLE_ID     7

#define MGTCGI_NETWORKS_ID				10	//IP地址对象
#define MGTCGI_NETWORKGROUPS_ID			11	//IP地址对象组
#define MGTCGI_SCHEDULES_ID				12	//时间对象
#define MGTCGI_SCHEDULEGROUPS_ID		13	//时间对象组
#define MGTCGI_VLANS_ID					14	//VLAN对象
#define MGTCGI_SESSIONS_ID				15	//会话数对象
#define MGTCGI_DOMAINS_ID				16	//domain对象
#define MGTCGI_DOMIANGROUPS_ID			17	//domain对象组
#define MGTCGI_EXTENSIONS_ID			18	//扩展名对象
#define MGTCGI_EXTENSIONGROUPS_ID		19	//扩展名对象组
#define MGTCGI_HTTPSERVERS_ID			20	//服务器对象
#define MGTCGI_HTTPSERVERGROUPS			21	//服务器对象组
#define MGTCGI_LAYER7PROTOCOLS_ID		22	//应用层协议
#define MGTCGI_CUSTOMPROTOCOLS_ID		23	//自定义协议
#define MGTCGI_CUSTOMPROTOCOLGROUPS_ID	24	//自定义协议组

#define MGTCGI_TOTALSESSIONS_ID			30	//设置总体参数
#define	MGTCGI_IPSESSIONS_ID			31	//网络层策略
#define MGTCGI_FIREWALLS_ID				32	//应用层策略
#define	MGTCGI_TRAFFICCHANNELS_ID		33	//流量通道
#define	MGTCGI_SHARECHANNELS_ID			34	//共享通道
#define	MGTCGI_PORTMIRRORS_ID			35	//镜像策略
#define MGTCGI_DNSPOLICYS_ID			36	//DNS管控策略
#define MGTCGI_HTTPDIRPOLICYS_ID		37	//HTTP目录策略
#define MGTCGI_URLFORMATS_ID			38	//设置URL格式


#define MGTCGI_SYS_PROTOINFO_ID			50	//特征库			
#define MGTCGI_SYS_LICENSE_ID			51	//License
#define MGTCGI_SYS_XMLCONFIG_ID			52	//配置文件导入导出
#define MGTCGI_SYS_DATE_ID				53	//系统时间	
#define MGTCGI_SYS_REBOOT_ID			54	//系统重启
#define MGTCGI_SYS_PASSWD_ID			55	//密码管理
#define	MGTCGI_SYS_FIRMWARE_ID			56	//固件升级
#define MGTCGI_SYS_ONLINE_DATE_ID       57
#define MGTCGI_SYS_SERVER_INFO_ID       58
#define MGTCGI_SYS_BACK_UP_ID			59
#define MGTCGI_SYS_RESTORE_ID			60
#define MGTCGI_SYS_UPDATE_URL_ID		61  //UPDATE URL
#define MGTCGI_SYS_URL_FILTER_ID        62  //URL过滤
#define MGTCGI_SYS_RESET_ID                    63  //系统重置

//Add by chenhao 2014年11月4日23:19:49
#define MGTCGI_LOG_ADSL_ID                 70  //ADSL LOG

#define MGTCGI_ROUTE_DNS_ID				80	//DNS
#define MGTCGI_ROUTE_LAN_ID				81	//LAN
#define MGTCGI_ROUTE_WAN_ID				82	//WAN
#define MGTCGI_PPPD_AUTH_ID				83
#define MGTCGI_ROUTE_ADSL_CLIENT_ID		84	//ADSL-CLIENT
#define MGTCGI_ROUTE_L2TPVPN_CLIENT_ID  85  //L2TPVPN-CLIENT
#define MGTCGI_ROUTE_ROUTING_ID			86	//ROUTING
#define MGTCGI_ROUTE_RTABLE_ID			87	//route-table
#define MGTCGI_ROUTE_RRULE_ID			88	//route-rule
#define MGTCGI_ROUTE_DNAT_ID			89	//DNAT
#define MGTCGI_ROUTE_SNAT_ID			90	//SNAT
//2012.1.11 hzh add
#define MGTCGI_ROUTE_LOAD_BALANCING_ID	91
//2012.3.20 hzh add
#define MGTCGI_ROUTE_MACBIND_ID			92	//MACBind
#define MGTCGI_ROUTE_VLAN_ID			93	//VLAN
#define MGTCGI_ROUTE_DHCPD_ID			94	//DHCP-SERVER
#define MGTCGI_ROUTE_DHCPD_HOST_ID		95	//STATIC DHCP-SERVER
#define MGTCGI_ROUTE_XMLCONFIG_ID       96  //路由配置文件导入导出


#define MGTCGI_ROUTE_DDNS_ID				98	//DDNS

//2014.11.3 add by chenhao
#define MGTCGI_SYS_TIMING_ADSL_REDIAL             99//adsl定时重拨
#define MGTCGI_SYS_TIMING_REBOOT            100//设备定时重启

//unused
#define MGTCGI_TOOLS_PRING				200  //ping
#define MGTCGI_TOOLS_TRACEROUTE			201	//traceroute
#define MGTCGI_CHART_NET_DEV			202	//proc/net/dev

#define MGTCGI_NETWORKDETECTION_ID  		203  //networkdetection

//系统信息
//结构:struct mgtcgi_xml_system
#define	MGTCGI_TYPE_SYSTEM				1

//管理口信息
//结构:struct mgtcgi_xml_management
#define	MGTCGI_TYPE_MANAGEMENT			2 					

//页面超时安全
//结构:struct mgtcgi_xml_auth
#define	MGTCGI_TYPE_AUTH				3							

//接口信息
//结构:struct mgtcgi_xml_interfaces
#define	MGTCGI_TYPE_INTERFACES			4					

						
//IP地址对象
//结构:struct mgtcgi_xml_networks
#define	MGTCGI_TYPE_NETWORKS			5	

//时间对象
//结构:struct mgtcgi_xml_schedules
#define	MGTCGI_TYPE_SCHEDULES			6	

// VLAN对象
//结构:struct mgtcgi_xml_vlans
#define	MGTCGI_TYPE_VLANS				7

//会话数对象
//结构:struct mgtcgi_xml_session_limits
#define	MGTCGI_TYPE_SESSION_LIMITS		8

//domain对象
//结构:struct mgtcgi_xml_domains
#define	MGTCGI_TYPE_DOMAINS				9

//扩展名对象
//结构:struct mgtcgi_xml_extensions
#define MGTCGI_TYPE_EXTENSIONS			10

//http服务器对象
//结构:struct mgtcgi_xml_httpservers
#define	MGTCGI_TYPE_HTTPSERVERS			11	

/*
所有对象组
networks_groups	IP地址对象组
protocols_groups	自定义协议对象组
schedules_groups	时间对象组
domain_groups	doamin对象组
extension_group	扩展名对象组
httpserver_group	服务器对象组
session_layer7_log		启用协议维护(启用的L7名称列表 保存所需结构)
结构:struct xml_groups                 */
#define	MGTCGI_TYPE_GROUPS				12						


//7层协议显示,保存使用:TYPE_GROUPS
//结构:struct mgtcgi_xml_layer7_protocols_show
#define	MGTCGI_TYPE_LAYER7_SHOWS		13

//自定义协议对象
//结构:struct mgtcgi_xml_protocols
#define	MGTCGI_TYPE_PROTOCOLS			14	


//日志配置管理，除7层协议
//结构:struct mgtcgi_xml_loggers
#define	MGTCGI_TYPE_LOGGER				15						



//ip会话数总体限制(设备总体参数)
//网络层策略设置
//结构:struct mgtcgi_xml_ip_session_limits
#define	MGTCGI_TYPE_IP_SESSION_LIMITS	16

//应用层策略( 防火墙策略)	
//结构:struct mgtcgi_xml_firewalls
#define	MGTCGI_TYPE_FIREWALLS			17

//流量通道和共享通道
//结构:struct mgtcgi_xml_traffics
#define	MGTCGI_TYPE_TRAFFIC				18						

//端口镜像
//结构:struct mgtcgi_xml_port_mirrors
#define	MGTCGI_TYPE_PORT_MIRRORS		19


//DNS管控策略
//结构:struct mgtcgi_xml_dns_policys
#define	MGTCGI_TYPE_DNS_POLICYS			20

//http目录策略
//结构:struct mgtcgi_xml_http_dir_policys
#define	MGTCGI_TYPE_HTTP_DIR_POLICYS	21

//networks_group组策略
#define MGTCGI_TYPE_NETWORKS_GROUP		22

//domain_group组策略
#define MGTCGI_TYPE_DOMAIN_GROUP		23
	
//extension_group组策略
#define MGTCGI_TYPE_EXTENSION_GROUP		24

//httpserver_group组策略
#define MGTCGI_TYPE_HTTPSERVER_GROUP	25

//protocols_group组策略
#define MGTCGI_TYPE_PROTOCOLS_GROUP		26

//protocols_group组策略
#define MGTCGI_TYPE_PROTOCOLS_GROUP_CN	27


//schedules_group组策略
#define MGTCGI_TYPE_SCHEDULES_GROUP		28

//port_scan端口扫描
#define MGTCGI_TYPE_PORTSCAN           29

// server info
#define MGTCGI_TYPE_SERVER_CONF        30

//零ping设置
#define MGTCGI_TYPE_NETWORKDETECTION                 31



//ROUTE MODE
#define MGTCGI_TYPE_DNS					50
#define MGTCGI_TYPE_INTERFACE_LANS		51
#define MGTCGI_TYPE_INTERFACE_WANS		52
#define MGTCGI_TYPE_INTERFACE_VLANS		53
#define MGTCGI_TYPE_PPPD_AUTHENTICATE	54
#define MGTCGI_TYPE_ADSL_CLIENTS		55
#define MGTCGI_TYPE_L2TPVPN_CLIENTS		56
#define MGTCGI_TYPE_LOAD_BALANCINGS		57 //2012.1.11 add
#define MGTCGI_TYPE_ROUTINGS			58
#define MGTCGI_TYPE_ROUTE_TABLES		59
#define MGTCGI_TYPE_ROUTE_RULES			60
#define MGTCGI_TYPE_NAT_DNATS			61
#define MGTCGI_TYPE_NAT_SNATS			62
#define MGTCGI_TYPE_MAC_BINDS			63 //2012.3.20 add
#define MGTCGI_TYPE_DHCPD_SERVERS		64 //2012.7.17 add
#define MGTCGI_TYPE_DHCPD_HOSTS			65 //2012.7.17 add

#define MGTCGI_TYPE_DDNS					66


typedef	unsigned char					U8;
typedef	unsigned short int 				U16;
typedef	unsigned int 					U32;
typedef	int          					INT32;


// port_scan  端口扫描
struct mgtcgi_xml_portscan_info{
	  char        type[GET_STRING_LENG];
      char        newer[GET_STRING_LENG];
      char        pending[GET_STRING_LENG];
      char        interval[GET_STRING_LENG];
};
struct mgtcgi_xml_portscan{
	U16 num;
	struct mgtcgi_xml_portscan_info  pinfo[0];
};

struct mgtcgi_xml_networkdetection
{
	INT32	zeropingenable;
};

 //system			系统信息
 struct mgtcgi_xml_system{
	char hostname[STRING_LENGTH];
	char domainname[STRING_LENGTH];
	char contact[STRING_LENGTH];
	char location[STRING_LENGTH];
	char password[STRING_LENGTH];
	U8   enable_stp;
	U8   auto_update;
	char update_url[URL_LENGTH];
	U8   ntp_enable;
	char ntp_server[STRING_LENGTH];
	char timezone[STRING_LENGTH];
	U32  bridge_mtu;
	U8   dbridge_perf;
	U8   snmp_enable;
	char snmp_ro_community[STRING_LENGTH];
	char snmp_rw_community[STRING_LENGTH];
};
	
//management		管理口
struct mgtcgi_xml_management{
	char ip[STRING_LENGTH];
	char netmask[STRING_LENGTH];
	char broadcast[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char server[STRING_LENGTH];
	char dns_primary[STRING_LENGTH];
	char dns_secondary[STRING_LENGTH];		
};

//auth			WEB页面安全设置
struct mgtcgi_xml_auth{
	U8   enable;
	U32  timeout_mode;
	U32  timeout_seconds;
	U8   enable_multi_user;
	char redirect_adv_url[URL_LENGTH];
	U8   redirect_http_request;
	char redirect_http_url[URL_LENGTH];
};

//interface		接口信息
struct mgtcgi_xml_interface_include_info{
	char name[STRING_LENGTH];
	char mode[STRING_LENGTH];
	U32 speed;
	char duplex[STRING_LENGTH];	
    U8 linkstate;
    U8 bridgeid;
};

struct mgtcgi_xml_interfaces{
	U16 num;
    U16 bridgenum;
	struct mgtcgi_xml_interface_include_info pinfo[0];
};

//vlans			vlan对象
struct mgtcgi_xml_vlan_info{
	char name[STRING_LENGTH];
	U16  vlans;
	char comment[STRING_LENGTH];
};
struct mgtcgi_xml_vlans{
	U16 num;
	struct mgtcgi_xml_vlan_info  pinfo[0];
};

//networks		IP地址对象
struct mgtcgi_xml_network_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char range[STRING_LENGTH];
	char comment[STRING_LENGTH];	
};

struct mgtcgi_xml_networks{
	U16 num;
	struct mgtcgi_xml_network_info  pinfo[0];
};

/*
所有对象组
networks_groups	IP地址对象组
protocols_groups	自定义协议对象组
schedules_groups	时间对象组
domain_groups	doamin对象组
extension_group	扩展名对象组
httpserver_group	服务器对象组
session_layer7_log		启用协议维护(启用的L7列表)
*/
struct mgtcgi_xml_group_include_info{
	char name[STRING_LENGTH];
};
struct mgtcgi_xml_group{
	U16 num;
	char group_name[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	struct mgtcgi_xml_group_include_info  *pinfo;
};
struct mgtcgi_xml_group_array{
	U16 num;
	struct mgtcgi_xml_group  pinfo[0];
};


//protocols				自定义协议对象
struct mgtcgi_xml_protocol_info{
	char name[STRING_LENGTH];
	char proto[STRING_LENGTH];
	char dport[STRING_LENGTH];	//端口号可能为空
	char sport[STRING_LENGTH];
	char match[STRING_LENGTH];
	char option[STRING_LENGTH];
	char comment[STRING_LENGTH];	
};

struct mgtcgi_xml_protocols{
	U16 num;
	struct mgtcgi_xml_protocol_info pinfo[0];
};

//schedules			时间对象
struct mgtcgi_xml_schedule_info{
	char name[STRING_LENGTH];
	char start_time[STRING_LENGTH];
	char stop_time[STRING_LENGTH];
	char days[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_schedules{
	U16 num;
	struct mgtcgi_xml_schedule_info  pinfo[0];
	
};

//domain_data		domain对象
struct mgtcgi_xml_domain_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_domains{
	U16 num;
	struct mgtcgi_xml_domain_info  pinfo[0];
};

//extension			扩展名对象
struct mgtcgi_xml_extension_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_extensions{
	U16 num;
	struct mgtcgi_xml_extension_info  pinfo[0];
};


//layer7_protocol_show，显示七层协议信息
//此功能要解析l7-protocols.xml和config.xml中的logger/session_layer7_log节点(是否选中).
/*
   l7-protocols.xml中没有"自定义协议组" 的定义,在解析完l7-protocols.xm所有协议后，
   在最后添加一个自定义协议组,自定义协议名称列表从config.xml的<protocols>节点获取。
   单个自定义协议(如WINBOX)格式为:
   name="WINBOX"		//转换成大写
   name2="WINBOX"		//原样自定义名称
   type="custom"
   typename="自定义协议"	//固定
   selected=1				//实际是否选中，即名称是否在"logger/session_layer7_log" 列表中。
   comment="WINBOX"			//原样自定义名称
*/
struct mgtcgi_xml_layer7_protocol_show_info{
	char name[STRING_LENGTH];		//英文名称，用于保存
	char match[STRING_LENGTH];		//中文名称url编码，用于html显示
	char type[STRING_LENGTH];			//英文分类，用于js分类
	char type_name[STRING_LENGTH];	//中文分类url编码，用于html显示
	U8   selected;					//是否选中，1-选中0-未选中；通过查询logger/session_layer7_log节点实现。
	char comment[L7_COMMENT_LENGTH];	//中文描述url编码，用于html显示
};

struct mgtcgi_xml_layer7_protocols_show{
	U16 num;
	struct mgtcgi_xml_layer7_protocol_show_info  pinfo[0];
};

struct mgtcgi_xml_session_layer7_item{
	char name[STRING_LENGTH];
};

//logger			日志管理，除开L7日志。
struct mgtcgi_xml_loggers{
	U8 block_log_enable;
	U8 http_log_enable;
	U8 session_log_enable;
	U8 scan_detect_log_enable;
	U8 dos_detect_log_enable;
	
	U16 num;
	struct mgtcgi_xml_session_layer7_item layer7log[0];
};

struct mgtcgi_xml_servinfo_info
{
    U8      type;
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
};

struct mgtcgi_xml_servinfo
{
    U16                              num;
    struct mgtcgi_xml_servinfo_info  pinfo[0];
};

struct mgtcgi_xml_servconf_info
{
    U8   type;
    char en_name[STRING_LENGTH];
    char en_zonename[STRING_LENGTH];
};

struct mgtcgi_xml_servconf
{
    U16                               num;
    struct  mgtcgi_xml_servconf_info  pinfo[0];
};


//traffic	流量通道，共享通道策略
/*
用classtype表示不同的通道，classtype的值如下:
0	共享通道
1	父通道
2	子通道
3	root通道		没有的对象初始化为0
*/
struct mgtcgi_xml_traffic_channel_info{
	U8   direct; 		//0表示上传upload,1表示下载download
	char classname[STRING_LENGTH];
	char parentname[STRING_LENGTH];
	char sharename[STRING_LENGTH];
       char share_comment[STRING_LENGTH];
	U8   classtype;
	char rate[STRING_LENGTH];
	U8   priority;
	U32  limit;					//限制IP数量
	U8   default_channel;		//是否作为默认通道
	U8   attribute;
	U8   weight;
	U8   bridge;
	char bandwidth[STRING_LENGTH];
	char comment[STRING_LENGTH];	
};

struct mgtcgi_xml_traffics{
	U16 num;
	U8  status;	// 状态:1-启用；0-禁用，对应config中traffic_control
	struct mgtcgi_xml_traffic_channel_info  pinfo[0];	
};

//firewalls			应用层策略
struct mgtcgi_xml_firewall_info{
	U16  name;					//编号
	char proto[STRING_LENGTH];
	char auth[STRING_LENGTH];
	char action[STRING_LENGTH];
	char schedule[STRING_LENGTH];
	char session_limit[STRING_LENGTH];
	char vlan[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char in_traffic[STRING_LENGTH];
	char out_traffic[STRING_LENGTH];
	char quota[STRING_LENGTH];
	U8   quota_action;
	char second_in_traffic[STRING_LENGTH];
	char second_out_traffic[STRING_LENGTH];
	U8   dscp;
    U8   learn_change;
	U8   log;
	U8   disabled;
	U8   bridge;
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_firewalls{
	U16 num;
	U8  application_firewall;
	U8  scan_detect;
	U8  dos_detect;
	struct mgtcgi_xml_firewall_info  pinfo[0];
};


//dns_policy			DNS管控策略
struct mgtcgi_xml_dns_policy_info{
	U16  name;
	char domain_name[STRING_LENGTH];
	U8   bridge;
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	U8   action;
	char targetip[STRING_LENGTH];
	char comment[STRING_LENGTH];
	U8   disabled;
};

struct mgtcgi_xml_dns_policys{
	U16 num;
	U8   application_policy;
	struct mgtcgi_xml_dns_policy_info  pinfo[0];
};


//http_dir_policy			目录策略和url设置
struct mgtcgi_xml_http_dir_policy_urlformat_info{
	U8   bridge;
	char format[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_http_dir_policy_info{
	U8   bridge;
	char extgroup[STRING_LENGTH];
	U8   name;
	U8   disabled;
	char servergroup[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_http_dir_policys{
	U16 format_num;
	U16 policy_num;
	struct mgtcgi_xml_http_dir_policy_urlformat_info *p_urlinfo;
	struct mgtcgi_xml_http_dir_policy_info *p_policyinfo;
};


//global_ip_session_limits，	总会话数设置，网络层策略设置
struct mgtcgi_xml_ip_session_limit_total_info{
	U8   bridge;
	U32  limit;
	char overhead[STRING_LENGTH];
};

struct mgtcgi_xml_ip_session_limit_global_info{
	U32   name;
	char  addr[STRING_LENGTH];
	char  dst[STRING_LENGTH];
	char  per_ip_session_limit[STRING_LENGTH];
	char  total_session_limit[STRING_LENGTH];
	U32    httplog;
	U32    sesslog;
	U32    httpdirpolicy;
	U32    dnspolicy;
	char  action[STRING_LENGTH];
	U32    bridge;
	char  comment[STRING_LENGTH];
};

struct mgtcgi_xml_ip_session_limits{
	U16 total_limit_num;
	U16 global_limit_num;
	struct mgtcgi_xml_ip_session_limit_total_info *p_totalinfo;	//总会话数限制
	struct mgtcgi_xml_ip_session_limit_global_info *p_globalinfo;//网络层策略设置	
};

//port_mirrors			桥端口镜像启用管理
struct mgtcgi_xml_port_mirror_status{
	U8 bridge;
	U8 enable;
};

//port_mirrors			桥端口镜像配置
struct mgtcgi_xml_port_mirror_info{	
	U32 name;	//ID
	char proto[STRING_LENGTH];
	char action[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	U8 	 disabled;
	U8	 bridge;
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_port_mirrors{
	U16 num;
	struct mgtcgi_xml_port_mirror_status  pstatus[MAX_BRIDGE_NUM];
	struct mgtcgi_xml_port_mirror_info  pinfo[0];
};

//httpserver			服务器对象
struct mgtcgi_xml_httpserver_info{
	char name[STRING_LENGTH];
	char ip_addr[STRING_LENGTH];
	char servertype[STRING_LENGTH];
	char prefix[STRING_LENGTH];
	char suffix[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_httpservers{
	U16 num;
	struct mgtcgi_xml_httpserver_info  pinfo[0];
};


//session_limit		会话数对象
struct mgtcgi_xml_session_limit_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
};

struct mgtcgi_xml_session_limits{
	U16 num;
	struct mgtcgi_xml_session_limit_info  pinfo[0];
};




/*********************************** 路由部分 ********************************/
//dns
struct mgtcgi_xml_dns{
	char dns_primary[STRING_LENGTH];
	char dns_secondary[STRING_LENGTH];
	char dns_thirdary[STRING_LENGTH];
};


//ddns 
struct mgtcgi_xml_ddns{
	char username[STRING_LENGTH];
	char password[STRING_LENGTH];
	char dns[STRING_LENGTH];
	U8   enable_ddns;
	char  provider[STRING_LENGTH];
};

//interface_lans
struct mgtcgi_xml_interface_lans_info{
	char ifname[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char newmac[STRING_LENGTH];
	char ip[STRING_LENGTH];
	char mask[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	char aliases[STRING_LENGTH];
};
struct mgtcgi_xml_interface_lans{
	U16 num;
	struct mgtcgi_xml_interface_lans_info pinfo[0];
};

//interface_wans
struct mgtcgi_xml_interface_wans_info{
	char ifname[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char newmac[STRING_LENGTH];
	char access[STRING_LENGTH];
	char ip[STRING_LENGTH];
	char mask[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U8   weight;
	U8   defaultroute;
};
struct mgtcgi_xml_interface_wans{
	U16 num;
	struct mgtcgi_xml_interface_wans_info pinfo[0];
};

// interface_vlans
struct mgtcgi_xml_interface_vlans_info{
	char name[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char ip[STRING_LENGTH];
	char mask[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U16	 id;
	char gateway[STRING_LENGTH];
	U8 	defaultroute;
};
struct mgtcgi_xml_interface_vlans{
	U16 num;
	struct mgtcgi_xml_interface_vlans_info pinfo[0];
};

//pppd_authenticate
struct mgtcgi_xml_pppd_authenticate{
	U8 pap;
	U8 chap;
	U8 mschap;
	U8 mschapv2;
	U8 eap;
};


//adsl_clients
struct mgtcgi_xml_adsl_clients_info{
	char name[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	char username[STRING_LENGTH];
	char password[STRING_LENGTH];
	char servername[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U16  mtu;
	U8   weight;
	U8   defaultroute;
	U8   peerdns;
	U8   enable;
};

struct mgtcgi_xml_adsl_clients{
	U16 num;
	struct mgtcgi_xml_adsl_clients_info pinfo[0];
};

//l2tp_vpn_client
struct mgtcgi_xml_l2tpvpn_clients_info{
	char name[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	char username[STRING_LENGTH];
	char password[STRING_LENGTH];
	char servername[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U16  mtu;
	U8   weight;
	U8   defaultroute;
	U8   peerdns;
	U8   enable;
};

struct mgtcgi_xml_l2tpvpn_clients{
	U16 num;
	struct mgtcgi_xml_l2tpvpn_clients_info pinfo[0];
};

//load balancing
struct mgtcgi_xml_load_balancings_info{
	char name[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	char rulename[STRING_LENGTH];
	char comment[COMMENT_LENGTH]; 
	U8	 weight;
};
struct mgtcgi_xml_load_balancings{
	U16 num;
	struct mgtcgi_xml_load_balancings_info pinfo[0];
};

//routings
struct mgtcgi_xml_routings_info{
	U16  id;
	char dst[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	U8	 priority;
	char table[STRING_LENGTH];
	char rule[STRING_LENGTH];
	char comment[COMMENT_LENGTH]; 
	U8   enable;
};
struct mgtcgi_xml_routings{
	U16 num;
	struct mgtcgi_xml_routings_info pinfo[0];
};

//route_tables
//mgtcgi_xml_group_array

//route_rules
struct mgtcgi_xml_route_rules_info{
	char name[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char protocol[STRING_LENGTH];
	char src_port[STRING_LENGTH];
    char dst_port[STRING_LENGTH];
	char tos[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
};
struct mgtcgi_xml_route_rules{
	U16 num;
	struct mgtcgi_xml_route_rules_info pinfo[0];
};


//nat_dnats
struct mgtcgi_xml_nat_dnats_info{
	U16  id;
	char ifname[STRING_LENGTH];
	char protocol[STRING_LENGTH];
	char wan_ip[STRING_LENGTH];
	char wan_port[STRING_LENGTH];
	char lan_ip[STRING_LENGTH];
	char lan_port[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U8   enable;
};
struct mgtcgi_xml_nat_dnats{
	U16 num;
	struct mgtcgi_xml_nat_dnats_info pinfo[0];
};

//nat_snats
struct mgtcgi_xml_nat_snats_info{
	U16	 id;
	char ifname[STRING_LENGTH];
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char protocol[STRING_LENGTH];
	char action[STRING_LENGTH];
	char dstip[STRING_LENGTH];
	char dstport[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U8   enable;
};
struct mgtcgi_xml_nat_snats{
	U16 num;
	struct mgtcgi_xml_nat_snats_info pinfo[0];
};

//mac bind
struct mgtcgi_xml_mac_binds_info{
	U16	 id;
	char ip[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
	U8	 action;
	U8	 bridge;
};
struct mgtcgi_xml_mac_binds{
	U8  learnt;
	U16 num;
	struct mgtcgi_xml_mac_binds_info pinfo[0];
};


//dhcpd-server
struct mgtcgi_xml_dhcpd_servers_info{
	char name[STRING_LENGTH];
	char ifname[STRING_LENGTH];
	char range[STRING_LENGTH];
	char mask[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char dnsname[STRING_LENGTH];
	char dns[STRING_LENGTH];
	U32	 lease_time;
	U32  max_lease_time;
	char comment[COMMENT_LENGTH];
	U8	 enable;
};
struct mgtcgi_xml_dhcpd_servers{
	U16 num;
	U8  dhcpd_enable;
	struct mgtcgi_xml_dhcpd_servers_info pinfo[0];
};

//static dhcpd-serveer
struct mgtcgi_xml_dhcpd_hosts_info{
	char name[STRING_LENGTH];
	char dhcpname[STRING_LENGTH];
	char mac[STRING_LENGTH];
	char ip[STRING_LENGTH];
	char mask[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char dnsname[STRING_LENGTH];
	char dns[STRING_LENGTH];
	U32	 lease_time;
	U32  max_lease_time;
	char comment[COMMENT_LENGTH];
	U8	 enable;
};
struct mgtcgi_xml_dhcpd_hosts{
	U16 num;
	struct mgtcgi_xml_dhcpd_hosts_info pinfo[0];
};

//Add by chenhao,2014-11-19 11:44:03
//Adsl Timing Redial
struct mgtcgi_xml_adsl_timing_redial{
      char name[STRING_LENGTH];
      char iface[STRING_LENGTH];
      char minute[STRING_LENGTH];
      char hour[STRING_LENGTH];
      char day[STRING_LENGTH];
      char month[STRING_LENGTH];
      char week[STRING_LENGTH];
      char comment[STRING_LENGTH];
};
struct mgtcgi_xml_adsl_timings{
      U16 num;
      struct mgtcgi_xml_adsl_timing_redial pinfo[0];
};

#endif

