#ifndef  __CROTON_MASTER_BASETASK_H__
#define __CROTON_MASTER_BASETASK_H__
#include "Porting/CrotonPorting.h"
#include "Common.h"

class  BaseTask
{
public:
	BaseTask(){};
	virtual ~BaseTask(){};
	virtual int  Decompose()=0; //�ֽ�����
	virtual void  Combine()=0;   //�ϲ�����
	virtual bool  IsCompleted()=0;  //�Ƿ����
	virtual int    SetLittleTaskStatus(int inner_id , TaskStatus status)=0;
	virtual int    GetLittleTask(std::string& little_task)=0;
	virtual unsigned int GetUUID()=0;
};

#endif
