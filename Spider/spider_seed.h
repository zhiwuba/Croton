#ifndef   __CROTON_SPIDER_SEED_H__
#define   __CROTON_SPIDER_SEED_H__
#include "spider_utils.h"

struct Seed
{
	StrVec url_;
	StrVec index_;
	StrVec pic_;
	IntPair pic_size_; //minֵ
};

class Spider_Seed
{
public:
	Spider_Seed();
	~Spider_Seed();
	
	int load();
	int get_seed(std::string& website, Seed** seed );
	
private:
	int prase_subject(char* line, char* subject);
	int prase_key_value(char* line, std::string& key, std::string& value);
	int set_seed(std::string& website, std::string& key, std::string& value );

	std::map<std::string,Seed*> m_seeds;
	
};


#endif

