#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
//#include <getopt.h>
//#include <opt.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <linux/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#include "cgic.h"
#include "common.h"

#define STRING_LENGTH               64
#define IP_INFO_VERSION             "1.0.07"

#define MAX_NET_STAT_ENTRY			1024
#define MAX_LAYER7_PROTOCOLS		1024
#define MAX_IP_PER_SUBNET			255
#define MAX_SUBNETS					1024

#define NETLINK_ACCOUNT             25
#define NETLINK_IDS_LOG             40

#define MAC_ADDR_MIN_LEN    		    	6
#define MAC_ADDR_MAX_NUM    		    	6
#define WIFI_CONNS_TOP_LIMIT                	256
#define MAX_USER_NUMS					256
#define NAME_MAX_LEN		       	    	32
#define IDS_LOG_MSG_BUFF_MAX        1024
#if 0
#define DEBUGP log_debug
#else
#define DEBUGP(format, args...)
#endif

typedef struct config_info
{
    int     debug;
	int		count;
	int		interface;
	int		bridge_id;
    int     all;
    char    *addr;
	char	*net;
	char	*proto;
}config_info_t;

enum account_dir
{
	NET_STAT_DIR_INT_TO_EXT,
	NET_STAT_DIR_EXT_TO_INT,
	NET_STAT_DIR_MAX
};

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

typedef struct user_stat_msg {
    u_int8_t    bridge_id;
	u_int32_t	addr;
	u_int16_t	layer7_id;
} user_stat_msg_t;

typedef struct ids_log_msg
{
	uint16_t         msg_len;
    char             message[IDS_LOG_MSG_BUFF_MAX];
} ids_log_msg_t;

struct user_info
{
    	unsigned char mac[MAC_ADDR_MAX_NUM];
};
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

#define NETQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2]

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

#define IDS_LOG_NETLINK_BASE             0x10
#define IDS_LOG_SHOW            	     (IDS_LOG_NETLINK_BASE+1)
#define IDS_LOG_FLUSH            	     (IDS_LOG_NETLINK_BASE+2)
#define IDS_LOG_NETLINK_MAX              (IDS_LOG_NETLINK_BASE+3)

#define IDS_LOG_DONE           	         0
#define IDS_LOG_ERROR          	         1
#define IDS_LOG_MEMERR        	         2
#define IDS_LOG_SHOW_DONE 	             3
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

#define MAX_SUB_NET_NODES                           10 /* using in show all ip */
#define MAX_PROTO_NUMS                              65536 /*using in show all proto info at net*/
#define MAX_PROTO_NODES                             128 /*65536/512 using in show all proto info at net*/
#define TOTAL_LAYER7_NAME       "total"
#define TOTAL_LAYER7_CAT        "total"

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

typedef struct ids_log_respond_info
{
	struct nlmsghdr     nlhdr;
	struct ids_log_msg  msg;
} ids_log_respond_info_t;

/******************************************
Add by:chenhao
Date:2013-8-2 16:09:34
*******************************************/
typedef struct mac_auth_msg 
{
    uint32_t ip_addr;
    uint32_t id;
    uint8_t mac[MAC_ADDR_MAX_NUM];
    uint8_t bridge_id;
    uint8_t dhcp_enable;
    char name[NAME_MAX_LEN];
}mac_auth_msg_t;

/******************************************
Add by:chenhao
Date:2013-8-2 16:09:56
*******************************************/
typedef struct shunwanap_msg_max
{
	char    			apname[18];
    	int     			num;
    	uint32_t 			check_time;
	struct user_info 	user[WIFI_CONNS_TOP_LIMIT];
}shunwanap_msg_max_t;

/******************************************
Add by:chenhao
Date:2013-8-2 16:10:33
*******************************************/
typedef struct mac_auth_getap_respond_info
{
    struct nlmsghdr nlhdr;
    shunwanap_msg_max_t info;   
}mac_auth_getap_respond_info_t;

config_info_t   config;
time_t			timestamp;

void addr_info_display_function(struct user_stat_entry * node)
{
    static int i=0;
    if(node)
    {
        if(i)
            printf(",");
        else{
            i = 1;
        }
        
        printf("{\"name\":\"%s\",\"type\":\"%s\",\"total_ses\":%u,\"cur_ses\":%u,\"in_packets\":%llu,\"out_packets\":%llu,\"in_bytes\":%llu,\"out_bytes\":%llu,\"time\":%d}",   
            node->layer7_name, 
            node->layer7_cat,
            node->counter.total_session_count, 
            node->counter.current_session_count, 
            node->counter.counter[NET_STAT_DIR_EXT_TO_INT].packets,
            node->counter.counter[NET_STAT_DIR_INT_TO_EXT].packets,
            node->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes,
            node->counter.counter[NET_STAT_DIR_INT_TO_EXT].bytes,
			(int)timestamp);
    }
}

