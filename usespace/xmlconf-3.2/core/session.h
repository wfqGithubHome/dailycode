#ifndef _ZERO_SESSOIN_H
#define _ZERO_SESSION_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Session : public Node{

		public:

			Session();
			~Session();
			bool isVaild();
			void output();
			std::string getCommand();

			void setSession(std::string session);
			std::string getSession();
			int			getSessionLimitNum();
		private:
			std::string session;
	};

}

#endif // _ZERO_SESSOIN_H
