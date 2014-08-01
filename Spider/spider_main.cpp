#include "spider_common.h"
#include "spider_config.h"
#include "spider_seed.h"
#include "spider_website.h"
#include "spider_executor.h"
#include "spider_storage.h"
#include "spider_database.h"
#include "spider_url_rinse.h"
#include "spider_http_client.h"

using namespace std;
using namespace boost::xpressive;


int test_regex()
{
	boost::xpressive::cregex  reg=boost::xpressive::cregex::compile("http://forgifs.com/gallery/main.php\\?g2_page=\\d{1,3}");
	bool bret=regex_match("http://forgifs.com/gallery/main.php?g2_page=6", reg);
	return 0;
}


void  test_regex2()
{
	char buffer[1023];
	//cregex my_regex=cregex::compile("^<div class=\"ugc-detail-container\">(.)+<p class=\"share-summary\">(.)+</p></div>$");
	cregex my_regex=cregex::compile("^<div class=\"ugc-detail-container\">(.)+</div>$");
	//cregex my_regex=cregex::compile("^<p class=\"share-summary\">(.)+</p>$");

	cmatch what;
	if( regex_search(buffer, what, my_regex) )
	{
		int size=what.size();
		for ( int i=0; i< what.size(); i++ )
		{
			cout<<what[i]<<endl;
			printf("\n\n\n\n");
		}
	}
	else
	{
		printf("not found . \n");
	}
}

int test_httpclientbase()
{
	UrlPtr url_ptr=create_url("http://forgifs.com/gallery/d/214974-1/Walks-off-motorcycle-accident.gif",UT_PICT);
	url_ptr->ip=strdup("108.162.201.43");
	Spider_Http_Client http_client;

	while(true)
	{
		printf("======================\n");
		int sock=http_client.send_request(url_ptr);
		char* body=NULL;
		int length=0;
		http_client.recv_response(sock,&body,length );
		if ( body!=NULL )
		{
			delete[] body;
		}
		printf("=======over=====\n");
		Sleep(2000);
	}
	return 0;
};

int main()
{
	int ret=init_network();
	if( ret!=0 )
	{
		LLOG(L_ERROR, "init_network error. ");
		return -1;
	}
	
	Spider_Config::instance().load();
	Spider_Seed spider_seed;
	if( 0!=spider_seed.load() )
	{
		return -1;
	}

	Spider_Storage::instance().initialize();
	Spider_Executor::instance().initialize();
	Spider_Url_Rinse::instance().initialize();

	while(true)	
	{
		Seed* seed=NULL;
		std::string site_name;
		spider_seed.get_seed(site_name,&seed);
		if ( seed!=NULL )
		{
			Spider_WebSite* website=Spider_WebSite_Factory::create_website(site_name);
			website->initialize(site_name.c_str(), seed);
			website->begin_process();  //处理seed节点
		}
		else
		{
			LLOG(L_DEBUG, "load all seed.");
			break;
		}
		Sleep(1000); //1s
	}
	
	Spider_Executor::instance().execute_loop(); //循环等待完成
	
	Spider_Url_Rinse::instance().uninitialize();
	Spider_Executor::instance().uninitialize();
	Spider_Storage::instance().uninitialize();
	uninit_network();
	return 0;
};

