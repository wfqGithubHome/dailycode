#include "protocol.h"
#include "protocolmgr.h"
#include "../global.h"

#define PAT_FILE_DIR	"/etc/signatures/Custom/"

namespace maxnet{

	ProtocolMgr::ProtocolMgr(bool addDefault){
		if(addDefault){
			add(0,"ANY", "l7", "" , "", "any","", "",  "any protocol");
			add(0,"any", "l7", "",  "", "any","", "",  "any protocol");
			add(0,"OTHERS", "l7", "", "", "others","", "", "others protocol");
			add(65530,"OTHERS_TCP", "l7", "", "", "others_tcp","", "", "others tcp protocols");
			add(65529,"OTHERS_UDP", "l7", "", "", "others_udp","", "", "others udp protocols");
			add(65533,"SOFT_BYPASS", "l7", "", "", "soft_bypass","", "", "soft bypass protocols");
			add(65535,"ANALYZING_TCP", "l7", "", "", "analyzing_tcp","", "", "analyzing_tcp protocols");
			add(65534,"ANALYZING_UDP", "l7", "", "", "analyzing_udp","", "", "analyzing_udp protocols");
			add(65531,"OTHERS_IPV4", "l7", "", "", "others_ipv4","", "", "others_ipv4 protocols");
			add(65528,"ICMP", "l7", "", "", "icmp","", "", "icmp protocols");
			add(65527,"ARP", "l7", "", "", "arp","", "", "arp protocols");
			add(65526,"MPLS", "l7", "", "", "mpls","", "", "mpls protocols");
			add(65525,"IPX", "l7", "", "", "ipx","", "", "ipx protocols");
			add(65524,"IPV6", "l7", "", "", "ipv6","", "", "ipv6 protocols");
			add(65523,"PPPOE", "l7", "", "", "pppoe","", "", "pppoe protocols");
			add(65500,"HTTP-BROWSE", "l7", "", "", "http-browse","", "", "http-browse protocols");
			add(65501,"HTTP-DOWNLOAD", "l7", "", "", "http-download","", "", "http-download protocols");
			add(65502,"HTTP-PART", "l7", "", "", "http-part","", "", "http-part protocols");
			add(65503,"HTTP-MIRROR", "l7", "", "", "http-mirror","", "", "http-mirror protocols");
			add(65504,"HTTP-TUNNEL", "l7", "", "", "http-tunnel","", "", "http-tunnel protocols");
			add(65505,"HTTP-OTHERS", "l7", "", "", "http-others","", "", "http-others protocols");
			add(65518,"P2P-COMMON", "l7", "", "", "p2p-common","", "", "p2p_traffic protocols");
			add(65517,"GAME-FLOW", "l7", "", "", "game-flow","", "", "Game flow protocols");
			customProtocolId = 10000;
		}

	}

	ProtocolMgr::~ProtocolMgr(){
#if 1		
		Protocol * proto = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			proto = (Protocol *)node_list.at(i);
		    if(!proto->isGroup())
				delete proto;
		}
		node_list.clear();
		//NodeMgr::Clear();
#endif
		return;
	}
	void ProtocolMgr::Clear(){
		customProtocolId = 0;
		NodeMgr::Clear();
	}

	void ProtocolMgr::addPatFile(Protocol * proto)
	{
		std::string filename = PAT_FILE_DIR + proto->getName() + ".sig";
		std::ofstream patfile;
		
		patfile.open(filename.c_str(), std::ios::out| std::ios::trunc); 

		patfile << proto->getName() << " " << customProtocolId << std::endl << std::endl;

		if(proto->getSPort().compare("") == 0){
			patfile << "[SRC 0,65535]";
		}
		else
		{
			patfile << "[SRC " << proto->getSPort() << "," << proto->getSPort() << "]";
		}

		if(proto->getDPort().compare("") == 0){
			patfile << "[DST 0,65535]";
		}
		else
		{
			patfile << "[DST " << proto->getDPort() << "," << proto->getDPort() << "]";
		}
		patfile << "[PROTO " << proto->getProto() << "]";
		
		patfile << std::endl;
		patfile << std::endl;

	    patfile.close();		

		customProtocolId++;
	}

	void ProtocolMgr::add(int id, std::string name, std::string protocol, std::string sport, std::string dport, std::string match, std::string type_,std::string option, std::string comment){
		Protocol * proto;

		if(find(name) != NULL)
		{
			return;
		}
		proto = new Protocol();
		proto->setId(id);
		proto->setName(name);
		proto->setProto(protocol);
		proto->setSPort(sport);
		proto->setDPort(dport);
		proto->setMatch(match);
		proto->setType(type_);
		proto->setOption(option);
		proto->setComment(comment);
		proto->setGroup(false);
		if(proto->isVaild()){
			if(protocol.compare("TCP") == 0 || protocol.compare("UDP") == 0)
			{
				proto->setMatch(name);

				addPatFile(proto);
			}
			addNode(proto);
		}
		else{
			delete proto;
		}
		return;
	}

	void ProtocolMgr::output(){
		Protocol * proto = NULL;
		std::cout  << "# protocols and protocols group" << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			proto = (Protocol *)node_list.at(i);
			proto->output();
		}
		
		return;
	}
}

