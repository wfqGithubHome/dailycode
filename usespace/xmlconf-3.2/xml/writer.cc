#include "writer.h"
#include "../global.h"

namespace maxnet{

	Writer::Writer(Log *log, const std::string filename){
		setLog(log);	
		setLogHeader("[W] ");
		this->filename = filename;
	}

	Writer::~Writer(){
		
	}

	/*  Split string of names separated by SPLIT_CHAR  */
	#define SPLIT_CHAR "/"
	void Writer::split(std::string instr, std::vector<std::string> *lstoutstr) {

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
	
	TiXmlNode  *Writer::findXmlElement(TiXmlNode *root, const std::string nodename)
	{
	
		TiXmlNode * parent = root;
		TiXmlNode * child = NULL;
		std::vector<std::string> token;

		// split the node name to keyword token
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

	int Writer::setNode(const std::string nodename, const std::string value){
		TiXmlDocument doc;
		TiXmlNode *node;
		TiXmlNode *child;

		if(!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
		{			
			std::cerr  << "open xml config file error: " << filename << std::endl;
			return 1;	
		}

		//dump_to_stdout(&doc);		
		
		node = findXmlElement(&doc, nodename);
		if(node == NULL)
		{
			std::cerr  << "write xml node error: node \"" << nodename << "\" not found! " << std::endl;	
			return 1;
		}
		if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			std::cerr  << "write xml node error: node \"" << nodename << "\" is not a element node! " << std::endl;
			return 1;
		}
		child = node;
	
		while(child && child->FirstChild())
		{
			child = child->FirstChild();
		}
		
		if(child == NULL || (child->Type() != TiXmlNode::TINYXML_TEXT && child->Type() != TiXmlNode::TINYXML_ELEMENT))
		{
			std::cerr  << "write xml node error: node \"" << nodename << "\" is a parent node, type: " << child->Type() << std::endl;
			return 1;
		}
		if(child->Type() == TiXmlNode::TINYXML_TEXT)
		{
			child->SetValue(value.c_str());
		}
		else
		{
			TiXmlText* textNode = new TiXmlText( "" );

			textNode->SetValue(value.c_str());
			child->LinkEndChild(textNode);
		}
		if(!doc.SaveFile(filename.c_str()))
		{
			std::cerr  << "write xml config file error: " << filename << std::endl;
			return 1;
		}
		return 0;
#if 0
	//
		xmlDocPtr doc=xmlParseFile(filename.c_str());
		if(doc == NULL){
			std::cerr  << "parse xml config file error: " << filename << std::endl;
			return 1;
		}

		xmlNodePtr node = xmlDocGetRootElement(doc);

		if(node == NULL){
			xmlFreeDoc(doc); 
			std::cerr  << "parse xml config file error: " << filename << std::endl;
			return 1;
		}

		xmlChar *buf; 
		node = findNode(node, nodename);
		if(node != NULL){

			if(node->children != NULL && node->children->next != NULL){
				std::cerr  << "set xml node error: node \"" << nodename << "\" is a parent node! " << std::endl;
				return 1;
			}
			buf = xmlNodeGetContent(node); 
			if(buf){
				if ((xmlStrcmp(buf, (const xmlChar *)value.c_str() ))) { 
					xmlNodeSetContent(node,(const xmlChar *)value.c_str());
				}
				xmlSaveFile(filename.c_str(), doc);
				xmlFree(buf); 
				xmlFreeNode(node);
				/* BUG xmlFreeDoc hang */
//				if(nodename.compare("system/update_url") != 0){
//					xmlFreeDoc(doc); 
//				}
				return 0;
			}
		}
		//xmlFree(buf); 
		xmlFreeNode(node);
		xmlFreeDoc(doc); 

		std::cerr  << "set xml node error: node \"" << nodename << "\" not found! " << std::endl;
		return 1;
#endif		

	}

}

