#ifndef __TIMINGMGR_H__
#define __TIMINGMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "timing.h"

namespace maxnet{
        class TimingMgr : public NodeMgr{
                public:
                    TimingMgr();
                    ~TimingMgr();

                    void output();

                    void add(std::string name, std::string iface, std::string minute,
                        std::string hour, std::string day, std::string month, std::string week, std::string comment);
            private:
            
        };

}

#endif
