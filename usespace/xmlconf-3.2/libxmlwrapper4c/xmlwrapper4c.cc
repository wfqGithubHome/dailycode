#include "xmlwrapper4c.h"

//#include "/usr/include/semaphore.h"
//#include <sys/sem.h>
//#include <semaphore.h>

#define MAX_FILENAME_LEN 256
#define XML_ERROR -1
#if 0
#define DEBUGPR(...) fprintf(stdout,__VA_ARGS__)
#else
#define DEBUGPR(...) 
#endif

static int BuildXmlGroups(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmlSystem(const struct mgtcgi_xml_system *const system,char * buf, const int buf_len);
static int BuildXmlManagement(const struct mgtcgi_xml_management *const mgt,char * buf, const int buf_len);
static int BuildXmlAuth(const struct mgtcgi_xml_auth *const auth,char * buf, const int buf_len);
static int BuildXmlInterface(const struct mgtcgi_xml_interfaces *const interfaces,char * buf, const int buf_len);
static int BuildXmlVlans(const struct mgtcgi_xml_vlans *const vlans,char * buf, const int buf_len);

/*2012-09-29 add   buildxmlportscans*/
static int BuildXmlportscans(const struct mgtcgi_xml_portscan *const portscans,char * buf, const int buf_len);
static int BuildXmlServConfs(const struct mgtcgi_xml_servconf *const servconfs,char * buf, const int buf_len);
/* add by chenhao 2013-7-16 17:16:03 */
static int BuildXmlNetworkdetection(const struct mgtcgi_xml_networkdetection *const networkdecection,char * buf, const int buf_len);

/* add by chenhao 2014-11-23 15:24:32 */
static int BuildXmlTimingAdsl(const struct mgtcgi_xml_adsl_timings *const timing,char * buf, const int buf_len);
static int BuildXmlWifi(const struct mgtcgi_xml_wifis * const wifi, char *buf, const int buf_len);

/* add by chenhao,2015-7-8 16:38:21 */
static int BuildXmlFirstGameChannel(const struct mgtcgi_xml_first_game_channels *const firstgc,char *buf,const int buf_len);

static int BuildXmlRouteServer(const struct mgtcgi_xml_routeserver* const info,char * buf, const int buf_len);
static int BuildXmlnetworks(const struct mgtcgi_xml_networks *const networks,char * buf, const int buf_len);
static int BuildXmlnetworkGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmDomainData(const struct mgtcgi_xml_domains*const domains,char * buf, const int buf_len);
static int BuildXmlDomainDataGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmExtensions(const struct mgtcgi_xml_extensions* const extensions,char * buf, const int buf_len);
static int BuildXmlExtensionGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmHttpserver(const struct mgtcgi_xml_httpservers * const httpserver,char * buf, const int buf_len);
static int BuildXmlHttpserverGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmProtocols(const struct mgtcgi_xml_protocols* const protocols,char * buf, const int buf_len);
static int BuildXmlProtocolGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmSchedules(const struct mgtcgi_xml_schedules* const schedules,char * buf, const int buf_len);
static int BuildXmlScheduleGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);
static int BuildXmLogger(const struct mgtcgi_xml_loggers* const logger,char * buf, const int buf_len);
static int BuildXmTraffic(const struct mgtcgi_xml_traffics* const traffic,char * buf, const int buf_len);
static int BuildXmFrewalls(const struct mgtcgi_xml_firewalls* const firewalls,char * buf, const int buf_len);
static int BuildXmDnsPolicy(const struct mgtcgi_xml_dns_policys* const dns_policy,char * buf, const int buf_len);
static int BuildXmHttpDirPolicy(const struct mgtcgi_xml_http_dir_policys* const http_dir_policy,char * buf, const int buf_len);
static int BuildXmGlobalIpSessionLimits(const struct mgtcgi_xml_ip_session_limits* const global_ip_session_limits,char * buf, const int buf_len);
static int BuildXmPortMirrors(const struct mgtcgi_xml_port_mirrors* const port_mirrors,char * buf, const int buf_len);
static int BuildXmlSessionLimit(const struct mgtcgi_xml_session_limits* const session_limit,char * buf, const int buf_len);
static int BuildXmlFirewallPolicy(const struct mgtcgi_xml_firewalls_policy* const firewallpolicy, char *buf, const int buf_len);

//sem_t *sem=NULL;

/**
* Description:	将一个字符串转换为int数字
* @param  str : 需要转换为数字的字符串
* @return		 返回转换后的数字
*/
static inline int64_t strtoint(const std::string & str)
{
	int64_t itmp = 0;
	std::stringstream stream_string;
	stream_string << str;
	stream_string >> itmp;

	return itmp;
}

/**
* Description:  将一个int数字转换字符串
* @param  num : 需要转换为数字的字符串
* @return        返回转换后的字符串
*/
inline std::string inttostr(int64_t num)
{
	std::string tmpstr;
	std::stringstream stream_string;
	stream_string << num;
	stream_string >> tmpstr;

	return tmpstr;
}

/*
	* remark: 从文件filename中获取由nodetype指定的XML结点内容，
			通过buf指针返回内获取的内存指针
			注意:此函数会生成申请一块内存，内存使用
			完毕后请使用free_xmlnode函数释放内存，
			否则将导致内存溢出
	*@configfile : 要读取的xml文件
	*@nodetype : 要读取的xml文件节点类型
	*@lanaguage : 在读取layer7file内容时，需要设置语言类型
	*@buf : 根据读取内容创建的内存块，以nodetype指定的结点类型对应的struct形式保存
	*@buflen : buf内存大小 
	*return: 成功返回buf 的内存的长度，失败返回XML_ERROR

	* 注意: 对于类型MGTCGI_TYPE_LAYER7_SHOWS和MGTCGI_TYPE_PROTOCOLS_GROUP都必须设置layer7file
			需要从文件layer7file中读取协议数据，
			根据lanaguage确定需要读取的协议段，
			再从文件configfile中获取由logger/session_layer7_log数据，
			根据session_layer7_log中的数据设置layer7的启用配置
			通过buf指针返回获取的layer7数据的mgtcgi_xml_layer7_protocols_show结构
			内存指针
*/

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>



int get_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size)
{
	return get_xml_nodeforLayer7(configfile , "",nodetype ,"",buf , out_size);
}

