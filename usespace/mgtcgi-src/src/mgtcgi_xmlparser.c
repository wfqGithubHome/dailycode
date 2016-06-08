#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/route_xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"
//#include "../include/mgtcgi_debug.h"

#define CGI_FORM_STRING		1
char * flash_xmlconf_path = "/flash/etc/config.xml";
char * working_xmlconf_path = "/etc/config.xml";

int check_ipaddr(const char *ipaddr);
int copy_xmlconf_file(const char * srcfile, const char * dstfile);
int apply_firewallpolicy(int firwall_enable);

/**************************************list_show**************************************/
void firewall_policy_list_show()
{
    int iserror=0,total=0,size=0,index=0,flag=0;
    struct mgtcgi_xml_firewalls_policy *firewallpolicy = NULL;
    
    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&firewallpolicy, &size);
    if (NULL == firewallpolicy){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    total = 0;
    flag = 0;
    printf("{\"data\":[");
    for (index=0; index < firewallpolicy->num; index++){
        if (flag == 1)
            printf(",");
        else
            flag = 1;

        printf("{\"id\":\"%u\",\"enable\":\"%u\",\"src\":\"%s\",\"dst\":\"%s\",\"isp\":\"%s\",\"proto\":\"%s\",\"session\":\"%s\",\"iface\":\"%s\","  \
               "\"action\":\"%s\",\"timed\":\"%s\",\"week\":\"%s\",\"day\":\"%s\",\"in_limit\":\"%s\",\"out_limit\":\"%s\",\"comment\":\"%s\"}",
                firewallpolicy->pinfo[index].id,firewallpolicy->pinfo[index].enable,firewallpolicy->pinfo[index].src,
                firewallpolicy->pinfo[index].dst,firewallpolicy->pinfo[index].isp,firewallpolicy->pinfo[index].proto,
                firewallpolicy->pinfo[index].session,firewallpolicy->pinfo[index].iface,firewallpolicy->pinfo[index].action,
                firewallpolicy->pinfo[index].timed,firewallpolicy->pinfo[index].week,firewallpolicy->pinfo[index].day,
                firewallpolicy->pinfo[index].in_limits,firewallpolicy->pinfo[index].out_limits,firewallpolicy->pinfo[index].comment);
        total++;
    }
    printf("],");
    
    printf("\"firewall_policy_enable\":%u,\"iserror\":0,\"total\":%d,\"msg\":\"\"}", firewallpolicy->enable, total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(firewallpolicy){
        free_xml_node((void *)&firewallpolicy);
    }
    return;

}

void layer7protocol_type_list_show()
{
    int iserror =-1,flag=0,total=0,size,i;
    struct mgtcgi_xml_layer7_protocols_show *l7protocol = NULL;
    
    get_xml_nodeforLayer7(MGTCGI_XMLTMP,L7_XMLFILE,MGTCGI_TYPE_LAYER7TYPE_SHOWS,
						"chinese",(void**)&l7protocol,&size);
    if ( NULL == l7protocol){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    printf("{\"data\":[");
    for(i = 0; i < l7protocol->num; i++){
        if (flag == 1)
            printf(",");
        else
            flag = 1;
        
        printf("{\"name\":\"%s\",\"catid\":\"%u\",\"value\":\"%s\"}",
            l7protocol->pinfo[i].name,l7protocol->pinfo[i].gid,l7protocol->pinfo[i].match);
        total++;
    }
    printf("],\"total\":%d,\"iserror\":0,\"msg\":\"\"}",total);
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(l7protocol){
        free_xml_node((void *)&l7protocol);
    }
    return;
}


/**************************************add_show**************************************/
void firewall_policy_add_show(void)
{
    int iserror=0,size=0,i,t=0,wan_size=0;
    struct mgtcgi_xml_route_tables *routing_tables = NULL;
    struct mgtcgi_xml_interface_wans *wans = NULL;
    
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTING_TABLES, (void**)&routing_tables, &size);
    if (NULL == routing_tables){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wans, &wan_size);
    if (NULL == routing_tables){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }
    
    printf("{\"table_list\":[");
    for(i = 0; i < routing_tables->num; i++){
        if(routing_tables->pinfo[i].enable == 1){
            if(t == 1){
                printf(",");
            }
            
            printf("{\"id\":\"%u\",\"value\":\"%s\"}",routing_tables->pinfo[i].id,routing_tables->pinfo[i].value);
            t = 1;
        }
    }
    printf("],\"if_wan_list\":[");

    t=0;
    for(i = 0; i < wans->num; i++){
        if(wans->pinfo[i].enable == 0)
            continue;
        
        if(t == 1){
            printf(",");
        }
        
        printf("{\"id\":\"%u\",\"ifname\":\"%s\"}",wans->pinfo[i].id,wans->pinfo[i].ifname);
        t = 1;
    }
    
    printf("],\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&routing_tables);
    return;
}

/**************************************edit_show**************************************/



/**************************************edit_save**************************************/
void firewall_policy_edit_save(void)
{
    int iserror=0,index=0,size=0,ret_value=0,firewall_enable=1;
    int old_id=2,enable = 0,i=0,iface_count=0;

    char dot = ',';
    char old_id_str[DIGITAL_LENGTH]={"1"};
    char enable_str[DIGITAL_LENGTH]={"0"};
    char src[STRING_LENGTH]={0};
    char dst[STRING_LENGTH] = {0};
    char isp[DIGITAL_LENGTH]={"1"};
    char proto[STRING_LENGTH]={0};
    char session[STRING_LENGTH]={0};
    char iface[LONG_STRING_LENGTH]={0};
    char action[LITTER_LENGTH]={"3"};
    char timed[LITTER_LENGTH] = {0};
    char week[STRING_LENGTH] = {0};
    char day[STRING_LENGTH] = {0};
    char in_limit[LITTER_LENGTH] = {0};
    char out_limit[LITTER_LENGTH] = {0};
    char comment[COMMENT_LENGTH] = {0};
    
    struct mgtcgi_xml_firewalls_policy *firewall_policy = NULL;

#if CGI_FORM_STRING
    cgiFormString("old_id", old_id_str, sizeof(old_id_str));
    cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("src", src, sizeof(src));
    cgiFormString("dst", dst, sizeof(dst));
    cgiFormString("isp", isp, sizeof(isp));
    cgiFormString("proto", proto, sizeof(proto));
    cgiFormString("session", session, sizeof(session));
    cgiFormString("iface", iface, sizeof(iface));
    cgiFormString("action", action, sizeof(action));
    cgiFormString("timed", timed, sizeof(timed));
    cgiFormString("week", week, sizeof(week));
    cgiFormString("day", day, sizeof(day));
    cgiFormString("in_limit", in_limit, sizeof(in_limit));
    cgiFormString("out_limit", out_limit, sizeof(out_limit));
    cgiFormString("comment", comment, sizeof(comment));
#endif

    if((strlen(old_id_str) < STRING_LITTLE_LENGTH)    
        || (strlen(iface) < STRING_LITTLE_LENGTH)    
        || (strlen(action) < STRING_LITTLE_LENGTH)  
        || (strlen(enable_str) < STRING_LITTLE_LENGTH)
        || (strlen(timed) < STRING_LITTLE_LENGTH)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strlen(src) != 0){
        if(check_ipaddr(src) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dst) != 0){
        if(check_ipaddr(dst) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    enable = atoi(enable_str);
    if (!(0 <= enable && enable <= 1)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strcmp(timed,"on") == 0){
        if((strlen(week) < STRING_LITTLE_LENGTH)
            || (strlen(day) < STRING_LITTLE_LENGTH)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }
    
    old_id = atoi(old_id_str);
    if(!((old_id > 0) && (old_id < 32))){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    //最多选择8个接口
    for(i=0;i<strlen(iface);i++){
        if(iface[i] == dot)
            iface_count++;
    }
    if(iface_count > 7){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&firewall_policy, &size);
    if (NULL == firewall_policy){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }
    firewall_enable = firewall_policy->enable;
    
    iserror = -1;
    for (index=0; index < firewall_policy->num; index++){
        if (firewall_policy->pinfo[index].id == old_id){
            firewall_policy->pinfo[index].id = old_id;
            firewall_policy->pinfo[index].enable = enable;
            strncpy(firewall_policy->pinfo[index].src, src, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].dst, dst, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].isp, isp, DIGITAL_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].proto, proto, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].session, session, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].action, action, LITTER_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].timed, timed, LITTER_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].week, week, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].day, day, STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].in_limits, in_limit, LITTER_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].out_limits, out_limit, LITTER_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].iface, iface, LONG_STRING_LENGTH-1);
            strncpy(firewall_policy->pinfo[index].comment, comment, COMMENT_LENGTH-1);

            iserror = 0;
            break;
        }
    }
    if (iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void*)firewall_policy, size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(apply_firewallpolicy(firewall_enable) != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    ret_value = copy_xmlconf_file(working_xmlconf_path, flash_xmlconf_path);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    free_xml_node((void*)&firewall_policy);
    return;

}

