#ifndef __INTERPROCCOM_H__
#define __INTERPROCCOM_H__

#include <string>
#include <pthread.h>

#include "l7ag_socket.h"
#include "tag_threadstat.h"
#include "dispatch_message.h"
#include "store_mibdata.h"

class	l7ag_subagent;

class	l7ag_interproccommunicate : public l7ag_socket
{
private:
	l7ag_subagent *	parent;
	THREAD_STAT_TAG	status;
	bool		stop_flag;
	
	pthread_mutex_t	mutex_sbuf;
	pthread_mutex_t	mutex_rbuf;
	char *	buffer_send;
	char *	buffer_recv;

protected:
	l7ag_interproccommunicate();

	static	void *	do_communicate( void * args );
	
public:
	l7ag_interproccommunicate( l7ag_subagent * p_subagent );
	~l7ag_interproccommunicate();
	
	void	set_parent( l7ag_subagent * p_subagent );

	void	start_thread();
	void	stop_thread(){stop_flag=true;}
	
	void	send_mibdatacollect( std::string oid );
	bool	getStopFlag(){return stop_flag;}
	void	setRequestMessage();
};

#endif //__INTERPROCCOM_H__

