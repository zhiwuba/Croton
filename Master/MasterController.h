#ifndef  __CROTON_MASTER_H__ 
#define __CROTON_MASTER_H__
#include "Common.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"

class MasterController
{
public:
	MasterController();
	~MasterController();
	bool  RunService(int port=8081);
	bool  StopService();

private:
	static void Accept_CallBack(struct evconnlistener* listener, evutil_socket_t fd, 
												struct sockaddr* address, int socklen, void* ctx);
    static	void Read_CallBack(struct bufferevent* bev, void *ctx);
    static void Write_CallBack(struct bufferevent* bev, void *ctx);

    static  unsigned int __stdcall ThreadProcess(void* param);
	
   event_base* m_event_base;
   SOCKET  m_listen_socket;
   handle_thread m_hThread;
   volatile  bool m_bExit;
};

#endif
