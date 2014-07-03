#include "TaskManager.h"
#include "Slave.h"
#include "DownloadBigFile.h"
#include "SlaveManager.h"

TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{
	std::map<int,BaseTask* >::iterator iter=m_TaskMap.begin();
	for ( ; iter!=m_TaskMap.end() ; )
	{
		delete iter->second;
		m_TaskMap.erase(iter++);
	}
}

int TaskManager::Process()
{
	LLOG(L_TRACE,"TaskManager Process .");
	AssignTask(); /*给Slave安排任务*/
	UpdateTask();/*更新任务的状态*/
	return 0;
}

int TaskManager::AssignTask()
{
	LLOG(L_TRACE,"TaskManager::AssignTask.");
	std::map<int,BaseTask* >::iterator iter=m_TaskMap.begin();
	for ( ;iter!=m_TaskMap.end() ; iter++)
	{
		Slave* slave;
		while ( NULL!=(slave=SlaveManager::Instance().GetIdleSlave()) )
		{
			int little_task_id=0;
			std::string  task_content;
			little_task_id=iter->second->GetLittleTask(task_content);
			if (little_task_id !=-1 )
			{
				TASKID task_id={ iter->second->GetUUID() , little_task_id };
				slave->DoTask(task_content, task_id);
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

int TaskManager::UpdateTask()
{
	LLOG(L_TRACE, "TaskManager::UpdateTask.");
	/*任务完成者*/
	std::map<int,BaseTask* >::iterator iter=m_TaskMap.begin();
	for ( ;iter!=m_TaskMap.end() ; )
	{
		if ( iter->second->IsCompleted() )
		{
			LLOG(L_TRACE,"Task is Completed.");
			iter->second->Combine();
			delete iter->second;
			m_TaskMap.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
	return 0;
}

int TaskManager::CreateTask(TASK_TYPE task_type , char* url )
{
	LLOG(L_DEBUG,"EnterInto CreateTask, task_type is %d.", task_type);
	int nret=-1;
	switch (task_type)
	{
	case DOWNLOAD_BIG_FILE:
		{
			DownloadBigFile* task_downloadfile=new DownloadBigFile();
			if (0 == task_downloadfile->SetAndCheckUrl(std::string(url))) 
			{
				if ( m_TaskMap.find(task_downloadfile->GetUUID())==m_TaskMap.end() )
				{
					task_downloadfile->Decompose();
					m_TaskMap[task_downloadfile->GetUUID()]=task_downloadfile;
					nret=0;
				}
				else
				{
					delete task_downloadfile;
				}
			}
		}
		break;
	case CRAWL_THE_WEB:
		{
		}
		break;
	default:
		LLOG(L_ERROR , "Error task type.");
		break;
	}
	LLOG(L_TRACE ,"ExitFrom CreateTask .");
	return nret;
}

int  TaskManager::SetTaskStatus( int task_id, int inner_id, TaskStatus status )
{
	int nret=-1;
	std::map<int,BaseTask* >::iterator iter=m_TaskMap.find(task_id);
	if ( iter!=m_TaskMap.end() )
	{
		if (0 == iter->second->SetLittleTaskStatus(inner_id, status) )
		{
			nret=0;
		}
	}
	return nret;
}

