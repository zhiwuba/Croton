#ifndef  __CROTON_SLAVE_DBFWORKER_H__
#define __CROTON_SLAVE_DBFWORKER_H__
#include "BaseWorker.h"


struct DBF_LittleTask
{
	char url[1024];
	char file_name[512];
	double start_pos;
	double end_pos;
	char ftp_ip[32];
	char ftp_user[64];
	char user_pwd[64];
	char dest_path[512];
};

class DBFWorker : public  BaseWorker
{
public:
	DBFWorker();
	virtual ~DBFWorker();
	virtual int  AssignTask(std::string task_content);
	virtual int  ProcessAndSubmit();
	virtual TASK_STATUS&  GetStatus(){return m_task_status;};  //返回完成的百分比 0--100. 
	virtual TASKID&  GetTaskID(){return m_task_id;};
	virtual int   GetProgress();          //获取进度
protected:
	DBF_LittleTask  m_task_content;

private:
	static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t write_callback(void* ptr,size_t size, size_t nmemb, void* stream);
	static int     progress_callback(float *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

	int DownloadFileFragment(const char* url, double start_pos, double end_pos, const  char* filename);    //下载文件
	int UploadToServer(const char* ftp_ip, const char* ftp_account, const char* ftp_passwd,
								   const char* sourcefile, const char* destfile); //FTP上传到Server
	static  unsigned int __stdcall ThreadProcess(void* p);
	
	handle_thread m_hThread;
	float   m_DownloadProgress;   //下载进度
	float   m_UploadProgress;       //上传进度

	TASKID m_task_id;
	TASK_STATUS  m_task_status;
};

#endif
