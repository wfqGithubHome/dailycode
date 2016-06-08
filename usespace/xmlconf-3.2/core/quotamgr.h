#ifndef _QUOTAMGR_H
#define _QUOTAMGR_H

#include <iostream>
#include <string>
#include <vector>
#include "nodemgr.h"



namespace maxnet{


	class QuotaMgr : public NodeMgr  {
		public:

			QuotaMgr();
			~QuotaMgr();
			void output();

			void setQuotaControl(bool status);
			bool getQuotaControl();

			void add(const std::string &name, const std::string &bandwidth_quota, 
				const std::string &ip_type,const std::string &cycle_type,const int id,const std::string &comment);
		private:
			bool QuotaControl;



	};

}

#endif 
