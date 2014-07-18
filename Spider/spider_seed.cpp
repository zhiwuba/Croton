#include "spider_seed.h"
#include "spider_config.h"
#include "pugixml/pugixml.hpp"


void Seed::set_pic_size( const char* c )
{
	pic_size_.first=atoi(c)*1024;
	while(*c!='-'&&*c!='\0')c++;
	pic_size_.second=(*c=='-')?atoi(++c)*1024:0;
}



/////////////////////////////////////////////////
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
	pugi::xml_document doc;
	doc.load_file(seed_path.c_str());

	const char* query_path="/seeds";
	pugi::xpath_node_set seed_set=doc.select_nodes(query_path);

	for ( pugi::xpath_node_set::const_iterator iter=seed_set.begin(); iter!=seed_set.end(); ++iter )
	{
		pugi::xml_node vnode=iter->node();
		if ( vnode )
		{
			std::string seed_name=vnode.attributes("name");
			std::string pic_size_str=vnode.attributes("pic_size");

			Seed* seed=new Seed;

			pugi::xpath_node_set start_urls=vnode.select_nodes("start/url");
			pugi::xpath_node_set pic_urls=vnode.select_nodes("pic/url");
			pugi::xpath_node_set index_urls=vnode.select_nodes("index/url");

			for ( pugi::xpath_node_set::const_iterator start_iter=seed_set.begin(); start_iter!=seed_set.end(); ++start_iter )
			{
				seed->start_url_.push_back(start_iter->node().value());
			}

			for ( pugi::xpath_node_set::const_iterator pic_iter=seed_set.begin(); pic_iter!=seed_set.end(); ++pic_iter )
			{
				seed->pic_url_.push_back(pic_iter->node().value());
			}

			for ( pugi::xpath_node_set::const_iterator index_iter=seed_set.begin(); index_iter!=seed_set.end(); ++index_iter )
			{
				seed->index_url_.push_back(index_iter->node().value());
			}
	
			seed->set_pic_size(pic_size_str.c_str());
			m_seeds[seed_name]=seed; //´æ´¢
		}
	}

	return 0;
}
