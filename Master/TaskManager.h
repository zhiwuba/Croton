#ifndef  __CROTON_MASTER_TASKMANAGER_H__
#define __CROTON_MASTER_TASKMANAGER_H__
#include "Porting/CrotonPorting.h"
#include "Common.h"
#include "BaseTask.h"

class TaskManager
{
public:
	static TaskManager& Instance()
	{
		static TaskManager _instance;
		return _instance;
	}
	int Process();

	int AssignTask();
	int UpdateTask();   /*更新任务*/
	int CreateTask(TASK_TYPE task_type , char* url );
	int SetTaskStatus( int task_id, int inner_id, TaskStatus status );
private:
	TaskManager();
	~TaskManager();
	TaskManager(const TaskManager&);
	TaskManager& operator=(const TaskManager&);

	std::map<int,BaseTask* > m_TaskMap;
};


#endif
