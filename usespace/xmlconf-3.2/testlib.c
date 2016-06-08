#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>

#include "libxmlwrapper4c/xmlwrapper4c.h"
#include "libxmlwrapper4c/mgtcgi_xmlconfig.h"

#define CONFIG_FILE	"/etc/config.xml"

/*
	Брвы:
	 gcc -o test  -L.  -l xmlwrapper4c  testlib.c
*/

int main(int argc,char** argv)
{
	int size = 0;
	int index = 0; 
	int count = 0;
	int i = 0;
	int x = 0;
	struct mgtcgi_xml_management *mgt = NULL;
	 struct mgtcgi_xml_system * system = NULL;
	 struct mgtcgi_xml_interfaces * interfaces = NULL;
	 struct mgtcgi_xml_networks*networks = NULL;
	 struct mgtcgi_xml_ip_session_limits * ipsession = NULL;
	 struct mgtcgi_xml_group_array * group = NULL;
	 struct mgtcgi_xml_schedules* schedules = NULL;
	 struct mgtcgi_xml_domains* domains = NULL;
	 struct mgtcgi_xml_extensions* extensions = NULL;
	 struct mgtcgi_xml_httpservers* httpservers = NULL;
	 struct mgtcgi_xml_layer7_protocols_show* layer7protocol = NULL;
	 struct mgtcgi_xml_traffics* traffics = NULL;
	 struct mgtcgi_xml_protocols * protocols = NULL;
	 struct mgtcgi_xml_firewalls* firewalls = NULL;
	 struct mgtcgi_xml_dns_policys* dnspolicys = NULL;
	 struct mgtcgi_xml_http_dir_policys* httppolicy = NULL;
	
	while(i == 0)
	{	
        i++;
#if 1  //node 
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_SYSTEM,(void**)&system,&size);
	printf("SYSTEM len = %d \n" , size);
    printf("system ptr = %x \n" , system);
	printf("hostname = %s \n" , system->hostname);
	printf("domainname = %s \n" , system->domainname);
	printf("contact = %s \n" , system->contact);
	printf("password = %s \n" , system->password);
	//save_xml_node(CONFIG_FILE, MGTCGI_TYPE_SYSTEM,(void*)system,size);
	printf("system ptr = %x \n\n" , system);
	free_xml_node((void*)&system);
#endif
	
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_MANAGEMENT,(void**)&mgt,&size);
	printf("MANAGEMENT len = %d \n" , size);
	printf("ip = %s \n" , mgt->ip);
	printf("netmask = %s \n" , mgt->netmask);
	printf("broadcast = %s \n" , mgt->broadcast);
	printf("gateway = %s \n" , mgt->gateway);
	printf("server = %s \n" , mgt->server);
	printf("dns_primary = %s \n" , mgt->dns_primary);
	
	strcpy(mgt->ip,"ip1.1.1.1");
	strcpy(mgt->netmask,"netmask");
	strcpy(mgt->gateway,"gateway ");
	strcpy(mgt->server,"server");
	//save_xml_node(CONFIG_FILE, MGTCGI_TYPE_MANAGEMENT,(void*)mgt,size);
    printf("mgt ptr = %x \n\n" , mgt);
	free_xml_node((void*)&mgt);

#endif
	
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_NETWORKS,(void**)&networks,&size);
	printf("networks len = %d \n" , size);
	count = (size - sizeof(struct mgtcgi_xml_networks)) / 
			sizeof(struct mgtcgi_xml_network_info);
	if((size - sizeof(struct mgtcgi_xml_networks)) % 
			sizeof(struct mgtcgi_xml_network_info) != 0)
        printf("NETWORKS return size error\n");
	
	printf("NETWORKS len = %d \n" , size);	
	printf("NETWORKS count = %d ; return num=%d .\n" , count , networks->num);
	for(index = 0; index < count ; ++index){
		printf("name = %s \n" , networks->pinfo[index].name);
		printf("value = %s \n" , networks->pinfo[index].value);
		printf("range = %s \n" , networks->pinfo[index].range);
		printf("comment = %s \n" , networks->pinfo[index].comment);
		printf("\n");
	}
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_NETWORKS,(void*)networks,size);
    printf("NETWORKS ptr = %x \n\n" , networks);
	free_xml_node((void*)&networks);
#endif

