#ifndef _ZERO_ROUTE_DNAT_H
#define _ZERO_ROUTE_DNAT_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class RouteDNAT : public Node{

		public:

			RouteDNAT();
			~RouteDNAT();
			bool isVaild();
			void output();
			std::string getCommand();

            void setID(std::string id);
            void setEnable(bool enable);
            void setloopEnable(bool enable);
			void setProtocoLId(const std::string protocol_id);
			void setcomment(const std::string comment);
			void setWanIp(const std::string wan_ip);
			void setWanPortBefore(const std::string wan_port_before);
			void setWanPortAfter(const std::string wan_port_after);
			void setLanPortBefore(const std::string lan_port_before);
			void setLANPortAfter(const std::string lan_port_after);
            void setLanIp(const std::string lan_ip);
            void setIfname(const std::string ifname);

            std::string	getID();
            bool		getEnable();
            bool		getloopEnable();
			std::string getProtocoLId();
			std::string getcomment();
			std::string getWanIp();
			std::string getWanPortBefore();
			std::string getWanPortAfter();
			std::string	getLanPortBefore();
			std::string	getLANPortAfter();
            std::string getLanIp();
            std::string getIfname();

		private:
            std::string id;
            bool        enable;
            bool        loop;
            std::string protocol_id;
            std::string comment;
            std::string wan_ip;
            std::string wan_port_before;
            std::string wan_port_after;
            std::string lan_port_before;
            std::string lan_port_after;
            std::string lan_ip;
            std::string ifname;
	};

}

#endif // _ZERO_ROUTE_DNAT_H