/**************************************add_save**************************************/
void firewall_policy_add_save(void)
{
    int iserror=0,ret_value=0;
    int old_num=0,new_num=0,old_size=0,new_size=0;
    int enable=0,max_id=1,i=0,iface_count=0;

    char dot = ',';
    char enable_str[DIGITAL_LENGTH]={"0"};
    char src[STRING_LENGTH]={0};
    char dst[STRING_LENGTH] = {0};
    char isp[DIGITAL_LENGTH]={"1"};
    char proto[STRING_LENGTH]={0};
    char session[STRING_LENGTH]={0};
    char iface[LONG_STRING_LENGTH]={0};
    char action[LITTER_LENGTH]={"3"};
    char timed[LITTER_LENGTH] = {0};
    char week[STRING_LENGTH] = {0};
    char day[STRING_LENGTH] = {0};
    char in_limit[LITTER_LENGTH] = {0};
    char out_limit[LITTER_LENGTH] = {0};
    char comment[COMMENT_LENGTH] = {0};

    struct mgtcgi_xml_firewalls_policy *old_nodes = NULL;
    struct mgtcgi_xml_firewalls_policy *new_nodes = NULL;

#if CGI_FORM_STRING
    cgiFormString("enable", enable_str, sizeof(enable_str));
    cgiFormString("src", src, sizeof(src));
    cgiFormString("dst", dst, sizeof(dst));
    cgiFormString("isp", isp, sizeof(isp));
    cgiFormString("proto", proto, sizeof(proto));
    cgiFormString("session", session, sizeof(session));
    cgiFormString("iface", iface, sizeof(iface));
    cgiFormString("action", action, sizeof(action));
    cgiFormString("timed", timed, sizeof(timed));
    cgiFormString("week", week, sizeof(week));
    cgiFormString("day", day, sizeof(day));
    cgiFormString("in_limit", in_limit, sizeof(in_limit));
    cgiFormString("out_limit", out_limit, sizeof(out_limit));
    cgiFormString("comment", comment, sizeof(comment));
#endif 

    if((strlen(iface) < STRING_LITTLE_LENGTH)    
        || (strlen(action) < STRING_LITTLE_LENGTH)  
        || (strlen(enable_str) < STRING_LITTLE_LENGTH)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strlen(src) != 0){
        if(check_ipaddr(src) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    if(strlen(dst) != 0){
        if(check_ipaddr(dst) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    enable = atoi(enable_str);
    if (!(0 <= enable && enable <= 1)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    if(strcmp(timed,"on") == 0){
        if((strlen(week) < STRING_LITTLE_LENGTH)
            || (strlen(day) < STRING_LITTLE_LENGTH)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
        }
    }

    //最多选择8个接口
    for(i=0;i<strlen(iface);i++){
        if(iface[i] == dot)
            iface_count++;
    }
    if(iface_count > 8){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    
    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&old_nodes, &old_size);
    if (NULL == old_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    //将数据添加到结构中
    old_num = old_nodes->num;
    new_num = old_num + 1;  
    max_id= new_num;

    old_size = sizeof(struct mgtcgi_xml_firewalls_policy) + 
                old_num * 
                sizeof(struct mgtcgi_xml_firewall_policy_info);
    new_size = sizeof(struct mgtcgi_xml_firewalls_policy) + 
                new_num * 
                sizeof(struct mgtcgi_xml_firewall_policy_info);

    new_nodes = (struct mgtcgi_xml_firewalls_policy *)malloc(new_size);
    if (NULL == new_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    memcpy(new_nodes, old_nodes, old_size);

    new_nodes->enable = old_nodes->enable;
    new_nodes->num = new_num;
    new_nodes->pinfo[old_num].id = max_id;
    new_nodes->pinfo[old_num].enable = enable;
    strncpy(new_nodes->pinfo[old_num].src, src, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].dst, dst, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].isp, isp, DIGITAL_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].proto, proto, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].session, session, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].iface, iface, LONG_STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].action, action, LITTER_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].timed, timed, LITTER_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].week, week, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].day, day, STRING_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].in_limits, in_limit, LITTER_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].out_limits, out_limit, LITTER_LENGTH-1);
    strncpy(new_nodes->pinfo[old_num].comment, comment, COMMENT_LENGTH-1);

    ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void*)new_nodes, new_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    } 
    else {
        ret_value = copy_xmlconf_file(working_xmlconf_path, flash_xmlconf_path);
        if (ret_value != 0){
            iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
            goto ERROR_EXIT;
        }
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
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    return;
}

