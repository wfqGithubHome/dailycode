#ifndef _ZERO_PROTOCOL_H
#define _ZERO_PROTOCOL_H

#include <iostream>
#include <string>
#include <vector>

#include "node.h"


namespace maxnet{


	class Protocol : public Node{

		public:

			Protocol();
			~Protocol();
			bool isVaild();
			void output();
			std::string getCommand();

            void setId(int id);
			void setProto(std::string proto);
			void setSPort(std::string sport);
			void setDPort(std::string dport);
			void setMatch(std::string match);
			void setType(std::string type);
			void setOption(std::string option);

            int getId();
			std::string getProto();
			std::string getSPort();
			std::string getDPort();
			std::string getMatch();
			std::string getType();
			std::string getOption();

		private:
            int id;
			std::string name;
			std::string protocol;
			std::string sport;
			std::string dport;
			std::string match;
			std::string type;
			std::string option;

	};

}

#endif // _ZERO_PROTOCOL_H
