#ifndef _ZERO_NODE_H
#define _ZERO_NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace maxnet{


	class Node {

		public:

			Node();
			~Node();
			void setName(std::string name);
			void setComment(std::string comment);

			std::string getName();
			std::string getComment();




			void addSubNode(std::string name);
			bool isGroup();

			void setGroup(bool isgroup);
			
			std::vector<std::string> getSubNodes();

			void setDisabled(int disabled);		
			int getDisabled();
			
		protected:
			std::string	name;
			std::string comment;
			bool isgroup;
			std::vector<std::string> subnode;
			
			int disabled;

          


	};

}

#endif // _ZERO_NODE_H
