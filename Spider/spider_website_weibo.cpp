#include "json/json.h"
#include "Albums.h"
#include "AlbumsCollections.h"
#include "URLManager.h"
#include "WeiboExplore.h"

#define  PIC_COUNT_PER_PAGE     30

WeiboExplore::WeiboExplore()
{

}

WeiboExplore::~WeiboExplore()
{

}

int WeiboExplore::Process(int sock, URL_PTR url)
{
	int  ret=0;
	int status_code;
	std::string recvbody;
	switch ( url->type )
	{
	case WEIBO_ALBUMS:
		{
			status_code=RecvResponse(sock,recvbody);
			if (status_code==200)
			{
				Albums* albums=GetAlbumsFromPage(url, recvbody);
				if ( albums!=NULL )
				{
					AlbumsCollections::Instance().AddAlbums(albums);
					URLARRAY  url_array;
					albums->GetNewUrlArray(url_array);
					URLManager::Instance().PushUrl(url_array);
				}
			}
			else
			{
				LLOG(L_ERROR,"WeiboExplore::Process  recv %s error.", url->url);
				ret=-1;
			}
		}
		break;
	case WEIBO_ALBUMS_INFO:
		{
			status_code=RecvResponse(sock,recvbody);
			if ( status_code==200 )
			{
				URLARRAY url_array=GetAlbumsUrlFromPage(url, recvbody);

				URLManager::Instance().PushUrl(url_array);
			}
			else
			{
				LLOG(L_ERROR,"WeiboExplore::Process  recv %s error.", url->url);
				ret=-1;
			}
		}
		break;
	case WEIBO_INDEX:
		{
			status_code=RecvResponse(sock, recvbody);
			if ( status_code==200 )
			{
				URLARRAY url_array=GetUserIndexFromPage(recvbody);
				URLManager::Instance().PushUrl(url_array);
			}
			else
			{
				LLOG(L_ERROR,"WeiboExplore::Process  recv %s error.", url->url);
				ret=-1;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}


Albums* WeiboExplore::GetAlbumsFromPage(URL_PTR url, std::string& content)
{
	Albums* albums=NULL;
	Json::Reader reader;
	Json::Value   albums_info;
	if ( reader.parse(content, albums_info ) )
	{
		if ( albums_info["result"].asBool() )
		{
			albums=AlbumsCollections::Instance().RetAlbumsByURL(url);
			Json::Value data=albums_info["data"];
			Json::Value photo_list=data["photo_list"];
			for (unsigned int i=0; i<photo_list.size(); i++  )
			{
				Json::Value info=photo_list[i];
				if ( false==info.empty() )
				{
					std::string photo_url=info["pic_host"].asString()+"/mw690/"+info["pic_name"].asString(); //TBD
					const char* caption=info["caption_render"].asCString();
					std::string ansi_cap=unicode_to_ansi(caption);
					albums->AddPicture(photo_url.c_str(), ansi_cap.c_str());
				}
				else
				{ //TODO: 为什么有null
					LLOG(L_ERROR,"WeiboExplore::GetAlbumsFromPage JsonValue is null.");
				}
			}
		}
	}
	else
	{
		LLOG2(L_ERROR,"WeiboExplore::GetAlbumsFromPage JsonPrase error..");
	}
	return albums;
}


URLARRAY WeiboExplore::GetAlbumsUrlFromPage(URL_PTR url, std::string& content)
{
	//筛选要读取的相册 拼装URL
	URLARRAY  urlarray;
	Json::Reader reader;
	Json::Value   info;
	if ( reader.parse(content, info) )
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
						
						char buffer[1024];
						sprintf(buffer, "http://photo.weibo.com/photos/get_all?uid=%s&album_id=%s&count=%d&page=%d&type=%d",
							album["uid"].asCString(), album["album_id"].asCString(), get_count,  page_num , album["type"].asInt() );
						URL_PTR albums_url=CreateURL( std::string(buffer),WEIBO_ALBUMS );
						url->albums_id=UrlHashCode(url);
						Albums* album=AlbumsCollections::Instance().RetAlbumsByURL(url);
						albums_url->albums_id=album->GetUUID();
						urlarray.push_back(albums_url);
						cur_pos+=get_count;
						page_num++;
					} while ( cur_pos<pic_count );

					break;
				}
			}
		}
	}
	return urlarray;
}

URLARRAY WeiboExplore::GetUserIndexFromPage(std::string& content)
{
	//"http://photo.weibo.com/albums/get_all?uid=2139885703&page=1&count=20"
	//转码
	Json::Value  js_content;
	Json::Reader reader;
	std::string    de_content;
	URLARRAY url_array;
	
	if ( reader.parse(content, js_content) )
	{
		Json::Value js_data=js_content["data"];
		Json::Value js_html=js_data["html"];
		de_content=js_html.asString();

		StrVec  lines;
		SplitString(de_content, lines, "\n");

		for (int i=0;i<lines.size(); i++ )
		{
			std::string line=lines[i];
			if ( line.find("<div class=\"WB_face\">")!=std::string::npos )
			{
				int start_pos=line.find("usercard=\"id=");
				if ( start_pos!=std::string::npos )
				{
					int end_pos=line.find("\"", start_pos+13);
					std::string user_id=line.substr(start_pos+13, end_pos-start_pos-13);
					std::string url="http://photo.weibo.com/albums/get_all?uid="+user_id+"&page=1&count=20";
					URL_PTR url_ptr=CreateURL(url, WEIBO_ALBUMS_INFO);
					url_array.push_back(url_ptr);
				}
			}
		}

	}
	return url_array;
}



