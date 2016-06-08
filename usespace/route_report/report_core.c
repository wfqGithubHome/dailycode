#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <time.h>
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

#include "xmlwrapper4c.h"
#include "route_xmlwrapper4c.h"
#include "mgtcgi_xmlconfig.h"

#include "include/report_core.h"
#include "include/log_report.h"
#include "include/md5.h"
#include "include/curl.h"
#include "include/common.h"
#include "include/ac_json.h"

#if 0
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

struct report_config_info report_config;
int rtime = 0;

static struct route_proto_speed_info temp[ROUTE_REPORT_PORTO_NUM] = {
    {
        .proto_name = "total",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Game",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Websp",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "P2Psp",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "P2P",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Web",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Chat",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "NetworkIST",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "FileTransfer",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Security",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Database",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Mail",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Stock",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Terminals",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Voip",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Rfc",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "GamesMaintain",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Bypass",
        .rx_bytes = 0,
        .tx_bytes = 0,
    },
    {
        .proto_name = "Others",
        .rx_bytes = 0,
        .tx_bytes = 0,
    }
};

void delay(int i)
{
    int j;
    for(j = 0; j < i; j++)
    {
        usleep(1000000);                 
    }    
}

void get_timestamp(time_t *timestamp)
{
    time_t tmp;
	tmp = time(NULL);

    *timestamp = tmp - 8*60*60;
}

void set_reuid()
{
	uid_t uid ,euid; 
	uid = getuid(); 
	euid = geteuid(); 
	setreuid(euid, uid);
}

void free_json_report(json_val_t **elemt)
{
    int         i;
    json_val_t  *tmp = *elemt;
    struct json_val_elem *e = NULL;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    if(elemt == NULL){
        return;
    }

    if(tmp == NULL){
        return;
    }

    DEBUGP("%s : tmp->type: %d\n",__FUNCTION__,tmp->type);

    for(i = 0; i < tmp->length; i++){
        e = tmp->u.object[i];
        if(NULL == e){
            continue;
        }

        if(e->key){
            DEBUGP("%s, e->key:%s\n",__FUNCTION__,e->key);
            free(e->key);
            e->key = NULL;
        }

        if(e->val){
            if(e->val->u.data){
                DEBUGP("%s, e->val->u.data:%s\n",__FUNCTION__,e->val->u.data);
                free(e->val->u.data);
                e->val->u.data = NULL;
            }

            DEBUGP("%s : e->val->type: %d\n",__FUNCTION__,e->val->type);

            free(e->val);
            e->val = NULL;
        }

        free(e);
        e = NULL;
    }

    if(tmp->u.object){
        free(tmp->u.object);
        tmp->u.object = NULL;
    }

    free(tmp);
    tmp = NULL;

    DEBUGP("%s: finish\n", __FUNCTION__);
}

static void md5sum_build(const char *src, char *md5sum)
{
	struct MD5Context md5c; 
	char tmp[3]={'\0'};  
	unsigned char ss[16]; 
	int i=0;
    
    DEBUGP("%s:begin\n", __FUNCTION__);
	
 	MD5Init( &md5c );   
     	MD5Update( &md5c, src,  strlen(src));
		
     	MD5Final( ss, &md5c );   
  
     	for( i=0; i<16; i++ ){   
    		sprintf(tmp,"%02X", ss[i] );   
    		strcat(md5sum,tmp);   
     	}	   
     
    DEBUGP("%s: finish, src:%s,src_length:%d,md5sum: %s\n", __FUNCTION__,src,strlen(src), md5sum);
}

