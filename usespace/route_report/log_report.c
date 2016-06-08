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

#include "include/log_report.h"

#if 0
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif
int get_curr_unixtime(void)  
{  
	char 		cmd[]="/bin/date +%s";
	char 		buffer[32];
	FILE 		*fp = NULL;
	int time=0;

	memset(buffer, 0, sizeof(buffer));
	//memset(cmd, 0, sizeof(cmd));
	//snprintf(cmd, sizeof(cmd),"");

	fp=popen(cmd,"r");
	fgets(buffer, 32, fp);
	pclose(fp);
	time= atoi(buffer);
	return time-8*60*60;
}
void anys_msg(char *msg_tem,char **log_time,char **log_ip){
	int msg_len=0,index,sign=0;
	msg_len=strlen(msg_tem);
	for(index=msg_len;index>0;index--){
		if(msg_tem[index]==32){
			*log_ip=msg_tem+index+1;
			break;
		}
	}
	for(index=0;index<msg_len;index++){
		if(msg_tem[index]==32){
			sign++;
			if(sign==2){
				msg_tem[index]='\0';
				*log_time=msg_tem;
				break;
			}
		}
	}
	//msg_tem[msg_len];
	return;
}
int strtotime(char datetime[]){
	struct tm tm_time;
	int unixtime;
	strptime(datetime, "%Y-%m-%d %H:%M:%S", &tm_time);
	unixtime = mktime(&tm_time);
	return unixtime;
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
int display_ids_log(char *ids_log_data)
{
    struct ids_log_msg request;
    struct ids_log_respond_info respond_msg;
    int sock;
    int size = sizeof(struct ids_log_respond_info);
    struct sockaddr_nl remote;
    int cnt = 0,msg_len=0;
   char *log_time=NULL,*log_ip=NULL;
   char msg_temp[MSG_TEM_LEN]={0},log_data_temp[LOG_TEM_LEN]={0};
    DEBUGP("%s:begin\n", __FUNCTION__);
     
   
    memset(&request, 0, sizeof(struct ids_log_msg));
	sock = __do_ids_command(IDS_LOG_SHOW, &request);
    if(sock == -1)
    {
        DEBUGP("%s: __do_ids_command failed\n", __FUNCTION__);
        return -1;
    }
    
    
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
        

        struct ids_log_msg * msg = &respond_msg.msg;
	msg_len=strlen(msg->message);
	if(msg_len>=MSG_TEM_LEN)continue;
	
	strcpy(msg_temp,msg->message);
	
	anys_msg(msg_temp,&log_time,&log_ip);
	if( get_curr_unixtime() > strtotime(log_time)+5*60 )continue;
	if(strlen(ids_log_data)>IDS_LOG_LEN-256)break;
	sprintf(log_data_temp,LOG_POST_OPT_TEMP,1,msg->message,"invalid tcp connections",strtotime(log_time),log_ip);

	if (cnt > 0) strcat(ids_log_data,",");
        
	strcat(ids_log_data,log_data_temp);
        cnt++;
    }
    
    close(sock);    
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}
