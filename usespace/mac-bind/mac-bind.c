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

#include <unistd.h>

#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/utsname.h>

#include "./mac-bind.h"
#include "./mgtcgi_xmlconfig.h"
#include "./route_xmlwrapper4c.h"


int show_status = 0;
int first = 0;
int print_count = 0;
int total = 0;
int page = 0;
int pagesize = 0; 
int learn_action = 0;
struct mac_bind_config_info mac_bind_config;

/**
* Description:  mac_bind打印头信息函数
* @param        no
* @return        no
*/
void print_show_head(void)
{

    fprintf(stdout, "%5s%10s%14s%21s%32s\n", "ACTION","Bridge","Ip_Addr","Mac_Addr","COMMENT"); 
    fprintf(stdout, "--------------------------------------------------------------------------------------------------------\n");
    return;
}

/**
* Description:  mac_bind获得最大bridge num
* @param        no
* @return        no
*/
int get_bridge_num()
{
#if 0
    char buf[100];
    FILE *fd = NULL;
    int  bridge_num = 0;

    if((fd = fopen("/etc/version", "r")) == NULL){
        return 0;
    }
    while(fgets(buf, sizeof(buf), fd))
    {
        if(sscanf(buf, "BRIDGE_NUM=\"%d\"", &bridge_num)){
            break;
        }
    }
#endif
    return 2;
}

#if 0
int get_mac_bind_learnt(void)
{
    int size,learnt = 0;
    struct mgtcgi_xml_mac_binds *node = NULL;
    get_route_xml_node(MAC_BIND_FILE, MGTCGI_TYPE_MAC_BINDS, (void**)&node, &size);
    if (NULL == node){
	    goto FREE_EXIT;
    }
    learnt = node->learnt;

FREE_EXIT:
    if(node){
         free_route_xml_node((void*)&node);
    }
    return learnt;
}
#endif

/**
* Description:  mac_bind显示与处理ip地址函数
* @param        addr                输入     ip
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
char * addr_to_dotted(const struct in_addr *addrp)
{
	static char buf[20];
	const unsigned char *bytep;

	bytep = (const unsigned char *) &(addrp->s_addr);
	sprintf(buf, "%d.%d.%d.%d", bytep[0], bytep[1], bytep[2], bytep[3]);
	
	return buf;
}

/**
* Description:  mac_bind显示处理ip地址函数
* @param        ip_addr                输入     ip
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
static char *print_ip(u_int32_t ip_addr)
{
	u_int32_t left = ip_addr;
	static char buf[BUFSIZ];

	sprintf(buf, "%s", addr_to_dotted((struct in_addr *) &left));

	return buf;

}

/**
* Description:  mac_bind 模块mac地址处理函数
* @param        mac_addr                输入     存储mac
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int parse_mac(uint8_t *mac_addr)
{
	int  i;
	int  mac = 0;
	int  ret   = DEV_OK;

    sscanf(mac_bind_config.mac_addr,"%X:%X:%X:%X:%X:%X",
	(unsigned int*)(mac_addr),(unsigned int*)(mac_addr+1),(unsigned int*)(mac_addr+2),
	(unsigned int*)(mac_addr+3),(unsigned int*)(mac_addr+4),(unsigned int*)(mac_addr+5));
	   
	for(i=0;i<6;i++){
		mac = mac_addr[i];
		if((mac<MAC_ADDR_MIN)&&(mac>MAC_ADDR_MAX)){
			fprintf(stderr, "Mac addr error.\n");
			return DEV_ERR;
		}
		mac = 0;
	}
	
	return ret;
}

/**
* Description:  mac_bind 模块ip地址处理函数
* @param        s                输入     ip字符串
* @param        min            输入     ip数字最小值
* @param        max           输入     ip数字最大值
* @param        ret             输入     ip转换数字
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int string_to_number_ll(const char *s, unsigned long long min,
		    unsigned long long max, unsigned long long *ret)
{
	char *end;
	unsigned long long number;
	
	errno = 0;
	number = strtoull(s, &end, 0);
	if (*end == '\0' && end != s) {
		if (errno != ERANGE && min <= number &&(!max || number <= max)) {
			*ret = number;
			return DEV_OK;
		}
	}
	
	return DEV_ERR;
}

/**
* Description:  mac_bind 模块ip地址处理函数
* @param        s                输入     ip字符串
* @param        min            输入     ip数字最小值
* @param        max           输入     ip数字最大值
* @param        ret             输入     ip转换数字
* @return        返回result
*/
int string_to_number_l(const char *s, unsigned long min, 
			unsigned long max,unsigned long *ret)
{
	int result;
	unsigned long long number;

	result = string_to_number_ll(s, min, max, &number);
	*ret = (unsigned long)number;

	return result;
}

