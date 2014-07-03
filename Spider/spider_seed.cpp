#include "spider_seed.h"
#include "spider_config.h"

const char* kSeedSubject[]={"general","special",NULL};
enum SUBJECT
{
	GENERAL,
	SPECIAL,
};

Spider_Seed::Spider_Seed()
{
	
}


Spider_Seed::~Spider_Seed()
{
	
}

int Spider_Seed::get_seed(std::string& website, Seed** seed )
{
	std::map<std::string,Seed*>::iterator iter=m_seeds.begin();
	if ( iter!=m_seeds.end() )
	{
		website=iter->first;
		*seed=iter->second;
		m_seeds.erase(iter);
	}
	return 0;
}

int Spider_Seed::load()
{
	std::string seed_path=Spider_Config::instance().module_path_+kSeedFileName;
	FILE* file=fopen(seed_path.c_str(), "r");
	if ( file!=NULL )
	{
		SUBJECT subject;
		std::string website;
		char line[1024];
		while (fgets(line,1024,file))
		{
			char* c=line;
			while (isspace(*c))c++;
			if ( *c=='[' )
			{
				char sub[100];
				int ret=prase_subject(line,sub);
				if ( ret==0 )
				{
					int i=0;
					for (; kSeedSubject[i]!=NULL ;i++)
					{
						if ( strcmp(sub,kSeedSubject[i])==0 )
							break;
					}
					subject=(SUBJECT)i;
				}
			}
			else if ( *c!='\0' )
			{
				std::string key, value;
				int ret=prase_key_value(line, key,value);
				if ( ret==0 )
				{
					if ( subject==GENERAL )
						set_seed(website,key,value);
					else if ( subject==SPECIAL )
						set_seed(website,key,value);
				}
			}
		}
	}

	return 0;
}

int  Spider_Seed::set_seed(std::string& website, std::string& key, std::string& value )
{
	if ( website.empty()&&key!="website")
	{
		LLOG(L_ERROR, "set_seed error.");
		return -1;
	}
	if ( key=="website" )
	{
		website=value;
		Seed* seed=new Seed;
		m_seeds[website]=seed;
	}
	else if ( key=="url" )
	{
		Seed* seed=m_seeds[website];
		if ( seed!=NULL )
		{
			split_string(value, seed->url_," ");
		}
	}
	else if ( key=="index" )
	{
		Seed* seed=m_seeds[website];
		if ( seed!=NULL )
		{
			split_string(value, seed->index_," ");
		}	
	}
	else if ( key=="pic" )
	{
		Seed* seed=m_seeds[website];
		if ( seed!=NULL )
		{
			split_string(value, seed->pic_ , " ");
		}
	}
	else if (key=="pic_size")
	{
		Seed* seed=m_seeds[website];
		if ( seed!=NULL )
		{
			const char*c =value.c_str();
			seed->pic_size_.first=atoi(c)*1024;
			while(*c!='-'&&*c!='\0')c++;
			seed->pic_size_.second=(*c=='-')?atoi(++c)*1024:0;
		}
	}

	return 0;
}


int Spider_Seed::prase_subject(char* line, char* subject)
{
	int ret=-1;
	char* c=line;
	while (isspace(*c))c++;
	if ( *c=='[' )
	{
		char* s=c;
		s++;
		c=line+strlen(line)-1;
		while (isspace(*c))c--;
		if ( *c==']' )
		{
			ret=0;
			strncpy(subject, s, c-s);
			subject[c-s]='\0';
		}
	}
	return ret;
}


int Spider_Seed::prase_key_value(char* line, std::string& key, std::string& value)
{
	int ret=-1;
	char* c=line;
	while(isspace(*c))c++;
	char* s=c;
	while (*c!=':'&&*c!='\0')c++;
	if ( *c!='\0'&&*c==':' )
	{
		char* k=c;
		while(isspace(*k))k--;
		key.assign(s, k-s);

		s=line+strlen(line)-1;
		while ( isspace(*s) )s--;
		c++;
		while (isspace(*c))c++;

		if ( s>=c )
		{
			value.assign(c, s+1-c);
			ret=0;
		}
	}
	return ret;
}

