#include "Albums.h"
#include "URLManager.h"

Picture::Picture(const char* url, const char* path, const char* description)
{
	if ( url!=NULL )
	{
		m_url=CreateURL(url, FILE_ET);		
		std::string temp(url);
		std::string filename=temp.substr(temp.find_last_of('/')+1);
		filename=std::string(path)+PATH_SEPARATOR+filename;
		m_url->filename=strdup(filename.c_str());
		m_description.assign(description);
	}
}

Picture::~Picture()
{
}

std::string Picture::ToString()
{
	std::string to_string;
	to_string=m_url->url;
	to_string+="\r";
	to_string+=m_description;
	return to_string;
}


Spider_Albums::Spider_Albums(URL_PTR url, const char* description)
{
	if ( url!=NULL&&description!=NULL )
	{
		m_url.assign(url->url);
		m_id=url_hash_code(url);
		m_description=description;
		char id[30];
		itoa(m_id,id,10);
		m_path=GetExePath()+PATH_SEPARATOR+std::string(id);
		create_dir(m_path.c_str());
	}
	else
	{
		LLOG2(L_ERROR , "Albums construct url is null.");
	}
}

Spider_Albums::Spider_Albums(const char* url, int id ,const char* description)
{
	if ( url!=NULL&&description!=NULL )
	{
		m_url.assign(url);
		m_id=id;
		m_description=description;
		char str_id[30];
		itoa(m_id,str_id,10);
		m_path=GetExePath()+PATH_SEPARATOR+std::string(str_id);
		create_dir(m_path.c_str());
	}
	else
	{
		LLOG2(L_ERROR , "Albums construct url is null.");
	}
}


Spider_Albums::~Spider_Albums(void)
{
	std::list<Picture*>::iterator iter=m_old_albums.begin();
	for ( ; iter!=m_old_albums.end() ; iter++ )
	{
		delete  (*iter);
	}
	m_old_albums.clear();
	iter=m_new_albums.begin();
	for (;iter!=m_new_albums.end(); iter++)
	{
		delete (*iter);
	}
	m_new_albums.clear();
}

int Spider_Albums::add_picture(const char* url, const char* description)
{
	int ret=0;
	if ( (url!=NULL)&&(!Spider_Url_Rinse::Instance().CheckURL(url)) )
	{
		LLOG(L_DEBUG,"Add %s to albums %d", url, m_id);
		Picture* picture=new Picture(url , m_path.c_str() , description);
		m_new_albums.push_back(picture);
	}
	else
	{
		LLOG(L_WARN,"AddPicture: url is null or duplicate url.");
		ret=-1;
	}
	return ret;
}

int Spider_Albums::get_newurl_array(URLARRAY& urlarray)
{
	std::list<Picture*>::iterator iter=m_new_albums.begin();
	for (;iter!=m_new_albums.end() ;iter++ )
	{
		URL_PTR temp=(*iter)->GetURL();
		if ( temp!=NULL)
		{
			urlarray.push_back(temp);
		}
		m_old_albums.push_back((*iter));
	}
	
	m_new_albums.clear();
	return 0;
}

/*
  {url\tid\tdescription}{picture\tpicture\t}
*/
std::string Spider_Albums::to_string()
{
	char str_id[30];
	itoa(m_id,str_id,10);
	std::string to_string;
	std::string albums_header;
	albums_header=m_url+"\t"+str_id+"\t"+m_description;
	std::string albums_dody;
	std::list<Picture*>::iterator iter=m_old_albums.begin();
	for ( ;iter!=m_old_albums.end(); iter++)
	{
		albums_dody+=(*iter)->ToString();
		albums_dody+="\t";
	}
	iter=m_new_albums.begin();
	for (;iter!=m_new_albums.end(); iter++ )
	{
		albums_dody+=(*iter)->ToString();
		albums_dody+="\t";
	}
	to_string="{"+albums_header+"}"+"{"+albums_dody+"}";
	return to_string;
}

Spider_Albums*  Spider_Albums::from_string(std::string& buffer)
{
	Spider_Albums* albums=NULL;
	int pos=buffer.find('}');
	std::string info=buffer.substr(1, pos-1);
	StrVec header_vec;
	split_string(info,header_vec,"\t");
	if ( header_vec.size()>=2 )
	{
		std::string description=(header_vec.size()==3? header_vec[2]:"");
		albums=new Spider_Albums(header_vec[0].c_str(), atoi(header_vec[1].c_str()), description.c_str());

		std::string pics=buffer.substr(pos+2, buffer.size()-pos-3);
		StrVec body_vec;
		split_string(pics, body_vec,"\t");
		for ( int i=0; i< body_vec.size(); i++ )
		{
			int r_pos=body_vec[i].find('\r');
			if ( r_pos!=std::string::npos)
			{
				std::string url=body_vec[i].substr(0, r_pos);
				std::string description=body_vec[i].substr(r_pos+1);
				if ( !url.empty() )
				{
					LLOG(L_DEBUG,"Add %s to albums %d", url.c_str(), albums->m_id);
					Picture* picture=new Picture(url.c_str() , albums->m_path.c_str() , description.c_str());
					albums->m_old_albums.push_back(picture);
				}
			}
		}
	}
	else
	{
		LLOG2(L_ERROR,"Albums information catch exception !!");
	}
	
	return albums;
}


