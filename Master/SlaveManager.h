#ifndef  __CROTON_MASTER_SLAVEMANAGER_H_ 
#define __CROTON_MASTER_SLAVEMANAGER_H_
#include "Porting/CrotonPorting.h"
#include "Slave.h"

class SlaveManager
{
public:
	static SlaveManager& Instance()
	{
		static SlaveManager _instance;
		return _instance;
	}
	int  Process(); //清楚掉死去的slave
	Slave* AddSlave(bufferevent* bev, ulong slave_id);
	int DeleteSlave(ulong id);
	Slave* GetSlave(ulong id);
	Slave* GetIdleSlave();
	inline int Amount(){ return m_SlaveMap.size(); }

private:
	SlaveManager();
	~SlaveManager();
	SlaveManager(const SlaveManager&);
	SlaveManager& operator=(const SlaveManager&);

	std::map<ulong, Slave*> m_SlaveMap; //ip为key
};

#endif
