#include "SlaveController.h"
#include "libevent/util-internal.h"
#include "json/json.h"
#include "event2/thread.h"
#include "TaskManager.h"
#include "SMProtocal.h"

#define MAX_LINE_LENGTH  10240
#define MAX_OUTPUT_BUFFER  10240

static void timeoutcb(evutil_socket_t fd, short what, void* arg)
{
	struct event_base* base=(event_base*)arg;
	LLOG(L_ERROR,"Connect TimeOut .");
	event_base_loopexit(base,NULL);
}

SlaveController::SlaveController()
{
	m_TaskManager=new TaskManager();
	m_slave_status=Unvested;
}

SlaveController::~SlaveController()
{
	if ( m_TaskManager!=NULL )
	{
		delete m_TaskManager;
		m_TaskManager=NULL;
	}
}

unsigned int __stdcall SlaveController::ThreadDoAffairs(void* p)
{
	LLOG(L_TRACE,"EnterInto ThreadDoAffairs .");
	//心跳
	SlaveController* pThis=(SlaveController*)p;
	if ( pThis!=NULL )
	{
		while ( pThis->m_slave_status==Logined )
		{
			ulong tick_count=GetTickCount()/1000;
			if ( tick_count%10==0 )  //每隔10s做一次心跳汇报
			{
				SMProtocal protocal;
				protocal.SetPkgType(HEARTBEAT);
				protocal.SetQpType(REQUEST);
				Json::Value body;
				body["Content"]="HeartBeat";
				protocal.SetBody(body);
				std::string pack_content;
				protocal.BuildPackage(pack_content);
				evbuffer_add(bufferevent_get_output(pThis->m_buffer_event),pack_content.c_str(), pack_content.length());
			}
			if ( tick_count%2==0 )   //每隔2s做一次任务完成检查
			{
				pThis->m_TaskManager->Process(pThis->m_buffer_event);
			}
			Sleep(1000); //休息1s
		}
	}
	LLOG(L_TRACE,"ExitFrom ThreadDoAffairs .");
	return 0;
}


int SlaveController::RunService(const char* ip, int port)
{
	evthread_use_windows_threads();

	m_event_base=event_base_new();
	struct event* evtimeout=evtimer_new(m_event_base,timeoutcb, m_event_base);
	
	struct timeval  timeout;
	timeout.tv_sec=CONNECT_TIMEOUT*2; 
	timeout.tv_usec=0;
	evtimer_add(evtimeout , &timeout );

	struct sockaddr_in  server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=inet_addr(ip);
	
	m_buffer_event=bufferevent_socket_new(m_event_base,-1,BEV_OPT_THREADSAFE);
	bufferevent_setcb(m_buffer_event,Read_CallBack, Write_CallBack, Event_CallBack, this);
	bufferevent_enable(m_buffer_event, EV_READ|EV_WRITE);
	if ( bufferevent_socket_connect(m_buffer_event, (sockaddr*)&server_addr, sizeof(server_addr))<0 )
	{
		bufferevent_free(m_buffer_event);
		LLOG(L_ERROR,"Connect Error.");
		return -1;
	}
	
	SMProtocal protocal;
	protocal.SetPkgType(LOGIN);
	protocal.SetQpType(REQUEST);
	Json::Value body;
	body["Content"]="Login";
	protocal.SetBody(body);
	std::string pack_content;
	protocal.BuildPackage(pack_content);
	evbuffer_add(bufferevent_get_output(m_buffer_event), pack_content.c_str(), pack_content.length());

	while(1)
	{
		event_base_dispatch(m_event_base);
	}
	//event_base_loop(m_event_base,EVLOOP_NONBLOCK);
	return 0;
}


int SlaveController::StopService()
{
	return 0;
}

void SlaveController::Write_CallBack(struct bufferevent* bev , void *ctx)
{
	struct evbuffer* output=bufferevent_get_output(bev);
}

