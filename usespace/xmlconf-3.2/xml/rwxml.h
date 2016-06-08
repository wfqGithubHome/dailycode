#ifndef _ZERO_RWXML_H
#define _ZERO_RWXML_H

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "../base/logwraper.h"
#include "../tinyxml/tinyxml.h"

namespace maxnet{


class RWXml  : public LogWraper {

  public:

    RWXml(Log *log, const std::string filename);
	~RWXml();
	int		deleteNode(const std::string nodename);
	int		replaceNode(const std::string content);
	int		replaceNode(const std::string nodename,const std::string content);
  protected:
	std::string		filename;
   private:
	TiXmlNode  * findXmlElement(TiXmlNode *root, const std::string nodename,
					TiXmlNode ** parent , TiXmlNode ** child);
	void split(std::string instr, std::vector<std::string> *lstoutstr);
    RWXml();
};

}

#endif // _ZERO_RWXML_H
