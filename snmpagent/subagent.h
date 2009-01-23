#ifndef __SUBAGENT_H__
#define __SUBAGENT_H__

//#include <pthread.h>

#include "UltraMonkeyL7.h"
#include "tag_threadstat.h"
#include "store_mibdata.h"
#include "queue_sendtrap.h"
#include "interproccom.h"
#include "watch_l7vsd.h"

class	l7ag_subagent
{
private:
	bool				stop_flag;
	THREAD_STAT_TAG			stat;
	
	l7ag_store_mibdata		mibdata;
	l7ag_queuesendtrap		trapqueue;
	
	l7ag_interproccommunicate *	communicator;
	
	l7ag_watch_l7vsd *		watchdog;
	
//	pthread_t			t_ipc;
//	pthread_t			t_watcher;

public:
	l7ag_subagent();
	~l7ag_subagent();

	void	do_subagent();
	void	stop_subagent();
	
};

#endif //__SUBAGENT_H__
