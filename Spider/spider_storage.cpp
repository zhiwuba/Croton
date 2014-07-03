#include "spider_storage.h"
#include "spider_config.h"
#include "spider_url_rinse.h"
#include "boost/filesystem.hpp"
#include "boost/xpressive/xpressive_dynamic.hpp"
using namespace boost::filesystem;

Spider_Storage::Spider_Storage()
{
	
}

Spider_Storage::~Spider_Storage()
{
	
}

int Spider_Storage::initialize()
{
	m_database=new Spider_Database();
	m_database->initialize();
	
	return 0;
}

int Spider_Storage::uninitialize()
{
	m_database->uninitialize();
	if ( m_database!=NULL )
		delete m_database;

	return 0;
}


int Spider_Storage::write_file(const char* website, const char* albums, UrlPtr url_ptr )
{
	if ( website!=NULL&&albums!=NULL&&url_ptr!=NULL&&url_ptr->filename!=NULL&&url_ptr->response!=NULL )
	{
		boost::filesystem::path  file_path=Spider_Config::instance().module_path_;
		file_path /=website;
		file_path /=Spider_Config::instance().current_date_;
		file_path /=albums;
		bool ret=true;
		if ( !exists(file_path) )
		{
			ret=create_directories(file_path);
		}
		if ( ret )
		{
			file_path /=url_ptr->filename;
			FILE* file=fopen(file_path.string().c_str(),"wb");
			if ( file!=NULL )
			{
				fwrite(url_ptr->response,1, url_ptr->length, file);
				fclose(file);
			}
			m_database->insert_record(website, albums, url_ptr);
		}		
	}
	return 0;
}

int Spider_Storage::write_file(const char* website, UrlPtr url_ptr )
{
	if ( website!=NULL&&url_ptr->filename!=NULL&&url_ptr->response!=NULL )
	{
		if ( false==Spider_Url_Rinse::instance().search_and_record_history(url_ptr) )
		{  
			boost::filesystem::path  file_path=Spider_Config::instance().storage_path_;
			file_path /=website;
			file_path /=Spider_Config::instance().current_date_;
			bool  ret=true;
			if ( !exists(file_path) )
			{
				ret=create_directories(file_path);
			}
			if ( ret )
			{
				file_path/=url_ptr->filename;
				FILE* file=fopen(file_path.string().c_str(),"wb");
				if ( file!=NULL )
				{
					fwrite(url_ptr->response,1,url_ptr->length, file);
					fclose(file);
				}
				m_database->insert_record(website,NULL, url_ptr);
			}	
		}
	}
	return 0;
}

