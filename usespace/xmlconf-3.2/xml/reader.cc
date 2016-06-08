#include "reader.h"
#include "../global.h"
#include "../tinyxml/tinyxml.h"

namespace maxnet{
	// ----------------------------------------------------------------------
	// STDOUT dump and indenting utility functions
	// ----------------------------------------------------------------------
	unsigned int Reader::NUM_INDENTS_PER_SPACE=2;
	
	Reader::Reader(Log *log, const std::string filename){
		setLog(log);	
		setLogHeader("[R] ");
		this->filename = filename;
	}

	Reader::~Reader(){
		
	}

	/*  Split string of names separated by SPLIT_CHAR  */
	#define SPLIT_CHAR "/"
	void Reader::split(std::string instr, std::vector<std::string> *lstoutstr) {

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
	
	TiXmlNode  *Reader::findXmlElement(TiXmlNode *root, const std::string nodename)
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




	const char * Reader::getIndent( unsigned int numIndents )
	{
		static const char * pINDENT="                                      + ";
		static const unsigned int LENGTH=strlen( pINDENT );
		unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
		if ( n > LENGTH ) n = LENGTH;

		return &pINDENT[ LENGTH-n ];
	}

	// same as getIndent but no "+" at the end
	const char * Reader::getIndentAlt( unsigned int numIndents )
	{
		static const char * pINDENT="                                        ";
		static const unsigned int LENGTH=strlen( pINDENT );
		unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
		if ( n > LENGTH ) n = LENGTH;

		return &pINDENT[ LENGTH-n ];
	}

	int Reader::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
	{
		if ( !pElement ) return 0;

		TiXmlAttribute* pAttrib=pElement->FirstAttribute();
		int i=0;
		int ival;
		double dval;
		const char* pIndent=getIndent(indent);
		printf("\n");
		while (pAttrib)
		{
			printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

			if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
			if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
			printf( "\n" );
			i++;
			pAttrib=pAttrib->Next();
		}
		return i;	
	}

	void Reader::dump_to_stdout( TiXmlNode* pParent, unsigned int indent)
	{
		if ( !pParent ) return;

		TiXmlNode* pChild;
		TiXmlText* pText;
		int t = pParent->Type();
		printf( "%s", getIndent(indent));
		int num;

		switch ( t )
		{
		case TiXmlNode::TINYXML_DOCUMENT:
			printf( "Document" );
			break;

		case TiXmlNode::TINYXML_ELEMENT:
			printf( "Element [%s]", pParent->Value() );
			num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
			switch(num)
			{
				case 0:  printf( " (No attributes)"); break;
				case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
				default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
			}
			break;

		case TiXmlNode::TINYXML_COMMENT:
			printf( "Comment: [%s]", pParent->Value());
			break;

		case TiXmlNode::TINYXML_UNKNOWN:
			printf( "Unknown" );
			break;

		case TiXmlNode::TINYXML_TEXT:
			pText = pParent->ToText();
			printf( "Text: [%s]", pText->Value() );
			break;

		case TiXmlNode::TINYXML_DECLARATION:
			printf( "Declaration" );
			break;
		default:
			break;
		}
		printf( "\n" );
		for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
		{
			dump_to_stdout( pChild, indent+1 );
		}
	}

	
	int Reader::getNode(const std::string nodename)
	{
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
			std::cerr  << "get xml node error: node \"" << nodename << "\" not found! " << std::endl;	
			return 1;
		}
		if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			std::cerr  << "get xml node error: node \"" << nodename << "\" is not a element node! " << std::endl;
			return 1;
		}
		child = node;
	
		while(child && child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			child = child->FirstChild();
		}
		if(child == NULL || child->Type() != TiXmlNode::TINYXML_TEXT)
		{
			std::cerr  << "get xml node error: node \"" << nodename << "\" is a parent node! " << std::endl;
			return 1;
		}
		std::cout << node->Value() << " = " << child->ToText()->Value() << std::endl;

		return 0;
	}


}

