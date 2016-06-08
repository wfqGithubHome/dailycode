#ifndef _MGTCGI_XMLCONFIG_
#define _MGTCGI_XMLCONFIG_

#define SERV_XMLFILE                        "/etc/servinfo.xml"                     
#define L7_XMLFILE							"/etc/protoinfo.xml"
#define MGTCGI_XMLTMP						"/etc/config.xml"
#define ROUTE_CONFIG_FILE					"/etc/route_config.xml"
#define MAC_BIND_FILE                       "/etc/mac_bind.xml"
#define IFACE_FILE                          "/etc/iface.xml"
#define WAN_CHANNEL_FILE					"/etc/channel.xml"
#define GUIDE_XML_FILE					    "/tmp/guide.xml"
#define BACKUP_XMLTMP_CMD					"/usr/local/httpd/htdocs/cgi-bin/sh/xmlconf.sh create"
#define ROUTE_BACKUP_XMLTMP_CMD				"/usr/local/httpd/htdocs/cgi-bin/sh/xmlconf.sh create"
#define GET_MAX_BRIDGE_NUM					"/usr/local/sbin/xmlconf -f /etc/config.xml -r -k \"xmlconfig/system/max_bridgenum\" | awk '{print $3}'"
#define GET_BRIDGE_NUM						"/usr/local/sbin/xmlconf -f /etc/config.xml -r -k \"xmlconfig/system/bridgenum\" | awk '{print $3}'"
#define GET_TOTAL_SESSION_LIMIT				"/bin/cat /proc/sys/net/dpifilter/state/ipv4_state_max 2>/dev/null"

//add by chenhao,2014-11-4 23:25:06
#define PPPD_ADSL_LOG_FILE_PATH             "/var/log/pppd.log"

//add by chenhao,2015-6-8 17:31:26
#define ROUTE_PORTMAP_INIT_URL              "http://www.znwba.com/routeservice/updateport.do"

#define DHCP_CONFIG_PATH                    "/etc/dnsmasq.conf"
#define DHCP_RESOLV_CONFPATH                "/etc/dnsmasq.resolv.conf"

#define	PAGE_LITTLE_VALUE					1
#define	STRING_LITTLE_LENGTH				1
#define BRIDGE_LITTLE_VALUE					0

#define MAX_BRIDGE_NUM						5
#define DIGITAL_LENGTH						20
#define LITTER_STRING                       16
#define LITTER_LENGTH                       32
#define STRING_LENGTH						64
#define LONG_STRING_LENGTH                            256
#define URL_LENGTH							256
#define COMMENT_LENGTH						256
#define L7_COMMENT_LENGTH					512
//#define CMD_LENGTH							1024
#define CMD_LENGTH							512
#define CMD_LITTER_LENGTH                   128

#define BUF_LENGTH_4K						4096
#define BUF_LENGTH							40960
#define STRING_LIST_LENGTH					102400
#define LOG_LINE_MAX						102400
#define LICENSE_KEY_SIZE					1152

#define	GET_STRING_LENG	                    20
#define GET_MAX_ADSL_LOG                    20



//web�������
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


//��html������id����
#define MGTCGI_SYSTEM_ID				1	//ϵͳ
#define MGTCGI_MANAGEMENT_ID			2	//����ӿ�
#define MGTCGI_INTERFACE_ID				3	//���ݽӿ�
#define MGTCGI_BRIDGE_NUM_ID			4	//��ȡ�ŵ�����
#define MGTCGI_DEVICE_VERSION_ID		5	//���ð汾��Ϣ
#define MGTCGI_INTERFACE_NAME_ID		6	//������ID����ýӿ�����
#define MGTCGI_AUTOUPDATE_ENABLE_ID     7

#define MGTCGI_NETWORKS_ID				10	//IP��ַ����
#define MGTCGI_NETWORKGROUPS_ID			11	//IP��ַ������
#define MGTCGI_SCHEDULES_ID				12	//ʱ�����
#define MGTCGI_SCHEDULEGROUPS_ID		13	//ʱ�������
#define MGTCGI_VLANS_ID					14	//VLAN����
#define MGTCGI_SESSIONS_ID				15	//�Ự������
#define MGTCGI_DOMAINS_ID				16	//domain����
#define MGTCGI_DOMIANGROUPS_ID			17	//domain������
#define MGTCGI_EXTENSIONS_ID			18	//��չ������
#define MGTCGI_EXTENSIONGROUPS_ID		19	//��չ��������
#define MGTCGI_HTTPSERVERS_ID			20	//����������
#define MGTCGI_HTTPSERVERGROUPS			21	//������������
#define MGTCGI_LAYER7PROTOCOLS_ID		22	//Ӧ�ò�Э��
#define MGTCGI_CUSTOMPROTOCOLS_ID		23	//�Զ���Э��
#define MGTCGI_CUSTOMPROTOCOLGROUPS_ID	24	//�Զ���Э����

