#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "include/md5.h"
#include "include/curl.h"
#include "include/utils.h"
#include "include/ac_json.h"
#include "include/ac_report.h"
#include "include/common.h"

#include "include/mgtcgi_xmlconfig.h"
#include "include/route_xmlwrapper4c.h"

#if 1
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

#define CURL_SETOPT(x,y,z) do {if(CURLE_OK != curl_easy_setopt(x,y,z)) goto exit;} while(0)

#define AC_REPORT_BUFFER_SIZE_MIN       1024
#define AC_REPORT_BUFFER_SIZE_MIDDLE    10240
#define AC_REPORT_BUFFER_SIZE_MAX       102400

#define AC_RESPOND_MAC_TIME_COUNT_MAX   4096
#define AC_RESPOND_IP_COUNT_MAX         4096
#define AC_RESPOND_APP_UNIT_COUNT_MAX   4096

#define AC_REPORT_IFADDR_LENGTH         256

struct http_respone_data
{
	unsigned char* buf;
	size_t buf_len;
	size_t data_len;
};

static void md5sum_build(unsigned char *src, char *md5sum)
{
    CMD5          md5;
	unsigned char md5_16[17] = {0};
    int i;

    DEBUGP("%s: begin, src: %s\n", __FUNCTION__, src);
    
    memset(md5_16, 0, 17);
    md5.MD5Update(src, strlen((char *)src));
	md5.MD5Final(md5_16);
	for(i = 0; i < 16; i++)
	{
		sprintf(md5sum + i * 2, "%02X", md5_16[i]);
	}

    DEBUGP("%s: finish, md5sum: %s\n", __FUNCTION__, md5sum);
}

static int md5sum_check(unsigned char *src, char *remote_md5sum)
{
    CMD5            md5;
    unsigned char   md5_16[17];
    char            md5sum[MD5SUM_LENGTH]={0};
    int             ret = -1;

    DEBUGP("%s: begin, src: %s, remote md5sum: %s\n", __FUNCTION__, src, remote_md5sum);
    
    memset(md5_16, 0, 17);
    memset(md5sum, 0, MD5SUM_LENGTH);
        
    md5.MD5Update(src, strlen((char *)src));
    md5.MD5Final(md5_16);
    for(int i = 0; i < 16; i++)
    {
        sprintf(md5sum + i * 2, "%02X", md5_16[i]);
    }

    if (memcmp(md5sum, remote_md5sum, 32) != 0)
    {
        goto exit;
    }

    ret = 0;
exit:
    DEBUGP("%s: finish, ret: %d, md5sum: %s remote_md5sum: %s\n", 
        __FUNCTION__, ret, md5sum, remote_md5sum);
    return ret;

}

int get_wan_ip(char *ip,int iplen,char *iface,int ifacelen)
{
    int             i,ret = -1,size;
    char            cmd[CMD_LEN] = {0};
    char            ifcmd[CMD_LEN] = {0};
    char            ip_str[STRING_LENGTH] = {0};
    char            ifname_str[STRING_LENGTH] = {0};
    struct          mgtcgi_xml_adsl_clients *adsl = NULL;
    char            *all_iface[3] = {"WAN0","WAN1","WAN2"};
    char            *commonsh = "/usr/local/httpd/htdocs/cgi-bin/sh/common.sh get_ip_by_interface";
    struct in_addr  inp;

    DEBUGP("%s: begin\n", __FUNCTION__);

    for(i=0;i<3;i++){
        snprintf(cmd,sizeof(cmd),"%s %s",commonsh,all_iface[i]);
        do_get_command(cmd,ip_str,sizeof(ip_str));
        if(inet_aton(ip_str,&inp) != 0){
            strncpy(ip,ip_str,iplen);
            strncpy(iface,all_iface[i],ifacelen);
            ret = 0;
            break;
        }
        memset(cmd,0,sizeof(cmd));
        memset(ip_str,0,sizeof(ip_str));
    }

    if(ret == 0){
        goto EXIT;
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	if (NULL == adsl){
		goto EXIT;
	}
    for(i=0;i<adsl->num;i++){
        snprintf(cmd,sizeof(cmd),"%s %s",commonsh,adsl->pinfo[i].name);
        do_get_command(cmd,ip_str,sizeof(ip_str));
        
        snprintf(ifcmd,sizeof(ifcmd),"/usr/local/sbin/pppoe-status /etc/ppp/%s.conf 2>/dev/null | grep \"peer\" | awk '{print $NF}'",adsl->pinfo[i].name);
        do_get_command(ifcmd,ifname_str,sizeof(ifname_str));
        if(inet_aton(ip_str,&inp) != 0){
            strncpy(ip,ip_str,iplen);
            strncpy(iface,ifname_str,ifacelen);
            ret = 0;
            break;
        }
        memset(cmd,0,sizeof(cmd));
        memset(ip_str,0,sizeof(ip_str));
    }

EXIT:
    DEBUGP("%s: finish,ip: [%s]\n", __FUNCTION__, ip);
    free_route_xml_node((void **)&adsl);
    return ret;
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

static void respond_common_extract(json_val_t *element, int *code, char *sign_buf)
{
	int i;

	switch (element->type) 
    {
    	case JSON_OBJECT_BEGIN:
		for (i = 0; i < element->length; i++) 
        {
            if (strcmp(element->u.object[i]->key, "code") == 0)
            {
                *code = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "returnSign") == 0)
            {
                strncpy(sign_buf, element->u.object[i]->val->u.data, MD5SUM_LENGTH);
            }
                
			respond_common_extract(element->u.object[i]->val, code, sign_buf);
		}
		break;
    	case JSON_ARRAY_BEGIN:
    		for (i = 0; i < element->length; i++)
            {
    			respond_common_extract(element->u.array[i], code, sign_buf);
    		}
    		break;
    	case JSON_FALSE:
    	case JSON_TRUE:
    	case JSON_NULL:
    	case JSON_INT:
    	case JSON_STRING:
    	case JSON_FLOAT:
    	default:
    		break;
	}
	return;
}

static void respond_state_extract(json_val_t *element, int *code, char *sign_buf, 
    int *mac_cnt, int *time_cnt, struct ac_respond_mac_time *mts)
{
	int i;

	switch (element->type) 
    {
    	case JSON_OBJECT_BEGIN:
		for (i = 0; i < element->length; i++) 
        {
            if (strcmp(element->u.object[i]->key, "code") == 0)
            {
                *code = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "returnSign") == 0)
            {
                strncpy(sign_buf, element->u.object[i]->val->u.data, MD5SUM_LENGTH);
            }
            else if (strcmp(element->u.object[i]->key, "mac") == 0)
            {
                mac_str_to_bin(element->u.object[i]->val->u.data, mts[*mac_cnt].mac);
                *mac_cnt = *mac_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "time") == 0)
            {
                mts[*time_cnt].time = atoi(element->u.object[i]->val->u.data);
                *time_cnt = *time_cnt + 1;
            }
                
			respond_state_extract(element->u.object[i]->val, code, sign_buf, mac_cnt, time_cnt, mts);
		}
		break;
    	case JSON_ARRAY_BEGIN:
    		for (i = 0; i < element->length; i++) 
            {
    			respond_state_extract(element->u.array[i], code, sign_buf, mac_cnt, time_cnt, mts);
    		}
    		break;
    	case JSON_FALSE:
    	case JSON_TRUE:
    	case JSON_NULL:
    	case JSON_INT:
    	case JSON_STRING:
    	case JSON_FLOAT:
    	default:
    		break;
	}
	return;
}