#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_INTERFACES,(void**)&interfaces,&size);
	printf("INTERFACES len = %d \n" , size);	
	printf("INTERFACES num=%d .\n" , count , interfaces->num);
	
	for(index = 0; index < interfaces->num ; ++index){
		printf("name = %s \n" , interfaces->pinfo[index].name);
		printf("mode = %s \n" , interfaces->pinfo[index].mode);
		printf("speed = %d \n" , interfaces->pinfo[index].speed);
		printf("duplex = %s \n" , interfaces->pinfo[index].duplex);
		printf("\n");
	}
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_INTERFACES,(void*)interfaces,size);
    printf("interfaces ptr = %x \n\n" , interfaces);
	free_xml_node((void*)&interfaces);
	
#endif
	
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_IP_SESSION_LIMITS,(void**)&ipsession,&size);	
	
	printf("IP_SESSION_LIMITS len = %d \n" , size); 
	printf("total_limit_num  = %d ; global_limit_num=%d .\n" , 
				ipsession->total_limit_num , ipsession->global_limit_num);
	
	for(index = 0; index < ipsession->total_limit_num ; ++index){
		printf("bridge = %d \n" , ipsession->p_totalinfo[index].bridge);
		printf("limit = %d \n" , ipsession->p_totalinfo[index].limit);
		printf("overhead = %s \n" , ipsession->p_totalinfo[index].overhead);
		printf("\n");
	}
		
	for(index = 0; index < ipsession->global_limit_num ; ++index){
		printf("name = %d \n" , ipsession->p_globalinfo[index].name);
		printf("addr = %s \n" , ipsession->p_globalinfo[index].addr);
		printf("dst = %s \n" , ipsession->p_globalinfo[index].dst);
		printf("per_ip_session_limit = %s \n" , ipsession->p_globalinfo[index].per_ip_session_limit);
		printf("total_session_limit = %s \n" , ipsession->p_globalinfo[index].total_session_limit);
		printf("httplog = %d \n" , ipsession->p_globalinfo[index].httplog);
		printf("sesslog = %d \n" , ipsession->p_globalinfo[index].sesslog);
		printf("httpdirpolicy = %d \n" , ipsession->p_globalinfo[index].httpdirpolicy);
		printf("dnspolicy = %d \n" , ipsession->p_globalinfo[index].dnspolicy);
		printf("action = %s \n" , ipsession->p_globalinfo[index].action);
		printf("bridge = %d \n" , ipsession->p_globalinfo[index].bridge);
		printf("comment = %s \n" , ipsession->p_globalinfo[index].comment);
		printf("\n");
	}
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_IP_SESSION_LIMITS,(void*)ipsession,size);
    printf("ipsession ptr = %x \n\n" , ipsession);
	free_xml_node((void*)&ipsession);
#endif

#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES,(void**)&schedules,&size);	
	
	printf("schedules len = %d \n" , size); 
  	printf("schedules num  = %d ;\n" , schedules->num );

	for(index = 0; index < schedules->num ; ++index){
		printf("name = %s \n" , schedules->pinfo[index].name);
		printf("start_time  = %s \n" , schedules->pinfo[index].start_time);
		printf("stop_time  = %s \n" , schedules->pinfo[index].stop_time);
		printf("days  = %s \n" , schedules->pinfo[index].days);
		printf("comment  = %s \n" , schedules->pinfo[index].comment);
		
		printf("\n");
	}
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES,(void*)schedules,size);
    printf("schedules ptr = %x \n\n" , schedules);
	free_xml_node((void*)&schedules);

#endif

#if 1
	printf("MGTCGI_TYPE_NETWORKS_GROUP 1\n" ); 

	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_NETWORKS_GROUP,(void**)&group,&size);	

	printf("group len = %d \n" , size); 
  	printf("group num  = %d ;\n\n" , group->num );
	
	for(index = 0; index < group->num ; ++index){
		printf("group_name = %s \n" , group->pinfo[index].group_name);
		printf("include num = %d \n\n" , group->pinfo[index].num);
		for(i = 0; i < group->pinfo[index].num ; ++i){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
			printf("-----------\n");
	//sprintf(group->pinfo[index].pinfo[i].name,"testgincludename-%d",i);
		}
		printf("\n");
		
	//sprintf(group->pinfo[index].group_name,"testgroupname-%d",index);
	}
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_NETWORKS_GROUP,(void*)group,size);
    printf("group ptr = %x \n\n" , group);
	free_xml_node((void**)&group);
