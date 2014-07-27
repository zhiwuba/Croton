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
		m_starts=seeds; //赋值
	}
	else
	{
		return  -1;
	}
	return 0;
}


int Spider_WebSite::begin_process()
{
	for (int i=0; i<m_starts.size(); i++  )
	{
		std::string url_str=m_starts[i];
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
	if (domain.find("weibo")!=std::string::npos )
	{
		website=new Spider_Website_Weibo();
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
#define  PIC_COUNT_PER_PAGE     30

int Spider_Website_Weibo::Process(UrlPtr& url_ptr)
{
	int  ret=0;
	if ( url_ptr->type==UT_HTML )
	{
		StrVec urls;
		char* html=url_ptr->response;
		
		bool is_index_page=false;
		for (int i=0 ;i<m_index_regex_vec.size(); i++ )
		{
			if ( regex_match(url_ptr->url, m_index_regex_vec[i]) )
			{  //是索引页
				is_index_page=true;
				break;
			}
		}

		if( is_index_page )
		{   //从图片索引中解析出图片信息
			get_pic_from_index(url_ptr);
		}
		else
		{   //从开始页中解析出索引链接
			parse_start_page(url_ptr);
		}
	}
	else if ( url_ptr->type==UT_PICT )
	{
		if ( url_ptr->length>=m_pic_size.first&&url_ptr->length<=m_pic_size.second )
		{   //大小符合
			Spider_Storage::instance().write_file(m_website_name.c_str(), url_ptr);
		}
	}
	return ret;
}


int Spider_Website_Weibo::get_pic_from_index(UrlPtr url, UrlPtrVec& url_array)
{
	Json::Reader reader;
	Json::Value   albums_info;
	if ( reader.parse(url->response, albums_info ) )
	{
		if ( albums_info["result"].asBool() )
		{
			Json::Value data=albums_info["data"];
			Json::Value photo_list=data["photo_list"];
			for (unsigned int i=0; i<photo_list.size(); i++  )
			{
				Json::Value info=photo_list[i];
				if ( false==info.empty() )
				{
					std::string photo_url=info["pic_host"].asString()+"/mw690/"+info["pic_name"].asString();
					const char* caption=info["caption_render"].asCString();
					std::string ansi_cap=unicode_to_ansi(caption);
					
					for ( int i=0 ;i <m_pic_regex_vec.size(); i++ )
					{
						if ( regex_match(photo_url.c_str(), m_pic_regex_vec[i]) )
						{   //符合规则的图片
							UrlPtr new_url=create_url(url, UT_PICT);
							new_url->belong=this;
							Spider_Url_Rinse::instance().rinse_url(new_url);
							break;
						}
					}
				}
				else
				{ //TODO: 为什么有null
					LLOG(L_ERROR,"WeiboExplore::get_pic_from_index JsonValue is null.");
				}
			}
		}
	}
	else
	{
		LLOG2(L_ERROR,"WeiboExplore::GetAlbumsFromPage JsonPrase error..");
	}
	return 0;
}


int Spider_Website_Weibo::parse_start_page(UrlPtr& url)
{
	//筛选要读取的相册 拼装URL
	Json::Reader reader;
	Json::Value   info;
	if ( reader.parse(url->response, info) )
	{
		if ( info["result"].asBool() )
		{
			Json::Value data=info["data"];
			Json::Value album_list=data["album_list"];
			for(unsigned int i=0; i<album_list.size(); i++)
			{
				Json::Value album=album_list[i];
				Json::Value count=album["count"];
				if ( album["type"].asInt()==3 ) //微博配图 应该是内容比较丰富的吧
				{
					int cur_pos=0;
					int pic_count= count["photos"].asInt();
					int get_count=0;
					int page_num=1;
					do 
					{
						int left=pic_count-cur_pos;
						get_count=left>30?30:left;

						char url[1024];
						sprintf(url, "http://photo.weibo.com/photos/get_all?uid=%s&album_id=%s&count=%d&page=%d&type=%d",
							album["uid"].asCString(), album["album_id"].asCString(), get_count,  page_num , album["type"].asInt() );

						UrlPtr new_url=create_url(url, UT_HTML);
						new_url->belong=this;
						Spider_Url_Rinse::instance().rinse_url(new_url);

						cur_pos+=get_count;
						page_num++;
					} while ( cur_pos<pic_count );

					break;
				}
			}
		}
	}
	return 0;
}
