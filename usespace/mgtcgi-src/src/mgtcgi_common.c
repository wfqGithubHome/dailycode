#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/route_xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

//对象组列表显示
//int get_xml_node(const char * filename,const int nodetype,
//							void ** buf, int * out_size);

//2011.12.30 add  route 

static char log_buff[LOG_LINE_MAX];

extern void flush_timer(void);
extern void start_timer(void);
extern int check_flash_mount(void);

const char *route_script_path = "/usr/local/httpd/htdocs/cgi-bin/sh";

int do_log(char *log)
{
	int fd = 0;
	fd = open("/home/mgtcgi_log.txt", O_CREAT | O_APPEND | O_WRONLY,00644);
	
	write(fd, log, strlen(log));
	close(fd);
    return 0;
}

void log_debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buff, LOG_LINE_MAX, fmt, args);
    do_log(log_buff);
    va_end(args);
}

int do_sys_command(char *cmd)
{
	int value=1;
	char buffer[DIGITAL_LENGTH]={0};
	FILE *pp = NULL;
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
	
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
			value = 0;
		else
			value = -1;
	}
	pclose(pp);
	return value;
}


//返回值: 1 已经连接; 0 未连接；
int get_link_detected(const char *ifname)
{
	int  ret_value = -1;
	char cmd[CMD_LITTER_LENGTH]={0};
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh list link %s", ifname);

	ret_value = do_sys_command(cmd);
	if (ret_value == 0)
		return 1;
	else
		return 0;
}

int get_link_speed(const char *ifname)
{
    char 		buf[128] ={0};
	FILE 		*pp;
	char cmd[CMD_LITTER_LENGTH] = {0};
    int ret = 0;

	snprintf(cmd, sizeof(cmd), "/sbin/ethtool %s 2>/dev/null | grep %s | awk -F ': ' '{print $2}' | awk -F 'Mb' '{print $1}'",
        ifname, "Speed:");
        
	pp = popen(cmd, "r");
    if (pp == NULL)
    {
        goto exit;
    }
    
	fgets(buf, sizeof(buf), pp);
    ret = atoi(buf);

exit:
    if (pp)
    {
        pclose(pp);
    }
    
    return ret;
}


int get_page_turn_index(char *page_str, char *pagesize_str, int total, int *index, int *index_end)
{
	int page=0,pagesize=0,iserror=0;
	page	= atoi(page_str);
	pagesize= atoi(pagesize_str);
	if ((page < PAGE_LITTLE_VALUE) ||
		(pagesize < PAGE_LITTLE_VALUE)){
		iserror = MGTCGI_PARAM_ERR;
		goto EXIT;
	}
		
	*index = ((page * pagesize) - pagesize);
	*index_end = (page * pagesize);
	if (*index > total){
		*index = total;
	}
	if (*index_end > total){
		*index_end=total;
	}

	iserror = 0;
EXIT:
	return iserror;
}

int apply_timing_adslredial(const char *iface, const char *minute, const char *hour,
                                const char *day, const char *month, const char *week)
{
    char *pppoe_restart = "/usr/local/sbin/pppoe-restart";
    char *crond_file_path = "/etc/crontabs/root";
    char *crond_restart="/etc/init.d/crond restart";

    char iface_str[STRING_LENGTH]={0};
    char minute_str[STRING_LENGTH]={0};
    char hour_str[STRING_LENGTH]={0};
    char day_str[STRING_LENGTH]={0};
    char month_str[STRING_LENGTH]={0};
    char week_str[STRING_LENGTH]={0};
    char cmd_str[STRING_LENGTH]={0};
    char crond_str[LONG_STRING_LENGTH] = {0};
    char cmd[CMD_LENGTH]={0};

    int index,i = 0;

    if(strlen(minute) == 0){
        strcpy(minute_str,"*");
    }else{
        strcpy(minute_str,minute);
    }

    if(strlen(hour) == 0){
        strcpy(hour_str,"*");
    }else{
        strcpy(hour_str,hour);
    }

    if(strlen(day) == 0){
        strcpy(day_str,"*");
    }else{
        strcpy(day_str,day);
    }

    if(strlen(month) == 0){
        strcpy(month_str,"*");
    }else{
        strcpy(month_str,month);
    }

    if(strlen(week) == 0){
        strcpy(week_str,"*");
    }else{
        strcpy(week_str,week);
    }

    for(index = 0; index <= strlen(iface); index++){
        if((iface[index] == ',') || (index == strlen(iface))){
                snprintf(cmd_str,sizeof(cmd_str),"%s /etc/ppp/%s.conf",pppoe_restart,iface_str);

                snprintf(crond_str,sizeof(crond_str),"%s %s %s %s %s %s",
                                            minute_str,hour_str,day_str,month_str,week_str,cmd_str);
                snprintf(cmd,sizeof(cmd),"echo \"%s\" >> %s",crond_str, crond_file_path);
                do_sys_command(cmd);

                memset(iface_str,0,sizeof(iface_str));
                memset(cmd_str,0,sizeof(cmd_str));
                memset(crond_str,0,sizeof(crond_str));
                memset(cmd,0,sizeof(cmd));
                i=0;
        }else{
            iface_str[i] = iface[index];
            i++;
        }

    }

    snprintf(cmd,sizeof(cmd),"%s",crond_restart);
    do_sys_command(cmd);
    return 0;
}

int remove_timing_adsl(const char *iface, const char *minute, const char *hour,
                                const char *day, const char *month, const char *week)
{
    char *common_script = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh";
    char *pppoe_restart = "/usr/local/sbin/pppoe-restart";
    char *crond_restart="/etc/init.d/crond restart";

    char iface_str[STRING_LENGTH]={0};
    char crond_str[LONG_STRING_LENGTH] = {0};
    char minute_str[STRING_LENGTH]={0};
    char hour_str[STRING_LENGTH]={0};
    char day_str[STRING_LENGTH]={0};
    char month_str[STRING_LENGTH]={0};
    char week_str[STRING_LENGTH]={0};
    char cmd_str[STRING_LENGTH]={0};
    char cmd[CMD_LENGTH]={0};
    int index,i = 0;

    if(strlen(minute) == 0){
        strcpy(minute_str,"*");
    }else{
        strcpy(minute_str,minute);
    }

    if(strlen(hour) == 0){
        strcpy(hour_str,"*");
    }else{
        strcpy(hour_str,hour);
    }

    if(strlen(day) == 0){
        strcpy(day_str,"*");
    }else{
        strcpy(day_str,day);
    }

    if(strlen(month) == 0){
        strcpy(month_str,"*");
    }else{
        strcpy(month_str,month);
    }

    if(strlen(week) == 0){
        strcpy(week_str,"*");
    }else{
        strcpy(week_str,week);
    }

     for(index = 0; index <= strlen(iface); index++){
        if((iface[index] == ',') || (index == strlen(iface))){
            snprintf(cmd_str,sizeof(cmd_str),"%s /etc/ppp/%s.conf",pppoe_restart,iface_str);

            snprintf(crond_str,sizeof(crond_str),"\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %s",
                                        minute_str,hour_str,day_str,month_str,week_str,cmd_str);

            snprintf(cmd,sizeof(cmd),"%s remove timing_adsl %s",common_script,crond_str);
            do_sys_command(cmd);

            memset(iface_str,0,sizeof(iface_str));
            memset(cmd_str,0,sizeof(cmd_str));
            memset(crond_str,0,sizeof(crond_str));
            memset(cmd,0,sizeof(cmd));
            i=0;
        }else{
            iface_str[i] = iface[index];
            i++;
        }
    }   

    snprintf(cmd,sizeof(cmd),"%s",crond_restart);
    do_sys_command(cmd);
    return 0;
}

unsigned long int get_total_session_limit(void)
{
	char 	cmd[CMD_LENGTH];
	char 	total_session_str[DIGITAL_LENGTH];
	unsigned long int 	total_session = 10000;
	FILE	*pp;
	snprintf(cmd, sizeof(cmd), GET_TOTAL_SESSION_LIMIT);
	if((pp = popen(cmd, "r")) != NULL){
		fgets(total_session_str, DIGITAL_LENGTH, pp);
		total_session = strtoul(total_session_str, 0, 10);
		if (0 == total_session)
			total_session = 200000;
	}
	pclose(pp);	
	
	return (total_session);
}


int get_max_bridge_num(void)
{
	char 	cmd[CMD_LENGTH];
	char 	bridge_num_str[DIGITAL_LENGTH];
	int 	bridge_num = 1;
	FILE	*pp;

	snprintf(cmd, sizeof(cmd), GET_MAX_BRIDGE_NUM);
	
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(bridge_num_str, DIGITAL_LENGTH, pp);
		bridge_num = atoi(bridge_num_str);
		if (!(0 < bridge_num && bridge_num < 256))
			bridge_num = 1;
	}
	pclose(pp);	

	return (bridge_num);
}


int get_bridge_num(void)
{
	char 	cmd[CMD_LENGTH];
	char 	bridge_num_str[DIGITAL_LENGTH];
	int 	bridge_num = 1;
	FILE	*pp;

	snprintf(cmd, sizeof(cmd), GET_BRIDGE_NUM);
	
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(bridge_num_str, DIGITAL_LENGTH, pp);
		bridge_num = atoi(bridge_num_str);
		if (!(0 < bridge_num && bridge_num < 256))
			bridge_num = 1;
	}
	pclose(pp);	

	return (bridge_num);
}

void get_traffic_comment(int direct, const char *name, char *comment)
{
       int size, i;
       struct mgtcgi_xml_traffics *channels = NULL;

       get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size);
    	if ( NULL == channels){
    		goto ERROR_EXIT;
    	}

       for(i = 0; i < channels->num; i++){
              if((direct == channels->pinfo[i].direct) &&
                (strcmp(name,channels->pinfo[i].classname) == 0)){
                     strcpy(comment,channels->pinfo[i].comment);
                     break;
              }
       }

ERROR_EXIT:
       free_xml_node((void*)&channels);
       return;
}

void get_interface_name(char data[], const char *bridgeid)
{
	char 	cmd[CMD_LITTER_LENGTH]={0};
	char 	buffer[CMD_LENGTH]={0};
	FILE	*pp;

	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_interface_name %s", bridgeid);
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(buffer, sizeof(buffer), pp);
		strncpy(data, buffer, sizeof(buffer));
		data[strlen(buffer)-1]='\0';
	}
	pclose(pp);	
}


#if 0
char *get_mgt_ipaddr(void)
{
	;
}
#endif

inline void *malloc_buf(int size)
{
	if(size >0 )
		return (malloc(size));
	else
		return (NULL);
}
inline void free_malloc_buf(void **buf)
{
	free(*buf);
	*buf = NULL;
}

void interface_list_show(void)
{
#if 0
	int size=0,index=0,iserror = 0;
	int max_bridgenum=1,link_detected=0;
    U16 total = 0;
    U16 link_speed = 0;
    struct mgtcgi_xml_interfaces *interfaces = NULL;

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_INTERFACES,(void**)&interfaces,&size);
	if ( NULL == interfaces){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	total = interfaces->num;
#endif

#if 1
    printf("{\"data\":["  
           "{\"name\":\"WAN0\",\"linkstatus\":%d,\"linkstate\":0,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"},"
           "{\"name\":\"WAN1\",\"linkstatus\":%d,\"linkstate\":0,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"},"
           "{\"name\":\"WAN2\",\"linkstatus\":%d,\"linkstate\":0,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"},"
           "{\"name\":\"WAN3\",\"linkstatus\":%d,\"linkstate\":0,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"},"
           "{\"name\":\"WAN4\",\"linkstatus\":%d,\"linkstate\":0,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"},"
           "{\"name\":\"LAN\",\"linkstatus\":%d,\"linkstate\":3,\"bridgeid\":0,\"mode\":\"auto\",\"speed\":1000,\"duplex\":\"full\",\"status\":\"normal\"}],"
           "\"total\":6,\"max_bridgenum\":1,\"iserror\":0,\"msg\":\"\"}",
           get_link_detected("WAN0"),get_link_detected("WAN1"),get_link_detected("WAN2"),get_link_detected("WAN3"),
           get_link_detected("WAN4"),get_link_detected("LAN"));
#endif

#if 0
	printf("{\"data\":[");
	for (index=0; index < total;)
    {
		link_detected = get_link_detected(interfaces->pinfo[index].name);
        link_speed = get_link_speed(interfaces->pinfo[index].name);

        
        log_debug("name: %s, linkstatus: %d, linkstate: %u, bridgeid: %u, mode: %s, speed: %u, duplex: %s\n",
			interfaces->pinfo[index].name, link_detected,
			interfaces->pinfo[index].linkstate,
			interfaces->pinfo[index].bridgeid,
			interfaces->pinfo[index].mode,
			(link_speed > 0 ? link_speed : interfaces->pinfo[index].speed),
			interfaces->pinfo[index].duplex);


        printf("{\"name\":\"%s\",\"linkstatus\":%d,\"linkstate\":%d,\"bridgeid\":%d,\"mode\":\"%s\",\"speed\":%d,\"duplex\":\"%s\",\"status\":\"normal\"}",\
			interfaces->pinfo[index].name, link_detected,
			interfaces->pinfo[index].linkstate,
			interfaces->pinfo[index].bridgeid,
			interfaces->pinfo[index].mode,
			(link_speed > 0 ? link_speed : interfaces->pinfo[index].speed),
			interfaces->pinfo[index].duplex);

		index++;
		if (index < interfaces->num)
			printf(",");
		else
			break;
	}
	printf("],");

	max_bridgenum = get_max_bridge_num();

	printf("\"total\":%d,\"max_bridgenum\":%d,\"iserror\":0,\"msg\":\"\"}",total, max_bridgenum);
#endif

//	goto FREE_EXIT;
#if 0
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
#endif
//FREE_EXIT:
	//free_xml_node((void*)&interfaces);
	return;	
}

void urlfilter_list_show(void)
{
	const char		*url_filter_file_path ="/etc/url_filter.info";
	char				buf[STRING_LENGTH] = {0};
	FILE				*fp = NULL;
	int				first = 0;
	int				iserror = 1;

	if(access(url_filter_file_path,F_OK) != 0)
	{
		iserror = 0;
		goto errorexit;
	}

	fp = fopen(url_filter_file_path,"r");
	if(NULL == fp)
	{
		goto errorexit;
	}

	printf("{\"urllist\":[");
	while(fgets(buf,STRING_LENGTH,fp) != NULL)
	{
		buf[strlen(buf) - 1] = '\0';
		if(first)
		{
			printf(",");
		}
		printf("\"%s\"",buf);
		memset(buf,0,STRING_LENGTH);
		first = 1;
	}
	
	iserror = 0;
	printf("],\"iserror\":\"%d\",\"msg\":\"\"}",iserror);
	goto exit;

errorexit:
	printf("{\"urllist\":[],\"iserror\":\"%d\",\"msg\":\"\"}",iserror);
	goto exit;
exit:
	if(fp)
	{
		fclose(fp);
	}
	return;
}

