#ifndef _QUOTA_H
#define _QUOTA_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Quota: public Node{

		public:

			Quota();
			~Quota();
			//std::string	getCommand();

			bool isVaild();
			void output();

			//void setName(std::string name);
			void setBandwidthQuota(std::string bandwidth_quota);
			void setIpType(std::string ip_type);
			void setCycleType(std::string cycle_type);
			void setQuotaID(int quotaid);

			int getQuotaID();
			//std::string getName();
			std::string getBandwidthQuota();
			std::string getIpType();
			std::string getCycleType();


		private:
			//std::string name;
			std::string bandwidth_quota;
			std::string ip_type;
			std::string cycle_type;
			int quotaid;
	


	};

}

#endif