/**
* Description:  mac_bind 模块ip地址处理函数
* @param        s                输入     ip字符串
* @param        min            输入     ip数字最小值
* @param        max           输入     ip数字最大值
* @param        ret             输入     ip转换数字
* @return        返回result
*/
int string_to_number(const char *s, unsigned int min, unsigned int max,unsigned int *ret)
{
	int result;
	unsigned long number;

	result = string_to_number_l(s, min, max, &number);
	*ret = (unsigned int)number;

	return result;
}

/**
* Description:  mac_bind 模块ip地址处理函数
* @param        dotted                输入     ip字符串
* @return        成功返回ip地址，失败返回NULL
*/
struct in_addr *dotted_to_addr(const char *dotted)
{
	static struct in_addr addr;
	unsigned char *addrp;
	char *p, *q;
	unsigned int onebyte;
	int i;
	char buf[20];

	strncpy(buf, dotted, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	addrp = (unsigned char *) &(addr.s_addr);
	
	p = buf;
	for (i = 0; i < 3; i++) {
		if ((q = strchr(p, '.')) == NULL)
			return (struct in_addr *) NULL;

		*q = '\0';
		if (string_to_number(p, 0, 255, &onebyte) == -1)
			return (struct in_addr *) NULL;

		addrp[i] = (unsigned char) onebyte;
		p = q + 1;
	}

	if (string_to_number(p, 0, 255, &onebyte) == -1)
		return (struct in_addr *) NULL;

	addrp[3] = (unsigned char) onebyte;

	return &addr;
}

/**
* Description:  mac_bind 模块ip地址处理函数
* @param        name                输入     ip字符串
* @return        成功返回ip地址，失败返回NULL
*/
static struct in_addr *network_to_addr(const char *name)
{
	struct netent *net;
	static struct in_addr addr;

	if ((net = getnetbyname(name)) != NULL) {
		if (net->n_addrtype != AF_INET)
			return (struct in_addr *) NULL;
		addr.s_addr = htonl((unsigned long) net->n_net);
		return &addr;
	}

