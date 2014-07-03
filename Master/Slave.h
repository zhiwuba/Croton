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
	int DoTask(std::string& task_content  ,TASKID& task_id); //��������
	int ProcessRead(struct bufferevent* bev);   //Master�����������Ӧ
	int ProcessWrite(struct bufferevent* bev);  //Master��������

	bool  CanBeAssigned();  //���Է��������
	ulong GetSlaveID();
	bool   CheckTimeOut();  //��������Ƿ�ʱ
	inline SlaveStatus GetStatus(){return m_status; };
	
private:
	int RetrieveTask();  //����δ��ɵ�����
	int PraseAndResponse(std::string& request,std::string& response);
	SlaveStatus m_status;
	ulong  m_slave_id;
	sockaddr_in  m_address;
	bufferevent* m_bufferevent;
	std::list<TASKID>  m_TaskList; //�����б�

	ulong  m_time_report_heartbeat; //��һ���ϱ�heartbeat��ʱ��
};

#endif
