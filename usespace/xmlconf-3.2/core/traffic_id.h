#ifndef _TRAFFIC_ID_
#define _TRAFFIC_ID_

#include <string>
#include <sys/types.h>
#include <opt.h>
#include <vector>



namespace maxnet{
	struct trafficPipeMaxID{
		int PipeID;
		int MaxChannelID;
	};

	class TrafficId
    {
		public:
		// Construction and Destruction.
		TrafficId(int nLen, std::string strMask);
		~TrafficId();

		//int getNextId();
		int getNextId_share();
		int getNextId_pipe();
		int getNextId_vc(int parent_id,int status);
		std::string getInterface();
		void removeId(int nId);

		private:
			char * buffer_share;
			char * buffer_pipe;
			//char * buffer_vc;
			int m_nLen;
			int parent_id;
			std::string m_strInterface;
	
			std::vector<trafficPipeMaxID> vecPipeMaxID;

			TrafficId();
	};
}

#define  SHARE_START_ID    65280
#define  PIPE_START_ID	    0
#define  PIPE_MAX_SIZE      65279


#endif
