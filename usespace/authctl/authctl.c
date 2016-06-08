#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <opt.h>
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
#include "authctl.h"


#define AUTH_CTL_VERSION        "1.0.04"
#define AUTH_CTL_NAME           "authctl"
#define AUTH_CTL_LONG_NAME      "authctl"
#define AUTH_CTL_DESCRIPTION    "DPI Auth contorl and information dump"

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format,args...)
#endif

#define NETLINK_AUTH 29
int     enable = 0;

typedef struct config_info
{
    int     debug;
	int		count;
    int     login;
    int     logout;
    int     flush;
    int     show;
    int     add;
    int     set;
	int		bridge_id;
    int     user_id;
    int     user_id_set;
    int     group_id;
    int     group_id_set;
    int     authed;
    int     nodelete;
    int     mnum;
    int     max_time;
    int     enable_set;
    char    *addr;
	char	*net;
	char	*username;
    char    *passwd;
	char    *groupname;
    char    *filename;
    char    *startip;
    char    *endip;
}config_info_t;

typedef struct user_auth_respond {
	struct nlmsghdr nlhdr;
    union
    {
        struct auth_entry       result_entry;
	    struct auth_info_entry  info_entry;
    } msg;
} user_auth_respond;

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

#define NETQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2]


config_info_t   config;
time_t			timestamp;
/*
static void get_utf8_name(unsigned char * string, unsigned char * utf8_string)
{
    unsigned char ch; 
    unsigned char * string_ptr = string;
    unsigned char * utf8_string_ptr = utf8_string;
    int i = 0;

    *utf8_string = '\0';
    ch = *string_ptr;

    while(ch != '\0')
    {
        snprintf((char *)utf8_string_ptr, 4, "%%%.2X", ch);
        utf8_string_ptr += 3;
        i += 3;
		string_ptr++;
        ch = *string_ptr;
    }
    return;

}
*/
static void get_timestamp(void)
{
	timestamp = time(NULL);
}

