#include "system.h"
#include <fcntl.h>
#include<unistd.h>
#include "../global.h"
#include <sstream>

namespace maxnet{

	System::System(){
		enableDBP = false;
		password2 = "";
	}

	System::~System(){
		
	}
	
	bool System::isVaild(){
		return true;
	}

	void System::output(){
		std::cout << "# system" << std::endl;
		std::cout << "SYSTEM_HOSTNAME=\"" << this->getHostName() << "\"" << std::endl;
		std::cout << "SYSTEM_DOMAINNAME=\"" << this->getDomainName() << "\"" << std::endl;
		std::cout << "SYSTEM_CONTACT=\"" << this->getContact() << "\"" << std::endl;
		std::cout << "SYSTEM_LOCATION=\"" << this->getLocation() << "\"" << std::endl;
		std::cout << "SYSTEM_PASSWORD='" << this->getPassword() << "'" << std::endl;
		std::cout << "SYSTEM_PASSWORD2='" << this->getPassword2() << "'" << std::endl;
		std::cout << "SYSTEM_MTU='" << this->getMTUNum() << "'" << std::endl;
		std::cout << "SYSTEM_TIMEZONE='" << this->getTimezone() << "'" << std::endl;

		std::cout << "SYSTEM_MAX_BRIDGENUM=" << this->getmax_bridgenum() << std::endl;
		std::cout << "SYSTEM_BRIDGENUM=" << this->getbridgenum() << std::endl;

		if(this->isSTPenable()){
			std::cout << "SYSTEM_STP='1'" << std::endl;
		}
		else{
			std::cout << "SYSTEM_STP='0'" << std::endl;
		}
#ifdef CONFIG_IXP
        if(this->isFAutoUpdateEnable()){
			std::cout << "SYSTEM_FIRMWARE_AUTO_UPDATE='1'" << std::endl;
        }
		if(this->isMAutoUpdateEnable()){
			std::cout << "SYSTEM_MODULE_AUTO_UPDATE='1'" << std::endl;
        }
#else
		if(this->isAutoUpdateEnable()){
			std::cout << "SYSTEM_AUTO_UPDATE='1'" << std::endl;
		}
		else{
			std::cout << "SYSTEM_AUTO_UPDATE='0'" << std::endl;
		}
#endif
		std::cout << "SYSTEM_UPDATE_URL=\"" << this->getUpdateURL() << "\"" << std::endl;

		if(this->isNTPenable()){
			std::cout << "SYSTEM_NTP_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "SYSTEM_NTP_ENABLE='0'" << std::endl;
		}
		if(this->isDBP()){
			std::cout << "SYSTEM_DBRIDGE_PERF='1'" << std::endl;
		}
		else{
			std::cout << "SYSTEM_DBRIDGE_PERF='0'" << std::endl;
		}
		std::cout << "SYSTEM_NTP_SERVER=\"" << this->getNTPServer() << "\"" << std::endl;
		if(this->isSNMPenable() && (this->getSNMPRO().size() > 0 || this->getSNMPRW().size() > 0)){
			std::cout << "SYSTEM_SNMP_ENABLE='1'" << std::endl;
		}
		else{
			std::cout << "SYSTEM_SNMP_ENABLE='0'" << std::endl;
		}
		std::cout << "SYSTEM_SNMP_RO_COMMUNITY='" << this->getSNMPRO() << "'" << std::endl;
		std::cout << "SYSTEM_SNMP_RW_COMMUNITY='" << this->getSNMPRW() << "'" << std::endl;
		std::cout << "SYSTEM_UMOUNT_INTERVAL_TIME='600'" << std::endl;
		return;

	}


	void System::setSNMPenable(bool enable)
	{
		this->enableSNMP = enable;
	}

	void System::setSNMPRO(std::string com)
	{
		this->SNMPRO = com;
	}

	void System::setSNMPRW(std::string com)
	{
		this->SNMPRW = com;
	}

	void System::setMTU(std::string mtu)
	{
		this->mtu = mtu;
	}

	void System::setEnableSTP(bool enable){
		enableSTP = enable;
	}
	
	void System::setDBP(bool enable){
		enableDBP = enable;
	}
#ifdef CONFIG_IXP
    void System::setEnableFAutoUpdate(bool enable){
		enableFAutoUpdate = enable;
	}
	void System::setEnableMAutoUpdate(bool enable){
		enableMAutoUpdate = enable;
	}
#else
	void System::setEnableAutoUpdate(bool enable){
		enableAutoUpdate = enable;
	}
#endif	
	void System::setUpdateURL(std::string url){
		updateURL = url;
	}


	void System::setHostName(std::string hostname){
		this->hostname = hostname;
	}

	void System::setDomainName(std::string domainname){
		this->domainname = domainname;
	}

	void System::setContact(std::string contact){
		this->contact = contact;
	}

	void System::setLocation(std::string location){
		this->location = location;
	}

