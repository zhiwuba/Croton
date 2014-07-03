#ifndef __CROTON_SPIDER_RENRENEXPLORE_H__
#define __CROTON_SPIDER_RENRENEXPLORE_H__
#include "ExploreBase.h"

class RenRenExplore:public ExploreBase
{
public:
    RenRenExplore();
    ~RenRenExplore();
	int Process(int sock, URL_PTR url);
	Albums*     GetAlbumsFromPage(URL_PTR url, std::string& content); 
	URLARRAY GetAlbumsIndexFromPage(std::string& content);
	URL_PTR          GetPhotoUrlFromShare(std::string& content);
};

#endif
