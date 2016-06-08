#include "portscan.h"
#include "portscanmgr.h"
#include "../global.h"

namespace maxnet{

	PortScanMgr::PortScanMgr(){

	}

	PortScanMgr::~PortScanMgr(){
#if 1
		PortScan * portscan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			portscan = (PortScan *)node_list.at(i);
			if(!portscan->isGroup())
				delete portscan;
		}
		node_list.clear();
#endif		
		return;
	}

   

    void PortScanMgr::add(std::string type, std::string newer, std::string pending,std::string interval){
		PortScan* portscan = new PortScan();
		portscan->setType(type);
		portscan->setNewer(newer);
		portscan->setPending(pending);
		portscan->setInterval(interval);
       
        
        if(portscan->isVaild()){
            addNode(portscan);
		}
		else{
			delete portscan;
		}
     

        
		return;
	}

	void PortScanMgr::output(){
		PortScan * portscan = NULL;
		std::cout  << "# ports " << std::endl;

		for(unsigned int i=0; i < node_list.size(); i++){
			portscan = (PortScan *)node_list.at(i);
			portscan->output();
		}
		

		
		return;

	}
}