#define MGTCGI_TOTALSESSIONS_ID			30	//�����������
#define	MGTCGI_IPSESSIONS_ID			31	//��������
#define MGTCGI_FIREWALLS_ID				32	//Ӧ�ò����
#define	MGTCGI_TRAFFICCHANNELS_ID		33	//����ͨ��
#define	MGTCGI_SHARECHANNELS_ID			34	//����ͨ��
#define	MGTCGI_PORTMIRRORS_ID			35	//�������
#define MGTCGI_DNSPOLICYS_ID			36	//DNS�ܿز���
#define MGTCGI_HTTPDIRPOLICYS_ID		37	//HTTPĿ¼����
#define MGTCGI_URLFORMATS_ID			38	//����URL��ʽ

//Add by chenhao,2015-1-22 15:59:01
#define MGTCGI_WIFI_ID                              39    //WIFI
#define MGTCGI_FIREWALLPOLICT_ID        40  //Ӧ�÷���
#define MGTCGI_WAN_CHANNEL_ID           41  //�ӿ�����ͨ��
#define MGTCGI_LAYER7PROTOCOLSTYPE_ID	42	//Ӧ�ò�Э����

#define MGTCGI_SYS_PROTOINFO_ID			50	//������			
#define MGTCGI_SYS_LICENSE_ID			51	//License
#define MGTCGI_SYS_XMLCONFIG_ID			52	//�����ļ����뵼��
#define MGTCGI_SYS_DATE_ID				53	//ϵͳʱ��	
#define MGTCGI_SYS_REBOOT_ID			54	//ϵͳ����
#define MGTCGI_SYS_PASSWD_ID			55	//�������
#define	MGTCGI_SYS_FIRMWARE_ID			56	//�̼�����
#define MGTCGI_SYS_ONLINE_DATE_ID       57
#define MGTCGI_SYS_SERVER_INFO_ID       58
#define MGTCGI_SYS_BACK_UP_ID			59
#define MGTCGI_SYS_RESTORE_ID			60
#define MGTCGI_SYS_UPDATE_URL_ID		61  //UPDATE URL
#define MGTCGI_SYS_URL_FILTER_ID        62  //URL����
#define MGTCGI_SYS_RESET_ID             63  //ϵͳ����
#define MGTCGI_SYS_SERVICE_UPDATE_ID    64  //�������

//Add by chenhao 2014��11��4��23:19:49
#define MGTCGI_LOG_ADSL_ID              70  //ADSL LOG

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
#define MGTCGI_ROUTE_LOAD_BALANCING_ID	92
//2012.3.20 hzh add
#define MGTCGI_ROUTE_MACBIND_ID			93	//MACBind
#define MGTCGI_ROUTE_VLAN_ID			94	//VLAN
#define MGTCGI_ROUTE_DHCPD_ID			95	//DHCP-SERVER
#define MGTCGI_ROUTE_DHCPD_HOST_ID		96	//STATIC DHCP-SERVER
#define MGTCGI_ROUTE_XMLCONFIG_ID       97  //·�������ļ����뵼��
#define MGTCGI_ROUTE_DDNS_ID			98	//DDNS
#define MGTCGI_ROUTE_PPTPDVPN_ID        101  //pptpd server
#define MGTCGI_ROUTE_PPTPD_ACCOUNT_ID   102

#define MGTCGI_ROUTE_MACVLAN_ID			103	//MACVLAN
#define MGTCGI_ROUTING_TABLES_ID		104	//routing tables
#define MGTCGI_ROUTING_INTERFACELINK_ID	105	//Interface Link


//2014.11.3 add by chenhao
#define MGTCGI_SYS_TIMING_ADSL_REDIAL   99  //adsl��ʱ�ز�
#define MGTCGI_SYS_TIMING_REBOOT            100//�豸��ʱ����

