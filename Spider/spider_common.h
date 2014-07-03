#ifndef  __CROTON_SPIDER_COMMON_H__
#define __CROTON_SPIDER_COMMON_H__
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <list>

#ifdef WIN32
#include <Winsock2.h>
#endif

#include "Porting/CrotonPorting.h"

static const  int   kDomainTableSize=100*1024;   //100K
static const  int   kDomainHashSize=20000;        //2W������
static const  int   kUrlTableSize=100*1024; 
static const  int   kUrlHashSize=64000000;

static const  int	kThreadPoolSize=20;   //����20���̳߳�
static const  int   kProcessCountPer=20;  //ÿ�δ���
static const  int   kMinSet=5;               //����5����ʱ������
 
static const  char*  kConfigFileName="spider.conf"; 
static const  char*  kSeedFileName="spider.seed";
static const  char*  KCookieFileName="spider.cookie";
static const  char*  kDnsFileName="spider.dns";
static const  char*  kHistoryFileName="spider.history";
static const  char*  kDatabaseName="storage";


typedef std::vector<std::string> StrVec;
typedef std::pair<int,int>           IntPair;
typedef std::map<std::string, std::string> FieldsMap;

#endif
