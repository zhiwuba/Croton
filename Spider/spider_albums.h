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
	URL_PTR  m_url;  //ͼƬ��url
	std::string  m_description;//ͼƬ�ı���
	std::string m_filename; //�ļ���
	int            m_filesize;   //ͼƬ�ļ���С
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
	std::list<Picture*>  m_old_albums;  //�ϴ�ɨ������
	std::list<Picture*>  m_new_albums; //���µ�ͼƬ�ļ�
	std::string  m_description;  //�������
	int             m_id;       //���UUID
	std::string  m_url;      //���URL
	std::string  m_path;   //���·��
};

#endif
