#ifndef _ZERO_NODEMGR_H
#define _ZERO_NODEMGR_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"

namespace maxnet{
	
#define MAX_BRIDGE_ID   5

	class NodeMgr {

		public:

			NodeMgr();
			~NodeMgr();
			void	addNode(Node * node);
			void	addRouteNode(Node * node);
			void	addGroup(std::string name, std::string comment);
			void	addSubNode(std::string group_name, std::string sub_name);
			void	addOnlySubNode(std::string group_name, std::string sub_name);

			Node *  find(std::string name);
			Node *  Onlyfind(std::string name);
			Node *  next();
			Node *  next(Node * ptr,  int index);
			int		getSubIndex();
			Node *  getCurrGroupNode();

			std::vector<Node *> getList();
			int		getCount();

			void Clear();
		protected:
			std::vector<Node *> node_list;
			Node *	group_node_ptr;
			int		sub_node_index;

		public:
			/*���ڱ�����MGR�м���ͨ�ö�����Ե���Ŀ*/
			int		groupNodeCount;		//group����ͳ��
			int		includeNodeCount;	//include����ͳ��
			int		dataNodeCount;		//���ݶ���ͳ��
	};

}

#endif // _ZERO_NODEMGR_H
