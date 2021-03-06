#ifndef _XML_ROUTE_PARSE_FOR_C_H
#define _XML_ROUTE_PARSE_FOR_C_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <time.h>
#ifdef __cplusplus
#include "../base/logwraper.h"
#include "../core/traffic_util.h"
#include "../core/logger.h"
#include "../tinyxml/tinyxml.h"
#include "../config/route_config.h"
#include "../xml/reader.h"#include "../xml/writer.h"
#include "../xml/route_parser.h"
#include "../xml/rwxml.h"
#include "../global.h"
#include <list>
using namespace maxnet;
#endif //__cplusplus
#include "mgtcgi_xmlconfig.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int get_route_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size);
int save_route_xml_node(const char * filename,const int nodetype, const void * buf, const int  buflen);
int free_route_xml_node(void ** buf);
int route_xml_do_command(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_XML_ROUTE_PARSE_FOR_C_H










