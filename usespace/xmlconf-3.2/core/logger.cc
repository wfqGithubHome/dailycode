#include "logger.h"
#include "../global.h"

namespace maxnet{

	Logger::Logger(){
		block_log = false;
		session_log = false;
		http_log = false;		
		scan_detect_log = false;
		dos_detect_log = false;
	}
  
	Logger::~Logger(){
		session_layer7_log.clear();
	}
	
	bool Logger::isVaild(){
		
		return true;
	}

	void Logger::output(){
		std::cout << "# logger" << std::endl;
		std::cout << "LOGGER_BLOCK_LOG=\"" << getIntbyBoolean(this->isBlockLog()) << "\"" << std::endl;
		std::cout << "LOGGER_HTTP_LOG=\"" << getIntbyBoolean(this->isHTTPLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SESSION_LOG=\"" << getIntbyBoolean(this->isSessionLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SCAN_DETECT_LOG=\"" << getIntbyBoolean(this->isScanDetectLog()) << "\"" << std::endl;
		std::cout << "LOGGER_DOS_DETECT_LOG=\"" << getIntbyBoolean(this->isDoSDetectLog()) << "\"" << std::endl;
		std::cout << "LOGGER_SESSION_LAYER7_LOG=\"";
		for(unsigned int i=0; i < session_layer7_log.size(); i++){
			if(i != 0){
				std::cout << " ";
			}
			
			std::cout << session_layer7_log.at(i);
		}	
		std::cout << "\"" << std::endl;

		return;

	}

	bool Logger::isBlockLog(){
		return this->block_log;
	}

	bool Logger::isSessionLog(){
		return this->session_log;
	}

	bool Logger::isScanDetectLog(){
		return this->scan_detect_log;
	}

	bool Logger::isDoSDetectLog(){
		return this->dos_detect_log;
	}

	bool Logger::isHTTPLog(){
		return this->http_log;
	}

	void Logger::setBlockLog(bool log){
		this->block_log = log;
	}


	void Logger::setSessionLog(bool log){
		this->session_log = log;
	}

	void Logger::setScanDetectLog(bool log){
		this->scan_detect_log = log;
	}

	void Logger::setDoSDetectLog(bool log){
		this->dos_detect_log = log;
	}

	void Logger::setHTTPLog(bool log){
		this->http_log = log;
	}

	void Logger::addSessionLayer7Log(std::string layer7){
	
		unsigned int i;
		for(i=0; i < session_layer7_log.size(); i++){
			if(session_layer7_log.at(i).compare(layer7) == 0)
				break;
		}
		if(i >= session_layer7_log.size() || (i < session_layer7_log.size() && session_layer7_log.at(i).compare(layer7) != 0)) {
				session_layer7_log.push_back(layer7);
		}
		
	}
    
	void Logger::setSessionLayer7Log(std::string layer7_protocols){
	
		std::vector<std::string> session_layer7_log_temp;
		split(layer7_protocols, &session_layer7_log_temp);
		for(unsigned int i=0; i < session_layer7_log_temp.size(); i++){
			addSessionLayer7Log(session_layer7_log_temp.at(i));
		}
		
		session_layer7_log_temp.clear();
		
	}

	/*  Split string of names separated by SPLIT_CHAR  */
	#define SPLIT_CHAR " "
	void Logger::split(std::string instr, std::vector<std::string> *lstoutstr) {

		std::string str = instr;
		unsigned int loc;
		//  Break string into substrings and store beginning
		while(1){
			loc = str.find(SPLIT_CHAR);
			if( loc == std::string::npos ){
				lstoutstr->push_back(str);
				return;
			}
			std::string substr = str.substr(0, loc);
			str.erase(0, loc + 1);
			lstoutstr->push_back(substr);
		}

	}

	int Logger::getIntbyBoolean(bool log){
		if(log) return 1;
		return 0;
	}
}

