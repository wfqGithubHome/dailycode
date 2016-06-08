#ifndef __ROUTE_PPTPD_ACCOUNTMGR_H__
#define __ROUTE_PPTPD_ACCOUNTMGR_H__

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"
#include "route_pptpd_account.h"

namespace maxnet{

    class PptpdAccountMgr : public NodeMgr{

        public:
            PptpdAccountMgr();
            ~PptpdAccountMgr();
            void output();

            void add(std::string username, std::string passwd, std::string ip);

    };

}

#endif //