//unused
#define MGTCGI_TOOLS_PRING				200  //ping
#define MGTCGI_TOOLS_TRACEROUTE			201	//traceroute
#define MGTCGI_CHART_NET_DEV			202	//proc/net/dev

#define MGTCGI_NETWORKDETECTION_ID  	203  //networkdetection

//ADD by chenhao,2015-12-31 10:28:47
#define MGTCGI_DHCP_LEASE_INFO_ID       204  //dhcp leases info


//ϵͳ��Ϣ
//�ṹ:struct mgtcgi_xml_system
#define	MGTCGI_TYPE_SYSTEM				1

//�������Ϣ
//�ṹ:struct mgtcgi_xml_management
#define	MGTCGI_TYPE_MANAGEMENT			2 					

//ҳ�泬ʱ��ȫ
//�ṹ:struct mgtcgi_xml_auth
#define	MGTCGI_TYPE_AUTH				3							

//�ӿ���Ϣ
//�ṹ:struct mgtcgi_xml_interfaces
#define	MGTCGI_TYPE_INTERFACES			4					

						
//IP��ַ����
//�ṹ:struct mgtcgi_xml_networks
#define	MGTCGI_TYPE_NETWORKS			5	

//ʱ�����
//�ṹ:struct mgtcgi_xml_schedules
#define	MGTCGI_TYPE_SCHEDULES			6	

// VLAN����
//�ṹ:struct mgtcgi_xml_vlans
#define	MGTCGI_TYPE_VLANS				7

//�Ự������
//�ṹ:struct mgtcgi_xml_session_limits
#define	MGTCGI_TYPE_SESSION_LIMITS		8

//domain����
//�ṹ:struct mgtcgi_xml_domains
#define	MGTCGI_TYPE_DOMAINS				9

//��չ������
//�ṹ:struct mgtcgi_xml_extensions
#define MGTCGI_TYPE_EXTENSIONS			10

//http����������
//�ṹ:struct mgtcgi_xml_httpservers
#define	MGTCGI_TYPE_HTTPSERVERS			11	

/*
���ж�����
networks_groups	IP��ַ������
protocols_groups	�Զ���Э�������
schedules_groups	ʱ�������
domain_groups	doamin������
extension_group	��չ��������
httpserver_group	������������
session_layer7_log		����Э��ά��(���õ�L7�����б� ��������ṹ)
�ṹ:struct xml_groups                 */
#define	MGTCGI_TYPE_GROUPS				12						


//7��Э����ʾ,����ʹ��:TYPE_GROUPS
//�ṹ:struct mgtcgi_xml_layer7_protocols_show
#define	MGTCGI_TYPE_LAYER7_SHOWS		13

//�Զ���Э�����
//�ṹ:struct mgtcgi_xml_protocols
#define	MGTCGI_TYPE_PROTOCOLS			14	


//��־���ù�����7��Э��
//�ṹ:struct mgtcgi_xml_loggers
#define	MGTCGI_TYPE_LOGGER				15						



//ip�Ự����������(�豸�������)
//������������
//�ṹ:struct mgtcgi_xml_ip_session_limits
#define	MGTCGI_TYPE_IP_SESSION_LIMITS	16

//Ӧ�ò����( ����ǽ����)	
//�ṹ:struct mgtcgi_xml_firewalls
#define	MGTCGI_TYPE_FIREWALLS			17

//����ͨ���͹���ͨ��
//�ṹ:struct mgtcgi_xml_traffics
#define	MGTCGI_TYPE_TRAFFIC				18						

//�˿ھ���
//�ṹ:struct mgtcgi_xml_port_mirrors
#define	MGTCGI_TYPE_PORT_MIRRORS		19


//DNS�ܿز���
//�ṹ:struct mgtcgi_xml_dns_policys
#define	MGTCGI_TYPE_DNS_POLICYS			20

//httpĿ¼����
//�ṹ:struct mgtcgi_xml_http_dir_policys
#define	MGTCGI_TYPE_HTTP_DIR_POLICYS	21

//networks_group�����
#define MGTCGI_TYPE_NETWORKS_GROUP		22

//domain_group�����
#define MGTCGI_TYPE_DOMAIN_GROUP		23
	
//extension_group�����
#define MGTCGI_TYPE_EXTENSION_GROUP		24

//httpserver_group�����
#define MGTCGI_TYPE_HTTPSERVER_GROUP	25

