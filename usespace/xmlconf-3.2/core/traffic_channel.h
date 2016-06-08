#ifndef _TRAFFIC_CHANNEL_H
#define _TRAFFIC_CHANNEL_H

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <sys/types.h>
#include <opt.h>

#include "node.h"

#define MAX_PRIO_NUM 17

namespace maxnet{

	class TrafficChannel : public Node{

		public:
			// Construction and Destruction.
			TrafficChannel();
			~TrafficChannel();

			void			output();

			void			setInterface(std::string interface);
	
			void			setChild(bool has_child);
			void			setDefault(bool b);
			void			setPriority(unsigned int pri);
			void			setId(int id);
			
			bool			HasChild();
			bool			isDefault();

			//add new on 2009.10.26
			void 		setParentName(std::string parent_name);
			void			setShareName(std::string share_name);
                     void               setShareComment(std::string share_comment);
			void			setClassName(std::string class_name);
			void			setRate(unsigned int rate);
			void			setRateString(std::string);
			void 		setBandwidth(unsigned int bandwidth);
			void 		setClassId(int class_id);
			void			setParentId(int parent_id);
			void			setShareId(int share_id);
			void			setLimit(unsigned int limit);	
			void			setWeight(unsigned int weight);
			void			setAttribute(unsigned int attribute);
			void			setClassType(unsigned int class_type);
			//add end

			int				getId();
			int				getClassId();
			int				getShareId();
			int				getParentId();
			std::string		getInterface();
			std::string		getParentName();
			std::string		getShareName();
                     std::string             getShareComment();
			std::string		getClassName();
			unsigned int	getRate();
			std::string		getRateString();
			unsigned int    getBandwidth();
			unsigned int		getLimit();
			unsigned int		getWeight();
			unsigned int		getPriority();
			unsigned int		getAttribute();
			unsigned int 		getClassType();
		
		private:
			std::string interface;
			std::string parent_name;//add new
			std::string share_name;
                     std::string share_comment;
			std::string class_name;
			unsigned int  rate;
			std::string ratestring;
			unsigned int  bandwidth;
			int		     class_id;
			int               parent_id;
			int		    share_id;
			unsigned int limit;
			unsigned int priority;
			unsigned int class_type;
			unsigned int attribute;
			unsigned int weight;// add new
			bool	is_default;


			int		id;	
			bool	has_child;

	};
}


#endif // _TRAFFIC_CHANNEL_H