static int parse(int *argc, char ***argv)
{
    char    buff[1024];
    char    *addr;
	char	*username;
    char    *passwd;
	char    *groupname;
    char    *filename;
	char	*net;
    char    *startip;
    char    *endip;
    
    int     version = 0;
    int     usage = 0;
    int     opt_count = 0;

    int     nodelete = 0;
    int     max_num = 0;
    
    config.debug = 0;
    config.bridge_id = 0;
    config.addr = NULL;
	config.net = NULL;
	config.username = NULL;
	config.groupname = NULL;
    config.user_id = 0;
    config.show = 0;
    config.count = 0;
    config.login = 0;
    config.logout = 0;
    config.show = 0;
    config.add = 0;
    config.flush = 0;
    config.authed = 0;
    config.user_id_set = 0;
    config.enable_set = 0;
    config.nodelete = 0;
    config.mnum = 0;
    config.max_time = 5;
    
    snprintf(buff, 1023, "%s - %s , version %s\n\n", AUTH_CTL_LONG_NAME, AUTH_CTL_DESCRIPTION, AUTH_CTL_VERSION);
    
    optTitle(buff);
    
    optProgName(AUTH_CTL_NAME);
    
    optrega(&config.debug,      OPT_INT,                                'd', "debug"," debug level #");
    optrega(&config.bridge_id,  OPT_INT,                                'b', "bridge"," dpi bridge id");
    optrega(&enable,            OPT_INT,                                'e', "enable"," wifi auth switch");
    optrega(&addr,				OPT_STRING,                             'a', "addr", " ip address");
    optrega(&username,		    OPT_STRING,                             'u', "username", " username");
    optrega(&passwd,		    OPT_STRING,                             'p', "passwd", " password");
    optrega(&groupname,		    OPT_STRING,                             'g', "groupname", " groupname");
    optrega(&config.user_id,    OPT_INT,                                'i', "userid", " user id");
    optrega(&config.group_id,   OPT_INT,                                'G', "groupid", " group id");    
    optrega(&net,				OPT_STRING,                             'n', "net", " network address");
    optrega(&config.count,		OPT_FLAG,                               'c', "count", " show count");
    optrega(&config.login,      OPT_FLAG,                               'l', "login", " login process");
    optrega(&config.logout,     OPT_FLAG,                               'o', "logout", " logout process");
    optrega(&config.flush,      OPT_FLAG,                               'f', "flush",  " flush process");
    optrega(&config.show,       OPT_FLAG,                               's', "show",  " show information");    
    optrega(&config.add,        OPT_FLAG,                               'A', "add", " add user and group");
    optrega(&config.set,        OPT_FLAG,                               'S', "set", " set wifi config");
    optrega(&config.max_time,   OPT_INT,                                't', "maxtime", " set max time");
    optrega(&filename,          OPT_STRING,                             'F', "filename", " import user info from file");
    optrega(&config.authed,     OPT_FLAG,                               'U', "authed", " show authed user or group");
    optrega(&startip,		    OPT_STRING,                             0,   "startip", " start ip pool");
    optrega(&endip,		        OPT_STRING,                             0,   "endip", " end ip pool");
    optrega(&nodelete,          OPT_INT,                                'D', "delete", " timeout delete enable,default 0");
    optrega(&max_num,           OPT_INT,                                0,   "maxnum", " max account for each user,default 0");
	optrega(&version,           OPT_FLAG,                               'v', "version", " version info");
    optrega(&usage,             OPT_FLAG,                               'h', "help"," display usage info");
     
    opt(argc, argv);
   
    if (version)
    {
        printf("%s - %s , version %s\n", AUTH_CTL_LONG_NAME, AUTH_CTL_DESCRIPTION, AUTH_CTL_VERSION);
        return 1;
    }
    if (usage)
    {
        optPrintUsage();
        return 1;
    }

    if (optinvoked(&addr))
    {
        config.addr = addr;
    }
    if (optinvoked(&net))
    {
        config.net = net;
    }
    if (optinvoked(&username))
    {
        config.username = username;
    }
    if (optinvoked(&passwd))
    {
        config.passwd = passwd;
    }
    if (optinvoked(&groupname))
    {
        config.groupname = groupname;
    }    
    if (optinvoked(&startip))
    {
        config.startip = startip;
    } 
    if (optinvoked(&endip))
    {
        config.endip = endip;
    } 
    if (optinvoked(&config.user_id))
    {
        config.user_id_set = 1;
    }
    if (optinvoked(&config.group_id))
    {
        config.group_id_set = 1;
    }
    if (optinvoked(&config.max_time))
    {
        if(config.max_time == 0){
            config.max_time = 5;
        }
    }
    if (optinvoked(&nodelete))
    {
        config.nodelete = nodelete;
    }
    if (optinvoked(&max_num))
    {
        config.mnum = max_num;
    }
    if (optinvoked(&enable))
    {
        config.enable_set = 1;
        if(enable)
        {
            enable = 1;
        }
    }
	if (config.addr && config.net)
	{
        optPrintUsage();
        return 1;
	}

    if (config.enable_set)
    {
        opt_count++;
    }
    if (config.show)
    {
        opt_count++;
    }
    if (config.count)
    {
        opt_count++;
    }
    if (config.add)
    {
        opt_count++;
    }
    if (config.login)
    {
        opt_count++;
    }
    if (config.logout)
    {
        opt_count++;
    }
    if (config.flush)
    {
        opt_count++;
    }
    if (opt_count != 1)
    {
        optPrintUsage();
        return 1;
    }
    
#if 0    
    if(!config.flush && !config.net && !config.addr && !config.username && !config.user_id)
    {
        optPrintUsage();
        return 1;
    }
#endif    
	return 0;
}

