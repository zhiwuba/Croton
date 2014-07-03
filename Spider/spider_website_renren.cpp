#include "RenRenExplore.h"
#include "Albums.h"
#include "CookieManager.h"
#include "URLManager.h"
#include "AlbumsCollections.h"

RenRenExplore::RenRenExplore()
{
	
}

RenRenExplore::~RenRenExplore()
{
	
}

int RenRenExplore::Process(int sock, URL_PTR url)
{
	int  ret=0;
	int status_code;
	std::string recvbody;

	switch ( url->type )
	{
	case RENREN_ALBUMS:
		{
			status_code=RecvResponse(sock,recvbody);
			if ( status_code==200 )
			{
				if (strstr(url->domain,"share.renren.com")!=NULL)
				{
					URL_PTR url=GetPhotoUrlFromShare(recvbody);
					URLManager::Instance().PushUrl(url);
				}
				else if (strstr(url->domain,"photo.renren.com")!=NULL )
				{
					Albums* albums=GetAlbumsFromPage(url, recvbody);
					AlbumsCollections::Instance().AddAlbums(albums);
					URLARRAY  url_array;
					albums->GetNewUrlArray(url_array);
					URLManager::Instance().PushUrl(url_array);
				}
			}
			else
			{
				LLOG(L_ERROR,"RenRenExplore::Process recv %s error.", url->url);
				ret=-1;
			}
		}
		break;
	case RENREN_INDEX:
		{
			status_code=RecvResponse(sock, recvbody);
			if ( status_code==200 )
			{
				URLARRAY url_array=GetAlbumsIndexFromPage(recvbody);
				URLManager::Instance().PushUrl(url_array);
			}
			else if (status_code==302)
			{
				URL_PTR url_c=CreateURL(recvbody,RENREN_INDEX);
				URLManager::Instance().PushUrl(url_c);
			}
			else
			{
				LLOG(L_ERROR,"RenRenExplore::Process recv %s error.", url->url);
				ret=-1;
			}
		}
	default:
		break;
	}
	return ret;
}

/*编码问题如何解决*/
Albums* RenRenExplore::GetAlbumsFromPage(URL_PTR url, std::string& content)
{
	Albums* albums=NULL;
	std::string  html=content; 
	StrVec  lines;
	SplitString(html, lines, "\n");
	
	unsigned int start_line=0;
	unsigned int end_line=0;
	for (start_line=0;start_line<lines.size() ;start_line++ )
	{
		if ( strncmp( lines[start_line].c_str(), "<div class=\"photo-list my-list\"", 31)==0)
		{ //找到相册list标志
			break;
		}
	}

	if ( start_line==0 )
	{
		LLOG(L_ERROR,"RenRenExplore::GetAlbumsFromPage  can't find photo-list my-list.");
		return  albums;
	}

	int div_count=0; 
	for (unsigned int i=start_line;i<lines.size();i++)
	{
		std::string& line=lines[i];
		if ( strncmp(line.c_str(), "<div",4)==0&&line.find("/>")==std::string::npos )
		{
			div_count++;
		}
		if ( line.find("</div>")!=std::string::npos )
		{
			div_count--;
		}
		if ( div_count==0 )
		{
			end_line=i;
			break;
		}
	}
	
	albums=new Albums(url, "");  //TODO: 描述
	for (unsigned  int i=start_line;i<end_line;i++ )
	{
		if ( strncmp(lines[i].c_str(), "<li", 3 )==0 )
		{
			std::string description;
			std::string url;
			for (;i<end_line;i++  )
			{
				std::string line=lines[i];
				if ( strncmp(line.c_str(), "<img src=",9 )==0 )
				{
					int start_pos=line.find("',large:'");
					int end_pos=line.find("',",start_pos+9);
					if ( start_pos!=std::string::npos&&start_pos<end_pos )
					{
						url=line.substr(start_pos+9, end_pos-start_pos-9);
					}
					continue;
				}
				if ( strncmp(line.c_str(),"<input type=\"text\" value=\"", 26)==0 )
				{
					int endpos=line.find("\"",26);
					if ( endpos!=std::string::npos )
					{
						description=line.substr(26, endpos-26);
					}
					continue;
				}
				if ( strncmp(line.c_str(), "</li>",5)==0 )
				{
					break;
				}
			}
			std::string ansi_desc=unicode_to_ansi(description.c_str());
			albums->AddPicture(url.c_str(), ansi_desc.c_str());
		}
	}

	return albums;
}


URLARRAY RenRenExplore::GetAlbumsIndexFromPage(std::string& content)
{   //索引信息
	URLARRAY   albums_index;  
	
	StrVec  lines;
	SplitString(content, lines, "\n");
	/**  解析相册索引  **/
	unsigned int current_line=0;
	unsigned int start_line=0, end_line=0;
	for ( ;current_line<lines.size(); current_line++ )
	{
		if ( strncmp( lines[current_line].c_str(), "<ol class=\"pagerpro\">", 21)==0 )
		{  //找到相册list标志
			start_line=current_line;
			continue;
		}
		if ( strncmp(lines[current_line].c_str(), "</ol>", 5)==0 )
		{
			end_line=current_line;
			break;
		}
	}

	/**解析出相册URL**/
	for (unsigned int i=end_line; i<lines.size(); i++ )
	{
		std::string line=lines[i];
		if ( line.find("<h3>")!=std::string::npos )
		{
			int start_pos=line.find("<a href=\"");
			int end_pos=line.find("\"",start_pos+9);
			if ( start_pos!=std::string::npos&&end_pos!=std::string::npos )
			{
				std::string url=line.substr(start_pos+9, end_pos-start_pos-9);
				URL_PTR object=CreateURL(url, RENREN_ALBUMS);
				albums_index.push_back(object);
			}
			else
			{
				LLOG2(L_ERROR,"RenRenExplore::GetAlbumsIndexFromPage prase albums error.");
			}
		}
		else if(line.compare("</ul>")==0)
		{
			break;
		}
	}
	
	return albums_index;
}

URL_PTR  RenRenExplore::GetPhotoUrlFromShare(std::string& content)
{
	StrVec  lines;
	SplitString(content, lines, "\n");

	unsigned int start_line=0;
	unsigned int end_line=0;
	for (start_line=0;start_line<lines.size() ;start_line++ )
	{
		if ( strncmp( lines[start_line].c_str(), "<div class=\"share-source share-from\">", 37)==0)
		{ //找到相册list标志
			break;
		}
	}
	
	std::string  photo_url;
	for (unsigned int i=start_line;i<lines.size();i++  )
	{
		std::string line=lines[i];
		int pos1=line.find("<a href=\"");
		if ( pos1!=std::string::npos )
		{
			int pos2=line.find("\"", pos1+9);
			photo_url=line.substr(pos1+9, pos2-pos1-9);
		}
		if ( strncmp(lines[i].c_str(),"</div>",6)==0 )
		{
			break;
		}
	}

	URL_PTR url;
	if ( !photo_url.empty() )
	{
		url=CreateURL(photo_url, RENREN_ALBUMS );
	}
	return url;
}


