#include <sys/types.h>
#include "guide_xmlconfig.h"
#include "guide_xmlwrapper4c.h"
#include <stdlib.h>
#include <stdio.h>

#if 0
#define DEBUGPR(...) fprintf(stdout,__VA_ARGS__)
#else
#define DEBUGPR(...) 
#endif

#define MAX_FILENAME_LEN 256
#define XML_ERROR -1

static int BuildXmlGuideLan(const struct guide_if_lan *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideServer(const struct guide_server *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideWan(const struct guide_if_wan *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideAdsl(const struct guide_adsl *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideAppbind(const struct guide_appbind *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideTc(const struct guide_tc *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideZone(const struct guide_sel_zone *const buf, char *strbuf, const int strbuf_len);
static int BuildXmlGuideDnat(const struct guide_dnat *const buf, char *strbuf, const int strbuf_len);

int get_guide_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size)
{	
	maxnet::Log pLog("guide-xmlconf_lib", false, false, false);
	maxnet::GuideParser xml_parser(&pLog,configfile, 0); 
	xml_parser.loadConfig();

	switch(nodetype)
    {   
		case GUIDE_TYPE_LAN:
			*out_size = xml_parser.GetStructGuideLan((struct guide_if_lan**)buf);
			break;
        case GUIDE_TYPE_SERVER:            
			*out_size = xml_parser.GetStructGuideServer((struct guide_server**)buf);
			break;
        case GUIDE_TYPE_WAN:
			*out_size = xml_parser.GetStructGuideWan((struct guide_if_wan**)buf);
			break;
        case GUIDE_TYPE_ADSL:            
			*out_size = xml_parser.GetStructGuideAdsl((struct guide_adsl**)buf);
            break;
        case GUIDE_TYPE_APPBIND:
			*out_size = xml_parser.GetStructGuideAppbind((struct guide_appbind**)buf);
			break;
        case GUIDE_TYPE_TC:            
			*out_size = xml_parser.GetStructGuideTc((struct guide_tc**)buf);
            break;
        case GUIDE_TYPE_ZONE:
			*out_size = xml_parser.GetStructGuideZone((struct guide_sel_zone**)buf);
			break;
        case GUIDE_TYPE_DNAT:            
			*out_size = xml_parser.GetStructGuideDnat((struct guide_dnat**)buf);
            break;
		default:
			*out_size = 0;
			*buf = NULL;
	}

	return *out_size;
}
 
int save_guide_xml_node(const char *filename, const int nodetype, const void * buf, const int buflen)
{
	#define STRING_BUFF_LEN 1024000
	maxnet::Log pLog("/var/log/guide-xmlconf_lib.log", false, false, false);	
	maxnet::RWXml rwxml(&pLog, filename);
	char *strbuf = NULL;
	char xmlnodename[URL_LENGTH] = {0};
	int buildlen = 0;
	
	if (buf == NULL)
	{
		return XML_ERROR;
	}

	strbuf = (char *)malloc(sizeof(char) * STRING_BUFF_LEN);
	if(!strbuf){
		return XML_ERROR;
	}
	
	memset(strbuf, 0, sizeof(char) * STRING_BUFF_LEN);	
	switch(nodetype)
    {           
		case GUIDE_TYPE_LAN:
			buildlen = BuildXmlGuideLan((struct guide_if_lan *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_lans" , URL_LENGTH-1);
			break;
		case GUIDE_TYPE_SERVER:
			buildlen = BuildXmlGuideServer((struct guide_server *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"servers" , URL_LENGTH-1);
			break;
        case GUIDE_TYPE_WAN:
			buildlen = BuildXmlGuideWan((struct guide_if_wan *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"interface_wans" , URL_LENGTH-1);
			break;
		case GUIDE_TYPE_ADSL:
			buildlen = BuildXmlGuideAdsl((struct guide_adsl *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"adsl_clients" , URL_LENGTH-1);
			break;
        case GUIDE_TYPE_APPBIND:
			buildlen = BuildXmlGuideAppbind((struct guide_appbind *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"appbinds" , URL_LENGTH-1);
			break;
		case GUIDE_TYPE_TC:
			buildlen = BuildXmlGuideTc((struct guide_tc *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"virtual_lines" , URL_LENGTH-1);
			break;
        case GUIDE_TYPE_ZONE:
			buildlen = BuildXmlGuideZone((struct guide_sel_zone *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"zones" , URL_LENGTH-1);
			break;
            
		case GUIDE_TYPE_DNAT:
			buildlen = BuildXmlGuideDnat((struct guide_dnat *)buf, strbuf, STRING_BUFF_LEN);
			strncpy(xmlnodename ,"dnat" , URL_LENGTH-1);
			break;
		default:
			buildlen = 0;
			break;
	}

	if (buildlen > 0)
    {
		if(rwxml.replaceNode(xmlnodename , strbuf) == 0)
        {
			free(strbuf);
			strbuf = NULL;
			return buildlen;	
		}
	}


	free(strbuf);
	strbuf = NULL;
	return XML_ERROR;
}

static int BuildXmlGuideLan(const struct guide_if_lan *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_lans>\n"  \
		"%s"  \
		"</interface_lans>" ;
	const char* format_str =	
		"	<lan ifname=\"%s\" ip=\"%s\" mask=\"%s\" mac=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;	
    
	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].ifname,
            buf->pinfo[index].ipaddr,
            buf->pinfo[index].netmask,
            buf->pinfo[index].mac);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideServer(const struct guide_server *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<servers>\n"  \
		"%s"  \
		"</servers>" ;
	const char* format_str =	
		"	<server parentid=\"%d\" value=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].parentid,
            buf->pinfo[index].value);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideWan(const struct guide_if_wan *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<interface_wans>\n"  \
		"%s"  \
		"</interface_wans>" ;
	const char* format_str =	
		"	<wan ifname=\"%s\" isp=\"%d\" quality=\"%d\" lineid=\"%d\" ip=\"%s\" mask=\"%s\" gateway=\"%s\" " \
		"mac=\"%s\" upload=\"%d\" download=\"%d\" conntype=\"%d\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].ifname,
            buf->pinfo[index].isp,
            buf->pinfo[index].quality,
            buf->pinfo[index].line_id,
            buf->pinfo[index].ipaddr,
            buf->pinfo[index].netmask,
            buf->pinfo[index].gateway,
            buf->pinfo[index].mac,
            buf->pinfo[index].upload,
            buf->pinfo[index].download,
            buf->pinfo[index].conntype);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideAdsl(const struct guide_adsl *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<adsl_clients>\n"  \
		"%s"  \
		"</adsl_clients>" ;
	const char* format_str =	
		"	<adslclient ifname=\"%s\" isp=\"%d\" quality=\"%d\" lineid=\"%d\" username=\"%s\" password=\"%s\" " \
		"upload=\"%d\" download=\"%d\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].ifname,
            buf->pinfo[index].isp,
            buf->pinfo[index].quality,
            buf->pinfo[index].line_id,
            buf->pinfo[index].username,
            buf->pinfo[index].password,
            buf->pinfo[index].upload,
            buf->pinfo[index].download);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideAppbind(const struct guide_appbind *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<appbinds>\n"  \
		"%s"  \
		"</appbinds>" ;
	const char* format_str =	
		"	<appbind name=\"%s\" apptype=\"%d\" bindtype=\"%d\" bindline=\"%d\" bypass=\"%d\" maxupload=\"%d\" " \
		"maxdownload=\"%d\" weight=\"%d\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].name,
            buf->pinfo[index].app_type,
            buf->pinfo[index].bind_type,
            buf->pinfo[index].bind_line_id,
            buf->pinfo[index].bypass,
            buf->pinfo[index].maxupload,
            buf->pinfo[index].maxdownload,
            buf->pinfo[index].weight);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideTc(const struct guide_tc *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<virtual_lines>\n"  \
		"%s"  \
		"</virtual_lines>" ;
	const char* format_str =	
		"	<virtual_line name=\"%s\" lineid=\"%d\" isp=\"%d\" quality=\"%d\" balance=\"%d\" upload=\"%d\" " \
		"download=\"%d\" enable=\"%d\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].name,
            buf->pinfo[index].line_id,
            buf->pinfo[index].isp,
            buf->pinfo[index].quality,
            buf->pinfo[index].balance,
            buf->pinfo[index].upload,
            buf->pinfo[index].download,
            buf->pinfo[index].enable);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideZone(const struct guide_sel_zone *const buf, char *strbuf, const int strbuf_len)
{	
	std::string strtemp="";
	const char* main_format_str =		
		"<zones>\n"  \
		"%s"  \
		"</zones>" ;
	const char* format_str =	
		"	<zone type=\"%d\" oem=\"%s\" isp=\"%s\" />\n";
	int bufindex = 0;
	int index = 0;	

	if (buf == NULL)
	{
		return XML_ERROR;
	}

	for(index=0; index < buf->num; index++)
    {
		bufindex += snprintf(strbuf + bufindex, strbuf_len, format_str,
            buf->pinfo[index].type,
            buf->pinfo[index].oem,
            buf->pinfo[index].isp);
		
		if(bufindex >= strbuf_len) 
		{
            break;
		}
	}
	
	strtemp = strbuf;
	strbuf[0] = '\0';	
	snprintf(strbuf, strbuf_len, main_format_str, strtemp.c_str());
	
	return strlen(strbuf);
}

static int BuildXmlGuideDnat(const struct guide_dnat *const buf, char *strbuf, const int strbuf_len)
{	
	const char* format_str =		
		"<dnat>\n"  \
		"<dnat_enable> %d </dnat_enable>"  \
		"</dnat>" ;

	snprintf(strbuf, strbuf_len, format_str, buf->enable);
	
	return strlen(strbuf);
}

int free_guide_xml_node(void ** buf)
{
	if(*buf != NULL)
    {
		free(*buf);
		*buf = NULL;
	}	
	return 0;
}

int guide_xml_do_command(int argc, char *argv[])
{
	GuideConfiguration* config = NULL;  
	GuideConfiguration real_config(&argc, &argv);
	int ret;
    config = &real_config;

	Log * pLog = new Log("/var/log/"GUIDE_PACKAGE_LONG_NAME".log", config->syslog, true, config->console);

	GuideParser * xml_parser = new GuideParser(pLog, config->main_config_filename, config->check);
    ret =  xml_parser->process();
	delete pLog;
	delete xml_parser;
	
	return ret;
}



