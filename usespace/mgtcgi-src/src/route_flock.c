#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../include/mgtcgi_xmlconfig.h"
#include "../include/route_xmlwrapper4c.h"


#ifndef SIGCTOM
#define SIGCTOM     43
#endif

pid_t tmp;
int a = 0;

void sig_router_func()
{
   a++;
   printf("a:%d\n",a);
   sleep(7);
   return;
}

int main(void)
{
/*
    int len = 0;
    struct mgtcgi_xml_snat_signals *test = NULL;

    len = sizeof(struct mgtcgi_xml_snat_signals) + sizeof(struct mgtcgi_xml_snat_signals_info) * 2;
    test = (struct mgtcgi_xml_snat_signals *)malloc(len);
    memset(test,0,len);

    test->num = 2;
    test->pinfo[0].enable = 1;
    strcpy(test->pinfo[0].protocol,"tcp");
    strcpy(test->pinfo[0].comment,"sss");
    strcpy(test->pinfo[0].src,"1.1.1.1");
    strcpy(test->pinfo[0].wanip,"2.2.2.2");
    strcpy(test->pinfo[0].wanport,"1000,1200");
    strcpy(test->pinfo[0].ifname,"WAN0");

    test->pinfo[1].enable = 1;
    strcpy(test->pinfo[1].protocol,"all");
    strcpy(test->pinfo[1].comment,"aaa");
    strcpy(test->pinfo[1].src,"3.3.3.3");
    strcpy(test->pinfo[1].wanip,"4.4.4.4");
    strcpy(test->pinfo[1].wanport,"5000-6000");
    strcpy(test->pinfo[1].ifname,"WAN0");

    save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SIGNAL_SNATS, (void *)test, len);

    FILE *fp;
    fp =fopen("./test","r");
    if(fp == NULL){
        printf("error!\n");
    }
    else{
        printf("success!\n");
    }*/

    unsigned int data = 0;
    printf("data: %u\n",data);
    return (0);
}
