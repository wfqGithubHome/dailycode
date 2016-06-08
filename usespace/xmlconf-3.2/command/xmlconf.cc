#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "config/config.h"
#include "xml/reader.h"
#include "xml/writer.h"
#include "xml/parser.h"
#include "xml/rwxml.h"
#include "global.h"

using namespace maxnet;

const Configuration* config;

int main(int argc, char *argv[])
{
    Configuration real_config(&argc, &argv);
    config = &real_config;
	int ret=0;

    Log * pLog = new Log(PACKAGE_LONG_NAME, config->syslog, false, config->console);
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
                                     "",
                                     true , //添加默认数据，如any ip地址
                                     config->check,
                                     config->print_fw,
                                     config->print_action,
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
