#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <sys/types.h>
#include <opt.h>

#include "node.h"

namespace maxnet{

	class Channel : public Node{

		public:
			Channel();
			~Channel();

			void			output();
            bool            isValid();
            
			void			setClassName(std::string class_name);
			void			setRate(std::string rate);
			void 		    setClassId(unsigned int class_id);
			void			setParentId(unsigned int parent_id);
			void			setShareId(unsigned int share_id);
			void			setLimit(unsigned int limit);	
            void			setDir(unsigned int dir);	
            void			setPriority(unsigned int priority);	
			void			setWeight(unsigned int weight);
			void			setAttribute(unsigned int attribute);
			void			setClassType(unsigned int class_type);
            void			setComment(std::string comment);

            std::string     getClassName();
            std::string	    getRate();
			unsigned int	getClassId();
			unsigned int	getShareId();
			unsigned int	getParentId();			
			unsigned int	getLimit();
            unsigned int	getDir();
			unsigned int	getWeight();
			unsigned int	getPriority();
			unsigned int	getAttribute();
			unsigned int 	getClassType();
            std::string     getComment();
		
		private:
			std::string  class_name;
			std::string  rate;
			unsigned int parent_id;
			unsigned int share_id;
            unsigned int classid;
			unsigned int limit;
			unsigned int priority;
			unsigned int class_type;
            unsigned int dir;
			unsigned int attribute;
			unsigned int weight;
            std::string  comment;
	};
}

#endif // __CHANNEL_H__