static void respond_getconfig_extract(json_val_t *element, int *code, char *sign_buf, 
    char *portal, int *cache_toplimit, int *wifi_conn_toplimit, int *timespan, int *mac_cnt, 
    int *time_cnt, int *ip_cnt, int *sip_cnt, struct ac_respond_mac_time *mts, unsigned int *ips, 
    unsigned int *sips)
{
    struct in_addr inaddr;
    int i;

	switch (element->type) 
    {
    	case JSON_OBJECT_BEGIN:
		for (i = 0; i < element->length; i++) 
        {
            if (strcmp(element->u.object[i]->key, "code") == 0)
            {
                *code = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "returnSign") == 0)
            {
                strncpy(sign_buf, element->u.object[i]->val->u.data, MD5SUM_LENGTH);
            }
            else if (strcmp(element->u.object[i]->key, "portal") == 0)
            {
                strncpy(portal, element->u.object[i]->val->u.data, APP_URL_LENGTH);
            }
            else if (strcmp(element->u.object[i]->key, "cacheToplimit") == 0)
            {
                *cache_toplimit = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "timeSpan") == 0)
            {
                *timespan = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "wifiConnectToplimit") == 0)
            {
                *wifi_conn_toplimit = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "mac") == 0)
            {
                mac_str_to_bin(element->u.object[i]->val->u.data, mts[*mac_cnt].mac);
                *mac_cnt = *mac_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "time") == 0)
            {
                mts[*time_cnt].time = atoi(element->u.object[i]->val->u.data);
                *time_cnt = *time_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "ip") == 0)
            {
                inet_aton(element->u.object[i]->val->u.data, &inaddr);
                ips[*ip_cnt] = inaddr.s_addr;
                *ip_cnt = *ip_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "sip") == 0)
            {
                inet_aton(element->u.object[i]->val->u.data, &inaddr);
                sips[*sip_cnt] = inaddr.s_addr;
                *sip_cnt = *sip_cnt + 1;
            }
                
			respond_getconfig_extract(element->u.object[i]->val, code, sign_buf, portal, 
                cache_toplimit, wifi_conn_toplimit, timespan, mac_cnt, time_cnt, ip_cnt, 
                sip_cnt, mts, ips, sips);
		}
		break;
    	case JSON_ARRAY_BEGIN:
    		for (i = 0; i < element->length; i++) 
            {
    			respond_getconfig_extract(element->u.array[i], code, sign_buf, portal, 
                    cache_toplimit, wifi_conn_toplimit, timespan, mac_cnt, time_cnt, ip_cnt, 
                    sip_cnt, mts, ips, sips);
    		}
    		break;
    	case JSON_FALSE:
    	case JSON_TRUE:
    	case JSON_NULL:
    	case JSON_INT:
    	case JSON_STRING:
    	case JSON_FLOAT:
    	default:
    		break;
	}
	return;
}

static void respond_getapp_extract(json_val_t *element, int *code, char *sign_buf, 
    int *appname_cnt, int *appsize_cnt, int *appid_cnt, int *appurl_cnt, int *appversion_cnt, 
    int *appstatus_cnt, int *appmd5sum_cnt, struct ac_respond_getapp_unit *app_units)
{
    
    int i;
    
    switch (element->type) 
    {
        case JSON_OBJECT_BEGIN:
        for (i = 0; i < element->length; i++) 
        {
            if (strcmp(element->u.object[i]->key, "code") == 0)
            {
                *code = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "returnSign") == 0)
            {
                strncpy(sign_buf, element->u.object[i]->val->u.data, MD5SUM_LENGTH);
            }
            else if (strcmp(element->u.object[i]->key, "app_name") == 0)
            {
               strncpy(app_units[*appname_cnt].app_name, element->u.object[i]->val->u.data, APP_NAME_LENGTH);
               *appname_cnt = *appname_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "app_url") == 0)
            {
               strncpy(app_units[*appurl_cnt].app_url, element->u.object[i]->val->u.data, APP_URL_LENGTH);
               *appurl_cnt = *appurl_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "app_version") == 0)
            {
               strncpy(app_units[*appversion_cnt].app_version, element->u.object[i]->val->u.data, APP_VERSION_LENGTH);
               *appversion_cnt = *appversion_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "app_size") == 0)
            {
                 app_units[*appsize_cnt].app_size = atoi(element->u.object[i]->val->u.data);
                 *appsize_cnt = *appsize_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "app_id") == 0)
            {
                 app_units[*appid_cnt].app_id = atoi(element->u.object[i]->val->u.data);
                 *appid_cnt = *appid_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "status") == 0)
            {
                 app_units[*appstatus_cnt].status = atoi(element->u.object[i]->val->u.data);
                 *appstatus_cnt = *appstatus_cnt + 1;
            }
            else if (strcmp(element->u.object[i]->key, "md5") == 0)
            {
               strncpy(app_units[*appmd5sum_cnt].md5sum, element->u.object[i]->val->u.data, MD5SUM_LENGTH);
               *appmd5sum_cnt = *appmd5sum_cnt + 1;
            }
                
            respond_getapp_extract(element->u.object[i]->val, code, sign_buf, appname_cnt, 
                appsize_cnt, appid_cnt, appurl_cnt, appversion_cnt, appstatus_cnt, 
                appmd5sum_cnt, app_units);
        }
        break;
        case JSON_ARRAY_BEGIN:
            for (i = 0; i < element->length; i++) 
            {
                respond_getapp_extract(element->u.array[i], code, sign_buf, appname_cnt, 
                    appsize_cnt, appid_cnt, appurl_cnt, appversion_cnt, appstatus_cnt, 
                    appmd5sum_cnt, app_units);
            }
            break;
        case JSON_FALSE:
        case JSON_TRUE:
        case JSON_NULL:
        case JSON_INT:
        case JSON_STRING:
        case JSON_FLOAT:
        default:
            break;
    }
    return;
}

