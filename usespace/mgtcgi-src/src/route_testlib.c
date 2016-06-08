#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include "../include/route_xmlwrapper4c.h"
//#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"


/*
	Брвы:
	 gcc -Wall -o route_testlib  -L. -l xmlwrapper4c  route_testlib.c
*/

#define ROUTE_CONFIG_FILE	"/etc/route_config.xml"

int main(int argc,char** argv)
{
	int size = 1;
	int index = 1; 
	int count = 1;
	int i = 1;
//	int x = 1;
	int ret_value = 1;
	struct mgtcgi_xml_dns *dns = NULL;
	struct mgtcgi_xml_interface_lans *lan = NULL;
	struct mgtcgi_xml_interface_wans *wan = NULL;
	struct mgtcgi_xml_pppd_authenticate *pppd = NULL;
	struct mgtcgi_xml_adsl_clients *adsl = NULL;
	struct mgtcgi_xml_l2tpvpn_clients *l2tpvpn = NULL;
	struct mgtcgi_xml_routings * routing = NULL;
	struct mgtcgi_xml_route_rules * rule = NULL;
	struct mgtcgi_xml_nat_dnats * dnat = NULL;
	struct mgtcgi_xml_nat_snats * snat = NULL;
	struct mgtcgi_xml_group_array * group = NULL;
	struct mgtcgi_xml_load_balancings *lb_group = NULL;
    struct mgtcgi_xml_dhcpd_servers *dhcpd = NULL;

    while(1){
        printf("                  The %d times\n",count);
        #if 1
#if 1
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void**)&dns, &size);
    if(dns == NULL){
        printf("Get dns xml config error\n");
        return 1;
    }
	
	printf("-----------MGTCGI_TYPE_DNS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", dns);
	printf("dns->dns_primary:%s\n", dns->dns_primary);
	printf("dns->dns_secondary:%s\n", dns->dns_secondary);
	printf("dns->dns_thirdary:%s\n", dns->dns_thirdary);

	printf("----- save ------");
	strcpy(dns->dns_thirdary, "61.177.7.1");
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DNS, (void*)dns, size);
	printf("save ret_value:%d\n", ret_value);
    if(ret_value < 0){
        printf("Save dns xml config error\n");
        return 1;
    }

	free_route_xml_node((void*)&dns);
	printf("free ptr:%p\n", dns);

#endif

#if 1  
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
	printf("-----------MGTCGI_TYPE_INTERFACE_LANS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", lan);
    if(lan == NULL){
        printf("Get lan xml config error\n");
        return 1;
    }

	if (lan != NULL){
		printf("lan->num:%d\n", lan->num);
		for (index=0; index < lan->num; index++){
			printf("lan->pinfo[%d].ifname:%s\n", index, lan->pinfo[index].ifname);
			printf("lan->pinfo[%d].ip:%s\n", index, lan->pinfo[index].ip);
			printf("lan->pinfo[%d].mask:%s\n", index, lan->pinfo[index].mask);
			printf("lan->pinfo[%d].comment:%s\n", index, lan->pinfo[index].comment);
            printf("lan->pinfo[%d].aliases:%s\n", index, lan->pinfo[index].aliases);
            printf("lan->pinfo[%d].mac:%s\n", index, lan->pinfo[index].mac);
		}
	}

	printf("----- save ------");
	for (index=0; index < lan->num; index++){
		strcpy(lan->pinfo[index].aliases, "172.168.0.254/24");
	}
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void*)lan, size);
	printf("save ret_value:%d\n", ret_value);
    if(ret_value < 0){
        printf("Save lan xml config error\n");
        return 1;
    }

	free_route_xml_node((void*)&lan);
	printf("free ptr:%p\n", lan);
	
#endif

