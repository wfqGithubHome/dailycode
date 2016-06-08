#include "rwxml.h"
#include "../global.h"

namespace maxnet{

	RWXml::RWXml(Log *log, const std::string filename){
		setLog(log);	
		setLogHeader("[W] ");
		this->filename = filename;
	}

	RWXml::~RWXml(){
		this->filename = "";
	}

	/*  Split string of names separated by SPLIT_CHAR  
	ie. "xmlconfig/system/hostname "
	*/
	#define SPLIT_CHAR "/"
	void RWXml::split(std::string instr, std::vector<std::string> *lstoutstr) {

		std::string str = instr;
		unsigned int loc;
		/*  Break string into substrings and store beginning  */
		while(1){
			loc = str.find(SPLIT_CHAR);
			if( loc == std::string::npos ){
				lstoutstr->push_back(str);
				return;
			}
			std::string substr = str.substr(0, loc);
			str.erase(0, loc + 1);
			lstoutstr->push_back(substr);
		}

	}
	
	TiXmlNode  *RWXml::findXmlElement(TiXmlNode *root, const std::string nodename,
										  TiXmlNode ** parent , TiXmlNode ** child)
	{	
		TiXmlNode * tmpNode = root;
		//TiXmlNode * child = NULL;
		std::vector<std::string> token;
		
		*parent = tmpNode;
		*child = NULL;

		// split the node name to keyword token
		split(nodename, &token);

		for(unsigned int i = 0; i < token.size(); i++)
		{
			*parent = tmpNode;
			for(*child = tmpNode->FirstChild(); *child; *child = (*child)->NextSibling())
			{
				if(strcmp((*child)->Value(), token.at(i).c_str()) == 0)
				{
					tmpNode = *child;
					break;
				}
			}
		}
        token.clear();
		return *child;
	}

	int RWXml::deleteNode(const std::string nodename){
		TiXmlDocument doc;
		//TiXmlNode *node;
		TiXmlNode * parent = NULL;
		TiXmlNode * child = NULL;

		if(!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr  << "open xml config file error: " << filename << std::endl;
			return 1;	
		}

		//dump_to_stdout(&doc);		
		
		findXmlElement(&doc, nodename, &parent,&child);
		if(child == NULL)
		{
			std::cerr  << "delete xml node error: node \"" << nodename << "\" not found! " << std::endl;	
			return 1;
		}
		if(child->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			std::cerr  << "delete xml node error: node \"" << nodename << "\" is not a element node! " << std::endl;
			return 1;
		}
		//printf("prent=%x , child=%x\n" , (unsigned int)parent , (unsigned int)child);

		parent->RemoveChild(child);

		if(!doc.SaveFile(filename.c_str()))
		{
			std::cerr  << "delete xml config file error: " << filename << std::endl;
			return 1;
		}
		return 0;

	}

	#define REPLACE_PRINTF 0
	int RWXml::replaceNode(const std::string content){
		TiXmlDocument doc;
		TiXmlDocument memdoc;
		TiXmlNode *memnode;
		TiXmlNode * parent = NULL;
		TiXmlNode * child = NULL;
		std::string childkey = "xmlconfig/";

		if(content.length() <= 3 )
			return 0;

		if(!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr  << "open xml config file error: " << filename << std::endl;
			return 1;	
		}
		#if REPLACE_PRINTF
		printf("will Parse\n");
		#endif	
	
		
		memdoc.Parse(content.c_str(), NULL , TIXML_ENCODING_UTF8);
		
		#if REPLACE_PRINTF
		memdoc.Print();
		#endif	
		
		memnode = &memdoc;
		
		memnode = memnode->FirstChild();
		childkey.append(memnode->Value());
		//printf("name =%s\n" , childkey.c_str());
		
		findXmlElement(&doc,childkey, &parent,&child);
		
#if REPLACE_PRINTF
		printf("prent=%x , child=%x\n" , (unsigned int)parent , (unsigned int)child);
#endif

		if(child == NULL)
		{
			std::cerr  << "replace xml node error: node \"" << memnode->Value() 
						<< "\" not found! " << std::endl;	
			return 1;
		}
		if(child->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			std::cerr  << "replace xml node error: node \"" << memnode->Value() 
						<< "\" is not a element node! " << std::endl;
			return 1;
		}
		
#if REPLACE_PRINTF
        printf("prent=%x , child=%x\n" , (unsigned int)parent , (unsigned int)child);
#endif
		parent->ReplaceChild(child , *memnode);

		if(!doc.SaveFile(filename.c_str()))
		{
			std::cerr  << "replace xml config file error: " << filename << std::endl;
			return 1;
		}
		
		return 0;
		
	}

	int RWXml::replaceNode(const std::string nodename, const std::string content){
		TiXmlDocument doc;
		TiXmlDocument memdoc;
		TiXmlNode *memnode;
		TiXmlNode * parent = NULL;
		TiXmlNode * child = NULL;
        TiXmlNode * tmpnod = NULL;
		std::string childkey = "xmlconfig/";
		
//        maxnet::Log sLog("/flash/etc/xmlconf.err", false,false,false); 

		if(content.length() <= 3 )
			return 0;

		if(!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
		{			
//			std::cerr  << "open xml config file error: " << filename << std::endl;
			return 1;	
		}
		
		#if REPLACE_PRINTF
		printf("will Parse\n");
		#endif	
		
		memdoc.Parse(content.c_str(), NULL , TIXML_ENCODING_UTF8);
        if (memdoc.Error())
        {
 //           sLog.debug("nodename: %s, error: %d, descr: %s\n", 
 //               nodename.c_str(), memdoc.ErrorId(), memdoc.ErrorDesc());
            return 1;
        }
		memnode = &memdoc;

		#if REPLACE_PRINTF
		memdoc.Print();
		#endif	
		
		memnode = memnode->FirstChild();
		childkey.append(nodename);
		
		#if REPLACE_PRINTF
		printf("name =%s\n" , childkey.c_str());
		#endif
		
		findXmlElement(&doc,childkey, &parent,&child);

        
#if REPLACE_PRINTF
        printf("prent=%x , child=%x\n" , (unsigned int)parent , (unsigned int)child);
#endif
        if(parent==NULL)
        {
        	std::cerr  << "parent xml node error: node \"" << nodename << "\" is not a element node! " << std::endl;
            return 1;
        }
		
        if(child == NULL)
		{
        	for (/*memnode = memnode->FirstChild()*/; memnode; memnode = memnode->NextSibling())
        	{
        		parent->LinkEndChild( memnode->Clone());
        	}	
            
			//std::cerr  << "replace xml node error: node \"" << nodename << "\" not found! " << std::endl;	
			//return 1;
			goto EXIT;
		}
		if(child->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
//			std::cerr  << "replace xml node error: node \"" << nodename << "\" is not a element node! " << std::endl;
			return 1;
		}
		
#if REPLACE_PRINTF
		printf("prent=%x , child=%x\n" , (unsigned int)parent , (unsigned int)child);
#endif

		tmpnod = parent->ReplaceChild(child , *memnode);
        if (tmpnod == 0)
        {
 //           sLog.debug("nodename: %s, replace child error\n", nodename.c_str());
            return 1;
        }
EXIT:
        if (doc.Error())
        {
  //          sLog.debug("nodename: %s, error: %d, descr: %s\n", 
 //               nodename.c_str(), doc.ErrorId(), doc.ErrorDesc());
            return 1;
        }
        
		if(!doc.SaveFile(filename.c_str()))
		{
//			std::cerr  << "replace xml config file error: " << filename << std::endl;
			return 1;
		}
		return 0;
	}

}

