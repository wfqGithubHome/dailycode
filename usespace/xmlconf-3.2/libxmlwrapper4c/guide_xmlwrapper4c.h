#ifndef _XML_GUIDE_PARSE_FOR_C_H
#define _XML_GUIDE_PARSE_FOR_C_H

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
#include "../config/guide_config.h"
#include "../xml/reader.h"
#include "../xml/writer.h"
#include "../xml/guide_parser.h"
#include "../xml/rwxml.h"
#include <list>
using namespace maxnet;
#endif //__cplusplus
#include "guide_xmlconfig.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int get_guide_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size);
int save_guide_xml_node(const char * filename,const int nodetype, const void * buf, const int  buflen);
int free_guide_xml_node(void ** buf);
int guide_xml_do_command(int argc, char * argv [ ]);

#ifdef __cplusplus
}
#endif 

#endif 










