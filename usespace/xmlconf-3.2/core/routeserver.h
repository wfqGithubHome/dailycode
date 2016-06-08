#ifndef _ROUTE_SERVER_H__
#define _ROUTE_SERVER_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Routeserver : public Node{

		public:

			Routeserver();
			~Routeserver();
			bool isVaild();
			void output();

            void setenable(bool enable);
            void setpasswdenable(bool enable);
            bool getenable();
            bool getpasswdenable();

		private:
			bool enable;
            bool passwd_enable;
	};
}

#endif // _ROUTE_SERVER_H__