void proto_all_info_display_function(struct user_stat_entry * node)
{
    static int i=0;

    if(i)
        printf(",");
    else
        i=1;
    
    printf("{\"ipaddr\":\"%u.%u.%u.%u\",\"total_session\":%u,\"cur_session\":%u,\"in_packets\":%llu,\"out_packets\":%llu,\"in_bytes\":%llu,\"out_bytes\":%llu,\"time\":%d}",
        NIPQUAD(node->src_ip), 
        node->counter.total_session_count, 
        node->counter.current_session_count, 
		node->counter.counter[NET_STAT_DIR_EXT_TO_INT].packets,
		node->counter.counter[NET_STAT_DIR_INT_TO_EXT].packets,
		node->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes,
		node->counter.counter[NET_STAT_DIR_INT_TO_EXT].bytes,
		(int)timestamp);
}

int _do_command(int type, struct user_stat_msg * request)
{
	int   sock;
	struct sockaddr_nl remote;
	struct sockaddr_nl local;
	struct nlmsghdr * nlhdr;
	struct iovec iov;
	struct msghdr msg;
    
    sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ACCOUNT);
    if(sock < 0){
        return -1;
    }
    memset(&local, 0, sizeof(struct sockaddr_nl));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();
    local.nl_groups = 0;
    if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
        close(sock);
        return -1;
    }

    memset(&remote, 0, sizeof(struct sockaddr_nl));
    remote.nl_family = AF_NETLINK;
    remote.nl_pid    = 0;
    remote.nl_groups = 0;

    memset(&msg, 0, sizeof(struct msghdr));
    nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct user_stat_entry) * 1024));
    memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(struct user_stat_msg));

    nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct user_stat_msg));
    nlhdr->nlmsg_pid = getpid();  /* self pid */
    nlhdr->nlmsg_flags = NLM_F_REQUEST;
    nlhdr->nlmsg_type = type;

    iov.iov_base = (void *)nlhdr;
    iov.iov_len = nlhdr->nlmsg_len;
    
    msg.msg_name = (void *)&remote;
    msg.msg_namelen = sizeof(remote);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    int ret = sendmsg(sock, &msg, 0);
    if (ret == -1) {
         return -1;
    }

    free(nlhdr);   
	return sock;
}

int __do_ids_command(int type, struct ids_log_msg *request)
{
	int                 sock;
	struct msghdr       msg;
	struct iovec        iov;
	struct sockaddr_nl  remote;
	struct sockaddr_nl  local;
	struct nlmsghdr     *nlhdr;

    DEBUGP("%s: begin, type = %d\n", __FUNCTION__, type);
    
	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_IDS_LOG); 

	if(sock < 0){
		DEBUGP("%s: create socket error\n", __FUNCTION__);
		return -1;
	}

	memset(&local, 0, sizeof(struct sockaddr_nl));

	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
		DEBUGP("%s: bind socket error\n", __FUNCTION__);
		close(sock);
		return -1;
	}

	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));
	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct ids_log_msg)));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct ids_log_msg));
	nlhdr->nlmsg_pid = getpid();  /* self pid */
	nlhdr->nlmsg_flags = NLM_F_REQUEST;
	nlhdr->nlmsg_type = type;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&remote;
	msg.msg_namelen = sizeof(remote);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	int ret = sendmsg(sock, &msg, 0);
	if (ret == -1) {
		 DEBUGP("%s: ids log sendmsg error\n", __FUNCTION__);
		 return -1;
	}

    free(nlhdr);   

    DEBUGP("%s: finish, sock = %d\n", __FUNCTION__, sock);
	return sock;
}