#if 1 
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void**)&wan, &size);
	printf("-----------MGTCGI_TYPE_INTERFACE_WANS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", wan);
    if(wan == NULL){
        printf("Get wan xml config error\n");
        return 1;
    }

	if (wan != NULL){
		printf("wan->num:%d\n", wan->num);
		for (index=0; index < wan->num; index++){

			printf("wan->pinfo[%d].ifname:%s\n", index, wan->pinfo[index].ifname);
			printf("wan->pinfo[%d].access:%s\n", index, wan->pinfo[index].access);
			printf("wan->pinfo[%d].ip:%s\n", index, wan->pinfo[index].ip);
			printf("wan->pinfo[%d].mask:%s\n", index, wan->pinfo[index].mask);
			printf("wan->pinfo[%d].gateway:%s\n", index, wan->pinfo[index].gateway);
			printf("wan->pinfo[%d].defaultroute:%d\n", index, wan->pinfo[index].defaultroute);
			printf("wan->pinfo[%d].comment:%s\n", index, wan->pinfo[index].comment);
            printf("wan->pinfo[%d].mac:%s\n", index,wan->pinfo[index].mac);
            printf("wan->pinfo[%d].newmac:%s\n", index, wan->pinfo[index].newmac);
            printf("wan->pinfo[%d].weight:%d\n", index, wan->pinfo[index].weight);
		}
	}

	printf("----- save ------");
	for (index=0; index < wan->num; index++){
		strcpy(wan->pinfo[index].ip, "192.168.0.113");
	}
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_WANS, (void*)wan, size);
	printf("save ret_value:%d\n", ret_value);
    if(ret_value < 0){
        printf("Save wan xml config error\n");
        return 1;
    }

	free_route_xml_node((void*)&wan);
	printf("free ptr:%p\n", wan);
		
#endif

#if 1 
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPPD_AUTHENTICATE, (void**)&pppd, &size);
	printf("-----------MGTCGI_TYPE_PPPD_AUTHENTICATE-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", pppd);
    if(pppd == NULL){
        printf("Get pppd xml config error\n");
        return 1;
    }
    
	printf("pppd->pap:%d\n", pppd->pap);
	printf("pppd->chap:%d\n", pppd->chap);
	printf("pppd->mschap:%d\n", pppd->mschap);
	printf("pppd->mschapv2:%d\n", pppd->mschapv2);
	printf("pppd->eap:%d\n",pppd->eap);

	printf("----- save ------");
	pppd->pap = 1;
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_PPPD_AUTHENTICATE, (void*)pppd, size);
	printf("save ret_value:%d\n", ret_value);
    if(ret_value < 0){
        printf("Save pppd xml config error\n");
        return 1;
    }

	free_route_xml_node((void*)&pppd);
	printf("free ptr:%p\n" , pppd);
#endif

