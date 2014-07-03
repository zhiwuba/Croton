#ifndef  __CROTON_SPIDER_WEIBOEXPLORE_H__
#define __CROTON_SPIDER_WEIBOEXPLORE_H__
#include "ExploreBase.h"

class WeiboExplore:public ExploreBase
{
public:
	WeiboExplore();
	~WeiboExplore();
	
	int    Process(int sock, URL_PTR url);
	
	Albums* GetAlbumsFromPage(URL_PTR url, std::string& content); 
	URLARRAY GetAlbumsUrlFromPage(URL_PTR url, std::string& content);
	URLARRAY GetUserIndexFromPage(std::string& content);
};


#endif
