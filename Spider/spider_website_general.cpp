#include "spider_website_general.h"
#include "spider_storage.h"
#include "spider_url_rinse.h"
#include "boost/xpressive/xpressive_dynamic.hpp"



Spider_Website_General::Spider_Website_General()
{

}

Spider_Website_General::~Spider_Website_General()
{

}


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