void SlaveController::Read_CallBack(struct bufferevent* bev , void *ctx)
{
	SlaveController* pThis=(SlaveController*)ctx;
	struct evbuffer* input=bufferevent_get_input(bev);
	
	while(1)
	{
		size_t eol_len=0;
		struct evbuffer_ptr eol=evbuffer_search_eol(input, NULL,&eol_len,EVBUFFER_EOL_LF);
		if ( eol.pos<0 )
		{   //未找到换行符
			size_t readable=evbuffer_get_length(input);
			if ( readable>MAX_LINE_LENGTH )
			{
				LLOG(L_ERROR,"input is too long %zd.", readable);
				bufferevent_free(bev);
			}
			break;
		}
		else if ( eol.pos>MAX_LINE_LENGTH )
		{
			LLOG(L_ERROR,"line is too long %zd .", eol.pos);
			bufferevent_free(bev);
			break;
		}
		else
		{
			assert(eol_len==1);
			char request[MAX_LINE_LENGTH+1];
			assert(eol.pos+1<sizeof(request));
			int req_len=evbuffer_remove(input,request, eol.pos+1);
			assert(req_len==eol.pos+1);
			request[req_len]='\0';
			pThis->ProcessRead(std::string(request) );
		}
	}
}

void SlaveController::Event_CallBack(struct bufferevent* bev, short events, void* ptr)
{
	if ( events&BEV_EVENT_CONNECTED )
	{
	}
	else if(events&BEV_EVENT_ERROR)
	{
		LLOG(L_ERROR,"NOT Connected.");
	}	
}

int SlaveController::ProcessWrite()
{
	LLOG(L_TRACE,"EnterInto ProcessWrite .");
	
	LLOG(L_TRACE,"ExitFrom ProcessWrite .");
	return 0;
}

int SlaveController::ProcessRead(std::string buffer)
{
	LLOG(L_TRACE,"EnterInto ProcessRead .");
	int nret=-1;
	SMProtocal  protocal;
	if ( protocal.PrasePackage(buffer)==0 )
	{
		if ( protocal.GetQpType()==REQUEST) //请求
		{
			if ( protocal.GetPkgType()==TASK)  //下发任务
			{
				Json::Value body=protocal.GetBody();
				protocal.SetQpType(RESPONSE);
				if ( m_TaskManager->AddTask(body.asString())==0 )
				{
					Json::Value code;
					code["Code"]="Sucess";
					protocal.SetBody(code);
				}
				else
				{
					Json::Value code;
					code["Code"]="Fail";
					protocal.SetBody(code);
				}
				std::string response;
				protocal.BuildPackage(response);
				evbuffer_add(bufferevent_get_output(m_buffer_event),response.c_str(), response.size());
			}
			else
			{
				LLOG(L_ERROR,"Package is error.");
			}
		}
		else if (protocal.GetQpType()==RESPONSE) //回应
		{
			switch ( protocal.GetPkgType() )
			{
			case LOGIN: //登陆
				{
					Json::Value  body=protocal.GetBody();
					if ( body["Code"].asString()=="Sucess" )
					{
						LLOG(L_ERROR,"Login Sucess.");
						m_slave_status=Logined; //已登录
						//登陆成功后开启心跳线程
						unsigned int  threadid;
						m_hThread=thread_create(NULL,0,(THREAD_FUN)ThreadDoAffairs, (void*)this , 0, &threadid);

					}
					else
					{
						LLOG(L_ERROR,"Login error .");
					}
				}
 				break;
			case HEARTBEAT: // 心跳
				{
					Json::Value body=protocal.GetBody();
					if ( body["Code"].asString()=="Sucess" )
					{
						LLOG(L_DEBUG,"HeatBeat response.");	
					}
				}
				break;
			case RESULT:  //结果
				{
					Json::Value body=protocal.GetBody();
					if ( body["Code"].asString()=="Sucess" )
					{
						LLOG(L_DEBUG,"Server responsed to RESULT.");
					}
				}
				break;
			case EXIT:     //准备退出
				{
					Json::Value body=protocal.GetBody();
					if ( body["Code"].asString()=="Sucess" )
					{
						LLOG(L_TRACE,"Exit Sucess. ");
					}
					else
					{
						LLOG(L_TRACE,"Exit Failed .");
					}
				}
				break;
			default:
				LLOG(L_ERROR,"Package is error .");
				break;
			}
		}
	}
	else
	{
		LLOG(L_ERROR, "Package is Error .");
	}
	LLOG(L_TRACE,"ExitFrom ProcessRead .");
	return nret;
}

