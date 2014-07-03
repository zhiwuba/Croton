#include "TaskManager.h"
#include "BaseWorker.h"
#include "DBFWorker.h"
#include "json/json.h"
#include "SMProtocal.h"

TaskManager::TaskManager()
{
	m_TaskListMutex=mutex_create();
}

TaskManager::~TaskManager()
{
	close_mutex(m_TaskListMutex);
}


int  TaskManager::Process(bufferevent* buffer_event)
{
	wait_for_mutex(m_TaskListMutex,INFINITE);
	std::list<BaseWorker*>::iterator  iter=m_TaskList.begin();
	for ( ; iter!=m_TaskList.end() ; )
	{
		if ( (*iter)->GetStatus()==SUBMITED )
		{
			/*发送Result命令*/
			SMProtocal protocal;
			protocal.SetPkgType(RESULT);
			protocal.SetQpType(REQUEST);
			Json::Value body;
			body["TaskID"]=(*iter)->GetTaskID().task_id;
			body["LittleTaskID"]=(*iter)->GetTaskID().inner_id;
			body["Code"]="Sucess";
			protocal.SetBody(body);
			std::string pack_content;
			protocal.BuildPackage(pack_content);
			evbuffer_add(bufferevent_get_output(buffer_event), pack_content.c_str(), pack_content.length());
			/*删除任务*/
			m_TaskList.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
	release_mutex(m_TaskListMutex);
	return 0;
}


int TaskManager::AddTask(std::string task_content)
{
	LLOG(L_TRACE,"EnterInto TaskManager::AddTask.");
	int nret=0;
	Json::Value body;
	Json::Reader  reader;
	if ( reader.parse(task_content,body) )
	{
		if ( body["TaskType"]=="DGF" )  //下载文件
		{
			DBFWorker* worker=new DBFWorker();
			worker->AssignTask(task_content);
			worker->ProcessAndSubmit();  /*异步处理*/
			wait_for_mutex(m_TaskListMutex,INFINITE);
			m_TaskList.push_back(worker);
			release_mutex(m_TaskListMutex);
		}
		else if (body["TaskType"]=="CTW") //爬取网页
		{
			
		}
		else
		{
			LLOG(L_ERROR,"TaskType is error.");
			nret=-1;
		}
	}
	LLOG(L_TRACE,"ExitFrom TaskManager::AddTask.");
	return nret;
}