static void respond_getportmap_extract(json_val_t *element,int *code,int *sport,int *dport, char *ip)
{
    int i;
    
    switch (element->type) 
    {
        case JSON_OBJECT_BEGIN:
        for (i = 0; i < element->length; i++) 
        {
            if (strcmp(element->u.object[i]->key, "code") == 0)
            {
                *code = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "localip") == 0)
            {
                strncpy(ip, element->u.object[i]->val->u.data, STRING_LENGTH-1);
            }
            else if (strcmp(element->u.object[i]->key, "sport") == 0)
            {
               *sport = atoi(element->u.object[i]->val->u.data);
            }
            else if (strcmp(element->u.object[i]->key, "dport") == 0)
            {
               *dport = atoi(element->u.object[i]->val->u.data);
            }
                
            respond_getportmap_extract(element->u.object[i]->val, code,sport,dport,ip);
        }
        break;
        case JSON_ARRAY_BEGIN:
            for (i = 0; i < element->length; i++) 
            {
                respond_getportmap_extract(element->u.object[i]->val, code,sport,dport,ip);
            }
            break;
        case JSON_FALSE:
        case JSON_TRUE:
        case JSON_NULL:
        case JSON_INT:
        case JSON_STRING:
        case JSON_FLOAT:
        default:
            break;
    }
    return;
}


/* process response from http server */
static int respond_setting_json_decode(char *data, int32_t data_len, 
    struct ac_respond_setting **respond)
{
    json_config                 config;
    json_val_t                  *element = NULL;
    
    struct ac_respond_setting   *my_respond = NULL;
    int                         code = -1;
    char                        sign_buf[MD5SUM_LENGTH] = {0};
    
    char                        *buf = NULL;
    int                         res, ret = -1;

    DEBUGP("%s: begin, data: %s, data_len: %d\n", __FUNCTION__, data, data_len);
    
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    my_respond = (struct ac_respond_setting *)malloc(sizeof(struct ac_respond_setting));
    if (my_respond == NULL)
    {
        DEBUGP("%s: alloc respond memory error\n", __FUNCTION__);
        goto exit;
    }
    memset(my_respond, 0, sizeof(struct ac_respond_setting));