	return (struct in_addr *) NULL;
}

/**
* Description:  mac_bind解析ip地址函数
* @param        ip_addr                输入     存储ip
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int parse_hostnetwork(uint32_t *ip_addr)
{
	struct in_addr *addrptmp;

	if ((addrptmp = dotted_to_addr(mac_bind_config.ip_addr)) != NULL ||
	    (addrptmp = network_to_addr(mac_bind_config.ip_addr)) != NULL) {
		*ip_addr = addrptmp->s_addr;
		return DEV_OK;
	}

	fprintf(stderr, "host/network `%s' not found\n", mac_bind_config.ip_addr);
	
	return DEV_ERR;
}

/**
* Description:  mac_bind 显示ACTION状态函数
* @param        respond               内核返回信源
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int show_mac_bind_action_status(struct mac_bind_respond_info *respond)
{
	struct mac_bind_msg *msg = respond->info;
	
	fprintf(stdout,"Mac-Bing Action Status:  ");
	
	if(msg->action){
		fprintf(stdout,"Bridge: %u , Action: %s.\n",msg->bridge_id,"NOT_MATCH_ACCEPT");
	}else{
		fprintf(stdout,"Bridge: %u , Action: %s.\n",msg->bridge_id,"NOT_MATCH_DROP");
	}
	
	return DEV_OK;
}


/**
* Description:  mac_bind 显示添加规则函数
* @param 　 show_status　    全局变量输入  
* @param        respond               内核返回信源
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int show_mac_bind_info_send(struct mac_bind_respond_info *respond)
{
    struct mac_bind_msg     *msg = respond->info;
    //char				    ip[STRING_LENGTH] = {0};
    int 					j = 0;
    int 					index_start;

    index_start = (page - 1) * pagesize;
    if(show_status){
        show_status = 0;
        printf("{\"data\":[");
	}

	while((msg[j].ip_addr != 0)&&(j < SHOW_MAX_NODES)){
        //strcpy(ip,print_ip(msg[j].ip_addr));

        total++;
        if((total > index_start) && (print_count < pagesize)){ 
            if(first)
            {
                printf(",");
            }
            first = 1;

            printf("{\"action\":\"%d\",\"bridge\":\"%u\",\"ip\":\"%s\",\"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"comment\":\"\"}", 
            msg[j].action,msg[j].bridge_id,print_ip(msg[j].ip_addr),msg[j].mac_addr[0], msg[j].mac_addr[1], 
            msg[j].mac_addr[2],msg[j].mac_addr[3], msg[j].mac_addr[4], msg[j].mac_addr[5]);
            print_count++;
        }

        j++;
        //memset(ip,0,sizeof(ip));
	}
    
	return DEV_OK;
}


/**
* Description:  mac_bind 显示添加规则函数
* @param 　 show_status　    全局变量输入  
* @param        respond               内核返回信源
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int show_mac_bind_info(struct mac_bind_respond_info *respond)
{
	struct mac_bind_msg 			*msg = respond->info;
	//char							ip[STRING_LENGTH] = {0};
	int							j = 0;
	
	if(show_status){
		show_status = 0;
		print_show_head();
	}

	while((msg[j].ip_addr!= 0)&&(j <SHOW_MAX_NODES)){
		//strcpy(ip,print_ip(msg[j].ip_addr));
        
		printf("%5d %5u%23s        %02X:%02X:%02X:%02X:%02X:%02X  \n", 
		       msg[j].action,msg[j].bridge_id,print_ip(msg[j].ip_addr),msg[j].mac_addr[0], msg[j].mac_addr[1], 
			msg[j].mac_addr[2],msg[j].mac_addr[3], msg[j].mac_addr[4], msg[j].mac_addr[5]);
		
		j++;
		//memset(ip,0,sizeof(ip));
	}

	return DEV_OK;
}


/**
* Description:  mac_bind 内核通信函数
* @param 　 mac_bind_config　    全局变量输入 
* @param       request                    用户发送信源
* @param       type                        动作类型
* @return       成功返回DEV_OK，失败返回DEV_ERR
*/
int communicate_kernel(struct mac_bind_msg *request,int type)
{
	int      sock;
	int      recv_len;
	int      respond_size;
	void   *respond;
	struct msghdr msg;
	struct iovec iov;
	struct sockaddr_nl remote;
	struct sockaddr_nl local;
	struct nlmsghdr * nlhdr;
	struct nlmsghdr * rnlhdr=NULL;
	socklen_t remote_addr_len;

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_IP_MAC); 

	if(sock < 0){
		fprintf(stderr,"create socket error\n");
		return DEV_ERR;
	}

	memset(&local, 0, sizeof(struct sockaddr_nl));

	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0){
		fprintf(stderr,"bind socket error\n");
		close(sock);
		return DEV_ERR;
	}

	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct mac_bind_msg)));

	memcpy(NLMSG_DATA(nlhdr), (char *)request, sizeof(struct mac_bind_msg));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct mac_bind_msg));
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
	if (ret == DEV_ERR) {
		 fprintf(stderr,"sendmsg error\n");
		 goto EXIT;
	}

	remote_addr_len = sizeof(struct sockaddr_nl);
	respond_size       = sizeof(struct mac_bind_respond_info);
	respond              = malloc(respond_size);

