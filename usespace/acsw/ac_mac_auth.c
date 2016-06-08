#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <link.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/utsname.h>

#include "include/utils.h"
#include "include/ac_mac_auth.h"
#include "include/ac_report.h"

#if 1
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

static void process_respond_show_from_kernel(struct mac_auth_respond_info* respond, 
    struct ac_report_mac *macs, int *macs_len)
{
    struct mac_auth_msg *pinfo = respond->info;
    int                 idx = *macs_len;
    int                 i;

    DEBUGP("%s: begin\n", __FUNCTION__);

    for (i = 0; i < SHOW_MAX_NODES; i++)
    {
        if((pinfo[i].mac[0]+pinfo[i].mac[1]+pinfo[i].mac[2]+
		    pinfo[i].mac[3]+pinfo[i].mac[4]+pinfo[i].mac[5]) == 0)
		{
			break;
		}

        memcpy((char *)macs[idx].mac, (char *)pinfo[i].mac, MAC_ADDR_MAX_NUM);

        idx++;
    }

    *macs_len = idx;
    DEBUGP("%s: finish, current mts_len: %d\n", __FUNCTION__, *macs_len);
    return;
}

static void process_respond_getap_from_kernel(struct mac_auth_getap_respond_info* respond, 
    char *ac_url, char *ac_id, enum ac_report_warning_type warning_type, 
    ac_warning_func process_warning, ac_setting_func process_setting)
{
    struct shunwanap_msg_max *pinfo = &respond->info;

    DEBUGP("%s: begin, ac_id: %s, warning_type: %u, apname: %s, diff_time: %u\n", 
        __FUNCTION__, ac_id, warning_type, pinfo->apname, pinfo->check_time);

    if (pinfo->check_time < AC_AP_TIMEOUT_WARING_THRESHOLD_LOW || 
        pinfo->check_time > AC_AP_TIMEOUT_WARING_THRESHOLD_HIGH)
    {
        process_setting(ac_url, ac_id, pinfo->apname);
        return;
    }

    process_warning(ac_url, ac_id, pinfo->apname, warning_type);
    DEBUGP("%s: finish\n", __FUNCTION__);
    return;
}

static int communicate_kernel(struct mac_auth_msg *request, int type, struct ac_report_mac *macs, 
    int *macs_len, char *ac_url, char *ac_id, enum ac_report_warning_type warning_type, 
    ac_warning_func process_warning, ac_setting_func process_setting)
{
	int                 sock;
	int                 recv_len;
	int                 respond_size = 0;
	void                *respond = NULL;
	struct msghdr       msg;
	struct iovec        iov;
	struct sockaddr_nl  remote;
	struct sockaddr_nl  local;
	struct nlmsghdr     * nlhdr = NULL;
	struct nlmsghdr     * rnlhdr = NULL;
	socklen_t           remote_addr_len;
    int                 res, ret = -1;

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_MAC_AUTH); 

	if(sock < 0){
		DEBUGP("create socket error\n");
		return ret;
	}

	memset(&local, 0, sizeof(struct sockaddr_nl));

	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
		DEBUGP("bind socket error\n");
		goto exit;
	}

	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct mac_auth_msg)));

	memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(struct mac_auth_msg));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct mac_auth_msg));
	nlhdr->nlmsg_pid = getpid(); 
	nlhdr->nlmsg_flags = NLM_F_REQUEST;
	nlhdr->nlmsg_type = type;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&remote;
	msg.msg_namelen = sizeof(remote);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	res = sendmsg(sock, &msg, 0);
	if (res == -1) 
    {
		 DEBUGP("sendmsg error\n");
		 goto exit;
	}

	remote_addr_len = sizeof(struct sockaddr_nl);

    if (type == MAC_AUTH_GET_AP_INFO)
    {
        respond_size = sizeof(struct mac_auth_getap_respond_info);
    }
    else
    {
	    respond_size = sizeof(struct mac_auth_respond_info);
    }
    
	respond = malloc(respond_size);
	if(respond == NULL)
    {
		DEBUGP("malloc wrong\n");
		goto exit;
	}

