#ifndef   __CROTON_SPIDER_SEED_H__
#define   __CROTON_SPIDER_SEED_H__
#include "spider_utils.h"

class Seed
{
public:
	void set_pic_size(const char* str);

	StrVec start_url_;
	StrVec index_url_;
	StrVec pic_url_;
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
	std::map<std::string,Seed*> m_seeds;
};


#endif