static int parse_json_report_interval_time( char * recv, int datalen ,uint32_t *interval_time)
{
    uint32_t                time;
    int                     ret = -1, res, i;
    int                     status,result;
    json_config             config;
    json_val_t              *element = NULL;

    DEBUGP("%s: begin, recv: %s\n", __FUNCTION__, recv);
    if(NULL == recv || datalen < 2)
        goto EXIT;

    memset(&config, 0, sizeof(json_config));
    config.max_nesting = 0;
    config.max_data = 0;
    config.allow_c_comments = 1;
    config.allow_yaml_comments = 1;

    res = do_tree(&config, recv, datalen, &element);
    if(res != 0 || element == NULL){
        DEBUGP("%s: bulid dom error\n", __FUNCTION__);
        goto EXIT;
    }

    for(i = 0; i < element->length; i++){
        if( strcmp(element->u.object[i]->key, "status") == 0 ){
            status = atoi(element->u.object[i]->val->u.data);
        }
        if( strcmp(element->u.object[i]->key, "result") == 0 ){
            result = atoi(element->u.object[i]->val->u.data);
        }
    }

    DEBUGP("%s: status: %d, result: %d\n", __FUNCTION__, status, result);

    if(status != 0)
        goto EXIT;

    time = result;
    if(time < DEFAULT_REPORT_INTERVAL_MIN_TIME){
        time = DEFAULT_REPORT_INTERVAL_MIN_TIME;
    }else if(time > DEFAULT_REPORT_INTERVAL_MAX_TIME){
        time = DEFAULT_REPORT_INTERVAL_MAX_TIME;
    }

    *interval_time = time;
    ret = 0;
EXIT:

    free_json_report(&element);
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

void get_vlan_iface(const char *ifname, char *iface)
{
	char    cmd[CMD_LENGTH] = {0};
    char    strbuf[CMD_LENGTH]={0};
    FILE    *pp = NULL;
    const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

    set_reuid();
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list vlan %s |awk -F ',' '{print $NF}'", script_path, ifname);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, sizeof(strbuf), pp);
		strbuf[strlen(strbuf)-1] = 0;
	}
	pclose(pp);

    strcpy(iface,strbuf);
    return;
}

void get_adslclient_link(const char *name, char *status, char *ip, char *gateway, char *uptime)
{
	int i=0;
	char *dot=",";
	char *ptr;
	char strbuf[CMD_LENGTH]={0};
	char cmd[CMD_LENGTH]={0};
	FILE *pp = NULL;
    const char *script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

    set_reuid();
	snprintf(cmd, sizeof(cmd), "%s/route_script.sh list adsl %s", script_path, name);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(strbuf, sizeof(strbuf), pp);
		strbuf[strlen(strbuf)-1] = 0;
	}
	pclose(pp);

	ptr=strtok(strbuf, dot);
	if (NULL != ptr){
		strcpy(status, ptr);
		i++;
	}
	while((ptr=strtok(NULL,dot))){
		if (NULL != ptr){
			if (i == 1){
				strcpy(ip, ptr);
			}
			else if (i == 2){
				strcpy(gateway, ptr);
			} 
			else if (i == 3){
				strcpy(uptime, ptr);
			}
			i++;
		} else {
			break;
		}
	}

}

void struct_init(struct route_proto_speed_info **tmp)
{
    char *p = NULL;

    if(*tmp != NULL){
        return;
    }

    p = (char *)malloc(ROUTE_REPORT_PORTO_NUM * sizeof(struct route_proto_speed_info));
    if(p == NULL){
        return;
    }

    *tmp = (struct route_proto_speed_info *)p;
    return;
}

void struct_free(struct route_proto_speed_info **tmp)
{
    if(*tmp == NULL){
        return;
    }

    free(*tmp);
    *tmp = NULL;
    return;
}


static int device_hwid_read(unsigned char *hwid)
{
    int ret = -1;
    FILE *fd = NULL;
    char buf[DEVICE_ID_LENGTH];

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    memset(buf, 0, DEVICE_ID_LENGTH); 
    fd = fopen("/etc/HWID", "r");
    if (fd == NULL)
    {
        goto exit;
    }

    if (fgets(buf, DEVICE_ID_LENGTH, fd))
    {
        strncpy((char *)hwid, buf, 32);
        ret = 0;
    }
    
exit:
    if (fd)
    {
        fclose(fd);
    }

    DEBUGP("%s: finish, ret: %d, hwid: %s\n", __FUNCTION__, ret, hwid);
    return ret;
}

long long int atoli(char str[]){
	long long int result=0;
	int i;
	for(i=0; i < strlen(str);i++){
		if(str[i] >= '0' && str[i]<='9'){
			result = result*10+str[i]-'0';
		}

	}
	return result;
}
  
static size_t http_respone_write(void *ptr, size_t size, size_t nmemb, void *userp)
{
   	size_t nb = size*nmemb;
    	struct http_respone_data* rdata = (struct http_respone_data*)userp;
    	if((rdata->data_len+nb) > rdata->buf_len)
    	{
       	 size_t sz = rdata->data_len+nb+1;
        	rdata->buf = (unsigned char*)realloc(rdata->buf, sz);
        	rdata->buf_len = sz;
    	}
    	memcpy(rdata->buf+rdata->data_len, ptr, nb);
    	rdata->data_len += nb;
    	rdata->buf[rdata->data_len] = '\0';
    	return nb;
}

