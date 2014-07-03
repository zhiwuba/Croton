#ifndef  __CROTON_SPIDER_EXECUTOR_H__
#define __CROTON_SPIDER_EXECUTOR_H__
#include "spider_url.h"
#include "spider_thread_pool.h"

class Spider_Executor
{
public:
	static Spider_Executor&  instance()
	{
		static Spider_Executor _instance;
		return _instance;
	};
	~Spider_Executor();
	int  initialize();
	int  uninitialize();

	int put_url(UrlPtr url_ptr);
	int put_urls(UrlPtrVec& url_ptrs);

	int wait_complete();
	
private:
	Spider_Executor();
	int              main_thread_aid();
	static  int    main_thread(void* param);
	static  int    worker_work(void* param);
	
	handle_recursivemutex m_queue_mutex;
	std::queue<UrlPtr>      m_task_queue;
	handle_thread              m_thread_handle; 
	handle_semaphore       m_complete;

	Spider_Thread_Pool*  m_thread_pool;

	fd_set  m_all_fdset; 
	
	volatile bool m_exit;
};


#endif
