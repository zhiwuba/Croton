#ifndef  __CROTON_SLAVE_BASEWORKER_H__
#define __CROTON_SLAVE_BASEWORKER_H__
#include "Porting/CrotonPorting.h"
#include "Common.h"

class BaseWorker
{
public:
	BaseWorker(){ };
	virtual ~BaseWorker(){ };
	virtual int AssignTask(std::string task_content)=0;    /*��������*/
	virtual int ProcessAndSubmit()=0;                           /*��������*/
	virtual TASK_STATUS& GetStatus()=0;                     /*��ȡ״̬*/
	virtual TASKID& GetTaskID()=0;                              /*��ȡID*/
	virtual int   GetProgress()=0;                                   /*��ȡ����*/
private:
	
};

#endif
