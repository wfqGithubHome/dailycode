#include "route_xmlwrapper4c.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>

#define MAX_FILENAME_LEN 256
#define XML_ERROR -1
#if 0
#define DEBUGPR(...) fprintf(stdout,__VA_ARGS__)
#else
#define DEBUGPR(...) 
#endif

static int BuildXmlRouteDNS(const struct mgtcgi_xml_dns *const buf,char *strbuf, const int strbuf_len);
static int BuildXmlInterfaceLink(const struct mgtcgi_xml_interface_links *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteLAN(const struct mgtcgi_xml_route_interface_lans *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteWAN(const struct mgtcgi_xml_interface_wans *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteVLAN(const struct mgtcgi_xml_interface_vlans *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteMacVLAN(const struct mgtcgi_xml_interface_macvlans *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlroutingtables (const struct mgtcgi_xml_route_tables *const buf,char *strbuf, const int strbuf_len);
static int BuildXmlRoutePPPDAuth(const struct mgtcgi_xml_pppd_authenticate *const buf,char *strbuf, const int strbuf_len);
static int BuildXmlRouteADSLClient(const struct mgtcgi_xml_adsl_clients *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteVPNClient(const struct mgtcgi_xml_l2tpvpn_clients *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteLoadBalancing(const struct mgtcgi_xml_load_balancings *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteRouting(const struct mgtcgi_xml_routings *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteRule(const struct mgtcgi_xml_route_rules *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteDNAT(const struct mgtcgi_xml_nat_dnats *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteSNAT(const struct mgtcgi_xml_nat_snats *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteMACBind(const struct mgtcgi_xml_mac_binds *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteTAbleGruop(const struct mgtcgi_xml_group_array * const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteGroups(const struct mgtcgi_xml_group_array * const group,char * buf, const int buf_len);

static int BuildXmlRouteDDNS(const struct mgtcgi_xml_ddns *const buf,char *strbuf, const int strbuf_len);
static int BuildXmlRouteDhcpd(const struct mgtcgi_xml_dhcpd_servers *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRouteDhcpdHost(const struct mgtcgi_xml_dhcpd_hosts *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRoutePptpdServer(const struct mgtcgi_xml_pptpd_server *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlRoutePptpdAccount(const struct mgtcgi_xml_pptpd_accounts_info *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlWanChannel(const struct mgtcgi_xml_wans_channels *const wans_channel, char * buf, const int buf_len);

int get_route_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size)
{	

	maxnet::Log pLog("route-xmlconf_lib", false,false,false);
	maxnet::RouteParser xml_parser(&pLog,configfile, "",
									0, 0, 0, 0,0,
									0, 0, 0,
									0, 0,
									0, 0, 0, 0, 0, 0);
	char path[]="/var/lock/router.lock";
	int fd,ret;

//	std::cout << "get_route_xml_node: begin" << std::endl;

	fd=open(path,O_WRONLY|O_CREAT);
	if(fd < 0){
		*out_size = 0;
		*buf = NULL;
		return 0;
	}

//	std::cout << "get_route_xml_node: 2" << std::endl;
	
	if(flock(fd,LOCK_EX)!=0){
		*out_size = 0;
		*buf = NULL;
		close(fd);
		return 0;
	}

//	std::cout << "get_route_xml_node: FLOCK,lock success" << std::endl;
    ret = xml_parser.loadConfig();
	flock(fd,LOCK_UN);
	close(fd);
	
//	std::cout << "get_route_xml_node: FLOCK,unlock success" << std::endl;
	if(ret)
	{
		*out_size = 0;
		*buf = NULL;
		return 0;
	}
	
	switch(nodetype){
		case MGTCGI_TYPE_DNS:
			*out_size = xml_parser.GetStructRouteDNS(
						(struct mgtcgi_xml_dns**)buf);
			break;
		
		case MGTCGI_TYPE_INTERFACE_LANS:
			*out_size = xml_parser.GetStructRouteInterfaceLAN(
						(struct mgtcgi_xml_route_interface_lans**)buf);
			break;	
		case MGTCGI_TYPE_INTERFACE_WANS:
			*out_size = xml_parser.GetStructRouteInterfaceWAN(
						(struct mgtcgi_xml_interface_wans**)buf);
			break;
		case MGTCGI_TYPE_INTERFACE_LINKS:
			*out_size = xml_parser.GetStructRouteInterfaceLink(
						(struct mgtcgi_xml_interface_links**)buf);
			break;
		case MGTCGI_TYPE_INTERFACE_VLANS:
			*out_size = xml_parser.GetStructRouteInterfaceVLAN(
						(struct mgtcgi_xml_interface_vlans**)buf);
			break;

		case MGTCGI_TYPE_INTERFACE_MACVLANS:
			*out_size = xml_parser.GetStructRouteInterfaceMacVLAN(
				(struct mgtcgi_xml_interface_macvlans**)buf);
			break;

		case MGTCGI_TYPE_PPPD_AUTHENTICATE:
			*out_size = xml_parser.GetStructRoutePPPDAuth(
						(struct mgtcgi_xml_pppd_authenticate**)buf);
			break;

		case MGTCGI_TYPE_ADSL_CLIENTS:
			*out_size = xml_parser.GetStructRouteADSLClient(
						(struct mgtcgi_xml_adsl_clients**)buf);
			break;

		case MGTCGI_TYPE_L2TPVPN_CLIENTS:
			*out_size = xml_parser.GetStructRouteVPNClient(
						(struct mgtcgi_xml_l2tpvpn_clients**)buf);
			break;

		case MGTCGI_TYPE_LOAD_BALANCINGS:
			*out_size = xml_parser.GetStructRouteLoadBalancing(
						(struct mgtcgi_xml_load_balancings**)buf);
			break;

		case MGTCGI_TYPE_ROUTINGS:
			*out_size = xml_parser.GetStructRouteRouting(
						(struct mgtcgi_xml_routings**)buf);
			break;

		case MGTCGI_TYPE_ROUTE_TABLES:
			*out_size = xml_parser.GetStructRouteTableGroups(
						(struct mgtcgi_xml_group_array**)buf);
			break;
			
		case MGTCGI_TYPE_ROUTE_RULES:
			*out_size = xml_parser.GetStructRouteRule(
						(struct mgtcgi_xml_route_rules**)buf);
			break;
		case MGTCGI_TYPE_NAT_DNATS:
			*out_size = xml_parser.GetStructRouteDNAT(
						(struct mgtcgi_xml_nat_dnats**)buf);
			break;
		case MGTCGI_TYPE_NAT_SNATS:
			*out_size = xml_parser.GetStructRouteSNAT(
						(struct mgtcgi_xml_nat_snats**)buf);
			break;
		case MGTCGI_TYPE_MAC_BINDS:
			*out_size = xml_parser.GetStructRouteMACBind(
						(struct mgtcgi_xml_mac_binds **)buf);
			break;
		case MGTCGI_TYPE_DHCPD_SERVERS:
			*out_size = xml_parser.GetStructRouteDhcpd(
						(struct mgtcgi_xml_dhcpd_servers **)buf);
			break;
		case MGTCGI_TYPE_DHCPD_HOSTS:
			*out_size = xml_parser.GetStructRouteDhcpdHost(
						(struct mgtcgi_xml_dhcpd_hosts **)buf);
			break;
		case MGTCGI_TYPE_DDNS:
			*out_size = xml_parser.GetStructRouteDDNS(
						(struct mgtcgi_xml_ddns**)buf);
			break;
		case MGTCGI_TYPE_PPTPD_SERVERS:
			*out_size = xml_parser.GetStructPptpdServer(
						(struct mgtcgi_xml_pptpd_server**)buf);
			break;
		case MGTCGI_TYPE_PPTPD_ACCOUNT:
			*out_size = xml_parser.GetStructPptpdAccount(
						(struct mgtcgi_xml_pptpd_accounts_info**)buf);
			break;
		case MGTCGI_TYPE_WAN_CHANNEL:
			*out_size = xml_parser.GetStructWansChannnel(
						(struct mgtcgi_xml_wans_channels **)buf);
			break;
		case MGTCGI_TYPE_ROUTING_TABLES:
			*out_size = xml_parser.GetStructRouteTables(
						(struct mgtcgi_xml_route_tables **)buf);
			break;
			
		default:
			*out_size = 0;
			*buf = NULL;
	}

	return *out_size;
}


/*
int delete_route_xml_node(const char *configfile, const int nodetype,void **node)
{
	maxnet::Log pLog("route-xmlconf_lib", false,false,false);
	maxnet::RouteParser xml_parser(&pLog,configfile, "",
									0, 0, 0, 0,0,
									0, 0, 0,
									0, 0,
									0, 0, 0, 0, 0);
	int resize = 0;

	switch(nodetype){
		case MGTCGI_TYPE_MAC_BINDS:
			resize = xml_parser.DeleteXmlRouteMacBind(configfile,(struct mgtcgi_xml_mac_binds **)node);
			break;
		default:
			break;
	}
	return resize;
}*/
 
int save_route_xml_node(const char *filename, const int nodetype, const void * buf, const int buflen)
{
	#define STRING_BUFF_LEN 1024000
	maxnet::Log pLog("/var/log/route-xmlconf_lib.log", false,false,false);	

	char *strbuf=NULL;
	char xmlnodename[URL_LENGTH] = {0};
	int buildlen = 0;
	char path[]="/var/lock/router.lock";
	int fd;
	maxnet::Filelog filelog("test","/var/log/test.log");
	if (NULL == buf)
		return XML_ERROR;

	strbuf = (char *)malloc(sizeof(char) * STRING_BUFF_LEN);
	if(!strbuf)
		return XML_ERROR;
	
	memset(strbuf, 0, sizeof(char) * STRING_BUFF_LEN);
	switch(nodetype){
		case MGTCGI_TYPE_DNS:
			buildlen = BuildXmlRouteDNS((struct mgtcgi_xml_dns *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"dns" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACE_LINKS:
			buildlen = BuildXmlInterfaceLink((struct mgtcgi_xml_interface_links *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_link" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACE_LANS:
			buildlen = BuildXmlRouteLAN((struct mgtcgi_xml_route_interface_lans *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_lans" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACE_WANS:
			buildlen = BuildXmlRouteWAN((struct mgtcgi_xml_interface_wans *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_wans" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACE_VLANS:
			buildlen = BuildXmlRouteVLAN((struct mgtcgi_xml_interface_vlans *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_vlans" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_INTERFACE_MACVLANS:
			buildlen = BuildXmlRouteMacVLAN((struct mgtcgi_xml_interface_macvlans *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_macvlans" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_PPPD_AUTHENTICATE:
			buildlen = BuildXmlRoutePPPDAuth((struct mgtcgi_xml_pppd_authenticate *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"pppd_authenticate" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ADSL_CLIENTS:
			buildlen = BuildXmlRouteADSLClient((struct mgtcgi_xml_adsl_clients *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"adsl_clients" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_L2TPVPN_CLIENTS:
			buildlen = BuildXmlRouteVPNClient((struct mgtcgi_xml_l2tpvpn_clients *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"l2tpvpn_clients" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_LOAD_BALANCINGS:
			buildlen = BuildXmlRouteLoadBalancing((struct mgtcgi_xml_load_balancings *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"load_balancings" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ROUTINGS:
			buildlen = BuildXmlRouteRouting((struct mgtcgi_xml_routings *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"routings" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ROUTE_RULES:
			buildlen = BuildXmlRouteRule((struct mgtcgi_xml_route_rules *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"route_rules" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_NAT_DNATS:
			buildlen = BuildXmlRouteDNAT((struct mgtcgi_xml_nat_dnats *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"nat_dnats" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_NAT_SNATS:
			buildlen = BuildXmlRouteSNAT((struct mgtcgi_xml_nat_snats *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"nat_snats" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ROUTE_TABLES:
			buildlen = BuildXmlRouteTAbleGruop((struct mgtcgi_xml_group_array *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"route_tables" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_MAC_BINDS:
			buildlen = BuildXmlRouteMACBind((struct mgtcgi_xml_mac_binds *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"mac_binds" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_DHCPD_SERVERS:
			buildlen = BuildXmlRouteDhcpd((struct mgtcgi_xml_dhcpd_servers *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"dhcpd_servers" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_DHCPD_HOSTS:
			buildlen = BuildXmlRouteDhcpdHost((struct mgtcgi_xml_dhcpd_hosts *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"dhcpd_host" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_DDNS:
			buildlen = BuildXmlRouteDDNS((struct mgtcgi_xml_ddns *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"ddns" , URL_LENGTH-1);
			break;	
		case MGTCGI_TYPE_PPTPD_SERVERS:
			buildlen = BuildXmlRoutePptpdServer((struct mgtcgi_xml_pptpd_server *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"pptpd_server" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_PPTPD_ACCOUNT:
			buildlen = BuildXmlRoutePptpdAccount((struct mgtcgi_xml_pptpd_accounts_info *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"pptpd_accounts" , URL_LENGTH-1);
		//	filelog.RawLog(strbuf,STRING_BUFF_LEN);
			break;
		case MGTCGI_TYPE_WAN_CHANNEL:
			buildlen = BuildXmlWanChannel((struct mgtcgi_xml_wans_channels *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"wan_channels" , URL_LENGTH-1);
			break;
		case MGTCGI_TYPE_ROUTING_TABLES:
			buildlen = BuildXmlroutingtables((struct mgtcgi_xml_route_tables *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"route_tables" , URL_LENGTH-1);
			break;
		default:
			buildlen = 0;
			break;
	}

	//printf("\nbuildsize:%d\n%s\n", buildlen, strbuf);
#if 1
	if (buildlen > 0){
		maxnet::RWXml rwxml(&pLog , filename);
		
		fd=open(path,O_WRONLY|O_CREAT);
		if(fd < 0){
			free(strbuf);
			return XML_ERROR;
		}
		
    	if(flock(fd,LOCK_EX)!=0){
			free(strbuf);
			close(fd);
			return XML_ERROR;
    	}
		
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
#endif
	free(strbuf);
    strbuf = NULL;
	return XML_ERROR;
}



/*
*remark : 根据struct结构数据构建System结点的字符串
*/
static int BuildXmlRouteDNS(const struct mgtcgi_xml_dns *const buf,char *strbuf, const int strbuf_len)
{
	const char *format_str = 
		"<dns>\n" \
		"	<dns_primary>%s</dns_primary>\n" \
		"	<dns_secondary>%s</dns_secondary>\n" \
		"	<dns_thirdary>%s</dns_thirdary>\n" \
		"</dns>\n";

	if (NULL == buf)
		return XML_ERROR; 
		
	snprintf(strbuf, strbuf_len, format_str,
		buf->dns_primary,
		buf->dns_secondary,
		buf->dns_thirdary);
	
	return strlen(strbuf);
}
static int BuildXmlRouteDDNS(const struct mgtcgi_xml_ddns *const buf,char *strbuf, const int strbuf_len)
{
	const char *format_str = 
		"<ddns>\n" \
		"	<enable_ddns>%u</enable_ddns>\n" \
		"	<provider>%s</provider>\n" \
		"	<username>%s</username>\n" \
		"	<password>%s</password>\n" \
		"	<dns>%s</dns>\n" \
		"</ddns>\n";

	if (NULL == buf)
		return XML_ERROR; 
		
	snprintf(strbuf, strbuf_len, format_str,
		buf->enable_ddns,
		buf->provider,
		buf->username,
		buf->password,
		buf->dns);
	
	return strlen(strbuf);
}

static int BuildXmlInterfaceLink(const struct mgtcgi_xml_interface_links *const buf, char *strbuf, const int strbuf_len)
{
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_link>\n"  \
		"%s"  \
		"</interface_link>\n" ;
	const char* format_str =	
		"	<link ifname=\"%s\" type=\"%s\" mode=\"%s\" speed=\"%u\" duplex=\"%s\" linkmode=\"%u\" />\n";
	int bufindex = 0;
	int index = 0;
	
	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].ifname,
			buf->pinfo[index].type,
			buf->pinfo[index].mode,
			buf->pinfo[index].speed,
			buf->pinfo[index].duplex,
			buf->pinfo[index].linkmode);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	return strlen(strbuf);
}

static int BuildXmlRouteLAN(const struct mgtcgi_xml_route_interface_lans *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_lans>\n"  \
		"    <ip>%s</ip>\n"  \
		"    <mask>%s</mask>\n"  \
		"    <mac>%s</mac>\n"  \
		"    <ifname>%s</ifname>"  \
		"    <dhcp>%s</dhcp>\n"  \
		"    <startip>%s</startip>\n"  \
		"    <endip>%s</endip>\n"  \
		"    <lease_time>%u</lease_time>\n"  \
		"    <gateway>%s</gateway>\n"  \
		"    <dhcp_mask>%s</dhcp_mask>\n"  \
		"    <first_dns>%s</first_dns>\n"  \
		"    <second_dns>%s</second_dns>\n"  \
		"    <subnet_enable>%u</subnet_enable>\n"  \
		"    <subnets>\n"  \
		"        %s\n"  \
		"    </subnets>\n"  \
		"</interface_lans>\n" ;
	const char* format_str =	
		"	    <subnet ip=\"%s\" mask=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->subnet_num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].ip,			
			buf->pinfo[index].mask);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str,
			buf->ip,
			buf->mask,
			buf->mac,
			buf->ifname,
			buf->dhcp_type,
			buf->dhcp_info.startip,
			buf->dhcp_info.endip,
			buf->dhcp_info.lease_time,
			buf->dhcp_info.gateway,
			buf->dhcp_info.dhcp_mask,
			buf->dhcp_info.first_dns,
			buf->dhcp_info.second_dns,
			buf->sunbet_enable,strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteWAN(const struct mgtcgi_xml_interface_wans *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_wans>\n"  \
		"%s"  \
		"</interface_wans>\n" ;
	const char* format_str =	
		"	<wan id=\"%u\" enable=\"%u\" ifname=\"%s\" access=\"%s\" type=\"%s\" ip=\"%s\" mask=\"%s\" gateway=\"%s\" username=\"%s\" passwd=\"%s\" servername=\"%s\" mtu=\"%s\" mac=\"%s\" dns=\"%s\" up=\"%s\" down=\"%s\" isp=\"%s\" default=\"%u\" time=\"%s\" week=\"%s\" day=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;
	
	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].enable,
			buf->pinfo[index].ifname,
			buf->pinfo[index].access,
			buf->pinfo[index].type,
			buf->pinfo[index].ip,
			buf->pinfo[index].mask,
			buf->pinfo[index].gateway,
			buf->pinfo[index].username,
			buf->pinfo[index].passwd,
			buf->pinfo[index].servername,
			buf->pinfo[index].mtu,
			buf->pinfo[index].mac,
			buf->pinfo[index].dns,
			buf->pinfo[index].up,
			buf->pinfo[index].down,
			buf->pinfo[index].isp,
			buf->pinfo[index].defaultroute,
			buf->pinfo[index].time,
			buf->pinfo[index].week,
			buf->pinfo[index].day);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	return strlen(strbuf);
}

static int BuildXmlRouteVLAN(const struct mgtcgi_xml_interface_vlans *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_vlans>\n"  \
		"%s"  \
		"</interface_vlans>\n" ;
	const char* format_str =	
		"	<vlan name=\"%s\" ifname=\"%s\" id=\"%u\" mac=\"%s\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].id,
			buf->pinfo[index].mac,
			buf->pinfo[index].comment);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteMacVLAN(const struct mgtcgi_xml_interface_macvlans *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_macvlans>\n"  \
		"%s"  \
		"</interface_macvlans>\n" ;
	const char* format_str =	
		"	<vlan name=\"%s\" ifname=\"%s\" mac=\"%s\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].mac,
			buf->pinfo[index].comment);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlroutingtables (const struct mgtcgi_xml_route_tables *const buf,char *strbuf, const int strbuf_len)
{
	std::string strtemp="";
	const char* main_format_str =		
		"<route_tables>\n"  \
		"%s"  \
		"</route_tables>\n" ;
	const char* format_str =	
		"	<table id=\"%u\" enable=\"%u\" value=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].enable,
			buf->pinfo[index].value);
		
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRoutePPPDAuth(const struct mgtcgi_xml_pppd_authenticate *const buf,char *strbuf, const int strbuf_len)
{
	const char *format_str = 
		"<pppd_authenticate>\n"
        "	<pap>%d</pap>\n"
        "	<chap>%d</chap>\n"
        "	<mschap>%d</mschap>\n"
        "	<mschapv2>%d</mschapv2>\n"
        "	<eap>%d</eap>\n"
    	"</pppd_authenticate>\n";

	if (NULL == buf)
		return XML_ERROR; 
		
	snprintf(strbuf, strbuf_len, format_str,
		buf->pap,
		buf->chap,
		buf->mschap,
		buf->mschapv2,
		buf->eap);
	
	return strlen(strbuf);
}

static int BuildXmlRouteADSLClient(const struct mgtcgi_xml_adsl_clients *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<adsl_clients>\n"
		"%s"
		"</adsl_clients>\n";

	const char* format_str =	
		"	<adslclient name=\"%s\" ifname=\"%s\" username=\"%s\" password=\"%s\" servername=\"%s\" mtu=\"%d\" weight=\"%d\" defaultroute=\"%d\" peerdns=\"%d\" enable=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].username,
			buf->pinfo[index].password,
			buf->pinfo[index].servername,
			buf->pinfo[index].mtu,
			buf->pinfo[index].weight,
			buf->pinfo[index].defaultroute,
			buf->pinfo[index].peerdns,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteVPNClient(const struct mgtcgi_xml_l2tpvpn_clients *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<l2tpvpn_clients>\n"
		"%s"
		"</l2tpvpn_clients>\n";

	const char* format_str =	
		"	<l2tpvpn name=\"%s\" ifname=\"%s\" username=\"%s\" password=\"%s\" servername=\"%s\" mtu=\"%d\" weight=\"%d\" defaultroute=\"%d\" peerdns=\"%d\" enable=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].username,
			buf->pinfo[index].password,
			buf->pinfo[index].servername,
			buf->pinfo[index].mtu,
			buf->pinfo[index].weight,
			buf->pinfo[index].defaultroute,
			buf->pinfo[index].peerdns,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}



static int BuildXmlRouteLoadBalancing(const struct mgtcgi_xml_load_balancings *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =			
		"<load_balancings>\n"
		"%s"
		"</load_balancings>";

	const char* format_str = "	<balancing name=\"%s\" ifname=\"%s\" rulename=\"%s\" weight=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].rulename,
			buf->pinfo[index].weight,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}



static int BuildXmlRouteRouting(const struct mgtcgi_xml_routings *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =			
		"<routings>\n"
		"%s"
		"</routings>";

	const char* format_str =	
		"	<routing id=\"%d\" dst=\"%s\" gateway=\"%s\" ifname=\"%s\" table=\"%s\" rule=\"%s\" enable=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].dst,
			buf->pinfo[index].gateway,
			buf->pinfo[index].ifname,
			buf->pinfo[index].table,
			buf->pinfo[index].rule,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteRule(const struct mgtcgi_xml_route_rules *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =			
		"<route_rules>\n"
		"%s"
    	"</route_rules>";

	const char* format_str =	
		"	<rule name=\"%s\" src=\"%s\" dst=\"%s\" protocol=\"%s\" src_port=\"%s\" dst_port=\"%s\" tos=\"%s\" action=\"%s\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].src,
			buf->pinfo[index].dst,
			buf->pinfo[index].protocol,
			buf->pinfo[index].src_port,
			buf->pinfo[index].dst_port,
			buf->pinfo[index].tos,
			buf->pinfo[index].action,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteDNAT(const struct mgtcgi_xml_nat_dnats *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =	
	"<nat_dnats>\n"
	"%s"
	"</nat_dnats>\n";

	const char* format_str =	
		"	<dnat id=\"%u\" enable=\"%u\" protocol_id=\"%s\" comment=\"%s\" wan_ip=\"%s\" wan_port_before=\"%s\" wan_port_after=\"%s\" lan_port_before=\"%s\" lan_port_after=\"%s\" lan_ip=\"%s\" ifname=\"%s\" loop=\"%u\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].enable,
			buf->pinfo[index].protocol,
			buf->pinfo[index].comment,
			buf->pinfo[index].wan_ip,
			buf->pinfo[index].wan_port_before,
			buf->pinfo[index].wan_port_after,
			buf->pinfo[index].lan_port_before,
			buf->pinfo[index].lan_port_after,
			buf->pinfo[index].lan_ip,
			buf->pinfo[index].ifname,
			buf->pinfo[index].loop_enable);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteSNAT(const struct mgtcgi_xml_nat_snats *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =	
		"<nat_snats>\n"
		"%s"
		"</nat_snats>";
	
	const char* format_str =	
		"	<snat id=\"%d\" ifname=\"%s\" src=\"%s\" dst=\"%s\" protocol=\"%s\" action=\"%s\" dstip=\"%s\" dstport=\"%s\" enable=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].ifname,
			buf->pinfo[index].src,
			buf->pinfo[index].dst,
			buf->pinfo[index].protocol,
			buf->pinfo[index].action,
			buf->pinfo[index].dstip,
			buf->pinfo[index].dstport,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}


static int BuildXmlRouteMACBind(const struct mgtcgi_xml_mac_binds *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =	
		"<mac_binds>\n"
		"<learnt>%d</learnt>\n"
		"%s"
		"</mac_binds>";
	
	const char* format_str =	
		"	<macbind id=\"%d\" ip=\"%s\" mac=\"%s\" action=\"%d\" bridge=\"%d\" comment=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].id,
			buf->pinfo[index].ip,
			buf->pinfo[index].mac,
			buf->pinfo[index].action,
			buf->pinfo[index].bridge,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, buf->learnt, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRoutePptpdServer(const struct mgtcgi_xml_pptpd_server *const buf, char *strbuf, const int strbuf_len)
{
	int bufindex = 0;
	
	const char *format_str = 
		"  <pptpd_server>\n"
		"    <enable>%d</enable>\n"
		"    <localip>%s</localip>\n"
		"    <startip>%s</startip>\n"
		"    <endip>%s</endip>\n"
		"    <dns>%s</dns>\n"
		"    <entry>%s</entry>\n"
		"  </pptpd_server>";

	if(NULL == buf){
		return XML_ERROR;
	}

	bufindex = snprintf(strbuf,strbuf_len,format_str,
			buf->enable,
			buf->localip,
			buf->startip,
			buf->endip,
			buf->dns,
			buf->entry_mode);

	return strlen(strbuf);
}

static int BuildXmlRoutePptpdAccount(const struct mgtcgi_xml_pptpd_accounts_info *const buf, char *strbuf, const int strbuf_len)
{
	int bufindex = 0, index = 0;
	std::string tmpstr = "";
	
	const char *format_str = 
		"  <pptpd_accounts>\n"
		"    %s\n"
		"  </pptpd_accounts>";
	const char *main_info_str = 
		"    <account username=\"%s\" passwd=\"%s\" ip=\"%s\" />";

	if(NULL == buf){
		return XML_ERROR;
	}

	for(index = 0; index < buf->num; index++){
		bufindex += snprintf(strbuf+bufindex,strbuf_len,main_info_str,
			buf->pinfo[index].username,
			buf->pinfo[index].passwd,
			buf->pinfo[index].ip);
	}

	tmpstr = strbuf;
	strbuf[0]='\0';
	snprintf(strbuf,strbuf_len,format_str,tmpstr.c_str());

	return strlen(strbuf);
}

static int BuildXmlRouteDhcpd(const struct mgtcgi_xml_dhcpd_servers *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =	
		"<dhcpd_servers>\n"
		"<dhcpd_enable>%d</dhcpd_enable>\n"
		"%s"
		"</dhcpd_servers>";
	
	const char* format_str =	
		"	<dhcpd name=\"%s\" ifname=\"%s\" range=\"%s\" mask=\"%s\" gateway=\"%s\" dnsname=\"%s\" dns=\"%s\" lease_time=\"%d\" max_lease_time=\"%d\" enable=\"%d\" comment=\"%s\" />";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].ifname,
			buf->pinfo[index].range,
			buf->pinfo[index].mask,
			buf->pinfo[index].gateway,
			buf->pinfo[index].dnsname,
			buf->pinfo[index].dns,
			buf->pinfo[index].lease_time,
			buf->pinfo[index].max_lease_time,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, buf->dhcpd_enable, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlRouteDhcpdHost(const struct mgtcgi_xml_dhcpd_hosts *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =	
		"<dhcpd_host>\n"
		"%s"
		"</dhcpd_host>";
	
	const char* format_str =	
		"	<host name=\"%s\" dhcpname=\"%s\" mac=\"%s\" ip=\"%s\" mask=\"%s\" gateway=\"%s\" dnsname=\"%s\" dns=\"%s\" lease_time=\"%d\" max_lease_time=\"%d\" enable=\"%d\" comment=\"%s\" />";
	int bufindex = 0;
	int index = 0;

	if (NULL == buf)
		return XML_ERROR;

	for(index=0; index < buf->num; index++){
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
			buf->pinfo[index].name,
			buf->pinfo[index].dhcpname,
			buf->pinfo[index].mac,
			buf->pinfo[index].ip,
			buf->pinfo[index].mask,
			buf->pinfo[index].gateway,
			buf->pinfo[index].dnsname,
			buf->pinfo[index].dns,
			buf->pinfo[index].lease_time,
			buf->pinfo[index].max_lease_time,
			buf->pinfo[index].enable,
			buf->pinfo[index].comment);
			
		if(bufindex >= strbuf_len) break;
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}


static int BuildXmlRouteTAbleGruop(const struct mgtcgi_xml_group_array * const buf, char *strbuf, const int strbuf_len)
{	
	const char *const head_str = "<route_tables>\n";
	const char *const foot_str = "</route_tables>";
	char * ptr = strbuf ;
	int tmplen = strbuf_len;
	if (NULL == buf)
		return XML_ERROR; 
	
	strcpy(ptr , head_str);
	ptr += strlen(head_str);
	tmplen -= strlen(head_str); 
	tmplen -= strlen(foot_str);
	
	if((tmplen = BuildXmlRouteGroups(buf , ptr , tmplen)) <= 0)
		return XML_ERROR;
	
	ptr += tmplen;	
	strcpy(ptr , foot_str);
	return strlen(strbuf);
}


static int BuildXmlRouteGroups(const struct mgtcgi_xml_group_array * const group, char * buf, const int buf_len)
{	
	int index_1 = 0;
	int index_2 = 0;
	char *ptr = buf;

	if (NULL == group)
		return 0;

	for(index_1 = 0; index_1 < group->num; index_1++){
		strcpy(ptr, "<group name=\"");
		ptr += 13;
		strcpy(ptr, group->pinfo[index_1].group_name);
		ptr += strlen(group->pinfo[index_1].group_name);

		strcpy(ptr, "\" comment=\"");
		ptr += 11;
		strcpy(ptr, group->pinfo[index_1].comment);
		ptr += strlen(group->pinfo[index_1].comment);
		
		strcpy(ptr, "\">\n");
		ptr  += 3; 
		
		for(index_2 = 0; index_2 < group->pinfo[index_1].num; index_2++){
			strcpy(ptr , "<include name=\"");
            ptr+=15;
			strcpy(ptr , group->pinfo[index_1].pinfo[index_2].name);
			ptr+=strlen(group->pinfo[index_1].pinfo[index_2].name);
			strcpy(ptr , "\" />\n" );
			ptr+=5;
			if((ptr-buf + 10) > buf_len ) 
				break;
		}
		//DEBUGPR(" include =:  %s \n" , buf);
		strcpy(ptr , "</group>\n");
		ptr += 9;
	}

	//snprintf(buf ,buf_len , "%s" , ptr.c_str());
	//printf("%s\n",buf);
	//DEBUGPR("%s\n" , buf);
	return strlen(buf);
}

static int BuildXmlWanChannel(const struct mgtcgi_xml_wans_channels *const wans_channel, char * buf, const int buf_len)
{
	U32	index = 0;
	int bufindex = 0;
	int old_id=0,old_enable=0,new_id=0,new_enable=0;
	std::string strtemp = "";
	std::string data = "";
	const char *main_format = 
		"<wan_channels>\n"
		"  %s"
		"</wan_channels>\n";
	const char *wans_format = 
		"<wans id=\"%d\" enable=\"%d\">\n"
		"    %s"
		"</wans>\n";
		
	const char *channel_format = 
		"      <channel classname=\"%s\" classtype=\"%u\" classid=\"%u\" dir=\"%u\" parentid=\"%u\" shareid=\"%u\" rate=\"%s\" attribute=\"%u\" prio=\"%u\" limit=\"%u\" weight=\"%u\" comment=\"%s\" />\n";

	for(index = 0; index < wans_channel->num; index++){
		new_id = wans_channel->pinfo[index].id;
		new_enable = wans_channel->pinfo[index].enable;
		
		if(new_id != old_id){
			if(old_id != 0){
				strtemp = buf;
				buf[0] = '\0';
				snprintf(buf,buf_len,wans_format,old_id,old_enable,strtemp.c_str());
				strtemp = "";
				strtemp = buf;
				data += strtemp;

				buf[0] = '\0';
				strtemp = "";
				bufindex = 0;
			}
		}

		bufindex += snprintf(buf+bufindex,buf_len,channel_format,
			wans_channel->pinfo[index].classname,
			wans_channel->pinfo[index].classtype,
			wans_channel->pinfo[index].classid,
			wans_channel->pinfo[index].direct,
			wans_channel->pinfo[index].parentid,
			wans_channel->pinfo[index].shareid,
			wans_channel->pinfo[index].rate,
			wans_channel->pinfo[index].attribute,
			wans_channel->pinfo[index].priority,
			wans_channel->pinfo[index].limit,
			wans_channel->pinfo[index].weight,
			wans_channel->pinfo[index].comment);

		old_id = new_id;
		old_enable = new_enable;
	}

	if(index > 0){
		if(index == wans_channel->num){	
			strtemp = "";
			strtemp = buf;
			snprintf(buf,buf_len,wans_format,old_id,old_enable,strtemp.c_str());
			strtemp = "";
			strtemp = buf;
			data = data + strtemp;
		}
	}

	buf[0] = '\0';

	snprintf(buf,buf_len,main_format,data.c_str());

	return strlen(buf);
}


/* 
remark: 删除之前分配的内存
return: 删除成功返回0
*/
int free_route_xml_node(void ** buf)
{
	if(*buf != NULL){
		free(*buf);
		*buf = NULL;
	}	
	return 0;
}





/* 
remark: 调用命令行处理函数，处理命令行输入
return: 成功返回0
*/
int route_xml_do_command(int argc, char *argv[])
{
	RouteConfiguration* config = NULL;

	RouteConfiguration real_config(&argc, &argv);
    config = &real_config;
	int ret;


	Log * pLog = new Log("/var/log/"ROUTE_PACKAGE_LONG_NAME".log", config->syslog, false, config->console);

	RouteParser * xml_parser = new RouteParser(pLog,config->main_config_filename,config->key,
							0, 0, config->print_ddns,config->print_lan,  config->print_wan,
							0, 0,
							config->print_loadb, config->print_routing, 0,
							config->print_route_rule, config->print_dnat, config->print_snat, config->print_macbind,
							config->print_adsl_client_passwd,config->print_routing_command);

    ret = xml_parser->process();
    delete pLog;
	delete xml_parser;
	
	return ret;

}



