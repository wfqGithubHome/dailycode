#ifndef __ROUTE_PPTPD_H__
#define __ROUTE_PPTPD_H__

#include <iostream.h>
#include <string.h>
#include <vector.h>

#include "node.h"

namespace maxnet{


        class Pptpd : public Node{

                public :

                    Pptpd();
                    ~Pptpd();
                    bool isVaild();
                    void output();

                    void setPptpdEnable(bool pptpdenable);
                    void setPptpdlocalip(std::string localip);
                    void setPptpdstartip(std::string startip);
                    void setPptpdendip(std::string endip);
                    void setPptpddns(std::string dns);
                    void setPptpdentry(std::string entry);
                    
                    bool getPptpdEnable();
                    std::string getPptpdlocalip();
                    std::string getPptpdstartip();
                    std::string getPptpdendip();
                    std::string getPptpddns();
                    std::string getPptpdentry();
                    

                private:
                    std::string localip;
                    std::string startip;
                    std::string endip;
                    std::string dns;
                    std::string entry;
                    bool pptpdenable;
        };
        
}

#endif

