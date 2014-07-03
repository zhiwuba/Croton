#include "SlaveManager.h"


SlaveManager::SlaveManager()
{
	
}

SlaveManager::~SlaveManager()
{
	
}

int  SlaveManager::Process()
{
	std::map<ulong,Slave*>::iterator iter=m_SlaveMap.begin();
	for ( ; iter!=m_SlaveMap.end() ; )
	{
		if (  iter->second->CheckTimeOut()  )
		{
			LLOG(L_TRACE, "A slave is timeout, next we should delete it.");
		}
		if ( iter->second->GetStatus()==DEAD )
		{
			delete iter->second;
			m_SlaveMap.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
	return 0;
}

Slave* SlaveManager::AddSlave(bufferevent*  bev , ulong slave_id)
{
	Slave* slave=NULL;
	std::map<ulong, Slave*>::iterator iter=m_SlaveMap.find(slave_id);
	if ( iter==m_SlaveMap.end()  )
	{
		slave=new Slave(bev, slave_id);
		m_SlaveMap[slave_id]=slave;
	}
	else
	{
		slave=iter->second;
	}
	return slave;
}

int SlaveManager::DeleteSlave(ulong id)
{
	int nret=-1;
	std::map<ulong,Slave*>::iterator iter=m_SlaveMap.find(id);
	if ( iter!=m_SlaveMap.end() )
	{
		delete iter->second;
		m_SlaveMap.erase(iter);
		nret=0;
	}
	return nret;
}

Slave* SlaveManager::GetSlave(ulong id)
{
	std::map<ulong,Slave*>::iterator iter=m_SlaveMap.find(id);
	if ( iter!=m_SlaveMap.end() )
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

Slave* SlaveManager::GetIdleSlave()
{
	std::map<ulong, Slave*>::iterator iter=m_SlaveMap.begin();
	for ( ; iter!=m_SlaveMap.end() ;iter++ )
	{
		if ( iter->second->CanBeAssigned() )
		{
			return (Slave*)iter->second;
		}
	}
	return NULL;
}

