#include <stdio.h>
#include <stdlib.h>
#include "MasterController.h"
#include "curl/curl.h"
#include "DownloadBigFile.h"
#include "SlaveManager.h"
#include "TaskManager.h"



int main()
{
	CURLcode code=curl_global_init(CURL_GLOBAL_WIN32);
	if ( code != CURLE_OK )
	{
		printf("curl_global_init error \n");
		return -1;
	}
	MasterController* controller=new MasterController();
	TaskManager::Instance().CreateTask(DOWNLOAD_BIG_FILE , "http://10.20.5.173/Fast1080P.wmv" );
	controller->RunService(8081);
	while (true)
	{
		Sleep(1000);
	}
	controller->StopService();
	curl_global_cleanup();
	return 0;
}
