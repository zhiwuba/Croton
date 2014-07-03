#include "MasterController.h"
#include "libevent/event.h"
#include "libevent/evhttp.h"
#include "SlaveManager.h"
#include "TaskManager.h"


struct bufferq
{
	char *buf;
    int len;
	int offset;
};

struct  Client
{
	struct  event  ev_read;
	struct  event  ev_write;
	std::queue<bufferq* >  writeq;
};


MasterController::MasterController()
{
	m_bExit=false;
}

MasterController::~MasterController()
{

}

bool MasterController::RunService(int port)
{
	LLOG(L_DEBUG,"RunService: port is %d\n", port );
	unsigned int  threadid;
	m_hThread=thread_create(NULL,0,(THREAD_FUN)ThreadProcess, (void*)this , 0, &threadid);
	
	m_event_base=event_base_new();
	if ( m_event_base==NULL )
	{
		LLOG(L_ERROR , "event_base_new error .");
		return false;
	}

	sockaddr_in localaddress;
	localaddress.sin_family=AF_INET;
	localaddress.sin_addr.s_addr=INADDR_ANY;
	localaddress.sin_port=htons(port);
	socklen_t  address_length=sizeof(localaddress);
	
	struct evconnlistener* listener=evconnlistener_new_bind(m_event_base,Accept_CallBack,NULL,
					LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,(struct sockaddr*)&localaddress, address_length);

	if ( listener==NULL )
	{
		LLOG(L_ERROR,"Couldn't Create listener.");
		return false;
	}
	
	event_base_dispatch(m_event_base);

	evconnlistener_free(listener);
	return true;
}


bool MasterController::StopService()
{
	LLOG(L_TRACE,"EnterInto StopService.");
	m_bExit=true; 
	event_base_loopbreak(m_event_base);
	event_base_free(m_event_base);
	m_event_base=NULL;
	WaitForSingleObject(m_hThread,INFINITE);
	LLOG(L_TRACE,"ExitFrom StopService.");
	return true;
}

void MasterController::Accept_CallBack(struct evconnlistener* listener, evutil_socket_t fd, 
															struct sockaddr* address, int socklen, void* ctx)
{
	LLOG(L_TRACE,"EnterInto Accept_CallBack .");
	struct event_base* base=evconnlistener_get_base(listener);
	struct bufferevent* bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);

	struct sockaddr_in* client_address=(sockaddr_in*)address;
	Slave* slave=SlaveManager::Instance().AddSlave(bev, client_address->sin_addr.s_addr);
	
	bufferevent_setcb(bev, Read_CallBack, Write_CallBack, NULL , (void*)slave);
	bufferevent_enable(bev,EV_READ|EV_WRITE);

	LLOG(L_DEBUG, "Accept Connection is %s .", inet_ntoa(client_address->sin_addr));
}

void MasterController::Read_CallBack(struct bufferevent* bev, void *ctx)
{
	Slave* slave=(Slave*)ctx;
	if ( slave!=NULL )
	{
		slave->ProcessRead(bev);
	}
}

void MasterController::Write_CallBack(struct bufferevent* bev, void *ctx)
{
	Slave* slave=(Slave*)ctx;
	if ( slave!=NULL )
	{
		slave->ProcessWrite(bev);
	}
}


unsigned int __stdcall MasterController::ThreadProcess(void* param)
{	
	LLOG(L_TRACE, "EnterInto MasterController::Process.");
	MasterController* mc=(MasterController*)param;
	while ( mc->m_bExit==false  )
	{
		ulong time_start=GetTickCount();
		SlaveManager::Instance().Process();  //清理DEAD的Slave
		TaskManager::Instance().Process();   //分发任务 清理已完成的任务等。
		ulong time_cost=GetTickCount()-time_start;
		if ( time_cost<10*1000 )
		{
			Sleep(10*1000-time_cost);
		}
	}
	LLOG(L_TRACE,"Exit  MasterController::Process.");
	return 0;
}
