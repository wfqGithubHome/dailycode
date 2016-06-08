#ifndef __AC_MAC_AUTH_H__
#define __AC_MAC_AUTH_H__

#include "ac_report.h"
// #define NETLINK_MAC_AUTH      				26

#define MAC_AUTH_NETLINK_BASE         0x10
#define MAC_AUTH_CONF_ADD             (MAC_AUTH_NETLINK_BASE+1)
#define MAC_AUTH_CONF_DEL             (MAC_AUTH_NETLINK_BASE+2)
#define MAC_AUTH_CONF_SHOW            (MAC_AUTH_NETLINK_BASE+3)
#define MAC_AUTH_CONF_FLUSH           (MAC_AUTH_NETLINK_BASE+4)
#define MAC_AUTH_LEARNING_SHOW        (MAC_AUTH_NETLINK_BASE+5)
#define MAC_AUTH_SET_DHCP_STATUS      (MAC_AUTH_NETLINK_BASE+6)
#define MAC_AUTH_GET_ID               (MAC_AUTH_NETLINK_BASE+7)
#define MAC_AUTH_GET_DHCP_STATUS      (MAC_AUTH_NETLINK_BASE+8)
#define MAC_AUTH_GET_AP_INFO          (MAC_AUTH_NETLINK_BASE+9)
#define MAC_AUTH_MAX                  (MAC_AUTH_NETLINK_BASE+10)

//return info
#define MAC_AUTH_DONE                 0
#define MAC_AUTH_ERROR                1
#define MAC_AUTH_MEMERR               2
#define MAC_AUTH_CONF_SHOW_DONE       3
#define MAC_AUTH_LEARNING_SHOW_DONE   4
#define MAC_AUTH_GET_ID_DONE          5
#define MAC_AUTH_GET_DHCP_STATUS_DONE 6
#define MAC_AUTH_GET_AP_INFO_DONE     7

#define MAC_ADDR_MIN_LEN    		    	6
#define MAC_ADDR_MAX_NUM    		    	6

#define SHOW_MAX_NODES            		    100
#define NAME_MAX_LEN		       	    	32

#define WIFI_CONNS_TOP_LIMIT                256

enum ac_mac_auth_op_type
{
    AC_MAC_AUTH_TABLE_SYNC,
    AC_MAC_AUTH_TABLE_INIT,
};

struct mac_auth_msg 
{
    uint32_t ip_addr;
    uint32_t id;
    uint8_t mac[MAC_ADDR_MAX_NUM];
    uint8_t bridge_id;
    uint8_t dhcp_enable;
    char name[NAME_MAX_LEN];
    uint8_t auth_type;
    int32_t timeout;
};

struct user_info
{
    unsigned char mac[MAC_ADDR_MAX_NUM];
};

struct shunwanap_msg_max
{
    char    apname[18];
    int     num;
    uint32_t check_time;
	struct user_info user[WIFI_CONNS_TOP_LIMIT];
};

struct mac_auth_respond_info
{
	struct nlmsghdr nlhdr;
	struct mac_auth_msg info[SHOW_MAX_NODES];
};

struct mac_auth_getap_respond_info
{
    struct nlmsghdr nlhdr;
    struct shunwanap_msg_max info;   
};

typedef int (* ac_warning_func) (char *ac_url, char *ac_id, char *ap_id, enum ac_report_warning_type type);
typedef int (* ac_setting_func) (char *ac_url, char *ac_id, char *ap_id);


extern int ac_mac_get_from_kernel(struct ac_report_mac *macs, int *macs_len);
extern int ac_mac_set_to_kernel(struct ac_respond_mac_time *mts, int mts_len, 
    enum ac_mac_auth_op_type type);
extern int ac_mac_auth_ap_info_get(char *ac_url, char *ac_id, enum ac_report_warning_type type, 
    ac_warning_func warning_func, ac_setting_func setting_func);


#endif