int do_netlink_cmd(int type, struct user_auth_msg * request, struct user_auth_respond * respond, int respond_size)
{
	int   sock;
	struct sockaddr_nl remote;
	struct sockaddr_nl local;
	struct nlmsghdr * nlhdr;
	struct iovec iov;
	struct msghdr msg;
       int request_size = sizeof(struct user_auth_msg);
        
        sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_AUTH);
    
        if(sock < 0){
            perror("create socket error\n");
            return -1;
        }
    
        memset(&local, 0, sizeof(struct sockaddr_nl));
    
        local.nl_family = AF_NETLINK;
        local.nl_pid = getpid();
        local.nl_groups = 0;
    
        if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
            printf("bind socket error\n");
            close(sock);
            return -1;
        }
    
        memset(&remote, 0, sizeof(struct sockaddr_nl));
        remote.nl_family = AF_NETLINK;
        remote.nl_pid    = 0;
        remote.nl_groups = 0;
    
        memset(&msg, 0, sizeof(struct msghdr));
    

    
        nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(request_size));
        
        memcpy(NLMSG_DATA(nlhdr), (void *)request, request_size);
    
        nlhdr->nlmsg_len = NLMSG_LENGTH(request_size);
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
             perror("sendmsg error\n");
             free(nlhdr);
             return -1;
        }
    
        free(nlhdr);
    

    
        socklen_t remote_addr_len = sizeof(struct sockaddr_nl);
    
        int recv_len = recvfrom(sock, respond, respond_size,
                0, (struct sockaddr*)&remote, &remote_addr_len);
    
        if(recv_len == -1){
            perror("recvmsg error\n");
             return -1;
        }
        close(sock);
	return 0;
}

int do_enable(int bridge_id, int enable)
{
    uid_t uid, euid;
    char cmd[WIFIAUTH_CMD_LEN] = {0};
    FILE *pp = NULL;

    uid = getuid();
    euid = geteuid(); 
	setreuid(euid, uid);

    snprintf(cmd,sizeof(cmd),"echo \"%d\" > /proc/sys/net/dpifilter/dpi-user-auth",enable);
    pp = popen(cmd,"r");
    if(NULL == pp){
        return -1;
    }
    return 0;
}

int do_count(int bridge_id)
{
       
        struct user_auth_msg request;
        struct user_auth_respond respond;
		int ret;
        
        request.bridge_id = bridge_id;
        request.addr = 0xffffffff;

		ret = do_netlink_cmd(AUTH_GET_COUNT, &request, &respond, sizeof(struct user_auth_respond));
		if(ret != 0)
			return ret;
        
#if 0    
        printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
            respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
            sizeof(struct protocol_account));
#endif
        if(respond.nlhdr.nlmsg_type != AUTH_GET_COUNT_DONE)
        {
            return -1;
        }

		printf("max_auth_host_support: %u\n", respond.msg.result_entry.auth_host_max);
		printf("max_auth_user_support: %u\n", respond.msg.result_entry.auth_user_max);
		printf("max_auth_group_support: %u\n", respond.msg.result_entry.auth_group_max);

		printf("auth_user_count: %u\n", respond.msg.result_entry.auth_user_count);
		printf("auth_group_count: %u\n", respond.msg.result_entry.auth_group_count);

		printf("authed_host_count: %u\n", respond.msg.result_entry.authed_host_count);
		printf("authed_user_count: %u\n", respond.msg.result_entry.authed_user_count);
		printf("authed_group_count: %u\n", respond.msg.result_entry.authed_group_count);

        printf("[ip-pool] startip:"NIPQUAD_FMT"\n",NIPQUAD(respond.msg.result_entry.startip));
        printf("[ip-pool] endip:"NIPQUAD_FMT"\n",NIPQUAD(respond.msg.result_entry.endip));

        return 0;

}

static void print_user_info_head(void)
{
    printf("%-4s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n","id","username","password","groupname","addrcount","nodelete","max_authip","updatetime");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    return;
}

static void print_auth_info_head(void)
{
    printf("%-16s%-15s%-15s%-15s%-15s\n","Ip","username","groupname","logintime","updatetime");
    printf("-------------------------------------------------------------------------------\n");
}