//protocols_group�����
#define MGTCGI_TYPE_PROTOCOLS_GROUP		26

//protocols_group�����
#define MGTCGI_TYPE_PROTOCOLS_GROUP_CN	27


//schedules_group�����
#define MGTCGI_TYPE_SCHEDULES_GROUP		28

//port_scan�˿�ɨ��
#define MGTCGI_TYPE_PORTSCAN           29

// server info
#define MGTCGI_TYPE_SERVER_CONF        30

//��ping����
#define MGTCGI_TYPE_NETWORKDETECTION                 31

//��Ϸ����ͨ������
#define MGTCGI_TYPE_FIRST_GAME_CHANNEL  32

#define MGTCGI_TYPE_ROUTESERVICE        33

//add by chenhao ,2016-3-29
#define MGTCGI_TYPE_FIREWALLPOLICY      34
#define MGTCGI_TYPE_WAN_CHANNEL         35
#define	MGTCGI_TYPE_LAYER7TYPE_SHOWS	36 

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
#define MGTCGI_TYPE_MAC_BINDS			64 //2012.3.20 add
#define MGTCGI_TYPE_DHCPD_SERVERS		65 //2012.7.17 add
#define MGTCGI_TYPE_DHCPD_HOSTS			66 //2012.7.17 add
#define MGTCGI_TYPE_DDNS				67
#define MGTCGI_TYPE_PPTPD_SERVERS		68  //add by chenhao,2016-1-29 11:22:07
#define MGTCGI_TYPE_PPTPD_ACCOUNT       69

#define MGTCGI_TYPE_INTERFACE_LINKS		70
#define MGTCGI_TYPE_INTERFACE_MACVLANS	71
#define MGTCGI_TYPE_ROUTING_TABLES    	72

#define MAX_WAN_NUM                     5
#define MAX_LAN_NUM                     5
#define MAX_PPP_NUM                     20
#define MAX_VLAN_NUM                    20
#define MAX_MAC_VLAN                    20

typedef	unsigned char					U8;
typedef	unsigned short int 				U16;
typedef	unsigned int 					U32;
typedef unsigned long int               U64;
typedef	int          					INT32;


// port_scan  �˿�ɨ��
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

 //system			ϵͳ��Ϣ
 struct mgtcgi_xml_system{
	char hostname[STRING_LENGTH];
	char domainname[STRING_LENGTH];
	char contact[STRING_LENGTH];
	char location[STRING_LENGTH];
	char password[STRING_LENGTH];
    char password2[STRING_LENGTH];
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
	
//management		�����
struct mgtcgi_xml_management{
	char ip[STRING_LENGTH];
	char netmask[STRING_LENGTH];
	char broadcast[STRING_LENGTH];
	char gateway[STRING_LENGTH];
	char server[STRING_LENGTH];
	char dns_primary[STRING_LENGTH];
	char dns_secondary[STRING_LENGTH];		
};

//auth			WEBҳ�氲ȫ����
struct mgtcgi_xml_auth{
	U8   enable;
	U32  timeout_mode;
	U32  timeout_seconds;
	U8   enable_multi_user;
	char redirect_adv_url[URL_LENGTH];
	U8   redirect_http_request;
	char redirect_http_url[URL_LENGTH];
};

//interface		�ӿ���Ϣ
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

//vlans			vlan����
struct mgtcgi_xml_vlan_info{
	char name[STRING_LENGTH];
	U16  vlans;
	char comment[STRING_LENGTH];
};
struct mgtcgi_xml_vlans{
	U16 num;
	struct mgtcgi_xml_vlan_info  pinfo[0];
};

//networks		IP��ַ����
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
���ж�����
networks_groups	IP��ַ������
protocols_groups	�Զ���Э�������
schedules_groups	ʱ�������
domain_groups	doamin������
extension_group	��չ��������
httpserver_group	������������
session_layer7_log		����Э��ά��(���õ�L7�б�)
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


//protocols				�Զ���Э�����
struct mgtcgi_xml_protocol_info{
	char name[STRING_LENGTH];
	char proto[STRING_LENGTH];
	char dport[STRING_LENGTH];	//�˿ںſ���Ϊ��
	char sport[STRING_LENGTH];
	char match[STRING_LENGTH];
	char option[STRING_LENGTH];
	char comment[STRING_LENGTH];	
};

struct mgtcgi_xml_protocols{
	U16 num;
	struct mgtcgi_xml_protocol_info pinfo[0];
};

//schedules			ʱ�����
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

//domain_data		domain����
struct mgtcgi_xml_domain_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_domains{
	U16 num;
	struct mgtcgi_xml_domain_info  pinfo[0];
};

//extension			��չ������
struct mgtcgi_xml_extension_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
	char comment[STRING_LENGTH];
};