#endif
	
#if 1	
	printf("MGTCGI_TYPE_SCHEDULES_GROUP \n" ); 
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);	
	printf("group len = %d \n" , size); 
  	printf("group num  = %d ;\n" , group->num );
	
	for(index = 0; index < group->num ; ++index){
		printf("group_name = %s \n" , group->pinfo[index].group_name);
		printf("include num = %d \n" , group->pinfo[index].num);
		for(i = 0; i < group->pinfo[index].num ; ++i){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
			printf("-----------\n");
		}
		printf("\n");
	}
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES_GROUP,(void*)group,size);
    printf("group ptr = %x \n\n" , group);
	free_xml_node((void**)&group);
		
#endif
	
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_DOMAINS,(void**)&domains,&size);	
	
	printf("domains len = %d \n" , size); 
	printf("domains num  = %d \n", domains->num );
	if(size > sizeof(struct mgtcgi_xml_domains)){

		for(index = 0; index < domains->num ; ++index){
			printf("name = %s \n" , domains->pinfo[index].name);
			printf("value  = %s \n" , domains->pinfo[index].value);
			printf("comment  = %s \n" , domains->pinfo[index].comment);
			
			printf("\n");
		}
	}
    printf("%s begin\n",__FUNCTION__);
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_DOMAINS,(void*)domains,size);
    printf("domains ptr = %x \n\n" , domains);
	free_xml_node((void*)&domains);
	
#endif

#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_EXTENSIONS,(void**)&extensions,&size);	
	
	printf("extensions len = %d \n" , size); 
	printf("extensions num  = %d \n", extensions->num );

	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_EXTENSIONS,(void*)extensions,size);
    printf("extensions ptr = %x \n\n" , extensions);
    free_xml_node((void*)&extensions);
	
	
#endif

#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTPSERVERS,(void**)&httpservers,&size);	
	
	printf("httpservers len = %d \n" , size); 
	printf("httpservers num  = %d \n", httpservers->num );

	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTPSERVERS,(void*)httpservers,size);
	free_xml_node((void*)&httpservers);
	printf("httpservers ptr = %x \n\n" , httpservers);
	
#endif

#if 1	
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_DOMAIN_GROUP,(void**)&group,&size);	
	
	printf("MGTCGI_TYPEDOMAIN_GROUP \n" ); 
	printf("group len = %d \n" , size); 
  	printf("group num  = %d ;\n" , group->num );
	
	for(index = 0; index < group->num ; ++index){
		printf("group_name = %s \n" , group->pinfo[index].group_name);
		printf("include num = %d \n" , group->pinfo[index].num);
		for(i = 0; i < group->pinfo[index].num ; ++i){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
			printf("-----------\n");
		}
		printf("\n");
	}
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_DOMAIN_GROUP,(void*)group,size);
	free_xml_node((void**)&group);printf("group ptr = %x \n\n" , group);
		
#endif
	
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_EXTENSION_GROUP,(void**)&group,&size);	
	printf("MGTCGI_TYPE_EXTENSION_GROUP \n" ); 
	printf("group len = %d \n" , size); 
  	printf("group num  = %d ;\n" , group->num );
	
	for(index = 0; index < group->num ; ++index){
		printf("group_name = %s \n" , group->pinfo[index].group_name);
		printf("include num = %d \n" , group->pinfo[index].num);
		for(i = 0; i < group->pinfo[index].num ; ++i){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
			printf("-----------\n");
		}
		printf("\n");
	}
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_EXTENSION_GROUP,(void*)group,size);
	free_xml_node((void**)&group);printf("group ptr = %x \n\n" , group);
	
#endif
		
#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTPSERVER_GROUP,(void**)&group,&size);	
	
	printf("MGTCGI_TYPE_HTTPSERVER_GROUP \n" );
	printf("group len = %d \n" , size); 
  	printf("group num  = %d ;\n" , group->num );
	
	for(index = 0; index < group->num ; ++index){
		printf("group_name = %s \n" , group->pinfo[index].group_name);
		printf("include num = %d \n" , group->pinfo[index].num);
		for(i = 0; i < group->pinfo[index].num ; ++i){
			printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
			printf("-----------\n");
		}
		printf("\n");
	}
	save_xml_node(CONFIG_FILE,  MGTCGI_TYPE_HTTPSERVER_GROUP, (void*)group,size);
    printf("group ptr = %x \n\n", group);
    free_xml_node((void**)&group);
	