    buf = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buf == NULL)
    {
        goto exit;
    }

    res = do_tree(&config, data, data_len, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto exit;
    }

    respond_common_extract(element, &code, sign_buf);
    if (code < 0 || strlen(sign_buf) != 32)
    {
        DEBUGP("%s: json string parser error\n", __FUNCTION__);
        goto exit;
    }

    snprintf(buf, AC_REPORT_BUFFER_SIZE_MIN, "%d%s", code, AC_REPORT_MAGIC);
    if (md5sum_check((unsigned char *)buf, sign_buf) != 0)
    {
        DEBUGP("%s: md5 checksum error\n", __FUNCTION__);
        goto exit;
    }
    
    my_respond->error = code;
    *respond = my_respond;
    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }
    
    if (ret != 0 && my_respond != NULL)
    {
        free(my_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int respond_state_json_decode(char *data, int32_t data_len, 
    struct ac_respond_state **respond)
{
    json_config                 config;
    json_val_t                  *element = NULL;
    char                        *buf = NULL;
    
    struct ac_respond_state     *my_respond = NULL;
    int                         respond_size = 0;
    
    int                         code = -1;
    char                        sign_buf[MD5SUM_LENGTH] = {0};
    int                         mac_cnt = 0;
    int                         time_cnt = 0;
    struct ac_respond_mac_time  *mts = NULL;
    
    int                         i, res, ret = -1;

    DEBUGP("%s: begin, data: %s, data_len: %d\n", __FUNCTION__, data, data_len);
    
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    buf = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buf == NULL)
    {
        goto exit;
    }

    mts = (struct ac_respond_mac_time *)malloc(sizeof(struct ac_respond_mac_time) * AC_RESPOND_MAC_TIME_COUNT_MAX);
    if (mts == NULL)
    {
        goto exit;
    }

    res = do_tree(&config, data, data_len, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto exit;
    }

    respond_state_extract(element, &code, sign_buf, &mac_cnt, &time_cnt, mts);
    if (code < 0 || mac_cnt != time_cnt)
    {
        goto exit;
    }

    snprintf(buf, AC_REPORT_BUFFER_SIZE_MIN, "%d%s", code, AC_REPORT_MAGIC);
    if (md5sum_check((unsigned char *)buf, sign_buf) != 0)
    {
        DEBUGP("%s: md5 checksum error\n", __FUNCTION__);
        goto exit;
    }

    respond_size = sizeof(struct ac_respond_state) + sizeof(struct ac_respond_mac_time) * mac_cnt;
    my_respond = (struct ac_respond_state *)malloc(respond_size);
    if (my_respond == NULL)
    {
        goto exit;
    }

    my_respond->error = code;
    my_respond->mac_time_count = mac_cnt;
    for (i = 0; i < mac_cnt; i++)
    {
        memcpy((char *)my_respond->mac_time[i].mac, (char *)mts[i].mac, MAC_ADDRESS_LENGTH);
        my_respond->mac_time[i].time = mts[i].time;
    }
    
    *respond = my_respond;
    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }

    if (mts)
    {
        free(mts);
    }
    
    if (ret != 0 && my_respond != NULL)
    {
        free(my_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int respond_warning_json_decode(char *data, int32_t data_len, 
    struct ac_respond_warning **respond)
{
    json_config                 config;
    json_val_t                  *element = NULL;
    struct ac_respond_warning   *my_respond = NULL;

    int                         code = -1;
    char                        sign_buf[MD5SUM_LENGTH] = {0};
    
    char                        *buf = NULL;
    int                         res, ret = -1;

    DEBUGP("%s: begin, data: %s, data_len: %d\n", __FUNCTION__, data, data_len);
    
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    buf = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buf == NULL)
    {
        goto exit;
    }

    res = do_tree(&config, data, data_len, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto exit;
    }

    respond_common_extract(element, &code, sign_buf);
    if (code < 0)
    {
        goto exit;
    }

    snprintf(buf, AC_REPORT_BUFFER_SIZE_MIN, "%d%s", code, AC_REPORT_MAGIC);
    if (md5sum_check((unsigned char *)buf, sign_buf) != 0)
    {
        DEBUGP("%s: md5 checksum error\n", __FUNCTION__);
        goto exit;
    }

    my_respond = (struct ac_respond_warning *)malloc(sizeof(struct ac_respond_warning));
    if (my_respond == NULL)
    {
        goto exit;
    }

    my_respond->error = code;
    *respond = my_respond;
    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }
    
    if (ret != 0 && my_respond != NULL)
    {
        free(my_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int respond_getconfig_json_decode(char *data, int32_t data_len, 
    struct ac_respond_getconfig **respond)
{
    json_config                 config;
    json_val_t                  *element = NULL;
    struct ac_respond_getconfig *my_respond = NULL;
    int                          respond_size = 0;

    int                         code = -1;
    char                        sign_buf[MD5SUM_LENGTH] = {0};
    char                        portal[APP_URL_LENGTH] = {0};
    char                        *pportal=NULL;
    int                         cache_toplimit = -1;
    int                         wifi_conn_toplimit = -1;
    int                         timespan = -1;
    int                         mac_cnt = 0;
    int                         time_cnt = 0;
    int                         ip_cnt = 0;
    int                         sip_cnt = 0;
    struct ac_respond_mac_time  *mts = NULL;
    unsigned int                *ips = NULL;
    unsigned int                *sips = NULL;

    struct ac_respond_mac_time  *pmt = NULL;
    unsigned int                *pip = NULL;
    unsigned int                *psip = NULL;
    
    char                        *buf = NULL;
    int                         i, res, ret = -1;

    DEBUGP("%s: begin, data: %s, data_len: %d\n", __FUNCTION__, data, data_len);
    
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    buf = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIDDLE);
    if (buf == NULL)
    {
        goto exit;
    }

    mts = (struct ac_respond_mac_time *)malloc(sizeof(struct ac_respond_mac_time)*AC_RESPOND_MAC_TIME_COUNT_MAX);
    if (mts == NULL)
    {
        goto exit;
    }

    ips = (unsigned int *)malloc(sizeof(unsigned int)*AC_RESPOND_IP_COUNT_MAX);
    if (ips == NULL)
    {
        goto exit;
    }

    sips = (unsigned int *)malloc(sizeof(unsigned int)*AC_RESPOND_IP_COUNT_MAX);
    if (sips == NULL)
    {
        goto exit;
    }

    res = do_tree(&config, data, data_len, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto exit;
    }

    respond_getconfig_extract(element, &code, sign_buf, portal, &cache_toplimit, 
        &wifi_conn_toplimit, &timespan, &mac_cnt, &time_cnt, &ip_cnt, &sip_cnt, 
        mts, ips, sips);
    
    if (code < 0)
    {
        goto exit;
    }

    if (mac_cnt != time_cnt)
    {
        goto exit;
    }

    if (code == 0)
    {
        snprintf(buf, AC_REPORT_BUFFER_SIZE_MIDDLE, "%d%d%d%s", 
            cache_toplimit, timespan, wifi_conn_toplimit, AC_REPORT_MAGIC);
    }
    else
    {
        snprintf(buf, AC_REPORT_BUFFER_SIZE_MIDDLE, "%d%s", code, AC_REPORT_MAGIC);
    }
    
    if (md5sum_check((unsigned char *)buf, sign_buf) != 0)
    {
        DEBUGP("%s: md5 checksum error\n", __FUNCTION__);
        goto exit;
    }

    respond_size = sizeof(struct ac_respond_getconfig)+
                   sizeof(struct ac_respond_mac_time)*mac_cnt+
                   sizeof(unsigned int)*ip_cnt+
                   sizeof(unsigned int)*sip_cnt;
    my_respond = (struct ac_respond_getconfig *)malloc(respond_size);
    if (my_respond == NULL)
    {
        goto exit;
    }

    my_respond->error = code;
    pportal = strstr(portal,"://");

    if(pportal){
        pportal += 3;
        strncpy(my_respond->portal, pportal, APP_URL_LENGTH);
    }
    else{
        strncpy(my_respond->portal, portal, APP_URL_LENGTH);
    }
    my_respond->cache_toplimit = cache_toplimit;
    my_respond->wifi_Conn_toplimit = wifi_conn_toplimit;
    my_respond->timespan = timespan;
    my_respond->mac_time_count = mac_cnt;
    my_respond->ip_count = ip_cnt;
    my_respond->sip_count = sip_cnt;
    my_respond->ips_offset = sizeof(ac_respond_mac_time)*mac_cnt;
    my_respond->sips_offset = my_respond->ips_offset + sizeof(unsigned int)*ip_cnt;
    
    pmt = (struct ac_respond_mac_time *)my_respond->buff;
    pip = (unsigned int *)(my_respond->buff + my_respond->ips_offset);
    psip = (unsigned int *)(my_respond->buff + my_respond->sips_offset);
    for (i = 0; i < mac_cnt; i++)
    {
        memcpy((char *)pmt[i].mac, (char *)mts[i].mac, MAC_ADDRESS_LENGTH);
        pmt[i].time = mts[i].time;
    }

    for (i = 0; i < ip_cnt; i++)
    {
        pip[i] = ips[i];
    }

    for (i = 0; i < sip_cnt; i++)
    {
        psip[i] = sips[i];
    }
    
    *respond = my_respond;
    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }

    if (mts)
    {
        free(mts);
    }

    if (ips)
    {
        free(ips);
    }

    if (sips)
    {
        free(sips);
    }
    
    if (ret != 0 && my_respond != NULL)
    {
        free(my_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int respond_getapp_json_decode(char *data, int32_t data_len, 
    struct ac_respond_getapp **respond)
{
    json_config                 config;
    json_val_t                  *element = NULL;
    struct ac_respond_getapp    *my_respond = NULL;
    int                         respond_size = 0;

    int                         code = -1;
    char                        sign_buf[MD5SUM_LENGTH] = {0};
    int                         appname_cnt = 0;    
    int                         appsize_cnt = 0;
    int                         appid_cnt = 0;
    int                         appurl_cnt = 0;
    int                         appversion_cnt = 0;
    int                         appstatus_cnt = 0;
    int                         appmd5sum_cnt = 0;
    struct ac_respond_getapp_unit *app_units = NULL;
    
    char                        *buf = NULL;
    int                         i, res, ret = -1;

    DEBUGP("%s: begin, data: %s, data_len: %d\n", __FUNCTION__, data, data_len);
    
	memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    buf = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buf == NULL)
    {
        goto exit;
    }

    res = do_tree(&config, data, data_len, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto exit;
    }

    app_units = (struct ac_respond_getapp_unit *)malloc(sizeof(struct ac_respond_getapp_unit) * AC_RESPOND_APP_UNIT_COUNT_MAX);
    if (app_units == NULL)
    {
        goto exit;
    }

    respond_getapp_extract(element, &code, sign_buf, &appname_cnt, &appsize_cnt, 
        &appid_cnt, &appurl_cnt, &appversion_cnt, &appstatus_cnt, &appmd5sum_cnt, app_units);
    if (code < 0)
    {
        goto exit;
    }

    if (appname_cnt != appsize_cnt || appsize_cnt != appid_cnt ||
        appid_cnt != appurl_cnt || appurl_cnt != appversion_cnt)
    {
        goto exit;
    }

    snprintf(buf, AC_REPORT_BUFFER_SIZE_MIN, "%d%s", code, AC_REPORT_MAGIC);
    if (md5sum_check((unsigned char *)buf, sign_buf) != 0)
    {
        DEBUGP("%s: md5 checksum error\n", __FUNCTION__);
        goto exit;
    }

    respond_size = sizeof(struct ac_respond_getapp) + sizeof(struct ac_respond_getapp_unit)*appname_cnt;
    my_respond = (struct ac_respond_getapp *)malloc(respond_size);
    if (my_respond == NULL)
    {
        goto exit;
    }

    my_respond->error = code;
    my_respond->unit_count = appname_cnt;
    for (i = 0; i < appname_cnt; i++)
    {
        strncpy(my_respond->units[i].app_name, app_units[i].app_name, APP_NAME_LENGTH-1);
        strncpy(my_respond->units[i].app_url, app_units[i].app_url, APP_URL_LENGTH-1);
        strncpy(my_respond->units[i].app_version, app_units[i].app_version, APP_VERSION_LENGTH-1);
        strncpy(my_respond->units[i].md5sum, app_units[i].md5sum, MD5SUM_LENGTH-1);
        my_respond->units[i].app_size = app_units[i].app_size;
        my_respond->units[i].app_id = app_units[i].app_id;
        my_respond->units[i].status = app_units[i].status;
    }

    DEBUGP("%s: unit count: %d\n", __FUNCTION__, my_respond->unit_count);
    *respond = my_respond;
    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }

    if (app_units)
    {
        free(app_units);
    }
    
    if (ret != 0 && my_respond != NULL)
    {
        free(my_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int respond_getportmap_json_decode(char *buffer, int datalen, struct ac_respond_getportmaps **respond)
{
    int                             res,ret = -1,total = 0,i = 0,sign = -1;
    int                             sport,dport,code,status;
    json_config                     config;
    json_val_t                      *element = NULL,*result;
    char                            localip[STRING_LENGTH] = {0};

    char                            get_key[STRING_LENGTH] = {0};
    char                            get_meg[STRING_LENGTH] = {0};
    struct ac_respond_getportmaps    *my_respond = NULL;

    DEBUGP("%s: begin, buffer: %s\n",
            __FUNCTION__,buffer);

    if(buffer == NULL || *respond != NULL){
        goto EXIT;
    }

    memset(&config, 0, sizeof(json_config));
	config.max_nesting = 0;
	config.max_data = 0;
	config.allow_c_comments = 1;
	config.allow_yaml_comments = 1;

    res = do_tree(&config, buffer, datalen, &element);
    if (res != 0 || element == NULL)
    {
        DEBUGP("%s: build dom error\n", __FUNCTION__);
        goto EXIT;
    }

    for (i = 0; i < element->length; i++) {
        if( strcmp(element->u.object[i]->key, "key") == 0 )
        {
            strncpy(get_key,element->u.object[i]->val->u.data,STRING_LENGTH-1);
        }
        if( strcmp(element->u.object[i]->key, "message") == 0 )
        {
            strncpy(get_meg,element->u.object[i]->val->u.data,STRING_LENGTH-1);
        }
	    if( strcmp(element->u.object[i]->key, "result") == 0 )
	    {
            result = element->u.object[i]->val;
	    }
        if( strcmp(element->u.object[i]->key, "status") == 0 )
	    {
            status = atoi(element->u.object[i]->val->u.data);
	    }
    }

    DEBUGP("%s:process, key:%s, message:%s, status:%d\n", __FUNCTION__,
            get_key,get_meg,status);


    if( status != 0 ){
	    goto EXIT;
    }

    for (i = 0; i < result->length; i++){
        if(strcmp(result->u.object[i]->key, "total") == 0)
        {
            total = atoi(result->u.object[i]->val->u.data);
        }
        if(strcmp(result->u.object[i]->key, "data") == 0){
            sign = i;
        }
    }

    if(sign < 0){
        goto EXIT;
    }
   
    my_respond = (struct ac_respond_getportmaps *)malloc(sizeof(struct ac_respond_getportmaps) + total*sizeof(struct ac_respond_getportmap));
    if(my_respond == NULL){
        goto EXIT;
    }
    memset(my_respond,0,sizeof(my_respond));
    my_respond->num = total;

    for(i = 0; i < total; i++){
	    respond_getportmap_extract(result->u.object[sign]->val->u.array[i],&code,&sport,&dport,localip);
	    my_respond->pinfo[i].code = code;
	    my_respond->pinfo[i].sport = sport;
	    my_respond->pinfo[i].dport = dport;
        strncpy(my_respond->pinfo[i].ip,localip,STRING_LENGTH-1);
    }
    *respond = my_respond;
    ret = 0;
   
EXIT:
    DEBUGP("%s:finish,respond: %p\n", __FUNCTION__,*respond);
    return ret;
}



/* process request to http server */
int do_ac_report_setting(char *ac_url, struct ac_report_setting *report, struct ac_respond_setting **respond)
{
	CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    CURLcode                    retval;
    int                         res, ret = -1;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, report, *respond);
    
    if (*respond != NULL)
    {
        goto exit;
    }

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIDDLE);
    if (buff == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIDDLE, "%s%s%d%d%d%s", 
        report->ac_id, 
        report->ap_id,
        report->cache_toplimit,
        report->wifi_conn_toplimit,
        report->timespan,
        AC_REPORT_MAGIC);
    
    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIDDLE, "acNum=%s&apNum=%s&\
cacheToplimit=%d&wifiConnectToplimit=%d&timeSpan=%d&sign=%s", 
        report->ac_id,
        report->ap_id,
        report->cache_toplimit,
        report->wifi_conn_toplimit,
        report->timespan,
        md5sum);
    
    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_SETTING_SUB_URL);
    DEBUGP("%s: ac report setting ifaddr: %s, postfield: %s\n", 
        __FUNCTION__, ifaddr, buff);
    
	CURL_SETOPT(my_curl, CURLOPT_POST, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
	CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
	
	retval = curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

    /* process respond */
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);

    res = respond_setting_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: parser respond setting json string error\n", __FUNCTION__);
        goto exit;
    }
    
	ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }

    if (rdata.buf)
    {
        free(rdata.buf);
    }
    
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_ac_report_portal(char *ac_url, struct ac_report_portal *report)
{
    CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    char                        *commit_url = NULL;
    CURLcode                    retval;
    int                         ret = -1;

    DEBUGP("%s: begin, report: %p\n", __FUNCTION__, report);

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buff == NULL)
    {
        goto exit;
    }

    commit_url = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (commit_url == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "%s"MACQUAD_FMT"%s", 
        report->ac_id, 
        MACQUAD(report->app_mac),
        AC_REPORT_MAGIC);
    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "acNum=%s&appmac="MACQUAD_FMT"&sign=%s", 
        report->ac_id, 
        MACQUAD(report->app_mac),
        md5sum);

    snprintf(commit_url, AC_REPORT_BUFFER_SIZE_MIN, "%s/%s?%s", 
        ac_url, AC_REPORT_PORTAL_SUB_URL, buff);
    
    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    DEBUGP("%s: commit_url: %s\n", __FUNCTION__, commit_url);
	CURL_SETOPT(my_curl, CURLOPT_HTTPGET, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, commit_url);
	
	retval= curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

	ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }

    
    if (rdata.buf)
    {
        free(rdata.buf);
    }
    
    if (commit_url)
    {
        free(commit_url);
    }
    
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_ac_report_state(char *ac_url, struct ac_report_state *report, struct ac_respond_state **respond)
{
    CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    char                        *macstr = NULL;
    CURLcode                    retval;
    int                         i, first_mac = 1;
    int                         res, ret = -1;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, report, *respond);
    
    if (*respond != NULL)
    {
        goto exit;
    }

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MAX);
    if (buff == NULL)
    {
        goto exit;
    }

    macstr = (char *)malloc(AC_REPORT_BUFFER_SIZE_MAX);
    if (macstr == NULL)
    {
        goto exit;
    }

    /* build mac serial */
    macstr[0] = '\0';
    for (i = 0; i < report->mac_count; i++)
    {
        if (first_mac)
        {
            snprintf(macstr, AC_REPORT_BUFFER_SIZE_MAX, MACQUAD_FMT, MACQUAD(report->mac[i].mac));
            first_mac = 0;
        }
        else
        {
            snprintf(macstr+strlen(macstr), AC_REPORT_BUFFER_SIZE_MAX-strlen(macstr), ","MACQUAD_FMT, MACQUAD(report->mac[i].mac));
        }
    }
    
    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MAX, "%d%s%d%d%d%s%s", 
        report->ac_count, 
        macstr,
        report->wifi_client_count, 
        report->up_data,
        report->down_data,
        report->ac_id,
        AC_REPORT_MAGIC);
    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MAX, "apCnt=%d&mac=%s&wifiClientCnt=%d&upData=%d&\
downData=%d&acNum=%s&sign=%s", 
        report->ac_count, 
        macstr,
        report->wifi_client_count, 
        report->up_data,
        report->down_data,
        report->ac_id,
        md5sum);
    
    rdata.buf = (unsigned char *)malloc(1024);
    rdata.buf_len = 1024;
    rdata.data_len = 0;
    my_curl = curl_easy_init();
    if(my_curl == NULL)
    {
        goto exit;
    }

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_STATE_SUB_URL);
    DEBUGP("%s: ifaddr: %s, post field: %s\n", __FUNCTION__, ifaddr, buff);
    CURL_SETOPT(my_curl, CURLOPT_POST, 1);
    CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
    CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
    CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
    CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
    CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
    CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
    CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
    
    retval = curl_easy_perform(my_curl);
    if(retval != CURLE_OK)
    {
        goto exit;
    }

    /* process respond */
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);
    
    res = respond_state_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: respond data json string error\n", __FUNCTION__);
        goto exit;
    }
    
    ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }

    
    if (rdata.buf)
    {
        free(rdata.buf);
    }
    
    if (macstr)
    {
        free(macstr);
    }
    
    if(my_curl)
    {
        curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;

}

