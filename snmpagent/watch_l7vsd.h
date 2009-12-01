#ifndef __WATCH_L7VSD_H__
#define __WATCH_L7VSD_H__

#include <string>
#include "tag_threadstat.h"

class	l7ag_queuesendtrap;
class	l7ag_watch_l7vsd
{
private:
	l7ag_queuesendtrap * queueclass;

protected:
	l7ag_watch_l7vsd();

	static void * watch( void * args );

public:
	l7ag_watch_l7vsd( l7ag_queuesendtrap * p_queueclass );
	~l7ag_watch_l7vsd();

	void	set_TrapQueueClass( l7ag_queuesendtrap * p_queueclass ){ queueclass = p_queueclass; }

	void	start_thread();
	void	stop_thread();
	THREAD_STAT_TAG	get_threadstatus();
};

#endif //__WATCH_L7VSD_H__