RECV:
	memset(respond, 0, respond_size);

	recv_len = recvfrom(sock, respond, respond_size,
	       0, (struct sockaddr*)&remote, &remote_addr_len);

	if(recv_len == -1){
		fprintf(stderr, "recvmsg error\n");
		goto EXIT;
	}

	rnlhdr = (struct nlmsghdr *)respond;

	switch(rnlhdr->nlmsg_type){
		case MAC_BINDING_DONE:
			break;
		case MAC_BINDING_SHOW_DONE:
            if(mac_bind_config.cgishow)
                show_mac_bind_info_send((struct mac_bind_respond_info*)respond);
            else
                show_mac_bind_info((struct mac_bind_respond_info*)respond);
			goto RECV;
			break;

		case MAC_BINDING_GET_DONE:
            
            if(show_status && mac_bind_config.cgishow){
                printf("{\"data\":[");
            }
            if(mac_bind_config.cgishow){
                struct mac_bind_msg *msg = ((struct mac_bind_respond_info*)respond)->info;
                learn_action = msg->action;
                printf("],\"learnt\":%d,\"total\":%d,\"iserror\":0,\"msg\":\"\"}",learn_action,total);
            }
            else 
			    show_mac_bind_action_status((struct mac_bind_respond_info*)respond);
			goto RECV;
			break;
					
		case MAC_BINDING_ERROR:
			fprintf(stderr, "mac_bind command error\n");
			break;
		
		case MAC_BINDING_MEMERR:
			fprintf(stderr, "mac_bind malloc memery error\n");
			break;
			
		default:
			fprintf(stderr, "mac_bind error, error type : %u\n", rnlhdr->nlmsg_type);
			break;
	}

EXIT:
	close(sock);
	if(respond)
		free(respond);
	if(nlhdr)
		free(nlhdr);

	return  DEV_OK;
}

