#include <stdio.h>
#include <stdint.h>
#include <opt.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <time.h>
#include "mgtcgi_xmlconfig.h"
#include "common.h"
#include "xmlwrapper4c.h"

//#define TC_LONG_NAME "tc"
//#define TC_NAME "tc"
//#define TC_DESCRIPTION "Traffic Control"
//#define TC_VERSION "2.1.01"

//#define IFNAMSIZ 16
#define TC_CAMMAND_DONE            0x00
#define TC_CAMMAND_ERROR           0x01
#define TC_INTERFACE_ERROR         0x02
#define TC_MALLOC_FAIL             0x03
#define TC_ROOT_SHOW_DONE          0x04
#define TC_ROOT_STAT_SHOW_DONE     0x05
#define TC_CLASS_SHOW_DONE         0x06
#define TC_CLASS_STAT_SHOW_DONE    0x07
#define TC_ALL_CLASS_SHOW_DONE     0x08
#define TC_ALL_STAT_SHOW_DONE      0x09
#define SHOW_MAX_NODES             100

#define NETLINK_TC 26

#define TC_NETLINK_BASE    0x10
#define TC_NETLINK_BASE    0x10
#define TC_ROOT_INIT        (TC_NETLINK_BASE+1)
#define TC_ROOT_CHANGE      (TC_NETLINK_BASE+2)
#define TC_ROOT_START       (TC_NETLINK_BASE+3)
#define TC_ROOT_DELETE      (TC_NETLINK_BASE+4)
#define TC_CLASS_ADD        (TC_NETLINK_BASE+5)
#define TC_CLASS_DELETE     (TC_NETLINK_BASE+6)
#define TC_CLASS_CHANGE     (TC_NETLINK_BASE+7)
#define TC_ROOT_SHOW        (TC_NETLINK_BASE+8)
#define TC_ROOT_STAT_SHOW   (TC_NETLINK_BASE+9)
#define TC_CLASS_SHOW       (TC_NETLINK_BASE+10)
#define TC_CLASS_STAT_SHOW  (TC_NETLINK_BASE+11)
#define TC_ALL_CLASS_SHOW   (TC_NETLINK_BASE+12)
#define TC_ALL_STAT_SHOW    (TC_NETLINK_BASE+13)
#define TC_ROOT_SET_DEFAULT (TC_NETLINK_BASE+14)
#define TC_SCHE_STATE       (TC_NETLINK_BASE+15)
#define TC_SCHE_INIT        (TC_NETLINK_BASE+16)
#define TC_NETLINK_MAX      (TC_NETLINK_BASE+17)


#define TC_MIN_SHARE_ID 65280
#define TC_MAX_PRIO 17

enum tc_class_type{TC_SHARE, TC_PIPE, TC_VC, TC_ROOT, TC_MAX_CLASS_TYPE};
enum tc_attibute_type{TC_NONE, TC_PER_IP, TC_MAX_ATTR_TYPEX};

struct user_tc_msg
{
    char     name[IFNAMSIZ];
    char     devname[IFNAMSIZ];
    uint16_t index;
    uint16_t classid;
    uint8_t  mark;
    uint8_t  state;
    uint8_t  dir;
    uint32_t maxrate[2];
    uint16_t defcls;
    uint16_t parentid;
    uint16_t shareid;
    uint16_t weight;
    uint16_t per_limit;
    uint8_t  type;
    uint8_t  prio;
    uint8_t  attr;
    uint8_t  buff;

};
/**
 * struct gnet_stats_basic - byte/packet throughput statistics
 * @bytes: number of seen bytes
 * @packets: number of seen packets
 */
struct tc_stats_basic
{
    uint64_t    bytes;          /* NUmber of enqueues bytes */
    uint64_t    packets;        /* Number of enqueued packets   */
};

struct tc_stats_queue
{
    uint32_t    qlen;
    uint32_t    backlog;
    uint64_t    drops;
    uint64_t    requeues;
    uint64_t    overlimits;
};

struct tc_stats_rate_est
{
    uint32_t    bps;
    uint32_t    pps;
};

struct tc_user_stat_entry
{
    char devname[IFNAMSIZ];
    char classname[IFNAMSIZ];
    uint16_t classid;
    uint8_t  type;
    struct tc_stats_basic bstats;
    struct tc_stats_queue qstats;
    
};

struct tc_stat_respond_info
{
    struct nlmsghdr nlhdr;
    struct tc_user_stat_entry info[SHOW_MAX_NODES];
};

struct tc_respond_info
{
    struct nlmsghdr nlhdr;
    struct user_tc_msg info[SHOW_MAX_NODES];
};

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format, args...)
#endif

int show_stat_info(struct tc_stat_respond_info *respond, int ifindex){
    int i = 0;
    static int j = 0;
    struct tc_user_stat_entry *msg = respond->info;    
    struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
    struct mgtcgi_xml_layer7_protocol_show_info *temp=NULL;
    int size,k;
    long int nowtime = 0;

    get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7TYPE_SHOWS,
                            "chinese",(void**)&l7protocol,&size);
    if(size == 0)
    {
        return 0;
    }
    nowtime = time((time_t*)NULL);
#if 0    
    if(strncmp(msg[0].devname,"INT",strlen("INT"))==0){
        dir = 0;
    }
    else{
        dir = 1;
    }
