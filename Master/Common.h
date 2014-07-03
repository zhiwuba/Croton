#ifndef  __CROTON_MASTER_COMMON_H__
#define  __CROTON_MASTER_COMMON_H__
#include "Porting/CrotonPorting.h"


enum  TaskStatus
{
	None,         // ��
	UnAssigned, //  δ����
	Assigned,   //  �ѷ��� δ���
	Completed //  ���
};

enum TASK_TYPE
{
	DOWNLOAD_BIG_FILE,
	CRAWL_THE_WEB
};

enum  SlaveStatus
{
	UNLOGIN,
	LOGINED,
	DEAD
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

//Body��������
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

/*Master �� Slave ֮��ͨ�ŵ�buffer*/
struct MSBuffer
{
	char *buf;
	int len;
};



#define  MAXBUFFER   1024*4
#define  SLAVE_TIMEOUT  120*1000    //ms 

#endif
