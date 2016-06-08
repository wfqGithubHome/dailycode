#ifndef _DPI_AUTH_H
#define _DPI_AUTH_H

#include <linux/types.h>

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

#define MACQUAD(addr) \
	((unsigned char *)addr)[0], \
	((unsigned char *)addr)[1], \
	((unsigned char *)addr)[2], \
	((unsigned char *)addr)[3], \
	((unsigned char *)addr)[4], \
	((unsigned char *)addr)[5]
#define MACQUAD_FMT "%02x%02x%02x%02x%02x%02x"


#define DPI_AUTH_USERNAME_MAX           32
#define DPI_AUTH_PASSWD_MAX             32
#define DPI_AUTH_GROUPNAME_MAX          32

#define AUTH_LOGIN_DONE                 0x20
#define AUTH_LOGOUT_DONE                0x21
#define	AUTH_FLUSH_DONE	                0x22
#define	AUTH_GET_COUNT_DONE		        0x23
#define AUTH_GET_USER_BY_ID_DONE	    0x24
#define AUTH_GET_USER_BY_NAME_DONE      0x25
#define AUTH_GET_USER_BY_ADDR_DONE      0x26
#define AUTH_GET_GROUP_BY_NAME_DONE     0x27
#define AUTH_GET_USER_LIST_DONE         0x28
#define AUTH_GET_GROUP_LIST_DONE        0x29
#define AUTH_GET_ADDR_LIST_DONE         0x3a
#define AUTH_GET_IPPOOL_LIST_DONE       0X3b
#define AUTH_ADD_USER_DONE              0x3c
#define AUTH_ADD_GROUP_DONE             0x3d
#define AUTH_ADD_IPPOOL_DONE            0X3e
#define AUTH_NULL_INFO                  0x40
#define AUTH_ERROR_INFO		            0x41


#define AUTH_NETLINK_BASE	            0x10	/* standard netlink messages below this */
#define AUTH_LOGIN	                    (AUTH_NETLINK_BASE + 1)		
#define AUTH_LOGOUT	                    (AUTH_NETLINK_BASE + 2)
#define AUTH_FLUSH	                    (AUTH_NETLINK_BASE + 3)		/* flush auth module, clear all */
#define AUTH_GET_COUNT                  (AUTH_NETLINK_BASE + 4)
#define AUTH_GET_USER_BY_ID             (AUTH_NETLINK_BASE + 5)
#define AUTH_GET_USER_BY_NAME	        (AUTH_NETLINK_BASE + 6)		
#define AUTH_GET_USER_BY_ADDR           (AUTH_NETLINK_BASE + 7)
#define AUTH_GET_GROUP_BY_NAME          (AUTH_NETLINK_BASE + 8)
#define AUTH_GET_USER_LIST	            (AUTH_NETLINK_BASE + 9)     /* get userid list summry */
#define AUTH_GET_GROUP_LIST             (AUTH_NETLINK_BASE + 10)
#define AUTH_GET_ADDR_LIST              (AUTH_NETLINK_BASE + 11)
#define AUTH_ADD_USER                   (AUTH_NETLINK_BASE + 12)
#define AUTH_ADD_GROUP                  (AUTH_NETLINK_BASE + 13)
#define AUTH_IPPOOL_SET                 (AUTH_NETLINK_BASE + 14)
#define AUTH_IPPOOL_SHOW                (AUTH_NETLINK_BASE + 15)
#define AUTH_NETLINK_MAX	            (AUTH_NETLINK_BASE + 16)




/*******************************************************************************************/
#define PROCESS_OK                              0
#define PROCESS_ERROR                       -1

#define WIFIAUTH_ACCOUNT_LENGTH              8
#define WIFIAUTH_GROUNP_LENGTH                  32
#define WIFIAUTH_MAX_ACCOUNT_NUM            32
#define WIFIAUTH_ACCOUNT_MAX_IP_NUM         32

#define WIFIAUTH_STRING_LENGTH                  64
#define WIFIAUTH_COMMENT_LENGTH               128