#endif

    for(i=0;i<SHOW_MAX_NODES;i++)
    {
       //if(msg[i].devname[0] == '\0')
       //     break;
        temp = NULL;
        if(msg[i].type != TC_VC){
            continue;
        }
        if(msg[i].classid / 64 != ifindex)
            continue;
        if(j)
            printf(",");
        else
            j=1;

        for( k= 0; k < l7protocol->num; k++){
            if(strcmp(msg[i].classname,l7protocol->pinfo[k].name) ==0){
                temp=&l7protocol->pinfo[k];
                break;
            }
        }
        
        if(temp==NULL)
            printf("{\"classname\":\"%s\",\"parentid\":%d,\"packets\":%llu,\"bytes\":%llu,\"classid\":%d,\"drops\":%llu,\"time\":%lu}", 
               msg[i].classname,msg[i].classid&0xFFC0,
               msg[i].bstats.packets,msg[i].bstats.bytes,msg[i].classid,msg[i].qstats.drops,nowtime);
        else
            printf("{\"classname\":\"%s\",\"parentid\":%d,\"packets\":%llu,\"bytes\":%llu,\"classid\":%d,\"drops\":%llu,\"time\":%lu}", 
               temp->match,msg[i].classid&0xFFC0,
               msg[i].bstats.packets,msg[i].bstats.bytes,msg[i].classid,msg[i].qstats.drops,nowtime);
    }
    
    if(l7protocol){
        free_xml_node((void*)&l7protocol);
    }
    return 0;
}

int _channel_get_info(int type, struct user_tc_msg *request,int ifindex)
{
	int   sock;
	struct sockaddr_nl remote;
	struct sockaddr_nl local;
	struct nlmsghdr * nlhdr;
	struct iovec iov;
    socklen_t remote_addr_len;
    void *respond;
    struct nlmsghdr * rnlhdr=NULL;
    int  respond_size;
    int recv_len;
	struct msghdr msg;
        
    sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_TC); /* NETLINK_TC 用于区分服务器端地址 */
    if(sock < 0){
        //fprintf(stderr,"create socket error\n");
        return -1;
    }

    memset(&local, 0, sizeof(struct sockaddr_nl));

    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();
    local.nl_groups = 0; /* nl_groups用于指定加入的多播组，0表示不加入任何多播组 */

    if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
        //fprintf(stderr,"bind socket error\n");
        close(sock);
        return -1;
    }

    memset(&remote, 0, sizeof(struct sockaddr_nl));
    remote.nl_family = AF_NETLINK;
    remote.nl_pid    = 0;
    remote.nl_groups = 0;

    memset(&msg, 0, sizeof(struct msghdr));
    
    nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct user_tc_msg)));

    memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(struct user_tc_msg));

    nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct user_tc_msg));
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
         free(nlhdr);
         return -1;
    }
    respond_size = sizeof(struct tc_stat_respond_info);
    respond = malloc(respond_size);
    if(!respond)
    {        
        goto exit;
    }

    printf("{\"data\":[");
recv:
    memset(respond, 0, respond_size);
    remote_addr_len = sizeof(struct sockaddr_nl);
   
    recv_len = recvfrom(sock, respond, respond_size,
           0, (struct sockaddr*)&remote, &remote_addr_len);

    if(recv_len == -1){
        printf("],\"iserror\":0,\"msg\":\"\"}");
        goto exit;
    }
    
    rnlhdr = (struct nlmsghdr *)respond;
    
    switch(rnlhdr->nlmsg_type){
        case TC_CAMMAND_DONE:
            break;
        case TC_CAMMAND_ERROR:
        case TC_INTERFACE_ERROR:
        case TC_MALLOC_FAIL:
            break;
#if 0
        case TC_ROOT_SHOW_DONE:
            show_root_info((struct tc_respond_info*)respond);
            goto recv;
        case TC_CLASS_SHOW_DONE:
        case TC_ALL_CLASS_SHOW_DONE:
            show_class_info((struct tc_respond_info *)respond);
            goto recv;
#endif
        case TC_ROOT_STAT_SHOW_DONE:
        case TC_CLASS_STAT_SHOW_DONE:
        case TC_ALL_STAT_SHOW_DONE:
            show_stat_info((struct tc_stat_respond_info *)respond,ifindex);
            goto recv;
        default:
            break;
    }
    printf("],\"iserror\":0,\"msg\":\"\"}");
exit:
    if(respond)
        free(respond);
    ret = 0;

    if(nlhdr)
        free(nlhdr);
    close(sock);
	return 0;
}

int channel_get_info(int type, struct user_tc_msg *request,int ifindex)
{
   return _channel_get_info(type, request,ifindex);//respond_msg, sizeof(struct tc_respond_info));
}

int __show_channel_all(int command, int ifindex,uint8_t dir){ 
    struct user_tc_msg request;
    memset(&request, 0, sizeof(request));
    request.dir = dir;
    //memcpy(request.devname, intername, strlen(intername));
    channel_get_info(command, &request,ifindex);
    return 0;
}

int channel_show_stat_all(int ifindex,uint8_t dir){
    __show_channel_all(TC_ALL_STAT_SHOW, ifindex,dir);
    return 0;
}

