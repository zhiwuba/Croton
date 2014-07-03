#include "DownloadBigFile.h"
#include "curl/curl.h"
#include "json/json.h"
#include "Config.h"

const long kFileSegmentSize=10*1024*1024;  //100M
const char* kDestFilePath="./Results/";

unsigned int GenerateUUID(const char* str)
{  //BKDRHash
	if ( str==NULL )
	{
		return 0;
	}
	else
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return (hash & 0x7FFFFFFF);
	}
}


DownloadBigFile::DownloadBigFile()
{
	m_task_id=0;
}

DownloadBigFile::~DownloadBigFile()
{

}

int   DownloadBigFile::SetAndCheckUrl(std::string url)
{
	int nret=0;
	m_url=url;
	if (url.empty() || url.find("http://")==std::string::npos||url.find('.')==std::string::npos)
	{
		LLOG(L_ERROR, "url is error.");
		nret= -1;
	}
	else
	{
		m_task_id=GenerateUUID(url.c_str());
		m_file_name=url.substr(url.find_last_of('/')+1 );
	}
	return nret;
}

int DownloadBigFile::Decompose()
{
	LLOG(L_TRACE,"EntetInto DownloadBigFile::Decompose.");
	double file_length=GetFileLength(m_url);
	int i=0;
	double start_pos=0;
	for (double left_length=file_length;left_length>0;i++ )
	{
		DBF_LittleTask  little_task;
		strcpy(little_task.url, m_url.c_str());
		little_task.start_pos=start_pos;
		sprintf(little_task.file_name,"%s.%d",m_file_name.c_str(), i);
		little_task.task_status=UnAssigned;
		little_task.inner_id=i;
		if ( left_length<kFileSegmentSize )
		{
			little_task.end_pos=file_length;
			m_LittleTaskList.push_back(little_task);
			break;
		}
		
		start_pos+=kFileSegmentSize;	
		little_task.end_pos=start_pos;
		left_length-=kFileSegmentSize;
		m_LittleTaskList.push_back(little_task);
	}
	LLOG(L_TRACE,"ExitFrom DownloadBigFile::Decompose.");
	return 0;
}

bool DownloadBigFile::IsCompleted()
{
	bool  completed=true;
	std::list<DBF_LittleTask>::iterator iter=m_LittleTaskList.begin();
	for ( ; iter!=m_LittleTaskList.end() ; iter++ )
	{
		if ( iter->task_status!=Completed )
		{
			completed=false;
			break;
		}
	}
	return completed;
}

void DownloadBigFile::Combine()
{
	LLOG(L_TRACE,"EnterInto Combine BigFile.");
	std::list<DBF_LittleTask>::iterator iter=m_LittleTaskList.begin();
	for ( ; iter!=m_LittleTaskList.end() ;  iter++)
	{
		MergeFile(iter->file_name,iter->end_pos-iter->start_pos,m_file_name.c_str(),iter->start_pos);
	}
	LLOG(L_TRACE,"Exit Combine BigFile.");
}

int  DownloadBigFile::GetLittleTask(std::string& little_task )
{
	int ret=-1;
	std::list<DBF_LittleTask>::iterator iter= m_LittleTaskList.begin();
	for ( ; iter!=m_LittleTaskList.end() ; iter++ )
	{
		if ( iter->task_status==UnAssigned )
		{
			if ( LittleTaskToString((DBF_LittleTask*)(&(*iter)), little_task) )
			{
				ret=iter->inner_id;
				iter->task_status=Assigned;
				break;
			}
		}
	}
	return ret;
}

bool  DownloadBigFile::LittleTaskToString(DBF_LittleTask* task, std::string& task_string)
{
	bool bret=false;
	if ( task!=NULL )
	{
		Json::FastWriter writer;
		Json::Value  content;
		std::string config_value;

		content["TaskType"]= "DGF";
		content["TaskID"]=      m_task_id;
		content["LittleTaskID"]=task->inner_id;
		content["Url"]=task->url;
		content["FileName"]=task->file_name;
		content["StartPos"]=  task->start_pos;
		content["EndPos"] =  task->end_pos;
		content["DestPath"]=kDestFilePath;
		int nret=Config::Instance().GetConfig("FtpIp", config_value);
		content["FtpIp"]= (nret==0?config_value:"127.0.0.1");
		nret=Config::Instance().GetConfig("FtpUser",config_value);
		content["FtpUser"]=(nret==0?config_value:"test");
		nret=Config::Instance().GetConfig("FtpPwd",config_value);
		content["FtpPwd"]=(nret==0?config_value:"test");
		content["DestPath"]=task->file_name;
		task_string=writer.write(content);

		//sprintf(task_string ,"<DGF><%d_%d><%s><%s><%d><%d><%d>",
		//	m_task_id,task->inner_id,task->url, task->file_name,task->start_pos,task->end_pos,task->task_status);
		bret=true;
	}
	return bret;
}


double DownloadBigFile::GetFileLength(std::string url)
{
	double file_length=0;
	CURL *handle=curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(handle, CURLOPT_HEADER,1); //只需要header头
	curl_easy_setopt(handle, CURLOPT_NOBODY,1); //不需要body
	
	if ( curl_easy_perform(handle)==CURLE_OK )
	{
		curl_easy_getinfo(handle,CURLINFO_CONTENT_LENGTH_DOWNLOAD, &file_length);
	}
	else
	{
		file_length=-1;
	}
	
	curl_easy_cleanup(handle);
	return file_length;
}

int DownloadBigFile::MergeFile(const char* sourcefile, int sourcefile_length ,const char* destfile, int destfile_offset)
{
	FILE* source=fopen(sourcefile ,"rb");
	if ( source==NULL )
	{
		LLOG(L_ERROR,"Open SourceFile Error.");
		return -1;
	}
	FILE* dest=fopen(destfile,"a+");
	if (dest==NULL)
	{
		fclose(source);
		LLOG(L_ERROR,"Open Dest File Error.");
		return -1;
	}
	//TODO: 大文件的合并研究
	int read_size=0;
	char buffer[1024*512];

	do
	{
		read_size=fread(buffer,1,1024*512, source);
		fwrite(buffer,1, read_size, dest);
	}
	while(read_size>0);

	fclose(source);
	fclose(dest);
	return 0;
}


int  DownloadBigFile::SetLittleTaskStatus(int inner_id, TaskStatus status)
{
	int nret=-1;
	std::list<DBF_LittleTask>::iterator iter=m_LittleTaskList.begin();
	for ( ; iter!=m_LittleTaskList.end() ; iter++  )
	{
		if ( iter->inner_id==inner_id )
		{
			iter->task_status=status;
			nret=0;
		}
	}
	return nret;
}
