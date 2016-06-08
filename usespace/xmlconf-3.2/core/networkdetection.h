/*****************************************
Auth:chenhao
Date:2013-7-12 15:12:13
*****************************************/
#ifndef __NETWORKDETECTION__H__
#define __NETWORKDETECTION__H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{


	class NetworkDetection : public Node{
		public:
			NetworkDetection();
			~NetworkDetection();
			bool isVaild();

			void setZeroPingValue(int zeropingvalue);
			int getZeroPingValue(void);

		private:
			int zeropingvalue;
	};
}

#endif