struct mgtcgi_xml_extensions{
	U16 num;
	struct mgtcgi_xml_extension_info  pinfo[0];
};


//layer7_protocol_show����ʾ�߲�Э����Ϣ
//�˹���Ҫ����l7-protocols.xml��config.xml�е�logger/session_layer7_log�ڵ�(�Ƿ�ѡ��).
/*
   l7-protocols.xml��û��"�Զ���Э����" �Ķ���,�ڽ�����l7-protocols.xm����Э���
   ��������һ���Զ���Э����,�Զ���Э�������б��config.xml��<protocols>�ڵ��ȡ��
   �����Զ���Э��(��WINBOX)��ʽΪ:
   name="WINBOX"		//ת���ɴ�д
   name2="WINBOX"		//ԭ���Զ�������
   type="custom"
   typename="�Զ���Э��"	//�̶�
   selected=1				//ʵ���Ƿ�ѡ�У��������Ƿ���"logger/session_layer7_log" �б��С�
   comment="WINBOX"			//ԭ���Զ�������
*/
struct mgtcgi_xml_layer7_protocol_show_info{
    U32  id;                        //��Ϸ��ӦID
    U32  gid;
	char name[STRING_LENGTH];		//Ӣ�����ƣ����ڱ���
	char match[STRING_LENGTH];		//��������url���룬����html��ʾ
	char type[STRING_LENGTH];			//Ӣ�ķ��࣬����js����
	char type_name[STRING_LENGTH];	//���ķ���url���룬����html��ʾ
	U8   selected;					//�Ƿ�ѡ�У�1-ѡ��0-δѡ�У�ͨ����ѯlogger/session_layer7_log�ڵ�ʵ�֡�
	U8   show;
	char comment[L7_COMMENT_LENGTH];	//��������url���룬����html��ʾ
};

struct mgtcgi_xml_layer7_protocols_show{
	U16 num;
	struct mgtcgi_xml_layer7_protocol_show_info  pinfo[0];
};

struct mgtcgi_xml_session_layer7_item{
	char name[STRING_LENGTH];
};

//logger			��־��������L7��־��
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


//traffic	����ͨ��������ͨ������
/*
��classtype��ʾ��ͬ��ͨ����classtype��ֵ����:
0	����ͨ��
1	��ͨ��
2	��ͨ��
3	rootͨ��		û�еĶ����ʼ��Ϊ0
*/
struct mgtcgi_xml_traffic_channel_info{
	U8   direct; 		//0��ʾ�ϴ�upload,1��ʾ����download
	char classname[STRING_LENGTH];
	char parentname[STRING_LENGTH];
	char sharename[STRING_LENGTH];
       char share_comment[STRING_LENGTH];
	U8   classtype;
	char rate[STRING_LENGTH];
	U8   priority;
	U32  limit;					//����IP����
	U8   default_channel;		//�Ƿ���ΪĬ��ͨ��
	U8   attribute;
	U8   weight;
	U8   bridge;
	char bandwidth[STRING_LENGTH];
	char comment[STRING_LENGTH];	
};

struct mgtcgi_xml_traffics{
	U16 num;
	U8  status;	// ״̬:1-���ã�0-���ã���Ӧconfig��traffic_control
	struct mgtcgi_xml_traffic_channel_info  pinfo[0];	
};