static void print_group_info_head(void)
{
    printf("%-4s%-15s%-15s%-15s%-15s\n","id","groupname","user_count","no_delete","max_authip");
    printf("---------------------------------------------------------------\n");
}


void print_user_group_info(struct auth_info_entry * entry)
{
    if(entry->user_id > 0 && entry->username[0] != '\0' && entry->groupname[0] != '\0')
    {
        printf("%-4u%-15s%-15s%-15s%-15d%-15d%-15d%-15d\n", 
                entry->user_id,
                entry->username,
                entry->password,
                entry->groupname,
                entry->addr_count,
                entry->no_delete,
                entry->max_authip_num_per_user,
                entry->last_update);
    }
}

void print_user_info(struct auth_info_entry * entry)
{
    if(entry->user_id > 0 && entry->username[0] != '\0')
    {
        printf("%-4u%-15s%-15s%-15s%-15d%-15d\n", 
            entry->user_id , entry->username, entry->password, entry->groupname,entry->addr_count,entry->last_update);
    }
}


void print_group_info(struct auth_info_entry * entry)
{
    if(entry->group_id > 0 && entry->groupname[0] != '\0')
    {
        printf("%-4u%-15s%-15u%-15u%-15u\n",
            entry->group_id, entry->groupname, entry->user_count, entry->no_delete, entry->max_authip_num_per_user);
    }
}

void print_auth_info(struct auth_info_entry * entry)
{
    char ipaddr[16] = {0};
    snprintf(ipaddr,sizeof(ipaddr),"%u.%u.%u.%u",NIPQUAD(entry->addr));
    if(entry->user_id > 0 && entry->group_id > 0)
    {
        printf("%-16s%-15s%-15s%-15u%-15u\n", ipaddr, 
            entry->username, entry->groupname, entry->login_time, entry->last_update);
    }
}


int display_userinfo_by_name(int bridge_id ,char *username)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;

    request.bridge_id = bridge_id;
    request.addr = 0;
    strncpy((char *)request.ainfo.username, username, sizeof(request.ainfo.username));
        
    ret = do_netlink_cmd(AUTH_GET_USER_BY_NAME, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
	printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
		respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
		sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_USER_BY_NAME_DONE)
    {
        return -1;
    }

    print_user_info_head();
    print_user_info(&respond.msg.info_entry);
        
    return 0;
}

int display_groupinfo_by_name(int bridge_id ,char *groupname)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;

    request.bridge_id = bridge_id;
    request.addr = 0;
    strncpy((char *)request.groupname, groupname, sizeof(request.groupname));
        
    ret = do_netlink_cmd(AUTH_GET_GROUP_BY_NAME, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
                printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                    respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                    sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_GROUP_BY_NAME_DONE)
    {
        return -1;
    }

    print_group_info(&respond.msg.info_entry);
        
    return 0;
}


int display_userinfo_by_id(int bridge_id , int user_id)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;
                
    request.bridge_id = bridge_id;
//    request.user_id = user_id;
        
    ret = do_netlink_cmd(AUTH_GET_USER_BY_ID, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
                printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                    respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                    sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_USER_BY_ID_DONE)
    {
        return -1;
    }

    print_user_info(&respond.msg.info_entry);
        
    return 0;
}


int _display_userinfo_by_addr(int bridge_id , uint32_t addr)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;
                
    request.bridge_id = bridge_id;
    request.addr = addr;
        
    ret = do_netlink_cmd(AUTH_GET_USER_BY_ADDR, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
                printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                    respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                    sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_USER_BY_ADDR_DONE)
    {
        return -1;
    }

    print_auth_info_head();
    print_auth_info(&respond.msg.info_entry);
        
    return 0;
}

int display_userinfo_by_addr(int bridge_id, char * ipaddress)
{
	in_addr_t addr = inet_addr(ipaddress);

	if(addr == INADDR_NONE){
		perror("input address is invalid\n");
		return -1;
	}
    return _display_userinfo_by_addr(bridge_id, addr);
}



