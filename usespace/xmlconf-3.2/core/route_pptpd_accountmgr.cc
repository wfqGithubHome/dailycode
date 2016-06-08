#include <sstream>
#include "route_pptpd_accountmgr.h"
#include "../global.h"

namespace maxnet{

	PptpdAccountMgr::PptpdAccountMgr(){

	}

	PptpdAccountMgr::~PptpdAccountMgr(){
		PptpdAccount * account = NULL;
		for(unsigned int i=0; i < node_list.size(); i++){
			account = (PptpdAccount *)node_list.at(i);
			delete account;
		}
		node_list.clear();
	}

	void PptpdAccountMgr::output(){

	}

	void PptpdAccountMgr::add(std::string username, std::string passwd, std::string ip){
		PptpdAccount *account = new PptpdAccount();
		account->setusername(username);
		account->setpasswd(passwd);
		account->setip(ip);

		if(account->isValid()){
			addNode(account);
		}
		else{
			delete account;
		}
	}

}