int do_report_info(const char *url, const char *buff, char *recv_buf)
{
    CURL                        *curl = NULL;
    CURLcode                    retval;
    struct http_respone_data    rdata;
	struct curl_slist           *headers = NULL;
    int                         ret = -1;

    DEBUGP("%s: begin, url: %s, post_data: %s\n", __FUNCTION__, url, buff);

    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    headers=curl_slist_append(headers, "Request-Line: POST /routeservice/routeinfo.do HTTP/1.1");
    curl_slist_append(headers, "Content-Type: text/plain");
    curl = curl_easy_init();
	if(curl == NULL)
	{
	    goto EXIT;
	}

    DEBUGP("%s:process, url:[%s], buff:[%s],strlen:%d\n", __FUNCTION__, url, buff,strlen(buff));

    CURL_SETOPT(curl, CURLOPT_POST, 1);
	CURL_SETOPT(curl, CURLOPT_HTTPHEADER,headers);
	CURL_SETOPT(curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(curl, CURLOPT_URL, url);
	CURL_SETOPT(curl, CURLOPT_POSTFIELDS, buff);
    CURL_SETOPT(curl, CURLOPT_TIMEOUT, 60);
    CURL_SETOPT(curl, CURLOPT_CONNECTTIMEOUT, 10);

    retval = curl_easy_perform(curl);
	if(retval != CURLE_OK)
	{
		goto EXIT;
	}

    if(recv_buf){
        strncpy(recv_buf, (char *)rdata.buf, COMMENT_LENGTH);
    }

    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);
    
	ret = 0;

EXIT:  
    if (rdata.buf)
    {
        free(rdata.buf);
    }
        
    if(curl)
    {
    	curl_easy_cleanup(curl);
    }
    curl_slist_free_all(headers);
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_report_proto_info(struct route_proto_speed_info *f, struct route_proto_speed_info *s, unsigned int time)
{
    int res,ret = -1,i,sign = 0;
    uint32_t        interval_time;
    time_t			timestamp;
    unsigned char hwid[DEVICE_ID_LENGTH] = {0};
    char key[STRING_LENGTH] = {0};
    char md5_src[LONG_STRING_LENGTH] = {0};
    char recv_buf[COMMENT_LENGTH] = {0};
    
    char buf[PROTO_POST_DATA_LENGTH] = {0};
    char up_data[PROTO_POST_DATA_LENGTH] = {0};
    char down_data[PROTO_POST_DATA_LENGTH] = {0};

    char up_proto_speed[ROUTE_REPORT_PORTO_NUM][STRING_LENGTH] = {{0}};
    char down_proto_speed[ROUTE_REPORT_PORTO_NUM][STRING_LENGTH] = {{0}};
    struct route_proto_speed_info *f_t = NULL;
    struct route_proto_speed_info *s_t = NULL;

  //  char *test = "{\"s1\":1,\"s2\":\"src\",\"s3\":{\"t1\":2},\"s4\":[\"123\",\"234\"]}";

    DEBUGP("%s:begin\n", __FUNCTION__);

    if(f == NULL || s == NULL){
        goto EXIT;
    }

    device_hwid_read(hwid);
    DEBUGP("%s:process, hwid:[%s]\n", __FUNCTION__, hwid);

    get_timestamp(&timestamp);
    DEBUGP("%s:process, timestamp:[%ld]\n", __FUNCTION__, timestamp);
    
    for(i = 0;i<ROUTE_REPORT_PORTO_NUM;i++){
        f_t = f + i;
        s_t = s + i;
        snprintf(up_proto_speed[i],STRING_LENGTH-1,"[\"%s\",\"%llu\"]",
                s_t->proto_name,(s_t->tx_bytes - f_t->tx_bytes)*8/time);
        snprintf(down_proto_speed[i],STRING_LENGTH-1,"[\"%s\",\"%llu\"]",
                s_t->proto_name,(s_t->rx_bytes - f_t->rx_bytes)*8/time);
    }

    for(i=0;i<ROUTE_REPORT_PORTO_NUM;i++){
        if(sign){
            strncat(up_data,",",1);
            strncat(down_data,",",1);
        }

        strncat(up_data,up_proto_speed[i],strlen(up_proto_speed[i]));
        strncat(down_data,down_proto_speed[i],strlen(down_proto_speed[i]));
        sign=1;
    }

    snprintf(md5_src,sizeof(md5_src),"%s%ld%s",hwid,timestamp,(char *)MD5KEY);
    md5sum_build(md5_src, key);

    snprintf(buf,sizeof(buf),"{\"data\":{\"up\":[%s],\"down\":[%s]},\"dtdate\":\"%ld\",\"deviceid\":\"%s\",\"key\":\"%s\"}",
            up_data,down_data,timestamp,hwid,key);
    
    DEBUGP("%s:process, buf:[%s], len:%d\n", __FUNCTION__, buf, strlen(buf));

    do_report_info(PROTO_POST_URL,buf,recv_buf);
    DEBUGP("%s:process, recv_buf: %s\n", __FUNCTION__, recv_buf);

    res = parse_json_report_interval_time(recv_buf,strlen(recv_buf),&interval_time);
    if(res == 0){
        rtime = interval_time;
    }
 // rtime = 10;
// parse_json_report_interval_time(test,strlen(test),&interval_time);

    DEBUGP("%s: rtime: %d\n", __FUNCTION__, rtime);
    
    ret = 0;
EXIT:
    DEBUGP("%s:finish, ret:%d\n", __FUNCTION__, ret);
    return ret;
}



static int check_input(void)
{
    if((report_config.base + report_config.flow + report_config.log + report_config.proto +
        report_config.help) == 0)
    {
		printf("action error! maybe multi-action or none-action!\n");    
		return -1;
    }

    return 0;
}

int anys_line_info(char *temp, char *info[]){
	int temp_size,i,info_index=0;
	temp_size=strlen(temp);

	for(i=0;i<temp_size&&info_index<20;i++){
		if(temp[i]==32||temp[i]==':'){
			temp[i]='\0';
		}else{
			if(i==0){
				info[info_index]=temp;
				info_index++;
			}else if(temp[i-1]=='\0'){
				info[info_index]=temp+i;
				info_index++;
			}
		}
	}

	return info_index;
}

int get_route_face_info(struct route_face_byte_infos *route_face)
{
	int size;
	FILE *pp=NULL;
	char cmd[64],line_temp[256];
	sprintf(cmd,"cat %s",FLOW_FILE);
	pp=popen(cmd,"r");
	char* face_info[20]={NULL};
	//struct route_face_byte_info *temp_face_info=NULL;
	while (fgets(line_temp, sizeof(line_temp), pp) != NULL) {
    	if (line_temp[strlen(line_temp) - 1] == '\n') {
        		line_temp[strlen(line_temp) - 1] = '\0'; 
    	}
    	size=anys_line_info(line_temp,face_info);

       	if(strcmp(face_info[0],"LAN")==0){
    		DEBUGP("LAN %s %s\n",face_info[1],face_info[9]);
    		route_face->LAN0.rx_byte= atoli(face_info[1]);
    		route_face->LAN0.tx_byte= atoli(face_info[9]);
    		DEBUGP("LAN %lld %lld\n",route_face->LAN0.rx_byte,route_face->LAN0.tx_byte);
    	}else if(strcmp(face_info[0],"WAN0")==0){
    		route_face->WAN0.rx_byte= atoli(face_info[1]);
    		route_face->WAN0.tx_byte= atoli(face_info[9]);
    	}else if(strcmp(face_info[0],"WAN1")==0){
    		route_face->WAN1.rx_byte= atoli(face_info[1]);
    		route_face->WAN1.tx_byte= atoli(face_info[9]);
    	}else if(strcmp(face_info[0],"WAN2")==0){
    		route_face->WAN2.rx_byte= atoli(face_info[1]);
    		route_face->WAN2.tx_byte= atoli(face_info[9]);
    	}
	//	DEBUGP("%d %s %s %s\n",size,face_info[0],face_info[1],face_info[9]);
    }


//EXIT:
	if(pp!=NULL){
		pclose(pp);
	}
	
	return 0;
}

void add_proto_info(struct user_stat_entry *node, struct route_proto_speed_info **proto)
{
    if(node == NULL || *proto == NULL){
        return;
    }

    struct route_proto_speed_info *tmp = *proto;
    struct route_proto_speed_info *s = *proto;
    int i,sign = 0;

    DEBUGP("%s:begin,name:[%s]\n", __FUNCTION__, node->layer7_cat);

    for(i = 0;i < ROUTE_REPORT_PORTO_NUM;i++){
        tmp = s + i;
        if(strcmp(tmp->proto_name,node->layer7_cat) == 0){
            tmp->rx_bytes += node->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes;
            tmp->tx_bytes += node->counter.counter[NET_STAT_DIR_INT_TO_EXT].bytes;
            sign = 1;
            break;
        }
    }

    if(sign == 0){
        s = s + (ROUTE_REPORT_PORTO_NUM - 1);
        s->rx_bytes += node->counter.counter[NET_STAT_DIR_EXT_TO_INT].bytes;
        s->tx_bytes += node->counter.counter[NET_STAT_DIR_INT_TO_EXT].bytes;
    }

    DEBUGP("%s :finish\n", __FUNCTION__);
    return;
}


int get_proto_info(struct route_proto_speed_info *buf)
{
    struct user_stat_msg request;
    struct respond_message respond_msg;
    struct route_proto_speed_info *tmp = NULL;
    struct user_stat_entry *node = NULL;
    struct sockaddr_nl remote;
    int sock, recv_len, size;
    int i, ret = -1, total = 0;

    size = sizeof(struct respond_message);
    socklen_t remote_addr_len = sizeof(struct sockaddr_nl);

    DEBUGP("%s: begin\n", __FUNCTION__);

    tmp = buf;
    request.bridge_id = 0;
    request.addr = 0;
    request.layer7_id = 0;

    sock = _do_command(NET_STAT_PROTOCOL_SUM,&request);
    if(sock == -1){
        goto EXIT;   
    }

    DEBUGP("%s: process,do_command is ok\n", __FUNCTION__);

    while(1){
        recv_len = recvfrom(sock, &respond_msg, size, 0, (struct sockaddr*)&remote, &remote_addr_len);
        if(recv_len == -1){
            break;
        }

        if(recv_len == 0){
            break;
        }

        if(respond_msg.nlhdr.nlmsg_type < NET_STAT_PROTOCOL_SUM_OK ||
            respond_msg.nlhdr.nlmsg_type > NET_STAT_ERROR_INFO){
            break;
        }

        if(respond_msg.nlhdr.nlmsg_type == NET_STAT_NULL_INFO){
            break;
        }

        size = (respond_msg.nlhdr.nlmsg_len - sizeof(struct nlmsghdr)) / sizeof(struct user_stat_entry);
        for(i = 0; i < size; i++){
            node = &respond_msg.account_entry[i];
            if(node == NULL){
                break;
            }
            if(node->src_ip == 0 && node->layer7_name[0] == '\0'){
                break;
            }
            add_proto_info(node,&tmp);
            total++;
        }
    }

    close(sock);
    ret = 0;
    
EXIT:
    DEBUGP("%s:finish, total:%d, ret: %d\n", __FUNCTION__, total, ret);
    return ret;
}


static int report_command_parse(int *argc, char ***argv)
{
	char buff[1024];

	memset(&report_config, 0, sizeof(struct report_config_info));

	snprintf(buff, 1024, "%s - %s ,version %s\n\n", 
	REPORT_LONG_NAME, REPORT_DESCRIPTION, REPORT_VERSION);
	optTitle(buff);
	optProgName(REPORT_NAME);
    
    optrega(&report_config.base,        	OPT_FLAG,        'b',   "base",       "report base info");
	optrega(&report_config.flow,            OPT_FLAG,        'f',   "flow",       "report Flow velocity");
	optrega(&report_config.log,             OPT_FLAG,        'l',   "log",        "report Attack logs");
    optrega(&report_config.proto,           OPT_FLAG,        'p',   "proto",      "report Protocol statistics");
    optrega(&report_config.time,            OPT_INT,         't',   "time",       "The frequency of the report");
	optrega(&report_config.help,            OPT_FLAG,        'h',   "help",       "usage");
	opt(argc, argv); 

    if(report_config.help)
    {
        optPrintUsage();
        return 1;
    }

    if(optinvoked(&report_config.time)){
        rtime = report_config.time;
        if(rtime >= 300){
            rtime = 300;
        }else if(rtime <= 10){
            rtime = 10;
        }
    }

	return 0;
}

static int report_base(void)
{
	int iserror = -1,len;
//    int index=0, size=0, sign=0, total=0;
//	unsigned char hwid[DEVICE_ID_LENGTH] = {0};
//	int timestamp;
//	char *adsl_post_data = NULL;
    char *post_data = NULL;
 //   char *tmp  = NULL;
//	char MD5_SRC[264] ={0},url[] = BASE_POST_URL,key[33] = {0};
    const char *cmd = "/usr/local/httpd/htdocs/cgi-bin/sh/routerReport_script.sh list base";

    len = 50 * 1024;
    post_data = (char *)malloc(len);
    if(NULL == post_data){
        goto EXIT;
    }
    memset(post_data,0,len);

    iserror = do_get_command(cmd,post_data,len);
    if(iserror != 0){
        goto EXIT;
    }
    DEBUGP("%s: post data: [%s]\n",__FUNCTION__, post_data);
#if 0
    char wan_str[BASE_POST_DATA_LENGTH] = {0};
    char adsl_info[BASE_ADSL_INFO_LENGTH] = {0};
    char ifname[STRING_LENGTH] = {0};
    char iface[STRING_LENGTH] = {0};
    char ip[STRING_LENGTH]={0};
	char gateway[STRING_LENGTH]={0};
	char uptime[STRING_LENGTH]={0};
	char status[STRING_LENGTH]={0};
	
	struct mgtcgi_xml_interface_wans *wan = NULL;
    struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_dns *dns = NULL;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		goto EXIT;
	}

    len = BASE_ADSL_INFO_LENGTH*(adsl->num)+BASE_WAN_INFO_LENGTH*3;
    adsl_post_data = (char *)malloc(len);
	if(adsl_post_data == NULL){
		DEBUGP("malloc false!");
		goto EXIT;
	}
	memset(adsl_post_data,0,sizeof(adsl_post_data));
    tmp = adsl_post_data;


    for(index = 0; index < adsl->num; index++){
        if(adsl->pinfo[index].enable == 0){
            continue;
        }

        get_adslclient_link(adsl->pinfo[index].name, status, ip, gateway, uptime);
        if(strncmp(status,"no",2) == 0){
            continue;
        }
        
        if(strncmp(adsl->pinfo[index].ifname,"WAN",3) == 0){
            snprintf(ifname,sizeof(ifname),"%s|%s",adsl->pinfo[index].name,adsl->pinfo[index].ifname);
        }
        else if(strncmp(adsl->pinfo[index].ifname,"ads",3) == 0 ||
            strncmp(adsl->pinfo[index].ifname,"vet",3) == 0){
            get_vlan_iface(adsl->pinfo[index].ifname,iface);
            snprintf(ifname,sizeof(ifname),"%s|%s|%s",adsl->pinfo[index].name,adsl->pinfo[index].ifname,iface);
        }

        snprintf(adsl_info,sizeof(adsl_info),"{\"ifname\":\"%s\",\"extend\":\"\",\"gateway\":\"%s\",\"netmask\":\"\",\"publicip\":\"%s\"}",
                ifname,gateway,ip);

        if(sign){
            strncat(tmp,",",1);
        }

        strncat(tmp,adsl_info,strlen(adsl_info));
        sign = 1; 
        total++;

        memset(adsl_info,0,sizeof(adsl_info));
        memset(ifname,0,sizeof(ifname));
        memset(iface,0,sizeof(iface));
        memset(ip,0,sizeof(ip));
        memset(gateway,0,sizeof(gateway));
        memset(uptime,0,sizeof(uptime));
        memset(status,0,sizeof(status));
    }

    DEBUGP("%s:process, adsl_post_data: %s\n", __FUNCTION__, adsl_post_data);
	
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	if (NULL == wan){
		DEBUGP("base WAN info report false!");
		goto EXIT;
	}
	
	for (index=0; index < wan->num; index++){
        snprintf(wan_str,sizeof(wan_str),"{\"ifname\":\"%s\",\"extend\":\"\",\"gateway\":\"%s\",\"netmask\":\"%s\",\"publicip\":\"%s\"}",
            wan->pinfo[index].ifname,wan->pinfo[index].gateway,wan->pinfo[index].mask,wan->pinfo[index].ip);
        
        if(sign){
            strncat(tmp,",",1);
        }

        strncat(tmp,wan_str,strlen(wan_str));
        sign = 1; 
        total++;
        memset(wan_str,0,sizeof(wan_str));
	}

    DEBUGP("%s:process, adsl_post_data:%s\n", __FUNCTION__, adsl_post_data);

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void**)&dns, &size);
	if (NULL == dns){
		DEBUGP("base DNS info report false!");
		goto EXIT;
	}
	timestamp = get_curr_unixtime();
	sprintf(MD5_SRC,"%d%s%d%s",total,hwid,timestamp,(char *)MD5KEY);
    md5sum_build(MD5_SRC,key);

    post_data = (char *)malloc(len+256);
    if(NULL == post_data){
        goto EXIT;
    }

	snprintf(post_data,len+256,"{\"data\":[%s],\"dns1\":\"114.114.114.114\",\"dns2\":\"8.8.8.8\",\"total\":\"%d\",\"deviceid\":\"%s\",\"dtdate\":\"%d\",\"key\":\"%s\"}",
        adsl_post_data, total, hwid, timestamp, key);
    DEBUGP("%s:process, post_data: %s\n", __FUNCTION__, post_data);