/*****************************************move*****************************************/

void firewall_policy_move_save(void)
{
    int iserror=-1,ret_value=-1,num=0,size,new_size=0,start_id=1;
    int old_id=0,new_id=0,i=0,j=0;
    char old_id_str[DIGITAL_LENGTH] = {0};
    char new_id_str[DIGITAL_LENGTH] = {0};
    struct mgtcgi_xml_firewalls_policy *old_nodes = NULL;
    struct mgtcgi_xml_firewalls_policy *new_nodes = NULL;

#if CGI_FORM_STRING	
    cgiFormString("old_id", old_id_str, sizeof(old_id_str));
    cgiFormString("new_id", new_id_str, sizeof(new_id_str));
#endif

    if((strlen(old_id_str) < STRING_LITTLE_LENGTH) ||
       (strlen(new_id_str) < STRING_LITTLE_LENGTH)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&old_nodes, &size);
    if (NULL == old_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    old_id = atoi(old_id_str);
    if(old_id >= old_nodes->num){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    new_id = atoi(new_id_str);
    if((old_nodes->num == 0) || (old_nodes->num == 1)){
        goto SUCCESS_EXIT;
    }

    if(old_id == new_id){
        goto SUCCESS_EXIT;
    }

    if(new_id > (old_nodes->num-1)){
        new_id = old_nodes->num-1;
    }   

    num = old_nodes->num;
    new_size = sizeof(struct mgtcgi_xml_firewalls_policy) + 
                num * 
                sizeof(struct mgtcgi_xml_firewall_policy_info);

    new_nodes = (struct mgtcgi_xml_firewalls_policy *)malloc(new_size);
    if (NULL == new_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    new_nodes->enable = old_nodes->enable;
    new_nodes->num = num;

    /*********交换数据*********/ 
    if(old_id > new_id){
        for(i=0,j=0;i<num && j<old_nodes->num;){
            if(i==new_id){
                new_nodes->pinfo[i].id = start_id;
                new_nodes->pinfo[i].enable = old_nodes->pinfo[old_id].enable;
                strncpy(new_nodes->pinfo[i].src, old_nodes->pinfo[old_id].src, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].dst, old_nodes->pinfo[old_id].dst, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].isp, old_nodes->pinfo[old_id].isp, DIGITAL_LENGTH-1);
                strncpy(new_nodes->pinfo[i].proto, old_nodes->pinfo[old_id].proto, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].session, old_nodes->pinfo[old_id].session, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].iface, old_nodes->pinfo[old_id].iface, LONG_STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].action, old_nodes->pinfo[old_id].action, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].timed, old_nodes->pinfo[old_id].timed, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].week, old_nodes->pinfo[old_id].week, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].day, old_nodes->pinfo[old_id].day, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].in_limits, old_nodes->pinfo[old_id].in_limits, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].out_limits, old_nodes->pinfo[old_id].out_limits, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].comment, old_nodes->pinfo[old_id].comment, COMMENT_LENGTH-1);
                i++;
                start_id++;
                continue;
            }

            if(j==old_id){
                j++;
                continue;
            }

            new_nodes->pinfo[i].id = start_id;
            new_nodes->pinfo[i].enable = old_nodes->pinfo[j].enable;
            strncpy(new_nodes->pinfo[i].src, old_nodes->pinfo[j].src, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].dst, old_nodes->pinfo[j].dst, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].isp, old_nodes->pinfo[j].isp, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[i].proto, old_nodes->pinfo[j].proto, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].session, old_nodes->pinfo[j].session, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].iface, old_nodes->pinfo[j].iface, LONG_STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].action, old_nodes->pinfo[j].action, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].timed, old_nodes->pinfo[j].timed, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].week, old_nodes->pinfo[j].week, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].day, old_nodes->pinfo[j].day, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].in_limits, old_nodes->pinfo[j].in_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].out_limits, old_nodes->pinfo[j].out_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].comment, old_nodes->pinfo[j].comment, COMMENT_LENGTH-1);
            i++;
            j++;
            start_id++;
        }
    }
    else{
        for(i=0,j=0;i < num;){
            
            
            if(i==new_id){
                new_nodes->pinfo[i].id = start_id;
                new_nodes->pinfo[i].enable = old_nodes->pinfo[old_id].enable;
                strncpy(new_nodes->pinfo[i].src, old_nodes->pinfo[old_id].src, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].dst, old_nodes->pinfo[old_id].dst, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].isp, old_nodes->pinfo[old_id].isp, DIGITAL_LENGTH-1);
                strncpy(new_nodes->pinfo[i].proto, old_nodes->pinfo[old_id].proto, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].session, old_nodes->pinfo[old_id].session, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].iface, old_nodes->pinfo[old_id].iface, LONG_STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].action, old_nodes->pinfo[old_id].action, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].timed, old_nodes->pinfo[old_id].timed, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].week, old_nodes->pinfo[old_id].week, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].day, old_nodes->pinfo[old_id].day, STRING_LENGTH-1);
                strncpy(new_nodes->pinfo[i].in_limits, old_nodes->pinfo[old_id].in_limits, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].out_limits, old_nodes->pinfo[old_id].out_limits, LITTER_LENGTH-1);
                strncpy(new_nodes->pinfo[i].comment, old_nodes->pinfo[old_id].comment, COMMENT_LENGTH-1);
                i++;
                start_id++;
                continue;
            }

            if(j==old_id){
                j++;
                continue;
            }

            if(!(j < old_nodes->num))
                break;

            new_nodes->pinfo[i].id = start_id;
            new_nodes->pinfo[i].enable = old_nodes->pinfo[j].enable;
            strncpy(new_nodes->pinfo[i].src, old_nodes->pinfo[j].src, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].dst, old_nodes->pinfo[j].dst, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].isp, old_nodes->pinfo[j].isp, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[i].proto, old_nodes->pinfo[j].proto, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].session, old_nodes->pinfo[j].session, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].iface, old_nodes->pinfo[j].iface, LONG_STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].action, old_nodes->pinfo[j].action, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].timed, old_nodes->pinfo[j].timed, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].week, old_nodes->pinfo[j].week, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].day, old_nodes->pinfo[j].day, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[i].in_limits, old_nodes->pinfo[j].in_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].out_limits, old_nodes->pinfo[j].out_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[i].comment, old_nodes->pinfo[j].comment, COMMENT_LENGTH-1);
            i++;
            j++;
            start_id++;
        }
    }

    ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void*)new_nodes, new_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }
    else {
        ret_value = copy_xmlconf_file(working_xmlconf_path, flash_xmlconf_path);
        if (ret_value != 0){
            iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
            goto ERROR_EXIT;
        }
    }


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
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    return;

}

