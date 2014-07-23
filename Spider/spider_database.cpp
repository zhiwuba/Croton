#include "spider_database.h"


Spider_Database::Spider_Database()
{
	m_exit=false;
}

Spider_Database::~Spider_Database()
{

}

int Spider_Database::initialize()
{
	if (!mysql_init(&m_mysql) )
	{
		LLOG(L_ERROR,"mysql_init error.");
		return -1;
	}
	else
	{
		int ret=connect();
		if ( ret==0 )
		{
			LLOG(L_TRACE, "connect mysql ok.");
		}
		m_queue_mutex=recursivemutex_create();
		m_thread=thread_create(NULL,0,(THREAD_FUN)thread_proc, this, 0,0);
	}
	return 0;
}

int Spider_Database::uninitialize()
{
	m_exit=true;
	thread_waitforend(m_thread,INFINITE);
	recursivemutex_destory(m_queue_mutex);
	close();
	return 0;
}

int Spider_Database::thread_aid()
{
	while ( !m_exit )
	{
		if ( m_commands.size()>0 )
		{
			std::string command;
			{
				Recursive_Lock lock(m_queue_mutex);
				command=m_commands.front();
				m_commands.pop();
			}
			int ret=query(command.c_str());
			if ( ret!=0 )
			{
				LLOG(L_ERROR,"insert_record error. ");
				return -1;
			}
		}
		else
		{
			Sleep(500);
		}
	}
	return 0;
}

int Spider_Database::thread_proc(void* param)
{
	Spider_Database* pthis=(Spider_Database*)param;
	if ( pthis!=NULL)
	{
		pthis->thread_aid();
	}
	return 0;
}

int Spider_Database::insert_record(const char* website, const char* albums, UrlPtr url_ptr)
{
	Recursive_Lock lock(m_queue_mutex);

	std::string file_path="original/"+url_ptr->filename;
	std::string thumb_path="thumb/"+url_ptr->filename;

	char command[2048];
	sprintf(command, "INSERT INTO hd_paints (file_path,thumb_path,date_added,header,comment,source_url,source_website) VALUES ('%s' , '%s', NOW(), '%s', '%s', '%s', '%s')",
		file_path.c_str(), 
		thumb_path.c_str(), 
		"", 
		url_ptr->comment,
		url_ptr->url,
		website );

	m_commands.push(std::string(command));
	return 0;
}

int Spider_Database::connect()
{
	Spider_Config::instance();
	MYSQL* ret=mysql_real_connect(&m_mysql, 
		Spider_Config::instance().mysql_host_.c_str(),
		Spider_Config::instance().mysql_user_.c_str(),
		Spider_Config::instance().mysql_password_.c_str(),
		Spider_Config::instance().mysql_db_.c_str(),
		Spider_Config::instance().mysql_port_,NULL,0);
	if ( ret==NULL )
	{
		LLOG(L_ERROR,"mysql: connect server error!!");
		return -1;
	}
	return 0;
}

int Spider_Database::query(const char* command)
{
	int ret=mysql_real_query(&m_mysql, command, strlen(command));
	if ( ret !=0)
	{
		LLOG(L_ERROR, "query error, code: %s ", mysql_error(&m_mysql));
		return -1;
	}
	return 0;
}

int Spider_Database::close()
{
	mysql_close(&m_mysql);
	return 0;
}

