#ifndef  __CROTON_MASTER_DOWNLOADBIGFILE_H__
#define __CROTON_MASTER_DOWNLOADBIGFILE_H__
#include  "Porting/CrotonPorting.h"
#include "BaseTask.h"
#include "Common.h"

struct DBF_LittleTask 
{
	char url[1024];
	char file_name[512];
	double start_pos;
	double end_pos;
	TaskStatus task_status;
	int  inner_id;
};


class  DownloadBigFile:public BaseTask
{
public:
	DownloadBigFile();
	virtual ~DownloadBigFile();
	virtual int  Decompose();
	virtual void  Combine();
	virtual bool  IsCompleted();

	int    SetAndCheckUrl(std::string url);
	int    SetLittleTaskStatus(int inner_id , TaskStatus status); //设置小任务的状态
	int    GetLittleTask(std::string& little_task);
	inline unsigned int GetUUID(){return m_task_id;};
private:
	int MergeFile(const char* sourcefile, int sourcefile_length ,const char* destfile, int destfile_offset);
	double GetFileLength(std::string url);
	bool     LittleTaskToString(DBF_LittleTask* task, std::string& task_string);
	std::list<DBF_LittleTask> m_LittleTaskList;
	std::string m_file_name;
	std::string m_url;
	unsigned int m_task_id;
};

#endif
