#ifndef __ROUTE_PPTPD_ACCOUNT_H__
#define __ROUTE_PPTPD_ACCOUNT_H__

#include <iostream.h>
#include <string.h>
#include <vector.h>

#include "node.h"

namespace maxnet{

    class PptpdAccount : public Node{

        public:
            PptpdAccount();
            ~PptpdAccount();

            bool isValid();
            void output();

            void setusername(std::string username);
            void setpasswd(std::string passwd);
            void setip(std::string ip);

            std::string getusername();
            std::string getpasswd();
            std::string getip();

        private:
            std::string username;
            std::string passwd;
            std::string ip;
    };
}

#endif //
