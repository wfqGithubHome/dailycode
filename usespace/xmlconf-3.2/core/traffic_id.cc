#include "traffic_id.h"
#include <iostream>

namespace maxnet
{

	TrafficId::TrafficId(int nLen, std::string strInterface){
		m_nLen = nLen;	
		buffer_share = new char[nLen];
		buffer_pipe   = new char[nLen];
		//buffer_vc      = new char[nLen];
		memset(buffer_share, 0, nLen);
		memset(buffer_pipe, 0, nLen);
		//memset(buffer_vc, 0, nLen);

		m_strInterface = strInterface;
	}

	TrafficId::TrafficId(){

	}

	TrafficId::~TrafficId(){
		delete[] buffer_share;
		delete[] buffer_pipe;
		vecPipeMaxID.clear();
		//delete[] buffer_vc;
	}

	std::string TrafficId::getInterface(){
		return m_strInterface;
	}

/*
	int TrafficId::getNextId(){
		//		std::cout<<__FUNCTION__<<"beginning"<<std::endl;
		//return -1;
		char * ptr = buffer;
		for(int i = 0; i < m_nLen; i++){
			//std::cout<<"*ptr = "<<*ptr<<std::endl;
			if(*ptr == '\0'){
				*ptr = '-';
		//			std::cout<<__FUNCTION__<<"**end"<<std::endl;
				return i+1;
			}		
			ptr++;
		}
		//	std::cout<<__FUNCTION__<<"&&end"<<std::endl;
		return -1;
	}

*/
	int TrafficId::getNextId_share(){
		char * ptr = buffer_share;
		for(int i = SHARE_START_ID; i < (m_nLen+SHARE_START_ID); i++){
			if(*ptr == '\0'){
				*ptr = '-';
				return i;
			}		
			ptr++;
		}
		return -1;
	}

	int TrafficId::getNextId_pipe(){
		char * ptr = buffer_pipe;
		for(int i = 0; i < PIPE_MAX_SIZE;  i = i+256 ){
			if(*ptr == '\0'){
				*ptr = '-';
				return i;
			}		
			ptr++;
		}
		return -1;
	}

	int TrafficId::getNextId_vc(int parent_id,int status){
		/*char * ptr = buffer_vc;
		if(status){
			memset(buffer_vc,0,256);
		}
		for(int i = (1+parent_id); i <( m_nLen+parent_id); i++){
			if(*ptr == '\0'){
				*ptr = '-';
				return i;
			}		
			ptr++;
		}*/
		int id = 0;
		
		for(size_t i= 0; i< vecPipeMaxID.size(); ++i){
			if(vecPipeMaxID[i].PipeID == parent_id){
				++vecPipeMaxID[i].MaxChannelID;
				id = vecPipeMaxID[i].MaxChannelID;
			}
		}
		
		if(id == 0){
			trafficPipeMaxID tmp ;
			tmp.PipeID = parent_id;
			id = tmp.MaxChannelID = parent_id +1;
			vecPipeMaxID.push_back(tmp);
		}
		
		if(id%256 < 256 )
			return id;
		
		return -1;
	}
/*	
	void TrafficId::removeId(int nId){
		if(nId >= 0 && nId < m_nLen){
			char *ptr = buffer;
			ptr = ptr + nId -1;

			*ptr = '\0';

		}

	}
*/	

}
