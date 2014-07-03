#ifndef  __CROTON_SPIDER_WEBSITE_GENERAL_H__
#define __CROTON_SPIDER_WEBSITE_GENERAL_H__

#include "spider_website.h"




class Spider_Website_General:public Spider_WebSite
{
public:
	Spider_Website_General();
	~Spider_Website_General();

	virtual int process(UrlPtr&  url_ptr);

private:
	

};



#endif
