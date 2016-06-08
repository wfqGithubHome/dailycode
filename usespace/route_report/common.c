#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
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

#include "include/common.h"

int do_get_command(const char *cmd, char *buffer, int lenght)
{
	FILE *pp = NULL;
	
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
    
	pp=popen(cmd,"r");
    if(pp == NULL){
        return -1;
    }

	fgets(buffer, lenght, pp);
    pclose(pp);
	buffer[strlen(buffer)-1] = 0;
	
	return 0;
}


int _do_command(int type, struct user_stat_msg * request)
{
	int   sock;
	struct sockaddr_nl remote;
	struct sockaddr_nl local;
	struct nlmsghdr * nlhdr = NULL;
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
        if(nlhdr){
            free(nlhdr);
        }
        return -1;
    }

    if(nlhdr){
        free(nlhdr);   
    }
	return sock;
}

