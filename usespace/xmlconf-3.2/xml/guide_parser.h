#ifndef GUIDE_ZERO_PARSER_H
#define GUIDE_ZERO_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <sstream>

#include "../base/logwraper.h"
#include "../core/node.h"
#include "../core/system.h"

#include "../core/logger.h"
#include "../tinyxml/tinyxml.h"
#include "../libxmlwrapper4c/mgtcgi_xmlconfig.h"

#include "../core/guide/guide_lanmgr.h"
#include "../core/guide/guide_servermgr.h"
#include "../core/guide/guide_wanmgr.h"
#include "../core/guide/guide_adslmgr.h"
#include "../core/guide/guide_appbindmgr.h"
#include "../core/guide/guide_tcmgr.h"
#include "../core/guide/guide_zonemgr.h"
#include "../core/guide/guide_dnat.h"

namespace maxnet
{

#define RET_ERROR 	-1

class GuideParser  : public LogWraper 
{
    public:
        GuideParser(Log *log, const std::string guide_config_filename,int check);
        ~GuideParser();
        
        int     process();
        int     loadConfig();
        void    unloadConfig();
        
        int     GetStructGuideLan(struct guide_if_lan ** p_xml);
        int     GetStructGuideServer(struct guide_server ** p_xml);
        int     GetStructGuideWan(struct guide_if_wan ** p_xml);
        int     GetStructGuideAdsl(struct guide_adsl ** p_xml);
        int     GetStructGuideAppbind(struct guide_appbind ** p_xml);
        int     GetStructGuideTc(struct guide_tc ** p_xml);
        int     GetStructGuideZone(struct guide_sel_zone ** p_xml);
        int     GetStructGuideDnat(struct guide_dnat ** p_xml);

        #define SPLIT_CHAR "/"
        void    split(std::string instr, std::vector<std::string> *lstoutstr , const char* splitchar=SPLIT_CHAR);
      
protected:

        std::string     guide_config_filename;  
        int             check;

        std::string     getNodeValue(TiXmlNode *node);
        std::string     getNodeValuebyName(const std::string nodename);
        
        int     processGuideLan();
        void    parseGuideLan(TiXmlNode * cur); 

        int     processGuideServer();
        void    parseGuideServer(TiXmlNode * cur); 

        int     processGuideWan();
        void    parseGuideWan(TiXmlNode * cur); 

        int     processGuideAdsl();
        void    parseGuideAdsl(TiXmlNode * cur); 

        int     processGuideAppbind();
        void    parseGuideAppbind(TiXmlNode * cur); 

        int     processGuideTc();
        void    parseGuideTc(TiXmlNode * cur); 

        int     processGuideZone();
        void    parseGuideZone(TiXmlNode * cur); 

        int     processGuideDnat();

private:

        Logger          logger;
        TiXmlDocument   doc;
        
        LanMgr          lanmgr;
        ServerMgr       servmgr;
        WanMgr          wanmgr;
        AdslMgr         adslmgr;
        AppbindMgr      appbindmgr;
        TCMgr           tcmgr;
        ZoneMgr         zonemgr;
        Dnat            dnat;
        
        TiXmlNode *findNode(TiXmlNode *parent, const std::string nodename);

};

}

#endif

