#ifndef __FIREWALL_POLICY_H__
#define __FIREWALL_POLICY_H__

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class FirewallPolicy : public Node{

		public:

			FirewallPolicy();
			~FirewallPolicy();
			bool isVaild();
			void output();

			void setId(std::string id);
			void setEnable(bool enable);
			void setSrc(std::string src);
			void setDst(std::string dst);
			void setIsp(std::string isp);
			void setProto(std::string proto);
            void setSession(std::string session);
			void setIface(std::string iface);
			void setAction(std::string action);
			void setTimed(std::string timed);
			void setWeek(std::string week);
			void setDay(std::string day);
            void setInLimit(std::string in_limit);
            void setOutLimit(std::string out_limit);
            void setComment(std::string comment);

			std::string getId();
			bool getEnable();
			std::string getSrc();
			std::string getDst();
			std::string getIsp();
			std::string getProto();
            std::string getSession();
			std::string getIface();
			std::string getAction();
			std::string getTimed();
			std::string getWeek();
			std::string getDay();
            std::string getInLimit();
            std::string getOutLimit();
            std::string getComment();

		private:
			std::string id;
            bool enable;
            std::string src;
            std::string dst;
            std::string isp;
            std::string proto;
            std::string session;
            std::string iface;
            std::string action;
            std::string timed;
            std::string week;
            std::string day;
            std::string in_limit;
            std::string out_limit;
            std::string comment;
	};
}

#endif // _ZERO_FIREWALL_H
