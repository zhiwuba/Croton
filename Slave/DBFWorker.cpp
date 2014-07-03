#include "DBFWorker.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "Porting/CrotonPorting.h"
#include "curl/curl.h"
#include "json/json.h"

DBFWorker::DBFWorker()
{
	m_hThread=NULL;
	m_UploadProgress=0.0;
	m_DownloadProgress=0.0;
}

DBFWorker::~DBFWorker()
{
	if ( m_hThread!=NULL )
	{
		thread_waitforend(m_hThread, INFINITE);
		thread_close(m_hThread);
	}
}

int DBFWorker::AssignTask(std::string task_content)
{
	int nret=-1;
	Json::Reader reader;
	Json::Value   body;
	if ( reader.parse(task_content, body)  )
	{
		strcpy( m_task_content.url , body["Url"].asCString());
		strcpy( m_task_content.file_name, body["FileName"].asCString() );
		m_task_content.start_pos=body["StartPos"].asDouble();
		m_task_content.end_pos=body["EndPos"].asDouble();
		strcpy(m_task_content.ftp_ip, body["FtpIp"].asCString() );
		strcpy(m_task_content.ftp_user, body["FtpUser"].asCString());
		strcpy(m_task_content.user_pwd, body["FtpPwd"].asCString());
		strcpy(m_task_content.dest_path, body["DestPath"].asCString());

		m_task_id.task_id=body["TaskID"].asInt();
		m_task_id.inner_id=body["LittleTaskID"].asInt();
		nret=0;
	}
	return nret;
}

int  DBFWorker::ProcessAndSubmit()
{
	unsigned int  threadid;
	m_hThread=thread_create(NULL,0,(THREAD_FUN)ThreadProcess, (void*)this , 0, &threadid);
	return 0;
}

int DBFWorker::GetProgress()
{
	int nret=0;
	if ( m_DownloadProgress<100 )
	{
		nret=m_DownloadProgress/2;
	}
	else
	{
		nret=(m_DownloadProgress+m_UploadProgress)/2;
	}
	return nret;
}

int  DBFWorker::progress_callback(float* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	*clientp=(float)((dlnow*100)/dltotal);   //下载百分比
	return  0;
}

size_t DBFWorker::write_callback(void* ptr,size_t size, size_t nmemb, void* stream)
{
	size_t ret=fwrite(ptr,size, nmemb ,(FILE*)stream);
	return ret;
}

int DBFWorker::DownloadFileFragment(const char* url, double start_pos, double end_pos, const  char* filename )    //下载文件
{
	LLOG(L_TRACE,"EnterInto DownloadFileFragment .");
	int nret=-1;
	if ( url!=NULL&&filename!=NULL&&start_pos>0&&end_pos>start_pos )
	{
		char range[256];
		sprintf(range,"%d-%d",(int)start_pos,(int)end_pos);
		FILE* file=fopen(filename,"wb");
		if ( file!=NULL )
		{	
			CURL* handler=curl_easy_init();
			CURLcode errorcode;
			curl_easy_setopt(handler,CURLOPT_URL, url);
			curl_easy_setopt(handler,CURLOPT_WRITEDATA, file);
			curl_easy_setopt(handler,CURLOPT_WRITEFUNCTION, write_callback );
			curl_easy_setopt(handler,CURLOPT_NOPROGRESS,false);
			curl_easy_setopt(handler,CURLOPT_PROGRESSDATA, &m_DownloadProgress);
			curl_easy_setopt(handler,CURLOPT_PROGRESSFUNCTION, progress_callback );
			curl_easy_setopt(handler, CURLOPT_RANGE, range );
			errorcode=curl_easy_perform(handler);
			if ( errorcode==CURLE_OK )
			{
				LLOG(L_TRACE,"DownloadFile Sucess.");
				nret=0;
			}
			else
			{
				LLOG(L_ERROR,"DownloadFile Failed.");
			}
			curl_easy_cleanup(handler);
			fclose(file);
		}
		else
		{
			LLOG(L_ERROR,"In DownloadFile, fopen file error.");
		}
	}
	LLOG(L_TRACE,"ExitFrom DownloadFileFragment.");
	return nret;
}

