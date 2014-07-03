#include "Slave.h"
#include "MSProtocal.h"
#include "TaskManager.h"

#define MAX_LINE_LENGTH 10240


Slave::Slave(bufferevent* bev, ulong slave_id): m_bufferevent(bev)
{
	m_status=UNLOGIN;
	m_slave_id=slave_id;
	m_time_report_heartbeat=GetTickCount();
}

Slave::~Slave()
{
	RetrieveTask(); //回收
}

int  Slave::ProcessRead(struct bufferevent* bev)
{
	struct evbuffer* input=bufferevent_get_input(bev);
	struct evbuffer* output=bufferevent_get_output(bev);

	while (1)
	{
		size_t eol_len=0;
		struct evbuffer_ptr eol=evbuffer_search_eol(input,NULL,&eol_len,EVBUFFER_EOL_LF);
		if (eol.pos < 0)
		{
			// not found '\n'
			size_t readable = evbuffer_get_length(input);
			if (readable > MAX_LINE_LENGTH) 
			{
				LLOG(L_ERROR, "input is too long %d.", readable);
				bufferevent_free(bev);
			}
			break;
		}
		else if (eol.pos > MAX_LINE_LENGTH)
		{
			LLOG(L_ERROR ,"line is too long %d", eol.pos);
			bufferevent_free(bev);
			break;
		}
		else
		{
			assert(eol_len == 1);
			// copy input buffer to request
			char request[MAX_LINE_LENGTH+1];
			assert(eol.pos+1 <= sizeof(request));
			int req_len = evbuffer_remove(input, request, eol.pos+1);
			assert(req_len == eol.pos+1);
			request[req_len]='\0';
			std::string response;
			int ret=PraseAndResponse(std::string(request), response);
			if ( ret==0 && !response.empty() )
			{
				evbuffer_add(output, response.c_str(), response.size());
			}
		}
	}
	return 0;
}

int Slave::ProcessWrite(struct bufferevent* bev )
{
	return  0;
}

int Slave::DoTask(std::string& task_content ,TASKID& task_id)
{
	if ( m_status==LOGINED )
	{
		std::string pack;
		MSProtocal protocal;
		protocal.SetPkgType(TASK);
		protocal.SetQpType(REQUEST);
		Json::Value body=task_content;
		protocal.SetBody(body);
		protocal.BuildPackage(pack);

		evbuffer_add(bufferevent_get_output(m_bufferevent), pack.c_str(), pack.length());
		m_TaskList.push_back(task_id);
	}
	else
	{
		LLOG(L_ERROR,"The Slave is Dead , DoTask error.");
		return -1;
	}
	return 0;
}

ulong Slave::GetSlaveID()
{
	return m_slave_id;
}

bool Slave::CheckTimeOut()
{
	ulong timeout=GetTickCount()-m_time_report_heartbeat;
	if ( timeout>SLAVE_TIMEOUT ) //两分钟超时
	{
		m_status=DEAD;
		return true;
	}
	else
	{
		return false;
	}
}


bool  Slave::CanBeAssigned()  //可以分配任务的
{
	bool nret=false;
	if ( m_status==LOGINED&&m_TaskList.size()<3 )
	{
		nret=true;
	}
	return nret;
}