int do_ac_report_warning(char *ac_url, struct ac_report_warning *report, struct ac_respond_warning **respond)
{
    CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    CURLcode                    retval;
    int                         res, ret = -1;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, report, *respond);
    DEBUGP("%s: ac_url: %s, ap_num: %s, ac_num: %s, warning_type: %u\n", 
        __FUNCTION__, ac_url, report->ap_id, report->ac_id, report->warning_type);
    
    if (*respond != NULL)
    {
        goto exit;
    }

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIDDLE);
    if (buff == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIDDLE, "%u%s%s%s", 
        report->warning_type, 
        report->ap_id,
        report->ac_id, 
        AC_REPORT_MAGIC);
    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIDDLE, "warningType=%u&apNum=%s&acNum=%s&sign=%s", 
        report->warning_type, 
        report->ap_id,
        report->ac_id,
        md5sum);
    
    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_WARNING_SUB_URL);
    DEBUGP("%s: ifaddr: %s, post field: %s\n", __FUNCTION__, ifaddr, buff);
    
	CURL_SETOPT(my_curl, CURLOPT_POST, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
	CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
	
	retval = curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

    /* process respond */
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);

    res = respond_warning_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: parser respond warning json string error\n", __FUNCTION__);
        goto exit;
    }
    
	ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }

    
    if (rdata.buf)
    {
        free(rdata.buf);
    }
        
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_ac_report_getconfig(char *ac_url, struct ac_report_getconfig *report, struct ac_respond_getconfig **respond)
{
    CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    CURLcode                    retval;
    int                         res, ret = -1;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, report, *respond);
    
    if (*respond != NULL)
    {
        goto exit;
    }

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buff == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "%s%s", 
        report->ac_id, 
        AC_REPORT_MAGIC);

    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "acNum=%s&sign=%s", 
        report->ac_id,
        md5sum);
    
    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_GETCONFIG_SUB_URL);
    DEBUGP("%s: ifaddr: %s, post field: %s\n", __FUNCTION__, ifaddr, buff);
    
	CURL_SETOPT(my_curl, CURLOPT_POST, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
	CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
	
	retval = curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

    /* process respond */
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);

    res = respond_getconfig_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: parser respond getconfig json string error\n", __FUNCTION__);
        goto exit;
    }
    
	ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }

    
    if (rdata.buf)
    {
        free(rdata.buf);
    }
        
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_ac_port_map(char *iface,char *sip,char *dip, unsigned int sport, unsigned int dport)
{
    char                        cmd[CMD_LEN] = {0};
    
    char                        *iptable = "/usr/local/sbin/iptables";
    int                         ret = -1;
    unsigned int                count = 0;
    FILE                        *fp = NULL;

    DEBUGP("%s: begin\n", __FUNCTION__);

    if(sip == NULL || dip == NULL || iface == NULL){
        goto EXIT;
    }

    if(sport > 65535 || dport > 65535){
        goto EXIT;
    }

    snprintf(cmd,sizeof(cmd),"%s -t nat -A PREROUTING -i %s --dst %s -p tcp -m multiport --dport %u -j DNAT --to-destination %s:%u",
                iptable, iface, sip, sport, dip, dport);
    DEBUGP("%s:process, cmd: %s\n",__FUNCTION__,cmd);
    
    fp = fopen(AC_PORTMAP_FILEPATH,"ab+");
    if(NULL == fp){
        goto EXIT;
    }
    
    count = fwrite(cmd, 1, strlen(cmd), fp);
    if(count != strlen(cmd)){
        fclose(fp);
        goto EXIT;
    }
    fwrite("\n",1,1,fp);
    fclose(fp);
    fp = NULL;

    ret = 0;
EXIT:
    DEBUGP("%s: finish ,ret: %d\n", __FUNCTION__, ret);
    if(fp){
        fclose(fp);
    }
    return ret;
}