#define WIFIAUTH_BUFFER_LEN                         128
#define WIFIAUTH_CMD_LEN                    512
/******************************************************************************************/

typedef struct auth_entry
{
    int         result;
    uint32_t    user_id;
    uint32_t    group_id;
    uint32_t    addr;
    uint32_t    startip;
    uint32_t    endip;
    uint32_t    auth_host_count;
    uint32_t    auth_user_count;
    uint32_t    auth_group_count;    
    uint32_t    authed_host_count;
    uint32_t    authed_user_count;
    uint32_t    authed_group_count;
    uint32_t    auth_host_max;
    uint32_t    auth_user_max;
    uint32_t    auth_group_max;
} auth_entry_t;

typedef struct auth_info_entry
{
    uint8_t       bridge_id; 
    uint32_t      user_id;
    uint16_t      group_id;  
    uint32_t      addr;
    uint32_t      addr_count;
    uint32_t      last_update;
    uint32_t      login_time;
    uint32_t      user_count;
    uint16_t      max_authip_num_per_user;
    uint16_t      no_delete;
    unsigned char username[DPI_AUTH_USERNAME_MAX];
    unsigned char password[DPI_AUTH_USERNAME_MAX];
    unsigned char groupname[DPI_AUTH_GROUPNAME_MAX];
} auth_info_entry_t;

/*
typedef struct user_auth_msg {
    uint8_t         bridge_id;
    uint8_t         link_id;
    uint32_t	    addr;
    unsigned char   username[DPI_AUTH_USERNAME_MAX];
    unsigned char   groupname[DPI_AUTH_GROUPNAME_MAX];    
    uint32_t	    user_id;
    uint32_t        group_id;
    uint32_t        first_id;
    uint32_t        last_id;
    uint32_t        authed;
    uint32_t        timestamp;
} user_auth_msg_t;
*/
/***************************************************************************/
typedef struct wifi_account
{
    char username[DPI_AUTH_USERNAME_MAX];
    char passwd[DPI_AUTH_USERNAME_MAX];
}wifi_account_t;

typedef struct user_auth_msg
{
    uint8_t   bridge_id;
    uint8_t   authed;
    uint32_t  addr;
    
    uint32_t  pc_discover_time;  //pc在线检查周期
    uint32_t  share_num;
    uint32_t  account_num; //账号个数
    uint32_t  startip;
    uint32_t  endip;
    uint32_t  first_id;
    uint32_t  last_id;
    uint32_t  nodelete;
    unsigned char groupname[DPI_AUTH_GROUPNAME_MAX];
    struct wifi_account ainfo;
}user_auth_msg_t;


//show account:admin,user
struct wifi_account_info
{
      uint16_t      group_id;
      uint16_t      ipnum;
      uint32_t      last_update;
      uint32_t      login_time;
      unsigned char groupname[WIFIAUTH_GROUNP_LENGTH]; 
      unsigned char username[WIFIAUTH_ACCOUNT_LENGTH];
      struct in_addr ipaddr[WIFIAUTH_ACCOUNT_MAX_IP_NUM];
};

//receive
struct wifi_auth_respond_info
{
        struct nlmsghdr nlhdr;
        struct wifi_account_info info;
};


struct wifi_auth_conf_info
{
        char *t;   //-t,basic,account,pool,user   默认为user
        int cgi_show;    //-s --show
        int web_show;  //-w
        int set;      //-S
        int get;      //-g
        int add;    // -a
        int del;    //-d
        int flush;    //-f
        int status;    //-u
        int login;    //--login

        int type;    //-y
        int enable;     //-e
        int pc_discover_time;  //-p
        int max_time;  //-m
        int share_num;  //-r

        int pool_enable;  //-E  --penable
        char *oldstartip;   //--oldstartip
        char *oldendip;    //--oldendip
        char *startip;    //--startip
        char *endip;    //--endip

        char *username; //--username
        char *passwd;  //--passwd

        char *ip;
};

#endif