#if 1 
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void**)&adsl, &size);
	printf("-----------MGTCGI_TYPE_ADSL_CLIENTS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", adsl);
    if(adsl == NULL){
        printf("Get adsl xml config error\n");
        return 1;
    }
	
	if (adsl != NULL){
		printf("adsl->num:%d\n", adsl->num);
		for (index=0; index < adsl->num; index++){
	
			printf("adsl->pinfo[%d].name:%s\n", index, adsl->pinfo[index].name);
			printf("adsl->pinfo[%d].ifname:%s\n", index, adsl->pinfo[index].ifname);
			printf("adsl->pinfo[%d].username:%s\n", index, adsl->pinfo[index].username);
			printf("adsl->pinfo[%d].password:%s\n", index, adsl->pinfo[index].password);
			printf("adsl->pinfo[%d].servername:%s\n", index, adsl->pinfo[index].servername);
			printf("adsl->pinfo[%d].mtu:%d\n", index, adsl->pinfo[index].mtu);
			printf("adsl->pinfo[%d].peerdns:%d\n", index, adsl->pinfo[index].peerdns);
			printf("adsl->pinfo[%d].enable:%d\n", index, adsl->pinfo[index].enable);
			printf("adsl->pinfo[%d].comment:%s\n", index, adsl->pinfo[index].comment);
		}

		printf("----- save ------");
		for (index=0; index < adsl->num; index++){
			strcpy(adsl->pinfo[index].username, "111");
            strcpy(adsl->pinfo[index].password, "111");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ADSL_CLIENTS, (void*)adsl, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save adsl xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&adsl);
	printf("free ptr:%p\n", adsl);
			
#endif

#if 1
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void**)&l2tpvpn, &size);
	printf("-----------MGTCGI_TYPE_L2TPVPN_CLIENTS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", l2tpvpn);
    if(l2tpvpn == NULL){
        printf("Get l2tpvpn xml config error\n");
        return 1;
    }

	if (l2tpvpn != NULL){
		printf("l2tpvpn->num:%d\n", l2tpvpn->num);
		for (index=0; index < l2tpvpn->num; index++){

			printf("l2tpvpn->pinfo[%d].name:%s\n", index, l2tpvpn->pinfo[index].name);
			printf("l2tpvpn->pinfo[%d].ifname:%s\n", index, l2tpvpn->pinfo[index].ifname);
			printf("l2tpvpn->pinfo[%d].username:%s\n", index, l2tpvpn->pinfo[index].username);
			printf("l2tpvpn->pinfo[%d].password:%s\n", index, l2tpvpn->pinfo[index].password);
			printf("l2tpvpn->pinfo[%d].servername:%s\n", index, l2tpvpn->pinfo[index].servername);
			printf("l2tpvpn->pinfo[%d].mtu:%d\n", index, l2tpvpn->pinfo[index].mtu);
			printf("l2tpvpn->pinfo[%d].peerdns:%d\n", index, l2tpvpn->pinfo[index].peerdns);
			printf("l2tpvpn->pinfo[%d].enable:%d\n", index, l2tpvpn->pinfo[index].enable);
			printf("l2tpvpn->pinfo[%d].comment:%s\n", index, l2tpvpn->pinfo[index].comment);
		}
		
		printf("----- save ------");
		for (index=0; index < l2tpvpn->num; index++){
			strcpy(l2tpvpn->pinfo[index].comment, "chtest");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_L2TPVPN_CLIENTS, (void*)l2tpvpn, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save l2tpvpn xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&l2tpvpn);
	printf("free ptr:%p\n", l2tpvpn);
			
#endif	

#if 1
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void**)&routing, &size);
	printf("-----------MGTCGI_TYPE_ROUTINGS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", routing);
    if(routing == NULL){
        printf("Get routing xml config error\n");
        return 1;
    }

	if (routing != NULL){
		printf("routing->num:%d\n", routing->num);
		for (index=0; index < routing->num; index++){
		
			printf("routing->pinfo[%d].id:%d\n", index, routing->pinfo[index].id);
			printf("routing->pinfo[%d].dst:%s\n", index, routing->pinfo[index].dst);
			printf("routing->pinfo[%d].gateway:%s\n", index, routing->pinfo[index].gateway);
			printf("routing->pinfo[%d].priority:%d\n", index, routing->pinfo[index].priority);
			printf("routing->pinfo[%d].ifname:%s\n", index, routing->pinfo[index].ifname);
			printf("routing->pinfo[%d].enable:%d\n", index, routing->pinfo[index].enable);
            printf("routing->pinfo[%d].rule:%s\n", index, routing->pinfo[index].rule);
            printf("routing->pinfo[%d].table:%s\n", index, routing->pinfo[index].table);
			printf("routing->pinfo[%d].comment:%s\n", index, routing->pinfo[index].comment);
		}

		printf("----- save ------");
		for (index=0; index < routing->num; index++){
			strcpy(routing->pinfo[index].comment, "tel");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTINGS, (void*)routing, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save routing xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&routing);
	printf("free ptr:%p\n", routing);
				
#endif

#if 1
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void**)&rule, &size);
	printf("-----------MGTCGI_TYPE_ROUTE_RULES-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", rule);
    if(rule == NULL){
        printf("Get rule xml config error\n");
        return 1;
    }

	if (rule != NULL){
		printf("rule->num:%d\n", rule->num);
		for (index=0; index < rule->num; index++){
		
			printf("rule->pinfo[%d].name:%s\n", index, rule->pinfo[index].name);
			printf("rule->pinfo[%d].src:%s\n", index, rule->pinfo[index].src);
			printf("rule->pinfo[%d].dst:%s\n", index, rule->pinfo[index].dst);
			printf("rule->pinfo[%d].tos:%s\n", index, rule->pinfo[index].tos);
			printf("rule->pinfo[%d].comment:%s\n", index, rule->pinfo[index].comment);
		}

		printf("----- save ------");
		for (index=0; index < rule->num; index++){
			strcpy(rule->pinfo[index].comment, "10.200.15.160");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_RULES, (void*)rule, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save rule xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&rule);
	printf("free ptr:%p\n", rule);
				
#endif

#if 1 
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void**)&dnat, &size);
	printf("-----------MGTCGI_TYPE_NAT_DNATS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", dnat);
    if(dnat == NULL){
        printf("Get dnat xml config error\n");
        return 1;
    }

	if (dnat != NULL){
		printf("dnat->num:%d\n", dnat->num);
		for (index=0; index < dnat->num; index++){
		
			printf("dnat->pinfo[%d].id:%d\n", index, dnat->pinfo[index].id);
			printf("dnat->pinfo[%d].ifname:%s\n", index, dnat->pinfo[index].ifname);
			printf("dnat->pinfo[%d].protocol:%s\n", index, dnat->pinfo[index].protocol);
			printf("dnat->pinfo[%d].wan_port:%s\n", index, dnat->pinfo[index].wan_port);
			printf("dnat->pinfo[%d].lan_ip:%s\n", index, dnat->pinfo[index].lan_ip);
			printf("dnat->pinfo[%d].lan_port:%s\n", index, dnat->pinfo[index].lan_port);
			printf("dnat->pinfo[%d].enable:%d\n", index, dnat->pinfo[index].enable);
			printf("dnat->pinfo[%d].comment:%s\n", index, dnat->pinfo[index].comment);

		}

		printf("----- save ------");
		for (index=0; index < dnat->num; index++){
			strcpy(dnat->pinfo[index].ifname, "WAN0");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_DNATS, (void*)dnat, size);
	    printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save dnat xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&dnat);
	printf("free ptr:%p\n", dnat);
					