#endif

#if 1
	get_xml_nodeforLayer7(CONFIG_FILE,"/etc/l7-protocols.xml",MGTCGI_TYPE_LAYER7_SHOWS,
				"english",(void**)&layer7protocol,&size);	
	
  	printf("layer7 protocol num  = %d ;\n" , layer7protocol->num );

	for(index = 0; index < layer7protocol->num ; ++index){
		printf("::::%d\nname = %s \n" , index,layer7protocol->pinfo[index].name);
		printf("match = %s \n" , layer7protocol->pinfo[index].match);
		printf("type = %s \n" , layer7protocol->pinfo[index].type);
		printf("type_name = %s \n" , layer7protocol->pinfo[index].type_name);
		printf("selected = %d \n" , layer7protocol->pinfo[index].selected);
		printf("comment = %s \n" , layer7protocol->pinfo[index].comment);
		
		printf("\n");
	}
    printf("layer7protocol ptr = %x \n\n" , layer7protocol);
	free_xml_node((void**)&layer7protocol);
#endif

#if 1
	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_TRAFFIC,(void**)&traffics,&size);	
	
	printf("MGTCGI_TYPE_TRAFFIC len = %d \n" , size); 
	printf("num  = %d status=%d\n" , traffics->num , traffics->status);
		
	for(index = 0; index < traffics->num ; ++index){
		printf(
			"<channel classname=\"%s\" parentname=\"%s\" sharename=\"%s\" "	\
			" classtype=\"%d\" rate=\"%s\" priority=\"%d\" limit=\"%d\" "		\
			" default=\"%d\" attribute=\"%d\" weight=\"%d\" bridge=\"%d\" comment=\"%s\" />\n" ,
		
			traffics->pinfo[index].classname,
			traffics->pinfo[index].parentname,
			traffics->pinfo[index].sharename,
			traffics->pinfo[index].classtype,
			traffics->pinfo[index].rate,
			traffics->pinfo[index].priority,
			traffics->pinfo[index].limit,
			traffics->pinfo[index].default_channel,
			traffics->pinfo[index].attribute,
			traffics->pinfo[index].weight,
			traffics->pinfo[index].bridge,
			traffics->pinfo[index].comment);
		
			//traffics->pinfo[index].comment[1] = 'z'; 
		printf("\n");
	}

/*
	struct mgtcgi_xml_traffics* pnew = NULL;
	int newsize = size + sizeof(struct mgtcgi_xml_traffic_channel_info);
 	pnew = (struct mgtcgi_xml_traffics*)malloc(newsize);
	memset((char*)pnew , 0 , newsize);
	memcpy((char*)pnew ,(char*)traffics , size);
	pnew->num = traffics->num +1;
	
//<channel classname="downA-1" parentname="downA" sharename="none" classtype="2"
//rate="200Mbps" priority="7" limit="0" default="1" attribute="0" weight="0" bridge="0" comment="" />
	pnew->pinfo[index].direct = 1;
	strcpy(pnew->pinfo[index].classname , "sss");
	strcpy(pnew->pinfo[index].parentname , "downA");
	strcpy(pnew->pinfo[index].sharename , "none");
	pnew->pinfo[index].classtype = 2;
	strcpy(pnew->pinfo[index].rate , "100Mbps");
	pnew->pinfo[index].priority = 7;
	pnew->pinfo[index].limit = 0;
	pnew->pinfo[index].default_channel = 0;
	pnew->pinfo[index].attribute = 0;
	pnew->pinfo[index].weight = 0;
	pnew->pinfo[index].bridge = 0;
	strcpy(pnew->pinfo[index].comment , "aaa");
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_TRAFFIC,(void*)pnew,newsize);
	
	//save_xml_node(CONFIG_FILE, MGTCGI_TYPE_TRAFFIC,(void*)traffics,size);
	free(pnew);
	free_xml_node((void*)&traffics);
    printf("traffics ptr = %x \n\n" , traffics);
*/
	
	save_xml_node(CONFIG_FILE, MGTCGI_TYPE_TRAFFIC,(void*)traffics,size);
    printf("traffics ptr = %x \n\n" , traffics);
    free_xml_node((void*)&traffics);
