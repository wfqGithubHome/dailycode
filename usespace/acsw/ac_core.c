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

#include "include/utils.h"
#include "include/md5.h"
#include "include/json.h"
#include "include/ac_json.h"
#include "include/ac_report.h"
#include "include/ac_mac_auth.h"
#include "include/ac_ip_classify.h"
#include "include/ac_cache.h"
#include "include/ac_core.h"
#include "include/common.h"

#if 1
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

#define AC_MAC_TIME_MAX 4096
#define AC_IP_LIST_MAX  4096

#define CMD_LENGTH      256
#define STRING_LENGTH   64

#define AC_PORTAL_URL_FILE_PATH "/proc/sys/net/dpifilter/auth/auth_redirect_http_url"

struct ac_report_config_info ac_report_config;
extern int total;
extern int right;

extern int respond_getportmap_json_decode(char *buffer, int datalen, struct ac_respond_getportmap **respond);



static int device_model_and_series_read(unsigned char *model,unsigned char *series)
{
    FILE                *fp = NULL;
    char                buf[BUFFER_LEN] = {0};
    struct split_elem   *elems = NULL;
    int                 elems_num;
    int                 sign = 0,ret = -1;
    
    DEBUGP("%s: begin\n", __FUNCTION__);

    fp = fopen("/etc/version","r");
    if(NULL == fp){
        goto EXIT;
    }

    while(fgets(buf, BUFFER_LEN - 1, fp) != NULL){
        if (split(buf, "\"", &elems, &elems_num) != 0){
            free(elems);
            elems = NULL;
            continue;
        }

        if(sign == 2){
            free(elems);
            elems = NULL;
            break;
        }

        if(strcmp("MODEL=",elems[0].val) == 0){
            strncpy((char *)model,elems[1].val,STRING_LENGTH);
            sign++;
        }

        if(strcmp("SERIES=",elems[0].val) == 0){
            strncpy((char *)series,elems[1].val,STRING_LENGTH);
            sign++;
        }

        free(elems);
        elems = NULL;
    }

    ret = 0;
EXIT:
    DEBUGP("%s: finish, ret: %d, model: %s, series: %s\n", __FUNCTION__, ret, model, series);
    if(elems){
        free(elems);
        elems = NULL;
    }
    return ret;
}

static void online_ipcnt_get(uint32_t *ipcnt)
{
    FILE                 *fp = NULL;
    char                 cmd[CMD_LENGTH] = {0};
    char                 buff[STRING_LENGTH] = {0};

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    sprintf(cmd,"/usr/local/sbin/ipinfo -b 0 -c -p 0 2>/dev/null");
    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        DEBUGP("can not get online ip count\n");
        return;
    }

    fgets(buff, STRING_LENGTH, fp);
    *ipcnt = atoi(buff);
    fclose(fp);

    DEBUGP("%s: finish, ipcnt: %u\n", __FUNCTION__, *ipcnt);
    return;
}


static int check_input(void)
{
    if((ac_report_config.setting + ac_report_config.portal + ac_report_config.state + 
   	    ac_report_config.warning + ac_report_config.config + ac_report_config.app + ac_report_config.webfile +
        ac_report_config.appinit + ac_report_config.help + ac_report_config.portmap) != 1)
    {
		printf("action error! maybe multi-action or none-action!\n");    
		return -1;
    }

    return 0;
}

static int ac_report_command_parse(int *argc, char ***argv)
{
	char buff[1024];

	memset(&ac_report_config, 0, sizeof(struct ac_report_config_info));

	snprintf(buff, 1024, "%s - %s ,version %s\n\n", 
	AC_REPORT_LONG_NAME, AC_REPORT_DESCRIPTION, AC_REPORT_VERSION);
	optTitle(buff);
	optProgName(AC_REPORT_NAME);

    optrega(&ac_report_config.url,          OPT_STRING,      'u' , "url" ,       "ac report url");
	optrega(&ac_report_config.setting,      OPT_FLAG,        's' , "setting" ,   "ac setting report");
	optrega(&ac_report_config.portal,       OPT_FLAG,        'p' , "portal" ,    "get ac portal page");
	optrega(&ac_report_config.state,        OPT_FLAG,        'S' , "state" ,     "ac state report");
    optrega(&ac_report_config.warning,      OPT_FLAG,        'w' , "warning" ,   "ac status report");
	optrega(&ac_report_config.config,       OPT_FLAG,        'c' , "config" ,    "get current config ");
	optrega(&ac_report_config.app,          OPT_FLAG,        'a' , "app" ,       "get app config");
    optrega(&ac_report_config.webfile,      OPT_FLAG,        'e' , "webfile" ,   "get web file");
    optrega(&ac_report_config.appinit,      OPT_FLAG,        'i' , "appinit" ,   "ac cache sync to kernel");
    optrega(&ac_report_config.portmap,      OPT_FLAG,        'm' , "portmap" ,   "get port map info from server");
    
	optrega(&ac_report_config.help,         OPT_FLAG,        'h',  "help" ,      "usage");

	opt(argc, argv); 

	return 0;
}