/**
* Description:  mac_bind 检查动作函数
* @param 　 mac_bind_config　    全局变量输入  
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int inline check_action()
{
    if((mac_bind_config.add + mac_bind_config.del + mac_bind_config.flush+ mac_bind_config.help+
	mac_bind_config.show +mac_bind_config.set+ mac_bind_config.get + mac_bind_config.cgishow) != 1){
		fprintf(stderr, "action error! maybe multi-action or none-action!\n");    
		return DEV_ERR;
    }

	return DEV_OK;
}

/**
* Description:  mac_bind 检查参数函数
* @param 　 mac_bind_config　    全局变量输入
* @param        type                       输入             执行规则类型     
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int inline check_parameter(int type)
{
	int bridge_num  = 0;
	
	if(type == MAC_BINDING_DELETE){
		goto CHECK_EXCEPT_NAME;
	}
#if 0	
	if((mac_bind_config.name == NULL) || (strlen(mac_bind_config.name)>NAME_MAX_LEN)){
		fprintf(stderr, "name parameter error.\n"); 
		return DEV_ERR;
	}
#endif
CHECK_EXCEPT_NAME:	

	bridge_num = get_bridge_num();
	if(mac_bind_config.bridge > bridge_num){
		fprintf(stderr, "bridge parameter error.\n"); 
		return DEV_ERR;
	}

	if((mac_bind_config.ip_addr == NULL) || 
	(strlen(mac_bind_config.ip_addr)<IP_MAC_ADDR_MIN_LEN)){
		fprintf(stderr, "ip parameter error.\n");   
		return DEV_ERR;
	}

	if((mac_bind_config.mac_addr == NULL) || (strchr(mac_bind_config.mac_addr, ':') == NULL) ||
	(strlen(mac_bind_config.mac_addr)<IP_MAC_ADDR_MIN_LEN)){
		fprintf(stderr, "mac parameter error.\n");   
		return DEV_ERR;
	}

	return DEV_OK;
}
#if 0
struct mac_bind_info
{
    char  ip_addr[16];
    char  mac_addr[18];
    //uint16_t  bridge_id;
    //char       name[NAME_MAX_LEN];
    uint8_t   action;
};

static char * mac_bind_get_ipaddr(char * buf,char *ip_addr)
{
    
    char *ipinfo=NULL;
    ipinfo = strchr(buf, ' ');
    if(!ipinfo)
    {
        return NULL;
    }
    memcpy(ip_addr,buf,ipinfo-buf);
    ipinfo++;
    return ipinfo;    
}

static char * mac_bind_get_macaddr(char * buf,char *mac_addr)
{
    
    char *info=NULL;
    if(memcmp(buf,"lladdr ",strlen("lladdr "))!=0)
    {
        return NULL;
    }    
    buf+=strlen("lladdr ");
    info = strchr(buf, ' ');
    if(!info)
    {
        return NULL;
    }
    memcpy(mac_addr,buf,info-buf);   
    info++;
    return info;    
}

static char * mac_bind_get_action(char *buf, uint8_t *action)
{
    
    //char *info=NULL;
    if(memcmp(buf,"PERMANENT",strlen("PERMANENT"))==0)
    {
        *action = 1;
    }
    else
        *action = 0;
    return NULL;    
}


int mac_bind_show()
{
	char 		buf[128];
    char *pbuf;
	FILE 		*pp;
	char cmd[128] = {0};
    struct mac_bind_info macinfo;
    
	snprintf(cmd, sizeof(cmd), "/usr/local/sbin/ip neigh show dev LAN");
	pp = popen(cmd, "r");
    if(pp==NULL)
        return 0;
    while(fgets(buf, sizeof(buf), pp)){
        memset(&macinfo,0,sizeof(struct mac_bind_info));
        pbuf = mac_bind_get_ipaddr(buf,macinfo.ip_addr);
        if(!pbuf)
        {
            continue;
        }
        pbuf = mac_bind_get_macaddr(pbuf,macinfo.mac_addr);
        if(!pbuf){
            continue;
        }
        mac_bind_get_action(pbuf,&macinfo.action);
        printf("%d %s %s\n",macinfo.action,macinfo.ip_addr,macinfo.mac_addr);
    }
	pclose(pp);
	return 0;	
}
#endif

/**
* Description:  mac_bind 执行规则函数
* @param 　 mac_bind_config　    全局变量输入  
* @param 　 tyep                           执行类型　   
* @return        成功返回DEV_OK，失败返回DEV_ERR
*/
int mac_bind_execute_rule(int type)
{
	int  ret  = DEV_ERR;
	uint8_t   mac_addr[12];
	uint32_t ip_addr = 0;
	struct mac_bind_msg request;

	memset(mac_addr,0,(sizeof(uint8_t)*12));
	memset(&request, 0, sizeof(struct mac_bind_msg));	
#if 0
    if(type == MAC_BINDING_SHOW)
    {
        mac_bind_show();
        return DEV_OK;
    }
#endif
	if(type == MAC_BINDING_ACTION){
		if(mac_bind_config.action== NULL){
			fprintf(stderr, "action parameter error.\n");  
			return ret;
		}

		if(!strcmp(mac_bind_config.action,"drop")){
			request.action = NOT_MATCH_DROP;
		}
		else if(!strcmp(mac_bind_config.action,"accept")){
			request.action = NOT_MATCH_ACCEPT;
		}
		else{
			fprintf(stderr, "action parameter error.\n");  
			return ret;
		}

		goto start_action;
	}
    
	if((type > MAC_BINDING_DELETE)){
		goto start_except_add_or_del;
	}

	if(check_parameter(type) != DEV_OK){
		return ret;
	}

	if(parse_hostnetwork(&ip_addr) != DEV_OK){
		return ret;
	}

	if(parse_mac(mac_addr) != DEV_OK){
		return ret;
	}
#if 0	
	if(type == MAC_BINDING_ADD){
		memcpy(request.name,mac_bind_config.name,strlen(mac_bind_config.name));
	}
#endif
    if(mac_bind_config.action!= NULL && !strcmp(mac_bind_config.action,"drop"))
    {
        request.action = MAC_BAND_FLAG;
    }

    request.ip_addr = ip_addr;
	memcpy(request.mac_addr,mac_addr,sizeof(request.mac_addr));
	
start_except_add_or_del:
start_action:
	
	request.bridge_id =  mac_bind_config.bridge;
	ret = communicate_kernel(&request,type);

	return ret;
}