RECV:
	memset(respond, 0, respond_size);
	
	recv_len = recvfrom(sock, respond, respond_size,
	       0, (struct sockaddr*)&remote, &remote_addr_len);

	if(recv_len == -1)
    {
		DEBUGP("recvmsg error\n");
		goto exit;
	}

	rnlhdr = (struct nlmsghdr *)respond;

	switch(rnlhdr->nlmsg_type)
    {	
		case MAC_AUTH_DONE:
            ret = 0;
			break;

		case MAC_AUTH_CONF_SHOW_DONE:
			process_respond_show_from_kernel((struct mac_auth_respond_info*)respond, macs, macs_len);
			goto RECV;

        case MAC_AUTH_GET_AP_INFO_DONE:
            process_respond_getap_from_kernel((struct mac_auth_getap_respond_info*)respond, ac_url, ac_id, 
                warning_type, process_warning, process_setting);
            goto RECV;
		case MAC_AUTH_ERROR:
			DEBUGP("mac_auth command error\n");
			break;
		
		case MAC_AUTH_MEMERR:
			DEBUGP("mac_auth malloc memery error\n");
			break;
			
		default:
			DEBUGP("mac_auth error, error type : %u\n", rnlhdr->nlmsg_type);
			break;
	}

exit:
	close(sock);
	if(respond)
	{
		free(respond);
	}

	if(nlhdr)
	{
		free(nlhdr);
	}

	return  0;
}

int ac_mac_get_from_kernel(struct ac_report_mac *macs, int *macs_len)
{
    struct mac_auth_msg request;
    int ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    memset(&request, 0, sizeof(request));
    ret = communicate_kernel(&request, MAC_AUTH_CONF_SHOW, macs, macs_len, NULL, NULL, AC_REPORT_WARNING_TYPE_NONE, NULL, NULL);

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int ac_mac_set_to_kernel(struct ac_respond_mac_time *mts, int mts_len, 
    enum ac_mac_auth_op_type type)
{
    struct mac_auth_msg request;
    int i, ret = -1;

    DEBUGP("%s: msg_len: %d\n", __FUNCTION__, mts_len);

    for (i = 0; i < mts_len; i++)
    {
        DEBUGP("%dth mac: "MACQUAD_FMT"\n", i, MACQUAD(mts[i].mac));
        if (type == AC_MAC_AUTH_TABLE_INIT)
        {
            memset(&request, 0, sizeof(request));
            strcpy(request.name, "vip");
            memcpy(request.mac, mts[i].mac, MAC_ADDR_MAX_NUM);
            communicate_kernel(&request, MAC_AUTH_CONF_ADD, NULL, 0, NULL, NULL, AC_REPORT_WARNING_TYPE_NONE, NULL, NULL);
            continue;
        }
        
        if (mts[i].time > 0)
        {
            memset(&request, 0, sizeof(request));
            strcpy(request.name, "user1");
            memcpy(request.mac, mts[i].mac, MAC_ADDR_MAX_NUM);
            communicate_kernel(&request, MAC_AUTH_CONF_ADD, NULL, 0, NULL, NULL, AC_REPORT_WARNING_TYPE_NONE, NULL, NULL);
            continue;
        }

        memset(&request, 0, sizeof(request));
        memcpy(request.mac, mts[i].mac, MAC_ADDR_MAX_NUM);
        communicate_kernel(&request, MAC_AUTH_CONF_DEL, NULL, 0, NULL, NULL, AC_REPORT_WARNING_TYPE_NONE, NULL, NULL);
    }

    ret = 0;
    DEBUGP("%s: finish\n", __FUNCTION__);
    return ret;
}

int ac_mac_auth_ap_info_get(char * ac_url, char *ac_id, enum ac_report_warning_type warning_type, 
    ac_warning_func process_warning, ac_setting_func process_setting)
{
    struct mac_auth_msg request;
    int ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    memset(&request, 0, sizeof(request));
    ret = communicate_kernel(&request, MAC_AUTH_GET_AP_INFO, NULL, 0, ac_url, ac_id, warning_type, 
        process_warning, process_setting);

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}