#endif 

#if 1
    	get_xml_node(CONFIG_FILE, MGTCGI_TYPE_PROTOCOLS,(void**)&protocols,&size);	
		
		printf("MGTCGI_TYPE_PROTOCOLS len = %d \n" , size); 
		printf("num  = %d \n" , protocols->num );
			
		for(index = 0; index < protocols->num ; ++index){
			printf(
				"<protocol name=\"%s\" proto=\"%s\" dport=\"%s\" sport=\"%s\" match=\"%s\" option=\"%s\" comment=\"%s\"/> \n" ,
			
				protocols->pinfo[index].name,
				protocols->pinfo[index].proto,
				protocols->pinfo[index].dport,
				protocols->pinfo[index].sport,
				protocols->pinfo[index].match,
				protocols->pinfo[index].option,
				protocols->pinfo[index].comment);
			
			printf("\n");
		}

		printf("Read Over\n");
		
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_PROTOCOLS,(void*)protocols,size);
        printf("protocols ptr = %x \n\n" , protocols);
		free_xml_node((void*)&protocols);
		
#endif 
	
#if 1
	    get_xml_nodeforLayer7(CONFIG_FILE,"/etc/l7-protocols.xml",MGTCGI_TYPE_PROTOCOLS_GROUP_CN,
				"chinese",
					(void**)&group,&size);	
		printf("MGTCGI_TYPE_PROTOCOLS_GROUP \n" );
		printf("group len = %d \n" , size); 
	  	printf("group num  = %d ;\n" , group->num );
		
		for(index = 0; index < group->num ; ++index){
			printf("group_name = %s \n" , group->pinfo[index].group_name);
			printf("include num = %d \n" , group->pinfo[index].num);
			printf("-----------\n");
			for(i = 0; i < group->pinfo[index].num ; ++i){
				printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
				printf("-----------\n");
			}
			printf("\n");
		}
			
		#if 0

		char * pnew = NULL;
		int oisize = size;

		char * pnewinclude = NULL;
		struct mgtcgi_xml_group_array* pnewarray = NULL;
		struct mgtcgi_xml_group * pnewgroup = NULL;
		const int numGroup = 20;
		const int numInclude = 10;
		
		int newsize = size + numGroup * sizeof(struct mgtcgi_xml_group) + 
			numGroup* numInclude * sizeof(struct mgtcgi_xml_group_include_info);
		pnew = malloc(newsize);
		memset(pnew , 0 , newsize);
		pnewarray =(struct mgtcgi_xml_group_array*)pnew; 
		pnewinclude = pnew + sizeof(struct mgtcgi_xml_group_array) +
				(numGroup) * sizeof(struct mgtcgi_xml_group);
		
		pnewarray->num = numGroup;

		for(i = 0; i< pnewarray->num; ++i){
			pnewarray->pinfo[i].num = numInclude;
			sprintf(pnewarray->pinfo[i].group_name , "test-%d", i);
			//printf("%s\n",pnewarray->pinfo[i].group_name);
			pnewarray->pinfo[i].pinfo = pnewinclude;
			for(x = 0; x< pnewarray->pinfo[i].num; ++x){
				strcpy(pnewarray->pinfo[i].pinfo[x].name, 
						"MEDIAPLAYER");
			}
			
			pnewinclude +=
				(pnewarray->pinfo[i].num * sizeof(struct mgtcgi_xml_group_include_info));			
		}			

		printf("pnewarray len = %d \n" , newsize); 
		printf("pnewarray num  = %d ;\n\n" , pnewarray->num );
		
		/*for(index = 0; index < pnewarray->num ; ++index){
			printf("pnewgroup = %s \n" , pnewarray->pinfo[index].group_name);
			printf("include num = %d \n\n" , pnewarray->pinfo[index].num);
			for(i = 0; i < pnewarray->pinfo[index].num ; ++i){
				printf("name = %s \n" , pnewarray->pinfo[index].pinfo[i].name);
				printf("-----------\n");
			}
			printf("\n");			
		}*/
		
		printf("\n\ntest\n\n");

		
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_PROTOCOLS_GROUP,(void*)pnew,newsize);
		free_xml_node((void**)&group);printf("group ptr = %x \n\n" , group);
		free(pnew);pnew = NULL;
		#endif
		
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_PROTOCOLS_GROUP,(void*)group,size);
        printf("group ptr = %x \n\n" , group);
		free_xml_node((void**)&group);	
