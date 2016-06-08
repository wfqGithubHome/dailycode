#ifndef _ZERO_WRITE_H
#define _ZERO_WRITE_H

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "../base/logwraper.h"
#include "../tinyxml/tinyxml.h"

namespace maxnet{


class Writer  : public LogWraper {

  public:

    Writer(Log *log, const std::string filename);
	~Writer();
	int		setNode(const std::string nodename, const std::string value);
  protected:
	std::string		filename;
   private:
	TiXmlNode  * findXmlElement(TiXmlNode *root, const std::string nodename);
	void split(std::string instr, std::vector<std::string> *lstoutstr);
    Writer();
};

}

#endif // _ZERO_WRITE_H
