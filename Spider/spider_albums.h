#ifndef  __CROTON_SPIDER_ALNUMS_H__
#define __CROTON_SPIDER_ALNUMS_H__


class  Picture
{
public:
	Picture(const char* url, const char* path ,const char* description);
	~Picture();
	URL_PTR   GetURL(){return m_url;};
	std::string ToString();
private:
	URL_PTR  m_url;  //图片的url
	std::string  m_description;//图片的标语
	std::string m_filename; //文件名
	int            m_filesize;   //图片文件大小
};

class Spider_Albums
{
public:
	Spider_Albums(URL_PTR url, const char* description);
	Spider_Albums(const char* url, int id ,const char* description);
	~Spider_Albums(void);
	static Spider_Albums* from_string(std::string& buffer);

	int add_picture(const char* url, const char* description);
	int get_newurl_array(URLARRAY& urlarray);
	int get_uuid(){return m_id;};
	const char* get_url(){return m_url.c_str(); };
	std::string   to_string();

private:
	std::list<Picture*>  m_old_albums;  //上次扫描的相册
	std::list<Picture*>  m_new_albums; //更新的图片文件
	std::string  m_description;  //相册描述
	int             m_id;       //相册UUID
	std::string  m_url;      //相册URL
	std::string  m_path;   //相册路径
};

#endif