#endif 
	
#if 1
		printf("MGTCGI_TYPE_SCHEDULES_GROUP 1\n" ); 
	
		get_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES_GROUP,(void**)&group,&size);	
	
		printf("group len = %d \n" , size); 
		printf("group num  = %d ;\n\n" , group->num );
	    /*	
		for(index = 0; index < group->num ; ++index){
			printf("group_name = %s \n" , group->pinfo[index].group_name);
			printf("include num = %d \n\n" , group->pinfo[index].num);
			for(i = 0; i < group->pinfo[index].num ; ++i){
				printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
				printf("-----------\n");
			}
			printf("\n");			
		}
		
		char * pnew = NULL;
		int oisize = size;

		char * pnewinclude = NULL;
		struct mgtcgi_xml_group_array* pnewarray = NULL;
		struct mgtcgi_xml_group * pnewgroup = NULL;
		
		int newsize = size + sizeof(struct mgtcgi_xml_group) + 
			3 * sizeof(struct mgtcgi_xml_group_include_info);
		pnew = malloc(newsize);
		memset(pnew , 0 , newsize);
		pnewarray =(struct mgtcgi_xml_group_array*)pnew; 
		pnewinclude = pnew + sizeof(struct mgtcgi_xml_group_array) +
				(group->num+1) * sizeof(struct mgtcgi_xml_group);
		
		pnewarray->num = group->num +1;

		for(i = 0; i< group->num; ++i){
			pnewarray->pinfo[i].num = group->pinfo[i].num;
			strcpy(pnewarray->pinfo[i].group_name , group->pinfo[i].group_name);
			pnewarray->pinfo[i].pinfo = pnewinclude;
			for(x = 0; x< pnewarray->pinfo[i].num; ++x){
				strcpy(pnewarray->pinfo[i].pinfo[x].name, 
						group->pinfo[i].pinfo[x].name);
			}
			
			pnewinclude +=
				(group->pinfo[i].num * sizeof(struct mgtcgi_xml_group_include_info));			
		}
			
		
		pnewgroup = (struct mgtcgi_xml_group*)&(pnewarray->pinfo[group->num]);
		pnewgroup->num =3 ;
		pnewgroup->pinfo = pnewinclude;
		sprintf(pnewgroup->group_name,"testgroup%d" , (char*)pnewgroup);
		strcpy(pnewgroup->pinfo[0].name,"ff");
		strcpy(pnewgroup->pinfo[1].name,"ss");
		strcpy(pnewgroup->pinfo[2].name,"ff");			


		printf("pnewarray len = %d \n" , newsize); 
		printf("pnewarray num  = %d ;\n\n" , pnewarray->num );
		
		for(index = 0; index < pnewarray->num ; ++index){
			printf("pnewgroup = %s \n" , pnewarray->pinfo[index].group_name);
			printf("include num = %d \n\n" , pnewarray->pinfo[index].num);
			for(i = 0; i < pnewarray->pinfo[index].num ; ++i){
				printf("name = %s \n" , pnewarray->pinfo[index].pinfo[i].name);
				printf("-----------\n");
			}
			printf("\n");			
		}
		printf("test\n\n");
		
		for(index = 0; index < pnewarray->num ; ++index){
			printf("==== \n\t %s \n" , (pnewarray->pinfo[index].pinfo));
			
		}

		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_NETWORKS_GROUP,(void*)pnew,newsize);
		*/
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES_GROUP,(void*)group,size);
        printf("group ptr = %x \n\n" , group);
		free_xml_node((void**)&group);		
#endif
	
#if 1
		get_xml_node(CONFIG_FILE,MGTCGI_TYPE_SCHEDULES_GROUP,
						(void**)&group,&size);	
			
			printf("MGTCGI_TYPE_SCHEDULES_GROUP \n" );
			printf("group len = %d \n" , size); 
			printf("group num  = %d ;\n" , group->num );
			
			for(index = 0; index < group->num ; ++index){
				printf("group_name = %s \n" , group->pinfo[index].group_name);
				printf("include num = %d \n" , group->pinfo[index].num);
				printf("-----------\n");
				for(i = 0; i < group->pinfo[index].num ; ++i){
					printf("name = %s \n" , group->pinfo[index].pinfo[i].name);
					printf("-----------\n");
				}
				printf("\n");
			}
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_SCHEDULES_GROUP,(void*)group,size);
        printf("group ptr = %x \n\n" , group);
		free_xml_node((void**)&group);
