#include "spider_website.h"
#include "spider_url_rinse.h"
#include "spider_storage.h"

/////////////////////////////////////////////////
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
		m_seeds=seeds; //赋值
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

/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////////
int Spider_Website_General::process(UrlPtr&  url_ptr)
{
	if ( url_ptr->type==UT_HTML )
	{
		StrVec urls;
		char* html=url_ptr->response;
		get_all_url( url_ptr->url , html, 0, strlen(html) ,urls);

		for (int i=0; i<urls.size(); i++)
		{
			bool bcont=false;
			std::string url=urls[i];
			for (int i=0 ;i<m_index_regex_vec.size(); i++ )
			{
				if ( regex_match(url.c_str(), m_index_regex_vec[i]) )
				{ //索引文件
					UrlPtr new_url=create_url(url, UT_HTML);
					new_url->belong=this;
					Spider_Url_Rinse::instance().rinse_url(new_url);
					goto goon;
				}
			}
			for ( int i=0 ;i <m_pic_regex_vec.size(); i++ )
			{
				if ( regex_match(url.c_str(), m_pic_regex_vec[i]) )
				{//图片文件
					UrlPtr new_url=create_url(url, UT_PICT);
					new_url->belong=this;
					Spider_Url_Rinse::instance().rinse_url(new_url);
					goto goon;
				}
			}
goon:
			continue;
		}
	}
	else if ( url_ptr->type==UT_PICT )
	{
		if ( url_ptr->length>=m_pic_size.first&&url_ptr->length<=m_pic_size.second )
		{   //大小符合
			Spider_Storage::instance().write_file(m_website_name.c_str(), url_ptr);
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////







