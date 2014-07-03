#ifndef  __CROTON_WEBSITE_H__
#define __CROTON_WEBSITE_H__ 
#include "spider_utils.h"
#include "spider_url.h"
#include "boost/xpressive/xpressive_dynamic.hpp"

class Spider_WebSite
{
public:
	Spider_WebSite();
	~Spider_WebSite();
	virtual int initialize(const char* website_name, StrVec& seeds,StrVec& index_regex, StrVec& pic_regex, IntPair& pic_size);
	virtual int begin_process();
	virtual int process(UrlPtr&  url_ptr)=0;

public:
	StrVec      m_seeds;
	std::string m_website_name;
	IntPair      m_pic_size; 

	std::vector<boost::xpressive::cregex>      m_index_regex_vec; 
	std::vector<boost::xpressive::cregex>      m_pic_regex_vec;
};

class Spider_WebSite_Factory
{
public:
	static Spider_WebSite* create_website(std::string domain);
};


#endif
