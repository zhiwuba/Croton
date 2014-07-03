#include "Porting/CrotonPorting.h"
#include "SlaveController.h"
#include "curl/curl.h"

int main()
{
	CURLcode code=curl_global_init(CURL_GLOBAL_WIN32);
	if ( code != CURLE_OK )
	{
		printf("curl_global_init error \n");
		return -1;
	}
	
	SlaveController  controller;
	controller.RunService("127.0.0.1", 8081);

	curl_global_cleanup();
	return 0;
};