int Slave::PraseAndResponse(std::string& request, std::string& response)
{
	int nret=0;
	MSProtocal  protocal;
	if ( 0==protocal.PrasePackage(request ))
	{
		if ( protocal.GetQpType()==REQUEST )
		{
			switch ( protocal.GetPkgType() )
			{
			case LOGIN:  /*对登陆的响应*/
				{
					LLOG(L_TRACE,"New Slave Login .");
					MSProtocal  package;
					Json::Value body;
					package.SetPkgType(LOGIN);
					package.SetQpType(RESPONSE);
					if ( m_status!=LOGINED )
					{
						m_status=LOGINED;
						body["Code"]="Sucess";
					}
					else
					{
						body["Code"]="Fail";
						LLOG(L_WARN," Login twice.");
					}
					package.SetBody(body);					
					package.BuildPackage(response);
				}
				break;
			case HEARTBEAT:  /*对心跳的响应*/
				{
					MSProtocal  package;
					Json::Value body;
					package.SetPkgType(HEARTBEAT);
					package.SetQpType(RESPONSE);
					body["Code"]="Sucess";
					package.SetBody(body);
					
					m_time_report_heartbeat=GetTickCount();
					package.BuildPackage(response);
				}
				break;
			case RESULT:  /*对result的响应*/
				{
					Json::Value body=protocal.GetBody();
					if ( body.size()==3 )
					{
						int taskid=body["TaskID"].asInt();
						int littletaskid=body["LittleTaskID"].asInt();
						MSProtocal  package;
						package.SetPkgType(RESULT);
						package.SetQpType(RESPONSE);
						Json::Value  temp;
						temp["TaskID"]=body["TaskID"];
						temp["LittleTaskID"]=body["LittleTaskID"];
						temp["Code"]="Sucess";
						package.SetBody(temp);
						if ( "Sucess"==body["Code"].asString() )
						{
							LLOG(L_DEBUG,"Task is Completed .");
							TaskManager::Instance().SetTaskStatus(taskid , littletaskid, Completed);
							TASKID temp={taskid, littletaskid};
							m_TaskList.remove(temp);
						}
						else if ( "Fail"==body["Code"].asString() )
						{  /*放弃任务，再重新分配*/
							LLOG(L_ERROR,"Task is Abandoned.");
							TaskManager::Instance().SetTaskStatus(taskid, littletaskid, UnAssigned); 
						}
						package.BuildPackage(response);
					}
				}
				break;
			case EXIT:  /*对Exit 的响应*/
				{
					m_status=DEAD;
					RetrieveTask();
					m_TaskList.clear();
					MSProtocal package;
					Json::Value body;
					body["Code"]="Sucess";
					package.SetPkgType(EXIT);
					package.SetQpType(RESPONSE);
					package.SetBody(body);
					package.BuildPackage(response);
				}
				break;
			default:
				LLOG(L_WARN,"Bad PkgType In PraseAndResponse.");
				nret=-1;
				break;
			}
		}
		else if ( protocal.GetQpType()==RESPONSE )
		{
			switch( protocal.GetPkgType() )
			{
			case TASK:  /*对下发任务的处理 不对客户端响应*/
				{
					Json::Value body=protocal.GetBody();
					if ( body.size()>=1 )
					{
						if ( "Sucess"==body["Code"].asString())
						{
							LLOG(L_DEBUG,"Assign The Task Successfully.");
						}
						else if ( "Fail"==body["Code"].asString() )
						{
							LLOG(L_ERROR,"Failed To Assign The Task.");
							if ( body.size()==3 )
							{
								int taskid=body[1].asInt();
								int littletaskid=body[2].asInt();
								TaskManager::Instance().SetTaskStatus(taskid, littletaskid , UnAssigned );
								TASKID temp={ taskid, littletaskid };
								m_TaskList.remove(temp);
							}
						}
					}
				}
				break;
			default:
				LLOG(L_ERROR,"Bad PkgType in response from client.");
			}
		}
		else
		{
			LLOG(L_WARN, "Bad QpType in PraseAndResponse.");
			nret=-1;		
		}
	}
	else
	{
		LLOG(L_WARN,"Bad Data  In PraseAndResponse.");
		nret=-1;
	}
	return nret;
}


int Slave::RetrieveTask()
{
	std::list<TASKID>::iterator iter=m_TaskList.begin(); 
	for ( ;iter!=m_TaskList.end() ;  iter++ )
	{
		TaskManager::Instance().SetTaskStatus(iter->task_id,iter->inner_id,UnAssigned);
	}
	return 0;
}
