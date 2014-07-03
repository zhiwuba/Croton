#include "spider_website.h"
#include "spider_url_rinse.h"
#include "spider_website_general.h"

Spider_WebSite::Spider_WebSite()
{

}


Spider_WebSite::~Spider_WebSite()
{

}


int Spider_WebSite::initialize(const char* website_name, StrVec& seeds,StrVec& index_regex, StrVec& pic_regex, IntPair& pic_size)
{
	if ( website_name!=NULL )
	{
		m_website_name.assign(website_name);

		for ( int i =0 ;i< index_regex.size(); i++ )
		{
			m_index_regex_vec.push_back(boost::xpressive::cregex::compile(index_regex[i]));
		}
		for ( int i=0; i<pic_regex.size(); i++)
		{
			m_pic_regex_vec.push_back(boost::xpressive::cregex::compile(pic_regex[i]));
		}
		m_pic_size=pic_size;
		m_seeds=seeds; //¸³Öµ
	}
	else
	{
		return  -1;
	}
	return 0;
}


int Spider_WebSite::begin_process()
{
	for (int i=0; i<m_seeds.size(); i++  )
	{
		std::string url_str=m_seeds[i];
		UrlPtr url=create_url(url_str.c_str(),UT_HTML);
		url->belong=(void*)this;
		Spider_Url_Rinse::instance().rinse_url(url);
	}
	return 0;
}

Spider_WebSite* Spider_WebSite_Factory::create_website(std::string domain)
{
	Spider_WebSite* website=NULL;
	if ( domain.find("renren")!=std::string::npos )
	{

	}
	else if (domain.find("weibo")!=std::string::npos )
	{

	}
	else
	{
		website=new Spider_Website_General();
	}
	return website;
}




