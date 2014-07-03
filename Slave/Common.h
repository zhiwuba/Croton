#ifndef  __CROTON_SLAVE_COMMON_H__
#define  __CROTON_SLAVE_COMMON_H__
#include "Porting/CrotonPorting.h"

enum TASK_TYPE
{
	DOWNLOAD_BIG_FILE,
	CRAWL_THE_WEB
};

enum  PKG_TYPE
{
	INVALID_PKG,
	LOGIN,
	HEARTBEAT,
	TASK,
	RESULT,
	EXIT
};

enum QP_TYPE
{
	INVALID_QP,
	REQUEST,
	RESPONSE
};

enum  TASK_STATUS
{
	INVALID_TASK, /*任务初始化状态*/
	ASSIGN, /*分配*/
	PROCESSED,  /*处理完成*/
	SUBMITED,    /*已经提交*/
};


enum SLAVE_STATUS
{
	Unvested,        /*没归属的*/
	Logined,          /*已登录的*/
	DisConnected /*已断开的*/
};

#define  CONNECT_TIMEOUT  10  /*连接超时10s*/

//Body包含的码
#define  JS_CODE "Code"  //OK or FAIL
#define  JS_TASKID "TaskID"
#define  JS_LITTLETASKID "LittleTaskID"

struct  TASKID
{
	int task_id;
	int inner_id;
	bool operator==(const TASKID& other) const
	{
		return (task_id==other.task_id)&&(inner_id==other.inner_id);
	}
};

/*Master 和 Slave 之间通信的buffer*/
struct MSBuffer
{
	char *buf;
	int len;
};



#endif