int display_src_node_info(const int type, const unsigned int bridge_id, 
    const unsigned int ip_addr, const unsigned int layer7_id, void (*display_function)(struct user_stat_entry *))
{
    struct user_stat_msg request;
    struct respond_message respond_msg;
    int sock;
    int size = sizeof(struct respond_message);
    struct sockaddr_nl remote;

    DEBUGP("%s:begin bridge_id=[%u],ip_addr=[%u.%u.%u.%u],layer7_id=[%u]\n",__FUNCTION__,bridge_id,NIPQUAD(ip_addr),layer7_id);
    if(layer7_id < 0 || layer7_id > 65535)
    {
        //perror("input layer7 id is invalid\n");
        return -1;
    }
    request.bridge_id = bridge_id;
    request.addr = ip_addr;
    request.layer7_id = layer7_id;
	sock = _do_command(type, &request);
    if(sock == -1)
		return -1;
    
    printf("{\"data\":[");
    while(1)
    {
        socklen_t remote_addr_len = sizeof(struct sockaddr_nl);
        int recv_len = recvfrom(sock, &respond_msg, size,
                0, (struct sockaddr*)&remote, &remote_addr_len);
        if(recv_len == -1){
            //perror("recvmsg error\n");
            break;
        }
        if(recv_len == 0){
            break;
        }
        if(respond_msg.nlhdr.nlmsg_type < NET_STAT_PROTOCOL_SUM_OK || 
            respond_msg.nlhdr.nlmsg_type > NET_STAT_ERROR_INFO)
        {
            break;
        }
        
		if(respond_msg.nlhdr.nlmsg_type == NET_STAT_NULL_INFO)
			break;
        int size = (respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr)) / sizeof(struct user_stat_entry);
        int i = 0;
        for(i = 0; i < size; i++){
            struct user_stat_entry * node = &respond_msg.account_entry[i];
            if(node == NULL)
                break;
            if(node->src_ip == 0 && node->layer7_name[0] == '\0' && node->layer7_id == 0)
                break;
            if(display_function)
                display_function(node);
        }
    }
    printf("],\"iserror\":0,\"msg\":\"\"}");
    close(sock);    
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

int get_core_message(struct respond_message **ppblock,int *count,int max_count,int type,unsigned int bridge_id,u_int32_t addr,u_int16_t proto)
{    
    struct user_stat_msg request;
    struct respond_message respond_msg;
    int sock;
    int size = sizeof(struct respond_message);
    struct sockaddr_nl remote;

    DEBUGP("%s:begin bridge_id=[%u]\n",__FUNCTION__,bridge_id);
    request.bridge_id = bridge_id;
    request.addr = addr;
    request.layer7_id =proto;
	sock = _do_command(type, &request);
    if(sock == -1)
    	return -1;
    while(1)
    {
        socklen_t remote_addr_len = sizeof(struct sockaddr_nl);
        int recv_len = recvfrom(sock, &respond_msg, size,
                0, (struct sockaddr*)&remote, &remote_addr_len);
        if(recv_len == -1){
            return -1;
        }
        if(recv_len == 0){
            break;
        }
        if(respond_msg.nlhdr.nlmsg_type < NET_STAT_PROTOCOL_SUM_OK || 
            respond_msg.nlhdr.nlmsg_type > NET_STAT_ERROR_INFO)
        {
            break;
        }
		if(respond_msg.nlhdr.nlmsg_type == NET_STAT_NULL_INFO)
			break;
        if(*count>=max_count){
            break;
        }
        *ppblock = (struct respond_message *)malloc(sizeof(struct respond_message));
        if(*ppblock==NULL)
        {
            close(sock);
            return -1;
        }
        memcpy(*ppblock,&respond_msg,sizeof(struct respond_message));
        (*count)++;
        ppblock++;
    }
    close(sock);
    return 0;
}

#define MAX_IP_NUM 220

void save_src_ip_info(struct user_stat_entry **ppnode,struct user_stat_entry *node)
{
    static int total_node = 0;
    int left = 1 , right = total_node, pos;
    u_int64_t  key = node->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes;

    if(ppnode[MAX_IP_NUM] &&  ppnode[MAX_IP_NUM]->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes >= key)
        return;
    if(ppnode[1] == NULL || ppnode[1]->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes <= key){
        pos = 1;
        goto EXIT;
    }
        
    while (left <= right) {
		pos = (left + right) >> 1;
		if (ppnode[pos]->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes > key) {
			left = pos + 1;
		} else if (pos && ppnode[pos - 1]->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes <= key) {
			right = pos - 1;
		} else {
		    break;
		}
	}
    
EXIT:
    if(total_node < (MAX_IP_NUM)){
        if(pos == total_node) pos++;
        total_node++;
    }
    else{
        free(ppnode[total_node]);
        ppnode[total_node] = NULL;
    }
    for(left=total_node;left > pos;left--){
        ppnode[left] = ppnode[left-1];
        ppnode[left-1] = NULL;
    }
    ppnode[pos] = (struct user_stat_entry *)malloc(sizeof(struct user_stat_entry));
    memcpy(ppnode[pos],node,sizeof(struct user_stat_entry));    
    return;
}