int get_xml_nodeforLayer7(const char * configfile,const char * layer7file,
				const int nodetype,
				const char * lanaguage,void ** buf, int * out_size)
{	
	//Parser *xml_parser = NULL;
	//Log * pLog = NULL;
	//memset(config_filename , '\0' , MAX_FILENAME_LEN * sizeof(char));
	//strncpy(config_filename , filename , MAX_FILENAME_LEN-1);
	char path[]="/var/lock/config.lock";
	int fd,ret;
	
  	maxnet::Log pLog("xmlconf_lib", false,false,false);
	maxnet::Parser xml_parser(&pLog,configfile,
							"","",layer7file,
							"", false,
							0,0,0,0,
							0,0,0,0,
							0,0,0);
	//printf("%s:bigin nodetype=%d\n",__FUNCTION__,nodetype);
	fd=open(path,O_WRONLY|O_CREAT);
	if(fd < 0){
		*out_size = 0;
		*buf = NULL;
		return 0;
	}
	
	if(flock(fd,LOCK_EX)!=0){
		*out_size = 0;
		*buf = NULL;
		close(fd);
		return 0;
	}
	
    ret = xml_parser.loadConfig();
	flock(fd,LOCK_UN);
	close(fd);
	if(ret)
	{
		*out_size = 0;
		*buf = NULL;
		return 0;
	}

#if 0
	fd=open(path,O_WRONLY|O_CREAT);
    if(flock(fd,LOCK_EX)==0){
		xml_parser.loadConfig();
		flock(fd,LOCK_UN);
		close(fd);
    }
	else{
		*out_size = 0;
		*buf = NULL;
		close(fd);
		return 0;
	}
#endif
	switch(nodetype){
		case MGTCGI_TYPE_SYSTEM:
			*out_size = xml_parser.GetStructSystem(
						(struct mgtcgi_xml_system**)buf);
			break;
		case MGTCGI_TYPE_MANAGEMENT:
			*out_size = xml_parser.GetStructManagerment(
						(struct mgtcgi_xml_management**)buf);
			break;
		case MGTCGI_TYPE_AUTH:
			*out_size = xml_parser.GetStructAuth(
						(struct mgtcgi_xml_auth**)buf);
			break;
		case MGTCGI_TYPE_INTERFACES:
			*out_size = xml_parser.GetStructInterface(
						(struct mgtcgi_xml_interfaces**)buf);
			break;
		case MGTCGI_TYPE_NETWORKS:
			*out_size = xml_parser.GetStructNetworks(
						(struct mgtcgi_xml_networks**)buf);
			break;
		case MGTCGI_TYPE_SCHEDULES:
			*out_size = xml_parser.GetStructScheduler(
						(struct mgtcgi_xml_schedules**)buf);
			break;
		case MGTCGI_TYPE_VLANS:
			*out_size = xml_parser.GetStructVlans(
						(struct mgtcgi_xml_vlans**)buf);
			break;
		case MGTCGI_TYPE_SESSION_LIMITS:
			*out_size = xml_parser.GetStructSessionlimit(
						(struct mgtcgi_xml_session_limits**)buf);
			break;
		case MGTCGI_TYPE_DOMAINS:
			*out_size = xml_parser.GetStructDomainData(
						(struct mgtcgi_xml_domains**)buf);			
			break;
		case MGTCGI_TYPE_EXTENSIONS:
			*out_size = xml_parser.GetStructExtensions(
						(struct mgtcgi_xml_extensions**)buf);
			break;
		case MGTCGI_TYPE_HTTPSERVERS:
			*out_size = xml_parser.GetStructHttpserver(
						(struct mgtcgi_xml_httpservers**)buf);
			break;		
		case MGTCGI_TYPE_PROTOCOLS:
			*out_size = xml_parser.GetStructProtocols(
						(struct mgtcgi_xml_protocols**)buf);
			break;
		case MGTCGI_TYPE_LOGGER:
			*out_size = xml_parser.GetStructLogger((struct mgtcgi_xml_loggers**)buf);
			break;
		case MGTCGI_TYPE_IP_SESSION_LIMITS:
			*out_size = xml_parser.GetStructIPSessionLimit(
						(struct mgtcgi_xml_ip_session_limits**)buf);
			break;
		case MGTCGI_TYPE_FIREWALLS:
			*out_size = xml_parser.GetStructFirewalls(
						(struct mgtcgi_xml_firewalls**)buf);
			break;
		case MGTCGI_TYPE_TRAFFIC:
			*out_size = xml_parser.GetStructTraffic(
						(struct mgtcgi_xml_traffics**)buf);
			break;
		case MGTCGI_TYPE_PORT_MIRRORS:
			*out_size = xml_parser.GetStructPortmirrors(
						(struct mgtcgi_xml_port_mirrors**)buf);
			break;
		case MGTCGI_TYPE_DNS_POLICYS:
			*out_size = xml_parser.GetStructDnsPolicy(
						(struct mgtcgi_xml_dns_policys**)buf);
			break;
		case MGTCGI_TYPE_HTTP_DIR_POLICYS:
			*out_size = xml_parser.GetStructHttpDirPolicy(
						(struct mgtcgi_xml_http_dir_policys**)buf);
			break;			
		case MGTCGI_TYPE_NETWORKS_GROUP:	
			*out_size = xml_parser.GetStructNetworksGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
		case MGTCGI_TYPE_DOMAIN_GROUP:
			*out_size = xml_parser.GetStructDomainDataGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
		case MGTCGI_TYPE_EXTENSION_GROUP:
			*out_size = xml_parser.GetStructExtensionGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
		case MGTCGI_TYPE_HTTPSERVER_GROUP:
			*out_size = xml_parser.GetStructHttpserverGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
		case MGTCGI_TYPE_SCHEDULES_GROUP:
			*out_size = xml_parser.GetStructSchedulerGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
              case MGTCGI_TYPE_PORTSCAN:
			*out_size = xml_parser.GetStructPortscan(
						(struct mgtcgi_xml_portscan**)buf);
			break;
		case MGTCGI_TYPE_NETWORKDETECTION:
			*out_size = xml_parser.GetStructNetworkDetection(
						(struct mgtcgi_xml_networkdetection **)buf);
			break;
		case MGTCGI_TYPE_FIRST_GAME_CHANNEL:
			*out_size = xml_parser.GetStructFirstGameChannel(
						(struct mgtcgi_xml_first_game_channels **)buf);
			break;
		case MGTCGI_TYPE_ROUTESERVICE:
			*out_size = xml_parser.GetStructRouteServerInfo(
						(struct mgtcgi_xml_routeserver **)buf);
			break;
		case MGTCGI_TYPE_FIREWALLPOLICY:
			*out_size = xml_parser.GetStructFirewallPolicy(
						(struct mgtcgi_xml_firewalls_policy **)buf);
			break;
		case MGTCGI_TYPE_LAYER7TYPE_SHOWS:
			*out_size = xml_parser.GetStructLayer7ProtocolType(
						lanaguage,(struct mgtcgi_xml_layer7_protocols_show **)buf);
			break;
		case MGTCGI_TYPE_LAYER7_SHOWS:	
			{					
				std::map<std::string,bool> mapSessonlayer7log;
				std::map<std::string,bool>::iterator iter;
				std::vector<std::string> vecSessionLayer7log;
				std::string tmp;
				size_t index = 0;	
				struct mgtcgi_xml_layer7_protocols_show * tmpPtr = NULL;
				tmp = xml_parser.GetSessionLayer7Log();
				xml_parser.split(tmp,&vecSessionLayer7log," ");

				for(index =0; index < vecSessionLayer7log.size(); ++index){		
					mapSessonlayer7log.insert(std::map<std::string,bool>::value_type(
							vecSessionLayer7log[index], true));		
				}
				*out_size = xml_parser.GetStructLayer7Protocols(lanaguage,(struct mgtcgi_xml_layer7_protocols_show **)buf);
				tmpPtr = (struct mgtcgi_xml_layer7_protocols_show *)*buf;
				for(index =0; index < tmpPtr->num; ++index){		
					iter = mapSessonlayer7log.find(tmpPtr->pinfo[index].name);
					if (iter != mapSessonlayer7log.end()){
						tmpPtr->pinfo[index].selected = 1;
					}
				}

				vecSessionLayer7log.clear();
				mapSessonlayer7log.clear();
			}
			break;
		case MGTCGI_TYPE_PROTOCOLS_GROUP:
			*out_size = xml_parser.GetStructProtocolGroups(lanaguage,
				(struct mgtcgi_xml_group_array**)buf);
				break;
		case MGTCGI_TYPE_PROTOCOLS_GROUP_CN:
			{					
				std::map<std::string,std::string> mapl7lanauage;
				std::map<std::string,std::string>::iterator iter;
				std::string tmp;
				size_t index = 0;	
				size_t Yindex = 0;
				struct mgtcgi_xml_layer7_protocols_show * tmpPtr = NULL;
				struct mgtcgi_xml_group_array * tmpPtrGrp = NULL;
				

				/*****************获取L7文件数据，
				*	将自定义协议组中的英文协议名称转换为中文
				****************************/
				*out_size = xml_parser.GetStructLayer7Protocols(lanaguage,
						(struct mgtcgi_xml_layer7_protocols_show * *)buf);
				tmpPtr = (struct mgtcgi_xml_layer7_protocols_show *)*buf;

				for(index =0; index < tmpPtr->num; ++index){
					mapl7lanauage.insert(std::map<std::string,std::string>::value_type(
							tmpPtr->pinfo[index].name ,tmpPtr->pinfo[index].match ));	
					//std::cout<<tmpPtr->pinfo[index].name << "  " <<
					//		tmpPtr->pinfo[index].name2 <<std::endl;
				}
				free_xml_node(buf);
				*out_size = 0;			


				/*****************获取自定义协议组数据************/
				*out_size = xml_parser.GetStructProtocolGroups(lanaguage,
							(struct mgtcgi_xml_group_array**)buf);
				tmpPtrGrp = (struct mgtcgi_xml_group_array *)*buf;
				
				for(index =0; index < tmpPtrGrp->num; ++index){	
					for(Yindex =0; Yindex < tmpPtrGrp->pinfo[index].num; ++Yindex){
						iter = mapl7lanauage.find(tmpPtrGrp->pinfo[index].pinfo[Yindex].name);
						if (iter != mapl7lanauage.end()){
							strncpy(tmpPtrGrp->pinfo[index].pinfo[Yindex].name , 
								iter->second.c_str() , STRING_LENGTH-1);
						}
					}
				}
				mapl7lanauage.clear();
			}
		   break;
		case MGTCGI_WIFI_ID:
			*out_size = xml_parser.GetStructWifiConf((struct mgtcgi_xml_wifis **)buf);
			break;
              case MGTCGI_TYPE_SERVER_CONF:
                     *out_size = xml_parser.GetStructServConf((struct mgtcgi_xml_servconf**)buf);
			break;
		case MGTCGI_SYS_TIMING_ADSL_REDIAL:
			*out_size = xml_parser.GetStructTimingAdsl((struct mgtcgi_xml_adsl_timings**)buf);
			break;
		default:
			*out_size = 0;
			*buf = NULL;
	}

	return *out_size;
}

int get_xml_nodeforservinfo(const char * configfile, const char * servinfofile,
    void ** buf, int * out_size)
{	
	maxnet::Log pLog("xmlconf_lib", false,false,false);
	struct mgtcgi_xml_servconf * servconf = NULL;
    struct mgtcgi_xml_servinfo * servinfo = NULL;
    int servconf_size = 0;
    int i, j, ret;
    
	maxnet::Parser xml_parser(&pLog,configfile,
							"","","",
							servinfofile, false,
							0,0,0,0,
							0,0,0,0,
							0,0,0);
	ret = xml_parser.loadConfig();
	if(ret){
		return 0;
	}
    
    servconf_size = xml_parser.GetStructServConf(&servconf);
    *out_size = xml_parser.GetStructServInfo((struct mgtcgi_xml_servinfo **)buf);
	if(*buf==NULL)
		return 0;
    
    servinfo = *(struct mgtcgi_xml_servinfo**)buf;    
    for (i = 0; i < servinfo->num; i++)
    {        
        for (j = 0; j < servconf->num; j++)
        {
            if (strcmp(servinfo->pinfo[i].en_name, servconf->pinfo[j].en_name) == 0 &&
                strcmp(servinfo->pinfo[i].en_zonename, servconf->pinfo[j].en_zonename) == 0)
            {
                servinfo->pinfo[i].selected = 1;
            }
        }
    }

	free_xml_node((void**)&servconf);
	return *out_size;
}


char * find_layer7_name(const char * configfile,const char * layer7file,
			const char * lanaguage,const char * const layer7match){
			
	std::string strname = "";
	bool isFinded = false;	
	size_t index = 0;	
	int ret;
	struct mgtcgi_xml_layer7_protocols_show * tmpPtr = NULL;
	//struct mgtcgi_xml_protocols* tmpPtrProto = NULL;

	if(configfile == NULL ||
		layer7file == NULL ||
		lanaguage == NULL ||
		layer7match == NULL ){
		
		return NULL;
	}		

	maxnet::Log pLog("xmlconf_lib", false,false,false);

	maxnet::Parser xml_parser(&pLog,configfile,
							"","",layer7file,
							"", false,
							0,0,0,0,
							0,0,0,0,0,
							0,0);
	ret = xml_parser.loadConfig();
	if(ret){
		return NULL;
	}

	/*****************获取L7文件数据，
	*	将自定义协议组中的英文协议名称转换为中文
	****************************/
	xml_parser.GetStructLayer7Protocols(lanaguage,&tmpPtr);
	
	//printf("layer7match == %s\n" , layer7match);

	for(index =0; index < tmpPtr->num; ++index){
		/*if(strcmp("GFSJ" , tmpPtr->pinfo[index].name)==0)
		{
			printf("name:%s , match=%s\n" ,
				tmpPtr->pinfo[index].name, tmpPtr->pinfo[index].match);
			for(int i = 0; i< strlen(tmpPtr->pinfo[index].match); i++){
				printf("match=%x\n" ,
				 (tmpPtr->pinfo[index].match)[i]);
			}
	printf("\n\n");
		}*/
		
		if(strncmp(layer7match , tmpPtr->pinfo[index].match , STRING_LENGTH)== 0){
			strname = tmpPtr->pinfo[index].name;
			isFinded = true;
			break;
		}
	}
	free_xml_node((void**)&tmpPtr);
	
	if(isFinded){
		return (char*)strname.c_str();
	}

	return NULL;
}
			