//firewalls			Ӧ�ò����
struct mgtcgi_xml_firewall_info{
	U16  name;					//���
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

//firewalls_policy			Ӧ�÷���
struct mgtcgi_xml_firewall_policy_info{
	U8   id;					//���
	U8   enable;
	char src[STRING_LENGTH];
	char dst[STRING_LENGTH];
	char isp[DIGITAL_LENGTH];
	char proto[STRING_LENGTH];
    char session[STRING_LENGTH];
	char action[LITTER_LENGTH];
	char timed[LITTER_LENGTH];
	char week[STRING_LENGTH];
	char day[STRING_LENGTH];
	char in_limits[LITTER_LENGTH];
	char out_limits[LITTER_LENGTH];
    char iface[LONG_STRING_LENGTH];
	char comment[COMMENT_LENGTH];
};

struct mgtcgi_xml_firewalls_policy{
    U8  enable;
	U16 num;
	struct mgtcgi_xml_firewall_policy_info  pinfo[0];
};

//wan channel �ӿ�����ͨ��
struct mgtcgi_xml_wans_channel_info{
    U8   id;
	U8   direct; 		//0��ʾ�ϴ�upload,1��ʾ����download
	U8   enable;
	U8   classtype;
    U16   classid;
    U16   parentid;
    U8   shareid;
	U8   priority;
	U8   attribute;
	U16  limit;					//����IP����
	U16  weight;    
	char classname[LITTER_STRING];
    char rate[LITTER_STRING];
	char comment[LITTER_LENGTH];	
};

struct mgtcgi_xml_wans_channels{
	U16 num;
	struct mgtcgi_xml_wans_channel_info  pinfo[0];	
};

//dns_policy			DNS�ܿز���
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


//http_dir_policy			Ŀ¼���Ժ�url����
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


//global_ip_session_limits��	�ܻỰ�����ã�������������
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
	struct mgtcgi_xml_ip_session_limit_total_info *p_totalinfo;	//�ܻỰ������
	struct mgtcgi_xml_ip_session_limit_global_info *p_globalinfo;//������������	
};

//port_mirrors			�Ŷ˿ھ������ù���
struct mgtcgi_xml_port_mirror_status{
	U8 bridge;
	U8 enable;
};

//port_mirrors			�Ŷ˿ھ�������
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

//httpserver			����������
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


//session_limit		�Ự������
struct mgtcgi_xml_session_limit_info{
	char name[STRING_LENGTH];
	char value[STRING_LENGTH];
};

struct mgtcgi_xml_session_limits{
	U16 num;
	struct mgtcgi_xml_session_limit_info  pinfo[0];
};

/*
add by chenhao, 2015-7-7 16:22:35
������Ϸͨ������
    */
struct mgtcgi_xml_first_game_channel_info{
    U32 id;
    char name[STRING_LENGTH];
};

struct mgtcgi_xml_first_game_channels{
    U16 num;
    struct mgtcgi_xml_first_game_channel_info pinfo[0];
};




/*********************************** ·�ɲ��� ********************************/
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

struct mgtcgi_xml_interface_lan_dhcp_info{
    U32  lease_time;
    char startip[LITTER_LENGTH];
	char endip[LITTER_LENGTH];
    char gateway[LITTER_LENGTH];
    char dhcp_mask[LITTER_LENGTH];
	char first_dns[LITTER_LENGTH];
	char second_dns[LITTER_LENGTH];
};
struct mgtcgi_xml_interface_lan_subnets_info{
    char ip[LITTER_LENGTH];
    char mask[LITTER_LENGTH];
};
struct mgtcgi_xml_route_interface_lans{
    U8   sunbet_enable;
    U8   subnet_num;
	char ip[LITTER_LENGTH];
    char mask[LITTER_LENGTH];
	char mac[LITTER_LENGTH];
    char ifname[LITTER_LENGTH];
    char dhcp_type[LITTER_LENGTH];
    struct mgtcgi_xml_interface_lan_dhcp_info dhcp_info;
	struct mgtcgi_xml_interface_lan_subnets_info pinfo[0];
};

//interface_wans
struct mgtcgi_xml_interface_wans_info{
    U8   id;
    U8   defaultroute;
    U8   enable;
	char ifname[LITTER_LENGTH];
    char access[LITTER_LENGTH];
    char type[LITTER_LENGTH];
	char ip[LITTER_LENGTH];
	char mask[LITTER_LENGTH];
	char gateway[LITTER_LENGTH];
    char username[LITTER_LENGTH];
    char passwd[LITTER_LENGTH];
    char servername[LITTER_LENGTH];
    char mtu[LITTER_LENGTH];
    char mac[LITTER_LENGTH];
    char dns[STRING_LENGTH];
    char up[LITTER_LENGTH];
    char down[LITTER_LENGTH];
    char isp[LITTER_LENGTH];
	char time[LITTER_LENGTH];
	char week[LITTER_LENGTH];
    char day[LITTER_LENGTH];
};
struct mgtcgi_xml_interface_wans{
	U16 num;
	struct mgtcgi_xml_interface_wans_info pinfo[0];
};

