#ifndef __COMMON_H__
#define __COMMON_H__

enum account_dir
{
	NET_STAT_DIR_INT_TO_EXT,
	NET_STAT_DIR_EXT_TO_INT,
	NET_STAT_DIR_MAX
};

#define NETLINK_ACCOUNT             25

#define MAX_NET_STAT_ENTRY			1024
#define DPI_AUTH_USERNAME_MAX			32
#define DPI_AUTH_GROUPNAME_MAX			32


#define NET_STAT_PROTOCOL_SUM_OK        0x20
#define NET_STAT_SUBNET_SUM_OK          0x21
#define	NET_STAT_HOST_INFO_OK	        0x22
#define	NET_STAT_NET_ADDR_INFO_OK		0x23
#define NET_STAT_NET_PROTO_INFO_OK	    0x24
#define NET_STAT_CONN_INFO_OK	        0x25
#define NET_STAT_HOST_SUM_OK            0x26
#define NET_STAT_SESSION_SUM_OK         0x27
#define NET_STAT_NET_SUM_OK             0x28
#define NET_STAT_DPI_FILTER_STAT_OK     0x29
#define NET_STAT_INTERFACE_INFO_OK      0x2a
#define NET_STAT_NULL_INFO		        0x2b
#define NET_STAT_ERROR_INFO             0x2c


/* Types of messages */
#define NET_STAT_NETLINK_BASE	          0x10	/* standard netlink messages below this */
#define NET_STAT_PROTOCOL_SUM            (NET_STAT_NETLINK_BASE + 1)
#define NET_STAT_SUBNET_SUM              (NET_STAT_NETLINK_BASE + 2)
#define NET_STAT_HOST_INFO	             (NET_STAT_NETLINK_BASE + 3)		/* Mode request from peer */
#define NET_STAT_PROTOCOL_INFO           (NET_STAT_NETLINK_BASE + 4)
#define NET_STAT_NET_ADDR_INFO	         (NET_STAT_NETLINK_BASE + 5)
#define NET_STAT_NET_PROTO_INFO	         (NET_STAT_NETLINK_BASE + 6)
#define NET_STAT_CONNTRACK_INFO	         (NET_STAT_NETLINK_BASE + 7)		/* Verdict from peer */ 
#define NET_STAT_USER_LOGIN	             (NET_STAT_NETLINK_BASE + 8)
#define NET_STAT_USER_LOGOUT	         (NET_STAT_NETLINK_BASE + 9)		/* Verdict from peer */ 
#define NET_STAT_HOST_SUM                (NET_STAT_NETLINK_BASE + 10)
#define NET_STAT_SESSION_SUM             (NET_STAT_NETLINK_BASE + 11)
#define NET_STAT_NET_SUM                 (NET_STAT_NETLINK_BASE + 12)
#define NET_STAT_INTERFACE_INFO          (NET_STAT_NETLINK_BASE + 13)
#define NET_STAT_DPI_FILTER_STAT         (NET_STAT_NETLINK_BASE + 14)
#define NET_STAT_NETLINK_MAX	         (NET_STAT_NETLINK_BASE + 15)


typedef struct user_stat_msg {
    u_int8_t    bridge_id;
	u_int32_t	addr;
	u_int16_t	layer7_id;
} user_stat_msg_t;

typedef struct stat_counter_unit
{
    u_int64_t        packets;
    u_int64_t        bytes;
} stat_counter_unit_t;

typedef struct stat_counter
{
    u_int32_t                     total_session_count;
    u_int32_t                     current_session_count;
    struct stat_counter_unit      counter[NET_STAT_DIR_MAX];
} stat_counter_t;

typedef struct user_stat_entry
{
    u_int32_t src_ip;
    u_int32_t dst_ip;    
    u_int16_t layer4_proto;
    u_int16_t layer4_src_port;
    u_int16_t layer4_dst_port;
    uint16_t  layer7_id;
    u_int8_t  dir;
    char      layer7_name[16];
    char      layer7_cat[16];
    uint32_t  user_id;
    uint16_t  group_id;
    char      username[DPI_AUTH_USERNAME_MAX];    
    char      groupname[DPI_AUTH_GROUPNAME_MAX];
#ifdef NET_STAT_DEBUG //add by pengysheng 20081216   
    uint32_t    net_stat_user;
    uint32_t    src_tran_user;
#endif
    struct stat_counter counter;
}user_stat_entry_t;

typedef struct respond_message {
	struct nlmsghdr nlhdr;
	struct user_stat_entry account_entry[MAX_NET_STAT_ENTRY];
} respond_msg_t;


int _do_command(int type, struct user_stat_msg * request);
int do_get_command(const char *cmd, char *buffer, int lenght);

#endif
