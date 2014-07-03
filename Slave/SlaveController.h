#ifndef  __CROTON_SLAVE_SLAVECONTROLLER_H__
#define __CROTON_SLAVE_SLAVECONTROLLER_H__
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "Common.h"

class TaskManager;
class SlaveController
{
public:
	SlaveController();
	~SlaveController();
	int  RunService(const char* ip, int port);
	int  StopService();

private:
	int ProcessRead(std::string);
	int ProcessWrite();

	static unsigned int __stdcall ThreadDoAffairs(void* p); //循环做一些日常事务
	static void Write_CallBack(struct bufferevent* bev, void *ctx);
	static void Read_CallBack(struct bufferevent* bev, void *ctx);
	static void Event_CallBack(struct bufferevent* bev, short events, void* ptr);

	event_base* m_event_base;
	bufferevent* m_buffer_event;
	handle_thread m_hThread;
	
	TaskManager* m_TaskManager;
	SLAVE_STATUS m_slave_status;
};

#endif
