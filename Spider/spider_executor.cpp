#include "spider_executor.h"
#include "spider_http_client.h"
#include "spider_website.h"
#include "spider_common.h"



Spider_Executor::Spider_Executor()
{
	m_exit=false;
}


Spider_Executor::~Spider_Executor()
{

}

int Spider_Executor::initialize()
{
	m_queue_mutex=recursivemutex_create();
	m_thread_pool=new Spider_Thread_Pool(kThreadPoolSize);
	m_thread_handle=thread_create(NULL,NULL,(THREAD_FUN)main_thread, this,NULL,NULL);
	return 0;
}

int Spider_Executor::uninitialize()
{
	m_exit=true;
	thread_waitforend(m_thread_handle,INFINITE);
	thread_close(m_thread_handle);
	recursivemutex_destory(m_queue_mutex);
	return 0;
}


int Spider_Executor::put_url(UrlPtr url_ptr)
{
	Recursive_Lock lock(m_queue_mutex);
	LLOG(L_TRACE,"Executor: put_urls %s. \n", url_ptr->url);
	m_task_queue.push(url_ptr);
	return 0;
}

int Spider_Executor::put_urls(UrlPtrVec& url_ptrs)
{
	Recursive_Lock lock(m_queue_mutex);
	for (int i=0; i<url_ptrs.size(); ++i)
	{
		LLOG(L_TRACE,"Executor: put_urls %s. \n", url_ptrs[i]->url);
		m_task_queue.push(url_ptrs[i]);
	}
	return 0;
}



int Spider_Executor::main_thread_aid()
{
#ifdef WIN32
	Spider_Http_Client http_client;
	std::map<int, UrlPtr> socket_url_map;
	FD_ZERO(&m_all_fdset);
	
	while  (!m_exit)
	{
		//FD_SET()  FD_CLR()  FD_ISSET()  FD_ZERO()
		int maxfds=0;
		
		if ( m_all_fdset.fd_count< kMinSet)
		{
			int count=0;
			Recursive_Lock lock(m_queue_mutex);
			while(count<kProcessCountPer&&m_task_queue.size()>0)
			{
				UrlPtr url_ptr=m_task_queue.front();
				m_task_queue.pop();
				count++;
				int sock=http_client.send_request(url_ptr);
				if ( sock>0 )
				{
					FD_SET(sock,&m_all_fdset);
					socket_url_map[sock]=url_ptr;
				}//if
			}//while
		}//if
	
		if ( m_all_fdset.fd_count>0  )
		{//数量大于0
			fd_set fd_read=m_all_fdset;
			int ret=select(maxfds,&fd_read,NULL,NULL,NULL );
			switch(ret)
			{
			case SOCKET_ERROR:
				LLOG(L_ERROR,"select error,code %d.", lasterror);
				break;
			case 0:  //超时
				LLOG(L_ERROR,"select timeout,code %d.", lasterror);
				break;
			default: //有多个
				for(int i=0; i<(int)m_all_fdset.fd_count; i++)
				{
					if(FD_ISSET(m_all_fdset.fd_array[i],&fd_read))
					{
						Job* job=new Job;
						job->execute_func_=worker_work;
						job->url_ptr_=socket_url_map[m_all_fdset.fd_array[i]];
						job->http_client_=&http_client;
						job->sock_=m_all_fdset.fd_array[i];
						m_thread_pool->push_work(job);
						FD_CLR(m_all_fdset.fd_array[i],&m_all_fdset);
					}
				}//for
				break;
			}//switch
		}//if
		
		Sleep(500);
	}//while

#else
	//linux  epoll

#endif 

	return 0;
}


int Spider_Executor::main_thread(void* param)
{
	Spider_Executor* pthis=(Spider_Executor*)param;
	pthis->main_thread_aid();	
	return 0;
}



int Spider_Executor::worker_work(void* param)
{
	Job* job=(Job*)param;
	if ( job!=NULL )
	{
		UrlPtr& url_ptr=job->url_ptr_; 
		int status_code=job->http_client_->recv_response(job->sock_,&url_ptr->response, url_ptr->length);
		if ( status_code==200 )
		{
			if ( url_ptr->belong!=NULL )
			{
				((Spider_WebSite*)url_ptr->belong)->process(url_ptr);
			}
		}
		else if (status_code==302)
		{
			UrlPtr new_url_ptr=create_url(url_ptr->response, url_ptr->type);
			new_url_ptr->belong=url_ptr->belong;
			Spider_Executor::instance().put_url(new_url_ptr);
		}
	}

	return 0;
}
