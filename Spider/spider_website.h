#ifndef  __CROTON_WEBSITE_H__
#define __CROTON_WEBSITE_H__ 
#include "spider_utils.h"
#include "spider_url.h"
#include "boost/xpressive/xpressive_dynamic.hpp"



//WebSite���� ���������Ϣ
class Spider_WebSite
{
public:
	Spider_WebSite(){};
	~Spider_WebSite(){};
	virtual int initialize(const char* website_name, Seed* seed);
	virtual int begin_process();
	virtual int process(UrlPtr&  url_ptr)=0;

public:
	Seed* m_seed;
	std::string m_website_name;
};

//WebSite������
class Spider_WebSite_Factory
{
public:
	static Spider_WebSite* create_website(std::string domain);
};

//����ͨ��վ���н���
class Spider_Website_General:public Spider_WebSite
{
public:
	virtual int process(UrlPtr&  url_ptr);
};

//��΢����վ���н���
class Spider_Website_Weibo:public Spider_WebSite
{
public:
	virtual int process(UrlPtr&  url_ptr);

private:
	int parse_start_page(UrlPtr& url);
	int get_pic_from_index(UrlPtr& url);
};

#endif
