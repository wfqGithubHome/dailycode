#ifndef _ZERO_PORTSCAN_H
#define _ZERO_PORTSCAN_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class PortScan : public Node{

		public:

			PortScan();
			~PortScan();
			bool isVaild();
			std::string getCommand();

			void output();

			void setPortScans(std::string Ports);
			std::string getPortScans();

        
        /* 2012-09-26 add */
        std::string getType();
        std::string getNewer();
        std::string getPending();
        std::string getInterval();
        
        
        
        void setType( std::string type);
        void setNewer( std::string newer);
        void setPending( std::string pending);
        void setInterval(std::string interval);
       
		private:
            std::string portscans;
            std::string type;
            std::string newer;
            std::string pending;
            std::string interval;



            
	};

}

#endif // _ZERO_PORT_SCAN_H

