#ifndef  __CROTON_SPIDER_UTILS_H__
#define __CROTON_SPIDER_UTILS_H__

#include "spider_common.h"

//��ʼ������__win32
int     init_network();
void  uninit_network();

//����ת��
int     hex_to_dec(char *s);
char  dec_to_char(int n);
int     char_to_dec(char c);

//��ȡ����
std::string get_date();

//����ת��
std::string unicode_to_ansi(const char* unicode);


// ��ȡ���ƶ�
float  get_similarity_degree(const char* source, const char* target);

//ƥ���ַ��� KMP
void  get_next(const char* partten, int* next);
int    get_match(const char* source, int start, int end,  const char* partten, int*next);

//ժȡurl
bool  is_url_char(char c);
int   get_all_url(const char* url, const char* html, int start, int end,StrVec& url_array);

//���ݱ�ǩ��ȡ��Χ(���öԳ���)
int  get_html_range(char* html, const char* mark, int& start, int& end);

std::string trim_string(const std::string& str, const std::string& drop);
int            split_string(const std::string& str, StrVec& items, const std::string& splitter);
int            prase_url(const std::string & url, std::string & host, std::string & uri ,int & port);

//����
std::string base64_encrypt(const char* data,int data_length);
std::string RSA_encrypt(const char* data ,  const char* n, const char* e);

int get_file_ext(const char* filename, char* ext);

class Recursive_Lock
{
public:
	explicit Recursive_Lock(handle_recursivemutex hmutex):m_recursivemutex(hmutex)
	{
		recursivemutex_lock(m_recursivemutex);
	}

	~Recursive_Lock()
	{
		recursivemutex_unlock(m_recursivemutex);
	}
private:
	handle_recursivemutex m_recursivemutex;
};


#endif