int do_login(int bridge_id , char * username, char *passwd, char * ipaddr ,int max_time)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;

	in_addr_t addr;

    if(ipaddr){
        addr = inet_addr(ipaddr);                
	    if(addr == INADDR_NONE){
		    perror("input address is invalid\n");
		    return -1;
	    }
    }
    else{
        addr = 0;
    }

    memset(&request,0,sizeof(request));
    request.bridge_id = bridge_id;
    request.addr = addr;

    if(addr){
        request.pc_discover_time = timestamp;
    }else{
        request.pc_discover_time = max_time;
    }
    strncpy((char *)request.ainfo.username, username, sizeof(request.ainfo.username));
    strncpy((char *)request.ainfo.passwd, passwd, sizeof(request.ainfo.passwd));
//	request.timestamp = (uint32_t)timestamp;
        
    ret = do_netlink_cmd(AUTH_LOGIN, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
                printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                    respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                    sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_LOGIN_DONE)
    {
        return -1;
    }

    ret = respond.msg.result_entry.result;
    printf("%d\n",ret);
    return ret;
}


int do_logout(int bridge_id , char * username, char * ipaddr)
{
        
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;

	in_addr_t addr = inet_addr(ipaddr);                
	if(addr == INADDR_NONE){
		perror("input address is invalid\n");
		return -1;
	}


    request.bridge_id = bridge_id;
    request.addr = addr;
    strncpy((char *)request.ainfo.username, username, sizeof(request.ainfo.username));
//	request.timestamp = (uint32_t)timestamp;

    ret = do_netlink_cmd(AUTH_LOGOUT, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
                printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                    respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                    sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_LOGOUT_DONE)
    {
        return -1;
    }

    ret = respond.msg.result_entry.result;
    return ret;
}

int do_flush(int bridge_id)
{
        struct user_auth_msg request;
        struct user_auth_respond respond;
        int ret;
    
        request.bridge_id = bridge_id;
//		request.timestamp = (uint32_t)timestamp;

        ret = do_netlink_cmd(AUTH_FLUSH, &request, &respond, sizeof(struct user_auth_respond));
        if(ret != 0)
            return ret;
                    
#if 0    
                    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                        sizeof(struct protocol_account));
#endif
        if(respond.nlhdr.nlmsg_type != AUTH_FLUSH_DONE)
        {
            return -1;
        }
    
        ret = respond.msg.result_entry.result;
        return ret;

}

int do_add_user(int bridge_id, char * username, char * passwd, char * groupname, int nodelete, int max_num, char *ipaddr)
{
    struct user_auth_msg request;
    struct auth_info_entry *entry;
    struct user_auth_respond respond;
    int ret;

    DEBUGP("%s:begin\n",__FUNCTION__);

    
    in_addr_t addr;
    
        if(ipaddr){
            addr = inet_addr(ipaddr);                
            if(addr == INADDR_NONE){
                perror("input address is invalid\n");
                return -1;
            }
        }
        else{
            addr = 0;
        }
    
    memset(&request, 0, sizeof(request));
    request.bridge_id = bridge_id;
    request.nodelete = nodelete;
    request.account_num = max_num;
    request.addr = addr;
    strncpy((char *)request.ainfo.username, username, sizeof(request.ainfo.username));
    strncpy((char *)request.ainfo.passwd, passwd, sizeof(request.ainfo.passwd));
    strncpy((char *)request.groupname, groupname, sizeof(request.groupname));

    DEBUGP("%s:process, bridgeid:%d,username:%s,passwd:%s,groupname:%s\n",__FUNCTION__,
                    request.bridge_id,
                    request.ainfo.username,
                    request.ainfo.passwd,
                    request.groupname);
            
    ret = do_netlink_cmd(AUTH_ADD_USER, &request, &respond, sizeof(struct user_auth_respond));
    DEBUGP("%s:process,ret:%d\n",__FUNCTION__,ret);
    if(ret != 0)
        return ret;
                    
#if 0    
                    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                        sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_ADD_USER_DONE)
    {
        return -1;
    }
    
    entry = &respond.msg.info_entry;
    
    printf("%s:%s\n",entry->username,entry->password);
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