int get_src_node_info_all(struct user_stat_entry **ppnode,const int type, const unsigned int bridge_id, 
    const unsigned int ip_addr, const unsigned int layer7_id)
{
    struct user_stat_msg request;
    struct respond_message respond_msg;
    int sock;
    int size = sizeof(struct respond_message);
    struct sockaddr_nl remote;

    request.bridge_id = bridge_id;
    request.addr = ip_addr;
    request.layer7_id = layer7_id;
	sock = _do_command(type, &request);
    if(sock == -1)
		return -1;
    
    while(1)
    {
        socklen_t remote_addr_len = sizeof(struct sockaddr_nl);
        int recv_len = recvfrom(sock, &respond_msg, size,
                0, (struct sockaddr*)&remote, &remote_addr_len);
        if(recv_len == -1){
            return -1;
        }
        if(recv_len == 0){
            break;
        }
        if(respond_msg.nlhdr.nlmsg_type < NET_STAT_PROTOCOL_SUM_OK || 
            respond_msg.nlhdr.nlmsg_type > NET_STAT_ERROR_INFO)
        {
            break;
        }
        
		if(respond_msg.nlhdr.nlmsg_type == NET_STAT_NULL_INFO)
			break;
        int size = (respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr)) / sizeof(struct user_stat_entry);
        int i = 0;
        for(i = 0; i < size; i++){
            struct user_stat_entry * node = &respond_msg.account_entry[i];
            if(node == NULL)
                break;
            if(node->src_ip == 0 && node->layer7_name[0] == '\0' && node->layer7_id == 0)
                break;
            save_src_ip_info(ppnode,node);
        }
    }
    close(sock);    
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

