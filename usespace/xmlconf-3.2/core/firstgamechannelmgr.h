#ifndef __FIRSTGAMECHANNELMGR_H__
#define __FIRSTGAMECHANNELMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"

namespace maxnet{

    class FirstgamechannelMgr : public NodeMgr{
        public:
            FirstgamechannelMgr();
            ~FirstgamechannelMgr();
            void output();
            void add(std::string id, std::string name);

        private:

    };
}

#endif //end
