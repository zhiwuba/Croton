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
	INVALID_TASK, /*�����ʼ��״̬*/
	ASSIGN, /*����*/
	PROCESSED,  /*�������*/
	SUBMITED,    /*�Ѿ��ύ*/
};


enum SLAVE_STATUS
{
	Unvested,        /*û������*/
	Logined,          /*�ѵ�¼��*/
	DisConnected /*�ѶϿ���*/
};

#define  CONNECT_TIMEOUT  10  /*���ӳ�ʱ10s*/

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



#endif