int do_add_group(int bridge_id, char * groupname, int nodelete, int max_num)
{
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;

    memset(&request,0,sizeof(request));
    request.bridge_id = bridge_id;
    request.nodelete = nodelete;
    request.account_num = max_num;
    strncpy((char *)request.groupname, groupname, sizeof(request.groupname));
            
    ret = do_netlink_cmd(AUTH_ADD_GROUP, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                    
#if 0    
                    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                        sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_ADD_GROUP_DONE)
    {
        return -1;
    }
    
    printf("%u\n", respond.msg.result_entry.group_id);
    return 0;
}

int do_add_ip_pool(int bridge_id, char *startip, char *endip)
{
    struct user_auth_msg request;
    struct user_auth_respond respond;
    int ret;
    
    request.bridge_id = bridge_id;
    request.startip = inet_addr(startip);
    request.endip = inet_addr(endip);

    ret = do_netlink_cmd(AUTH_IPPOOL_SET, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;  

    if(respond.nlhdr.nlmsg_type != AUTH_ADD_GROUP_DONE)
    {
        return -1;
    }
    
    printf("%d\n", respond.msg.result_entry.result);
    
    return 0;
}

#if 0
int display_addrinfo_by_net(int bridge_id, uint32_t netaddr)
{
    int max = 256;
    int i;
    unsigned char           display_uname[(DPI_AUTH_USERNAME_MAX + 1) * 3];
    struct user_auth_msg request;
    struct user_auth_respond *respond = malloc(sizeof(struct nlmsghdr) + sizeof(struct user_auth_userinfo_entry) * max);
    struct user_auth_userinfo_entry * entry;
    int ret;
        
    request.bridge_id = bridge_id;
    request.addr = netaddr;
                
    ret = do_netlink_cmd(AUTH_GET_NET_IPLIST, &request, respond, sizeof(struct nlmsghdr) + sizeof(struct user_auth_userinfo_entry) * max);
    if(ret != 0)
        return ret;
                        
#if 0    
                        printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
                            respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
                            sizeof(struct protocol_account));
#endif
    if(respond->nlhdr.nlmsg_type != AUTH_GET_NET_IPLIST_DONE)
    {
        return -1;
    }
    
    max = (respond->nlhdr.nlmsg_len - sizeof(struct nlmsghdr))/sizeof(struct user_auth_userinfo_entry);
    entry = &respond->msg.user_info_entry;
    
    for( i = 0; i < max; i++)
    {
        if(entry->user_id > 0)
        {
			get_utf8_name(entry->username, display_uname);
            printf("%u.%u.%u.%u %u %s\n",
                NIPQUAD(entry->addr), entry->user_id, display_uname);
        }
        entry++;
    }
    return 0;

}
#endif

static int _display_authinfo_by_idrange(int bridge_id, uint32_t first_id, uint32_t last_id)
{
    struct user_auth_msg request;
    struct user_auth_respond *respond;
    struct auth_info_entry * entry;
    int count;
    int i;
    int ret;
    
    respond = malloc(sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_id - first_id));

    memset(&request,0,sizeof(request));
    request.bridge_id = bridge_id;
    request.first_id = first_id;
    request.last_id = last_id;
    
    ret = do_netlink_cmd(AUTH_GET_ADDR_LIST, &request, respond, sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_id - first_id));
    if(ret != 0)
        return ret;
                        
#if 0    
    printf(" Received message payload: %d, sizeof(struct auth_info_entry): %d\n", 
        respond->nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct auth_info_entry));
