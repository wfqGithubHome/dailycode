#include <iostream>
#include <fstream>
#include "parser.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../libxmlwrapper4c/guide_xmlconfig.h"
#include "guide_parser.h"

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace maxnet
{
  	GuideParser::GuideParser(Log *log, const std::string guide_config_filename, int check)
	{
		setLog(log);	
		setLogHeader("[G] ");
		this->guide_config_filename = guide_config_filename;
		this->check = check;
	}

	GuideParser::~GuideParser()
    {
		
	}
	
	int GuideParser::loadConfig()
    {

		if(!doc.LoadFile(guide_config_filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr << "open xml guide config file error: " << guide_config_filename << std::endl;
			return 1;	
		}
		return 0;
	}

	void GuideParser::unloadConfig()
    {
		return;
	}

    void GuideParser::split(std::string instr, std::vector<std::string> *lstoutstr , const char* splitchar) 
    {

		std::string str = instr;
		unsigned int loc;
		/*	Break string into substrings and store beginning  */
		while(1)
        {
			loc = str.find(splitchar);
			if( loc == std::string::npos )
            {
				lstoutstr->push_back(str);
				return;
			}
			std::string substr = str.substr(0, loc);
			str.erase(0, loc + 1);
			lstoutstr->push_back(substr);
		}
	}

	std::string GuideParser::getNodeValue(TiXmlNode *node)
	{
		TiXmlNode *child;		
		std::string value = "";
		if(node == NULL)
		{
			return value;
		}
		if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			return value;
		}
		child = node;
	
		while(child && child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			child = child->FirstChild();
		}
		if(child == NULL || child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			return value;
		}
		value = child->ToText()->Value();
		return value;		
	}
	
	std::string GuideParser::getNodeValuebyName(const std::string nodename){
		TiXmlNode *node;	

		node = findNode(&doc, "xmlconfig/" + nodename);
		return getNodeValue(node);
	}

	TiXmlNode  *GuideParser::findNode(TiXmlNode *root, const std::string nodename)
	{
		TiXmlNode * parent = root;
		TiXmlNode * child = NULL;
		std::vector<std::string> token;
        
		split(nodename, &token);
		for(unsigned int i = 0; i < token.size(); i++)
		{
			for(child = parent->FirstChild(); child; child = child->NextSibling())
			{
				if(strcmp(child->Value(), token.at(i).c_str()) == 0)
				{
					parent = child;
					break;
				}
			}
		}
		
		token.clear();
		return child;
	}
    
	void GuideParser::parseGuideLan(TiXmlNode * cur)
    {
        std::string ifname = "";
    	std::string ipaddr = "";
    	std::string netmask = "";
        std::string mac="";
        maxnet::Log pLog("guide_lan_pasrse", false, false, false);

	    while(cur)
        {
    		if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

    			TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
    			while(attr)
                {
                    if (strcmp(attr->Name(), "ifname") == 0)
                    {
                        ifname = attr->Value() ? attr->Value() : "";
                    }
                    
    			    if(strcmp(attr->Name(), "ip") == 0)
                    {
    				    ipaddr = attr->Value() ? attr->Value() : "";
    				}
    				else if(strcmp(attr->Name(), "mask") == 0)
                    {
    					netmask = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "mac") == 0)
                    {
    					mac = attr->Value() ? attr->Value() : "";
          			}
    				attr=attr->Next();		
    			}
                
                lanmgr.add(ifname, mac, ipaddr, netmask);
                debug("ifname: %s, mac: %s, ipaddr: %s, netmask: %s\n", 
                    ifname.c_str(),
                    mac.c_str(),
                    ipaddr.c_str(),
                    netmask.c_str());

                ifname = "";
                mac = "";
                ipaddr = "";
                netmask = "";
                
                if(cur->NextSibling())
                {
                    parseGuideLan(cur->NextSibling());
    		    }
    		}
            
    		cur=cur->FirstChild();
    	}
    	return;
    }

    void GuideParser::parseGuideServer(TiXmlNode * cur)
    {
        std::string parentid = "";
        std::string value = "";

        while(cur){
            if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {
                TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                while(attr)
                {
                    if(strcmp(attr->Name(), "parentid") == 0)
                    {
                        parentid = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "value") == 0)
                    {
                        value = attr->Value() ? attr->Value() : "";
                    }
                    
                    attr=attr->Next();      
                }
                
                servmgr.add(parentid, value);
                debug("parentid: %s, value: %s\n", 
                    parentid.c_str(),
                    value.c_str());
                
                parentid = "";
                value = "";
                
                if(cur->NextSibling())
                {
                    parseGuideServer(cur->NextSibling());
                }
            }   
            cur=cur->FirstChild();
        }
        return;
    }

    void GuideParser::parseGuideWan(TiXmlNode * cur)
    {
        std::string ifname = "";
    	std::string line_id = "";
    	std::string isp = "";
        std::string quality="";
        std::string upload = "";
    	std::string download = "";
    	std::string ipaddr = "";
        std::string netmask="";
        std::string gateway = "";
    	std::string mac = "";
    	std::string conntype = "";
        maxnet::Log pLog("guide_wan_pasrse", false, false, false);

	    while(cur)
        {
    		if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

    			TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
    			while(attr)
                {
                    if (strcmp(attr->Name(), "ifname") == 0)
                    {
                        ifname = attr->Value() ? attr->Value() : "";
                    }
                    
    			    if(strcmp(attr->Name(), "lineid") == 0)
                    {
    				    line_id = attr->Value() ? attr->Value() : "";
    				}
    				else if(strcmp(attr->Name(), "isp") == 0)
                    {
    					isp = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "quality") == 0)
                    {
    					quality = attr->Value() ? attr->Value() : "";
          			}
                    if (strcmp(attr->Name(), "upload") == 0)
                    {
                        upload = attr->Value() ? attr->Value() : "";
                    }
                    
    			    if(strcmp(attr->Name(), "download") == 0)
                    {
    				    download = attr->Value() ? attr->Value() : "";
    				}
    				else if(strcmp(attr->Name(), "ip") == 0)
                    {
    					ipaddr = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "mask") == 0)
                    {
    					netmask = attr->Value() ? attr->Value() : "";
          			}
                    else if(strcmp(attr->Name(), "gateway") == 0)
                    {
    					gateway = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "mac") == 0)
                    {
    					mac = attr->Value() ? attr->Value() : "";
          			}
                    else if(strcmp(attr->Name(), "conntype") == 0)
                    {
    					conntype = attr->Value() ? attr->Value() : "";
          			}
                    
    				attr=attr->Next();		
    			}
                
                wanmgr.add(ifname, isp, quality, line_id, upload, download, mac, 
                    ipaddr, netmask, gateway, conntype);
                debug("ifname: %s, isp: %s, quality: %s, line_id: %s, upload: %s, download: %s, \
                    mac: %s, ipaddr: %s, netmask: %s, gateway: %s, conntype: %s\n",
                    ifname.c_str(),
                    isp.c_str(),
                    quality.c_str(),
                    line_id.c_str(),
                    upload.c_str(),
                    download.c_str(),
                    mac.c_str(),
                    ipaddr.c_str(),
                    netmask.c_str(),
                    gateway.c_str(),
                    conntype.c_str());
                
                ifname = "";
                isp = "";
                quality = "";
                line_id = "";
                upload = "";
                download = "";
                mac = "";
                ipaddr = "";
                netmask = "";
                gateway = "";
                conntype = "";
                        
                if(cur->NextSibling())
                {
                    parseGuideWan(cur->NextSibling());
    		    }
    		}
            
    		cur=cur->FirstChild();
    	}
    	return;
    }

    void GuideParser::parseGuideAdsl(TiXmlNode * cur)
    {
        std::string ifname = "";
    	std::string line_id = "";
    	std::string isp = "";
        std::string quality="";
        std::string upload = "";
    	std::string download = "";
    	std::string username = "";
        std::string password ="";
        maxnet::Log pLog("guide_adsl_pasrse", false, false, false);

	    while(cur)
        {
    		if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

    			TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
    			while(attr)
                {
                    if (strcmp(attr->Name(), "ifname") == 0)
                    {
                        ifname = attr->Value() ? attr->Value() : "";
                    }
                    
    			    if(strcmp(attr->Name(), "lineid") == 0)
                    {
    				    line_id = attr->Value() ? attr->Value() : "";
    				}
    				else if(strcmp(attr->Name(), "isp") == 0)
                    {
    					isp = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "quality") == 0)
                    {
    					quality = attr->Value() ? attr->Value() : "";
          			}
                    if (strcmp(attr->Name(), "upload") == 0)
                    {
                        upload = attr->Value() ? attr->Value() : "";
                    }
                    
    			    if(strcmp(attr->Name(), "download") == 0)
                    {
    				    download = attr->Value() ? attr->Value() : "";
    				}
    				else if(strcmp(attr->Name(), "username") == 0)
                    {
    					username = attr->Value() ? attr->Value() : "";
    				}
                    else if(strcmp(attr->Name(), "password") == 0)
                    {
    					password = attr->Value() ? attr->Value() : "";
          			}
                    
    				attr=attr->Next();		
    			}
                
                adslmgr.add(ifname, isp, quality, line_id, upload, download, 
                    username, password);
                debug("ifname: %s, isp: %s, quality: %s, lineid: %s, upload: %s, download: %s, \
                    username: %s, password: %s\n",
                    ifname.c_str(),
                    isp.c_str(),
                    quality.c_str(),
                    line_id.c_str(),
                    upload.c_str(),
                    download.c_str(),
                    username.c_str(),
                    download.c_str());
                
                ifname = "";
                isp = "";
                quality = "";
                line_id = "";
                upload = "";
                download = "";
                username = "";
                password = "";
                        
                if(cur->NextSibling())
                {
                    parseGuideAdsl(cur->NextSibling());
    		    }
    		}
            
    		cur=cur->FirstChild();
    	}
    	return;
    }
    
    void GuideParser::parseGuideAppbind(TiXmlNode * cur)
    {
        std::string name = "";
        std::string apptype = "";
        std::string bindtype = "";
        std::string bindline ="";
        std::string bypass = "";
        std::string maxupload = "";
        std::string maxdownload = "";
        std::string weight ="";
        maxnet::Log pLog("guide_appbind_pasrse", false, false, false);

        while(cur)
        {
            if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

                TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                while(attr)
                {
                    if (strcmp(attr->Name(), "name") == 0)
                    {
                        name = attr->Value() ? attr->Value() : "";
                    }
                    
                    if(strcmp(attr->Name(), "apptype") == 0)
                    {
                        apptype = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "bindtype") == 0)
                    {
                        bindtype = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "bindline") == 0)
                    {
                        bindline = attr->Value() ? attr->Value() : "";
                    }
                    if (strcmp(attr->Name(), "bypass") == 0)
                    {
                        bypass = attr->Value() ? attr->Value() : "";
                    }
                    
                    if(strcmp(attr->Name(), "maxupload") == 0)
                    {
                        maxupload = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "maxdownload") == 0)
                    {
                        maxdownload = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "weight") == 0)
                    {
                        weight = attr->Value() ? attr->Value() : "";
                    }
                    
                    attr=attr->Next();      
                }
                
                appbindmgr.add(name, apptype, bindtype, bindline, bypass, maxupload, 
                    maxdownload, weight);
                debug("name: %s, apptype: %s, bindtype: %s, bindline: %s, bypass: %s, maxupload: %s, \
                    maxdownload: %s, weight: %s\n",
                    name.c_str(),
                    apptype.c_str(),
                    bindtype.c_str(),
                    bindline.c_str(),
                    bypass.c_str(),
                    maxupload.c_str(),
                    maxdownload.c_str(),
                    weight.c_str());
                
                name = "";
                apptype = "";
                bindtype = "";
                bindline = "";
                bypass = "";
                maxupload = "";
                maxdownload = "";
                weight = "";
                        
                if(cur->NextSibling())
                {
                    parseGuideAppbind(cur->NextSibling());
                }
            }
            
            cur=cur->FirstChild();
        }
        return;
    }
    
    void GuideParser::parseGuideTc(TiXmlNode * cur)
    {
        std::string name = "";
        std::string line_id = "";
        std::string isp = "";
        std::string quality="";
        std::string balance = "";
        std::string upload = "";
        std::string download = "";
        std::string enable ="";
        maxnet::Log pLog("guide_tc_pasrse", false, false, false);

        while(cur)
        {
            if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

                TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                while(attr)
                {
                    if (strcmp(attr->Name(), "name") == 0)
                    {
                        name = attr->Value() ? attr->Value() : "";
                    }
                    
                    if(strcmp(attr->Name(), "lineid") == 0)
                    {
                        line_id = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "isp") == 0)
                    {
                        isp = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "quality") == 0)
                    {
                        quality = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "balance") == 0)
                    {
                        balance = attr->Value() ? attr->Value() : "";
                    }
                    if (strcmp(attr->Name(), "upload") == 0)
                    {
                        upload = attr->Value() ? attr->Value() : "";
                    }
                    
                    if(strcmp(attr->Name(), "download") == 0)
                    {
                        download = attr->Value() ? attr->Value() : "";
                    }
                    
                    else if(strcmp(attr->Name(), "enable") == 0)
                    {
                        enable = attr->Value() ? attr->Value() : "";
                    }
                    
                    attr=attr->Next();      
                }
                
                tcmgr.add(name, line_id, isp, quality, balance, upload, download, enable);
                debug("name: %s, line_id: %s, isp: %s, quality: %s, balance: %s, upload: %s, \
                    download: %s, enable: %s\n", 
                    name.c_str(),
                    line_id.c_str(),
                    isp.c_str(),
                    quality.c_str(),
                    balance.c_str(),
                    upload.c_str(),
                    download.c_str(),
                    enable.c_str());
                
                name = "";
                isp = "";
                quality = "";
                line_id = "";
                balance = "";
                upload = "";
                download = "";
                enable = "";
                        
                if(cur->NextSibling())
                {
                    parseGuideTc(cur->NextSibling());
                }
            }
            
            cur=cur->FirstChild();
        }
        return;
    }

    void GuideParser::parseGuideZone(TiXmlNode * cur)
    {
        std::string type = "";
        std::string oem = "";
        std::string isp = "";
        maxnet::Log pLog("guide_zone_pasrse", false, false, false);
        
        while(cur)
        {
            if(cur->Type() ==  TiXmlNode::TINYXML_ELEMENT)
            {

                TiXmlAttribute *attr = cur->ToElement()->FirstAttribute();
                while(attr)
                {
                    if (strcmp(attr->Name(), "type") == 0)
                    {
                        type = attr->Value() ? attr->Value() : "";
                    }
                    
                    if(strcmp(attr->Name(), "oem") == 0)
                    {
                        oem = attr->Value() ? attr->Value() : "";
                    }
                    else if(strcmp(attr->Name(), "isp") == 0)
                    {
                        isp = attr->Value() ? attr->Value() : "";
                    }
                    
                    attr=attr->Next();      
                }
                
                zonemgr.add(type, oem, isp);
                debug("type: %s, oem: %s, isp: %s\n", 
                    type.c_str(),
                    oem.c_str(),
                    isp.c_str());
                
                type = "";
                oem = "";
                isp = "";
                        
                if(cur->NextSibling())
                {
                    parseGuideZone(cur->NextSibling());
                }
            }
            
            cur=cur->FirstChild();
        }
        return;
    }

	int	GuideParser::processGuideLan()
    {
		TiXmlNode * if_lans_node = findNode(&doc, "xmlconfig/interface_lans");

        debug("\n\n-------------------LANS---------------------\n");
		if(if_lans_node != NULL && if_lans_node->FirstChild())
        {   
			parseGuideLan(if_lans_node->FirstChild());
		}

		return 0;
	}

	int	GuideParser::processGuideServer()
    {
		TiXmlNode * server_node = findNode(&doc, "xmlconfig/servers");
        
        debug("\n\n-------------------SERVERS---------------------\n");
		if(server_node != NULL && server_node->FirstChild())
        {   
			parseGuideServer(server_node->FirstChild());
		}

		return 0;
	}

    int	GuideParser::processGuideWan()
    {
		TiXmlNode * wan_node = findNode(&doc, "xmlconfig/interface_wans");

        debug("\n\n-------------------WANS---------------------\n");
		if(wan_node != NULL && wan_node->FirstChild())
        {   
			parseGuideWan(wan_node->FirstChild());
		}

		return 0;
	}

    int	GuideParser::processGuideAdsl()
    {
		TiXmlNode * adsl_node = findNode(&doc, "xmlconfig/adsl_clients");

        debug("\n\n-------------------ADSLS---------------------\n");
		if(adsl_node != NULL && adsl_node->FirstChild())
        {   
			parseGuideAdsl(adsl_node->FirstChild());
		}

		return 0;
	}

    int	GuideParser::processGuideAppbind()
    {
		TiXmlNode * appbind_node = findNode(&doc, "xmlconfig/appbinds");

        debug("\n\n-------------------APPBIND---------------------\n");
		if(appbind_node != NULL && appbind_node->FirstChild())
        {   
			parseGuideAppbind(appbind_node->FirstChild());
		}

		return 0;
	}

    int	GuideParser::processGuideTc()
    {
		TiXmlNode * tc_node = findNode(&doc, "xmlconfig/virtual_lines");

        debug("\n\n-------------------TC---------------------\n");
		if(tc_node != NULL && tc_node->FirstChild())
        {   
			parseGuideTc(tc_node->FirstChild());
		}

		return 0;
	}

    int	GuideParser::processGuideZone()
    {
		TiXmlNode * zone_node = findNode(&doc, "xmlconfig/zones");

        debug("\n\n-------------------ZONE---------------------\n");
		if(zone_node != NULL && zone_node->FirstChild())
        {   
			parseGuideZone(zone_node->FirstChild());
		}

		return 0;
	}
    
    int GuideParser::processGuideDnat()
    {
        int ret = 1;

        dnat.setEnable(getNodeValuebyName("dnat/dnat_enable"));
        
        debug("\n\n-------------------DNAT---------------------\n");
        debug("enable: %s\n", dnat.getEnable().c_str());
        
		if(dnat.isVaild())
        {
			ret = 0;
		}
		else
        {
			error("dnat enable is not vaild!");
		}
		return ret;
    }

	int GuideParser::process()
    {
		int ret = 1;
        
		ret = loadConfig();
		if(ret != 0) 
		{
            return 1;
		}

		processGuideLan();
        processGuideServer();
        processGuideWan();
        processGuideAdsl();
        processGuideAppbind();
        processGuideTc();
        processGuideZone();
        processGuideDnat();
        
		unloadConfig();

		if(check == 0)
		{
            
		}
        
		return 0;
	}

    int GuideParser::GetStructGuideLan(struct guide_if_lan ** p_xml)
    {	
    	int struct_size = 0;	
    	int iIndex = 0;	
    	std::vector<Node *> lst;	
    	struct guide_if_lan * tmpPtr = NULL;	
        
    	if(*p_xml != NULL)
    	{
    		return RET_ERROR;
    	}
    	if(processGuideLan() > 0)
    	{
    		return RET_ERROR;
    	}
    	
    	struct_size = sizeof(struct guide_if_lan) + 	
    			lanmgr.dataNodeCount * 	
    			sizeof(struct guide_if_lan_info);	
    	*p_xml =(struct guide_if_lan*)malloc(struct_size);

    	if(NULL == *p_xml)
        {   
    		return RET_ERROR;	
    	}
        
    	tmpPtr = *p_xml;	
    	memset(tmpPtr , 0 ,  struct_size);				
    	tmpPtr->num = lanmgr.dataNodeCount; 	
    	lst = lanmgr.getList();	
    	for(unsigned int i = 0; i < lst.size(); i++)
        {   
    		Lan *item = (Lan *) lst.at(i);	
    		if(item->isGroup())
            {   
    			continue;	
    		}		

            strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIfName().c_str(), STRING_LENGTH);
    		strncpy(tmpPtr->pinfo[iIndex].ipaddr, item->getIP().c_str(),STRING_LENGTH);
    		strncpy(tmpPtr->pinfo[iIndex].netmask, item->getMask().c_str(),STRING_LENGTH);	
            strncpy(tmpPtr->pinfo[iIndex].mac , item->getMAC().c_str(),STRING_LENGTH);	

    		++iIndex;	
    	}	

    	return struct_size;
    }

    int GuideParser::GetStructGuideServer(struct guide_server ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_server * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }

        if(processGuideServer() > 0)
        {
            return RET_ERROR;
        }
        
        struct_size = sizeof(struct guide_server) +     
                servmgr.dataNodeCount *    
                sizeof(struct guide_server_info);   
        *p_xml =(struct guide_server*)malloc(struct_size);
    
        if(*p_xml == NULL)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr , 0 ,  struct_size);              
        tmpPtr->num = servmgr.dataNodeCount;   
        lst = servmgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            Server *item = (Server *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            tmpPtr->pinfo[iIndex].parentid = atoi(item->getParentid().c_str());
            strncpy(tmpPtr->pinfo[iIndex].value, item->getValue().c_str(), STRING_LENGTH);    
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    int GuideParser::GetStructGuideWan(struct guide_if_wan ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_if_wan * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }
        
        if(processGuideWan() > 0)
        {
            return RET_ERROR;
        }
        
        struct_size = sizeof(struct guide_if_wan) +     
                wanmgr.dataNodeCount *    
                sizeof(struct guide_if_wan_info);   
        *p_xml =(struct guide_if_wan*)malloc(struct_size);
    
        if(NULL == *p_xml)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr , 0 ,  struct_size);              
        tmpPtr->num = wanmgr.dataNodeCount;   
        lst = wanmgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            Wan *item = (Wan *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            strncpy(tmpPtr->pinfo[iIndex].ifname, item->getIfname().c_str(), STRING_LENGTH);
            tmpPtr->pinfo[iIndex].line_id = atoi(item->getLineid().c_str());
            tmpPtr->pinfo[iIndex].isp = atoi(item->getISP().c_str());
            tmpPtr->pinfo[iIndex].quality = atoi(item->getQuality().c_str());
            tmpPtr->pinfo[iIndex].upload = atoi(item->getUpload().c_str());
            tmpPtr->pinfo[iIndex].download = atoi(item->getDownload().c_str());
            strncpy(tmpPtr->pinfo[iIndex].ipaddr, item->getIPaddr().c_str(), STRING_LENGTH);
            strncpy(tmpPtr->pinfo[iIndex].netmask, item->getNetmask().c_str(), STRING_LENGTH);
            strncpy(tmpPtr->pinfo[iIndex].gateway, item->getGateway().c_str(), STRING_LENGTH);
            strncpy(tmpPtr->pinfo[iIndex].mac, item->getMAC().c_str(), STRING_LENGTH);
            tmpPtr->pinfo[iIndex].conntype = atoi(item->getConntype().c_str());
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    int GuideParser::GetStructGuideAdsl(struct guide_adsl ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_adsl * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }
        
        if(processGuideAdsl() > 0)
        {
            return RET_ERROR;
        }
        
        struct_size = sizeof(struct guide_adsl) +     
                adslmgr.dataNodeCount *    
                sizeof(struct guide_adsl_info);   
        *p_xml =(struct guide_adsl*)malloc(struct_size);
    
        if(NULL == *p_xml)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr , 0 ,  struct_size);              
        tmpPtr->num = adslmgr.dataNodeCount;   
        lst = adslmgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            Adsl *item = (Adsl *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            strncpy(tmpPtr->pinfo[iIndex].ifname, item->getAdslIfname().c_str(), STRING_LENGTH);
            tmpPtr->pinfo[iIndex].line_id = atoi(item->getAdslLineid().c_str());
            tmpPtr->pinfo[iIndex].isp = atoi(item->getAdslISP().c_str());
            tmpPtr->pinfo[iIndex].quality = atoi(item->getAdslQuality().c_str());
            tmpPtr->pinfo[iIndex].upload = atoi(item->getAdslUpload().c_str());
            tmpPtr->pinfo[iIndex].download = atoi(item->getAdslDownload().c_str());
            strncpy(tmpPtr->pinfo[iIndex].username, item->getAdslUsername().c_str(), STRING_LENGTH);
            strncpy(tmpPtr->pinfo[iIndex].password, item->getAdslPassword().c_str(), STRING_LENGTH);
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    int GuideParser::GetStructGuideAppbind(struct guide_appbind ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_appbind * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }
        
        if(processGuideAppbind() > 0)
        {
            return RET_ERROR;
        }
        
        struct_size = sizeof(struct guide_appbind) +     
                appbindmgr.dataNodeCount *    
                sizeof(struct guide_appbind_info);   
        *p_xml =(struct guide_appbind*)malloc(struct_size);
    
        if(NULL == *p_xml)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr , 0 ,  struct_size);              
        tmpPtr->num = appbindmgr.dataNodeCount;   
        lst = appbindmgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            Appbind *item = (Appbind *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            strncpy(tmpPtr->pinfo[iIndex].name, item->getName().c_str(), STRING_LENGTH);
            tmpPtr->pinfo[iIndex].bind_line_id = atoi(item->getBindLineid().c_str());
            tmpPtr->pinfo[iIndex].app_type = atoi(item->getAppType().c_str());
            tmpPtr->pinfo[iIndex].bind_type = atoi(item->getBindType().c_str());
            tmpPtr->pinfo[iIndex].bypass = atoi(item->getBypass().c_str());
            tmpPtr->pinfo[iIndex].maxupload = atoi(item->getMaxUpload().c_str());
            tmpPtr->pinfo[iIndex].maxdownload = atoi(item->getMaxDownload().c_str());
            tmpPtr->pinfo[iIndex].weight = atoi(item->getWeight().c_str());
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    
    int GuideParser::GetStructGuideTc(struct guide_tc ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_tc * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }

        debug("%s: start processGuideTc\n", __FUNCTION__);
        if(processGuideTc() > 0)
        {
            return RET_ERROR;
        }
        debug("%s: fini processGuideTc, node count: %d\n", __FUNCTION__, tcmgr.dataNodeCount);
        
        struct_size = sizeof(struct guide_tc) +     
                tcmgr.dataNodeCount *    
                sizeof(struct guide_tc_info);   
        *p_xml =(struct guide_tc *)malloc(struct_size);
    
        if(NULL == *p_xml)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr, 0, struct_size);              
        tmpPtr->num = tcmgr.dataNodeCount;   
        lst = tcmgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            TC *item = (TC *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            strncpy(tmpPtr->pinfo[iIndex].name, item->getName().c_str(), STRING_LENGTH);
            tmpPtr->pinfo[iIndex].line_id = atoi(item->getLineid().c_str());
            tmpPtr->pinfo[iIndex].isp = atoi(item->getISP().c_str());
            tmpPtr->pinfo[iIndex].quality = atoi(item->getQuality().c_str());
            tmpPtr->pinfo[iIndex].balance = atoi(item->getBalance().c_str());
            tmpPtr->pinfo[iIndex].upload = atoi(item->getUpload().c_str());
            tmpPtr->pinfo[iIndex].download = atoi(item->getDownload().c_str());
            tmpPtr->pinfo[iIndex].enable = atoi(item->getEnable().c_str());
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    int GuideParser::GetStructGuideZone(struct guide_sel_zone ** p_xml)
    {
        int struct_size = 0;    
        int iIndex = 0; 
        std::vector<Node*> lst; 
        struct guide_sel_zone * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }
        
        if(processGuideZone() > 0)
        {
            return RET_ERROR;
        }
        
        struct_size = sizeof(struct guide_sel_zone) +     
                zonemgr.dataNodeCount *    
                sizeof(struct guide_sel_zone_info);   
        *p_xml =(struct guide_sel_zone *)malloc(struct_size);
    
        if(NULL == *p_xml)
        { 
            return RET_ERROR;   
        }       
        tmpPtr = *p_xml;    
        memset(tmpPtr ,0, struct_size);              
        tmpPtr->num = zonemgr.dataNodeCount;   
        lst = zonemgr.getList();   
        for(unsigned int i = 0; i < lst.size(); i++)
        {   
            Zone *item = (Zone *) lst.at(i);    
            if(item->isGroup())
            {    
                continue;   
            }       
            
            tmpPtr->pinfo[iIndex].type = atoi(item->getType().c_str());
            strncpy(tmpPtr->pinfo[iIndex].oem, item->getOEM().c_str(), STRING_LENGTH);
            strncpy(tmpPtr->pinfo[iIndex].isp, item->getISP().c_str(), STRING_LENGTH);
                      
            ++iIndex;   
        }   
        
        return struct_size;
    }

    int GuideParser::GetStructGuideDnat(struct guide_dnat ** p_xml)
    {    
        struct guide_dnat * tmpPtr = NULL;    
        
        if(*p_xml != NULL)  
        {
            return RET_ERROR;       
        }
        
        if(processGuideDnat() > 0)
        {
            return RET_ERROR;
        }
          
        *p_xml =(struct guide_dnat *)malloc(sizeof(struct guide_dnat));
        if(*p_xml == NULL)
        { 
            return RET_ERROR;   
        }       
        
        tmpPtr = *p_xml;    
        memset(tmpPtr ,0, sizeof(struct guide_dnat));        
        
        tmpPtr->enable = atoi(dnat.getEnable().c_str());                      
        
        return sizeof(struct guide_dnat);
    }
        

}


