#ifndef _ZERO_LOGGER_H
#define _ZERO_LOGGER_H

#include <iostream>
#include <string>
#include <vector>

namespace maxnet{


	class Logger {

		public:

			Logger();
			~Logger();
			bool isVaild();
			void output();

			bool	isBlockLog();
			bool	isHTTPLog();
			bool	isSessionLog();
			bool	isScanDetectLog();
			bool	isDoSDetectLog();

			void	setBlockLog(bool log);
			void	setSessionLog(bool log);
			void	setScanDetectLog(bool log);
			void	setDoSDetectLog(bool log);
			void	setHTTPLog(bool log);
			void	setSessionLayer7Log(std::string layer7_protocols);
			void	addSessionLayer7Log(std::string layer7);
            int     getIntbyBoolean(bool log);
            std::vector<std::string> session_layer7_log;
            
		private:
			bool	block_log;
			bool	session_log;
			bool	http_log;

			bool	scan_detect_log;
			bool	dos_detect_log;
			void	split(std::string instr, std::vector<std::string> *outs);
			
	};

}

#endif // _ZERO_LOGGER_H