#endif
	do_report_info(BASE_POST_URL,post_data, NULL);
    iserror = 0;
EXIT:
#if 0
    if(adsl){
        free_xml_node((void*)&adsl);
    }
	if(wan){
	    free_xml_node((void*)&wan);
    }
	if(dns){
	    free_xml_node((void*)&dns);
    }
    if(adsl_post_data){
        free(adsl_post_data);
        adsl_post_data = NULL;
    }
#endif
    if(post_data){
		free(post_data);
        post_data = NULL;
	}
    DEBUGP("%s: finish\n", __FUNCTION__);
	return iserror;
}

static int report_flow(void)
{
	unsigned char	hwid[DEVICE_ID_LENGTH] = {0};
	char FACE_INFO[6][FLOW_FACE_INFO_LENGTH] = {{0},{0},{0},{0}},MD5_SRC[128] ={0},key[33] = {0};
	char *post_data = NULL;
    char recv_buf[COMMENT_LENGTH] = {0};
	int			res, timestamp;
    uint32_t    interval_time;	
	struct route_face_byte_infos *pre_route_face=NULL, * nt_route_face=NULL;
    
    device_hwid_read(hwid);
	pre_route_face = (struct route_face_byte_infos *)malloc(sizeof(struct route_face_byte_infos));
	nt_route_face = (struct route_face_byte_infos *)malloc(sizeof(struct route_face_byte_infos));

	memset(pre_route_face,0,sizeof(struct route_face_byte_infos));
	memset(nt_route_face,0,sizeof(struct route_face_byte_infos));
		
	get_route_face_info(pre_route_face);
	sleep(3);
	get_route_face_info(nt_route_face);
	timestamp = get_curr_unixtime();
	sprintf(FACE_INFO[0],FLOW_POST_IFNAME_TEMP,"LAN",(nt_route_face->LAN0.rx_byte-pre_route_face->LAN0.rx_byte)/3,
		(nt_route_face->LAN0.tx_byte-pre_route_face->LAN0.tx_byte)/3,timestamp);
	sprintf(FACE_INFO[1],FLOW_POST_IFNAME_TEMP,"WAN0",(nt_route_face->WAN0.rx_byte-pre_route_face->WAN0.rx_byte)/3,
		(nt_route_face->WAN0.tx_byte-pre_route_face->WAN0.tx_byte)/3,timestamp);
	sprintf(FACE_INFO[2],FLOW_POST_IFNAME_TEMP,"WAN1",(nt_route_face->WAN1.rx_byte-pre_route_face->WAN1.rx_byte)/3,
		(nt_route_face->WAN1.tx_byte-pre_route_face->WAN1.tx_byte)/3,timestamp);
	sprintf(FACE_INFO[3],FLOW_POST_IFNAME_TEMP,"WAN2",(nt_route_face->WAN2.rx_byte-pre_route_face->WAN2.rx_byte)/3,
		(nt_route_face->WAN2.tx_byte-pre_route_face->WAN2.tx_byte)/3,timestamp);
	DEBUGP("%lld %lld\n",(nt_route_face->LAN0.rx_byte-pre_route_face->LAN0.rx_byte)/3,(nt_route_face->LAN0.tx_byte-pre_route_face->LAN0.tx_byte)/3);
	sprintf(MD5_SRC,"%s%d%s",hwid,timestamp,MD5KEY);
	
	md5sum_build(MD5_SRC,key);
	post_data=(char*)malloc(FLOW_POST_DATA_LENGTH);
	if(post_data == NULL){
		DEBUGP("malloc false!");
		goto EXIT;
	}
	memset(post_data,0,FLOW_POST_DATA_LENGTH);
	
	sprintf(post_data,FLOW_POST_DATA_TEMP,FACE_INFO[0],FACE_INFO[1],FACE_INFO[2],FACE_INFO[3],hwid,timestamp,key);
	DEBUGP("%s\n",post_data);
	do_report_info(FLOW_POST_URL,post_data, recv_buf);
    DEBUGP("%s: process, recv_buf: %s\n", __FUNCTION__, recv_buf);

    res = parse_json_report_interval_time(recv_buf, strlen(recv_buf), &interval_time);
    if(res == 0){
        rtime = interval_time;
    }
EXIT:
	if(post_data != NULL){
		free(post_data);
		post_data = NULL;
	}
    if(pre_route_face){
        free(pre_route_face);
        pre_route_face = NULL;
    }
    if(nt_route_face){
        free(nt_route_face);
        nt_route_face = NULL;
    }
	return 0;
}