size_t DBFWorker::read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	  curl_off_t nread;
	 /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
	 size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);
	 nread = (curl_off_t)retcode;
	 return retcode;
}

int DBFWorker::UploadToServer(const char* ftp_ip, const char* ftp_account, const char* ftp_passwd,
							  const char* sourcefile, const char* destfile)
{
	LLOG(L_TRACE, "EnterInto DFBWorker::UploadToServer.");
	int nret=-1;
	if ( ftp_ip!=NULL&&ftp_account!=NULL&&sourcefile!=NULL&&destfile!=NULL )
	{
		char ftp_url[1024];
		sprintf(ftp_url, "ftp://%s/%s", ftp_ip, destfile );
		char user_pwd[512];
		sprintf(user_pwd,"%s:%s", ftp_account, ftp_passwd );
	
		struct _stat file_info;
		_stat(sourcefile , &file_info);
		curl_off_t  file_size=(curl_off_t)file_info.st_size;

		FILE *source=fopen(sourcefile,"rb");
		if ( source !=NULL)
		{
			CURL*  handler=curl_easy_init();
			CURLcode errorcode;
			curl_easy_setopt(handler, CURLOPT_UPLOAD, 1L );
			curl_easy_setopt(handler, CURLOPT_URL, ftp_url);
			curl_easy_setopt(handler, CURLOPT_USERPWD, user_pwd );
			curl_easy_setopt(handler, CURLOPT_READDATA, source);
			curl_easy_setopt(handler, CURLOPT_READFUNCTION,read_callback );
			curl_easy_setopt(handler, CURLOPT_INFILESIZE_LARGE, file_size);
			curl_easy_setopt(handler, CURLOPT_NOPROGRESS,false);
			curl_easy_setopt(handler, CURLOPT_PROGRESSDATA, &m_UploadProgress);
			curl_easy_setopt(handler, CURLOPT_PROGRESSFUNCTION, progress_callback);

			errorcode = curl_easy_perform(handler);
			/* Check for errors */
			if(errorcode != CURLE_OK)
			{
				LLOG(L_ERROR ,"curl_easy_perform() failed: %s.",curl_easy_strerror(errorcode));
			}
			else
			{
				LLOG(L_TRACE,"UploadToServer Sucess .");
				nret=0;
			}
			curl_easy_cleanup(handler);
			fclose(source);
		}
		else
		{
			LLOG(L_ERROR, "Can't Find %s .",sourcefile);	
		}
	}

	LLOG(L_TRACE,"ExitFrom DFBWorker::UploadToServer.");
	return nret;
}

// 下载进程
unsigned int __stdcall DBFWorker::ThreadProcess(void* p)
{
	DBFWorker* pThis=(DBFWorker*)p;
	if ( pThis )
	{
		int nret=0;
		DBF_LittleTask& task_content=pThis->m_task_content;
		/*下载文件片段*/
		nret=pThis->DownloadFileFragment(task_content.url, task_content.start_pos, task_content.end_pos, task_content.file_name);
		if ( nret!=0  )
		{
			LLOG(L_ERROR,"DownloadFileFragment error.");
			return -1;
		}
		pThis->m_task_status=PROCESSED; //处理完成
		/*上传结果*/
		nret=pThis->UploadToServer(task_content.ftp_ip, task_content.ftp_user, task_content.user_pwd, task_content.file_name, task_content.dest_path);
		if ( nret!=0 )
		{
			LLOG(L_ERROR,"UploadToServer error.");
			return -1;
		}
		pThis->m_task_status=SUBMITED; //提交完成
	}
	return 0;
}
