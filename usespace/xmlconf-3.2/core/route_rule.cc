#include <sstream>
#include "route_rule.h"
#include "../global.h"

namespace maxnet{

	RouteRule::RouteRule(){

	}

	RouteRule::~RouteRule(){
		
	}
	
	bool RouteRule::isVaild(){
		if(name.size() <= 0) return false;
		return true;
	}

	std::string RouteRule::getCommand(){
		std::string command = "";
		std::string src_str = "";
		std::string dst_str = "";
		std::string protocol_str = "";
		std::string port_str = "";
		std::string tos_str = "";
		std::stringstream stream_string;
		/*char buffer[10] = {0};
		unsigned int tos_i = 0;
		unsigned int valid_flag = 0;*/
		
		src_str = getSRC();
		dst_str = getDST();
		tos_str = getTOS();
		if (src_str.size() > 0){
			command = " from " + src_str;
		}
		if (dst_str.size() > 0){
			command = command + " to " + dst_str;
		}

		
		/*if ((tos_str != "null") && 
			(tos_str.size() > 0)){
			stream_string << tos_str;
			stream_string >> tos_i;
			if (0 <= tos_i && tos_i <= 15){
				sprintf(buffer, "0x%x", tos_i * 2);
				command = command + " tos " + buffer;
			}
			else if(valid_flag == 0){
				command="";
			}
		}*/
		return command;
	}

	void RouteRule::output(){
		std::cout << "# \"" << getName() << "\"\t\""
							<< getSRC() << "\"\t\""
							<< getDST() << "\"\t\""
							<< getProtocol() << "\"\t\""
							<< getSRCPort() << "\"\t\""
							<< getDSTPort() << "\"\t\""
							<< getTOS() << "\"\t\""
							<< getAction() << "\"\t\""
							<< getFWMark() << "\"\t\t//" << getComment() << std::endl;

		return;

	}

	/*void RouteRule::setName(const std::string name){
		this->name = name;
	}*/
	
	void RouteRule::setSRC(const std::string src){
		this->src = src;
	}

	void RouteRule::setDST(const std::string dst){
		this->dst = dst;
	}

	void RouteRule::setProtocol(const std::string protocol){
		this->protocol = protocol;
	}

	void RouteRule::setSRCPort(const std::string src_port){
		this->src_port = src_port;
	}
	
	void RouteRule::setDSTPort(const std::string dst_port){
		this->dst_port = dst_port;
	}

	void RouteRule::setTOS(const std::string tos){
		this->tos = tos;
	}

	void RouteRule::setAction(const std::string action){
		this->action = action;
	}
	
	void RouteRule::setFWMark(const std::string fwmark){
		this->fwmark = fwmark;
	}


	/*std::string RouteRule::getName(){
		return name;
	}*/

	std::string RouteRule::getSRC(){
		return src;
	}
	
	std::string RouteRule::getDST(){
		return dst;
	}

	std::string RouteRule::getProtocol(){
		return protocol;
	}

	std::string RouteRule::getSRCPort(){
		return src_port;
	}
	
	std::string RouteRule::getDSTPort(){
		return dst_port;
	}
	
	std::string RouteRule::getTOS(){
		return tos;
	}

	std::string RouteRule::getAction(){
		return action;
	}
	
	std::string RouteRule::getFWMark(){
		return fwmark;
	}

}

