#include "channel.h"

namespace maxnet
{

	Channel::Channel(){
		rate = "";
		parent_id = 0;
		share_id = 0;
		classid = 0;
		limit = 0;
		class_type = 0;
		weight = 0;
		attribute = 0;
		dir = 0;
		class_name = "";
		comment = "";
	}

	Channel::~Channel(){

	}

	void	Channel::output(){	
		std::cout << "# \"name=" << getClassName() << "\", classtype=\"" << getClassType() << "\", classid=\"" << getClassId()<< "\", dir=\"" << getDir() 
				<< "\", parent_id=\"" << getParentId()<< "\", share_id=\"" << getShareId()
				<< "\", rate=\"" << getRate()<< "\", attr=\"" << getAttribute()
				<< "\", share_id=\"" << getShareId()<< "\", parent_id=\"" <<getParentId()
				<< "\", prio=\"" <<  getPriority() << "\", limit=\"" << getLimit() << "\", weight=\"" << getWeight()
				<< "\", comment=\"" << getComment() <<  "\"\t\t// " << std::endl;
				
	}

	bool    Channel::isValid(){
		if(class_name.size() <= 0) return false;
		if(classid <= 0) return false;
		
		if(class_type == 2){
			if(classid <= 0) return false;
			if(share_id <= 0) return false;
			if(parent_id <= 0) return false;
		}

		return true;
	}

	void	Channel::setClassName(std::string class_name){
		this->class_name = class_name;
	}
	void	Channel::setRate(std::string rate){
		this->rate = rate;
	}
	void 	Channel::setClassId(unsigned int class_id){
		this->classid = class_id;
	}
	void	Channel::setParentId(unsigned int parent_id){
		this->parent_id = parent_id;
	}
	void	Channel::setShareId(unsigned int share_id){
		this->share_id = share_id;
	}
	void	Channel::setLimit(unsigned int limit){
		this->limit = limit;
	}
    void	Channel::setDir(unsigned int dir){
		this->dir = dir;
	}
    void	Channel::setPriority(unsigned int priority){
		this->priority = priority;
	}
	void	Channel::setWeight(unsigned int weight){
		this->weight = weight;
	}
	void	Channel::setAttribute(unsigned int attribute){
		this->attribute = attribute;
	}
	void	Channel::setClassType(unsigned int class_type){
		this->class_type = class_type;
	}
    void	Channel::setComment(std::string comment){
		this->comment = comment;
	}

	std::string		Channel::getClassName(){
		return class_name;
	}
	unsigned int	Channel::getClassId(){
		return classid;
	}
	unsigned int	Channel::getShareId(){
		return share_id;
	}
	unsigned int	Channel::getParentId(){
		return parent_id;
	}
	std::string		Channel::getRate(){
		return rate;
	}
	unsigned int	Channel::getLimit(){
		return limit;
	}
    unsigned int	Channel::getDir(){
		return dir;
	}
	unsigned int	Channel::getWeight(){
		return weight;
	}
	unsigned int	Channel::getPriority(){
		return priority;
	}
	unsigned int	Channel::getAttribute(){
		return attribute;
	}
	unsigned int 	Channel::getClassType(){
		return class_type;
	}
    std::string     Channel::getComment(){
		return comment;
	}


}