#endif

#if 1 
	ret_value = get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void**)&snat, &size);
	printf("-----------MGTCGI_TYPE_NAT_SNATS-----------\n");
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", snat);
    if(snat == NULL){
        printf("Get snat xml config error\n");
        return 1;
    }

	if (snat != NULL){
		printf("snat->num:%d\n", snat->num);
		for (index=0; index < snat->num; index++){
		
			printf("snat->pinfo[%d].id:%d\n", index, snat->pinfo[index].id);
			printf("snat->pinfo[%d].ifname:%s\n", index, snat->pinfo[index].ifname);
			printf("snat->pinfo[%d].src:%s\n", index, snat->pinfo[index].src);
			printf("snat->pinfo[%d].dst:%s\n", index, snat->pinfo[index].dst);
			printf("snat->pinfo[%d].protocol:%s\n", index, snat->pinfo[index].protocol);
			printf("snat->pinfo[%d].action:%s\n", index, snat->pinfo[index].action);
			printf("snat->pinfo[%d].dstip:%s\n", index, snat->pinfo[index].dstip);
			printf("snat->pinfo[%d].dstport:%s\n", index, snat->pinfo[index].dstport);
			printf("snat->pinfo[%d].enable:%d\n", index, snat->pinfo[index].enable);
			printf("snat->pinfo[%d].comment:%s\n", index, snat->pinfo[index].comment);
		}
		
		printf("----- save ------");
		for (index=0; index < snat->num; index++){
			strcpy(snat->pinfo[index].ifname, "WAN0");
		}
		ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_NAT_SNATS, (void*)snat, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save snat xml config error\n");
            return 1;
        }
	}
	
	free_route_xml_node((void*)&snat);
	printf("free ptr:%p\n", snat);
					
#endif


#if 1
	
	printf("-----------MGTCGI_TYPE_ROUTE_TABLES-----------\n"); 
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void**)&group, &size);
	printf("size:%d\n", size);
	printf("malloc ptr:%p\n", group);
    if(group == NULL){
        printf("Get group xml config error\n");
        return 1;
    }
    
	printf("group->num:%d\n",group->num);
	
	for(index = 0; index < group->num ; index++){
		printf("group->pinfo[%d].group_name:%s\n", index, group->pinfo[index].group_name);
		printf("group->pinfo[%d].comment:%s\n", index, group->pinfo[index].comment);
		printf("group->pinfo[%d].num:%d\n", index, group->pinfo[index].num);
		
		for(i = 0; i < group->pinfo[index].num ; i++){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
		}
		printf("\n");	
	}

	printf("----- save ------");
	for (index=0; index < group->num; index++){
		strcpy(group->pinfo[index].comment, "TEST");
	}
	ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ROUTE_TABLES, (void*)group, size);
		printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save group xml config error\n");
            return 1;
        }
	
	free_route_xml_node((void*)&group);
	printf("free ptr:%p\n", group);
