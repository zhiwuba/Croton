#ifndef  __CROTON_WEBSITE_H__
#define __CROTON_WEBSITE_H__ 
#include "spider_utils.h"
#include "spider_url.h"
#include "boost/xpressive/xpressive_dynamic.hpp"

//WebSite基类 保存基本信息
class Spider_WebSite
{
public:
	Spider_WebSite(){};
	~Spider_WebSite(){};
	virtual int initialize(const char* website_name, StrVec& starts,StrVec& index_regex, StrVec& pic_regex, IntPair& pic_size);
	virtual int begin_process();
	virtual int process(UrlPtr&  url_ptr)=0;

public:
	StrVec      m_starts;
	std::string m_website_name;
	IntPair      m_pic_size; 

	std::vector<boost::xpressive::cregex>      m_index_regex_vec; 
	std::vector<boost::xpressive::cregex>      m_pic_regex_vec;
};

//WebSite工厂类
class Spider_WebSite_Factory
{
public:
	static Spider_WebSite* create_website(std::string domain);
};

//对普通网站进行解析
class Spider_Website_General:public Spider_WebSite
{
public:
	virtual int process(UrlPtr&  url_ptr);
};

//对微博网站进行解析
class Spider_Website_Weibo:public Spider_WebSite
{
public:
	int Process(UrlPtr&  url_ptr);

private:
	int parse_start_page(UrlPtr& url);
	int get_pic_from_index(UrlPtr url, UrlPtrVec& url_array);
};

#endif
