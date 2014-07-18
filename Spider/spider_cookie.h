#ifndef __CROTON_SPIDER_COOKIEMANAGER_H__
#define __CROTON_SPIDER_COOKIEMANAGER_H__
#include "json/json.h"
#include "spider_common.h"
#include "spider_http_client.h"


class Cookie
{
public:
	static Cookie* from_string(std::string content);
	std::string& operator()(std::string key);
	void         set_cookie(std::string cookie);
	std::string to_string();
	Cookie();
	~Cookie();
private:
	handle_mutex  m_mutex; 
	std::map<std::string, std::string> m_cookie_map;
};

class Spider_Cookie: public Spider_Http_Client_Base
{
public:
    static Spider_Cookie&  instance()
	{
		static Spider_Cookie _instance;
		return _instance;
	}

    ~Spider_Cookie();

	int login();

	Cookie* get_cookie(std::string key);

private:
	Spider_Cookie();
	int  save_cookie();
	int  load_cookie();
	int  login_renren(const char* account, const char* password);
	int  login_weibo(const char* account, const char* password);
private:
	/*��½����*/
	int renren_getencryptkey(Json::Value& encrypt_key, Cookie* cookie);
	int renren_showcaptcha(std::string email, Cookie* cookie);
	int renren_getcaptcha(std::string& code, Cookie* cookie);
	std::string renren_gettoken(std::string url, Cookie* cookie);
	std::string renren_encryptpwd(const char* e, const char* n, const char* pwd);
	int renren_ajaxlogin(const char* email, const char* password ,const char* code, 
						Json::Value& encrypt_key, Cookie* cookie, std::string& home_url);

	/*��½����΢��*/
	int sina_preloginstatus(std::string en_account, std::string& servertime, 
											std::string& nonce, std::string& rsakv,std::string& pubkey,Cookie* cookie);
	int sina_dologin(std::string& en_acount ,std::string& en_password,
								 std::string& servertime, std::string& nonce, std::string&rsakv, std::string& weibo_url,
								 std::string& verification, Cookie* cookie);
	
	int weibo_ajaxlogin(std::string url, Cookie* cookie);
	int weibo_codes(Cookie* cookie, std::string& verification);

    std::map<std::string, Cookie*> m_cookie_database;
};


#endif