#endif
    if(respond->nlhdr.nlmsg_type == AUTH_NULL_INFO)
    {
        free(respond);
        return 0;
    }
    if(respond->nlhdr.nlmsg_type != AUTH_GET_ADDR_LIST_DONE)
    {
        free(respond);
        return -1;
    }
    count = (respond->nlhdr.nlmsg_len - sizeof(struct nlmsghdr))/sizeof(struct auth_info_entry);
    entry = &respond->msg.info_entry;

    for( i = 0; i < count; i++)
    {
        print_auth_info(entry);
        entry++;
    }
    free(respond);    
    return 0;


}

int display_all_addr(int bridge_id)
{

    int i;
    uint32_t max_addr;
    int ret;
    struct user_auth_msg request;
    struct user_auth_respond respond;
                
    request.bridge_id = bridge_id;
    request.addr = 0xffffffff;
        
    ret = do_netlink_cmd(AUTH_GET_COUNT, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
                
#if 0    
    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_COUNT_DONE)
    {
        return -1;
    }
    
    max_addr = respond.msg.result_entry.auth_host_max;

    print_auth_info_head();
    for( i = 1; i < max_addr; i += 255)
    {
        ret = _display_authinfo_by_idrange(bridge_id, i, i + 255);
        if(ret != 0)
        {
            return ret;
        }
    }

    return 0;

}

static int _display_groupinfo_by_idrange(int bridge_id, uint32_t first_group_id, uint32_t last_group_id, uint32_t authed)
{
    struct user_auth_msg request;
    struct user_auth_respond *respond;
    struct auth_info_entry * entry;
    int count;
    int i;
    int ret;

    respond = malloc(sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_group_id - first_group_id));

    request.bridge_id = bridge_id;
    request.first_id = first_group_id;
    request.last_id = last_group_id;
    request.authed = authed;
    
    ret = do_netlink_cmd(AUTH_GET_GROUP_LIST, &request, respond, sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_group_id - first_group_id));
    if(ret != 0)
        return ret;
                        
#if 0    
    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct protocol_account));
#endif
    if(respond->nlhdr.nlmsg_type == AUTH_NULL_INFO)
    {
        free(respond);
        return 0;
    }
    if(respond->nlhdr.nlmsg_type != AUTH_GET_GROUP_LIST_DONE)
    {
        free(respond);
        return -1;
    }
    count = (respond->nlhdr.nlmsg_len - sizeof(struct nlmsghdr))/sizeof(struct auth_info_entry);
    entry = &respond->msg.info_entry;

    for( i = 0; i <= count; i++)
    {
        print_group_info(entry);
        entry++;
    }
    free(respond);    

    return 0;
}

int display_all_group(int bridge_id, int authed)
{

    int i;
    uint32_t max_group;
    int ret;
    struct user_auth_msg request;
    struct user_auth_respond respond;
            
    request.bridge_id = bridge_id;
    request.addr = 0xffffffff;

    ret = do_netlink_cmd(AUTH_GET_COUNT, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
            
#if 0    
    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_COUNT_DONE)
    {
        return -1;
    }

    max_group = respond.msg.result_entry.auth_group_max;

    print_group_info_head();
    for( i = 0; i < max_group; i += 256)
    {
        ret = _display_groupinfo_by_idrange(bridge_id, i, i + 256, authed);
        if(ret != 0)
        {
            return ret;
        }
    }

    return 0;

}


static int _display_userinfo_by_idrange(int bridge_id, uint32_t first_user_id, uint32_t last_user_id, uint32_t authed)
{
    struct user_auth_msg request;
    struct user_auth_respond *respond;
    struct auth_info_entry * entry;
    int count;
    int i;
    int ret;
    
    respond = malloc(sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_user_id - first_user_id));

    request.bridge_id = bridge_id;
    request.first_id = first_user_id;
    request.last_id = last_user_id;
    request.authed = authed;
    
    ret = do_netlink_cmd(AUTH_GET_USER_LIST, &request, respond, sizeof(struct nlmsghdr) + sizeof(struct auth_info_entry) * (last_user_id - first_user_id));
    if(ret != 0)
        return ret;
                        
