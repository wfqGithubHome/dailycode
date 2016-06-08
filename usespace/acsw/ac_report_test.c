#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "include/utils.h"
#include "include/md5.h"
#include "include/json.h"
#include "include/ac_json.h"
#include "include/ac_report.h"

int main(void)
{
    struct ac_report_setting setting;
    struct ac_respond_setting *setting_respond = NULL;

    struct ac_report_portal portal;

    struct ac_report_state *state = NULL;
    struct ac_respond_state *state_respond = NULL;

    struct ac_report_warning warning;
    struct ac_respond_warning *warning_respond = NULL;

    struct ac_report_getconfig getconfig;
    struct ac_respond_getconfig *getconfig_respond = NULL;
    struct ac_respond_mac_time  *pmt = NULL;
    unsigned int                *pip = NULL;

    struct ac_report_getapp getapp; 
    struct ac_respond_getapp *getapp_respond = NULL;
    
    int i, ret;
    
    /* setting */
    strncpy(setting.ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    strncpy(setting.ap_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    setting.cache_toplimit = 0;
    setting.wifi_conn_toplimit = 0;
    setting.timespan = 5;

    ret = do_ac_report_setting(&setting, &setting_respond);

    if (ret == 0)
    {
        printf("ac_setting code: %d\n", setting_respond->error);
    }
    else
    {
        printf("error, ac_setting failed\n");
    }

    sleep(30);

portal:
    /* portal */
    strncpy(portal.ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    mac_str_to_bin("112233445566", portal.app_mac);
    ret = do_ac_report_portal(&portal);
    if (ret == 0)
    {
        printf("ac portal successed\n");
    }
    else
    {
        printf("ac portal failed\n");
    }

    sleep(30);

state:
    /* state */
    state = (struct ac_report_state *)malloc(sizeof(struct ac_report_state) + sizeof(struct ac_report_mac) * 2);
    state->ac_count = 0;
    state->wifi_client_count = 0;
    state->up_data = 0;
    state->down_data = 0;
    strncpy(state->ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    state->mac_count = 2;
    mac_str_to_bin("112233445566", state->mac[0].mac);
    mac_str_to_bin("ffffffffffff", state->mac[1].mac);
    ret = do_ac_report_state(state, &state_respond);
    if (ret == 0)
    {
        printf("-------------------ac respond state---------------------------\n");
        printf("ac respond state: code: %d, mac_time_count: %d\n", 
            state_respond->error, state_respond->mac_time_count);
        for (i = 0; i < state_respond->mac_time_count; i++)
        {
            printf("mac: "MACQUAD_FMT", time: %d\n", 
                MACQUAD(state_respond->mac_time[i].mac), 
                state_respond->mac_time[i].time);
        }
    }
    else
    {
        printf("ac report state failed\n");
    }

    
    sleep(30);

warning:
    /* warning */
    warning.warning_type = AC_REPORT_WARNING_TYPE_TIMEOUT;
    strncpy(warning.ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    strncpy(warning.ap_id, "8f04534a996c2282837780cb78b0b624", DEVICE_ID_LENGTH);
    ret = do_ac_report_warning(&warning, &warning_respond);
    if (ret == 0)
    {
        printf("ac report warning successed, code: %d\n", warning_respond->error);
    }
    else
    {
        printf("ac report warning failed\n");
    }

    
    sleep(30);
getconfig:
    strncpy(getconfig.ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    ret = do_ac_report_getconfig(&getconfig, &getconfig_respond);
    if (ret == 0)
    {
        printf("-------------------ac respond getconfig---------------------------\n");
        printf("code: %d, cache_toplimit: %d, timespan: %d, portal: %s, wifi_conn_toplimit: %d, mt_cnt: %d, ip_cnt: %d, ip_offset: %d\n", 
            getconfig_respond->error, 
            getconfig_respond->cache_toplimit, 
            getconfig_respond->timespan, 
            getconfig_respond->portal,
            getconfig_respond->wifi_Conn_toplimit, 
            getconfig_respond->mac_time_count, 
            getconfig_respond->ip_count, 
            getconfig_respond->ips_offset);
        pmt = (struct ac_respond_mac_time *)getconfig_respond->buff;
        for (i = 0; i < getconfig_respond->mac_time_count; i++)
        {
            printf("mac: "MACQUAD_FMT", time: %d\n", MACQUAD(pmt[i].mac), pmt[i].time);
        }

        pip = (unsigned int *)(getconfig_respond->buff + sizeof(struct ac_respond_mac_time) * getconfig_respond->mac_time_count);
        for (i = 0; i < getconfig_respond->ip_count; i++)
        {
            printf("ip: "NIPQUAD_FMT"\n", NIPQUAD(pip[i]));
        }
    }
    else
    {
        printf("ac respond getconfig failed\n");
    }
    
    sleep (30);

getapp:

    strncpy(getapp.ac_id, "8f04534a996c2282837780cb78b0b623", DEVICE_ID_LENGTH);
    ret = do_ac_report_getapp(&getapp, &getapp_respond);
    if (ret == 0)
    {
        printf("-------------------ac respond getapp---------------------------\n");
        printf("code: %d, unit_count: %d\n", getapp_respond->error, getapp_respond->unit_count);
        for (i = 0; i < getapp_respond->unit_count; i++)
        {
            printf("appname: %s, appsize: %d, appid: %d, appurl: %s, appver: %s\n", 
                getapp_respond->units[i].app_name, 
                getapp_respond->units[i].app_size, 
                getapp_respond->units[i].app_id,
                getapp_respond->units[i].app_url, 
                getapp_respond->units[i].app_version);
        }
    }
    else
    {
        printf("ac respond getapp failed\n");
    }
    
exit:
    if (setting_respond)
    {
        free(setting_respond);
    }

    if (state)
    {
        free(state);
    }
    
    if (state_respond)
    {
        free(state_respond);
    }

    if (warning_respond)
    {
        free(warning_respond);
    }

    if (getconfig_respond)
    {
        free(getconfig_respond);
    }

    if (getapp_respond)
    {
        free(getapp_respond);
    }
    
    return 0;
    
}


