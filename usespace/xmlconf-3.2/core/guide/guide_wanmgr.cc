#include "guide_wan.h"
#include "guide_wanmgr.h"

namespace maxnet
{
	WanMgr::WanMgr()
    {

	}

	WanMgr::~WanMgr()
    {
#if 1
		Wan * if_wan = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			if_wan = (Wan *)node_list.at(i);
			if(!if_wan->isGroup())
				delete if_wan;
		}
		node_list.clear();
#endif
		return;
	}

	void WanMgr::add(std::string ifname, std::string isp, std::string quality, std::string lineid,   
                       std::string upload, std::string download, std::string mac, std::string ipaddr,
				       std::string netmask, std::string gateway, std::string conntype)
	{

		Wan * if_wan = new Wan();
		if_wan->setIfname(ifname);
		if_wan->setISP(isp);
		if_wan->setQuality(quality);
		if_wan->setLineid(lineid);
		if_wan->setUpload(upload);
		if_wan->setDownload(download);
		if_wan->setMAC(mac);
		if_wan->setIPaddr(ipaddr);
		if_wan->setNetmask(netmask);
		if_wan->setGateway(gateway);
        if_wan->setConntype(conntype);
		
		if(if_wan->isVaild())
        {
			addNode(if_wan);
		}
		else
        {
			delete if_wan;
		}
        
		return;
	}

}