int do_ac_report_getportmap(const char *url, struct ac_report_getportmap *portmap, struct ac_respond_getportmaps **respond_portmap)
{
    CURL                        *curl = NULL;
    CURLcode                    retval;
    struct http_respone_data    rdata;
    struct curl_slist           *headers = NULL;
    
    time_t			            timestamp;
    char                        md5_src[BUFFER_LEN] = {0};
    char                        key[STRING_LENGTH] = {0};
    char                        buff[PORT_MAP_BUF_LENGTH] = {0};
    int                         res, ret = -1,num = 0;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, portmap, *respond_portmap);

    if(*respond_portmap!= NULL){
        goto exit;
    }
    
    rdata.buf = (unsigned char *)malloc(1024);
    if(rdata.buf == NULL){
        goto exit;
    }
	rdata.buf_len = 1024;
	rdata.data_len = 0;

    headers=curl_slist_append(headers, "Request-Line: POST /routeservice/routeinfo.do HTTP/1.1");
    curl_slist_append(headers, "Content-Type: text/plain");
    curl = curl_easy_init();
	if(curl == NULL)
	{
	    goto exit;
	}
    
RECV:
    get_timestamp(&timestamp);

    snprintf(md5_src,sizeof(md5_src),"%s%d%s", portmap->ac_id, (int)timestamp, MD5KEY);
    md5sum_build((unsigned char *)md5_src,key);

    snprintf(buff,sizeof(buff),"{\"series\":\"%s\",\"model\":\"%s\",\"dtdate\":\"%ld\",\"deviceid\":\"%s\",\"key\":\"%s\"}",
            portmap->serics,portmap->model,timestamp,portmap->ac_id,key);

    DEBUGP("%s:process, url:[%s], buff:[%s]\n", __FUNCTION__, url, buff);
    
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
        if(num > 3){
            goto exit;
        }
        num++;
        memset(buff,0,sizeof(buff));
        memset(md5_src,0,sizeof(md5_src));
        memset(rdata.buf,0,sizeof(rdata.buf));
		goto RECV;
	}
    
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);

    res = respond_getportmap_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond_portmap);
    if (res != 0)
    {
        DEBUGP("%s: parser respond getconfig json string error\n", __FUNCTION__);
        goto exit;
    }
    
	ret = 0;