int display_all_src_node_info(uint16_t bridge_id,unsigned int layer7_id)
{
    struct respond_message *net_node[MAX_SUB_NET_NODES]={NULL},*node;
    struct user_stat_entry *ppnode[MAX_IP_NUM + 1]={NULL};
    int   type=NET_STAT_SUBNET_SUM;
    int   count = 0,i=0,j=0;
    int   size = sizeof(struct respond_message);
    DEBUGP("%s:begin bridge_id=[%u]\n",__FUNCTION__,bridge_id);
    
    if(layer7_id)
        type=NET_STAT_PROTOCOL_INFO;
    
    if(get_core_message(net_node,&count,MAX_SUB_NET_NODES,type,bridge_id,0,layer7_id)!=0)
    {
        for(i=0;i<count;i++)
            if(net_node[i]!=NULL)
                free(net_node[i]);
        return -1;
    }
    
    type= layer7_id? NET_STAT_NET_PROTO_INFO : NET_STAT_NET_ADDR_INFO;
    
    //ppnode = (struct respond_message *)malloc(220 * sizeof(struct respond_message));
    for(j=0;j<count;j++)
    {
        node = net_node[j];
        size = (node->nlhdr.nlmsg_len - sizeof(struct nlmsghdr)) / sizeof(struct user_stat_entry);
        for(i = 0; i < size; i++){
            struct user_stat_entry * src_entry = &node->account_entry[i];
            if(src_entry->src_ip == 0 && src_entry->layer7_name[0] == '\0' && src_entry->layer7_id == 0)
                break;
            if(src_entry->src_ip==0){
                ppnode[0] = (struct user_stat_entry *)malloc(sizeof(struct user_stat_entry));
                memcpy(ppnode[0],src_entry,sizeof(struct user_stat_entry));
                //net_addr_info_display_function_1(src_entry);
            }
            else
                get_src_node_info_all(ppnode,type, bridge_id, src_entry->src_ip, layer7_id);        
        }
        free(node);
    }

    printf("{\"data\":[");
    for(j=0;(j<=MAX_IP_NUM && ppnode[j]);j++){
        //if(ppnode[j])
        proto_all_info_display_function(ppnode[j]);
        free(ppnode[j]);
        ppnode[j] = NULL;
    }
    printf("],\"iserror\":0,\"msg\":\"\"}");
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

static void get_timestamp(void)
{
	timestamp = time(NULL);
}

int display_addr_info(int bridge_id, in_addr_t ip_addr){
   	if(ip_addr == 0)
        return display_src_node_info(NET_STAT_PROTOCOL_SUM, bridge_id, ip_addr, 0, addr_info_display_function);
    return display_src_node_info(NET_STAT_HOST_INFO, bridge_id, ip_addr, 0, addr_info_display_function);
}

static int display_interface_stat_info(const unsigned int bridge_id)
{
    struct user_stat_msg request;
    struct respond_message respond_msg;
	
    int sock;
	int size = 0;
	uint64_t * count;
    struct sockaddr_nl remote;
    socklen_t remote_addr_len;
    int recv_len;

    request.bridge_id = bridge_id;
    request.addr = 0;
    request.layer7_id = 0;
	
	size = sizeof(uint64_t) * 12 + sizeof(struct nlmsghdr);
    sock = _do_command(NET_STAT_INTERFACE_INFO,&request);
	if(sock == -1)
		return -1;

    remote_addr_len = sizeof(struct sockaddr_nl);

    recv_len = recvfrom(sock, &respond_msg, size,
            0, (struct sockaddr*)&remote, &remote_addr_len);
    if(recv_len == -1){
        close(sock);
        return -1;
    }
    close(sock);
    printf("{\"data\":[");
	count = (uint64_t *)&respond_msg.account_entry[0];
	printf("{\"name\":\"INT%d\",\"rx_bytes\":%llu,\"rx_packets\":%llu,\"rx_errors\":%llu,\"tx_bytes\":%llu,\"tx_packets\":%llu,\"tx_errors\":%llu,\"time\":%d},",
        bridge_id,
        *count,
		*(count + 1),
		*(count + 2),
		*(count + 3),
		*(count + 4),
		*(count + 5),
		(int)timestamp);

	count += 6;
	printf("{\"name\":\"EXT%d\",\"rx_bytes\":%llu,\"rx_packets\":%llu,\"rx_errors\":%llu,\"tx_bytes\":%llu,\"tx_packets\":%llu,\"tx_errors\":%llu,\"time\":%d}",
        bridge_id,
        *count,
		*(count + 1),
		*(count + 2),
		*(count + 3),
		*(count + 4),
		*(count + 5),
		(int)timestamp);
    printf("],\"iserror\":0,\"msg\":\"\"}");
	return 0;
}

int get_protoid_by_name(char *protoname, uint16_t *protoid)
{
    int i;
    char 	cmd[512];
    FILE	*pp;
    char    proto[20];

    if(protoname[0]==0){
        *protoid = 0;
        return 0;
    }
    
    for(i=0;i<strlen(protoname);i++) 
        protoname[i]=tolower((char)protoname[i]);
    
    if(strcmp(protoname,"total")==0)
        *protoid=0;   
    else if(strcmp(protoname,"analyzing_tcp")==0)
        *protoid=65535;
    else if(strcmp(protoname,"analyzing_udp")==0)
        *protoid=65534;
    else if(strcmp(protoname, "soft_bypass")==0)
        *protoid=65533;
    else if(strcmp(protoname,"others")==0)
        *protoid=65532;
    else if(strcmp(protoname,"others_ipv4")==0)
        *protoid=65531;
    else if(strcmp(protoname,"others_tcp")==0)
        *protoid=65530;
    else if(strcmp(protoname,"others_udp")==0)
        *protoid=65529;
    else if(strcmp(protoname,"icmp")==0)
        *protoid=65528;
    else if(strcmp(protoname,"arp")==0)
        *protoid=65527;
    else if(strcmp(protoname,"mpls")==0)
        *protoid=65526;
    else if(strcmp(protoname, "ipx")==0)
        *protoid=65525;
    else if(strcmp(protoname,"ipv6")==0)
        *protoid=65524;
    else if(strcmp(protoname,"pppoe")==0)
        *protoid=65523;
    else if(strcmp(protoname,"http-browse")==0)
        *protoid=65500;
    else if(strcmp(protoname,"http-download")==0)
        *protoid=65501;
    else if(strcmp(protoname,"http-part")==0)
        *protoid=65502;
    else if(strcmp(protoname,"http-mirror")==0)
        *protoid=65503;
    else if(strcmp(protoname,"http-tunnel")==0)
        *protoid=65504;
    else if(strcmp(protoname, "http-others")==0)
        *protoid=65505;
    else if(strcmp(protoname,"nonstate_tcp")==0)
        *protoid=65522;
    else if(strcmp(protoname,"nonstate_udp")==0)
        *protoid=65521;
    else if(strcmp(protoname,"error_udp")==0)
        *protoid=65519;
    else if(strcmp(protoname,"error_tcp")==0)
        *protoid=65520;
    else if(strcmp(protoname,"p2p-common")==0)
        *protoid=65518;
    else if(strcmp(protoname,"game-flow")==0)    
        *protoid=65517;
    else{
        sprintf(cmd,"/usr/local/sbin/signctl --proto %s --dump |grep Layer7ID | awk -F \' \' \'{print $2}\' 2> /dev/null",protoname);
        if((pp = popen(cmd, "r")) != NULL ){
    		fgets(proto, 20, pp);
    		*protoid = atoi(proto);
    	}
    	pclose(pp);	    
        if(*protoid == 0)
            return PARAMTER_ERROR;
    }

    return 0;
    
}


int get_device_system(int bridgeid)
{
    char 	cmd[512];
    char    strrt[20];
    int     ip_count   = 0;
    int     ip_session = 0;
    int     mem_total  = 0;
    int     mem_free   = 0;
    int     cpu_av     = 0;
    FILE    *pp;

    memset(strrt,0,sizeof(strrt));
    sprintf(cmd,"/usr/local/sbin/ipinfo -b %d -c -p 0 2>/dev/null",bridgeid);
    if((pp = popen(cmd, "r")) != NULL ){
		fgets(strrt, 20, pp);
		ip_count = atoi(strrt);
	}
	pclose(pp);

    memset(strrt,0,sizeof(strrt));
    sprintf(cmd,"/usr/local/sbin/ipinfo -b %d -c -a 0 2>/dev/null",bridgeid);
    if((pp = popen(cmd, "r")) != NULL ){
		fgets(strrt, 20, pp);
		ip_session = atoi(strrt);
	}
	pclose(pp);

    memset(strrt,0,sizeof(strrt));
    //sprintf(cmd,"grep \"MemTotal:\" /proc/meminfo | awk \'{printf \"\%.0f\\\n\", $2/1024\}\'");
    strcpy(cmd,"grep \"MemTotal:\" /proc/meminfo | awk \'{printf \"\%.0f\\\n\", $2/1024}\'");
    if((pp = popen(cmd, "r")) != NULL ){
		fgets(strrt, 20, pp);
		mem_total = atoi(strrt);
	}
	pclose(pp);

    memset(strrt,0,sizeof(strrt));
    //sprintf(cmd,"/bin/grep \"MemFree:\" /proc/meminfo | awk \'{printf \"\%.0f\\n\", $2/1024}\'");
    strcpy(cmd,"/bin/grep \"MemFree:\" /proc/meminfo | awk \'{printf \"\%.0f\\\n\", $2/1024}\'");
    if((pp = popen(cmd, "r")) != NULL ){
		fgets(strrt, 20, pp);
		mem_free = atoi(strrt);
	}
	pclose(pp);

    memset(strrt,0,sizeof(strrt));
    sprintf(cmd,"/usr/local/sbin/mpstat 1 1 | tail -n 1 | awk \'{print $10}\' 2> /dev/null");
    if((pp = popen(cmd, "r")) != NULL ){
		fgets(strrt, 20, pp);
		cpu_av = atoi(strrt);
	}
	pclose(pp);

    printf("{\"data\":[");
    printf("{\"bridgeid\":%d,\"ip_count\":%u,\"ip_session\":%u,\"mem_total\":%u,\"mem_free\":%u,\"cpu_av\":%u}",
        bridgeid,ip_count,ip_session,mem_total,mem_free,cpu_av);
    printf("],\"iserror\":0,\"msg\":\"\"}");
    
    return 0;
}

int display_ids_log(void)
{
    struct ids_log_msg request;
    struct ids_log_respond_info respond_msg;
    int sock;
    int size = sizeof(struct ids_log_respond_info);
    struct sockaddr_nl remote;
    int cnt = 0;

    DEBUGP("%s:begin\n", __FUNCTION__);

    memset(&request, 0, sizeof(struct ids_log_msg));
	sock = __do_ids_command(IDS_LOG_SHOW, &request);
    if(sock == -1)
    {
        DEBUGP("%s: __do_ids_command failed\n", __FUNCTION__);
        return -1;
    }
    
    printf("{\"data\":[");
    while(1)
    {
        socklen_t remote_addr_len = sizeof(struct sockaddr_nl);

        DEBUGP("%s: recv ids log, cnt = %d\n", __FUNCTION__, cnt);
        
        int recv_len = recvfrom(sock, &respond_msg, size,
                0, (struct sockaddr*)&remote, &remote_addr_len);

        DEBUGP("%s: finish recv ids log, cnt = %d\n", __FUNCTION__, cnt);
        if(recv_len <= 0){
            break;
        }

        if ((respond_msg.nlhdr.nlmsg_type == IDS_LOG_DONE) || 
            (respond_msg.nlhdr.nlmsg_type != IDS_LOG_SHOW_DONE))
        {
            break;
        }
        
        if (cnt > 0)
        {
            printf(",");
        }
        struct ids_log_msg * msg = &respond_msg.msg;
        printf("{\"message\":\"%s\"}", msg->message);
        cnt++;
    }
    
    printf("],\"iserror\":0,\"msg\":\"\"}");
    close(sock);    
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

int ids_log_flush(void)
{
    struct ids_log_msg request;
    struct ids_log_respond_info respond_msg;
    int sock;
    int size = sizeof(struct ids_log_respond_info);
    struct sockaddr_nl remote;
    int ret = -1;

    DEBUGP("%s:begin\n", __FUNCTION__);

    memset(&request, 0, sizeof(struct ids_log_msg));
	sock = __do_ids_command(IDS_LOG_FLUSH, &request);
    if(sock == -1)
		goto error_exit;
    
    socklen_t remote_addr_len = sizeof(struct sockaddr_nl);
    int recv_len = recvfrom(sock, &respond_msg, size,
            0, (struct sockaddr*)&remote, &remote_addr_len);
    if(recv_len <= 0){
        goto error_exit;
    }
    if(respond_msg.nlhdr.nlmsg_type > IDS_LOG_SHOW_DONE)
    {
        goto error_exit;
    }
    
    ret = 0;
    printf("{\"iserror\":0,\"msg\":\"\"}");
	goto free_exit;
    
error_exit:
	printf("{\"iserror\":%d,\"msg\":\"\"}", ret);
free_exit:
    if (sock > 0)
	    close(sock);
	return ret;
}

int get_bridgeid_by_name(char * bridgename,uint16_t *bridgeid)
{
    if(bridgename[0]==0){
        *bridgeid = 0;
        return 0;
    }
    *bridgeid = atoi(bridgename);
    if(*bridgeid<5)
        return 0;
    return PARAMTER_ERROR;
}

int get_ipaddr_by_name(char * ipaddrname,in_addr_t *ip_addr)
{
    if(ipaddrname[0]==0){
        *ip_addr = 0;
        return 0;
    }
    *ip_addr = inet_addr(ipaddrname);
	if(*ip_addr == INADDR_NONE){
		//perror("input address is invalid\n");
		return PARAMTER_ERROR;
	}
    return 0;
}

/*************************************************
Add by:chenhao
Date:2013-8-2 16:11:17
*************************************************/
int get_ap_info(void)
{
	int                 						sock;
	int                 						recv_len;
	int                 						respond_size = 0;
	int 								first = 1;
	int								nums = -1;
	void                					*respond = NULL;
	struct msghdr       				msg;
	struct iovec        					iov;
	struct sockaddr_nl 				remote;
	struct sockaddr_nl  				local;
	struct nlmsghdr     				*nlhdr = NULL;
	struct nlmsghdr     				*rnlhdr = NULL;
	mac_auth_msg_t					*request = NULL;
	mac_auth_getap_respond_info_t	   	*respond_data = NULL;
	shunwanap_msg_max_t				respond_s[MAX_USER_NUMS];
	socklen_t           					remote_addr_len;
       int                 						i,res, ret = -1;

	memset(respond_s,'\0',MAX_USER_NUMS);

	request = (mac_auth_msg_t	*)malloc(sizeof(mac_auth_msg_t));
	if(request == NULL)
	{
		goto exit;
	}
	memset(request,'\0',sizeof(request));

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_MAC_AUTH); 

	if(sock < 0){
		goto exit;
	}

	memset(&local, 0, sizeof(struct sockaddr_nl));

	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
		goto exit;
	}
	
	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(mac_auth_msg_t)));
	if(nlhdr == NULL)
	{
		goto exit;
	}

	memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(mac_auth_msg_t));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(mac_auth_msg_t));
	nlhdr->nlmsg_pid = getpid(); 
	nlhdr->nlmsg_flags = NLM_F_REQUEST;
	nlhdr->nlmsg_type = MAC_AUTH_GET_AP_INFO;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&remote;
	msg.msg_namelen = sizeof(remote);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	res = sendmsg(sock, &msg, 0);
	if (res == -1) 
    	{
    		printf("send error %s ",strerror(errno));
		 goto exit;
	}

	remote_addr_len = sizeof(struct sockaddr_nl);
	respond_size = sizeof(struct mac_auth_getap_respond_info);
    
	respond = malloc(respond_size);
	if(respond == NULL)
    	{
		goto exit;
	}

