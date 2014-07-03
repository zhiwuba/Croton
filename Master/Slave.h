#ifndef  __CROTON_MASTER_SLAVE_H__
#define __CROTON_MASTER_SLAVE_H__

#include <queue>
#include "Common.h"
#include "libevent/event.h"
#include "json/json.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"


class MSProtocal;
class  Slave
{
public:
	Slave(bufferevent* bev, ulong slave_id);
	~Slave();
	int DoTask(std::string& task_content  ,TASKID& task_id); //安排任务
	int ProcessRead(struct bufferevent* bev);   //Master处理请求和响应
	int ProcessWrite(struct bufferevent* bev);  //Master发起请求

	bool  CanBeAssigned();  //可以分配任务的
	ulong GetSlaveID();
	bool   CheckTimeOut();  //检测心跳是否超时
	inline SlaveStatus GetStatus(){return m_status; };
	
private:
	int RetrieveTask();  //回收未完成的任务
	int PraseAndResponse(std::string& request,std::string& response);
	SlaveStatus m_status;
	ulong  m_slave_id;
	sockaddr_in  m_address;
	bufferevent* m_bufferevent;
	std::list<TASKID>  m_TaskList; //任务列表

	ulong  m_time_report_heartbeat; //上一次上报heartbeat的时间
};

#endif
