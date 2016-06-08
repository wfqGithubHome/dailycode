#include "portscan.h"
#include "../global.h"

namespace maxnet{

	PortScan::PortScan(){

	}

	PortScan::~PortScan(){
		
	}
	
	bool PortScan::isVaild(){
		if(type.size() <= 0) return false;
		//if(portscans.size() <=0) return false;
		return true;
	}

	std::string PortScan::getCommand(){
		std::string command;
		command = "--portscans " + getPortScans();
		return command;
	}
    /*  12-09-26 add prot_scan */
     std::string PortScan::getType(){
             return type;
         }
    
     std::string PortScan::getNewer(){
          return newer;
      }
    std::string PortScan::getPending(){
             return pending;
         }
     std::string PortScan::getInterval(){
             return interval;
         }
    
     void PortScan::setType(std::string type){
          this->type = type;
      }
     void PortScan::setNewer(std::string newer){
             this->newer = newer;
         }
     void PortScan::setPending(std::string pending){
             this->pending = pending;
         }
     void PortScan::setInterval(std::string interval){
             this->interval = interval;
         }
    
 

	void PortScan::output(){
		std::cout << "# \"" << getType() << "\"\t\"" 
							<< getNewer() << "\"\t\"" << getPending() << "\"\t\t//" << getInterval() << std::endl;

		return;

	}



	void PortScan::setPortScans(std::string portscans){
		this->portscans = portscans;
	}


	std::string PortScan::getPortScans(){
		return portscans;
	}



}