/*
* remark: 将一个内存节点从struct的形式保存到filename指定的xml文件中
*@filename : 要修改的xml文件
*@nodetype : 要修改的xml文件节点类型
*@buf : 要保存的内存块，以nodetype指定的结点类型对应的struct形式保存
*@buflen : buf内存大小 
*return: 成功返回buf生成的字符串的长度，失败返回XML_ERROR
*/
int save_xml_node(const char * filename,const int nodetype,const void * buf, const int	buflen)
{
	#define STRING_BUFF_LEN 1024000
	char *strbuf  = (char *)malloc(sizeof(char) * STRING_BUFF_LEN);
	
	char xmlnodename[URL_LENGTH] = {0};
	int buildlen = 0;
	char path[]="/var/lock/config.lock";
	int fd;
	
	if(!strbuf){
		return XML_ERROR;
    	}
	memset(strbuf ,0 , sizeof(char) * STRING_BUFF_LEN);

	maxnet::Log pLog("/var/log/xmlconf_lib.log", false,false,false);	
	switch(nodetype){
		case MGTCGI_TYPE_SYSTEM:
			buildlen = BuildXmlSystem((struct mgtcgi_xml_system*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"system" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_MANAGEMENT:
			buildlen = BuildXmlManagement((struct mgtcgi_xml_management*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"management" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_AUTH:
			buildlen = BuildXmlAuth((struct mgtcgi_xml_auth*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"auth" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACES:
			buildlen = BuildXmlInterface((struct mgtcgi_xml_interfaces*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_NETWORKS:	
			buildlen = BuildXmlnetworks((struct mgtcgi_xml_networks*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"networks" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_SCHEDULES:
			buildlen = BuildXmSchedules((struct mgtcgi_xml_schedules*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"schedules" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_VLANS:
			buildlen = BuildXmlVlans((struct mgtcgi_xml_vlans*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"vlans" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_SESSION_LIMITS:
			buildlen = BuildXmlSessionLimit((struct mgtcgi_xml_session_limits*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"session_limit" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_DOMAINS:
			buildlen = BuildXmDomainData((struct mgtcgi_xml_domains*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"domain_data" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_EXTENSIONS:
			buildlen = BuildXmExtensions((struct mgtcgi_xml_extensions*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"extension" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_HTTPSERVERS:
			buildlen = BuildXmHttpserver((struct mgtcgi_xml_httpservers*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"httpserver" , URL_LENGTH-1);
			break;		
		case MGTCGI_TYPE_PROTOCOLS:
			buildlen = BuildXmProtocols((struct mgtcgi_xml_protocols*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"protocols" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_LOGGER:
			buildlen = BuildXmLogger((struct mgtcgi_xml_loggers*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"logger" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_IP_SESSION_LIMITS:
			buildlen = BuildXmGlobalIpSessionLimits((struct mgtcgi_xml_ip_session_limits*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"global_ip_session_limits" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_FIREWALLS:
			buildlen = BuildXmFrewalls((struct mgtcgi_xml_firewalls*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"firewalls" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_TRAFFIC:		
			buildlen = BuildXmTraffic((struct mgtcgi_xml_traffics*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"traffic" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_PORT_MIRRORS:
			buildlen = BuildXmPortMirrors((struct mgtcgi_xml_port_mirrors*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"port_mirrors" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_DNS_POLICYS:
			buildlen = BuildXmDnsPolicy((struct mgtcgi_xml_dns_policys*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"dns_policy" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_HTTP_DIR_POLICYS:
			buildlen = BuildXmHttpDirPolicy((struct mgtcgi_xml_http_dir_policys*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"http_dir_policy" , URL_LENGTH-1);
			break;			
		case MGTCGI_TYPE_NETWORKS_GROUP:
			buildlen = BuildXmlnetworkGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"networks_groups" , URL_LENGTH-1);			
			break;
		case MGTCGI_TYPE_DOMAIN_GROUP:
			buildlen = BuildXmlDomainDataGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"domain_groups" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_EXTENSION_GROUP:
			buildlen = BuildXmlExtensionGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"extension_group" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_HTTPSERVER_GROUP:
			buildlen = BuildXmlHttpserverGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"httpserver_group" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_PROTOCOLS_GROUP:
			buildlen = BuildXmlProtocolGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"protocols_groups" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_SCHEDULES_GROUP:
			buildlen = BuildXmlScheduleGruop((struct mgtcgi_xml_group_array*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"schedules_groups" , URL_LENGTH-1);
			break;
		
        /*2012-09-29 add */    
        case MGTCGI_TYPE_PORTSCAN:
    		buildlen = BuildXmlportscans((struct mgtcgi_xml_portscan*)buf ,strbuf, STRING_BUFF_LEN);
    		strncpy(xmlnodename ,"portscans", URL_LENGTH-1);
    		break;
        case MGTCGI_TYPE_SERVER_CONF:
       		buildlen = BuildXmlServConfs((struct mgtcgi_xml_servconf*)buf ,strbuf, STRING_BUFF_LEN);
        	strncpy(xmlnodename, "servinfos", URL_LENGTH-1);
    		break;	
	/* add by chenhao 2013-7-16 16:41:18 */
		case MGTCGI_TYPE_NETWORKDETECTION:
			buildlen = BuildXmlNetworkdetection((struct mgtcgi_xml_networkdetection *)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "network_check", URL_LENGTH-1);
            	break;
		/*add by chenhao,2015-7-8 15:52:39*/
		case MGTCGI_TYPE_FIRST_GAME_CHANNEL:
			buildlen = BuildXmlFirstGameChannel((struct mgtcgi_xml_first_game_channels*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "first_game_channel", URL_LENGTH-1);
			break;
	/* Add by chenhao ,2014-11-21 18:01:27 */
		case MGTCGI_SYS_TIMING_ADSL_REDIAL:
			buildlen = BuildXmlTimingAdsl((struct mgtcgi_xml_adsl_timings*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "timings", URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ROUTESERVICE:
			buildlen = BuildXmlRouteServer((struct mgtcgi_xml_routeserver*)buf ,strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "routeserverinfos", URL_LENGTH-1);
			break;
		case MGTCGI_WIFI_ID:
			buildlen = BuildXmlWifi((struct mgtcgi_xml_wifis*)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "wifi", URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_FIREWALLPOLICY:
			buildlen = BuildXmlFirewallPolicy((struct mgtcgi_xml_firewalls_policy*)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename, "firewalls_policy", URL_LENGTH-1);
			break;
        	default:
	    		buildlen = 0;
	    	break;
	}

    if(buildlen > 0 ){
		fd=open(path,O_WRONLY);
    	if(flock(fd,LOCK_EX)!=0){
			free(strbuf);
			close(fd);
			return XML_ERROR;
    	}
		maxnet::RWXml rwxml(&pLog , filename);
	    if(rwxml.replaceNode(xmlnodename , strbuf) == 0){
			flock(fd,LOCK_UN);
			free(strbuf);
			strbuf = NULL;
			close(fd);
			return buildlen;	
		}
		flock(fd,LOCK_UN);
		close(fd);
    }
	if(strbuf){
	    free(strbuf);
        strbuf = NULL;
    }
	return XML_ERROR;
}


/* 
remark: 删除之前分配的内存
return: 删除成功返回0
*/
int free_xml_node(void ** buf)
{
//printf("buf = %x \n" ,*buf);
	if(*buf != NULL){
		free(*buf);
		*buf = NULL;
	}	
	
//	printf("free xml node\n" , ); 
	return 0;
}

/*
*remark : 根据struct结构数据构建group结点的字符串
*/
static int BuildXmlGroups(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	int index_1 = 0;
	int index_2 = 0;
	char *ptr = buf;

	for(index_1 = 0; index_1 < group->num; ++index_1){
		strcpy(ptr , "<group name=\"");
		ptr += 13;
		strcpy(ptr, group->pinfo[index_1].group_name);
		ptr += strlen(group->pinfo[index_1].group_name);
		strcpy(ptr , "\">\n");
		ptr  += 3; 
		for(index_2 = 0; index_2 < group->pinfo[index_1].num; ++index_2){
			strcpy(ptr , "<include name=\"");
            ptr+=15;
			strcpy(ptr , group->pinfo[index_1].pinfo[index_2].name);
			ptr+=strlen(group->pinfo[index_1].pinfo[index_2].name);
			strcpy(ptr , "\" />\n" );
			ptr+=5;
			if((ptr-buf + 10) > buf_len ) break;
		}
		DEBUGPR(" include =:  %s \n" , buf);
		strcpy(ptr , "</group>\n");
		ptr += 9;
	}

	//snprintf(buf ,buf_len , "%s" , strgroup.c_str());
	
	DEBUGPR("%s\n" , buf);
	return strlen(buf);

}


/*
*remark : 根据struct结构数据构建System结点的字符串
*/
static int BuildXmlSystem(const struct mgtcgi_xml_system *const system,char * buf, const int buf_len)
{
const char* format_str = 
	"  <system>\n"  \
	"	 <hostname>%s</hostname>\n"  \
	"	 <domainname>%s</domainname>\n"	\
	"	 <contact>%s</contact>\n"  \
	"	 <location>%s</location>\n"  \
	"	 <password>%s</password>\n"  \
	"	 <password2>%s</password2>\n"  \
	"	 <enable_stp>%d</enable_stp>\n"	\
	"	 <auto_update>%d</auto_update>\n"  \
	"	 <update_url>%s</update_url>\n"  \
	"	 <ntp_enable>%d</ntp_enable>\n"	\
	"	 <ntp_server>%s</ntp_server>\n"	\
	"	 <timezone>%s</timezone>\n"  \
	"	 <bridge_mtu>%d</bridge_mtu>\n"  \
	"	 <dbridge_perf>%d</dbridge_perf>\n"	\
	"	 <snmp_enable>%d</snmp_enable>\n"  \
	"	 <snmp_ro_community>%s</snmp_ro_community>\n"  \
	"	 <snmp_rw_community>%s</snmp_rw_community>\n"	\
	"  </system>\n" ;
		
	snprintf(buf, buf_len, format_str,
		system->hostname,
		system->domainname,
		system->contact,
		system->location,
		system->password,
		system->password2,
		system->enable_stp,
		system->auto_update,
		system->update_url,
		system->ntp_enable,
		system->ntp_server,
		system->timezone,
		system->bridge_mtu,
		system->dbridge_perf,
		system->snmp_enable,
		system->snmp_ro_community,
		system->snmp_rw_community);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建managerment结点的字符串
*/
static int BuildXmlManagement(const struct mgtcgi_xml_management *const mgt,char * buf, const int buf_len)
{
const char* format_str = 
	"  <management>\n"  \
	"    <ip>%s</ip>\n"  \
	"    <netmask>%s</netmask>\n"  \
	"    <broadcast>%s</broadcast>\n"  \
	"    <gateway>%s</gateway>\n"  \
	"    <server>%s</server>\n"  \
	"    <dns_primary>%s</dns_primary>\n"  \
	"    <dns_secondary>%s</dns_secondary>\n"  \
	"  </management>\n" ;
		
	snprintf(buf ,buf_len, format_str,
		mgt->ip,
		mgt->netmask,
		mgt->broadcast,
		mgt->gateway,
		mgt->server,
		mgt->dns_primary,
		mgt->dns_secondary);
	
	return strlen(buf);
}


/*
*remark : 根据struct结构数据构建Auth结点的字符串
*/
static int BuildXmlAuth(const struct mgtcgi_xml_auth *const auth,char * buf, const int buf_len)
{
const char* format_str = 
	"  <auth>\n"  \
	"	 <enable>%d</enable>\n"	\
	"	 <timeout_mode>%d</timeout_mode>\n"	\
	"	 <timeout_seconds>%d</timeout_seconds>\n"  \
	"	 <enable_multi_user>%d</enable_multi_user>\n"  \
	"	 <deny_guest_access>%s</deny_guest_access>\n"  \
	"	 <redirect_http_request>%d</redirect_http_request>\n"  \
	"	 <redirect_http_url>%s</redirect_http_url>\n"  \
	"  </auth>\n"  ;
		
	snprintf(buf ,buf_len, format_str,
		auth->enable,
		auth->timeout_mode,
		auth->timeout_seconds,
		auth->enable_multi_user,
		auth->redirect_adv_url,
		auth->redirect_http_request,
		auth->redirect_http_url);
	
	return strlen(buf);
}


/*
*remark : 根据struct结构数据构建interface结点的字符串
*/
static int BuildXmlInterface(const struct mgtcgi_xml_interfaces *const interfaces,char * buf, const int buf_len)
{
	std::string strtemp="";
	const char* mainnode_format_str =		
			"  <interface>\n"  \
			"%s" \
			"  </interface>\n" ;
	const char* format_str = 
			"	 <%s>\n"	\
			"	   <mode>%s</mode>\n"  \
			"	   <speed>%d</speed>\n"  \
			"	   <duplex>%s</duplex>\n"  \
			"	   <linkmode>%d</linkmode>\n"  \
			"	   <bridgeid>%d</bridgeid>\n"  \
			"	 </%s>\n" ;
	int bufindex = 0;
	int index = 0;

	for(index = 0; index < interfaces->num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str,
			interfaces->pinfo[index].name,
			interfaces->pinfo[index].mode,
			interfaces->pinfo[index].speed,
			interfaces->pinfo[index].duplex,
			interfaces->pinfo[index].linkstate,
			interfaces->pinfo[index].bridgeid,
			interfaces->pinfo[index].name);
		
		if(bufindex >= buf_len) break;
	}
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	strtemp.c_str());
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建vlans结点的字符串
*/
static int BuildXmlVlans(const struct mgtcgi_xml_vlans *const vlans,char * buf, const int buf_len)
{	
	std::string strtemp="";
	const char* mainnode_format_str =		
		"  <vlans>\n"  \
		"  %s\n"  \
		"  </vlans>\n" ;
	const char* format_str = 
		"    <vlan name=\"%s\" vlans=\"%d\" comment=\"%s\" />\n";	
	int bufindex = 0;
	int index = 0;

	for(index = 0; index < vlans->num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str,
			vlans->pinfo[index].name,
			vlans->pinfo[index].vlans,
			vlans->pinfo[index].comment);
		
		if(bufindex >= buf_len) break;
	}
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	strtemp.c_str());
	
	return strlen(buf);
}



/*2012-09-29 add buildxmlportscans*/    
static int BuildXmlportscans(const struct mgtcgi_xml_portscan *const portscans,char * buf, const int buf_len)
{	
	std::string strtemp="";
	const char* mainnode_format_str =		
		"  <portscans>\n"  \
		"  %s\n"  \
		"  </portscans>\n" ;
	const char* format_str = 
		"    <portscan conntype=\"%s\" newconn=\"%s\" doubtconn=\"%s\" loginterval=\"%s\" />\n";	
	int bufindex = 0;
	int index = 0;

	for(index = 0; index < portscans->num; ++index){
		bufindex += snprintf(buf + bufindex ,buf_len, format_str,
			portscans->pinfo[index].type,
			portscans->pinfo[index].newer,
			portscans->pinfo[index].pending,
            portscans->pinfo[index].interval);
        
		if(bufindex >= buf_len) break;
	}
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	strtemp.c_str());
	
	return strlen(buf);
}

static int BuildXmlServConfs(const struct mgtcgi_xml_servconf *const servconfs,char * buf, const int buf_len)
{
    std::string strtemp="";
	const char* mainnode_format_str =		
		"  <servinfos>\n"  \
		"  %s\n"  \
		"  </servinfos>\n" ;
	const char* format_str = 
		"    <servinfo type=\"%u\" en_name=\"%s\" en_zonename=\"%s\" />\n";	
	int bufindex = 0;
	int index = 0;
    
	for(index = 0; index < servconfs->num; ++index){
		bufindex += snprintf(buf + bufindex ,buf_len, format_str,
			servconfs->pinfo[index].type,
			servconfs->pinfo[index].en_name,
			servconfs->pinfo[index].en_zonename);
        
		if(bufindex >= buf_len) break;
	}
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	strtemp.c_str());

	return strlen(buf);
}

static int BuildXmlNetworkdetection(const struct mgtcgi_xml_networkdetection *const networkdecection,char * buf, const int buf_len)
{
	const char  *formatStr = 
		" <network_check>\n"  \
		" 	<zero_ping>%d</zero_ping>\n"  \
		" </network_check>\n";
	snprintf(buf,buf_len,formatStr,
			networkdecection->zeropingenable);

	return strlen(buf);
}

static int BuildXmlFirstGameChannel(const struct mgtcgi_xml_first_game_channels *const firstgc,char *buf,const int buf_len)
{
	std::string strtemp="";
	const char *mainformat =
		"  <first_game_channel>\n" \
		"  %s\n" \
		"  </first_game_channel>\n";

	const char *format_str =
		"    <game id=\"%d\" name=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	for(index = 0; index < firstgc->num; ++index){
		bufindex += snprintf(buf + bufindex ,buf_len, format_str,
			firstgc->pinfo[index].id,
			firstgc->pinfo[index].name);
        
		if(bufindex >= buf_len) break;
	}

	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainformat, strtemp.c_str());
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建networks结点的字符串
*/
static int BuildXmlnetworks(const struct mgtcgi_xml_networks *const networks,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <networks>\n"  \
		"  %s"  \
		"  </networks>\n" ;
	const char* format_str = 
		"    <addr name=\"%s\" value=\"%s\" range=\"%s\" comment=\"%s\" />\n" ;	*/
		
	int index = 0;
	const char *const head_str = "<networks>\n";
	const char *const foot_str = "</networks>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < networks->num; ++index){
		strcpy(ptr , "<addr name=\"");
		ptr += strlen("<addr name=\"");	
		strcpy(ptr , networks->pinfo[index].name);
		ptr += strlen(networks->pinfo[index].name);	
		strcpy(ptr , "\" value=\"");
		ptr += strlen("\" value=\"");
		strcpy(ptr , networks->pinfo[index].value);
		ptr += strlen(networks->pinfo[index].value);
		strcpy(ptr , "\" range=\"");
		ptr += strlen("\" range=\"");		
		strcpy(ptr , networks->pinfo[index].range);
		ptr += strlen(networks->pinfo[index].range);	
		strcpy(ptr , "\" comment=\"");
		ptr += strlen("\" comment=\"");		
		strcpy(ptr , networks->pinfo[index].comment);
		ptr += strlen(networks->pinfo[index].comment);	
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");
		
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建networks_groups结点的字符串
*/
static int BuildXmlnetworkGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <networks_groups>\n"  \
		"%s"  \
		"  </networks_groups>\n" ;
	*/
	const char *const head_str = "<networks_groups>\n";
	const char *const foot_str = "</networks_groups>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	
	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(buf);

}

/*
*remark : 根据struct结构数据构建domain_data结点的字符串
*/
static int BuildXmDomainData(const struct mgtcgi_xml_domains*const domains,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =	
		"  <domain_data>\n"  \
		"  %s"  \
		"  </domain_data>\n" ;
	const char* format_str = 
		"    <domain name=\"%s\" value=\"%s\" comment=\"%s\" />\n" ;
	*/
	int index = 0;
	const char *const head_str = "<domain_data>\n";
	const char *const foot_str = "</domain_data>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < domains->num; ++index){		
		strcpy(ptr , "<domain name=\"");
		ptr += strlen("<domain name=\"");	
		strcpy(ptr , domains->pinfo[index].name);
		ptr += strlen(domains->pinfo[index].name);	
		strcpy(ptr , "\" value=\"");
		ptr += strlen("\" value=\"");	
		strcpy(ptr , domains->pinfo[index].value);
		ptr += strlen(domains->pinfo[index].value);	
		strcpy(ptr , "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr , domains->pinfo[index].comment);
		ptr += strlen(domains->pinfo[index].comment);	
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");

		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建domain_groups 结点的字符串
*/
static int BuildXmlDomainDataGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <domain_groups>\n"  \
		"%s"  \
		"  </domain_groups>\n"  ;
	*/
	const char *const head_str = "<domain_groups>\n";
	const char *const foot_str = "</domain_groups>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	
	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(buf);

}

/*
*remark : 根据struct结构数据构建extension结点的字符串
*/
static int BuildXmExtensions(const struct mgtcgi_xml_extensions* const extensions,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =
			"  <extension>\n"  \
				"%s"  \
			"  </extension>\n" ;
	const char* format_str = 
			"	   <ext name=\"%s\" value=\"%s\" comment=\"%s\" />\n" ;
	*/
	int index = 0;
	const char *const head_str = "<extension>\n";
	const char *const foot_str = "</extension>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < extensions->num; ++index){		
		strcpy(ptr , "<ext name=\"");
		ptr += strlen("<ext name=\"");	
		strcpy(ptr , extensions->pinfo[index].name);
		ptr += strlen(extensions->pinfo[index].name);	
		strcpy(ptr , "\" value=\"");
		ptr += strlen("\" value=\"");	
		strcpy(ptr , extensions->pinfo[index].value);
		ptr += strlen(extensions->pinfo[index].value); 
		strcpy(ptr , "\" comment=\"");
		ptr += strlen("\" comment=\""); 
		strcpy(ptr , extensions->pinfo[index].comment);
		ptr += strlen(extensions->pinfo[index].comment);	
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");

		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);

	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建extension_group 结点的字符串
*/
static int BuildXmlExtensionGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <extension_group>\n"  \
		"%s"  \
		"  </extension_group>\n"  ;
	*/
	const char *const head_str = "<extension_group>\n";
	const char *const foot_str = "</extension_group>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	
	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(buf);

}

/*
*remark : 根据struct结构数据构建httpserver结点的字符串
*/
static int BuildXmHttpserver(const struct mgtcgi_xml_httpservers * const httpserver,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =
			"  <httpserver>\n"  \
				"%s"  \
			"  </httpserver>\n" ;
	const char* format_str = 
"    <http name=\"%s\" ip_addr=\"%s\" servertype=\"%s\" prefix=\"%s\" suffix=\"%s\" comment=\"%s\" />\n" ;
	*/
	int index = 0;
	const char *const head_str = "<httpserver>\n";
	const char *const foot_str = "</httpserver>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str); 
	
	for(index = 0; index < httpserver->num; ++index){		
		strcpy(ptr ,  "<http name=\"");
		ptr += strlen("<http name=\"");	
		strcpy(ptr , httpserver->pinfo[index].name);
		ptr += strlen(httpserver->pinfo[index].name);	
		strcpy(ptr ,  "\" ip_addr=\"");
		ptr += strlen("\" ip_addr=\"");	
		strcpy(ptr , httpserver->pinfo[index].ip_addr);
		ptr += strlen(httpserver->pinfo[index].ip_addr);	
		strcpy(ptr ,  "\" servertype=\"");
		ptr += strlen("\" servertype=\"");	
		strcpy(ptr , httpserver->pinfo[index].servertype);
		ptr += strlen(httpserver->pinfo[index].servertype);	
		strcpy(ptr ,  "\" prefix=\"");
		ptr += strlen("\" prefix=\"");	
		strcpy(ptr , httpserver->pinfo[index].prefix);
		ptr += strlen(httpserver->pinfo[index].prefix);	
		strcpy(ptr ,  "\" suffix=\"");
		ptr += strlen("\" suffix=\"");	
		strcpy(ptr , httpserver->pinfo[index].suffix);
		ptr += strlen(httpserver->pinfo[index].suffix);
		strcpy(ptr ,  "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr , httpserver->pinfo[index].comment);
		ptr += strlen(httpserver->pinfo[index].comment);
		strcpy(ptr ,  "\" />\n");
		ptr += strlen("\" />\n");			
	
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);

	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建httpserver_group 结点的字符串
*/
static int BuildXmlHttpserverGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <httpserver_group>\n"  \
		"%s"  \
		"  </httpserver_group>\n"  ;
	*/
	const char *const head_str = "<httpserver_group>\n";
	const char *const foot_str = "</httpserver_group>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str);	

	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(buf);

}

/*
*remark : 根据struct结构数据构建protocols结点的字符串
*/
static int BuildXmProtocols(const struct mgtcgi_xml_protocols* const protocols,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =
			"  <protocols>\n"  \
				"%s"  \
			"  </protocols>\n" ;
	const char* format_str = 
		"	 <protocol name=\"%s\" proto=\"%s\" dport=\"%s\" sport=\"%s\" " \
		" match=\"%s\" option=\"%s\" comment=\"%s\" />\n" ;
	*/
	int index = 0;
	const char *const head_str = "<protocols>\n";
	const char *const foot_str = "</protocols>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < protocols->num; ++index){
		strcpy(ptr , "<protocol name=\"");
		ptr += strlen("<protocol name=\"");	
		strcpy(ptr , protocols->pinfo[index].name);
		ptr += strlen(protocols->pinfo[index].name);	
		strcpy(ptr , "\" proto=\"");
		ptr += strlen("\" proto=\"");	
		strcpy(ptr , protocols->pinfo[index].proto);
		ptr += strlen(protocols->pinfo[index].proto);	
		strcpy(ptr , "\" dport=\"");
		ptr += strlen("\" dport=\"");	
		strcpy(ptr , protocols->pinfo[index].dport);
		ptr += strlen(protocols->pinfo[index].dport);	
		strcpy(ptr , "\" sport=\"");
		ptr += strlen("\" sport=\"");	
		strcpy(ptr , protocols->pinfo[index].sport);
		ptr += strlen(protocols->pinfo[index].sport);	
		strcpy(ptr , "\" match=\"");
		ptr += strlen("\" match=\"");	
		strcpy(ptr , protocols->pinfo[index].match);
		ptr += strlen(protocols->pinfo[index].match);	
		strcpy(ptr , "\" option=\"");
		ptr += strlen("\" option=\"");	
		strcpy(ptr , protocols->pinfo[index].option);
		ptr += strlen(protocols->pinfo[index].option);	
		strcpy(ptr , "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr , protocols->pinfo[index].comment);
		ptr += strlen(protocols->pinfo[index].comment);
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");	
	
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建protocols_groups 结点的字符串
*/
static int BuildXmlProtocolGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <protocols_groups>\n"  \
		"%s"  \
		"  </protocols_groups>\n"  ;
	*/
	const char *const head_str = "<protocols_groups>\n";
	const char *const foot_str = "</protocols_groups>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	
	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建schedules结点的字符串
*/
static int BuildXmSchedules(const struct mgtcgi_xml_schedules* const schedules,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =
			"  <schedules>\n"  \
				"%s"  \
			"  </schedules>\n" ;
	const char* format_str = 
		"	 <schedule name=\"%s\" start_time=\"%s\" stop_time=\"%s\" days=\"%s\" comment=\"%s\" />\n";
		*/
	int index = 0;
	const char *const head_str = "<schedules>\n";
	const char *const foot_str = "</schedules>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < schedules->num; ++index){
		strcpy(ptr , "<schedule name=\"");
		ptr += strlen("<schedule name=\"");	
		strcpy(ptr , schedules->pinfo[index].name);
		ptr += strlen(schedules->pinfo[index].name);	
		strcpy(ptr , "\" start_time=\"");
		ptr += strlen("\" start_time=\"");	
		strcpy(ptr , schedules->pinfo[index].start_time);
		ptr += strlen(schedules->pinfo[index].start_time);	
		strcpy(ptr , "\" stop_time=\"");
		ptr += strlen("\" stop_time=\"");	
		strcpy(ptr , schedules->pinfo[index].stop_time);
		ptr += strlen(schedules->pinfo[index].stop_time);	
		strcpy(ptr , "\" days=\"");
		ptr += strlen("\" days=\"");	
		strcpy(ptr , schedules->pinfo[index].days);
		ptr += strlen(schedules->pinfo[index].days);	
		strcpy(ptr , "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr , schedules->pinfo[index].comment);
		ptr += strlen(schedules->pinfo[index].comment);	
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");	
	
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建schedules_groups 结点的字符串
*/
static int BuildXmlScheduleGruop(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =		
		"  <schedules_groups>\n"  \
		"%s"  \
		"  </schedules_groups>\n"  ;
	*/
	const char *const head_str = "<schedules_groups>\n";
	const char *const foot_str = "</schedules_groups>\n";
	char * ptr = buf ;
	int tmplen = buf_len;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	
	if((tmplen = BuildXmlGroups(group , ptr , tmplen)) < 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(buf);

}

/*
*remark : 根据struct结构数据构建logger结点的字符串
*/
static int BuildXmLogger(const struct mgtcgi_xml_loggers* const logger,char * buf, const int buf_len)
{	
	std::string strtemp="";

	const char* format_str = 
		"  <logger>\n"	\
		"	 <block_log>%d</block_log>\n"  \
		"	 <http_log>%d</http_log>\n"	\
		"	 <session_log>%d</session_log>\n"  \
		"	 <session_layer7_log>%s</session_layer7_log>\n"  \
		"	 <scan_detect_log>%d</scan_detect_log>\n"  \
		"	 <dos_detect_log>%d</dos_detect_log>\n"	\
		"  </logger>\n" ;
	
	int index = 0;
	strtemp = "";

	for(index = 0; index < logger->num; ++index){
		strtemp.append(logger->layer7log[index].name);
		strtemp.append(" ");
	}
	
	snprintf(buf ,buf_len, format_str,	
		logger->block_log_enable,
		logger->http_log_enable,
		logger->session_log_enable,
		strtemp.c_str(),
		logger->scan_detect_log_enable,
		logger->dos_detect_log_enable);
	
	return strlen(buf);
}

static int BuildXmlFirewallPolicy(const struct mgtcgi_xml_firewalls_policy* const firewallpolicy, char *buf, const int buf_len)
{
	std::string strtemp="";
	const char* main_format_str =	
		"<firewalls_policy>\n"
		"<firewall_enable>%u</firewall_enable>\n"
		"%s"
		"</firewalls_policy>\n";

	const char* format_str = "	<firewall id=\"%u\" enable=\"%u\" src=\"%s\" dst=\"%s\" isp=\"%s\" proto=\"%s\" session=\"%s\" iface=\"%s\" action=\"%s\" timed=\"%s\" week=\"%s\" day=\"%s\" in_limit=\"%s\" out_limit=\"%s\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < firewallpolicy->num; index++){
		bufindex += snprintf(buf + bufindex, buf_len, format_str,
			firewallpolicy->pinfo[index].id,
			firewallpolicy->pinfo[index].enable,
			firewallpolicy->pinfo[index].src,
			firewallpolicy->pinfo[index].dst,
			firewallpolicy->pinfo[index].isp,
			firewallpolicy->pinfo[index].proto,
			firewallpolicy->pinfo[index].session,
			firewallpolicy->pinfo[index].iface,
			firewallpolicy->pinfo[index].action,
			firewallpolicy->pinfo[index].timed,
			firewallpolicy->pinfo[index].week,
			firewallpolicy->pinfo[index].day,
			firewallpolicy->pinfo[index].in_limits,
			firewallpolicy->pinfo[index].out_limits,
			firewallpolicy->pinfo[index].comment);
			
		if(bufindex >= buf_len) break;
	}
	
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf, buf_len, main_format_str, firewallpolicy->enable, strtemp.c_str());
	
	return strlen(buf);
}

static int BuildXmlWifi(const struct mgtcgi_xml_wifis* const wifi, char *buf, const int buf_len)
{
	int index;
	int poolbuflen = 0;
	std::string admin_str = "";
	std::string admins = "";
	std::string pool_str = "";
	std::string pools = "";

	const char *mainformat =
		"  <wifi>\n"  \
		"    <enable>%d</enable>\n"  \
		"    <pc_discover_time>%d</pc_discover_time>\n"  \
		"    <max_time>%d</max_time>\n"  \
		"    <share_num>%d</share_num>\n"  \
		"    <admins>%s</admins>\n"
		"%s"  \
		"  </wifi>\n";

	const char *poolsformat = 
		"    <address_pools>\n"  \
		"%s"  \
		"    </address_pools>\n";
	const char *pool = 
		"      <pool id=\"%d\" startip=\"%s\" endip=\"%s\" comment=\"%s\" enable=\"%d\" />\n";

	for(index = 0; index < wifi->pool_num; index++){
		poolbuflen+= snprintf(buf+poolbuflen, buf_len - poolbuflen, pool,
					wifi->pinfo[index].id,
					wifi->pinfo[index].startip,
					wifi->pinfo[index].endip,
					wifi->pinfo[index].comment,
					wifi->pinfo[index].enable);
		if(poolbuflen > buf_len) break;
	}
	pool_str = buf;
	buf[0] = '\0';
	snprintf(buf,buf_len,poolsformat,pool_str.c_str());
	pools = buf;
	buf[0] = '\0';

	snprintf(buf,buf_len,mainformat,
				wifi->enable,
				wifi->pc_discover_time,
				wifi->max_time,
				wifi->share_num,
				wifi->admins,
				pools.c_str());
	return strlen(buf);
}

static int BuildXmlTimingAdsl(const struct mgtcgi_xml_adsl_timings* const timing_adsl,char * buf, const int buf_len)
{
    int index = 0;
    int buflenth = 0;
    std::string adsl_main_str = "";
    std::string adsl_str = "";
    const char *mainformat_str = 
		"  <timings>\n"  \
		"%s"  \
		"  </timings>\n";
    const char *adsl_mainformat_str =
		"    <adsls>\n"  \
	       "%s"  \
	       "    </adsls>\n";
    const char *adsl_format_str =
		"    <adsl name=\"%s\" interface=\"%s\" minute=\"%s\" hour=\"%s\" day=\"%s\" month=\"%s\" week=\"%s\" comment=\"%s\" />\n";

    for(;index < timing_adsl->num; index++){
        buflenth += snprintf(buf+buflenth, buf_len - buflenth, adsl_format_str, 
			timing_adsl->pinfo[index].name,
			timing_adsl->pinfo[index].iface,
			timing_adsl->pinfo[index].minute,
			timing_adsl->pinfo[index].hour,
			timing_adsl->pinfo[index].day,
			timing_adsl->pinfo[index].month,
			timing_adsl->pinfo[index].week,
			timing_adsl->pinfo[index].comment);
	if(buflenth > buf_len) break;
    }
    adsl_str = buf;
    buf[0] = '\0';	

    snprintf(buf, buf_len, adsl_mainformat_str, adsl_str.c_str());
    adsl_main_str = buf;
    buf[0] = '\0';	

    snprintf(buf, buf_len, mainformat_str, adsl_main_str.c_str());
    return strlen(buf);
}

static int BuildXmlRouteServer(const struct mgtcgi_xml_routeserver* const info,char * buf, const int buf_len)
{
    const char *mainformat_str = 
		"  <routeserverinfos>\n"  \
		"    <enable>%d</enable>\n"  \
		"    <passwd_enable>%d</passwd_enable>\n"  \
		"  </routeserverinfos>\n";
	
    snprintf(buf, buf_len, mainformat_str, info->enable, info->passwd_enable);
    return strlen(buf);
}

static inline std::string ConvertRateToString(const char * const rate)
{
	int64_t tmprate ; 
	std::string strrate;
	
	tmprate = strtoint(rate);
	if(tmprate>=1000000){
		strrate = inttostr(tmprate/1000000);
		strrate.append("Mbps");
	}else if(tmprate>=1000){
		strrate = inttostr(tmprate/1000);
		strrate.append("Kbps");
	}else {
		strrate = tmprate;
	}

	return strrate;
}

/*
*remark : 根据struct结构数据构建traffic结点的字符串
*/
#if 0
#define TF_PRINT(...) printf(__VA_ARGS__)
#else
#define TF_PRINT(...) 
#endif
static int BuildXmTraffic(const struct mgtcgi_xml_traffics* const traffic,
								char * buf, const int buf_len)
{	
	const int MAX_WHIL_STACK = 65535; int max_whil_i = 0;
	const int UPLOAD = 0;	//在bridgechannel中保存的数据序号
	const int DOWNLOAD = 1;//在bridgechannel中保存的数据序号
	const int TYPE_SHARE_CHANNEL = 0 ; //共享通道classtype
	const int TYPE_TRAFFIC_CHANNEL = 1 ; //流量通道classtype
	const int TYPE_VC_CHANNEL = 2 ; //流量通道的VC通道classtype
	const int TYPE_ROOT_CHANNEL = 3 ; //上传下载根通道classtype
	std::vector<struct mgtcgi_xml_traffic_channel_info*> bridgeTraffic[2][MAX_BRIDGE_NUM]; //保存10个桥的分别数据，每个桥[0]上传，[1]为下载
	std::map<std::string,bool> lastTrafficname[2][MAX_BRIDGE_NUM];	
	std::vector<struct mgtcgi_xml_traffic_channel_info*> bridgeShare[2][MAX_BRIDGE_NUM]; //保存10个桥的分别数据，每个桥[0]上传，[1]为下载
	std::map<std::string,bool> lastSharename[2][MAX_BRIDGE_NUM];
	std::map<std::string,bool>::iterator iter;
	std::list<struct mgtcgi_xml_traffic_channel_info*> waitchannel;
	
	std::string strtemp , strAllBuf;
	struct mgtcgi_xml_traffic_channel_info* tmpPtr = NULL;
	struct mgtcgi_xml_traffic_channel_info* mainchnnelPtr = NULL;
	
	const char* mainnode_format_str =
		"  <traffic>\n"  \
		"    <traffic_control>%d</traffic_control>\n"  \
		"%s"  \
		"  </traffic>\n" ;
	const char* mainchannels_format_str =
		"	 <channels classname=\"%s\" bandwidth=\"%s\" bridge=\"%d\" comment=\"%s\">\n" \
		"%s"  \
		"	 </channels>\n" ;
	const char* format_str = 
		"	   <channel classname=\"%s\" parentname=\"%s\" sharename=\"%s\" "   \
		" classtype=\"%d\" rate=\"%s\" priority=\"%d\" limit=\"%d\" " 		\
		" default=\"%d\" attribute=\"%d\" weight=\"%d\" bridge=\"%d\" comment=\"%s\" />\n" ;		
	
	//int bufindex = 0;
	int index = 0;
	
	for(index = 0; index < traffic->num; ++index){
		tmpPtr = (struct mgtcgi_xml_traffic_channel_info*)&(traffic->pinfo[index]);		
		waitchannel.push_back(tmpPtr);
	}
	
	index = 0;
	while(waitchannel.size() > 0 && (++max_whil_i) <= MAX_WHIL_STACK){
		tmpPtr = (struct mgtcgi_xml_traffic_channel_info*)(waitchannel.front());
		waitchannel.pop_front();
			
TF_PRINT("classname=%s classtype=%d direct=%d\n" ,tmpPtr->classname,tmpPtr->classtype,tmpPtr->direct); 
		if(tmpPtr->classtype== TYPE_ROOT_CHANNEL ){
			if(strcmp(tmpPtr->classname , "upload")== 0){
TF_PRINT("classname=%s \t\t--1 \n" ,tmpPtr->classname); 				bridgeTraffic[UPLOAD][tmpPtr->bridge].push_back(tmpPtr);
				lastTrafficname[UPLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type("upload",true));
				
				bridgeShare[UPLOAD][tmpPtr->bridge].push_back(tmpPtr);
				lastSharename[UPLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type("upload",true));
				continue;
			}
			else if(strcmp(tmpPtr->classname ,"download")== 0){
				bridgeTraffic[DOWNLOAD][tmpPtr->bridge].push_back(tmpPtr);
				lastTrafficname[DOWNLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type("download",true));
				
				bridgeShare[DOWNLOAD][tmpPtr->bridge].push_back(tmpPtr);
				lastSharename[DOWNLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type("download",true));
				continue;
			}
			
		}
		else if(tmpPtr->classtype== TYPE_SHARE_CHANNEL){
			if(tmpPtr->direct == UPLOAD ){
				iter = lastSharename[UPLOAD][tmpPtr->bridge].find(std::string(tmpPtr->parentname));
				if (iter != lastSharename[UPLOAD][tmpPtr->bridge].end()){			
				   bridgeShare[UPLOAD][tmpPtr->bridge].push_back(tmpPtr);				   
				   lastSharename[UPLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type(tmpPtr->classname,true));
TF_PRINT("classname=%s \t\t--2 \n" ,tmpPtr->classname); 		   
			   }
			   else{
			    	waitchannel.push_back(tmpPtr);
TF_PRINT("classname=%s \t\t--3 \n" ,tmpPtr->classname); 
			   }			
			}
			else{ 
				 iter = lastSharename[DOWNLOAD][tmpPtr->bridge].find(std::string(tmpPtr->parentname));
				 if (iter != lastSharename[DOWNLOAD][tmpPtr->bridge].end()){			 
					bridgeShare[DOWNLOAD][tmpPtr->bridge].push_back(tmpPtr);				
					lastSharename[DOWNLOAD][tmpPtr->bridge].insert(
						 std::map<std::string,bool>::value_type(tmpPtr->classname,true));
				
TF_PRINT("classname=%s \t\t--4 \n" ,tmpPtr->classname); 
				}
				else{
					 waitchannel.push_back(tmpPtr);
TF_PRINT("classname=%s \t\t--5 \n" ,tmpPtr->classname); 
				}			
			}
			continue;
		}
		else if(tmpPtr->classtype== TYPE_TRAFFIC_CHANNEL ||
				tmpPtr->classtype== TYPE_VC_CHANNEL){
			if(tmpPtr->direct == UPLOAD ){
				iter = lastTrafficname[UPLOAD][tmpPtr->bridge].find(std::string(tmpPtr->parentname));
				if (iter != lastTrafficname[UPLOAD][tmpPtr->bridge].end()){			
				   bridgeTraffic[UPLOAD][tmpPtr->bridge].push_back(tmpPtr); 			   
				   lastTrafficname[UPLOAD][tmpPtr->bridge].insert(
						std::map<std::string,bool>::value_type(tmpPtr->classname,true));
TF_PRINT("classname=%s \t\t--6 \n" ,tmpPtr->classname); 		   
			   }
			   else{
					waitchannel.push_back(tmpPtr);
TF_PRINT("classname=%s \t\t--7 \n" ,tmpPtr->classname); 
			   }			
			}
			else{ 
				 iter = lastTrafficname[DOWNLOAD][tmpPtr->bridge].find(std::string(tmpPtr->parentname));
				 if (iter != lastTrafficname[DOWNLOAD][tmpPtr->bridge].end()){			 
					bridgeTraffic[DOWNLOAD][tmpPtr->bridge].push_back(tmpPtr);				
					lastTrafficname[DOWNLOAD][tmpPtr->bridge].insert(
						 std::map<std::string,bool>::value_type(tmpPtr->classname,true));
TF_PRINT("classname=%s \t\t--8 \n" ,tmpPtr->classname); 
				}
				else{
					 waitchannel.push_back(tmpPtr);
TF_PRINT("classname=%s \t\t--9 \n" ,tmpPtr->classname); 
				}	 
			}
			continue;
		}
	}

	if(waitchannel.size() > 0){
		strncpy(buf , "" , buf_len);
		return 0;
	}

	for(int ibridge=0; ibridge < MAX_BRIDGE_NUM; ++ibridge){	
		if(bridgeTraffic[UPLOAD][ibridge].size()== 0)
			break;
		strtemp = "";
		for(int idirect=0; idirect < 2 ; ++idirect){	
			
			for(index = 0; index< (int)bridgeShare[idirect][ibridge].size();++index){
				tmpPtr = (struct mgtcgi_xml_traffic_channel_info*)
							(bridgeShare[idirect][ibridge].at(index));		
				if(tmpPtr->classtype == TYPE_ROOT_CHANNEL){					
					mainchnnelPtr = tmpPtr;
				}
				else {					
					snprintf(buf ,buf_len, format_str,
						tmpPtr->classname,
						tmpPtr->parentname,
						tmpPtr->sharename,
						tmpPtr->classtype,
						tmpPtr->rate,
						tmpPtr->priority,
						tmpPtr->limit,
						tmpPtr->default_channel,
						tmpPtr->attribute,
						tmpPtr->weight,
						tmpPtr->bridge,
						tmpPtr->comment);	
					strtemp.append(buf);
				}
				
			}			
			
			for(index = 0; index< (int)bridgeTraffic[idirect][ibridge].size();++index){
				tmpPtr = (struct mgtcgi_xml_traffic_channel_info*)
							(bridgeTraffic[idirect][ibridge].at(index));

				if(tmpPtr->classtype == TYPE_ROOT_CHANNEL){					
					mainchnnelPtr = tmpPtr;
				}
				else {		
					snprintf(buf ,buf_len, format_str,
						tmpPtr->classname,
						tmpPtr->parentname,
						tmpPtr->sharename,
						tmpPtr->classtype,
						tmpPtr->rate,
						tmpPtr->priority,
						tmpPtr->limit,
						tmpPtr->default_channel,
						tmpPtr->attribute,
						tmpPtr->weight,
						tmpPtr->bridge,
						tmpPtr->comment);
					strtemp.append(buf);	

				}
			}

			snprintf(buf ,buf_len, mainchannels_format_str,	
				mainchnnelPtr->classname,
				mainchnnelPtr->rate,
				mainchnnelPtr->bridge,
				mainchnnelPtr->comment,
				strtemp.c_str());
			
			strAllBuf.append(buf);
			strtemp = "";
		}
	}
	snprintf(buf ,buf_len, mainnode_format_str,	
			traffic->status,
			strAllBuf.c_str());
    for(index = 0;index < 2; index ++){
		for(int br=0;br<MAX_BRIDGE_NUM;br++)
		{
			bridgeTraffic[index][br].clear();
			bridgeShare[index][br].clear();
			lastTrafficname[index][br].clear();
			lastSharename[index][br].clear();
		}
    }
	TF_PRINT("\n\n---------------------------\nbuf_len = %d\n%s\n",buf_len,buf);
	
	return strlen(buf);
	
}



/*
*remark : 根据struct结构数据构建firewalls结点的字符串
*/
static int BuildXmFrewalls(const struct mgtcgi_xml_firewalls* const firewalls,char * buf, const int buf_len)
{	
	/*std::string strtemp="";
	const char* mainnode_format_str =
			"  <firewalls>\n"  \
			"    <application_firewall>%d</application_firewall>\n"  \
			"    <scan_detect>%d</scan_detect>\n"  \
			"    <dos_detect>%d</dos_detect>\n"  \
				"%s"  \
			"  </firewalls>\n" ;
	const char* format_str = 
	"    <firewall name=\"%d\" proto=\"%s\" auth=\"%s\" action=\"%s\" schedule=\"%s\" " \
	" session_limit=\"%s\" vlan=\"%s\" src=\"%s\" dst=\"%s\" mac=\"%s\" in_traffic=\"%s\" "\
	" out_traffic=\"%s\" quota=\"%s\" quota_action=\"%d\" second_in_traffic=\"%s\" " \
	" second_out_traffic=\"%s\" log=\"%d\" " \
	" disabled=\"%d\" bridge=\"%d\" comment=\"%s\" />\n";
	*/
	std::string strtemp="";
	int index = 0;
	const char *const head_str = "<firewalls>\n";
	const char *const foot_str = "</firewalls>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	strcpy(ptr ,  "<application_firewall>");
	ptr += strlen("<application_firewall>");	
	strtemp = inttostr(firewalls->application_firewall);
	strcpy(ptr , strtemp.c_str());
	ptr += strtemp.length();
	strcpy(ptr ,  "</application_firewall>\n<scan_detect>");
	ptr += strlen("</application_firewall>\n<scan_detect>");
	strtemp = inttostr(firewalls->scan_detect);
	strcpy(ptr , strtemp.c_str());
	ptr += strtemp.length();
	strcpy(ptr ,  "</scan_detect>\n<dos_detect>");
	ptr += strlen("</scan_detect>\n<dos_detect>");
	strtemp = inttostr(firewalls->dos_detect);
	strcpy(ptr , strtemp.c_str());
	ptr += strtemp.length();
	strcpy(ptr ,  "</dos_detect>\n");
	ptr += strlen("</dos_detect>\n");	

	for(index = 0; index < firewalls->num; ++index){
		strcpy(ptr ,  "<firewall name=\"");
		ptr += strlen("<firewall name=\"");	
		strtemp = inttostr(firewalls->pinfo[index].name);
		strcpy(ptr , strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" proto=\"");
		ptr += strlen("\" proto=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].proto);
		ptr += strlen(firewalls->pinfo[index].proto);	
		strcpy(ptr ,  "\" auth=\"");
		ptr += strlen("\" auth=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].auth);
		ptr += strlen(firewalls->pinfo[index].auth);	
		strcpy(ptr ,  "\" action=\"");
		ptr += strlen("\" action=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].action);
		ptr += strlen(firewalls->pinfo[index].action);	
		strcpy(ptr ,  "\" schedule=\"");
		ptr += strlen("\" schedule=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].schedule);
		ptr += strlen(firewalls->pinfo[index].schedule);	
		strcpy(ptr ,  "\" session_limit=\"");
		ptr += strlen("\" session_limit=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].session_limit);
		ptr += strlen(firewalls->pinfo[index].session_limit);	
		strcpy(ptr ,  "\" vlan=\"");
		ptr += strlen("\" vlan=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].vlan);
		ptr += strlen(firewalls->pinfo[index].vlan);	
		strcpy(ptr ,  "\" src=\"");
		ptr += strlen("\" src=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].src);
		ptr += strlen(firewalls->pinfo[index].src);	
		strcpy(ptr ,  "\" dst=\"");
		ptr += strlen("\" dst=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].dst);
		ptr += strlen(firewalls->pinfo[index].dst);	
		strcpy(ptr ,  "\" mac=\"");
		ptr += strlen("\" mac=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].mac);
		ptr += strlen(firewalls->pinfo[index].mac);	
		strcpy(ptr ,  "\" in_traffic=\"");
		ptr += strlen("\" in_traffic=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].in_traffic);
		ptr += strlen(firewalls->pinfo[index].in_traffic);	
		strcpy(ptr ,  "\" out_traffic=\"");
		ptr += strlen("\" out_traffic=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].out_traffic);
		ptr += strlen(firewalls->pinfo[index].out_traffic);	
		strcpy(ptr ,  "\" quota=\"");
		ptr += strlen("\" quota=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].quota);
		ptr += strlen(firewalls->pinfo[index].quota);	
		strcpy(ptr ,  "\" quota_action=\"");
		ptr += strlen("\" quota_action=\"");	
		strtemp = inttostr(firewalls->pinfo[index].quota_action);
		strcpy(ptr , strtemp.c_str());
		ptr += strtemp.length();		
		strcpy(ptr ,  "\" second_in_traffic=\"");
		ptr += strlen("\" second_in_traffic=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].second_in_traffic);
		ptr += strlen(firewalls->pinfo[index].second_in_traffic);	
		strcpy(ptr ,  "\" second_out_traffic=\"");
		ptr += strlen("\" second_out_traffic=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].second_out_traffic);
		ptr += strlen(firewalls->pinfo[index].second_out_traffic);
		strcpy(ptr ,  "\" log=\"");
		ptr += strlen("\" log=\"");	
		strtemp = inttostr(firewalls->pinfo[index].log);
		strcpy(ptr , strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" disabled=\"");
		ptr += strlen("\" disabled=\"");
		strtemp = inttostr(firewalls->pinfo[index].disabled);
		strcpy(ptr , strtemp.c_str());
		ptr += strtemp.length();		
		strcpy(ptr ,  "\" bridge=\"");
		ptr += strlen("\" bridge=\"");
		strtemp = inttostr(firewalls->pinfo[index].bridge);
		strcpy(ptr , strtemp.c_str());
		ptr += strtemp.length();		
		strcpy(ptr ,  "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr ,  firewalls->pinfo[index].comment);
		ptr += strlen(firewalls->pinfo[index].comment);	
		strcpy(ptr ,  "\" />\n");
		ptr += strlen("\" />\n");	
			
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}


/*
*remark : 根据struct结构数据构建dns_policy结点的字符串
*/
static int BuildXmDnsPolicy(const struct mgtcgi_xml_dns_policys* const dns_policy,char * buf, const int buf_len)
{	
	/*
	const char* mainnode_format_str =
			"  <dns_policy>\n"  \
			"    <application_policy>%d</application_policy>\n"  \
				"%s"  \
			"  </dns_policy>\n" ;
	const char* format_str = 
	"    <dataitem name=\"%d\" domain_name=\"%s\" bridge=\"%d\" src=\"%s\"  " \
	" dst=\"%s\" action=\"%d\" targetip=\"%s\" comment=\"%s\" disabled=\"%d\" />\n" ;
	*/
	std::string strtemp="";
	int index = 0;
	const char *const head_str = "<dns_policy>\n";
	const char *const foot_str = "</dns_policy>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str); 
	strcpy(ptr ,  "<application_policy>");
	ptr += strlen("<application_policy>");	
	strtemp = inttostr(dns_policy->application_policy);
	strcpy(ptr ,  strtemp.c_str());
	ptr += strtemp.length();	
	strcpy(ptr ,  "</application_policy>\n");
	ptr += strlen("</application_policy>\n");	
	
	for(index = 0; index < dns_policy->num; ++index){
		strcpy(ptr ,  "<dataitem name=\"");
		ptr += strlen("<dataitem name=\"");		
		strtemp = inttostr(dns_policy->pinfo[index].name);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" domain_name=\"");
		ptr += strlen("\" domain_name=\"");	
		strcpy(ptr ,  dns_policy->pinfo[index].domain_name);
		ptr += strlen(dns_policy->pinfo[index].domain_name);	
		strcpy(ptr ,  "\" bridge=\"");
		ptr += strlen("\" bridge=\"");			
		strtemp = inttostr(dns_policy->pinfo[index].bridge);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();
		strcpy(ptr ,  "\" src=\"");
		ptr += strlen("\" src=\"");	
		strcpy(ptr ,  dns_policy->pinfo[index].src);
		ptr += strlen(dns_policy->pinfo[index].src);	
		strcpy(ptr ,  "\" dst=\"");
		ptr += strlen("\" dst=\"");	
		strcpy(ptr ,  dns_policy->pinfo[index].dst);
		ptr += strlen(dns_policy->pinfo[index].dst);	
		strcpy(ptr ,  "\" action=\"");
		ptr += strlen("\" action=\"");			
		strtemp = inttostr(dns_policy->pinfo[index].action);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();
		strcpy(ptr ,  "\" targetip=\"");
		ptr += strlen("\" targetip=\"");	
		strcpy(ptr ,  dns_policy->pinfo[index].targetip);
		ptr += strlen(dns_policy->pinfo[index].targetip);	
		strcpy(ptr ,  "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr ,  dns_policy->pinfo[index].comment);
		ptr += strlen(dns_policy->pinfo[index].comment);	
		strcpy(ptr ,  "\" disabled=\"");
		ptr += strlen("\" disabled=\"");		
		strtemp = inttostr(dns_policy->pinfo[index].disabled);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");	
	
		if((ptr-buf+30)>= buf_len) break;
	}

	strcpy(ptr , foot_str);
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建http_dir_policy结点的字符串
*/
static int BuildXmHttpDirPolicy(const struct mgtcgi_xml_http_dir_policys* const http_dir_policy,char * buf, const int buf_len)
{	
	/*
	const char* mainnode_format_str =
			"  <http_dir_policy>\n"  \
			"%s"  \
			"  </http_dir_policy>\n" ;
	const char* format_str_policy = 
		"    <policy bridge=\"%d\" extgroup=\"%s\" name=\"%d\" disabled=\"%d\" servergroup=\"%s\" comment=\"%s\" />\n" ;
	
	const char* format_str_urlformat = 
		"    <url_format bridge=\"%d\" format=\"%s\" comment=\"%s\" />\n ";
	*/
	std::string strtemp="";
	int index = 0;
	const char *const head_str = "<http_dir_policy>\n";
	const char *const foot_str = "</http_dir_policy>\n";
	char * ptr = buf ;
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	
	for(index = 0; index < http_dir_policy->policy_num; ++index){	
		strcpy(ptr ,  "<policy bridge=\"");
		ptr += strlen("<policy bridge=\"");	
		strtemp = inttostr(http_dir_policy->p_policyinfo[index].bridge);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" extgroup=\"");
		ptr += strlen("\" extgroup=\"");	
		strcpy(ptr ,  http_dir_policy->p_policyinfo[index].extgroup);
		ptr += strlen(http_dir_policy->p_policyinfo[index].extgroup);	
		strcpy(ptr ,  "\" name=\"");
		ptr += strlen("\" name=\"");
		strtemp = inttostr(http_dir_policy->p_policyinfo[index].name);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" disabled=\"");
		ptr += strlen("\" disabled=\"");	
		strtemp = inttostr(http_dir_policy->p_policyinfo[index].disabled);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" servergroup=\"");
		ptr += strlen("\" servergroup=\"");	
		strcpy(ptr ,  http_dir_policy->p_policyinfo[index].servergroup);
		ptr += strlen(http_dir_policy->p_policyinfo[index].servergroup);	
		strcpy(ptr ,  "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr ,  http_dir_policy->p_policyinfo[index].comment);
		ptr += strlen(http_dir_policy->p_policyinfo[index].comment);		
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");

		if((ptr-buf+30)>= buf_len) break;
	}
	
	for(index = 0; index < http_dir_policy->format_num; ++index){
		if((ptr-buf+30)>= buf_len) break;

		strcpy(ptr ,  "<url_format bridge=\"");
		ptr += strlen("<url_format bridge=\"");	
		strtemp = inttostr(http_dir_policy->p_urlinfo[index].bridge);
		strcpy(ptr ,  strtemp.c_str());
		ptr += strtemp.length();	
		strcpy(ptr ,  "\" format=\"");
		ptr += strlen("\" format=\"");	
		strcpy(ptr ,  http_dir_policy->p_urlinfo[index].format);
		ptr += strlen(http_dir_policy->p_urlinfo[index].format);	
		strcpy(ptr ,  "\" comment=\"");
		ptr += strlen("\" comment=\"");	
		strcpy(ptr ,  http_dir_policy->p_urlinfo[index].comment);
		ptr += strlen(http_dir_policy->p_urlinfo[index].comment);
		strcpy(ptr , "\" />\n");
		ptr += strlen("\" />\n");		
	}	

	strcpy(ptr , foot_str);

	return strlen(buf);
}


/*
*remark : 根据struct结构数据构建global_ip_session_limits结点的字符串
*/
static int BuildXmGlobalIpSessionLimits(const struct mgtcgi_xml_ip_session_limits* const global_ip_session_limits,char * buf, const int buf_len)
{	
	std::string strtemp = "";
	const char* mainnode_format_str =
			"  <global_ip_session_limits>\n"  \
			"%s"  \
			"  </global_ip_session_limits>\n" ;
	const char* format_str_total = 		
		"	 <total_session_limit bridge=\"%d\" limit=\"%d\" overhead=\"%s\" />\n" ;
	
	const char* format_str_global = 
		"    <global_ip_session_limit name=\"%d\" addr=\"%s\" dst=\"%s\" per_ip_session_limit=\"%s\" " \
		" total_session_limit=\"%s\" httplog=\"%d\" sesslog=\"%d\" httpdirpolicy=\"%d\" dnspolicy=\"%d\"  " \
		" action=\"%s\" bridge=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	//printf("1 %d\n",global_ip_session_limits->global_limit_num);

	for(index = 0; index < global_ip_session_limits->total_limit_num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str_total,
			global_ip_session_limits->p_totalinfo[index].bridge,
			global_ip_session_limits->p_totalinfo[index].limit,
			global_ip_session_limits->p_totalinfo[index].overhead);
		
		if(bufindex >= buf_len) break;
	}
	strtemp = (buf);
	buf[0] = '\0';
	bufindex = 0;
	for(index = 0; index < global_ip_session_limits->global_limit_num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str_global,
			global_ip_session_limits->p_globalinfo[index].name,
			global_ip_session_limits->p_globalinfo[index].addr,
			global_ip_session_limits->p_globalinfo[index].dst,
			global_ip_session_limits->p_globalinfo[index].per_ip_session_limit,
			global_ip_session_limits->p_globalinfo[index].total_session_limit,
			global_ip_session_limits->p_globalinfo[index].httplog,
			global_ip_session_limits->p_globalinfo[index].sesslog,
			global_ip_session_limits->p_globalinfo[index].httpdirpolicy,
			global_ip_session_limits->p_globalinfo[index].dnspolicy,
			global_ip_session_limits->p_globalinfo[index].action,
			global_ip_session_limits->p_globalinfo[index].bridge,
			global_ip_session_limits->p_globalinfo[index].comment);
		
		if(bufindex >= buf_len) break;
	}
	strtemp.append(buf);
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	strtemp.c_str());
	//printf(buf);
	return strlen(buf);

}


/*
*remark : 根据struct结构数据构建port_mirrors结点的字符串
*/
static int BuildXmPortMirrors(const struct mgtcgi_xml_port_mirrors* const port_mirrors,char * buf, const int buf_len)
{	
	std::string strtemp="";
	const char* mainnode_format_str =
			"  <port_mirrors>\n"  \
			"%s"  \
			"  </port_mirrors>\n" ;
	const char* format_str_enable = 		
		"    <port_mirror_enable bridge=\"%d\" enable=\"%d\" />\n" ;
	
	const char* format_str_policy = 
		"    <port_mirror name=\"%d\" proto=\"%s\" action=\"%s\" src=\"%s\" dst=\"%s\" disabled=\"%d\" bridge=\"%d\" comment=\"%s\" />\n";

	int bufindex = 0;
	int index = 0;


	for(index = 0; index < MAX_BRIDGE_NUM; ++index){
		if(index> 0 &&
			port_mirrors->pstatus[index].bridge == 0 && 
			port_mirrors->pstatus[index].enable == 0)
				break;
		
		bufindex += snprintf(buf+bufindex ,buf_len, format_str_enable,
			port_mirrors->pstatus[index].bridge,
			port_mirrors->pstatus[index].enable);
		
		if(bufindex >= buf_len) break;
	}
	strtemp = buf;
	buf[0] = '\0';	
	bufindex = 0;
	for(index = 0; index < port_mirrors->num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str_policy,
			port_mirrors->pinfo[index].name,
			port_mirrors->pinfo[index].proto,
			port_mirrors->pinfo[index].action,
			port_mirrors->pinfo[index].src,
			port_mirrors->pinfo[index].dst,
			port_mirrors->pinfo[index].disabled,
			port_mirrors->pinfo[index].bridge,
			port_mirrors->pinfo[index].comment);

		if(bufindex >= buf_len) break;
	}
	strtemp.append(buf);
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	
			strtemp.c_str());
	
	return strlen(buf);
}

/*
*remark : 根据struct结构数据构建session_limit结点的字符串
*/
static int BuildXmlSessionLimit(const struct mgtcgi_xml_session_limits* const session_limit,char * buf, const int buf_len)
{	
	std::string strtemp="";
	const char* mainnode_format_str =
			"  <session_limit>\n"  \
			"%s"  \
			"  </session_limit>\n" ;
	const char* format_str = 		
			"    <sessionlimit name=\"%s\" value=\"%s\" />\n"  ;
	
	int bufindex = 0;
	int index = 0;

	for(index = 0; index < session_limit->num; ++index){
		bufindex += snprintf(buf+bufindex ,buf_len, format_str,
			session_limit->pinfo[index].name,
			session_limit->pinfo[index].value);

		if(bufindex >= buf_len) break;
	}
	
	strtemp = buf;
	buf[0] = '\0';	
	snprintf(buf ,buf_len, mainnode_format_str,	
			strtemp.c_str());
	
	return strlen(buf);
}

/* 
remark: 调用命令行处理函数，处理命令行输入
return: 成功返回0
*/
int xml_do_command(int argc, char *argv[])
{
	Configuration* config = NULL;

    Configuration real_config(&argc, &argv);
    config = &real_config;
	int ret;

    Log * pLog = new Log("/var/log/"PACKAGE_LONG_NAME".log", config->syslog, false, config->console);
	
    if(config->read)
    {
        Reader * xml_reader = new Reader(pLog, config->main_config_filename);
        ret = xml_reader->getNode(config->key);
		delete pLog;
		delete xml_reader;
		return ret;
    }

    if(config->write)
    {
        Writer * xml_writer = new Writer(pLog, config->main_config_filename);
		ret = xml_writer->setNode(config->key, config->value);
		delete pLog;
		delete xml_writer;
		return ret;
    }
#if 0
/*删除节点功能，暂不提供*/
	if(config->delete_node)
	{
		RWXml * rwxml = new RWXml(pLog , config->main_config_filename);

        return rwxml->deleteNode(config->key);	
	}
	if(config->replace_node)
	{
		std::string content= "   <global_ip_session_limits> \n"	\
    "  <total_session_limit bridge=\"0\" limit=\"1024\" overhead=\"bypass\" /> " \
	"    <total_session_limit bridge=\"1\" limit=\"2048\" overhead=\"block\" />   " \
	"  </global_ip_session_limits>" ;
	
		RWXml * rwxml = new RWXml(pLog , config->main_config_filename);

        return rwxml->replaceNode(config->key , content);	
	}
#endif

    Parser * xml_parser = new Parser(pLog,  config->main_config_filename,
                                     config->netaddr_config_filename,
                                     config->rfc_proto_config_filename,
                                     config->l7_proto_config_filename,
                                     "", true , //添加默认数据，如any ip地址
                                     config->check,
                                     config->print_fw,
                                     config->print_action,
                                     /*2012-09-28 add port_scan*/
                                     config->print_portscan,
                                     config->print_macbind,
                                     config->print_macauth,
                                     config->print_quota,
                                     config->print_globalip,
                                     config->print_port_mirror,
                                     config->print_dnshelper,
                                     config->print_httpdirpolicy);

    ret = xml_parser->process();
    delete pLog;
	delete xml_parser;
	
	return ret;
	
}