#endif 
	
#if 1
		get_xml_node(CONFIG_FILE, MGTCGI_TYPE_FIREWALLS,(void**)&firewalls,&size);	
		
		printf("MGTCGI_TYPE_FIREWALLS len = %d \n" , size); 
		printf("num  = %d \n" , firewalls->num );
			
		for(index = 0; index < firewalls->num ; ++index){
			printf(
				"<firewall name=\"%d\" proto=\"%s\" auth=\"%s\" action=\"%s\" " \
				" schedule=\"%s\" session_limit=\"%s\" vlan=\"%s\" src=\"%s\"  "\
				" dst=\"%s\" mac=\"%s\" in_traffic=\"%s\" out_traffic=\"%s\" quota=\"%s\" " \
				" quota_action=\"%d\" second_in_traffic=\"%s\" second_out_traffic=\"%s\" log=\"%d\" " \
				" disabled=\"%d\" bridge=\"%d\" comment=\"%s\" />	   \n" ,
			
				firewalls->pinfo[index].name,
				firewalls->pinfo[index].proto,
				firewalls->pinfo[index].auth,
				firewalls->pinfo[index].action,
				firewalls->pinfo[index].schedule,
				firewalls->pinfo[index].session_limit,
				firewalls->pinfo[index].vlan,
				firewalls->pinfo[index].src,
				firewalls->pinfo[index].dst,
				firewalls->pinfo[index].mac,
				firewalls->pinfo[index].in_traffic,
				firewalls->pinfo[index].out_traffic,
				firewalls->pinfo[index].quota,
				firewalls->pinfo[index].quota_action,
				firewalls->pinfo[index].second_in_traffic,
				firewalls->pinfo[index].second_out_traffic,
				firewalls->pinfo[index].log,
				firewalls->pinfo[index].disabled,
				firewalls->pinfo[index].bridge,
				firewalls->pinfo[index].comment);
			
			printf("\n");
		}
		
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_FIREWALLS,(void*)firewalls,size);
        printf("firewalls ptr = %x \n\n" , firewalls);
		free_xml_node((void*)&firewalls);
#endif 

#if 1
		get_xml_node(CONFIG_FILE, MGTCGI_TYPE_DNS_POLICYS,(void**)&dnspolicys,&size);	
		
		printf("MGTCGI_TYPE_TRAFFIC len = %d \n" , size); 
		printf("num  = %d application_policy=%d\n" , dnspolicys->num , dnspolicys->application_policy);
			
		for(index = 0; index < dnspolicys->num ; ++index){
			printf(  
		"<dataitem name=\"%d\" domain_name=\"%s\" bridge=\"%d\" src=\"%s\" dst=\"%s\" " \
		" action=\"%d\" targetip=\"%s\" comment=\"%s\" disabled=\"%d\" /> " ,
				dnspolicys->pinfo[index].name,
				dnspolicys->pinfo[index].domain_name,
				dnspolicys->pinfo[index].bridge,
				dnspolicys->pinfo[index].src,
				dnspolicys->pinfo[index].dst,
				dnspolicys->pinfo[index].action,
				dnspolicys->pinfo[index].targetip,
				dnspolicys->pinfo[index].comment,
				dnspolicys->pinfo[index].disabled);

			printf("\n");
		}
	/*
		struct mgtcgi_xml_dns_policys* pnew = NULL;
		int newsize = size + sizeof(struct mgtcgi_xml_dns_policy_info);
		pnew = (struct mgtcgi_xml_dns_policys*)malloc(newsize);
		memset((char*)pnew , 0 , newsize);
		memcpy((char*)pnew ,(char*)dnspolicys , size);
		pnew->num = dnspolicys->num +1;
		
		pnew->pinfo[index].name = 11;
		strcpy(pnew->pinfo[index].domain_name , "any");
		pnew->pinfo[index].bridge = 0;
		strcpy(pnew->pinfo[index].src , "any");
		strcpy(pnew->pinfo[index].dst , "any");
		pnew->pinfo[index].action = 1;
		strcpy(pnew->pinfo[index].targetip , "test");
		pnew->pinfo[index].disabled = 1;
		strcpy(pnew->pinfo[index].comment , "aaa");
		
		printf("\nnew data === \n");
		for(index = 0; index < pnew->num ; ++index){
					printf(  
				"<dataitem name=\"%d\" domain_name=\"%s\" bridge=\"%d\" src=\"%s\" dst=\"%s\" " \
				" action=\"%d\" targetip=\"%s\" comment=\"%s\" disabled=\"%d\" /> " ,
						pnew->pinfo[index].name,
						pnew->pinfo[index].domain_name,
						pnew->pinfo[index].bridge,
						pnew->pinfo[index].src,
						pnew->pinfo[index].dst,
						pnew->pinfo[index].action,
						pnew->pinfo[index].targetip,
						pnew->pinfo[index].comment,
						pnew->pinfo[index].disabled);
		
					printf("\n");
		}

		//printf("pnew= %x ,newsize=%d\n",pnew,newsize);
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_DNS_POLICYS,(void*)pnew,newsize);
		free(pnew);*/
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_DNS_POLICYS,(void*)dnspolicys,size);
        printf("dnspolicys ptr = %x \n\n" , dnspolicys);
		free_xml_node((void*)&dnspolicys);