#endif


#if 1
        printf("-----------MGTCGI_TYPE_LOAD_BALANCLINGS-----------\n"); 
        get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void**)&lb_group, &size);
        printf("size:%d\n", size);
        printf("malloc ptr:%p\n", lb_group);
        if(lb_group == NULL){
            printf("Get balancing xml config error\n");
            return 1;
        }
        
        printf("lb_group->num:%d\n",lb_group->num);
        
        for(index=0; index< lb_group->num;index++){
            printf("lb_group->pinfo[%d].name:%s\n", index, lb_group->pinfo[index].name);
            printf("lb_group->pinfo[%d].ifname:%s\n", index, lb_group->pinfo[index].ifname);
            printf("lb_group->pinfo[%d].rulename:%s\n", index, lb_group->pinfo[index].rulename);
            printf("lb_group->pinfo[%d].comment:%s\n", index, lb_group->pinfo[index].comment);
            printf("lb_group->pinfo[%d].weight:%d\n", index, lb_group->pinfo[index].weight);
        }
    
        printf("----- save ------\n");
        for (index=0; index < lb_group->num; index++){
            strcpy(lb_group->pinfo[index].comment, "TEST");
        }
        ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_LOAD_BALANCINGS, (void*)lb_group, size);
        printf("save ret_value:%d\n", ret_value);
        if(ret_value < 0){
            printf("Save balancing xml config error\n");
            return 1;
        }
        
        free_route_xml_node((void*)&lb_group);
        printf("free ptr:%p\n", lb_group);
#endif

#endif
#if 1
    printf("--------------MGTCGI_TYPE_DHCPD_SERVER-----------------\n");
    get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void**)&dhcpd, &size);
    printf("size:%d\n",size);
    printf("malloc dhcpd:%p\n",dhcpd);
    if(dhcpd == NULL){
            printf("Get dhcpd xml config error\n");
            return 1;
    }

    if(dhcpd){
        printf("dhcpd_server num:%d\n",dhcpd->num);
        printf("dhcpd_server enable:%d\n",dhcpd->dhcpd_enable);
        for(index=0;index<dhcpd->num;index++){
            printf("dhcpd->pinfo[%d].name:%s\n", index, dhcpd->pinfo[index].name);
            printf("dhcpd->pinfo[%d].ifname:%s\n", index, dhcpd->pinfo[index].ifname);
            printf("dhcpd->pinfo[%d].range:%s\n", index, dhcpd->pinfo[index].range);
            printf("dhcpd->pinfo[%d].mask:%s\n", index, dhcpd->pinfo[index].mask);
            printf("dhcpd->pinfo[%d].gateway:%s\n", index, dhcpd->pinfo[index].gateway);
            printf("dhcpd->pinfo[%d].dnsname:%s\n", index, dhcpd->pinfo[index].dnsname);
            printf("dhcpd->pinfo[%d].dns:%s\n", index, dhcpd->pinfo[index].dns);
            printf("dhcpd->pinfo[%d].lease_time:%d\n", index, dhcpd->pinfo[index].lease_time);
            printf("dhcpd->pinfo[%d].max_lease_time:%d\n", index, dhcpd->pinfo[index].max_lease_time);
            printf("dhcpd->pinfo[%d].comment:%s\n", index, dhcpd->pinfo[index].comment);
            printf("dhcpd->pinfo[%d].enable:%d\n", index, dhcpd->pinfo[index].enable);
        }

        printf("----- save -------\n");

        for(index=0;index<dhcpd->num;index++){
            dhcpd->pinfo[index].max_lease_time = 43200;
        }

        ret_value = save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_DHCPD_SERVERS, (void*)dhcpd, size);
        printf("save ret_value:%d\n", ret_value); 
        if(ret_value < 0){
            printf("Save dhcpd xml config error\n");
            return 1;
        }
    }
    
    free_route_xml_node((void*)&dhcpd);
    printf("free ptr:%p\n", dhcpd);

#endif
    count++;
  //  sleep(10);
    }
	return 1;	
}