/**
* Description:  mac_bind 命令行解析函数
* @param 　argc　      输入   命令行字符个数
* @param 　argv　      输入   命令行字符
* @return       成功返回DEV_OK，失败返回DEV_ERR
*/
static int mac_bind_command_parse(int *argc, char ***argv)
{
	char buff[128];

	memset(&mac_bind_config, 0, sizeof(struct mac_bind_config_info));

    mac_bind_config.bridge = 0;
    
	snprintf(buff, 1023, "%s - %s ,version %s\n\n", 
	IP_MAC_LONG_NAME,IP_MAC_DESCRIPTION, IP_MAC_VERSION);
	optTitle(buff);
	optProgName(IP_MAC_NAME);
	
	optrega(&mac_bind_config.bridge,          	OPT_INT,                'b', "bridge"  ,  "bridge ID");
       optrega(&mac_bind_config.page,          	OPT_INT,                'p', "page"  ,  "page");
       optrega(&mac_bind_config.page_size,          	OPT_INT,                'z', "pagesize"  ,  "pagesize");
	optrega(&mac_bind_config.name,          	OPT_STRING,             'n', "name"   , "name");
	optrega(&mac_bind_config.ip_addr,       	OPT_STRING,             0 , "ip"         , "ip addr");
	optrega(&mac_bind_config.mac_addr,    	    OPT_STRING,             0 , "mac"      , "mac addr");
	optrega(&mac_bind_config.action,          	OPT_STRING,             0 ,  "action"  , "action paramet");
	optrega(&mac_bind_config.add,            	OPT_FLAG,               'a', "add"      , "add policy");
	optrega(&mac_bind_config.del,              	OPT_FLAG,               'd', "del"       , "del  policy");
	optrega(&mac_bind_config.flush,            	OPT_FLAG,               'f', "flush"     ,  "flush policy");
	optrega(&mac_bind_config.change,        	OPT_FLAG,               'c', "change" , "change policy");
	optrega(&mac_bind_config.show,           	OPT_FLAG,               's', "show"    , "show policy");
       optrega(&mac_bind_config.cgishow,           OPT_FLAG,              'w', "webshow"    , "web show policy");
    
	optrega(&mac_bind_config.set,          	    OPT_FLAG,               'S' ,"set"      , "set action status");
	optrega(&mac_bind_config.get,          	    OPT_FLAG,               'g', "get"      ,  "get action status");
	optrega(&mac_bind_config.help,          	OPT_FLAG,               'h', "help"     , "usage");

	opt(argc, argv); 

	return DEV_OK;
}

/**
* Description:  mac_bind 主函数
* @param 　argc　      输入   命令行字符个数
* @param 　argv　      输入   命令行字符
* @return       成功返回DEV_OK，失败返回DEV_ERR
*/
int main(int argc, char *argv[])
{
	int ret = DEV_ERR; 

	if(mac_bind_command_parse(&argc, &argv) != DEV_OK){
		return ret;
	}

	if(check_action() != DEV_OK){
		return ret;
	}

	if(mac_bind_config.add){
		return mac_bind_execute_rule(MAC_BINDING_ADD);
	}
	if(mac_bind_config.del){
		return mac_bind_execute_rule(MAC_BINDING_DELETE);
	}
	if(mac_bind_config.flush){
		return mac_bind_execute_rule(MAC_BINDING_FLUSH);
	}
	if(mac_bind_config.show){
		show_status=1;
		return mac_bind_execute_rule(MAC_BINDING_SHOW);
	}
    if(mac_bind_config.cgishow){
        show_status=1;
        if(mac_bind_config.page && mac_bind_config.page_size){
            page = mac_bind_config.page;
            pagesize = mac_bind_config.page_size;
            ret = mac_bind_execute_rule(MAC_BINDING_SHOW);
            mac_bind_execute_rule(MAC_BINDING_GET_ACTION);
        }else{
            printf("{\"data\":[],\"iserror\":0,\"msg\":\"Parameter error\"}");
        }	
    }
	if(mac_bind_config.action){
		return mac_bind_execute_rule(MAC_BINDING_ACTION);
	}
	if(mac_bind_config.get){
		return mac_bind_execute_rule(MAC_BINDING_GET_ACTION);
	}
	if(mac_bind_config.help){
		optPrintUsage();		 
	}

	return ret;	
}

