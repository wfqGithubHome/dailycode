#ifndef _ZERO_READ_H
#define _ZERO_READ_H

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "../base/logwraper.h"
#include "../tinyxml/tinyxml.h"

namespace maxnet{


class Reader  : public LogWraper {

  public:

    Reader(Log *log, const std::string filename);
	~Reader();
	int		getNode(const std::string nodename);
  protected:
	std::string		filename;
   private:
    static unsigned int NUM_INDENTS_PER_SPACE;
    
	TiXmlNode  * findXmlElement(TiXmlNode *parent, const std::string nodename);
	void split(std::string instr, std::vector<std::string> *lstoutstr);
    Reader();

    const char * getIndent( unsigned int numIndents);
	const char * getIndentAlt( unsigned int numIndents);
	int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
	void dump_to_stdout(TiXmlNode* pParent, unsigned int indent = 0);
};

}

#endif // _ZERO_READ_H