#if 0    
    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct protocol_account));
#endif
    if(respond->nlhdr.nlmsg_type == AUTH_NULL_INFO)
    {
        free(respond);
        return 0;
    }
    if(respond->nlhdr.nlmsg_type != AUTH_GET_USER_LIST_DONE)
    {
        free(respond);
        return -1;
    }
    count = (respond->nlhdr.nlmsg_len - sizeof(struct nlmsghdr))/sizeof(struct auth_info_entry);
    entry = &respond->msg.info_entry;

    for( i = 0; i < count; i++)
    {
        print_user_group_info(entry);
        entry++;
    }
    free(respond);    
    return 0;


}

int display_all_user(int bridge_id, int authed)
{

    int i;
    uint32_t max_user;
    int ret;
    struct user_auth_msg request;
    struct user_auth_respond respond;
            
    request.bridge_id = bridge_id;
    request.addr = 0xffffffff;
    
    ret = do_netlink_cmd(AUTH_GET_COUNT, &request, &respond, sizeof(struct user_auth_respond));
    if(ret != 0)
        return ret;
            
#if 0    
    printf(" Received message payload: %d, sizeof(struct protocol_account): %d\n", 
        respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr),
        sizeof(struct protocol_account));
#endif
    if(respond.nlhdr.nlmsg_type != AUTH_GET_COUNT_DONE)
    {
        return -1;
    }

    max_user = respond.msg.result_entry.auth_user_max;

    print_user_info_head();
    for( i = 0; i < max_user; i += 255)
    {
        ret = _display_userinfo_by_idrange(bridge_id, i, i + 255, authed);
        if(ret != 0)
        {
            return ret;
        }
    }

    return 0;

}



int main(int argc, char *argv[])
{

    int ret = parse(&argc, &argv);
    if(ret != 0)
        return ret;
	
	get_timestamp();

    if(config.enable_set)
    {
        return do_enable(config.bridge_id, enable);
    }
	if(config.count)
	{
		return do_count(config.bridge_id);
	}
    if(config.login && config.username && config.passwd)
    {
        return do_login(config.bridge_id, config.username, config.passwd, config.addr ,config.max_time);
    }
    if(config.logout && config.username && config.addr)
    {
        return do_logout(config.bridge_id, config.username, config.addr);
    }
    if(config.flush)
    {
        return do_flush(config.bridge_id);
    }
	if(config.add && config.username && config.passwd && config.groupname)
	{
		return do_add_user(config.bridge_id, config.username, config.passwd, config.groupname, config.nodelete, config.mnum, config.addr);
	}
    if(config.add && config.groupname)
    {
        return do_add_group(config.bridge_id, config.groupname, config.nodelete, config.mnum);
    }
    if(config.add && config.startip && config.endip)
    {
        return do_add_ip_pool(config.bridge_id, config.startip, config.endip);
    }
    if(config.show)
    {
        if(config.username)
        {
            return display_userinfo_by_name(config.bridge_id, config.username);
        }
        if(config.groupname)
        {
            return display_groupinfo_by_name(config.bridge_id, config.groupname);
        }
        if(config.addr)
        {
            if(strcmp(config.addr, "0") == 0 || strcmp(config.addr, "0.0.0.0") == 0)
            {
                return display_all_addr(config.bridge_id);
            }
            else
            {
                return display_userinfo_by_addr(config.bridge_id, config.addr);
            }
        }
        if(config.user_id_set)
        {
            if(config.user_id == 0)
            {
                return display_all_user(config.bridge_id, config.authed);
            }
            else
            {
                return display_userinfo_by_id(config.bridge_id, config.user_id);
            }
        }
        if(config.group_id_set)
        {
            if(config.group_id == 0)
            {
                return display_all_group(config.bridge_id, config.authed);
            }
        }
        optPrintUsage();
        return 0;
    }
    if(config.set){
        
    }

	optPrintUsage();

	return 0;
}

