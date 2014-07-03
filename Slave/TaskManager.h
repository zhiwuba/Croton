#ifndef  __CROTON_SLAVE_TASKMANAGER_H__
#define __CROTON_SLAVE_TASKMANAGER_H__
#include "Common.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

class BaseWorker;
class TaskManager
{
public:
	TaskManager();
	~TaskManager();
	int  AddTask(std::string  task_content);
	int  Process(bufferevent* buffer_event);
private:
	std::list<BaseWorker*>  m_TaskList; //要完成的Task
	handle_mutex   m_TaskListMutex;   //TaskList 互斥锁
};


#endif
