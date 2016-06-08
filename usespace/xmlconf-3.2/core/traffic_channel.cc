#include "traffic_channel.h"
#include "traffic_util.h"


namespace maxnet
{

	TrafficChannel::TrafficChannel(){
		priority = 3;
	}

	TrafficChannel::~TrafficChannel(){

	}



	void	TrafficChannel::output(){	
		std::cout << "# " << getInterface() << " - [" << getClassId() << ":" << getParentId()<< ":" << getShareId() 
				<<  "] \""
				<< " class=\"" << getClassName()<< "\", dev=\"" << getInterface()
				<< "\", type=\"" << getClassType()<< "\", class_id=\"" <<getClassId()
				<< "\", share_id=\"" << getShareId()<< "\", parent_id=\"" <<getParentId()
				<< "\", maxrate=\"" <<  TrafficUtil::printRate(getRate()) << "\", prio=\"" << getPriority()<< "\", attr=\"" << getAttribute()
				<< "\", limit=\"" <<getLimit()<< "\",weight=\"" <<getWeight()<<"\",default=\"" <<isDefault()<< "\"\t\t// " << getComment() << std::endl;
				
	}

//set
	void	TrafficChannel::setDefault(bool b){
		this->is_default = b;
	}

	
	bool	TrafficChannel::isDefault(){
		return this->is_default;
	}


	void	TrafficChannel::setPriority(unsigned int priority){
		//std::cout << "setPriority " <<std::endl;
		if(priority >= 0 && priority < MAX_PRIO_NUM){
			this->priority = priority;
		}
	}


	void	TrafficChannel::setId(int id){
		this->id = id;
	}

	int		TrafficChannel::getId(){
		return id;
	}

	void	TrafficChannel::setInterface(std::string interface){
		this->interface = interface;
	}

	void	TrafficChannel::setParentName(std::string parent_name){
		this->parent_name= parent_name;
	}

	void	TrafficChannel::setShareName(std::string share_name){
		this->share_name = share_name;
	}

	void TrafficChannel::setShareComment(std::string share_comment){
		this->share_comment = share_comment;
	}

	void	TrafficChannel::setClassName(std::string class_name){
		this->class_name = class_name;
	}
	void	TrafficChannel::setRate(unsigned int rate){
		this->rate = rate;
	}
	void	TrafficChannel::setRateString(std::string ratestr){
		this->ratestring = ratestr;
	}

	void	TrafficChannel::setClassId(int class_id){
		this->class_id = class_id;
	}

	void	TrafficChannel::setParentId(int parent_id){
		this->parent_id = parent_id;
	}
	
	void	TrafficChannel::setShareId(int share_id){
		this->share_id = share_id;
	}
	


	void	TrafficChannel::setBandwidth(unsigned int bandwidth){
		this->bandwidth = bandwidth;
	}
/*
	void	TrafficChannel::setPerLimit(int	per_limit){
		if(per_limit <= 0)
			this->per_limit = 0;
		else if(per_limit > 51200)
			this->per_limit = 51200;
		else
			this->per_limit = per_limit;
	}
*/
	void	TrafficChannel::setClassType(unsigned int class_type){
		this->class_type= class_type;
	}

	void	TrafficChannel::setAttribute(unsigned int attribute){
		this->attribute = attribute;
	}
	
	void	TrafficChannel::setWeight(unsigned int weight){
		this->weight = weight;
	}

	
	void	TrafficChannel::setLimit(unsigned int limit){
		this->limit = limit;
	}
	
	void	TrafficChannel::setChild(bool has_child){
		this->has_child = has_child;
	}
//get
	
	std::string	TrafficChannel::getInterface(){
		return interface;
	}

	std::string	TrafficChannel::getParentName(){
		return parent_name;
	}

	std::string	TrafficChannel::getShareName(){
		return share_name;
	}

	std::string	TrafficChannel::getShareComment(){
		return share_comment;
	}

	std::string	TrafficChannel::getClassName(){
		return class_name;
	}
	
	int			TrafficChannel::getClassId(){
		return class_id;
	}

	int			TrafficChannel::getParentId(){
		return parent_id;
	}

	int			TrafficChannel::getShareId(){
		return share_id;
	}
	
	unsigned int TrafficChannel::getRate(){
		return rate;
	}
	std::string TrafficChannel::getRateString(){
		return this->ratestring;
	}

	unsigned int		TrafficChannel::getBandwidth(){
		return bandwidth;
	}

	unsigned int		TrafficChannel::getPriority(){
		return this->priority;
	}

	unsigned int		TrafficChannel::getLimit(){
		return limit;
	}

	unsigned int		TrafficChannel::getWeight(){
		return weight;
	}

	unsigned int		TrafficChannel::getAttribute(){
		return attribute;
	}

	unsigned int		TrafficChannel::getClassType(){
		return class_type;
	}

	bool		TrafficChannel::HasChild(){
		return has_child;
	}
}
