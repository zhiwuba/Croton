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

typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned __int64  uint64_t;


static const  int   kDomainTableSize=100*1024;   //100K
static const  int   kDomainHashSize=20000;        //2W������
static const  int   kUrlTableSize=100*1024; 
static const  uint   kUrlHashSize=0xfffffffe;    //513M�Ĺ��˱�

static const  int	kThreadPoolSize=20;   //����20���̳߳�
static const  int   kProcessCountPer=20;  //ÿ�δ���
static const  int   kMinSet=5;               //����5����ʱ������
static const  long kRecordHistoryInterval=1000*60; //unit: ms. 1����

static const  char*  kConfigFileName="spider.conf"; 
static const  char*  kSeedFileName="seed.xml";
static const  char*  KCookieFileName="spider.cookie";
static const  char*  kDnsFileName="spider.dns";
static const  char*  kHistoryFileName="spider.history";


typedef std::vector<std::pair<std::string,std::string>> StrPairVec;
typedef std::vector<std::string> StrVec;
typedef std::pair<int,int>           IntPair;
typedef std::map<std::string, std::string> FieldsMap;

#endif
