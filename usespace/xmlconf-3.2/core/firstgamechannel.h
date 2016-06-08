#ifndef __FIRSTGAMECHANNEL_H__
#define __FIRSTGAMECHANNEL_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{

    class Firstgamechannel : public Node{
    
        public:
            Firstgamechannel();
            ~Firstgamechannel();
            bool isValid();
            void output();

            void setId(std::string id);
            void setChannelName(std::string name);

            int getchannelId();
            std::string getChannelName();


        private:
            int     channel_id;
            std::string name;
    };
}

#endif //end