void adsl_log_show(void)
{
      char      buf[COMMENT_LENGTH] = {0};
      char      cmd[CMD_LENGTH] = {0};
      char      str[STRING_LENGTH] = {0};
      int       status = -1,iserror = -1,sign = 0;
      int       max_log = GET_MAX_ADSL_LOG;
      FILE      *fp = NULL;
      FILE      *pp = NULL;

      if(access(PPPD_ADSL_LOG_FILE_PATH,0) != 0){
            iserror = 0;
            goto ERROR_EXIT;
      }

      fp = fopen(PPPD_ADSL_LOG_FILE_PATH,"r");
      if(NULL == fp){
            iserror = MGTCGI_READ_FILE_ERR;
            goto ERROR_EXIT;
      }

      printf("{\"data\":[");
      while(max_log-- && (fgets(buf,sizeof(buf),fp) != NULL)){
            if(buf[strlen(buf) - 1] == '\n'){
                buf[strlen(buf) - 1] = '\0';
            }
            snprintf(cmd,sizeof(cmd),"echo \"%s\" |awk -F ':' '{print $NF}'",buf);
            pp = popen(cmd,"r");
            if(NULL == pp){
                  memset(buf,0,sizeof(buf));
                  memset(cmd,0,sizeof(cmd));
                  continue;
            }

            fgets(str,sizeof(str),pp);
            pclose(pp);
            
            status = atoi(str);
            if(sign){
                  printf(",");
            }else{
                  sign = 1;
            }

            printf("{\"log\":\"%s\",\"status\":%d}",buf,status);
            memset(buf,0,sizeof(buf));
            memset(cmd,0,sizeof(cmd));
            status = -1;
      }

      fclose(fp);
      iserror = 0;
      printf("],\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto EXIT;
ERROR_EXIT:
      printf("{\"data\":[],\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto EXIT;
EXIT:
      return;
}

int copy_xmlconf_file(const char * srcfile, const char * dstfile)
{
    int ret_value=0;
    char cmd[512];

    flush_timer();
    if(check_flash_mount() != 0){
        ret_value = -1;
        goto EXIT;
    }

    snprintf(cmd, sizeof(cmd), "/bin/chmod +w /flash/etc/config.xml;/bin/cp %s %s ;sync;sync;/bin/chmod -w /flash/etc/config.xml",srcfile,dstfile);
	do_sys_command(cmd);

    start_timer();  
EXIT:
	return ret_value;
}

int check_interface_list(const char *string)
{
	char str_name[STRING_LENGTH] = {0};
    char *str = NULL;
	int  i=0,j=0;
	int comma_num=0,semicolon_num=0,comma_count=4;
    
    str = (char *)malloc(sizeof(char)*BUF_LENGTH_4K);
    if(NULL == str){
        return -1;
    }
    memset(str,0,sizeof(char)*BUF_LENGTH_4K);
    
	strncpy(str, string, sizeof(char)*BUF_LENGTH_4K);
    
    if(strlen(str) == 0){
        return 0;
    }
    
	for (i=0,j=0; i < strlen(str); i++){
		if (str[i] == ','){
			comma_num++;
			j = 0;
			continue;
		}
		if (str[i] == ';'){
			if (comma_num != comma_count)
				return  -1;
			
			semicolon_num++;
			comma_num=0;
			j = 0;
			continue;
		}

		if (j < sizeof(str_name))
			str_name[j] = str[i];
		else
			return -1;
		j++;
	}

    if(str){
        free(str);
        str = NULL;
    }
	return (semicolon_num);
}


void interface_edit_save(void)
{
	int index=0,iserror=0,i,j,result=0,count=0,new_size=0;
	char cmd[CMD_LENGTH];
//	char value[BUF_LENGTH_4K]={"eth0,3,0,auto,100,full;eth1,2,0,auto,100,full;"};
	char tmpstr[STRING_LENGTH]={"0"};
	int speed=0,linkstate=0,bridgeid=0,bridge_num=0;
	int comma_num=0,semicolon_num=0,mgtnum=0;
    char *value = NULL;
	struct mgtcgi_xml_interfaces *new_nodes = NULL;

    value = (char *)malloc(sizeof(char)*BUF_LENGTH_4K);
    if(NULL == value){
        iserror = MGTCGI_DUPLICATE_ERR;
        goto ERROR_EXIT;
    }
    memset(value,0,sizeof(char)*BUF_LENGTH_4K);

	cgiFormString("value", value, sizeof(value));

	count = check_interface_list(value);
	if (count < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_size = sizeof(struct mgtcgi_xml_interfaces) + 
			count * 
			sizeof(struct mgtcgi_xml_interface_include_info);

	new_nodes = (struct mgtcgi_xml_interfaces *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	new_nodes->num = count;
	comma_num=0;
	semicolon_num=0;
	memset(tmpstr, 0, sizeof(tmpstr));
	for (index=0,i=0,j=0; i<strlen(value); i++){
		//eth0,1,0,auto,100
		if (value[i] == ','){
			if (comma_num == 0){	//name
				strcpy(new_nodes->pinfo[index].name, tmpstr);
			}
			else if (comma_num == 1){	//linkstate
				linkstate = atoi(tmpstr);
				new_nodes->pinfo[index].linkstate = linkstate;
			}
			else if (comma_num == 2){	//bridgeid
				bridgeid = atoi(tmpstr);
				new_nodes->pinfo[index].bridgeid = bridgeid;
			}
			else if (comma_num == 3){	//mode
				strcpy(new_nodes->pinfo[index].mode, tmpstr);	
			}
			else if (comma_num == 4){	//speed
				speed = atoi(tmpstr);
				new_nodes->pinfo[index].speed = speed;
			}
			else {
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num++;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (value[i] == ';'){
			if (comma_num == 5){	//duplex
				strcpy(new_nodes->pinfo[index].duplex, tmpstr);
				index++;
			}

			semicolon_num++;
			if (semicolon_num > new_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num=0;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (j < sizeof(tmpstr))
			tmpstr[j] = value[i];
		j++;
	}

	//参数验证
	mgtnum = 0;
	for (i=0; i < new_nodes->num; i++){
		for(j=i+1; j<new_nodes->num; j++){
			//同一个桥只运行配置一个内网口
			if((new_nodes->pinfo[i].bridgeid == new_nodes->pinfo[j].bridgeid) &&
				(new_nodes->pinfo[i].linkstate == 1) && 
				(new_nodes->pinfo[j].linkstate == 1)){ 
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
		}
		//必须有一个管理口
		if(new_nodes->pinfo[i].linkstate == 3){ 
			mgtnum++;
		}
	}
	
	if (mgtnum != 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	#if 0
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name = %s\n", index, new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].linkstate = %d\n", index, new_nodes->pinfo[index].linkstate);
		printf("new_nodes->pinfo[%d].bridgeid = %d\n", index, new_nodes->pinfo[index].bridgeid);
		printf("new_nodes->pinfo[%d].mode = %s\n", index, new_nodes->pinfo[index].mode);
		printf("new_nodes->pinfo[%d].speed = %d\n", index, new_nodes->pinfo[index].speed);
		printf("new_nodes->pinfo[%d].duplex = %s\n", index, new_nodes->pinfo[index].duplex);
	}
	#endif
		 
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_INTERFACES,(void*)new_nodes,new_size);
	if (result >= 0){
		/*for(i=0; i<sizeof(name); i++) {
			toupper_name[i] = toupper(name[i]);
		}
		memset(cmd, 0, sizeof(cmd));
		if (strcmp(mode, "auto") == 0){//自动
			snprintf(cmd, sizeof(cmd), "/sbin/ethtool -s %s autoneg on 2>/dev/null",toupper_name);
		} else {
			snprintf(cmd, sizeof(cmd), "/sbin/ethtool -s %s speed %d duplex %s autoneg off 2>/dev/null",toupper_name,speed,duplex);
		}
		system(cmd);*/

		//设置已经配置的最大桥数
		bridge_num = 0;
		for (index=0; index < new_nodes->num; index++){
			if (new_nodes->pinfo[index].linkstate == 1){
				bridge_num++;
			}
		}

		memset(cmd, 0, sizeof(cmd));	
		snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh save_network  %d", bridge_num);
		system(cmd);	
	}
	else {
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
    if(value){
        free(value);
        value = NULL;
    }
	return;	
}


/*******************************网络层策略移动*******************************************/
void copy_globalinfo_value(struct mgtcgi_xml_ip_session_limits * new_nodes,
										struct mgtcgi_xml_ip_session_limits * old_nodes, 
										int new_index, int old_index, int name_id)
{
	if (name_id  < 0)
		new_nodes->p_globalinfo[new_index].name = old_nodes->p_globalinfo[old_index].name;
	else
		new_nodes->p_globalinfo[new_index].name = name_id;//重新修改编号
	
	strcpy(new_nodes->p_globalinfo[new_index].addr, old_nodes->p_globalinfo[old_index].addr);
	strcpy(new_nodes->p_globalinfo[new_index].dst, old_nodes->p_globalinfo[old_index].dst);
	strcpy(new_nodes->p_globalinfo[new_index].per_ip_session_limit, old_nodes->p_globalinfo[old_index].per_ip_session_limit);
	strcpy(new_nodes->p_globalinfo[new_index].total_session_limit, old_nodes->p_globalinfo[old_index].total_session_limit);
	new_nodes->p_globalinfo[new_index].httplog = old_nodes->p_globalinfo[old_index].httplog;
	new_nodes->p_globalinfo[new_index].sesslog = old_nodes->p_globalinfo[old_index].sesslog;
	new_nodes->p_globalinfo[new_index].httpdirpolicy = old_nodes->p_globalinfo[old_index].httpdirpolicy;
	new_nodes->p_globalinfo[new_index].dnspolicy = old_nodes->p_globalinfo[old_index].dnspolicy;
	new_nodes->p_globalinfo[new_index].bridge = old_nodes->p_globalinfo[old_index].bridge;
	strcpy(new_nodes->p_globalinfo[new_index].action, old_nodes->p_globalinfo[old_index].action);
	strcpy(new_nodes->p_globalinfo[new_index].comment, old_nodes->p_globalinfo[old_index].comment);
}

extern int check_cgifrom_digit(int value, int min, int max);

void ipsessions_move_save(void)
{
	int index,old_size,new_size,new_index,iserror=0,result=0;
	int old_total_num=0,old_global_num=0,
		new_total_num=0,new_global_num=0;
	int new_name=0,old_name=0,bridge=0,ret_value=0,max_index=0,
		new_name_index=0,old_name_index=0,name_id=0;
	char new_name_str[DIGITAL_LENGTH]={"0"};
	char old_name_str[DIGITAL_LENGTH]={"2"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_ip_session_limits *old_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limits *new_nodes = NULL;	
	struct mgtcgi_xml_ip_session_limit_total_info *p_totalinfo_start = NULL;
	struct mgtcgi_xml_ip_session_limit_global_info *p_globalinfo_start = NULL;

	cgiFormString("new_name",new_name_str,STRING_LENGTH);
	cgiFormString("old_name",old_name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	if((strlen(new_name_str) < STRING_LITTLE_LENGTH) && 
		(strlen(old_name_str) < STRING_LITTLE_LENGTH) &&
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_name = atoi(new_name_str);
	old_name = atoi(old_name_str);
	bridge = atoi(bridge_str);

	ret_value = check_cgifrom_digit(new_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(old_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(bridge, 0, 256);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (new_name == old_name){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (old_nodes->global_limit_num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	old_name_index = -1;
	new_name_index = -1;
	max_index = 0;
	for (index=0,new_index=0; index < old_nodes->global_limit_num; index++){
		if (old_nodes->p_globalinfo[index].bridge== bridge){ 
			if (old_nodes->p_globalinfo[index].name == old_name)
				old_name_index = index;//old_name的位置

			if (old_nodes->p_globalinfo[index].name == new_name)
				new_name_index = index;//new_name的位置

			max_index = index;
		}
	}
	if (old_name_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_name_index == -1){//没有找到要移动的条目的位置
		if (new_name > old_name)
			new_name_index = max_index;
		else
			new_name_index = 0;
	}

	if (new_name_index == old_name_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	/***********************可以移动，构建新结构***************************************/
	old_total_num = old_nodes->total_limit_num;
	old_global_num = old_nodes->global_limit_num;	//网络层
	new_total_num = old_total_num;
	new_global_num = old_global_num;
	
	new_size = sizeof(struct mgtcgi_xml_ip_session_limits) + 
		new_total_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_total_info) +
		new_global_num * 
			sizeof(struct mgtcgi_xml_ip_session_limit_global_info);

	new_nodes = (struct mgtcgi_xml_ip_session_limits *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}

	p_totalinfo_start = (struct mgtcgi_xml_ip_session_limit_total_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits));
	p_globalinfo_start = (struct mgtcgi_xml_ip_session_limit_global_info*)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_ip_session_limits) + 
						old_total_num * 
						sizeof(struct mgtcgi_xml_ip_session_limit_total_info));

	//修改数据，指针
	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->total_limit_num = new_total_num;
	new_nodes->global_limit_num = new_global_num;
	new_nodes->p_totalinfo = p_totalinfo_start;
	new_nodes->p_globalinfo = p_globalinfo_start;
	//拷贝 总会话数限制 数据
	for (index=0; index < new_nodes->total_limit_num; index++){
		new_nodes->p_totalinfo[index].bridge = old_nodes->p_totalinfo[index].bridge;
		new_nodes->p_totalinfo[index].limit = old_nodes->p_totalinfo[index].limit;
		strcpy(new_nodes->p_totalinfo[index].overhead, old_nodes->p_totalinfo[index].overhead);
	}


	//拷贝 网络层策略 数据
	name_id = 0;
	for (index=0,new_index=0; index < new_nodes->global_limit_num; index++,new_index++){
		if (old_nodes->p_globalinfo[index].bridge== bridge){ 

			if (index == old_name_index){ 
				new_index--; //跳过old_name,跳过的部分在new_name位置插入
			
			}else if (index == new_name_index){ //插入old_name，并拷贝当前数据

				if (old_name_index < new_name_index){//往下移动

					//printf("1-name_id:%d\n",name_id);
					copy_globalinfo_value( new_nodes, old_nodes, new_index, index, name_id);
			
					name_id++;
					
					new_index++;
					//printf("2-name_id:%d\n",name_id);
					copy_globalinfo_value( new_nodes, old_nodes, new_index, old_name_index, name_id);

				} else {//往上移动
					//printf("3-name_id:%d\n",name_id);
					copy_globalinfo_value( new_nodes, old_nodes, new_index, old_name_index, name_id);
					name_id++;
					
					new_index++;
					//printf("4-name_id:%d\n",name_id);
					copy_globalinfo_value( new_nodes, old_nodes, new_index, index, name_id);
				}
				name_id++;

			} else {
				//printf("5-name_id:%d\n",name_id);
				copy_globalinfo_value( new_nodes, old_nodes, new_index, index, name_id);
				name_id++;
			}
	
		} else {
 			//不能修改的的另外一组数据(另外的其他桥)，原样拷贝
 			//printf("6-name_id:%d\n",old_nodes->p_globalinfo[index].name);
 			copy_globalinfo_value(new_nodes, old_nodes, new_index, index, -1);
		}
		
	}

#if 0
	for (index=0; index < new_nodes->global_limit_num; index++){
		printf("name:%d, src:%s, bridge:%d\n", new_nodes->p_globalinfo[index].name,
									new_nodes->p_globalinfo[index].addr,
									new_nodes->p_globalinfo[index].bridge);
	}
#endif 
	
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_IP_SESSION_LIMITS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return;
}


/*******************************应用层策略移动*******************************************/
void copy_firewalls_value(struct mgtcgi_xml_firewalls * new_nodes,
										struct mgtcgi_xml_firewalls * old_nodes, 
										int new_index, int old_index, int name_id)
{

	if (name_id  < 0)
		new_nodes->pinfo[new_index].name = old_nodes->pinfo[old_index].name;
	else
		new_nodes->pinfo[new_index].name = name_id;//重新修改编号

	new_nodes->pinfo[new_index].log = old_nodes->pinfo[old_index].log;
	new_nodes->pinfo[new_index].disabled = old_nodes->pinfo[old_index].disabled;
	new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[old_index].bridge;
	new_nodes->pinfo[new_index].dscp = old_nodes->pinfo[old_index].dscp;
	new_nodes->pinfo[new_index].learn_change = old_nodes->pinfo[old_index].learn_change;
	strcpy(new_nodes->pinfo[new_index].schedule, old_nodes->pinfo[old_index].schedule);
	strcpy(new_nodes->pinfo[new_index].vlan, old_nodes->pinfo[old_index].vlan);
	strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[old_index].src);
	strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[old_index].dst);
	strcpy(new_nodes->pinfo[new_index].session_limit, old_nodes->pinfo[old_index].session_limit);
	strcpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[old_index].proto);
	strcpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[old_index].action);
	strcpy(new_nodes->pinfo[new_index].in_traffic, old_nodes->pinfo[old_index].in_traffic);
	strcpy(new_nodes->pinfo[new_index].out_traffic, old_nodes->pinfo[old_index].out_traffic);
	strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment);
	/*页面不显示的参数*/
	strcpy(new_nodes->pinfo[new_index].auth, old_nodes->pinfo[old_index].auth);
	strcpy(new_nodes->pinfo[new_index].mac, old_nodes->pinfo[old_index].mac);
	strcpy(new_nodes->pinfo[new_index].quota, old_nodes->pinfo[old_index].quota);
	strcpy(new_nodes->pinfo[new_index].second_in_traffic, old_nodes->pinfo[old_index].second_in_traffic);
	strcpy(new_nodes->pinfo[new_index].second_out_traffic, old_nodes->pinfo[old_index].second_out_traffic);
	new_nodes->pinfo[new_index].quota_action = old_nodes->pinfo[old_index].quota_action;
}

void firewalls_move_save(void)
{
	int index,old_size,new_size,new_index;
	int old_num,new_num,iserror=0,result=0;
	int new_name=0,old_name=0,bridge=0,ret_value=0,max_index=0,
			new_name_index=0,old_name_index=0,name_id=0;
	char new_name_str[DIGITAL_LENGTH]={"0"};
	char old_name_str[DIGITAL_LENGTH]={"2"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_firewalls *old_nodes = NULL;	
	struct mgtcgi_xml_firewalls *new_nodes = NULL;	

	cgiFormString("new_name",new_name_str,STRING_LENGTH);
	cgiFormString("old_name",old_name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	if((strlen(new_name_str) < STRING_LITTLE_LENGTH) && 
		(strlen(old_name_str) < STRING_LITTLE_LENGTH) &&
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_name = atoi(new_name_str);
	old_name = atoi(old_name_str);
	bridge = atoi(bridge_str);

	ret_value = check_cgifrom_digit(new_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(old_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(bridge, 0, 256);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (new_name == old_name){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}


	old_name_index = -1;
	new_name_index = -1;
	max_index = 0;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 
			if (old_nodes->pinfo[index].name == old_name)
				old_name_index = index;//old_name的位置

			if (old_nodes->pinfo[index].name == new_name)
				new_name_index = index;//new_name的位置

			max_index = index;
		}
	}
	if (old_name_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_name_index == -1){//没有找到要移动的条目的位置
		if (new_name > old_name)
			new_name_index = max_index;
		else
			new_name_index = 0;
	}
	
	if (new_name_index == old_name_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}


	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	
	new_size = sizeof(struct mgtcgi_xml_firewalls) + 
		new_num * 
			sizeof(struct mgtcgi_xml_firewall_info);

	
	new_nodes = (struct mgtcgi_xml_firewalls *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}

	memset(new_nodes, 0, new_size);

	//拷贝 数据
	new_nodes->num = old_nodes->num;
	new_nodes->application_firewall = old_nodes->application_firewall;
	new_nodes->dos_detect = old_nodes->dos_detect;
	new_nodes->scan_detect = old_nodes->scan_detect;
	name_id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 

			if (index == old_name_index){ 
				new_index--; //跳过old_name,跳过的部分在new_name位置插入
			
			}else if (index == new_name_index){ //插入old_name，并拷贝当前数据

				if (old_name_index < new_name_index){//往下移动
					//printf("1-name_id:%d\n",name_id);
					copy_firewalls_value( new_nodes, old_nodes, new_index, index, name_id);
			
					name_id++;
					
					new_index++;
					//printf("2-name_id:%d\n",name_id);
					copy_firewalls_value( new_nodes, old_nodes, new_index, old_name_index, name_id);

				} else {//往上移动
					//printf("3-name_id:%d\n",name_id);
					copy_firewalls_value( new_nodes, old_nodes, new_index, old_name_index, name_id);
					name_id++;
					
					new_index++;
					//printf("4-name_id:%d\n",name_id);
					copy_firewalls_value( new_nodes, old_nodes, new_index, index, name_id);
				}
				name_id++;

			} else {
				//printf("5-name_id:%d\n",name_id);
				copy_firewalls_value( new_nodes, old_nodes, new_index, index, name_id);
				name_id++;
			}
	
		} else {
			//不能修改的的另外一组数据(另外的其他桥)，原样拷贝
			//printf("6-name_id:%d\n",old_nodes->pinfo[index].name);
			copy_firewalls_value(new_nodes, old_nodes, new_index, index, -1);
		}
		
	}

#if 0
	for (index=0; index < new_nodes->num; index++){
		printf("name:%d, src:%s, bridge:%d\n", new_nodes->pinfo[index].name,
									new_nodes->pinfo[index].src,
									new_nodes->pinfo[index].bridge);
	}
#endif 


	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}



/*******************************端口镜像策略移动*******************************************/
void copy_portmirrors_value(struct mgtcgi_xml_port_mirrors * new_nodes,
										struct mgtcgi_xml_port_mirrors * old_nodes, 
										int new_index, int old_index, int name_id)
{
	if (name_id  < 0)
		new_nodes->pinfo[new_index].name = old_nodes->pinfo[old_index].name;
	else
		new_nodes->pinfo[new_index].name = name_id;//重新修改编号

	new_nodes->pinfo[new_index].disabled = old_nodes->pinfo[old_index].disabled;
	new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[old_index].bridge;
	strcpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[old_index].proto);
	strcpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[old_index].action);
	strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[old_index].src);
	strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[old_index].dst);
	strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment);
}

void portmirrors_move_save()
{
	int index,old_size,new_size,new_index;
	int old_num,new_num,iserror=0,result=0;
	int new_name=0,old_name=0,bridge=0,ret_value=0,max_index=0,
			new_name_index=0,old_name_index=0,name_id=0;
	char new_name_str[DIGITAL_LENGTH]={"0"};
	char old_name_str[DIGITAL_LENGTH]={"2"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_port_mirrors *old_nodes = NULL;	
	struct mgtcgi_xml_port_mirrors *new_nodes = NULL;	

	cgiFormString("new_name",new_name_str,STRING_LENGTH);
	cgiFormString("old_name",old_name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	if((strlen(new_name_str) < STRING_LITTLE_LENGTH) && 
		(strlen(old_name_str) < STRING_LITTLE_LENGTH) &&
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_name = atoi(new_name_str);
	old_name = atoi(old_name_str);
	bridge = atoi(bridge_str);

	ret_value = check_cgifrom_digit(new_name, 0, 65535);
	if (0 != ret_value){
		iserror  = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(old_name, 0, 65535);
	if (0 != ret_value){
		iserror  = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(bridge, 0, 256);
	if (0 != ret_value){
		iserror  = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (new_name == old_name){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}

	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	old_name_index = -1;
	new_name_index = -1;
	max_index = 0;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 
			if (old_nodes->pinfo[index].name == old_name)
				old_name_index = index;//old_name的位置

			if (old_nodes->pinfo[index].name == new_name)
				new_name_index = index;//new_name的位置

			max_index = index;
		}
	}
	if (old_name_index == -1){//没有找到被移动的条目
		iserror  = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_name_index == -1){//没有找到要移动的条目的位置
		if (new_name > old_name)
			new_name_index = max_index;
		else
			new_name_index = 0;
	}
	
	if (new_name_index == old_name_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	
	new_size = sizeof(struct mgtcgi_xml_port_mirrors) + 
		MAX_BRIDGE_NUM * 
		sizeof(struct mgtcgi_xml_port_mirror_status) +
		new_num * 
			sizeof(struct mgtcgi_xml_port_mirror_info);

	
	new_nodes = (struct mgtcgi_xml_port_mirrors *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto SUCCESS_EXIT;
	}

	memset(new_nodes, 0, new_size);

	//拷贝状态数据
	for (index=0; index < MAX_BRIDGE_NUM; index++){
		new_nodes->pstatus[index].bridge = old_nodes->pstatus[index].bridge;
		new_nodes->pstatus[index].enable= old_nodes->pstatus[index].enable;
	}

	//拷贝 数据
	new_nodes->num = old_nodes->num;
	name_id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 

			if (index == old_name_index){ 
				new_index--; //跳过old_name,跳过的部分在new_name位置插入
			
			}else if (index == new_name_index){ //插入old_name，并拷贝当前数据

				if (old_name_index < new_name_index){//往下移动
					//printf("1-name_id:%d\n",name_id);
					copy_portmirrors_value( new_nodes, old_nodes, new_index, index, name_id);
			
					name_id++;
					
					new_index++;
					//printf("2-name_id:%d\n",name_id);
					copy_portmirrors_value( new_nodes, old_nodes, new_index, old_name_index, name_id);

				} else {//往上移动
					//printf("3-name_id:%d\n",name_id);
					copy_portmirrors_value( new_nodes, old_nodes, new_index, old_name_index, name_id);
					name_id++;
					
					new_index++;
					//printf("4-name_id:%d\n",name_id);
					copy_portmirrors_value( new_nodes, old_nodes, new_index, index, name_id);
				}
				name_id++;

			} else {
				//printf("5-name_id:%d\n",name_id);
				copy_portmirrors_value( new_nodes, old_nodes, new_index, index, name_id);
				name_id++;
			}
	
		} else {
			//不能修改的的另外一组数据(另外的其他桥)，原样拷贝
			//printf("6-name_id:%d\n",old_nodes->pinfo[index].name);
			copy_portmirrors_value(new_nodes, old_nodes, new_index, index, -1);
		}
		
	}

#if 0
	for (index=0; index < new_nodes->num; index++){
		printf("name:%d, src:%s, bridge:%d\n", new_nodes->pinfo[index].name,
									new_nodes->pinfo[index].src,
									new_nodes->pinfo[index].bridge);
	}
#endif 
	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_PORT_MIRRORS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return;
}


void copy_dnspolicys_value(struct mgtcgi_xml_dns_policys * new_nodes,
										struct mgtcgi_xml_dns_policys * old_nodes, 
										int new_index, int old_index, int name_id)
{
	if (name_id  < 0)
		new_nodes->pinfo[new_index].name = old_nodes->pinfo[old_index].name;
	else
		new_nodes->pinfo[new_index].name = name_id;//重新修改编号

	new_nodes->pinfo[new_index].disabled = old_nodes->pinfo[old_index].disabled;
	new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[old_index].bridge;
	new_nodes->pinfo[new_index].action = old_nodes->pinfo[old_index].action;
	strcpy(new_nodes->pinfo[new_index].domain_name, old_nodes->pinfo[old_index].domain_name);
	strcpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[old_index].src);
	strcpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[old_index].dst);
	strcpy(new_nodes->pinfo[new_index].targetip, old_nodes->pinfo[old_index].targetip);
	strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[old_index].comment);
}


/*******************************DNS策略移动*******************************************/
void dnspolicys_move_save()
{
	int index,old_size,new_size,new_index;
	int old_num,new_num,iserror=0,result=0;
	int new_name=0,old_name=0,bridge=0,ret_value=0,max_index=0,
			new_name_index=0,old_name_index=0,name_id=0;
	char new_name_str[DIGITAL_LENGTH]={"0"};
	char old_name_str[DIGITAL_LENGTH]={"2"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_dns_policys *old_nodes = NULL;	
	struct mgtcgi_xml_dns_policys *new_nodes = NULL;	

	cgiFormString("new_name",new_name_str,STRING_LENGTH);
	cgiFormString("old_name",old_name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	if((strlen(new_name_str) < STRING_LITTLE_LENGTH) && 
		(strlen(old_name_str) < STRING_LITTLE_LENGTH) &&
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_name = atoi(new_name_str);
	old_name = atoi(old_name_str);
	bridge = atoi(bridge_str);

	ret_value = check_cgifrom_digit(new_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(old_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(bridge, 0, 256);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (new_name == old_name){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	old_name_index = -1;
	new_name_index = -1;
	max_index = 0;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 
			if (old_nodes->pinfo[index].name == old_name)
				old_name_index = index;//old_name的位置

			if (old_nodes->pinfo[index].name == new_name)
				new_name_index = index;//new_name的位置

			max_index = index;
		}
	}
	if (old_name_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_name_index == -1){//没有找到要移动的条目的位置
		if (new_name > old_name)
			new_name_index = max_index;
		else
			new_name_index = 0;
	}
	
	if (new_name_index == old_name_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}


	/***********************可以移动，构建新结构***************************************/
	old_num = old_nodes->num;
	new_num = old_num;
	
	new_size = sizeof(struct mgtcgi_xml_dns_policys) + 
			 	new_num *
				sizeof(struct mgtcgi_xml_dns_policy_info);

	
	new_nodes = (struct mgtcgi_xml_dns_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);

	//拷贝 数据
	new_nodes->num = old_nodes->num;
	new_nodes->application_policy = old_nodes->application_policy;
	name_id = 0;
	for (index=0,new_index=0; index < new_nodes->num; index++,new_index++){
		if (old_nodes->pinfo[index].bridge== bridge){ 

			if (index == old_name_index){ 
				new_index--; //跳过old_name,跳过的部分在new_name位置插入
			
			}else if (index == new_name_index){ //插入old_name，并拷贝当前数据

				if (old_name_index < new_name_index){//往下移动

					//printf("1-name_id:%d\n",name_id);
					copy_dnspolicys_value( new_nodes, old_nodes, new_index, index, name_id);
					name_id++;
					
					new_index++;
					//printf("2-name_id:%d\n",name_id);
					copy_dnspolicys_value( new_nodes, old_nodes, new_index, old_name_index, name_id);

				} else {//往上移动
					//printf("3-name_id:%d\n",name_id);
					copy_dnspolicys_value( new_nodes, old_nodes, new_index, old_name_index, name_id);
					name_id++;
					
					new_index++;
					//printf("4-name_id:%d\n",name_id);
					copy_dnspolicys_value( new_nodes, old_nodes, new_index, index, name_id);
				}
				name_id++;

			} else {
				//printf("5-name_id:%d\n",name_id);
				copy_dnspolicys_value( new_nodes, old_nodes, new_index, index, name_id);
				name_id++;
			}
	
		} else {
			//不能修改的的另外一组数据(另外的其他桥)，原样拷贝
			//printf("6-name_id:%d\n",old_nodes->pinfo[index].name);
			copy_dnspolicys_value(new_nodes, old_nodes, new_index, index, -1);
		}
		
	}

#if 0
	for (index=0; index < new_nodes->num; index++){
		printf("name:%d, src:%s, bridge:%d\n", new_nodes->pinfo[index].name,
									new_nodes->pinfo[index].src,
									new_nodes->pinfo[index].bridge);
	}
#endif 

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_DNS_POLICYS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return;
}



/*******************************http目录策略移动*******************************************/
void copy_httpdirpolicys_value(struct mgtcgi_xml_http_dir_policys * new_nodes,
										struct mgtcgi_xml_http_dir_policys * old_nodes, 
										int new_index, int old_index, int name_id)
{
	if (name_id  < 0)
		new_nodes->p_policyinfo[new_index].name = old_nodes->p_policyinfo[old_index].name;
	else
		new_nodes->p_policyinfo[new_index].name = name_id;//重新修改编号

	new_nodes->p_policyinfo[new_index].disabled = old_nodes->p_policyinfo[old_index].disabled;
	new_nodes->p_policyinfo[new_index].bridge = old_nodes->p_policyinfo[old_index].bridge;
	strcpy(new_nodes->p_policyinfo[new_index].extgroup, old_nodes->p_policyinfo[old_index].extgroup);
	strcpy(new_nodes->p_policyinfo[new_index].servergroup, old_nodes->p_policyinfo[old_index].servergroup);
	strcpy(new_nodes->p_policyinfo[new_index].comment, old_nodes->p_policyinfo[old_index].comment);
	
}

void httpdirpolicys_move_save(void)
{
	int index,old_size,new_size,new_index,iserror=0,result=0;
	int old_policy_num=0,old_format_num=0,
		new_policy_num=0,new_format_num=0;
	int new_name=0,old_name=0,bridge=0,ret_value=0,max_index=0,
			new_name_index=0,old_name_index=0,name_id=0;
	char new_name_str[DIGITAL_LENGTH]={"0"};
	char old_name_str[DIGITAL_LENGTH]={"2"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	struct mgtcgi_xml_http_dir_policys *old_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policys *new_nodes = NULL;
	struct mgtcgi_xml_http_dir_policy_urlformat_info *p_urlinfo_start = NULL;
	struct mgtcgi_xml_http_dir_policy_info *p_policyinfo_start = NULL;

	cgiFormString("new_name",new_name_str,STRING_LENGTH);
	cgiFormString("old_name",old_name_str,STRING_LENGTH);
	cgiFormString("bridge",bridge_str,STRING_LENGTH);

	if((strlen(new_name_str) < STRING_LITTLE_LENGTH) && 
		(strlen(old_name_str) < STRING_LITTLE_LENGTH) &&
		(strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_name = atoi(new_name_str);
	old_name = atoi(old_name_str);
	bridge = atoi(bridge_str);

	ret_value = check_cgifrom_digit(new_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(old_name, 0, 65535);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	ret_value = check_cgifrom_digit(bridge, 0, 256);
	if (0 != ret_value){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (new_name == old_name){//如果相等，不做处理
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->policy_num < 1){//没有数据可以移动
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	old_name_index = -1;
	new_name_index = -1;
	max_index = 0;
	for (index=0,new_index=0; index < old_nodes->policy_num; index++){
		if (old_nodes->p_policyinfo[index].bridge== bridge){ 
			if (old_nodes->p_policyinfo[index].name == old_name)
				old_name_index = index;//old_name的位置

			if (old_nodes->p_policyinfo[index].name == new_name)
				new_name_index = index;//new_name的位置

			max_index = index;
		}
	}
	if (old_name_index == -1){//没有找到被移动的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (new_name_index == -1){//没有找到要移动的条目的位置
		if (new_name > old_name)
			new_name_index = max_index;
		else
			new_name_index = 0;
	}
	
	if (new_name_index == old_name_index){
		iserror = MGTCGI_SUCCESS;
		goto SUCCESS_EXIT;
	}


	/***********************可以移动，构建新结构***************************************/
	old_format_num = old_nodes->format_num;
	old_policy_num = old_nodes->policy_num;
	new_format_num = old_format_num;
	new_policy_num = old_policy_num;
	
	new_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
		new_format_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
		new_policy_num * 
			sizeof(struct mgtcgi_xml_http_dir_policy_info);

	new_nodes = (struct mgtcgi_xml_http_dir_policys *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	p_urlinfo_start = (struct mgtcgi_xml_http_dir_policy_urlformat_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys));
	p_policyinfo_start = (struct mgtcgi_xml_http_dir_policy_info *)
						((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys) + 
						new_format_num * 
						sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));

	//修改dir_policy数量，指针
	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->format_num = new_format_num;
	new_nodes->policy_num = new_policy_num;
	new_nodes->p_urlinfo = p_urlinfo_start;
	new_nodes->p_policyinfo = p_policyinfo_start;

	for (index=0; index < new_nodes->format_num; index++){
		new_nodes->p_urlinfo[index].bridge = old_nodes->p_urlinfo[index].bridge;
		strcpy(new_nodes->p_urlinfo[index].format, new_nodes->p_urlinfo[index].format);
		strcpy(new_nodes->p_urlinfo[index].comment, new_nodes->p_urlinfo[index].comment);
	}


	name_id = 0;
	for (index=0,new_index=0; index < new_nodes->policy_num; index++,new_index++){
		if (old_nodes->p_policyinfo[index].bridge== bridge){ 

			if (index == old_name_index){ 
				new_index--; //跳过old_name,跳过的部分在new_name位置插入
			
			}else if (index == new_name_index){ //插入old_name，并拷贝当前数据

				if (old_name_index < new_name_index){//往下移动

					//printf("1-name_id:%d\n",name_id);
					copy_httpdirpolicys_value( new_nodes, old_nodes, new_index, index, name_id);
			
					name_id++;
					new_index++;
					//printf("2-name_id:%d\n",name_id);
					copy_httpdirpolicys_value( new_nodes, old_nodes, new_index, old_name_index, name_id);

				} else {//往上移动
					//printf("3-name_id:%d\n",name_id);
					copy_httpdirpolicys_value( new_nodes, old_nodes, new_index, old_name_index, name_id);
					name_id++;
					
					new_index++;
					//printf("4-name_id:%d\n",name_id);
					copy_httpdirpolicys_value( new_nodes, old_nodes, new_index, index, name_id);
				}
				name_id++;

			} else {
				//printf("5-name_id:%d\n",name_id);
				copy_httpdirpolicys_value( new_nodes, old_nodes, new_index, index, name_id);
				name_id++;
			}
	
		} else {
			//不能修改的的另外一组数据(另外的其他桥)，原样拷贝
			//printf("6-name_id:%d\n",old_nodes->p_policyinfo[index].name);
			copy_httpdirpolicys_value(new_nodes, old_nodes, new_index, index, -1);
		}
		
	}

#if 0
	for (index=0; index < new_nodes->policy_num; index++){
		printf("name:%d, extgroup:%s, bridge:%d\n", new_nodes->p_policyinfo[index].name,
									new_nodes->p_policyinfo[index].extgroup,
									new_nodes->p_policyinfo[index].bridge);
	}
#endif 

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
SUCCESS_EXIT:
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return;
}

void adsl_status_log_empty(void)
{
       char                     cmd[STRING_LENGTH]={0};
       int                        iserror = 1;
       FILE			      *pp = NULL;

       snprintf(cmd,sizeof(cmd),"/bin/rm -rf %s 2>/dev/null",PPPD_ADSL_LOG_FILE_PATH);
       pp = popen(cmd,"r");
	if(NULL == pp)
	{
		 goto exit;
	}

	iserror = 0;
       pclose(pp);
exit:
       printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
       return;
}

void url_filter_move_save(void)
{
	const char 		*cmd="/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh url_filter remove";
	char				buf[STRING_LENGTH] = {0};
	FILE				*pp = NULL;
	int				iserror = 1;

	pp = popen(cmd,"r");
	if(NULL == pp)
	{
		goto exit;
	}

	fgets(buf,sizeof(buf),pp);
	buf[strlen(buf) - 1] = '\0';
	iserror = atoi(buf);
       pclose(pp);
exit:
	printf("{\"iserror\":\"%d\",\"msg\":\"\"}",iserror);
	return;
}


/*************************** URL显示, 编辑 **************************************/
void urlformats_edit_show(void)
{
	int size=0,index=0;
	char bridge_str[DIGITAL_LENGTH];
	char format[STRING_LENGTH]={""};
	char comment[STRING_LENGTH]={""};
	int bridge=0,iserror=0;
	
	struct mgtcgi_xml_http_dir_policys * httpdirpolicys = NULL;	

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);


	if (strlen(bridge_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	bridge	= atoi(bridge_str);
	if (bridge < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httpdirpolicys,&size);
	if ( NULL == httpdirpolicys){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(format, 0, sizeof(format));
	memset(comment, 0, sizeof(comment));
	for (index=0; index < httpdirpolicys->format_num; index++){
		if (httpdirpolicys->p_urlinfo[index].bridge == bridge){
			strcpy(format, httpdirpolicys->p_urlinfo[index].format);	
			strcpy(comment, httpdirpolicys->p_urlinfo[index].comment);
			break;
		}
	}
	printf("{\"data\":[{\"format\":\"%s\",\"comment\":\"%s\"}],", format, comment);

/******************************释放分配的内存********************************/	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(httpdirpolicys){
	    free_xml_node((void*)&httpdirpolicys);
        httpdirpolicys=NULL;
    }
	return;
}

void urlformats_edit_save(void)
{
	int index=0,new_size,old_size,iserror=0,result=0;
	int old_policy_num=0,old_format_num=0,
		new_policy_num=0,new_format_num=0;
	char bridge_str[DIGITAL_LENGTH];
	char format[STRING_LENGTH]={""};
	char comment[STRING_LENGTH]={""};
	int bridge=0,url_edit_flag=0;
	
	struct mgtcgi_xml_http_dir_policys *old_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policys *new_nodes = NULL; 
	struct mgtcgi_xml_http_dir_policy_urlformat_info *p_urlinfo_start = NULL;
	struct mgtcgi_xml_http_dir_policy_info *p_policyinfo_start = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("format",format,DIGITAL_LENGTH);
	cgiFormString("comment",comment,DIGITAL_LENGTH);

	if ((strlen(bridge_str) < STRING_LITTLE_LENGTH) && 
		(strlen(format) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	bridge	= atoi(bridge_str);
	if (bridge < 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	url_edit_flag = 0;
	for (index=0; index < old_nodes->format_num; index++){
		if (old_nodes->p_urlinfo[index].bridge == bridge){
			strcpy(old_nodes->p_urlinfo[index].format, format);
			strcpy(old_nodes->p_urlinfo[index].comment, comment);
			url_edit_flag = 1;
			break;
		}
	}

	if (url_edit_flag == 0){ //增加一条记录

		old_format_num = old_nodes->format_num;
		old_policy_num = old_nodes->policy_num;
		new_format_num = old_format_num + 1;
		new_policy_num = old_policy_num;

		old_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
				new_format_num * 
				sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
				new_policy_num * 
				sizeof(struct mgtcgi_xml_http_dir_policy_info);

		new_size = sizeof(struct mgtcgi_xml_http_dir_policys) + 
				new_format_num * 
				sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info) +
				new_policy_num * 
				sizeof(struct mgtcgi_xml_http_dir_policy_info);

		new_nodes = (struct mgtcgi_xml_http_dir_policys *)malloc_buf(new_size);
		if (NULL == new_nodes){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		memset(new_nodes, 0, sizeof(new_nodes));
		p_urlinfo_start = (struct mgtcgi_xml_http_dir_policy_urlformat_info *)
							((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys));
		p_policyinfo_start = (struct mgtcgi_xml_http_dir_policy_info *)
							((char *)new_nodes + sizeof(struct mgtcgi_xml_http_dir_policys) + 
							new_format_num * 
							sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));
		
		//修改dir_policy数量，指针
		new_nodes->format_num= new_format_num;
		new_nodes->policy_num = new_policy_num;
		new_nodes->p_urlinfo = p_urlinfo_start;
		new_nodes->p_policyinfo = p_policyinfo_start;

		//拷贝url设置并增加一条url
		for (index=0; index < old_nodes->format_num; index++){
			new_nodes->p_urlinfo[index].bridge = old_nodes->p_urlinfo[index].bridge;
			strcpy(new_nodes->p_urlinfo[index].format, old_nodes->p_urlinfo[index].format);
			strcpy(new_nodes->p_urlinfo[index].comment, old_nodes->p_urlinfo[index].comment);
		}
		new_nodes->p_urlinfo[old_format_num].bridge = bridge;
		strcpy(new_nodes->p_urlinfo[old_format_num].format, format);
		strcpy(new_nodes->p_urlinfo[old_format_num].comment, comment);

		//原样复制http目录策略数据
		for (index=0; index < new_nodes->policy_num; index++){
			new_nodes->p_policyinfo[index].name = old_nodes->p_policyinfo[index].name;
			new_nodes->p_policyinfo[index].disabled = old_nodes->p_policyinfo[index].disabled;
			new_nodes->p_policyinfo[index].bridge = old_nodes->p_policyinfo[index].bridge;
			strcpy(new_nodes->p_policyinfo[index].extgroup, old_nodes->p_policyinfo[index].extgroup);
			strcpy(new_nodes->p_policyinfo[index].servergroup, old_nodes->p_policyinfo[index].servergroup);
			strcpy(new_nodes->p_policyinfo[index].comment, old_nodes->p_policyinfo[index].comment);
		}

		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)new_nodes, new_size);

	} else { // 修改

		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)old_nodes, old_size);
	}
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

/******************************释放分配的内存********************************/	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}



/***************************系统管理部分********************************/

void get_sys_bridge_num(void)
{
	int bridge_num = 0;
	bridge_num = get_bridge_num();
	printf("{\"iserror\":0,\"bridge_num\":%d,\"msg\":\"\"}",bridge_num);
	return;
}

void get_sys_interface_name(void)
{
	char bridgeid[STRING_LENGTH]={"0"};
	char data[CMD_LENGTH]={0};

	cgiFormString("bridgeid",bridgeid,STRING_LENGTH);
	
	get_interface_name(data, bridgeid);
	printf("%s",data);
	return;
}


void str_copy(char *p, char string[])
{
	int pos,i,len;
	pos = strcspn(p, "\"");
	len = strlen(p);
	for (i=0; pos < len; i++, pos++){
		string[i] = p[pos];
	}	
}
void device_version_show(void)
{
	FILE *version_fd;
	char buffer[CMD_LENGTH];
	char *p;
	int line, pos,iserror=0;
	char name[STRING_LENGTH]={0};
	char version[STRING_LENGTH]={0};
	char series[STRING_LENGTH]={0};
	char vendor[STRING_LENGTH]={0};
	char model[STRING_LENGTH]={0};
	char build_time[STRING_LENGTH]={0};
	
	if ((version_fd = fopen("/etc/version", "r")) == NULL){
		iserror = 3;
		goto EXIT;
	}

	line = 0;
	memset (buffer, 0, sizeof(buffer));
	while ((fgets(buffer, sizeof(buffer), version_fd) != NULL))
	{
		line++;
		p = buffer;

		if ((*p == '\n') || (*p == '#') || (*p == '\0'))
			continue;

		p[strlen(p)-1] = '\0';

		if ((pos = strcspn(p, "=")) == 0) {
			iserror = 3;
			goto EXIT;
		}
		
		strncpy(name, p, pos);
		name[pos] = '\0';
		if (strcmp(name, "VERSION") == 0){
			str_copy(p, version);
			//printf("version:%s\n", version);
		}
		else if (strcmp(name, "SERIES") == 0){
			str_copy(p, series);
			//printf("series:%s\n", series);
		}
		else if (strcmp(name, "VENDOR") == 0){
			str_copy(p, vendor);
			//printf("vendor:%s\n", vendor);
		}		
		else if (strcmp(name, "MODEL") == 0){
			str_copy(p, model);
			//printf("model:%s\n", model);
		}			
		else if (strcmp(name, "BUILD_TIME") == 0){
			str_copy(p, build_time);
			//printf("build_time:%s\n", build_time);
		}
		
		if (line > 1024)
			break;
		memset (buffer, 0, sizeof(buffer));
	}
	fclose(version_fd);

	EXIT:
		printf("{\"data\":[{\"version\":%s,\"series\":%s,\
\"vendor\":%s,\"model\":%s,\"build_time\":%s}],\"iserror\":%d,\"msg\":\"\"}",
version, series, vendor, model, build_time, iserror);
		return;
}

void l7proto_version_show(void)
{

	char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_l7proto_version");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}

void license_version_show(void)
{

	char 		cmd[CMD_LENGTH];
	char 		buffer[BUF_LENGTH_4K];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_license_info");

	fp=popen(cmd,"r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}


void firmware_version_show(void)
{

	char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_firmware_version");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}


void sys_date_info_show(void)
{

	char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/bin/echo \"{\\\"data\\\":[{\\\"year\\\":\\\"`/bin/date +%%Y`\\\",\
\\\"month\\\":\\\"`/bin/date +%%m`\\\",\
\\\"day\\\":\\\"`/bin/date +%%d`\\\",\
\\\"hour\\\":\\\"`/bin/date +%%H`\\\",\
\\\"minute\\\":\\\"`/bin/date +%%M`\\\",\
\\\"second\\\":\\\"`/bin/date +%%S`\\\"\
}],\\\"iserror\\\":0,\\\"msg\\\":\\\"\\\"}\"");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}

void sys_online_date_info_show(void)
{
    char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_sys_online_date");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}

void signature_auto_update_show(void)
{
    char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_auto_update_enable");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}

void ac_server_info_show(void)
{
       char 		cmd[CMD_LENGTH];
	char 		buffer[CMD_LENGTH];
	FILE 		*fp = NULL;

	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_ac_server_info");

	fp=popen(cmd,"r");
	fgets(buffer, CMD_LENGTH, fp);
	pclose(fp);

	printf("%s",buffer);
	return;
}
void sys_date_setting(void)
{
	int year,month,day,hour,minute,second,iserror=0;
	char cmd[CMD_LENGTH];
	char year_str[DIGITAL_LENGTH];
	char month_str[DIGITAL_LENGTH];
	char day_str[DIGITAL_LENGTH];
	char hour_str[DIGITAL_LENGTH];
	char minute_str[DIGITAL_LENGTH];
	char second_str[DIGITAL_LENGTH];

	cgiFormString("year",year_str,DIGITAL_LENGTH);
	cgiFormString("month",month_str,DIGITAL_LENGTH);
	cgiFormString("day",day_str,DIGITAL_LENGTH);
	cgiFormString("hour",hour_str,DIGITAL_LENGTH);
	cgiFormString("minute",minute_str,DIGITAL_LENGTH);
	cgiFormString("second",second_str,DIGITAL_LENGTH);

	if((strlen(year_str) < STRING_LITTLE_LENGTH) && 
		(strlen(month_str) < STRING_LITTLE_LENGTH) &&
		(strlen(day_str) < STRING_LITTLE_LENGTH)&& 
		(strlen(hour_str) < STRING_LITTLE_LENGTH) && 
		(strlen(minute_str) < STRING_LITTLE_LENGTH) &&
		(strlen(second_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	year = atoi(year_str);
	month = atoi(month_str);
	day = atoi(day_str);
	hour = atoi(hour_str);
	minute = atoi(minute_str);
	second = atoi(second_str);

	if (check_cgifrom_digit(year, 1999, 2050) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_cgifrom_digit(month, 1, 12) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_cgifrom_digit(day, 1, 31) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_cgifrom_digit(hour, 0, 23) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_cgifrom_digit(minute, 0, 59) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if (check_cgifrom_digit(second, 0, 59) == -1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/bin/date -s %s%s%s%s%s;/sbin/hwclock --systohc",
				month_str,day_str,hour_str,minute_str,year_str);

	system(cmd);
	
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	return;
}

void sys_update_url(void)
{
	int iserror = 0;
	char url_str[URL_LENGTH];
	char url[URL_LENGTH];
	char cmd[CMD_LENGTH];
	
	memset(url_str,'\0',URL_LENGTH);
	memset(url,'\0',URL_LENGTH);
	memset(cmd,'\0',CMD_LENGTH);
	cgiFormString("url",url_str,URL_LENGTH);
	
	if(strlen(url_str) != 0)
	{
		if(url_str[0] != 'h')
		{
			snprintf(url,URL_LENGTH,"%s%s","http://",url_str);
		}
		else
		{
			strncpy(url,url_str,URL_LENGTH);
		}

		snprintf(cmd,CMD_LENGTH,"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh update_url \"%s\"",url);
		iserror = do_sys_command(cmd);
		if (iserror != 0)
		{
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
	
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
	
FREE_EXIT:
	return;
}

void sys_reboot_setting(void)
{
	char cmd[CMD_LENGTH]={0};

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/bin/sync;/bin/sync;/sbin/reboot;/sbin/reboot");
	system(cmd);

	printf("{\"iserror\":0,\"msg\":\"\"}");
	return;
}

void sys_reset_setting(void)
{
       char cmd[CMD_LENGTH]={0};

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh sys_reset");
    do_sys_command(cmd);

	printf("{\"iserror\":0,\"msg\":\"\"}");
	return;
}

void sys_change_admin_passwd(void)
{
	char new_passwd[STRING_LENGTH]={0};
	char cmd[CMD_LENGTH]={0};
	int iserror=0;
	
	cgiFormString("new_passwd", new_passwd, sizeof(new_passwd));
	if (strlen(new_passwd) == 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh change_passwd %s", new_passwd);
	iserror = do_sys_command(cmd);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	return;
}

int import_file(const char *tmpfile_name, char file_name[])
{ 
    cgiFilePtr file; 
    int  tpmfile_fp;
    char import_file_name[STRING_LENGTH]={0};
	char filepath[512]={0};
    //char buffer[BUF_LENGTH]={0}; 
    char *buffer=NULL;
    char *tmpStr=NULL; 
    int file_size=0,read_size=0,write_size=0,tmp=0,iserror=0;

    //取得html页面中file元素的值，应该是文件在客户机上的路径名 
   if (cgiFormFileName("filepath", filepath, sizeof(filepath)) != cgiFormSuccess) {
		iserror = MGTCGI_IMPORT_FPATH_ERR;	//文件路径读取错误
        goto EXTI; 
    } 
    cgiFormFileSize("filepath", &file_size); 

    //取得文件类型
    //cgiFormFileContentType("file", contentType, sizeof(contentType)); 

    //目前文件存在于系统临时文件夹中，通常为/tmp，
    //通过该命令打开临时文件。临时文件的名字与用户文件的名字不同
	if (cgiFormFileOpen("filepath", &file) != cgiFormSuccess) { 
    	iserror = MGTCGI_IMPORT_FOPEN_ERR;	//文件打开错误
        goto EXTI; 
    } 

    tmp=-1; 
    //从路径名解析出用户文件名 
    while(1){ 
        tmpStr=strstr(filepath + tmp + 1,"\\"); 
        if(NULL==tmpStr) 
        tmpStr=strstr(filepath + tmp + 1,"/");//if "\\" is not path separator, try "/" 
        if(NULL!=tmpStr) 
            tmp = (int)(tmpStr-filepath); 
        else 
            break; 
    } 

    strcpy(import_file_name, filepath + tmp + 1); 
	strcpy(file_name, import_file_name);
	/*if (strcmp(import_file_name, file_name) != 0){
		iserror = 13;	//上传文件名错误
		goto EXTI;	
	}*/
	
	buffer = (char *)malloc(BUF_LENGTH * sizeof(char));
    if(!buffer)
    {
        cgiFormFileClose(file);
        goto EXTI;
    }
    memset(buffer,0,BUF_LENGTH*sizeof(char));
    //在当前目录下建立新的文件，第一个参数实际上是路径名，此处的含义是在cgi程序所在的目录（当前目录））建立新文件 
    tpmfile_fp=open(tmpfile_name, O_WRONLY | O_TRUNC | O_CREAT, 0755); 
    if(tpmfile_fp < 0){ 
		iserror = MGTCGI_IMPROT_FSAVE_ERR;		//文件保存错误
        goto EXTI; 

    } 
    
    //从系统临时文件中读出文件内容，并放到刚创建的目标文件中 
    while (cgiFormFileRead(file, buffer,sizeof(buffer), &read_size) == cgiFormSuccess){ 
        if(read_size>0) {
            write(tpmfile_fp, buffer, read_size); 
			write_size += read_size;
        }
    } 

	
    cgiFormFileClose(file); 
    close(tpmfile_fp); 

	if (write_size == file_size)
		iserror = 0;
	else
		iserror = MGTCGI_IMPROT_FSAVE_ERR;	//文件保存错误

EXTI:
    if(buffer!=NULL){
        free(buffer);
        buffer = NULL;
    }
	return (iserror);
} 

int export_file(const char *file_name, const char *export_name)
{
	FILE *fp = NULL; 
	//char buf[BUF_LENGTH]={0};
	char *buf=NULL;
	int  len = 0; 
	struct stat st;

	fp = fopen(file_name, "rb");
	if(NULL == fp) 
		return -1; 
	//memset (&st, 0x0, sizeof(struct stat));
	fstat(fileno(fp), &st);
	//stat("/etc/config.xml", &st);
	buf=(char *)malloc(BUF_LENGTH*sizeof(char));
    if(!buf){
        goto EXIT;
    }
    memset(buf,0,BUF_LENGTH*sizeof(char));
	printf("Content-Type:application/octet-stream\n"); 
	printf("Content-Length:%ld\n", st.st_size);
	printf("Content-Disposition:attachment;filename=%s\n\n", export_name); 

	while(!feof(fp)){ 
	len = fread(buf, 1, BUF_LENGTH-1, fp);
	if (len < 0 )
		break;

	fwrite(buf, 1, len, stdout);
	if(fflush(stdout))
		break; 
	} 

EXIT:
	fclose(fp);
	fflush(stdout);
    if(buf){
        free(buf);
        buf = NULL;
    }
	return 0; 
}


int recons_system_interface(const char *xmlfilename)
{
	int index=0,iserror=0,i,j,result=0,count=0,new_size=0;
	char value[BUF_LENGTH_4K]={"eth0,1,0,auto,100,full;eth1,1,0,auto,100,full;"};
	char tmpstr[STRING_LENGTH]={"0"};
	int speed=0,linkstate=0,bridgeid=0;
	int comma_num=0,semicolon_num=0,mgtnum=0;
	struct mgtcgi_xml_interfaces *new_nodes = NULL;

	char	cmd[CMD_LENGTH]={0};
	char	buffer[BUF_LENGTH_4K]={0};
	FILE	*pp;
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh recons_ifconfig %s", xmlfilename);
	if( (pp = popen(cmd, "r")) != NULL ){
		fgets(buffer, sizeof(buffer), pp);
		strncpy(value, buffer, sizeof(buffer));
		value[strlen(buffer)-1]='\0';
	}
	pclose(pp); 

	count = check_interface_list(value);
	if (count < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	new_size = sizeof(struct mgtcgi_xml_interfaces) + 
			count * 
			sizeof(struct mgtcgi_xml_interface_include_info);

	new_nodes = (struct mgtcgi_xml_interfaces *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, new_size);
	new_nodes->num = count;
	comma_num=0;
	semicolon_num=0;
	memset(tmpstr, 0, sizeof(tmpstr));
	for (index=0,i=0,j=0; i<strlen(value); i++){
		//eth0,1,0,auto,100
		if (value[i] == ','){
			if (comma_num == 0){	//name
				strcpy(new_nodes->pinfo[index].name, tmpstr);
			}
			else if (comma_num == 1){	//linkstate
				linkstate = atoi(tmpstr);
				new_nodes->pinfo[index].linkstate = linkstate;
			}
			else if (comma_num == 2){	//bridgeid
				bridgeid = atoi(tmpstr);
				new_nodes->pinfo[index].bridgeid = bridgeid;
			}
			else if (comma_num == 3){	//mode
				strcpy(new_nodes->pinfo[index].mode, tmpstr);	
			}
			else if (comma_num == 4){	//speed
				speed = atoi(tmpstr);
				new_nodes->pinfo[index].speed = speed;
			}
			else {
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num++;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (value[i] == ';'){
			if (comma_num == 5){	//duplex
				strcpy(new_nodes->pinfo[index].duplex, tmpstr);
				index++;
			}

			semicolon_num++;
			if (semicolon_num > new_nodes->num){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
			
			comma_num=0;
			j = 0;
			memset(tmpstr, 0, sizeof(tmpstr));
			continue;
		}
		
		if (j < sizeof(tmpstr))
			tmpstr[j] = value[i];
		j++;
	}

	//参数验证
	mgtnum = 0;
	for (i=0; i < new_nodes->num - 1; i++){
		for(j=i+1; j<new_nodes->num; j++){
			//同一个桥只运行配置一个内网口
			if((new_nodes->pinfo[i].bridgeid == new_nodes->pinfo[j].bridgeid) &&
				(new_nodes->pinfo[i].linkstate == 1) && 
				(new_nodes->pinfo[j].linkstate == 1)){ 
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
		//必须有一个管理口
		if(new_nodes->pinfo[i].linkstate == 3){ 
			mgtnum++;
		}
	}
	if (mgtnum != 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

#if 0
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].name = %s\n", index, new_nodes->pinfo[index].name);
		printf("new_nodes->pinfo[%d].linkstate = %d\n", index, new_nodes->pinfo[index].linkstate);
		printf("new_nodes->pinfo[%d].bridgeid = %d\n", index, new_nodes->pinfo[index].bridgeid);
		printf("new_nodes->pinfo[%d].mode = %s\n", index, new_nodes->pinfo[index].mode);
		printf("new_nodes->pinfo[%d].speed = %d\n", index, new_nodes->pinfo[index].speed);
		printf("new_nodes->pinfo[%d].duplex = %s\n", index, new_nodes->pinfo[index].duplex);
	}
#endif
		 
	result = save_xml_node(xmlfilename, MGTCGI_TYPE_INTERFACES,(void*)new_nodes,new_size);
	if (result >= 0){
		iserror=0;
		//设置已经配置的最大桥数	
	}
	else {
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}

ERROR_EXIT:
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return iserror; 
}

void sys_xmconfig_import(void)
{
	int iserror = 0;
	char url[URL_LENGTH]={"0"};
	char cmd[CMD_LENGTH];
	char buffer[DIGITAL_LENGTH];
	char file_name[STRING_LENGTH]={0};
	const char *tmpfile_name="/tmp/import_xmlconf.tmp";
	const char *apply_xmlconf_cmd="/usr/local/httpd/htdocs/cgi-bin/sh/checkfile.sh appy_xmlconf";
	FILE *fp = NULL;

	cgiFormString("url", url, sizeof(url));

	iserror = import_file(tmpfile_name, file_name);
	if (iserror != 0)
		goto EXIT;

#if 0
	//修改接口信息
	iserror = recons_system_interface(tmpfile_name);
	if (iserror != 0){
		iserror = MGTCGI_IMPROT_FILE_BAD;
		goto EXIT;
	}
#endif
	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"%s %s",apply_xmlconf_cmd, tmpfile_name);

	fp=popen(cmd,"r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);

	iserror = atoi(buffer);
	if (iserror != 0)
		iserror = MGTCGI_IMPROT_FILE_BAD;

EXIT:
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
	//printf("Location:%s?iserror=%d\r\n\r\n\r\n",url,iserror);
	//log_debug("<script>window.location='%s?iserror=%d';</script>",url,iserror);
	return;
}

void sys_url_filter_import(void)
{
	const char 	*tmpfile_name="/tmp/url_filter.tmp";
	const char 	*apply_url_filter_cmd = "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh url_filter import";
	char 		file_name[STRING_LENGTH]={0};
	char			url[URL_LENGTH]= {0};
	char			cmd[CMD_LENGTH] = {0};
	char			buf[STRING_LENGTH] = {0};
	FILE			*fp = NULL;
	int			iserror = -1,stat;

	cgiFormString("url", url, sizeof(url));

	stat = import_file(tmpfile_name, file_name);
	if (stat != 0)
	{
		goto EXIT;
	}
	
	snprintf(cmd,sizeof(cmd),"%s %s",apply_url_filter_cmd,tmpfile_name);
	fp = popen(cmd,"r");
	if(NULL != fp)
	{
		fgets(buf,sizeof(buf),fp);
		buf[strlen(buf) - 1] = '\0';
		iserror = atoi(buf);
	}

EXIT:
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror);
	return;
}

void sys_service_update_inport(void)
{
    int iserror = 0;
    char url[URL_LENGTH]={"0"};
    char cmd[CMD_LENGTH];
    char file_name[STRING_LENGTH]={""};
    const char *tmpfile_name="/tmp/routeserver.tmp";

    cgiFormString("url", url, sizeof(url));

    iserror = import_file(tmpfile_name, file_name);
    if (iserror != 0)
    	goto EXIT;

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/checkfile.sh appy_routeserver %s", tmpfile_name);
    iserror = do_sys_command(cmd);
    if (iserror != 0){
    	iserror = MGTCGI_IMPROT_FILE_BAD;
    	goto EXIT;
    }
    
EXIT:
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
	return;
}

void url_filter_addsave(void)
{
	char		*path = "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh url_filter add";
	char		key[STRING_LENGTH] = {0};
	char		cmd[CMD_LENGTH] = {0};
	char		buffer[STRING_LENGTH] = {0};
	FILE		*pp = NULL;
	int		iserror = -1;

	cgiFormString("key", key, sizeof(key));
	
	snprintf(cmd,sizeof(cmd),"%s %s",path,key);
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
		{
			iserror = 0;
		}
	}
	pclose(pp);

	printf("{\"iserror\":\"%d\",\"msg\":\"\"}",iserror);
	return;
}

void sys_xmconfig_export(void)
{
	int iserror = 0;
	const char *export_name="config.xml";
	const char *file_name="/etc/config.xml";
	
	iserror = export_file(file_name, export_name);
	return;
}

int get_urlfilter_path_filename(char *filename)
{
	int retval=-1;
	char cmd[CMD_LENGTH]={0};
	char buf[CMD_LENGTH]={0};
	FILE *pp;
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh get urlfilter_filename");
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buf, sizeof(buf), pp);
		buf[strlen(buf)-1] = '\0';
		if (strlen(buf) < 5){
			retval = -1;
		}
		else{
			strcpy(filename, buf);
			retval = 0;
		}	
	}
	pclose(pp);
	
	return retval;
}

int del_urlfilter_path_filename(char *filename)
{
	int retval=0;
	char cmd[CMD_LENGTH]={0};
	FILE *pp;
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh remove urlfilter %s", filename);
	pp = popen(cmd, "r");
	pclose(pp);
	
	return retval;
}

void url_filter_export(void)
{
	const char 		*export_name="url_filter.txt";
	char 		file_name[CMD_LENGTH] = {"0"};
	int 				iserror = 0, retval;

	retval = get_urlfilter_path_filename(file_name);
	
	if (retval == 0){
		iserror=export_file(file_name, export_name);
		del_urlfilter_path_filename(file_name);
	}
	return;
}

int get_macbind_path_filename(char *filename)
{
	int retval=-1;
	char cmd[CMD_LENGTH]={0};
	char buf[CMD_LENGTH]={0};
	FILE *pp;
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh get macbind_filename");
	pp = popen(cmd, "r");
	if (pp != NULL){
		fgets(buf, sizeof(buf), pp);
		buf[strlen(buf)-1] = '\0';
		if (strlen(buf) < 5){
			retval = -1;
		}
		else{
			strcpy(filename, buf);
			retval = 0;
		}	
	}
	pclose(pp);
	
	return retval;
}


int del_macbind_path_filename(char *filename)
{
	int retval=0;
	char cmd[CMD_LENGTH]={0};
	FILE *pp;
	
	snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh remove macbind %s", filename);
	pp = popen(cmd, "r");
	pclose(pp);
	
	return retval;
}


void sys_protoinfo_import()
{
	int iserror = 0,i;
	char url[URL_LENGTH]={"0"};
	char cmd[CMD_LENGTH];
	char buffer[DIGITAL_LENGTH];
	char file_name[STRING_LENGTH]={""};
	char md5[STRING_LENGTH]={""};
	char md5_str[STRING_LENGTH]={""};
	const char *tmpfile_name="/tmp/import_protoinfo.tmp";
	const char *apply_xmlconf_cmd="/usr/local/httpd/htdocs/cgi-bin/sh/checkfile.sh appy_protoinfo";
	FILE *fp = NULL;

	cgiFormString("url", url, sizeof(url));
	cgiFormString("md5",md5,sizeof(md5));

	iserror = import_file(tmpfile_name, file_name);
	if (iserror != 0)
		goto EXIT;
		
	for(i = 0; i < strlen(md5); i++){
	    md5_str[i] = tolower(md5[i]);
	}
	
	memset(buffer, 0, sizeof(buffer));
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"%s %s %s %s",apply_xmlconf_cmd, tmpfile_name, file_name, md5_str);

	fp=popen(cmd,"r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);

	iserror = atoi(buffer);
	if (iserror != 0)
		iserror = MGTCGI_IMPROT_FILE_BAD;
	
EXIT:
	printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
	return;

}

void sys_firmware_import()
{
		int iserror = 0,i;
		char url[URL_LENGTH]={"0"};
		char cmd[CMD_LENGTH];
		char buffer[DIGITAL_LENGTH];
		char file_name[STRING_LENGTH]={""};
		char md5[STRING_LENGTH]={""};
		char md5_str[STRING_LENGTH]={""};
		const char *tmpfile_name="/tmp/import_firmware.tmp";
		const char *apply_xmlconf_cmd="/usr/local/httpd/htdocs/cgi-bin/sh/checkfile.sh appy_firmware";
		FILE *fp = NULL;
	
		cgiFormString("url", url, sizeof(url));
		cgiFormString("md5",md5,sizeof(md5));
	
		iserror = import_file(tmpfile_name, file_name);
		if (iserror != 0)
			goto EXIT;
			
		for(i = 0; i < strlen(md5); i++){
			md5_str[i] = tolower(md5[i]);
		}
		
		memset(buffer, 0, sizeof(buffer));
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd),"%s %s %s %s",apply_xmlconf_cmd, tmpfile_name, file_name, md5_str);
	
		fp=popen(cmd,"r");
		fgets(buffer, sizeof(buffer), fp);
		pclose(fp);
	
		iserror = atoi(buffer);
		if (iserror != 0){
			iserror = MGTCGI_IMPROT_FILE_BAD;
            goto EXIT;
		}

EXIT:
		printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
		return;

}
void sys_license_import(void)
{
	int iserror = 0;
	char url[URL_LENGTH]={"0"};
	char cmd[CMD_LENGTH];
	char file_name[STRING_LENGTH]={""};
	const char *tmpfile_name="/tmp/license_key.tmp";

	cgiFormString("url", url, sizeof(url));

	iserror = import_file(tmpfile_name, file_name);
	if (iserror != 0)
		goto EXIT;

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/checkfile.sh appy_license %s", tmpfile_name);
	iserror = do_sys_command(cmd);
	if (iserror != 0){
		iserror = MGTCGI_IMPROT_FILE_BAD;
		goto EXIT;
	}

	EXIT:
		printf("<script>window.location='%s?iserror=%d';</script>",url,iserror); 
		return;
}


void print_groups_list(struct mgtcgi_xml_group_array * group)
{
	int index=0,index_end=0,i=0,type_num;
	int total = 0;

	total =  group->num;
	index = 0;
	index_end = group->num;
	
	printf("{\"data\":[");
	for (; index < index_end ;){
		type_num = group->pinfo[index].num;
		printf("{\"name\":\"%s\",\"objlist\":\"",group->pinfo[index].group_name);
		for(i=0; i< type_num;){
			printf("%s ",group->pinfo[index].pinfo[i].name);
			i++;			
		}
		index++;
		if (index < index_end){
			printf("\"},");
		}
		else{
			printf("\"}");
			break;
		}
	}
	printf("],");
	return;
}


void print_groups_name_list(struct mgtcgi_xml_group_array * group)
{
	int index=0,flag=0;

	flag = 0;
	printf("{\"group_name\":[");
	for (index=0; index < group->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",group->pinfo[index].group_name);
	}
	printf("],");
	return;
}



//流量通道，共享通道 列表显示函数
void channels_list_show(int channel)
{
	int size=0,index=0,index_end=0,flag=0,iserror=0;
	int total=0,bridge=0,bridge_num=0,traffic_control=0;
	int classtype1,classtype2,classtype3;
	char bridge_str[DIGITAL_LENGTH];
    //   char share_comment[STRING_LENGTH]={""};
	struct mgtcgi_xml_traffics *channels = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	if (strlen(bridge_str) < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	bridge	= atoi(bridge_str);
	if (!(0 <= bridge && bridge <=10)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size); 
	if ( NULL == channels){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	index = 0;
	index_end = channels->num;
	total = 0;
	bridge_num = 2;	// 桥数量要根据数据接口设置的桥信息确定
	traffic_control = channels->status;

	if (channel == 1){	//流量通道
		classtype1	= 3;
		classtype2	= 1;
		classtype3	= 2;
	}
	else if (channel == 2){	//共享通道
		classtype1	= 3;
		classtype2	= 0;
		classtype3	= 0;
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	flag = 0;
	printf("{\"data\":[");
	
	for (index=0; index < index_end; index++){
		if ((bridge == channels->pinfo[index].bridge) &&
			((channels->pinfo[index].classtype == classtype1) || 
			 (channels->pinfo[index].classtype == classtype2) ||
			 (channels->pinfo[index].classtype == classtype3))){
			total++;
			if (0 == flag)
				flag = 1; 
			else
				printf(",");
            
			printf("{\"direct\":%d,\"classname\":\"%s\",\"parentname\":\"%s\",\"sharename\":\"%s\",\"share_comment\":\"%s\",\
\"classtype\":%d,\"rate\":\"%s\",\"priority\":%d,\"limit\":%d,\"default_channel\":%d,\"attribute\":%d,\
\"weight\":%d,\"bridge\":%d,\"bandwidth\":\"%s\",\"comment\":\"%s\"}",\
				channels->pinfo[index].direct,channels->pinfo[index].classname,
				channels->pinfo[index].parentname,channels->pinfo[index].sharename,
				channels->pinfo[index].share_comment,
				channels->pinfo[index].classtype,channels->pinfo[index].rate,
				channels->pinfo[index].priority,channels->pinfo[index].limit,
				channels->pinfo[index].default_channel,channels->pinfo[index].attribute,
				channels->pinfo[index].weight,channels->pinfo[index].bridge,
				channels->pinfo[index].bandwidth,channels->pinfo[index].comment);	
		}
	}
	printf("],");
	
	bridge_num = get_bridge_num();
	printf("\"total\":%d,\"bridge_num\":%d,\"traffic_control\":%d,\"iserror\":0,\"msg\":\"\"}",\
		      total,bridge_num,traffic_control);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&channels);
	return;
}

int check_share_classname(struct mgtcgi_xml_traffics *channels, int bridge, 
								int direct, int check_classtype, const char *check_classname)
{
	int index=0,ret_value=0;
 	
	for (index=0; index < channels->num; index++){
		if ((channels->pinfo[index].bridge == bridge) &&
		(channels->pinfo[index].direct == direct) &&
		(channels->pinfo[index].classtype == check_classtype)&&
		(strcmp(channels->pinfo[index].sharename, check_classname) == 0)){//检查对应桥，上/下载通道的vc通道使用了该共享通道
			ret_value = 1;
			break;
		}
	}

	return (ret_value);
}


//打印共享通道列表
void print_share_list(struct mgtcgi_xml_traffics *channels, 
								int bridge, int direct, int check_classtype)
{
	int index=0,flag=0,share_flag=0;

	if (!(1 <= check_classtype && check_classtype <= 2))
		return;
	
	printf("\"share_list\":[");
	for (index=0; index < channels->num; index++){
		share_flag = 0;
		if ((channels->pinfo[index].bridge == bridge) &&
			(channels->pinfo[index].direct == direct) &&
			(channels->pinfo[index].classtype == 0)) {//确定桥，上/下载通道，共享通道

			//检查该共享通道名称是否可以打印
			share_flag = check_share_classname(channels, bridge, direct, check_classtype, 
												channels->pinfo[index].classname);	
			if (share_flag == 0){//没有被其他通道使用或在 同一个通道里。
				if (0 == flag){
					flag = 1;
					printf("[\"%s\",\"%s\"]",channels->pinfo[index].classname,channels->pinfo[index].comment);
				} else {
					printf(",[\"%s\",\"%s\"]",channels->pinfo[index].classname,channels->pinfo[index].comment);
				}
			}
		} 
	}
	printf("],");
}

//流量通道，共享通道 添加显示函数
// 1,流量通道
// 2,共享通道
void channels_add_show(int channel)
{
	int size=0,index=0,index_end=0,flag=0,iserror=0;
	int bridge,direct,classtype,check_classtype=0;
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char direct_str[DIGITAL_LENGTH]={"0"};
	char classtype_str[DIGITAL_LENGTH]={"1"};
	char classname_str[STRING_LENGTH]={"upload"};
	struct mgtcgi_xml_traffics *channels = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("direct",direct_str,DIGITAL_LENGTH);
	cgiFormString("classtype",classtype_str,DIGITAL_LENGTH);
	cgiFormString("classname",classname_str,STRING_LENGTH);

	if ((strlen(bridge_str) < 1) || (strlen(direct_str) < 1) ||
		 (strlen(classtype_str) < 1) || (strlen(classname_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	bridge	= atoi(bridge_str);
	direct	= atoi(direct_str);
	classtype	= atoi(classtype_str);
	if ((bridge < 0) || (direct < 0) || (classtype < 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (1 == channel){	//流量通道
		if  (!((classtype == 3) || (classtype == 1))){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}	
	}	
	else if (2 == channel){	//共享通道
		if (!(classtype == 3)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	//mgtcgi_type = MGTCGI_TYPE_TRAFFIC 
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size); ;
	if ( NULL == channels){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	index = 0;
	index_end = channels->num;
	
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		if ((bridge == channels->pinfo[index].bridge) &&
			(channels->pinfo[index].direct == direct) && 
			(channels->pinfo[index].classtype == classtype) &&
			(0 == strcmp(channels->pinfo[index].classname, classname_str))){
			
			printf("{\"parentname\":\"%s\",\"rate\":1,\"priority\":1,\"limit\":1,\
\"default_channel\":0,\"attribute\":0,\"weight\":1,\"comment\":\"%s\"}",channels->pinfo[index].classname,channels->pinfo[index].comment);

				break;
		}
	}
	printf("],");

	//显示共享通道列表
	if (channel == 1) {
		if (classtype == 3)//增加pipe通道，检查vc通道
			check_classtype = 2;
		
		else if (classtype == 1)//增加vc通道，检查pipe通道
			check_classtype = 1;

		if ((check_classtype == 1) || (check_classtype == 2))
			print_share_list(channels, bridge, direct, check_classtype);
	}
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&channels);
	return;
}


//流量通道，共享通道 编辑显示函数
void channels_edit_show(int channel)
{
	int size=0,index=0,index_end=0,flag=0,iserror=0;
	int bridge,direct,classtype=0,check_classtype=0;
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char direct_str[DIGITAL_LENGTH]={"1"};
	char classtype_str[DIGITAL_LENGTH]={"1"};
	char classname_str[STRING_LENGTH]={"web"};
       char parent_name[STRING_LENGTH]={"0"};
	struct mgtcgi_xml_traffics *channels = NULL;

	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("direct",direct_str,DIGITAL_LENGTH);
	cgiFormString("classtype",classtype_str,DIGITAL_LENGTH);
	cgiFormString("classname",classname_str,STRING_LENGTH);
	
	if ((strlen(bridge_str) < 1) || (strlen(direct_str) < 1) ||
		 (strlen(classtype_str) < 1) || (strlen(classname_str) < 1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	bridge	= atoi(bridge_str);
	direct	= atoi(direct_str);
	classtype	= atoi(classtype_str);
	if ((bridge < 0) || (direct < 0) || (classtype < 0)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC, (void**)&channels, &size); ;
	if ( NULL == channels){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	index = 0;
	index_end = channels->num;
	
	flag = 0;
	printf("{\"data\":[");
	for (index=0; index < index_end; index++){
		if ((bridge == channels->pinfo[index].bridge) &&
			(channels->pinfo[index].direct == direct) && 
			(channels->pinfo[index].classtype == classtype) &&
			(0 == strcmp(channels->pinfo[index].classname, classname_str))){

			flag = 1;
                            strcpy(parent_name,channels->pinfo[index].parentname);
			
			printf("{\"direct\":%d,\"classname\":\"%s\",\"parentname\":\"%s\",\"sharename\":\"%s\",\
\"classtype\":%d,\"rate\":\"%s\",\"priority\":%d,\"limit\":%d,\"default_channel\":%d,\"attribute\":%d,\
\"weight\":%d,\"bridge\":%d,\"bandwidth\":\"%s\",\"comment\":\"%s\"",\
				channels->pinfo[index].direct,channels->pinfo[index].classname,
				channels->pinfo[index].parentname,channels->pinfo[index].sharename,
				channels->pinfo[index].classtype,channels->pinfo[index].rate,
				channels->pinfo[index].priority,channels->pinfo[index].limit,
				channels->pinfo[index].default_channel,channels->pinfo[index].attribute,
				channels->pinfo[index].weight,channels->pinfo[index].bridge,
				channels->pinfo[index].bandwidth,channels->pinfo[index].comment);
				break;
		}
	}

       for (index=0; index < index_end; index++){
              if ((channels->pinfo[index].direct == direct) &&
                 (0 == strcmp(channels->pinfo[index].classname, parent_name))){
                        printf(",\"parent_comment\":\"%s\"}",channels->pinfo[index].comment);
                        break;
                }
       }
	printf("],");

	//显示共享通道列表
	if (channel == 1) {
		if (classtype == 1)//编辑pipe通道，检查vc通道
			check_classtype = 2;
		
		else if (classtype == 2)//编辑vc通道，检查pipe通道
			check_classtype = 1;

		if ((check_classtype == 1) || (check_classtype == 2))
			print_share_list(channels, bridge, direct, check_classtype);
		else
			printf("\"share_list\":[],");//root通道编辑
			
	}
	
	printf("\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
	free_xml_node((void*)&channels);
	return;
}

//通道增加保存
// 1,流量通道
// 2,共享通道
void channels_add_save(int channel)
{
	int index,old_size,new_size,parentname_flag;
	int old_num=0,new_num=0,default_flag;
	int direct,classtype,priority,limit,default_channel,attribute,weight,bridge;
	int iserror=0,result=0,n=0,len=0,i,j;
    int primary_t=0,slave1_t=0,slave2_t=0;
    char sign[4]={"0"};
	char direct_str[DIGITAL_LENGTH]={"1"}; 		//0表示上传upload,1表示下载download
	char classname[STRING_LENGTH]={"test2"};
	char parentname[STRING_LENGTH]={"download"};
	char sharename[STRING_LENGTH]={"none"};
	char classtype_str[DIGITAL_LENGTH]={"1"};
	char rate[STRING_LENGTH]={"100M"};
	char priority_str[DIGITAL_LENGTH]={"0"};
	char limit_str[DIGITAL_LENGTH]={"50"};
	char default_channel_str[DIGITAL_LENGTH]={"0"};		//是否作为默认通道
	char attribute_str[DIGITAL_LENGTH]={"0"};
	char weight_str[DIGITAL_LENGTH]={"1"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char bandwidth[STRING_LENGTH]={"0"};
	char comment[STRING_LENGTH]={"test"};
//    char *classname_t[3] = {"primary","slave1","slave2"};

	struct mgtcgi_xml_traffics * old_nodes = NULL; 
	struct mgtcgi_xml_traffics * new_nodes = NULL; 
	
	cgiFormString("direct",direct_str,DIGITAL_LENGTH);
	cgiFormString("classname",classname,STRING_LENGTH);
	cgiFormString("parentname",parentname,STRING_LENGTH);
	cgiFormString("sharename",sharename,STRING_LENGTH);
	cgiFormString("classtype",classtype_str,DIGITAL_LENGTH);
	cgiFormString("rate",rate,STRING_LENGTH);
	cgiFormString("priority",priority_str,DIGITAL_LENGTH);
	cgiFormString("limit",limit_str,DIGITAL_LENGTH);
	cgiFormString("default_channel",default_channel_str,STRING_LENGTH);
	cgiFormString("attribute",attribute_str,DIGITAL_LENGTH);
	cgiFormString("weight",weight_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("bandwidth",bandwidth,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(direct_str) < STRING_LITTLE_LENGTH)			
		|| (strlen(classtype_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	direct = atoi(direct_str);
	classtype = atoi(classtype_str);
	bridge = atoi(bridge_str);
	if (!((0 <= direct && direct <= 1) &&
			(0 <= classtype && classtype <= 3) &&
			(0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (3 == classtype){//root
		if(	(strlen(bandwidth) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	} else if (0 == classtype){//share
		if( (strlen(parentname) < STRING_LITTLE_LENGTH)			
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)){	//每个IP	
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	} else if (1 == classtype){//panrent
		if((strlen(parentname) < STRING_LITTLE_LENGTH)		
			|| (strlen(sharename) < STRING_LITTLE_LENGTH)		
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽
			|| (strlen(priority_str) < STRING_LITTLE_LENGTH)	//DSCP	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)		//在线IP数	
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)	//每个IP	
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	} else if (2 == classtype){//shell
		if((strlen(parentname) < STRING_LITTLE_LENGTH)		
			|| (strlen(sharename) < STRING_LITTLE_LENGTH)		
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽
			|| (strlen(priority_str) < STRING_LITTLE_LENGTH)	//DSCP	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)		//在线IP数	
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)	//每个IP	
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)
			|| (strlen(default_channel_str) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		
	} else {
		iserror =MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	priority = atoi(priority_str);
	limit = atoi(limit_str);
	default_channel = atoi(default_channel_str);
	attribute = atoi(attribute_str);
	weight = atoi(weight_str);
	
	if (!((0 <= priority && priority <= 256) &&
		(0 <= limit && limit <= 65535) &&
		(0 <= default_channel && default_channel <= 1) &&
		(0 <= attribute && attribute <= 1) &&
		(0 <= weight && weight <= 100))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

    memset(classname,0,sizeof(classname));
    if(classtype == 1){
        strcpy(classname,"pipe");
    }else if(classtype == 2){
        strcpy(classname,"vc");
    }
	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
    for(index=0;index<old_nodes->num;index++){
        if(old_nodes->pinfo[index].classtype != 0){
            continue;
        }
        if(old_nodes->pinfo[index].direct != direct){
            continue;
        }
        if(primary_t == 0){
            if(strcmp("primary",old_nodes->pinfo[index].classname) == 0){
                primary_t = 1;
            }
        }
        if(slave1_t == 0){
            if(strcmp("slave1",old_nodes->pinfo[index].classname) == 0){
                slave1_t = 1;
            }
        }
        if(slave2_t == 0){
            if(strcmp("slave2",old_nodes->pinfo[index].classname) == 0){
                slave2_t = 1;
            }
        }
    }

    if(channel == 2){
        memset(classname,0,sizeof(classname));
        if(primary_t == 0){
            strcpy(classname,"primary");
        }else if(slave1_t == 0){
            strcpy(classname,"slave1");
        }else if(slave2_t == 0){
            strcpy(classname,"slave2");
        }
    }
    
       for(index=0;index < old_nodes->num;index++){
              if(old_nodes->pinfo[index].direct == direct){
                    if(memcmp(comment,old_nodes->pinfo[index].comment,STRING_LENGTH-1) == 0){
                            iserror = MGTCGI_DUPLICATE_ERR;
		              goto ERROR_EXIT;
                    }
              }
       }

       if(classtype == 1 || classtype == 2){
            for(index=0;index < old_nodes->num;index++){
                    if ((old_nodes->pinfo[index].direct == direct) && 
                            (old_nodes->pinfo[index].classtype == classtype)){
                                len = strlen(old_nodes->pinfo[index].classname);
                                for(i=0,j=0;i < len;i++){
                                    if((old_nodes->pinfo[index].classname[i] >= '0') && 
                                        (old_nodes->pinfo[index].classname[i] <= '9')){
                                            sign[j] = old_nodes->pinfo[index].classname[i];
                                            j++;
                                    }
                                }

                                if(n < atoi(sign)){
                                    n = atoi(sign);
                                }
                                memset(sign,'\0',sizeof(sign));
                    }
            }   
            n++;
            sprintf(sign,"%d",n);
            strcat(classname,sign);
        }

	parentname_flag = 0;
	default_flag = 0;
	for (index=0; index < old_nodes->num; index++){
		if ((old_nodes->pinfo[index].bridge == bridge) &&
			(old_nodes->pinfo[index].direct == direct)){
			if ((0 == parentname_flag) && //父通道名称检查
				(0 == strcmp(old_nodes->pinfo[index].classname, parentname)))
				parentname_flag = 1;

			if (2 == classtype){
				if (1 == old_nodes->pinfo[index].default_channel)
					default_flag = 1;// 1 有默认通道
					
				if ((0 == old_nodes->pinfo[index].default_channel)
					&& (1 != default_flag))
					default_flag = 2;// 2 无默认通道
			}

		} 
	}
	if (0 == parentname_flag){	//没有父通道
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	if ((1 == default_flag) && (1 == default_channel))
		default_flag = 3;//删除旧的默认通道
			
	if ((2 == default_flag) && (1 != default_channel))
		default_flag = 4;//无默认通道，设置为默认通道标志，通常为第一条vc

	
	/*****************************重新构建数据结构****************************/
	old_num = old_nodes->num;
	new_num = old_num + 1;	

	old_size = sizeof(struct mgtcgi_xml_traffics) + 
			old_num * 
				sizeof(struct mgtcgi_xml_traffic_channel_info);
	new_size = sizeof(struct mgtcgi_xml_traffics) + 
			new_num * 
				sizeof(struct mgtcgi_xml_traffic_channel_info);

	new_nodes = (struct mgtcgi_xml_traffics *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	//通道顺序排列:
	//上传通道
	//root通道，父通道
	//共享通道
	//shell通道
	//vc通道
	//插入到最后
	memset(new_nodes, 0, sizeof(new_nodes));
	memcpy(new_nodes, old_nodes, old_size);
	new_nodes->num = new_num;
	//默认通道修改
	if ((2 == classtype) && (3 == default_flag)){//删除旧的默认通道
		for (index=0; index < old_nodes->num; index++){
			if ((new_nodes->pinfo[index].classtype == classtype) &&
				(new_nodes->pinfo[index].bridge == bridge) &&
				(new_nodes->pinfo[index].direct == direct) &&
				(new_nodes->pinfo[index].default_channel == 1)){

				new_nodes->pinfo[index].default_channel = 0;
			}				
		}
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	index = old_num;
	new_nodes->pinfo[index].direct = direct;
	new_nodes->pinfo[index].classtype = classtype;
	new_nodes->pinfo[index].priority = priority;
	new_nodes->pinfo[index].limit = limit;
	new_nodes->pinfo[index].default_channel = default_channel;
	new_nodes->pinfo[index].attribute = attribute;
	new_nodes->pinfo[index].weight = weight;
	new_nodes->pinfo[index].bridge = bridge;
	strcpy(new_nodes->pinfo[index].classname, classname);
	strcpy(new_nodes->pinfo[index].parentname, parentname);
	strcpy(new_nodes->pinfo[index].sharename, sharename);
	strcpy(new_nodes->pinfo[index].rate, rate);
	strcpy(new_nodes->pinfo[index].bandwidth, bandwidth);
	strcpy(new_nodes->pinfo[index].comment, comment);

	if ((2 == classtype) && (4 == default_flag)){//无默认通道，设置第一条vc为默认通道
		for (index=0; index < new_nodes->num; index++){
			if ((new_nodes->pinfo[index].classtype == classtype) &&
				(new_nodes->pinfo[index].bridge == bridge) &&
				(new_nodes->pinfo[index].direct == direct)){
				
				new_nodes->pinfo[index].default_channel = 1;
				break;
			}			
		}
	}

	result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void*)new_nodes, new_size);
	if (result < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
	printf("{\"data\":{\"classname\":\"%s\",\"comment\":\"%s\"},\"iserror\":0,\"msg\":\"\"}",classname,comment);
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}

extern int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,
					const char *name, const char *old_name, int check_id);

//通道增加编辑
// 1,流量通道
// 2,共享通道
void channels_edit_save(int channel)
{
	int index,old_size,size,parentname_flag;
	int direct,classtype,priority,limit,default_channel,attribute,
		weight,bridge,default_flag=0;
	int diff_name=0,iserror=6,ret_value=0,check_id=0,result=0;
	
	char old_classname[STRING_LENGTH]={"upload"};
	char direct_str[DIGITAL_LENGTH]={"0"}; 		//0表示上传upload,1表示下载download
	char classname[STRING_LENGTH]={"upload"};
	char parentname[STRING_LENGTH]={"root"};
	char sharename[STRING_LENGTH]={"none"};
	char classtype_str[DIGITAL_LENGTH]={"3"};
	char rate[STRING_LENGTH]={"100M"};
	char priority_str[DIGITAL_LENGTH]={"0"};
	char limit_str[DIGITAL_LENGTH]={"0"};
	char default_channel_str[DIGITAL_LENGTH]={"0"};		//是否作为默认通道
	char attribute_str[DIGITAL_LENGTH]={"0"};
	char weight_str[DIGITAL_LENGTH]={"1"};
	char bridge_str[DIGITAL_LENGTH]={"0"};
	char bandwidth[STRING_LENGTH]={"0"};
	char comment[STRING_LENGTH]={"test"};	

	struct mgtcgi_xml_traffics * old_nodes = NULL; 
	//关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("old_classname",old_classname,STRING_LENGTH);
	cgiFormString("direct",direct_str,DIGITAL_LENGTH);
	cgiFormString("classname",classname,STRING_LENGTH);
	cgiFormString("parentname",parentname,STRING_LENGTH);
	cgiFormString("sharename",sharename,STRING_LENGTH);
	cgiFormString("classtype",classtype_str,DIGITAL_LENGTH);
	cgiFormString("rate",rate,STRING_LENGTH);
	cgiFormString("priority",priority_str,DIGITAL_LENGTH);
	cgiFormString("limit",limit_str,DIGITAL_LENGTH);
	cgiFormString("default_channel",default_channel_str,STRING_LENGTH);
	cgiFormString("attribute",attribute_str,DIGITAL_LENGTH);
	cgiFormString("weight",weight_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);
	cgiFormString("bandwidth",bandwidth,STRING_LENGTH);
	cgiFormString("comment",comment,STRING_LENGTH);


	/******************************参数检查*****************************/
	if((strlen(direct_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(old_classname) < STRING_LITTLE_LENGTH)	
		|| (strlen(classname) < STRING_LITTLE_LENGTH)		
		|| (strlen(classtype_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	direct = atoi(direct_str);
	classtype = atoi(classtype_str);
	bridge = atoi(bridge_str);
	if (!((0 <= direct && direct <= 1) &&
		(0 <= classtype && classtype <= 3) &&
		(0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	if (3 == classtype){//root
		if(	(strlen(rate) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	} else if (0 == classtype){//share
		if( (strlen(parentname) < STRING_LITTLE_LENGTH)			
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)){	//每个IP	
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

	} else if (1 == classtype){//panrent
		if((strlen(parentname) < STRING_LITTLE_LENGTH)		
			|| (strlen(sharename) < STRING_LITTLE_LENGTH)		
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽
			|| (strlen(priority_str) < STRING_LITTLE_LENGTH)	//DSCP	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)		//在线IP数	
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)	//每个IP	
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
	} else if (2 == classtype){//shell
		if((strlen(parentname) < STRING_LITTLE_LENGTH)		
			|| (strlen(sharename) < STRING_LITTLE_LENGTH)		
			|| (strlen(rate) < STRING_LITTLE_LENGTH)			//最大带宽
			|| (strlen(priority_str) < STRING_LITTLE_LENGTH)	//DSCP	
			|| (strlen(limit_str) < STRING_LITTLE_LENGTH)		//在线IP数	
			|| (strlen(attribute_str) < STRING_LITTLE_LENGTH)	//每个IP	
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)
			|| (strlen(weight_str) < STRING_LITTLE_LENGTH)
			|| (strlen(default_channel_str) < STRING_LITTLE_LENGTH)){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		
	} else {
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	priority = atoi(priority_str);
	limit = atoi(limit_str);
	default_channel = atoi(default_channel_str);
	attribute = atoi(attribute_str);
	weight = atoi(weight_str);
	if (!((0 <= priority && priority <= 256) &&
		(0 <= limit && limit <= 65535) &&
		(0 <= default_channel && default_channel <= 1) &&
		(0 <= attribute && attribute <= 1) &&
		(0 <= weight && weight <= 100))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

       for(index=0;index < old_nodes->num;index++){
              if((old_nodes->pinfo[index].direct == direct) && 
                (strncmp(classname,old_nodes->pinfo[index].classname,STRING_LENGTH) != 0)){
                    if(memcmp(comment,old_nodes->pinfo[index].comment,STRING_LENGTH) == 0){
                            iserror = MGTCGI_DUPLICATE_ERR;
		              goto ERROR_EXIT;
                    }
              }
        }

	if (3 != classtype){
		parentname_flag = 0;
		default_flag = 0;
		for (index=0; index < old_nodes->num; index++){
			if ((old_nodes->pinfo[index].bridge == bridge) &&
				(old_nodes->pinfo[index].direct == direct)){
				if ((0 == parentname_flag) && //父通道名称有无检查
					(0 == strcmp(old_nodes->pinfo[index].classname, parentname))){
					parentname_flag = 1;
				}

				if (2 == classtype){
					if (1 == old_nodes->pinfo[index].default_channel)
						default_flag = 1;// 1 有默认通道
						
					if ((0 == old_nodes->pinfo[index].default_channel)
						&& (1 != default_flag))
						default_flag = 2;// 2 无默认通道
				}
				
			} 
		}

		if (0 == parentname_flag){	//没有父通道
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}

		if ((1 == default_flag) && (1 == default_channel))
			default_flag = 3;//删除旧的默认通道
			
		if ((2 == default_flag) && (1 != default_channel))
			default_flag = 4;//无默认通道，设置为默认通道标志，通常为第一条vc
		
		
	} else { //classtype=3，root通道检查
		if (strcmp(parentname, "root") != 0){
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		if (strcmp(classname, old_classname) != 0){//root通道不能修改名称
			iserror = MGTCGI_PARAM_ERR;
			goto ERROR_EXIT;
		}
		if (0 == direct){
			if (0 != strcmp(classname, "upload")){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		} else {
			if (0 != strcmp(classname, "download")){
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (strcmp(classname, old_classname) == 0){
		diff_name = 0;//名称没有被修改
	} else {
		diff_name = 1; //名称修改了，检测是否重复

		for (index=0; index < old_nodes->num; index++){
			//重名检查
			if ((old_nodes->pinfo[index].bridge == bridge) &&
			(old_nodes->pinfo[index].direct == direct)){
				if (0 == strcmp(old_nodes->pinfo[index].classname, classname)){//重名检查
					iserror = MGTCGI_DUPLICATE_ERR;
					goto ERROR_EXIT;
				}
		
				//名称是否被使用检查
				if ((0 == classtype) && 
					(0 != old_nodes->pinfo[index].classtype)){//share
					if (0 == strcmp(old_nodes->pinfo[index].sharename, old_classname)){//old_name被使用
						iserror = MGTCGI_USED_ERR;
						goto ERROR_EXIT;
					}
					
				} else if ((1 == classtype) && 
						   (2 == old_nodes->pinfo[index].classtype)){//parent
					if (0 == strcmp(old_nodes->pinfo[index].parentname, old_classname)){//old_name被vc使用
						iserror = MGTCGI_USED_ERR;
						goto ERROR_EXIT;
					}
				}	
			}
		}
	}

	if ((1 == diff_name) && (2 == classtype)){//检查vc是否被firewall使用
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		
		//应用层策略检查
		if (0 == direct)//upload
			check_id = 7; //out,upload,外出流量
		else
			check_id = 6; //in,download,进入流量
		
		ret_value = firewall_name_check(firewalls, classname, old_classname, check_id);
		if (0 != ret_value){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}


	//删除旧的默认通道
	//printf("default_flag:%d\n",default_flag);
	if ((2 == classtype) && (3 == default_flag)){
		for (index=0; index < old_nodes->num; index++){
			if ((old_nodes->pinfo[index].classtype == classtype) &&
				(old_nodes->pinfo[index].bridge == bridge) &&
				(old_nodes->pinfo[index].direct == direct) &&
				(old_nodes->pinfo[index].default_channel == 1)){

				old_nodes->pinfo[index].default_channel = 0;
			}				
		}
	}

	iserror = -1;
	for (index=0; index < old_nodes->num; index++){
		if ((old_nodes->pinfo[index].bridge == bridge) &&
			(old_nodes->pinfo[index].direct == direct) &&
			(strcmp(old_nodes->pinfo[index].classname, old_classname) == 0)){
				old_nodes->pinfo[index].direct = direct;
				old_nodes->pinfo[index].classtype = classtype;
				old_nodes->pinfo[index].priority = priority;
				old_nodes->pinfo[index].limit = limit;
				old_nodes->pinfo[index].default_channel = default_channel;
				old_nodes->pinfo[index].attribute = attribute;
				old_nodes->pinfo[index].weight = weight;
				old_nodes->pinfo[index].bridge = bridge;
				strcpy(old_nodes->pinfo[index].classname, classname);
				strcpy(old_nodes->pinfo[index].parentname, parentname);
				strcpy(old_nodes->pinfo[index].sharename, sharename);
				strcpy(old_nodes->pinfo[index].rate, rate);
				strcpy(old_nodes->pinfo[index].bandwidth, bandwidth);
				strcpy(old_nodes->pinfo[index].comment, comment);

				iserror = 0;
				break;
		}
	}

	if ((2 == classtype) && (4 == default_flag)){//无默认通道，设置第一条vc为默认通道
		for (index=0; index < old_nodes->num; index++){
			if ((old_nodes->pinfo[index].classtype == classtype) &&
				(old_nodes->pinfo[index].bridge == bridge) &&
				(old_nodes->pinfo[index].direct == direct)){

				old_nodes->pinfo[index].default_channel = 1;
				break;
			}			
		}
	}

#if 0
	printf("new_nodes->num:%d\n",old_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].direct:%d\n",index,old_nodes->pinfo[index].direct);
		printf("new_nodes->pinfo[%d].classtype:%d\n",index,old_nodes->pinfo[index].classtype);
		printf("new_nodes->pinfo[%d].priority:%d\n",index,old_nodes->pinfo[index].priority);
		printf("new_nodes->pinfo[%d].limit:%d\n",index,old_nodes->pinfo[index].limit);

		printf("new_nodes->pinfo[%d].default_channel:%d\n",index,old_nodes->pinfo[index].default_channel);
		printf("new_nodes->pinfo[%d].attribute:%d\n",index,old_nodes->pinfo[index].attribute);
		printf("new_nodes->pinfo[%d].weight:%d\n",index,old_nodes->pinfo[index].weight);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,old_nodes->pinfo[index].bridge);
		
		printf("new_nodes->pinfo[%d].classname:%s\n",index,old_nodes->pinfo[index].classname);
		printf("new_nodes->pinfo[%d].parentname:%s\n",index,old_nodes->pinfo[index].parentname);
		printf("new_nodes->pinfo[%d].sharename:%s\n",index,old_nodes->pinfo[index].sharename);
		printf("new_nodes->pinfo[%d].rate:%s\n",index,old_nodes->pinfo[index].rate);
		printf("new_nodes->pinfo[%d].bandwidth:%s\n",index,old_nodes->pinfo[index].bandwidth);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,old_nodes->pinfo[index].comment);
	}
#endif

	if (0 == iserror){
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void*)old_nodes, old_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	}
	else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}
		
/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(firewalls){
	    free_xml_node((void*)&firewalls);
        firewalls = NULL;
    }
	return;
}


//通道删除
// 1,流量通道
// 2,共享通道
void channels_remove_save(int channel)
{
	int index,old_size,new_size,size,result=0;
	int old_num=0,new_num=0;
	int direct,bridge,classtype;
	int iserror=6,ret_value=0,check_id=0;
	int remove_num,new_index,default_flag=0;
	
	char direct_str[DIGITAL_LENGTH]={"0"}; 		//0表示上传upload,1表示下载download
	char classname[STRING_LENGTH]={"aa"};
	char classtype_str[DIGITAL_LENGTH]={"0"};
	char bridge_str[DIGITAL_LENGTH]={"0"};

	struct mgtcgi_xml_traffics * old_nodes = NULL; 
	struct mgtcgi_xml_traffics * new_nodes = NULL; 
	//关联的结构定义
	struct mgtcgi_xml_firewalls *firewalls = NULL; //应用层策略

	cgiFormString("direct",direct_str,DIGITAL_LENGTH);
	cgiFormString("classname",classname,STRING_LENGTH);
	cgiFormString("classtype",classtype_str,DIGITAL_LENGTH);
	cgiFormString("bridge",bridge_str,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if((strlen(direct_str) < STRING_LITTLE_LENGTH)	
		|| (strlen(classname) < STRING_LITTLE_LENGTH)		
		|| (strlen(classtype_str) < STRING_LITTLE_LENGTH)
		|| (strlen(bridge_str) < STRING_LITTLE_LENGTH)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	direct = atoi(direct_str);
	classtype = atoi(classtype_str);
	bridge = atoi(bridge_str);
	if (!((0 <= direct && direct <= 1) &&
		(0 <= classtype && classtype <= 3) &&
		(0 <= bridge && bridge <= 256))){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void**)&old_nodes,&old_size);
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	if (old_nodes->num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	remove_num = -1;
	default_flag = 0;
	for (index=0; index < old_nodes->num; index++){
		if ((old_nodes->pinfo[index].classtype == classtype) &&
			(old_nodes->pinfo[index].bridge == bridge) &&
			(old_nodes->pinfo[index].direct == direct) &&
			(strcmp(old_nodes->pinfo[index].classname, classname) == 0)){
			remove_num = index;
			default_flag = old_nodes->pinfo[index].default_channel;
			break;
		}
	}


	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}


	for (index=0; index < old_nodes->num; index++){
		if ((old_nodes->pinfo[index].bridge == bridge) &&
			(old_nodes->pinfo[index].direct == direct)){

			if (0 == classtype){//share
				if ((old_nodes->pinfo[index].classtype != 0) &&
					(strcmp(old_nodes->pinfo[index].sharename, classname) == 0)){
					iserror = MGTCGI_USED_ERR;
					goto ERROR_EXIT;
				}
						
			} else if (1 == classtype){//pipe
				if ((old_nodes->pinfo[index].classtype == 2) &&
					(strcmp(old_nodes->pinfo[index].parentname, classname) == 0)){
					iserror = MGTCGI_USED_ERR;
					goto ERROR_EXIT;
				}

			} else if (2 == classtype){
				;//vc单独检查
			} else {
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}

	if (2 == classtype){
		get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLS, (void**)&firewalls, &size); ;
		if ( NULL == firewalls){
			iserror = MGTCGI_READ_FILE_ERR;
			goto ERROR_EXIT;
		}
		//应用层策略检查
		if (0 == direct)//upload
			check_id = 7; //out,upload,外出流量
		else
			check_id = 6; //in,download,进入流量
		ret_value = firewall_name_check(firewalls, classname, classname, check_id);
		if (0 != ret_value){
			if (1 == ret_value){
				iserror = MGTCGI_USED_ERR;
				goto ERROR_EXIT;
			}
			else{
				iserror = MGTCGI_PARAM_ERR;
				goto ERROR_EXIT;
			}
		}
	}
	
	/*************************可以删除，重新构建数据结构****************************/
	old_num = old_nodes->num;
	new_num = old_num - 1;
	if (new_num < 0)
		new_num = 0;
	
	old_size = sizeof(struct mgtcgi_xml_traffics) + 
			old_num * 
				sizeof(struct mgtcgi_xml_traffic_channel_info);
	new_size = sizeof(struct mgtcgi_xml_traffics) + 
			new_num * 
				sizeof(struct mgtcgi_xml_traffic_channel_info);
	
	new_nodes = (struct mgtcgi_xml_traffics *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	memset(new_nodes, 0, sizeof(new_nodes));
	new_nodes->num = new_num;
	new_nodes->status = old_nodes->status;

	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			new_nodes->pinfo[new_index].direct = old_nodes->pinfo[index].direct;
			new_nodes->pinfo[new_index].classtype = old_nodes->pinfo[index].classtype;
			new_nodes->pinfo[new_index].priority = old_nodes->pinfo[index].priority;
			new_nodes->pinfo[new_index].limit = old_nodes->pinfo[index].limit;
			new_nodes->pinfo[new_index].default_channel = old_nodes->pinfo[index].default_channel;
			new_nodes->pinfo[new_index].attribute = old_nodes->pinfo[index].attribute;
			new_nodes->pinfo[new_index].weight = old_nodes->pinfo[index].weight;
			new_nodes->pinfo[new_index].bridge = old_nodes->pinfo[index].bridge;
			strcpy(new_nodes->pinfo[new_index].classname, old_nodes->pinfo[index].classname);
			strcpy(new_nodes->pinfo[new_index].parentname, old_nodes->pinfo[index].parentname);
			strcpy(new_nodes->pinfo[new_index].sharename, old_nodes->pinfo[index].sharename);
			strcpy(new_nodes->pinfo[new_index].rate, old_nodes->pinfo[index].rate);
			strcpy(new_nodes->pinfo[new_index].bandwidth, old_nodes->pinfo[index].bandwidth);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			
			new_index++;
		}
	}

	if (1 == default_flag){//默认通道被删除，设置第一条vc为默认通道
		for (index=0; index < new_nodes->num; index++){
			if ((new_nodes->pinfo[index].classtype == 2) &&
				(new_nodes->pinfo[index].bridge == bridge) &&
				(new_nodes->pinfo[index].direct == direct)){

				new_nodes->pinfo[index].default_channel = 1;
				break;
			}			
		}
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	if (new_index == new_nodes->num){
		iserror = 0;
		result = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void*)new_nodes, new_size);
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if 0
	printf("new_nodes->num:%d\n",old_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].direct:%d\n",index,old_nodes->pinfo[index].direct);
		printf("new_nodes->pinfo[%d].classtype:%d\n",index,old_nodes->pinfo[index].classtype);
		printf("new_nodes->pinfo[%d].priority:%d\n",index,old_nodes->pinfo[index].priority);
		printf("new_nodes->pinfo[%d].limit:%d\n",index,old_nodes->pinfo[index].limit);

		printf("new_nodes->pinfo[%d].default_channel:%d\n",index,old_nodes->pinfo[index].default_channel);
		printf("new_nodes->pinfo[%d].attribute:%d\n",index,old_nodes->pinfo[index].attribute);
		printf("new_nodes->pinfo[%d].weight:%d\n",index,old_nodes->pinfo[index].weight);
		printf("new_nodes->pinfo[%d].bridge:%d\n",index,old_nodes->pinfo[index].bridge);
		
		printf("new_nodes->pinfo[%d].classname:%s\n",index,old_nodes->pinfo[index].classname);
		printf("new_nodes->pinfo[%d].parentname:%s\n",index,old_nodes->pinfo[index].parentname);
		printf("new_nodes->pinfo[%d].sharename:%s\n",index,old_nodes->pinfo[index].sharename);
		printf("new_nodes->pinfo[%d].rate:%s\n",index,old_nodes->pinfo[index].rate);
		printf("new_nodes->pinfo[%d].bandwidth:%s\n",index,old_nodes->pinfo[index].bandwidth);
		printf("new_nodes->pinfo[%d].comment:%s\n",index,old_nodes->pinfo[index].comment);
	}
#endif
/**************************成功，释放内存***************************/	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
    if(firewalls){
	    free_xml_node((void*)&firewalls);
        firewalls = NULL;
    }
	return;
}


void channels_enable_save(void)
{
	int enable = 1,new_size,ret_value,iserror=0;
	char enable_str[DIGITAL_LENGTH]={"1"};

	struct mgtcgi_xml_traffics * new_nodes = NULL; 

	cgiFormString("enable",enable_str,DIGITAL_LENGTH);

	/******************************参数检查*****************************/
	if(strlen(enable_str) < STRING_LITTLE_LENGTH){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	enable = atoi(enable_str);
	if (!(0 <= enable && enable <=1)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}

	/******************************获取原始数据*****************************/
	get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void**)&new_nodes,&new_size);
	if ( NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_nodes->status = enable;
	
	ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_TRAFFIC,(void*)new_nodes, new_size);

	/**************************成功，释放内存***************************/
	if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
	}
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
	return;
}


/************************ 资源管理 下拉列表 ****************************/
//ip地址对象(组)下拉列表:
void print_ipaddrs_list(
				struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups)
{
	int index=0,flag=0;
	printf("\"iplist\":[");
	for (index=0; index < networks->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",networks->pinfo[index].name);
	}

	for (index=0; index < networkgroups->num; index++){
		if (flag == 1){
			printf(",");
		}else{
			if (flag == 0)
				flag = 2;
			else
				printf(",");
		}
			
		printf("\"%s\"",networkgroups->pinfo[index].group_name);
	}
	printf("]");

}

//时间对象(组)列表
void print_schedules_list(
				struct mgtcgi_xml_schedules *schedules,
				struct mgtcgi_xml_group_array *schedulegroups)

{
	int index=0,flag=0;
	printf("\"schedulelist\":[");
	for (index=0; index < schedules->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",schedules->pinfo[index].name);
	}

	for (index=0; index < schedulegroups->num; index++){
		if (flag == 1){
			printf(",");
		}else{
			if (flag == 0)
				flag = 2;
			else
				printf(",");
		}
			
		printf("\"%s\"",schedulegroups->pinfo[index].group_name);
	}
	printf("]");

}

//VLAN对象列表
void print_vlans_list(struct mgtcgi_xml_vlans *vlans)
{
	int index=0,flag=0;
	printf("\"vlanlist\":[");
	for (index=0; index < vlans->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",vlans->pinfo[index].name);
	}
	printf("]");

}

//会话数下拉列表:
void print_sessions_list(
				struct mgtcgi_xml_session_limits *session_limits)
{
	int index=0,flag=0;
	printf("\"sessionlist\":[");
	for (index=0; index < session_limits->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",session_limits->pinfo[index].name);
	}
	printf("]");	

}

//domain对象(组)下拉列表:
void print_domains_list(
				struct mgtcgi_xml_domains * domains,
				struct mgtcgi_xml_group_array * domaingroups)

{
	int index=0,flag=0;
	printf("\"domainlist\":[");
	for (index=0; index < domains->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");
		
		printf("\"%s\"",domains->pinfo[index].name);
	}

	for (index=0; index < domaingroups->num; index++){
		if (flag == 1){
			printf(",");
		}else{
			if (flag == 0)
				flag = 2;
			else
				printf(",");
		}
			
		printf("\"%s\"",domaingroups->pinfo[index].group_name);
	}
	printf("]");	
}




//扩展名对象(组)下拉列表:
void print_extensions_list(
				struct mgtcgi_xml_http_dir_policys *httpdir_policy,
				struct mgtcgi_xml_group_array *extensiongroups,
				int bridge,
				const char *edit_extgroup)
{
	int index=0,flag=0,i=0,use_flag=0;
	printf("\"extensionlist\":[");

	if (edit_extgroup != NULL){
		printf("\"%s\"", edit_extgroup);
		flag = 1;
	} else {
		flag = 0;
	}
	for (index=0; index < extensiongroups->num; index++){
	use_flag = 0;

		for (i = 0; i < httpdir_policy->policy_num; i++){
			if ((bridge == httpdir_policy->p_policyinfo[i].bridge)&& 
				(strcmp(httpdir_policy->p_policyinfo[i].extgroup, 
				extensiongroups->pinfo[index].group_name) == 0)){
				use_flag = 1;
				break;
			}
		}

		if (use_flag == 0){
			if (flag == 0){
				printf("\"%s\"",extensiongroups->pinfo[index].group_name);
				flag = 1;
			} else {
				printf(",\"%s\"",extensiongroups->pinfo[index].group_name);
			}
		}
	}
	printf("]");	

}

//http服务器对象组下拉列表:
void print_httpservers_list(
				struct mgtcgi_xml_group_array *httpservergroups)
{
	int index=0,flag=0;
	printf("\"httpserverlist\":[");
	for (index=0; index < httpservergroups->num; index++){
		if (0 == flag)
			flag = 1;
		else
			printf(",");

			
		printf("\"%s\"",httpservergroups->pinfo[index].group_name);
	}
	printf("]");	
}


//进入流量通道对象列表
void print_inchannels_list(struct mgtcgi_xml_traffics *channels, char bridge)
{
	int index=0,flag=0;
	char direct_in=1,channel_valid=2;
    
    U8 bridge_id = atoi(&bridge);
	printf("\"inchannel_list\":[");
	for (index=0; index < channels->num; index++){
		if ((bridge_id == channels->pinfo[index].bridge) &&
				(channels->pinfo[index].direct == direct_in) && 
				(channels->pinfo[index].classtype == channel_valid)){

				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("[\"%s\",\"%s\"]",channels->pinfo[index].classname,channels->pinfo[index].comment);
		}
	}
	
	printf("]");
}


//外出流量通道对象列表
void print_outchannels_list(struct mgtcgi_xml_traffics *channels, char bridge)
{
	int index=0,flag=0;
	char direct_out=0,channel_valid=2;
    U8 bridge_id = atoi(&bridge);
    
	printf("\"outchannel_list\":[");
	for (index=0; index < channels->num; index++){
		if ((bridge_id == channels->pinfo[index].bridge) &&
				(channels->pinfo[index].direct == direct_out) && 
				    (channels->pinfo[index].classtype == channel_valid)){
				
				if (0 == flag)
					flag = 1;
				else
					printf(",");
				
				printf("[\"%s\",\"%s\"]",channels->pinfo[index].classname,channels->pinfo[index].comment);
		}
	}
	printf("]");
}

//网络层策略:添加、编辑下拉框
void ipsessions_add_edit_list(
				struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups,
				struct mgtcgi_xml_session_limits *session_limits)

{	
	//ip地址对象
	print_ipaddrs_list(networks, networkgroups);
	printf(",");

	//会话数对象
	print_sessions_list(session_limits);
	printf(",");
	
}


//防火墙策略:添加、编辑列表下拉框
void firewalls_add_edit_list(
				struct mgtcgi_xml_schedules *schedules,
				struct mgtcgi_xml_group_array *schedulegroups,
				struct mgtcgi_xml_vlans *vlans,
				struct mgtcgi_xml_networks *networks,
				struct mgtcgi_xml_group_array *networkgroups,
				struct mgtcgi_xml_session_limits *session_limits,
				struct mgtcgi_xml_traffics *channels,
				char bridge)

{
	//时间对象
	print_schedules_list(schedules, schedulegroups);
	printf(",");
	
	//vlan对象
	print_vlans_list(vlans);
	printf(",");
	
	//ip地址对象
	print_ipaddrs_list(networks, networkgroups);
	printf(",");

	//会话数对象
	print_sessions_list(session_limits);
	printf(",");
	
	//进入流量通道对象
	print_inchannels_list(channels, bridge);
	printf(",");

	//外出流量通道对象
	print_outchannels_list(channels, bridge);
	printf(",");

	return;
}

int groups_add_save(int mgtcgi_group_type, int add_includ_num,
						const char *name, const char *value, const char *comment)
{
	int old_size,new_size,size,index,i=0,ret_value,iserror=0;
	int old_total_group_num=0,old_total_inclue_num=0,
		new_total_group_num=0,new_total_inclue_num=0;
	char *dot=",";
	char *ptr;
	char *str = NULL;

	struct mgtcgi_xml_group_array *old_nodes = NULL;
	struct mgtcgi_xml_group_array *new_nodes = NULL;
	struct mgtcgi_xml_group_array *tmp_ptr = NULL;
	struct mgtcgi_xml_group_include_info *new_include_start = NULL; 

	str = (char *)malloc(STRING_LIST_LENGTH * sizeof(char));
	if (NULL == str){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	memset(str, 0, STRING_LIST_LENGTH * sizeof(char));
	memcpy(str, value, STRING_LIST_LENGTH * sizeof(char));

	/******************************重复性检查*****************************/
	if (mgtcgi_group_type == MGTCGI_TYPE_PROTOCOLS_GROUP){
		get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
						"english", (void**)&old_nodes,&size);
	}
	else if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES)
		get_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void**)&old_nodes,&old_size);

	else{
		get_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void**)&old_nodes,&old_size);
	}
		
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
		
	//组名不能重名
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].group_name, name)){
			iserror = MGTCGI_DUPLICATE_ERR;
			goto ERROR_EXIT;
		}	
	}
		
	#if 0
	log_debug("old_nodes->num:%d\n",old_nodes->num);
	for (index=0; index < old_nodes->num; index++){
		log_debug("old_nodes->pinfo[%d].group_name:%s\n",index,old_nodes->pinfo[index].group_name);
		log_debug("old_nodes->pinfo[%d].num:%d\n",index,old_nodes->pinfo[index].num);
		for (i=0; i < old_nodes->pinfo[index].num; i++){
			log_debug("old_nodes->pinfo[%d].pinfo[%d].name:%s\n",index,i,old_nodes->pinfo[index].pinfo[i].name);
		}
	}
	#endif
	
	/****************************重新构建新结构***************************/
	old_total_group_num = old_nodes->num;
	old_total_inclue_num = 0;
	for (index=0; index < old_nodes->num; index++){
		old_total_inclue_num += old_nodes->pinfo[index].num;
	}
	new_total_group_num = old_total_group_num + 1;
	new_total_inclue_num = old_total_inclue_num + add_includ_num;

	old_size = sizeof(struct mgtcgi_xml_group_array) + 
		old_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		old_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_size = sizeof(struct mgtcgi_xml_group_array) + 
		new_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		new_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_nodes = (struct mgtcgi_xml_group_array *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	tmp_ptr = new_nodes;
	memcpy(tmp_ptr, old_nodes, old_size);

    
	tmp_ptr->num = new_total_group_num;
	
	new_include_start = (struct mgtcgi_xml_group_include_info *)
						((char*)tmp_ptr + sizeof(struct mgtcgi_xml_group_array) + 
						new_total_group_num * 
						sizeof(struct mgtcgi_xml_group));	

	tmp_ptr->pinfo[old_total_group_num].num = add_includ_num;	//修改添加接收的include数量
	strcpy(tmp_ptr->pinfo[old_total_group_num].group_name, name);//修改添加的组名称
	strcpy(tmp_ptr->pinfo[old_total_group_num].comment, comment);
	//修改添加组 include的指针地址
	tmp_ptr->pinfo[old_total_group_num].pinfo = (struct mgtcgi_xml_group_include_info  *)
		((char*)new_include_start + 
			old_total_inclue_num * sizeof(struct mgtcgi_xml_group_include_info));

	//inclue赋值
	i = 0;
	if (add_includ_num > 0){
		//strcpy(str, value);
		ptr=strtok(str,dot);
		if ((NULL != ptr) && (i < add_includ_num)){
			strcpy(tmp_ptr->pinfo[old_total_group_num].pinfo[i].name, ptr);
			i++;
		}
		while((ptr=strtok(NULL,dot))){
			if ((NULL != ptr) && (i < add_includ_num)){
				strcpy(tmp_ptr->pinfo[old_total_group_num].pinfo[i].name, ptr);
				i++;
			} else {
				break;
			}
		}
	}

#if 0
	log_debug("\n\nnew_nodes->num:%d size=%d\n",new_nodes->num,new_size);
	for (index=0; index < tmp_ptr->num; index++){
		log_debug("tmp_ptr->pinfo[%d].group_name:%s\n",index,tmp_ptr->pinfo[index].group_name);
		log_debug("tmp_ptr->pinfo[%d].num:%d\n",index,tmp_ptr->pinfo[index].num);
		for (i=0; i < tmp_ptr->pinfo[index].num; i++){
			log_debug("tmp_ptr->pinfo[%d].pinfo[%d].name:%s\n",index,i,tmp_ptr->pinfo[index].pinfo[i].name);
		}
	}
#endif
	if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES){
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void*)new_nodes, new_size);
        log_debug("save_route_xml_node ret_value=%d \n",ret_value);
	} 
	else {
		ret_value = save_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void*)new_nodes, new_size);
	}
	if (ret_value < 0){
		iserror = 4;
		goto ERROR_EXIT;
	}
	
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }
    if(str){
        free(str);
        str = NULL;
    }
	return 0;
}

void sys_config_back_up(void)
{
    char cmd[CMD_LITTER_LENGTH] = {0};
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh back_up_config_xml");
    system(cmd);
	printf("{\"iserror\":0,\"msg\":\"\"}");
	return;  
}

void sys_config_restore(void)
{
    char cmd[CMD_LITTER_LENGTH] = {0};

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh restore_config_xml");
    system(cmd);
    printf("{\"iserror\":0,\"msg\":\"\"}");
	return;
}


int groups_edit_save(int mgtcgi_group_type, int add_includ_num, int diff_name,
							const char *name, const char *old_name, const char *value, const char *comment)
{
	int old_size,new_size,size,index,i,result=0;
	int old_total_group_num=0,old_total_inclue_num=0,
		new_total_group_num=0,new_total_inclue_num=0;
	int iserror = 7,include_count = 0, edit_include_num=0;
	char *dot=",";
	char *ptr;
	//char str[STRING_LIST_LENGTH];
	char * str = NULL;

	struct mgtcgi_xml_group_array *old_nodes = NULL;
	struct mgtcgi_xml_group_array *new_nodes = NULL;
	struct mgtcgi_xml_group_include_info *new_include_start = NULL; 

	/******************************重复性检查*****************************/
	if (mgtcgi_group_type == MGTCGI_TYPE_PROTOCOLS_GROUP){
		get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
						"english", (void**)&old_nodes,&size);
	}
	else if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES)
		get_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void**)&old_nodes,&old_size);

	else{
		get_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void**)&old_nodes,&old_size);
	}
		
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	
	//组名不能重名
	if (1 == diff_name){
		for (index=0; index < old_nodes->num; index++){
			if (0 == strcmp(old_nodes->pinfo[index].group_name, name)){
				iserror = MGTCGI_DUPLICATE_ERR;
				goto ERROR_EXIT;
			}
		}
	}
		
	#if 0
	printf("old_nodes->num:%d\n",old_nodes->num);
	for (index=0; index < old_nodes->num; index++){
		printf("old_nodes->pinfo[%d].group_name:%s\n",index,old_nodes->pinfo[index].group_name);
		printf("old_nodes->pinfo[%d].num:%d\n",index,old_nodes->pinfo[index].num);
		for (i=0; i < old_nodes->pinfo[index].num; i++){
			printf("old_nodes->pinfo[%d].pinfo[%d].name:%s\n",index,i,old_nodes->pinfo[index].pinfo[i].name);
		}
	}
	#endif

	/****************************重新构建新结构***************************/
	iserror = -1;
	old_total_group_num = old_nodes->num;
	old_total_inclue_num = 0;
	for (index=0; index < old_nodes->num; index++){
		old_total_inclue_num += old_nodes->pinfo[index].num;

		if(strcmp(old_nodes->pinfo[index].group_name, old_name) == 0){
			edit_include_num = old_nodes->pinfo[index].num;
			iserror = 0;
		}
	}
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

	
	new_total_group_num = old_total_group_num;
	new_total_inclue_num = old_total_inclue_num + add_includ_num - edit_include_num;
	if (new_total_inclue_num < 0){
		new_total_inclue_num = 0;
	}

	old_size = sizeof(struct mgtcgi_xml_group_array) + 
		old_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		old_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_size = sizeof(struct mgtcgi_xml_group_array) + 
		new_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		new_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_nodes = (struct mgtcgi_xml_group_array *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_include_start = (struct mgtcgi_xml_group_include_info *)
						((char*)new_nodes + sizeof(struct mgtcgi_xml_group_array) + 
						new_total_group_num * 
						sizeof(struct mgtcgi_xml_group));	

	//修改添加组 include的指针地址
	iserror = 7;
	include_count = 0;
	new_nodes->num = new_total_group_num;
    str = (char *) malloc(STRING_LIST_LENGTH * sizeof(char));
    if(!str){
        goto ERROR_EXIT;
    }
    memset(str,0,STRING_LIST_LENGTH * sizeof(char));
    
	for (index=0; index < new_nodes->num; index++){
		if ((0 == strcmp(old_nodes->pinfo[index].group_name, old_name)) && (7 == iserror)){	
			strcpy(new_nodes->pinfo[index].group_name, name);
			strcpy(new_nodes->pinfo[index].comment, comment);
			new_nodes->pinfo[index].num = add_includ_num;
			new_nodes->pinfo[index].pinfo = (struct mgtcgi_xml_group_include_info *)
			((char*)new_include_start + include_count * sizeof(struct mgtcgi_xml_group_include_info));
			
			strcpy(str, value);
			ptr=strtok(str,dot);
			i = 0;
			if (NULL != ptr){
				strcpy(new_nodes->pinfo[index].pinfo[i].name, ptr);
				i++;
			}
			while((ptr=strtok(NULL,dot))){
				if (NULL != ptr){
					strcpy(new_nodes->pinfo[index].pinfo[i].name, ptr);
					i++;
				}
			}
			
			iserror = 0;
			include_count += new_nodes->pinfo[index].num;
			
		}
        else {
		
			strcpy(new_nodes->pinfo[index].group_name, old_nodes->pinfo[index].group_name);
			strcpy(new_nodes->pinfo[index].comment, old_nodes->pinfo[index].comment);
			new_nodes->pinfo[index].num = old_nodes->pinfo[index].num;
			new_nodes->pinfo[index].pinfo = (struct mgtcgi_xml_group_include_info *)
			((char*)new_include_start + 
			include_count * sizeof(struct mgtcgi_xml_group_include_info));
			
			for (i=0; i < new_nodes->pinfo[index].num; i++)
				strcpy(new_nodes->pinfo[index].pinfo[i].name, old_nodes->pinfo[index].pinfo[i].name);

			include_count += old_nodes->pinfo[index].num;
		}
		
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
	if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if 0
	printf("\nnew_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].group_name:%s\n",index,new_nodes->pinfo[index].group_name);
		printf("new_nodes->pinfo[%d].num:%d\n",index,new_nodes->pinfo[index].num);
		for (i=0; i < new_nodes->pinfo[index].num; i++){
			printf("new_nodes->pinfo[%d].pinfo[%d].name:%s\n",index,i,new_nodes->pinfo[index].pinfo[i].name);
		}
	}
#endif

	if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES){
		result = save_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void*)new_nodes, new_size);
	} 
	else {
		result = save_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void*)new_nodes, new_size);
	}
	if (result < 0){
		iserror = 4;
		goto ERROR_EXIT;
	}
/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
	    free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

    if(str){
        free(str);
        str = NULL;
    }
	return result;
}


int groups_remove_save(int mgtcgi_group_type, const char *name)
{
	int old_size,new_size,size,index,i;
	int old_total_group_num=0,old_total_inclue_num=0,
		new_total_group_num=0,new_total_inclue_num=0;
	int include_count = 0;
	int iserror=7,result=0;
	int remove_num,new_index,remove_include_num=0;

	struct mgtcgi_xml_group_array *old_nodes = NULL;
	struct mgtcgi_xml_group_array *new_nodes = NULL;
	struct mgtcgi_xml_group_include_info *new_include_start = NULL; 


	/******************************重复性检查*****************************/
	if (mgtcgi_group_type == MGTCGI_TYPE_PROTOCOLS_GROUP){
		get_xml_nodeforLayer7(MGTCGI_XMLTMP, L7_XMLFILE, MGTCGI_TYPE_PROTOCOLS_GROUP,
						"english", (void**)&old_nodes,&size);
	}
	else if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES)
		get_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void**)&old_nodes,&old_size);

	else{
		get_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void**)&old_nodes,&old_size);
	}
		
	if ( NULL == old_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	if (old_nodes->num < 1){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	remove_num = -1;
	remove_include_num = 0;
	for (index=0; index < old_nodes->num; index++){
		if (0 == strcmp(old_nodes->pinfo[index].group_name, name)){
			remove_num = index;
			remove_include_num = old_nodes->pinfo[index].num;
			break;
		}
	}
	if (!((-1 != remove_num) && (remove_num < old_nodes->num))){//没有找到要删除的条目
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
		
	#if 0
	printf("old_nodes->num:%d\n",old_nodes->num);
	for (index=0; index < old_nodes->num; index++){
		printf("old_nodes->pinfo[%d].group_name:%s\n",index,old_nodes->pinfo[index].group_name);
		printf("old_nodes->pinfo[%d].num:%d\n",index,old_nodes->pinfo[index].num);
		for (i=0; i < old_nodes->pinfo[index].num; i++){
			printf("old_nodes->pinfo[%d].pinfo[%d].name:%s\n",index,i,old_nodes->pinfo[index].pinfo[i].name);
		}
	}
	#endif

	/****************************重新构建新结构***************************/
	old_total_group_num = old_nodes->num;
	old_total_inclue_num = 0;
	for (index=0; index < old_nodes->num; index++){
		old_total_inclue_num += old_nodes->pinfo[index].num;
	}
	new_total_group_num = old_total_group_num - 1;
	new_total_inclue_num = old_total_inclue_num - remove_include_num;

	old_size = sizeof(struct mgtcgi_xml_group_array) + 
		old_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		old_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_size = sizeof(struct mgtcgi_xml_group_array) + 
		new_total_group_num * 
			sizeof(struct mgtcgi_xml_group) +
		new_total_inclue_num * 
			sizeof(struct mgtcgi_xml_group_include_info);

	new_nodes = (struct mgtcgi_xml_group_array *)malloc_buf(new_size);
	if (NULL == new_nodes){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	new_include_start = (struct mgtcgi_xml_group_include_info *)
						((char*)new_nodes + sizeof(struct mgtcgi_xml_group_array) + 
						new_total_group_num * 
						sizeof(struct mgtcgi_xml_group));	

	//修改添加组 include的指针地址
	new_nodes->num = new_total_group_num;
	for (index=0,new_index=0; index < old_nodes->num; index++){
		if ((remove_num != index) && (new_index < new_nodes->num)){
			
			strcpy(new_nodes->pinfo[new_index].group_name, old_nodes->pinfo[index].group_name);
			strcpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment);
			new_nodes->pinfo[new_index].num = old_nodes->pinfo[index].num;
			new_nodes->pinfo[new_index].pinfo = (struct mgtcgi_xml_group_include_info *)
			((char*)new_include_start + 
			include_count * sizeof(struct mgtcgi_xml_group_include_info));

			for (i=0; i < old_nodes->pinfo[index].num; i++){
				strcpy(new_nodes->pinfo[new_index].pinfo[i].name, old_nodes->pinfo[index].pinfo[i].name);
			}

			include_count += new_nodes->pinfo[new_index].num;
			new_index++;
		}
	}
    if(old_nodes){
	    free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }

	if (new_index == new_nodes->num){	
		if (mgtcgi_group_type == MGTCGI_TYPE_ROUTE_TABLES){
			result = save_route_xml_node(ROUTE_CONFIG_FILE, mgtcgi_group_type,(void*)new_nodes, new_size);
		} 
		else {
			result = save_xml_node(MGTCGI_XMLTMP, mgtcgi_group_type,(void*)new_nodes, new_size);
		}
		
		if (result < 0){
			iserror = MGTCGI_WRITE_FILE_ERR;
			goto ERROR_EXIT;
		}
		iserror = 0;
	} else {
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
	}

#if 0
	printf("\nnew_nodes->num:%d\n",new_nodes->num);
	for (index=0; index < new_nodes->num; index++){
		printf("new_nodes->pinfo[%d].group_name:%s\n",index,new_nodes->pinfo[index].group_name);
		printf("new_nodes->pinfo[%d].num:%d\n",index,new_nodes->pinfo[index].num);
		for (i=0; i < new_nodes->pinfo[index].num; i++){
			printf("new_nodes->pinfo[%d].pinfo[%d].name:%s\n",index,i,new_nodes->pinfo[index].pinfo[i].name);
		}
	}
#endif

/**************************成功，释放内存***************************/
	printf("{\"iserror\":0,\"msg\":\"\"}");
	goto FREE_EXIT;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
	goto FREE_EXIT;
FREE_EXIT:
    if(old_nodes){
        free_xml_node((void*)&old_nodes);
        old_nodes = NULL;
    }
    if(new_nodes){
        free_xml_node((void *)&new_nodes);
        new_nodes = NULL;
    }

	return 0;
}

void signature_remote_update(void)
{
    int iserror = 0;
    char *buffer = NULL;
	const char *apply_xmlconf_cmd="/usr/local/sbin/upload_internet signature 0";
	FILE *fp = NULL;

	buffer = (char *)malloc(BUF_LENGTH * sizeof(char));
    if (buffer == NULL)
    {
        iserror = MGTCGI_IMPROT_FILE_BAD;
        goto EXIT;
    }
	memset(buffer, 0, BUF_LENGTH * sizeof(char));

	fp=popen(apply_xmlconf_cmd, "r");
    if (fp == NULL)
    {
        iserror = MGTCGI_IMPROT_FILE_BAD;
        goto EXIT;
    }
    
	fgets(buffer, BUF_LENGTH, fp);
	pclose(fp);

	iserror = atoi(buffer);
	if (iserror == 0)
	{
        goto EXIT;
	}

    if (iserror == 1)
    {
        iserror = MGTCGI_REMOTE_UPDATE_SERVER_CLOSE;
    }
    else if (iserror == 3)
    {
        iserror = MGTCGI_REMOTE_UPDATE_LAST_VER;
    }
    else if (iserror == 4)
    {
        iserror = MGTCGI_REMOTE_UPDATE_NO_ZONE;
    }
    else
    {
        iserror = MGTCGI_REMOTE_UPDATE_ARG_ERR;
    }
	
EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}", iserror);
    if (buffer)
    {
        free(buffer);
    }
    return;
}

void firmware_remote_update(void)
{
    int iserror = 0;
    char *buffer = NULL;
	const char *apply_xmlconf_cmd="/usr/local/sbin/upload_internet firmware";
	FILE *fp = NULL;

    buffer = (char *)malloc(BUF_LENGTH * sizeof(char));
    if (buffer == NULL)
    {
        iserror = MGTCGI_IMPROT_FILE_BAD;
        goto EXIT;
    }
	memset(buffer, 0, BUF_LENGTH * sizeof(char));

	fp=popen(apply_xmlconf_cmd, "r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);

	iserror = atoi(buffer);
	if (iserror == 0)
	{
        goto EXIT;
	}

    if (iserror == 1)
    {
        iserror = MGTCGI_REMOTE_UPDATE_SERVER_CLOSE;
    }
    else if (iserror == 3)
    {
        iserror = MGTCGI_REMOTE_UPDATE_LAST_VER;
    }
    else if (iserror == 4)
    {
        iserror = MGTCGI_REMOTE_UPDATE_NO_ZONE;
    }
    else
    {
        iserror = MGTCGI_REMOTE_UPDATE_ARG_ERR;
    }
    
EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}", iserror);
    if (buffer)
    {
        free(buffer);
    }
    return;
}

void signature_signature_autoupdate_cmd(void)
{
    int  iserror=1;
	int autoupdate=0;
	char autoupdate_str[STRING_LENGTH]={0};
	char cmd[CMD_LENGTH]={0};

	cgiFormString("auto", autoupdate_str, sizeof(autoupdate_str));
	autoupdate=atoi(autoupdate_str);
	if (autoupdate != 0)
		autoupdate = 1;

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/common.sh auto_update_enable_update %d", autoupdate);
    iserror = do_sys_command(cmd);
	if (iserror != 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
    printf("{\"iserror\":0,\"msg\":\"\"}");
	return;
ERROR_EXIT:
	printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    return;  
}

int apply_firewallpolicy(int firwall_enable){
    int ret_value=0;
	char cmd[CMD_LENGTH]={0};

	snprintf(cmd, sizeof(cmd), "%s/route_script.sh apply firewallpolicy \"enable=%u\"", route_script_path,firwall_enable);
	ret_value = do_sys_command(cmd);
	return ret_value;
}

