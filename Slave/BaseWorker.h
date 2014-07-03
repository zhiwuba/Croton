#ifndef  __CROTON_SLAVE_BASEWORKER_H__
#define __CROTON_SLAVE_BASEWORKER_H__
#include "Porting/CrotonPorting.h"
#include "Common.h"

class BaseWorker
{
public:
	BaseWorker(){ };
	virtual ~BaseWorker(){ };
	virtual int AssignTask(std::string task_content)=0;    /*分配任务*/
	virtual int ProcessAndSubmit()=0;                           /*处理任务*/
	virtual TASK_STATUS& GetStatus()=0;                     /*获取状态*/
	virtual TASKID& GetTaskID()=0;                              /*获取ID*/
	virtual int   GetProgress()=0;                                   /*获取进度*/
private:
	
};

#endif