RECV:
	memset(respond, 0, respond_size);
	
	recv_len = recvfrom(sock, respond, respond_size,
	       0, (struct sockaddr*)&remote, &remote_addr_len);

	if(recv_len == -1)
    {
		goto exit;
	}

	rnlhdr = (struct nlmsghdr *)respond;

	switch(rnlhdr->nlmsg_type)
      {	
		case MAC_AUTH_DONE:
            		ret = 0;
			break;

             case MAC_AUTH_GET_AP_INFO_DONE:
			respond_data = (mac_auth_getap_respond_info_t *)respond;
			nums++;
			strncpy(respond_s[nums].apname,respond_data->info.apname,18);
			respond_s[nums].num = respond_data->info.num;
			respond_s[nums].check_time = respond_data->info.check_time;
			respond_data = NULL;
                    goto RECV;
					
		case MAC_AUTH_ERROR:
			break;
		
		case MAC_AUTH_MEMERR:
			break;
			
		default:
			break;
	}

	if(ret == 0)
	{
		printf("{\"data\":[");
		for(i = 0;i <= nums;i++)
		{
			if(first)
			{
				first = 0;
			}
			else
			{
				printf(",");
			}
			
			printf("{\"apname\":\"%s\",\"ternum\":%d,\"check_time\":%d}",
					respond_s[i].apname,
					respond_s[i].num,
					respond_s[i].check_time);
		}
		printf("],\"iserror\":%d,\"message\":\"\"}",ret);
		goto exit;
	}
	else
	{
		goto exit;
	}

