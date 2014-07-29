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

int test_rinse()
{
	UrlPtrVec url_vec;

	UrlPtr url_ptr1=create_url("http://www.cnblogs.com/tonykan/archive/2012/11/26/2788433.html",UT_HTML);
	url_vec.push_back(url_ptr1);
	UrlPtr url_ptr2=create_url("http://sxj007.blog.51cto.com/521729/121527",UT_HTML);
	url_vec.push_back(url_ptr2);
	UrlPtr url_ptr3=create_url("http://sxj007.blog.51cto.com/521729/d-3",UT_HTML);
	url_vec.push_back(url_ptr3);
	UrlPtr url_ptr4=create_url("http://bbs.csdn.net/topics/310181034",UT_HTML);
	url_vec.push_back(url_ptr4);
	UrlPtr url_ptr5=create_url("http://blog.51cto.com/zt/555",UT_HTML);
	url_vec.push_back(url_ptr5);
	UrlPtr url_ptr6=create_url("http://www.jspcn.net/forum/viewthread.php?tid=21",UT_HTML);
	url_vec.push_back(url_ptr6);
	UrlPtr url_ptr7=create_url("http://www.w3school.com.cn/sql/sql_insert.asp",UT_HTML);
	url_vec.push_back(url_ptr7);
	UrlPtr url_ptr8=create_url("http://blog.csdn.net/candyliuxj/article/details/7853938",UT_HTML);
	url_vec.push_back(url_ptr8);


	Spider_Executor::instance().initialize();
	Spider_Url_Rinse::instance().rinse_urls(url_vec);
	return 0;
}

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
	init_network();
	
	Spider_Config::instance().load();
	Spider_Seed spider_seed;
	spider_seed.load();

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
			website->initialize(site_name.c_str(), seed->start_url_, seed->index_url_, seed->pic_url_,seed->pic_size_);
			website->begin_process();  //处理seed节点
			
			delete seed;
		}
		else
		{
			LLOG(L_DEBUG, "load all seed.");
			break;
		}
		Sleep(1000*1000); //1s
	}
	
	Spider_Executor::instance().wait_complete(); //等待完成
	
	Spider_Url_Rinse::instance().uninitialize();
	Spider_Executor::instance().uninitialize();
	Spider_Storage::instance().uninitialize();
	uninit_network();
	return 0;
};