#endif 

#if 1
		get_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void**)&httppolicy,&size);	
		
		printf("MGTCGI_TYPE_HTTP_DIR_POLICYS len = %d \n" , size); 
		printf("num  = %d \n" , httppolicy->policy_num );
			
		for(index = 0; index < httppolicy->policy_num ; ++index){
			printf(
		"<policy name =%d bridge=\"%d\" extgroup=\"%s\" servergroup=\"%s\" comment=\"%s\" /> " ,
				httppolicy->p_policyinfo[index].name,
				httppolicy->p_policyinfo[index].bridge,
				httppolicy->p_policyinfo[index].extgroup,
				httppolicy->p_policyinfo[index].servergroup,
				httppolicy->p_policyinfo[index].comment);

			printf("\n");
		}
/*
		struct mgtcgi_xml_http_dir_policys* pnew = NULL;
		int newsize = size + sizeof(struct mgtcgi_xml_http_dir_policy_info);
		pnew = (struct mgtcgi_xml_http_dir_policys*)malloc(newsize);
		memset((char*)pnew , 0 , newsize);
		memcpy((char*)pnew ,(char*)httppolicy , size);
		pnew->format_num= httppolicy->format_num;
		pnew->policy_num= httppolicy->policy_num +1;
		 
		pnew->p_urlinfo=(struct mgtcgi_xml_http_dir_policy_urlformat_info *)
						( (char*)pnew+ sizeof(struct mgtcgi_xml_http_dir_policys));
		pnew->p_policyinfo= (struct mgtcgi_xml_http_dir_policy_info *)
						( (char*)pnew+ sizeof(struct mgtcgi_xml_http_dir_policys) +			
						pnew->format_num * 
							sizeof(struct mgtcgi_xml_http_dir_policy_urlformat_info));

		
		pnew->p_policyinfo[index].name = 5;
		pnew->p_policyinfo[index].bridge = 0;
		strcpy(pnew->p_policyinfo[index].extgroup , "video");
		strcpy(pnew->p_policyinfo[index].servergroup,"svrgroup1");
		strcpy(pnew->p_policyinfo[index].comment,"ffffffff");
		
		printf("\nnew data === \n");
		for(index = 0; index < pnew->policy_num ; ++index){
			printf(
		"<policy name =%d bridge=\"%d\" extgroup=\"%s\" servergroup=\"%s\" comment=\"%s\" /> " ,
				pnew->p_policyinfo[index].name,
				pnew->p_policyinfo[index].bridge,
				pnew->p_policyinfo[index].extgroup,
				pnew->p_policyinfo[index].servergroup,
				pnew->p_policyinfo[index].comment);

				printf("\n");
		}

		//printf("pnew= %x ,newsize=%d\n",pnew,newsize);
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)pnew,newsize);
		free(pnew);*/
		
		save_xml_node(CONFIG_FILE, MGTCGI_TYPE_HTTP_DIR_POLICYS,(void*)httppolicy,size);
        printf("httppolicy ptr = %x \n\n" , httppolicy);

		free_xml_node((void*)&httppolicy);
#endif 
    }


	return  0;
}