exit:
	close(sock);
	if(request)
	{
		free(request);
	}
	
	if(respond)
	{
		free(respond);
	}

	if(nlhdr)
	{
		free(nlhdr);
	}
	
	return ret;
}

int cgiMain()
{
	char  	  type[STRING_LENGTH];
    char      in_bridge[STRING_LENGTH];
    //char      in_addr[STRING_LENGTH];
    char      ipaddr[STRING_LENGTH];
    char      protoinfo[STRING_LENGTH];
    in_addr_t ip_addr;
    uint16_t  protoid;
	int		  result = PARAMTER_ERROR;
    uint16_t  bridge_id;
    char      intername[STRING_LENGTH];
    
	cgiFormString("type",type,STRING_LENGTH);
	cgiFormString("bridge",in_bridge,STRING_LENGTH);
    result = get_bridgeid_by_name(in_bridge,&bridge_id);
    if(result != 0){
       goto EXIT;
    }
    
    get_timestamp();
#if _MGTCGI_DEBUG_GET_
    printf("content-type:text/html\n\n");   
#else
    printf("content-type:application/json\n\n");    
#endif
	if (strcmp(type,"proto") == 0 ){//列表显示所有
        cgiFormString("ipaddr",ipaddr,STRING_LENGTH);
        result = get_ipaddr_by_name(ipaddr,&ip_addr);
        if(result != 0){
           goto EXIT;
        }
		result = display_addr_info(bridge_id,ip_addr);
	}	
	else if(strcmp(type,"ipaddr") == 0 ){//点击添加
        cgiFormString("proto",protoinfo,STRING_LENGTH);
        result = get_protoid_by_name(protoinfo,&protoid);
        //DEBUGP("%s: protoinfo=%s protoid=%d\n",__FUNCTION__,protoinfo,protoid);
        if(result!=0)
            goto EXIT;
        result = display_all_src_node_info(bridge_id,protoid);
	}
    else if(strcmp(type,"channel") == 0){
        int ifindex;
        uint8_t dir;
        char str_dir[STRING_LENGTH]={0};
        cgiFormString("interface",intername,STRING_LENGTH);
        cgiFormString("dir",str_dir,STRING_LENGTH);
        ifindex = atoi(intername);
        dir = atoi(str_dir);
        result = channel_show_stat_all(ifindex,dir);
    }
    else if(strcmp(type,"system")==0){
        result = get_device_system(bridge_id);
    }
    else if(strcmp(type,"interface")==0){
        result = display_interface_stat_info(bridge_id);
    }
    else if (strcmp(type, "idslog-show") == 0)
    {
        result = display_ids_log();
    }
    else if (strcmp(type, "idslog-flush") == 0)
    {
        result = ids_log_flush();
    }
	else if(strcmp(type,"apinfo") == 0)
	{
		result = get_ap_info();
	}
    
EXIT:
	error_msg(result);    
	return (0);
}