static int ac_setting_report_process(char *ac_url, char *ac_id, char *ap_id)
{
    struct ac_report_setting    setting;
    struct ac_respond_setting   *setting_respond = NULL;
    int                         res, ret = -1;

    DEBUGP("%s: begin, ac_url: %s, ac_id: %s, ap_id: %s\n", 
        __FUNCTION__, ac_url, ac_id, ap_id);

    strncpy(setting.ac_id, ac_id, DEVICE_ID_LENGTH);
    strncpy(setting.ap_id, ap_id, DEVICE_ID_LENGTH);
    setting.cache_toplimit = 32;
    setting.wifi_conn_toplimit = 32;
    setting.timespan = 300;
    
    res = do_ac_report_setting(ac_url, &setting, &setting_respond);
    if (res != 0)
    {
        printf("do_ac_report_setting failed\n");
        goto exit;
    }

    if (setting_respond->error != 0)
    {
        printf("setting_respond->error: %d\n", setting_respond->error);
        goto exit;
    }

    ret = 0;
exit:
    
    if (setting_respond)
    {
        free(setting_respond);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_setting_report(char *ac_url)
{
    unsigned char               hwid[DEVICE_ID_LENGTH] = {0};
    struct ac_report_setting    setting;
    struct ac_respond_setting   *setting_respond = NULL;
    int                         ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    memset(&setting, 0, sizeof(setting));
    strncpy(setting.ac_id, (char *)hwid, DEVICE_ID_LENGTH);
    strncpy(setting.ap_id, (char *)hwid, DEVICE_ID_LENGTH);
    setting.cache_toplimit = 0;
    setting.wifi_conn_toplimit = 0;
    setting.timespan = 5;

    ret = do_ac_report_setting(ac_url, &setting, &setting_respond);
    
    if (ret == 0)
    {
        DEBUGP("ac_setting code: %d\n", setting_respond->error);
    }
    else
    {
        DEBUGP("error, ac_setting failed\n");
    }

    free(setting_respond);
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_portal_get(char *ac_url)
{
    unsigned char               hwid[DEVICE_ID_LENGTH] = {0};
    struct ac_report_portal     portal;
    int                         ret = -1;
    
    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    memset(&portal, 0, sizeof(portal));
    strncpy(portal.ac_id, (char*)hwid, DEVICE_ID_LENGTH);
    portal.app_mac[0] = '\0';

    ret = do_ac_report_portal(ac_url, &portal);
    if (ret != 0)
    {
        DEBUGP("%s: ac report portal respond error\n", __FUNCTION__);
    }
    
    DEBUGP("%s: finish\n", __FUNCTION__);
    return ret;
    
}

static int ac_state_report(char *ac_url)
{
    struct ac_report_state     *state = NULL;
    int                         state_size;
    struct ac_respond_state    *state_respond = NULL;

    unsigned char              hwid[DEVICE_ID_LENGTH] = {0};
    uint32_t                   online_ipcnt = 0;
    
    struct ac_report_mac       *macs = NULL;
    int                         macs_size;
    int                         macs_count = 0;
    int                         i, res, ret = -1;
    
    DEBUGP("%s: begin\n", __FUNCTION__);

    macs_size = sizeof(struct ac_report_mac) * AC_MAC_TIME_MAX;
    macs = (struct ac_report_mac *)malloc(macs_size);
    if (macs == NULL)
    {
        DEBUGP("%s: alloc memory failed\n", __FUNCTION__);
        goto exit;
    }

    memset((char *)macs, 0, sizeof(macs_size));
    res = ac_mac_get_from_kernel(macs, &macs_count);
    if (res != 0)
    {
        DEBUGP("%s: get mac list from kernel failed\n", __FUNCTION__);
        goto exit;
    }

    state_size = sizeof(struct ac_report_state) + sizeof(struct ac_report_mac) * macs_count;
    state = (struct ac_report_state *)malloc(state_size);
    if (state == NULL)
    {
        DEBUGP("%s: alloc state memory failed\n", __FUNCTION__);
        goto exit;
    }

    device_hwid_read(hwid);
    online_ipcnt_get(&online_ipcnt);
    state->ac_count = 0;
    state->wifi_client_count = online_ipcnt;
    state->up_data = 0;
    state->down_data = 0;
    strncpy(state->ac_id, (char *)hwid, DEVICE_ID_LENGTH);
    state->mac_count = macs_count;
    for (i = 0; i < macs_count; i++)
    {
        memcpy((char *)state->mac[i].mac, (char *)macs[i].mac, MAC_ADDRESS_LENGTH);
    }

    res = do_ac_report_state(ac_url, state, &state_respond);
    if (res != 0)
    {
        DEBUGP("%s: do_ac_report_state failed\n", __FUNCTION__);
        goto exit;
    }

    if (state_respond->error != 0)
    {
        printf("state_respond->error: %d\n", state_respond->error);
        goto exit;
    }

    res = ac_mac_set_to_kernel(state_respond->mac_time, state_respond->mac_time_count, AC_MAC_AUTH_TABLE_SYNC);
    if (res != 0)
    {
        printf("sync mac list to kernel failed\n");
        goto exit;
    }

    ret = 0;
exit:
    if (macs)
    {
        free(macs);
    }
    
    if (state)
    {
        free(state);
    }

    if (state_respond)
    {
        free(state_respond);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_waring_report_process(char *ac_url, char *ac_id, char *ap_id, enum ac_report_warning_type warning_type)
{
    struct ac_report_warning    warning;
    struct ac_respond_warning   *warning_respond = NULL;
    int                         res, ret = -1;

    DEBUGP("%s: begin, ac_url: %s, ac_id: %s, ap_id: %s, warning_type: %u\n", 
        __FUNCTION__, ac_url, ac_id, ap_id, warning_type);

    strncpy(warning.ac_id, ac_id, DEVICE_ID_LENGTH);
    strncpy(warning.ap_id, ap_id, DEVICE_ID_LENGTH);
    warning.warning_type = warning_type;
    
    res = do_ac_report_warning(ac_url, &warning, &warning_respond);
    if (res != 0)
    {
        printf("do_ac_report_warning failed\n");
        goto exit;
    }

    if (warning_respond->error != 0)
    {
        printf("warning_respond->error: %d\n", warning_respond->error);
        goto exit;
    }

    ret = 0;
exit:
    
    if (warning_respond)
    {
        free(warning_respond);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_warning_report(char *ac_url)
{
    unsigned char               hwid[DEVICE_ID_LENGTH] = {0};
    int                         ret;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    ret = ac_mac_auth_ap_info_get(ac_url, (char *)hwid, AC_REPORT_WARNING_TYPE_TIMEOUT, 
                ac_waring_report_process, ac_setting_report_process);
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_config_get(char *ac_url)
{
    FILE                        *fp = NULL;
    struct ac_report_getconfig  getconfig;
    struct ac_respond_getconfig *getconfig_respond = NULL;
    struct ac_respond_mac_time  *pmt = NULL;
    unsigned int                *pip = NULL;
    unsigned int                *psip = NULL;

    unsigned char               hwid[DEVICE_ID_LENGTH] = {0};
    char                        cmd[COMMAND_STRING_LENGTH] = {0};
    int                         res, ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    strncpy(getconfig.ac_id, (char *)hwid, DEVICE_ID_LENGTH);

    memset(cmd,0,sizeof(cmd));
    snprintf(cmd, COMMAND_STRING_LENGTH, "echo \"50 6 */2 * * /sbin/reboot\" >> /etc/crontabs/root");
    system(cmd);
    
    system("/etc/init.d/crond restart");

try_again:
    res = do_ac_report_getconfig(ac_url, &getconfig, &getconfig_respond);
    if (res != 0)
    {
        printf("do_ac_report_getconfig failed\n");
        ten_second_delay();
        if (getconfig_respond)
        {
            free(getconfig_respond);
        }
        goto try_again;
    }

    if (getconfig_respond->error)
    {
        printf("getconfig_respond->error: %d\n", getconfig_respond->error);
        ten_second_delay();
        if (getconfig_respond)
        {
            free(getconfig_respond);
        }
        goto try_again;
    }

    pmt = (struct ac_respond_mac_time *)getconfig_respond->buff;
    res = ac_mac_set_to_kernel(pmt, getconfig_respond->mac_time_count, AC_MAC_AUTH_TABLE_INIT);
    if (res != 0)
    {
        printf("init mac list failed\n");
        ten_second_delay();
        if (getconfig_respond)
        {
            free(getconfig_respond);
        }
        goto try_again;
    }
    
    pip = (unsigned int *)(getconfig_respond->buff + getconfig_respond->ips_offset);
    psip = (unsigned int *)(getconfig_respond->buff + getconfig_respond->sips_offset);
    res = ac_ips_set_to_kernel(pip, getconfig_respond->ip_count, psip, getconfig_respond->sip_count);
    if (res != 0)
    {
        printf("init ip list failed\n");
        ten_second_delay();
        if (getconfig_respond)
        {
            free(getconfig_respond);
        }
        goto try_again;
    }

    fp = fopen(AC_PORTAL_URL_FILE_PATH, "wt+");
    if (fp == NULL)
    {
        ten_second_delay();
        if (getconfig_respond)
        {
            free(getconfig_respond);
        }
        goto try_again;
    }

    fputs(getconfig_respond->portal, fp);
    fclose(fp);

    snprintf(cmd, COMMAND_STRING_LENGTH, "echo \"*/%d * * * * /usr/local/sbin/ac_report\" >> /etc/crontabs/root", getconfig_respond->timespan);
    system(cmd);
    
    system("/etc/init.d/crond restart");

    ret = 0;
    if (getconfig_respond)
    {
        free(getconfig_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static int ac_port_map_init(char *url)
{
    struct ac_report_getportmap     getportmap;
    struct ac_respond_getportmaps   *respond_getportmap = NULL;
    char                            iface[STRING_LENGTH] = {0};
    char                            sip[STRING_LENGTH] = {0};
    unsigned char                   hwid[DEVICE_ID_LENGTH] = {0};
    unsigned char                   model[STRING_LENGTH] = {0};
    unsigned char                   series[STRING_LENGTH] = {0};
    int                             res, ret = -1, i = 0;
    FILE                            *fp = NULL;
    const char                      *command = "/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh apply dnat >/dev/null 2>&1";

    DEBUGP("%s: begin\n", __FUNCTION__);

    if(url == NULL){
        goto EXIT;
    }

    memset(&getportmap,0,sizeof(getportmap));
    device_hwid_read(hwid);
    device_model_and_series_read(model,series);
    strncpy(getportmap.ac_id, (char *)hwid, DEVICE_ID_LENGTH);
    strncpy(getportmap.model, (char *)model, STRING_LENGTH);
    strncpy(getportmap.serics, (char *)series, STRING_LENGTH);

    DEBUGP("%s:process, hwid:[%s], model:[%s], series:[%s]\n",
            __FUNCTION__,hwid,model,series);

    res = get_wan_ip(sip,sizeof(sip),iface,sizeof(iface));
    if(res != 0){
        goto EXIT;
    }

    DEBUGP("%s:process, sip:[%s], iface:[%s]\n",
            __FUNCTION__,sip,iface);

    res = do_ac_report_getportmap(url, &getportmap, &respond_getportmap);
    if (res != 0)
    {
        printf("ac_port_map_init failed\n");
        goto EXIT;
    }

    DEBUGP("%s:process, code:[%d], sport:[%d], dport:[%d], ip:[%s]\n", 
            __FUNCTION__,
            respond_getportmap->pinfo[0].code,
            respond_getportmap->pinfo[0].sport,
            respond_getportmap->pinfo[0].dport,
            respond_getportmap->pinfo[0].ip);

    fp = fopen(AC_PORTMAP_FILEPATH,"w");
	
    if(NULL == fp){
        goto EXIT;
    } else {
        fclose(fp);
        fp = NULL;
    }
	
    for(i=0; i < respond_getportmap->num; i++){
    	if(respond_getportmap->pinfo[i].code == 0){
           res = do_ac_port_map(iface,sip,respond_getportmap->pinfo[i].ip,respond_getportmap->pinfo[i].sport,respond_getportmap->pinfo[i].dport);
	    }
    }

    system(command);
    if(res == 0)
        ret = 0;

EXIT:
    DEBUGP("%s: finish, ret:%d\n", __FUNCTION__, ret);
    if(respond_getportmap){
        free(respond_getportmap);
        respond_getportmap = NULL;
    }
    return ret;
}

static int ac_app_get(char *ac_url)
{
    struct ac_report_getapp     getapp; 
    struct ac_respond_getapp    *getapp_respond = NULL;
    unsigned char               hwid[DEVICE_ID_LENGTH] = {0};
    int                         res, ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    strncpy(getapp.ac_id, (char *)hwid, DEVICE_ID_LENGTH);

    res = do_ac_report_getapp(ac_url, &getapp, &getapp_respond);
    if (res != 0)
    {
        printf("do_ac_report_getapp failed\n");
        goto exit;
    }

    if (getapp_respond->error)
    {
        printf("getapp_respond->error: %d\n", getapp_respond->error);
        goto exit;
    }

    ret = ac_cache_file_system_update(getapp_respond->units, getapp_respond->unit_count);

exit:
    if (getapp_respond)
    {
        free(getapp_respond);
    }

    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
    
}

static int ac_webfile_get(char *ac_url)
{
    struct ac_report_getwebfile     getwebfile; 
    //struct ac_respond_getwebfile    *getwebfile_respond = NULL;
    unsigned char                   hwid[DEVICE_ID_LENGTH] = {0};
    int                             res, ret = -1;

    DEBUGP("%s: begin\n", __FUNCTION__);

    device_hwid_read(hwid);
    strncpy(getwebfile.ac_id, (char *)hwid, DEVICE_ID_LENGTH);

    res = do_ac_report_getwebfile(ac_url, &getwebfile);
    if (res != 0)
    {
        printf("do_ac_report_getapp failed\n");
        goto exit;
    }

    ret = ac_webfile_system_update(GET_WEBFILEINFO_FILENAME, DEVICE_WEBFILEINFO_FILENAME);
    if(ret == 0)
    {
        char cmd[AC_COMMAND_LENGTH] = {0};
        snprintf(cmd, AC_COMMAND_LENGTH, "cp %s %s >/dev/null 2>&1", 
                            GET_WEBFILEINFO_FILENAME, DEVICE_WEBFILEINFO_FILENAME);
        system(cmd);
        snprintf(cmd, AC_COMMAND_LENGTH, "cp %s %s >/dev/null 2>&1 && chmod -R 755 %s", 
                            GET_WEBFILEINFO_FILENAME, DEVICE_WEBFILEINFO_FLASHFILE, DEVICE_WIFISEVER_ROOTPAHT);
        system(cmd);
    }

exit:
#if 0
    if (getwebfile_respond)
    {
        free(getwebfile_respond);
    }
#endif
    DEBUGP("%s: finish, ret: %d, total:%d, success:%d\n", __FUNCTION__, ret, total, right);
    return ret;
    
}

int main(int argc, char *argv[])
{
    char *ac_url = AC_REPORT_URL;
	int ret = -1; 

	if(ac_report_command_parse(&argc, &argv) != 0)
    {
		return ret;
	}

	if(check_input() != 0)
    {
		return ret;
	}

    if (ac_report_config.url)
    {
        ac_url = ac_report_config.url;
    }

	if(ac_report_config.setting)
    {
		return ac_setting_report(ac_url);
	}
	else if(ac_report_config.portal)
    {
		return ac_portal_get(ac_url);
	}
	else if(ac_report_config.state)
    {
		return ac_state_report(ac_url);
	}
	else if(ac_report_config.warning)
    {
		return ac_warning_report(ac_url);
	}
	else if(ac_report_config.config)
    {
		return ac_config_get(ac_url);
	}
	else if(ac_report_config.app)
    {
		return ac_app_get(ac_url);
	}
    else if(ac_report_config.webfile)
    {
        return ac_webfile_get(ac_url);
    }
    else if(ac_report_config.appinit)
    {
		return ac_cache_context_init();
	}
    else if(ac_report_config.portmap){
        return ac_port_map_init(ac_url);
    }
    else if(ac_report_config.help)
    {
		optPrintUsage();		 
	}

    ret = 0;
	return ret;	
}



