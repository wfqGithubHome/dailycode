#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#include "include/ac_ip_classify.h"

#if 0
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

static int communicate_kernel(struct ip_classify_msg *request,int type)
{
	int                 sock;
	int                 recv_len;
	int                 respond_size;
	void                *respond;
	struct msghdr       msg;
	struct iovec        iov;
	struct sockaddr_nl  remote;
	struct sockaddr_nl  local;
	struct nlmsghdr     * nlhdr;
	struct nlmsghdr     * rnlhdr=NULL;
	socklen_t           remote_addr_len;
    int                 res, ret = -1;

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_IP_CLASSIFY); 

	if(sock < 0)
    {
		DEBUGP("create socket error\n");
		return ret;
	}

	memset(&local, 0, sizeof(struct sockaddr_nl));

	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0)
    {
		DEBUGP("bind socket error\n");
		goto exit;
	}

	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct ip_classify_msg)));

	memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(struct ip_classify_msg));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct ip_classify_msg));
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
	respond_size    = sizeof(struct ip_classify_respond_info);
	respond         = malloc(respond_size);

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
		case IP_CLASSIFY_DONE:
            ret = 0;
			break;
        
		case IP_CLASSIFY_ERROR:
			DEBUGP("ip-classify command error\n");
			break;
		
		case IP_CLASSIFY_MEMERR:
			DEBUGP("ip-classify malloc memery error\n");
			break;
			
		default:
			DEBUGP("ip-classify error, error type : %u\n", rnlhdr->nlmsg_type);
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

	return  ret;
}

int ac_ips_set_to_kernel(uint32_t *ips, int ips_len, uint32_t *sips, int sips_len)
{
    struct ip_classify_msg request;
    int i, ret = -1;

    DEBUGP("%s: begin, ips_len: %d, sips_len: %d\n", __FUNCTION__, ips_len, sips_len);

    for (i = 0; i < ips_len; i++)
    {
        DEBUGP("%dth ip: "NIPQUAD_FMT"\n", i, NIPQUAD(ips[i]));
        request.ip_addr = ips[i];
        request.dport = 0;
        request.lay7id = 65532;
        request.masklen = 32;
        
        communicate_kernel(&request, IP_CLASSIFY_ADD);
    }

    for (i = 0; i < sips_len; i++)
    {
        DEBUGP("%dth server ip: "NIPQUAD_FMT"\n", i, NIPQUAD(sips[i]));
        request.ip_addr = sips[i];
        request.dport = 0;
        request.lay7id = 65533;
        request.masklen = 32;
        
        communicate_kernel(&request, IP_CLASSIFY_ADD);
    }

    ret = 0;
    
    DEBUGP("%s: finish\n", __FUNCTION__);
    return ret;
}

