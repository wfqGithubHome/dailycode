#ifndef __PPTPDMGR_H__
#define __PPTPDMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_pptpd.h"

namespace maxnet{

    class PptpdMgr : public NodeMgr{

        public:
            PptpdMgr();
            ~PptpdMgr();
            void output();
            void add(std::string localip, std::string startip, std::string endip, std::string dns,
                std::string entry, bool enable);
    };

}

#endif