exit:  
    if (rdata.buf)
    {
        free(rdata.buf);
    }
        
    if(curl)
    {
    	curl_easy_cleanup(curl);
    }
    if(headers)
    {
        curl_slist_free_all(headers);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int do_ac_report_getapp(char *ac_url, struct ac_report_getapp *report, struct ac_respond_getapp **respond)
{
    CURL                        *my_curl = NULL;
    struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    CURLcode                    retval;
    int                         res, ret = -1;

    DEBUGP("%s: begin, report: %p, respond: %p\n", __FUNCTION__, report, *respond);
    
    if (*respond != NULL)
    {
        goto exit;
    }

    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buff == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "%s%s", 
        report->ac_id, 
        AC_REPORT_MAGIC);
    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "acNum=%s&sign=%s", 
        report->ac_id,
        md5sum);
    
    rdata.buf = (unsigned char *)malloc(1024);
	rdata.buf_len = 1024;
	rdata.data_len = 0;
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_GETAPP_SUB_URL);
    DEBUGP("%s: ifaddr: %s, post field: %s\n", __FUNCTION__, ifaddr, buff);
	CURL_SETOPT(my_curl, CURLOPT_POST, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, http_respone_write);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, &rdata);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
	CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
	
	retval = curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

    /* process respond */
    DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);

    res = respond_getapp_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: parser respond getconfig json string error\n", __FUNCTION__);
        goto exit;
    }
    
	ret = 0;

