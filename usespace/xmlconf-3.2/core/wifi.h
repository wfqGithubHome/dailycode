#ifndef __WIFI_H__
#define __WIFI_H__

#include <iostream.h>
#include <string.h>
#include <vector.h>

#include "node.h"

namespace maxnet{


        class Wifi : public Node{

                public :

                    Wifi();
                    ~Wifi();
                    bool adminisvalid();
                    bool poolisvalid();
                    void output();

                    void setWifiAdminUsername(std::string wifiadminusername);
                    void setWifiAdminpasswd(std::string wifiadminpasswd);

                    void setWifiPoolId(std::string wifipoolid);
                    void setWifiPoolStartip(std::string wifipoolstartip);
                    void setWifiPoolEndip(std::string wifipoolendip);
                    void setWifiPoolComment(std::string wifipoolcomment);
                    void setWifiPoolEnable(bool wifipoolenable);
                    
                    std::string getWifiAdminUsername();
                    std::string getWifiAdminpasswd();

                    std::string getWifiPoolId();
                    std::string getWifiPoolStartip();
                    std::string getWifiPoolEndip();
                    std::string getWifiPoolComment();
                    bool getWifiPoolEnable();

                private:
                    std::string wifiadminusername;
                    std::string wifiadminpasswd;

                    std::string wifipoolid;
                    std::string wifipoolstartip;
                    std::string wifipoolendip;
                    std::string wifipoolcomment;
                    bool wifipoolenable;
        };

}

#endif