static int report_log(void)
{
	char post_data[POST_DATA_LEN]={0},ids_log[IDS_LOG_LEN]={0};
	unsigned char	hwid[DEVICE_ID_LENGTH] = {0};
	char MD5_SRC[264] ={0},key[33] = {0};
	int timestamp = get_curr_unixtime();
	
	device_hwid_read(hwid);
	sprintf(MD5_SRC,"%s%d%s",hwid,timestamp,(char *)MD5KEY);
	md5sum_build(MD5_SRC,key);
	
	display_ids_log(ids_log);
    if(strlen(ids_log) > 0){
	    sprintf(post_data,LOG_POST_DATA_TEMP,ids_log,hwid,timestamp,key);
	    do_report_info(SECUTY_LOG_POST_URL,post_data,NULL);
    }
	return 0;
}

static int report_proto(int sign){
    int iserror = -1, ret, i;
    unsigned int delay_t = 1;
    struct route_proto_speed_info *first = NULL;
    struct route_proto_speed_info *second = NULL;

    struct route_proto_speed_info *tmp_first = NULL;
    struct route_proto_speed_info *tmp_second = NULL;

    struct route_proto_speed_info *s = NULL;

    DEBUGP("%s: begin\n", __FUNCTION__);

    if(sign == 0){
        iserror = 0;
        goto EXIT;
    }

    struct_init(&first);
    struct_init(&second);

    DEBUGP("%s:process, first:[%p], second:[%p]\n", __FUNCTION__,
            first,second);

    if(first == NULL || second == NULL){
        goto EXIT;
    }

    memcpy(first,temp,sizeof(temp));
    memcpy(second,temp,sizeof(temp));

    tmp_first = first;
    tmp_second = second;

    ret = get_proto_info(tmp_first);
    if(ret != 0){
        goto EXIT;
    }
    delay(delay_t);
    ret = get_proto_info(tmp_second);
    if(ret != 0){
        goto EXIT;
    }

    s = first;
    for(i = 0;i<ROUTE_REPORT_PORTO_NUM;i++){
        DEBUGP("name:%s, in_bytes:%llu, out_bytes:%llu\n",
            s->proto_name,s->rx_bytes,s->tx_bytes);
        s++;
    }

    DEBUGP("------------------------------------------------------\n");
    s = second;
    for(i = 0;i<ROUTE_REPORT_PORTO_NUM;i++){
        DEBUGP("name:%s, in_bytes:%llu, out_bytes:%llu\n",
            s->proto_name,s->rx_bytes,s->tx_bytes);
        s++;
    }

    do_report_proto_info(tmp_first,tmp_second,delay_t);
    iserror = 0;

EXIT:
    if(first){
        struct_free(&first);
        first = NULL;
    }
    if(second){
        struct_free(&second);
        second = NULL;
    }

    DEBUGP("%s:finish, iserror:[%d]\n", __FUNCTION__, iserror);
    return iserror;
}

int main(int argc, char *argv[])
{

	int ret = -1; 

	if(report_command_parse(&argc, &argv) != 0)
    {
		return ret;
	}

	if(check_input() != 0)
    {
		return ret;
	}

#if 1
    if(rtime == 0){
        do{
            if(report_config.base){
                report_base();
            }
            if(report_config.flow){
                report_flow();
            }
            if(report_config.log){
                report_log();
            }
            if(report_config.proto){
                report_proto(1);
            }
        }while(0);
    }else{
        do{
            if(report_config.base){
                report_base();
            }
            if(report_config.flow){
                report_flow();
            }
            if(report_config.log){
                report_log();
            }
            if(report_config.proto){
                report_proto(1);
            }
            delay(rtime);
        }while(1);
    }
#endif

    ret = 0;
	return ret;	
}