/**************************************remove_save**************************************/
void firewall_policy_remove_save(void)
{
    int iserror=0,index=0,size=0,ret_value=0;
    int old_size=0,new_size=0,new_index=0,old_num=0,new_num=0;
    char id_str[STRING_LENGTH]={"vlan1"};
    int id=0,min_id=1,firewall_enable=1;
    struct mgtcgi_xml_firewalls_policy *old_nodes = NULL;
    struct mgtcgi_xml_firewalls_policy *new_nodes = NULL;

#if CGI_FORM_STRING	
    cgiFormString("policy_id", id_str, sizeof(id_str));
#endif
    if((strlen(id_str) < STRING_LITTLE_LENGTH)){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }
    id = atoi(id_str);

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&old_nodes, &size);
    if (NULL == old_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }
    firewall_enable = old_nodes->enable;

    old_num = old_nodes->num;
    new_num = old_num - 1;  
    if (new_num < 0)
        new_num = 0;

    old_size = sizeof(struct mgtcgi_xml_firewalls_policy) + 
                old_num * 
                sizeof(struct mgtcgi_xml_firewall_policy_info);
    new_size = sizeof(struct mgtcgi_xml_firewalls_policy) + 
                new_num * 
                sizeof(struct mgtcgi_xml_firewall_policy_info);

    new_nodes = (struct mgtcgi_xml_firewalls_policy *)malloc(new_size);
    if (NULL == new_nodes){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    memset(new_nodes, 0, new_size);
    new_nodes->enable = old_nodes->enable;
    new_nodes->num = new_num;

    iserror = -1;
    for (new_index=0,index=0; index < old_nodes->num; index++){
        if (old_nodes->pinfo[index].id == id){
            iserror = 0;
            continue;
        }
        if (new_index < new_nodes->num){
            new_nodes->pinfo[new_index].id = min_id;
            new_nodes->pinfo[new_index].enable = old_nodes->pinfo[index].enable;
            strncpy(new_nodes->pinfo[new_index].src, old_nodes->pinfo[index].src, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].dst, old_nodes->pinfo[index].dst, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].isp, old_nodes->pinfo[index].isp, DIGITAL_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].proto, old_nodes->pinfo[index].proto, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].session, old_nodes->pinfo[index].session, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].iface, old_nodes->pinfo[index].iface, LONG_STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].action, old_nodes->pinfo[index].action, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].timed, old_nodes->pinfo[index].timed, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].week, old_nodes->pinfo[index].week, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].day, old_nodes->pinfo[index].day, STRING_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].in_limits, old_nodes->pinfo[index].in_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].out_limits, old_nodes->pinfo[index].out_limits, LITTER_LENGTH-1);
            strncpy(new_nodes->pinfo[new_index].comment, old_nodes->pinfo[index].comment, COMMENT_LENGTH-1);
        }

        new_index++;
        min_id++;
    }

    if (iserror != 0){
        iserror = MGTCGI_NO_TARGET_ERR;
        goto ERROR_EXIT;
    }

    ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void*)new_nodes, new_size);
    if (ret_value < 0){
        iserror = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(apply_firewallpolicy(firewall_enable) != 0){
        iserror = MGTCGI_APPLY_ERR;
        goto ERROR_EXIT;
    }

    ret_value = copy_xmlconf_file(working_xmlconf_path, flash_xmlconf_path);
    if (ret_value != 0){
        iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
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
        free_xml_node((void*)&new_nodes);
        new_nodes = NULL;
    }
    return;

}

