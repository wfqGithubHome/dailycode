#ifndef __WIFIMGR_H__
#define __WIFIMGR_H__

#include <iostream>
#include <string>
#include <vector>

#include "nodemgr.h"
#include "wifi.h"

namespace maxnet{

        class WifiMgr : public NodeMgr{
                public :
                    WifiMgr();
                    ~WifiMgr();
                    void output();

                    void setWifiEnable(bool wifienable);
                    void setPcTime(std::string wifipctime);
                    void setMaxTime(std::string wifimaxtime);
                    void setShareNum(std::string wifisharenum);
                    void setAdmins(std::string wifiadmins);

                    bool getWifiEnable();
                    std::string getPcTime();
                    std::string getMaxTime();
                    std::string getShareNum();
                    std::string getAdmins();

                    std::vector<Wifi *> getAdminList();
                    std::vector<Wifi *> getPoolList();

                    int getAdminCount();
                    int getPoolCount();

                    void admin_add(std::string username, std::string passwd);
                    void pool_add(std::string id, std::string startip, std::string endip, std::string comment, bool enable);
                    
                private:
                    void add_admin_node(Wifi *node);
                    void add_pool_node(Wifi *node);
                    
                    std::vector<Wifi *> admin_node_list;
                    std::vector<Wifi *> pool_node_list;
                    int admindatacount;
                    int pooldatacount;

                    bool wifienable;
                    std::string wifipctime;
                    std::string wifimaxtime;
                    std::string wifisharenum;
                    std::string wifiadmins;
        };

}

#endif