	void System::setPassword(std::string password){
		this->password = password;
	}

	void System::setPassword2(std::string password2){
		this->password2 = password2;
	}

	void System::setmax_bridgenum(std::string max_bridgenum){
		this->max_bridgenum = max_bridgenum;
	}

	void System::setbridgenum(std::string bridgenum){
		this->bridgenum = bridgenum;
	}


	std::string System::getHostName(){
		return hostname;
	}

	std::string System::getDomainName(){
		return domainname;

	}
	std::string	System::getContact(){
		return contact;
	}
	std::string System::getLocation(){
		return location;
	}
	
	std::string System::getMTU()
	{
		return mtu;
	}

	int System::getMTUNum()
	{
		std::stringstream streamMTU;
		streamMTU << mtu;
		int  nMTU = 0;
		streamMTU >> nMTU;
		if(nMTU < 512 || nMTU > MAX_MTU)
			return 1500;	
		return nMTU;
	}

	std::string System::getPassword(){
		return password;
	}

	std::string System::getPassword2(){
		return password2;
	}

	bool		System::isSNMPenable()
	{
		return enableSNMP;
	}

    std::string System::getmax_bridgenum(){
		return max_bridgenum;
	}

	std::string System::getbridgenum(){
		return bridgenum;
	}

	std::string	System::getSNMPRO()
	{
		return SNMPRO;
	}

	std::string	System::getSNMPRW()
	{
		return SNMPRW;
	}

	bool		System::isSTPenable(){
		return enableSTP;
	}
#ifdef CONFIG_IXP
    bool        System::isFAutoUpdateEnable(){
        return enableFAutoUpdate;
    }
    bool        System::isMAutoUpdateEnable(){
		return enableMAutoUpdate;
    }
#else
	bool		System::isAutoUpdateEnable(){
		return enableAutoUpdate;
	}
#endif
	std::string	System::getUpdateURL(){
		return updateURL;
	}

	std::string System::getTimezone(){
		return time_zone;
	}

	void System::setTimezone(std::string tz){
		time_zone = tz;
	}

	std::string System::getNTPServer(){
		return ntp_server;
	}

	void System::setNTPServer(std::string server){
		ntp_server = server;
	}

	bool System::isNTPenable(){
		return enableNTP;
	}

	bool System::isDBP(){
		return enableDBP;
	}
	
	void System::setEnableNTP(bool enable){
		enableNTP = enable;
	}
	
	/* Add by:chenhao*/
	/* Date:2013-8-7 16:09:15 */
	/* Modifi by chenhao , Date:2013-9-17 19:35:39*/
	bool	System::checkLinkState(const char *interfaceName){
			int type,sign = 0;
			char cmd[512] = {0};
			snprintf(cmd,sizeof(cmd),"/usr/local/httpd/htdocs/cgi-bin/sh/route_script.sh list ");
			
			if (strncmp(interfaceName, "WAN",3) == 0 || strncmp(interfaceName, "LAN",3) == 0)
			{
				type = 1;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd)," link %s",interfaceName);
			}
			else if (strncmp(interfaceName, "ads",3) == 0)
			{
				type = 2;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd)," adsl %s",interfaceName);
			}
			else if (strncmp(interfaceName, "l2t",3) == 0)
			{
				type = 3;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd), " l2tpvpn %s", interfaceName);
			}
			else if (strncmp(interfaceName, "vla",3) == 0)
			{
				type = 4;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd), " vlan %s", interfaceName);
			}
			else if (strncmp(interfaceName, "bal",3) == 0)
			{
				type = 5;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd), " balance %s", interfaceName);
			}
			else if (strncmp(interfaceName, "vet",3) == 0)
			{
				type = 6;
				snprintf(cmd + strlen(cmd),sizeof(cmd) - strlen(cmd), " vlan %s", interfaceName);
			}
			else
			{
				return false;
			}
			
			char buffer[100]={0};
			FILE *pp = NULL;
			pp=popen(cmd,"r");
			if (pp != NULL){
				fgets(buffer, 100, pp);
				buffer[strlen(buffer)-1] = 0;
				pclose(pp);

				switch(type)
				{
					case 1:
						if(strncmp(buffer, "0",1) == 0)
						{
							sign = 1;
						}
						break;
					case 2:
						if(strncmp(buffer,"yes",3) == 0)
						{
							sign = 1;
						}
						break;
					case 3:
						if(strncmp(buffer,"yes",3) == 0)
						{
							sign = 1;
						}
						break;
					case 4:
						if(strncmp(buffer,"yes",3) == 0)
						{
							sign = 1;
						}
						break;
					case 5:
						if(strncmp(buffer, "0",1) == 0)
						{
							sign = 1;
						}
						break;
					case 6:
						if(strncmp(buffer,"yes",3) == 0)
						{
							sign = 1;
						}
					default:
						break;
				}

				if(sign)
				{
					return true;
				}
				return false;
			}
			
			return false;
	}
}