/**************************************cmd_save**************************************/

void firewall_policy_cmd(void)
{
    int iserror=0,ret_value=0,size=0,enable=0;
    char command[STRING_LENGTH]={0};
    char enable_str[STRING_LENGTH]={"0"};
    struct mgtcgi_xml_firewalls_policy *firewalllolicy = NULL;
        
#if CGI_FORM_STRING
    cgiFormString("command", command, sizeof(command));
    cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

    if(strlen(command) < STRING_LITTLE_LENGTH){
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    enable = atoi(enable_str);
    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void**)&firewalllolicy, &size);
    if (NULL == firewalllolicy){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    if(strcmp(command,"apply")== 0){
        ret_value = apply_firewallpolicy(firewalllolicy->enable);
    	if (ret_value != 0){
    		iserror = MGTCGI_APPLY_ERR;
    		goto ERROR_EXIT;
    	}
    }
    else if(strcmp(command,"status")== 0){
        firewalllolicy->enable = enable;
        ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_FIREWALLPOLICY, (void*)firewalllolicy, size);
        if (ret_value < 0){
            iserror = MGTCGI_WRITE_FILE_ERR;
            goto ERROR_EXIT;
        }
        else{
            ret_value = copy_xmlconf_file(working_xmlconf_path, flash_xmlconf_path);
            if (ret_value != 0){
                iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
                goto ERROR_EXIT;
            }
        }
    }
    else{
        iserror = MGTCGI_PARAM_ERR;
        goto ERROR_EXIT;
    }

    printf("{\"iserror\":0,\"msg\":\"\"}");
    goto FREE_EXIT;
ERROR_EXIT:
    printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
    goto FREE_EXIT;
FREE_EXIT:
    if(firewalllolicy){
        free_xml_node((void*)&firewalllolicy);
    }
    return;
}