//interface		�ӿ���Ϣ
struct mgtcgi_xml_interface_link_info{
	char ifname[LITTER_LENGTH];
	char type[LITTER_LENGTH];
    char mode[LITTER_LENGTH];
    char duplex[LITTER_LENGTH];	
	U32 speed;
    U8 linkmode;
};

struct mgtcgi_xml_interface_links{
	U16 num;
	struct mgtcgi_xml_interface_link_info pinfo[0];
};

// interface_vlans
struct mgtcgi_xml_interface_vlans_info{
	char name[LITTER_LENGTH];
	char ifname[LITTER_LENGTH];
	char mac[LITTER_LENGTH];
	char comment[COMMENT_LENGTH];
	U16	 id;
};
struct mgtcgi_xml_interface_vlans{
	U16 num;
	struct mgtcgi_xml_interface_vlans_info pinfo[0];
};

// interface_macvlans
struct mgtcgi_xml_interface_macvlans_info{
	char name[LITTER_LENGTH];
	char ifname[LITTER_LENGTH];
	char mac[LITTER_LENGTH];
	char comment[COMMENT_LENGTH];
};
struct mgtcgi_xml_interface_macvlans{
	U16 num;
	struct mgtcgi_xml_interface_macvlans_info pinfo[0];
};

// route tables
struct mgtcgi_xml_route_tables_info{
    U8   enable;
	U8   id;
	char value[LITTER_LENGTH];
};
struct mgtcgi_xml_route_tables{
	U8 num;
	struct mgtcgi_xml_route_tables_info pinfo[0];
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
    char action[STRING_LENGTH];
	char comment[COMMENT_LENGTH];
};
struct mgtcgi_xml_route_rules{
	U16 num;
	struct mgtcgi_xml_route_rules_info pinfo[0];
};


//nat_dnats
struct mgtcgi_xml_nat_dnats_info{
    U32  id;
    U8   enable;
    char protocol[LITTER_STRING];
    char comment[STRING_LENGTH];
    char wan_ip[STRING_LENGTH];
    char wan_port_before[DIGITAL_LENGTH];
    char wan_port_after[DIGITAL_LENGTH];
    char lan_port_before[DIGITAL_LENGTH];
    char lan_port_after[DIGITAL_LENGTH];
    char lan_ip[STRING_LENGTH];
    char ifname[STRING_LENGTH];
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

struct mgtcgi_xml_pptpd_server{
    U8 enable;
    char localip[STRING_LENGTH];
    char startip[STRING_LENGTH];
    char endip[STRING_LENGTH];
    char dns[STRING_LENGTH];
    char entry_mode[STRING_LENGTH];
};

struct mgtcgi_xml_pptpd_account{
    char username[STRING_LENGTH];
    char passwd[STRING_LENGTH];
    char ip[STRING_LENGTH];
};

struct mgtcgi_xml_pptpd_accounts_info{
    U16 num;
    struct mgtcgi_xml_pptpd_account pinfo[0];
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

//Add by chenhao,2015-1-22 15:59:59
//WIFI format
struct mgtcgi_xml_wifi_address_pool{
      U16 id;
      U8 enable;
      char startip[STRING_LENGTH];
      char endip[STRING_LENGTH];
      char comment[STRING_LENGTH];
};
struct mgtcgi_xml_wifis{
      U8 enable;
      U16 pc_discover_time;
      U16 max_time;
      U16 share_num;
      char admins[LONG_STRING_LENGTH];
      
      U16 pool_num;
      struct mgtcgi_xml_wifi_address_pool pinfo[0];
};

//Add by chenhao,2015-11-5 11:12:46
//route server info
struct mgtcgi_xml_routeserver{
    U8  enable;
    U8  passwd_enable;
};

//ADD by chenhao 2015-12-3 09:14:23
struct dev_firmark_info{
    char    dev_name[STRING_LENGTH];
    INT32   firmark;
    INT32   table;
};

struct ptotocol_id{
    U8      id;
    char    name[STRING_LENGTH];
};

typedef enum protocol_ids
{
    PROTOCOL_ID_BEGIN,
    PROTOCOL_ID_TCP,
    PROTOCOL_ID_UDP,
    PROTOCOL_ID_ICMP,
    PROTOCOL_ID_ALL,
    PROTOCOL_ID_END
}protocol_ids;

struct channel_traffic_info{
    char classname[STRING_LENGTH];
};

#endif