exit:
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }
    
    if (rdata.buf)
    {
        free(rdata.buf);
    }
        
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

extern size_t write_data(char *buffer, size_t size, size_t nitems, void *outstream);

int do_ac_report_getwebfile(char *ac_url, struct ac_report_getwebfile *report)
{
    CURL                        *my_curl = NULL;
    //struct http_respone_data    rdata;
    char                        ifaddr[AC_REPORT_IFADDR_LENGTH] = {0};
    
    char                        *buff = NULL;
    char                        *encode_buff = NULL;
    char                        md5sum[MD5SUM_LENGTH] = {0};
    CURLcode                    retval;
    int                         ret = -1;
    FILE *fp = NULL;

    DEBUGP("%s: begin, report: %p\n", __FUNCTION__, report);
    buff = (char *)malloc(AC_REPORT_BUFFER_SIZE_MIN);
    if (buff == NULL)
    {
        goto exit;
    }

    /* calculate md5sum for report */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "%s%s", 
        report->ac_id, 
        AC_REPORT_MAGIC);
    
    DEBUGP("%s:buff=%s\n",__FUNCTION__,buff);

    encode_buff = curl_easy_escape(my_curl, buff, strlen(buff));
    
    if (encode_buff == NULL)
    {
        goto exit;
    }
    
    md5sum_build((unsigned char *)encode_buff, md5sum);

    /* build post field */
    snprintf(buff, AC_REPORT_BUFFER_SIZE_MIN, "acNum=%s&sign=%s", 
        report->ac_id,
        md5sum);
#if 0    
    rdata.buf = (unsigned char *)malloc(AC_REPORT_BUFFER_SIZE_MAX);
	rdata.buf_len = AC_REPORT_BUFFER_SIZE_MAX;
	rdata.data_len = 0;
#endif
    fp = fopen(GET_WEBFILEINFO_FILENAME, "wt+"); 
    if (fp == NULL)
    {
        printf("%s: can not open or create\n", __FUNCTION__);
        goto exit;
    }
    
    my_curl = curl_easy_init();
	if(my_curl == NULL)
	{
	    goto exit;
	}

    snprintf(ifaddr, AC_REPORT_IFADDR_LENGTH, "%s/%s/", ac_url, AC_REPORT_GETWEBFILE_SUB_URL);
    DEBUGP("%s: ifaddr: %s, post field: %s\n", __FUNCTION__, ifaddr, buff);
	CURL_SETOPT(my_curl, CURLOPT_POST, 1);
	CURL_SETOPT(my_curl, CURLOPT_WRITEFUNCTION, write_data);
	CURL_SETOPT(my_curl, CURLOPT_WRITEDATA, (void*)fp);
	CURL_SETOPT(my_curl, CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
	CURL_SETOPT(my_curl, CURLOPT_VERBOSE, 0);
	CURL_SETOPT(my_curl, CURLOPT_NOSIGNAL, 1L);
	CURL_SETOPT(my_curl, CURLOPT_URL, ifaddr);
	CURL_SETOPT(my_curl, CURLOPT_POSTFIELDS, buff);
	
	retval = curl_easy_perform(my_curl);
	if(retval != CURLE_OK)
	{
		goto exit;
	}

    /* process respond */
    //DEBUGP("%s: recv buf: %s, data_len: %u\n", __FUNCTION__, rdata.buf, rdata.data_len);
#if 0
    res = respond_getapp_json_decode((char *)rdata.buf, (int32_t)rdata.data_len, respond);
    if (res != 0)
    {
        DEBUGP("%s: parser respond getconfig json string error\n", __FUNCTION__);
        goto exit;
    }
#endif
	ret = 0;

exit:
    if(fp)
    {
        fclose(fp);
        fp=NULL;
    }
    if (encode_buff)
    {
        curl_free(encode_buff);
    }
    
    if (buff)
    {
        free(buff);
    }
        
    if(my_curl)
    {
    	curl_easy_cleanup(my_curl);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

