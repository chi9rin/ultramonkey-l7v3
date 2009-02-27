//
//!	@file	session thread controler.
//!	@brief	session used 2 threads. thread pooling unit is 2 thread control.
//
//	copyright (c) sdy corporation. 2009
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "session_thread_control.h"

namespace l7vs{

//
//!	@brief upstream thread bind function.
//
void	session_thread_control::upstream_run(){
	sched_setaffinity( 0, sizeof( cpu_set_t ), &vsnic_cpumask );
	state_tag	state;
	{	// get first state from class upstream state.
		boost::mutex::scoped_lock upcond_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
	for(;;){	// thread loop
		if( state == WAIT ){	// after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( upthread_condition_mutex );
			upthread_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
		}
		else if( state == EXIT ){ // this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->up_thread_run();	//session upstream thread looping.
			stopupstream();
		}
		boost::mutex::scoped_lock	upcond_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
}
//
//! @brief	downstream thread bind function,
//
void	session_thread_control::downstream_run(){
	sched_setaffinity( 0, sizeof( cpu_set_t ), &rsnic_cpumask );
	state_tag	state;
	{
		boost::mutex::scoped_lock downcond_lock( downthread_condition_mutex );	//downstream state is lock
		state = downthread_state;	//thread local state is update.
	}
	for(;;){	//thread loop
		if( state == WAIT ){	//after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( downthread_condition_mutex );
			downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
		}
		else if( state == EXIT ){// this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->down_thread_run();//session downstream thread looping.
			stopdownstream();
		}
		boost::mutex::scoped_lock	downcond_lock( downthread_condition_mutex );	//downstream state lock
		state = downthread_state;	// thread local sate is update.
	}
}
//
//! @brief	start upstream function.
//
void	session_thread_control::startupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	//upstream state lock
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
}
//
//! @brief	stop upstream function
//
void	session_thread_control::stopupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
	if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
}
//
//! @brief	start downstream function
//
void	session_thread_control::startdownstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );		// downstream state lock
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
}
//
//! @brief	stop downstream function.
//
void	session_thread_control::stopdownstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );	// downstream state lock
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
}
//
//!	@brief	upstream and downstream threads finished function
//
void	session_thread_control::join(){
	boost::mutex::scoped_lock	uplock( upthread_condition_mutex );	//upstream state lock
	upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	upthread_condition.notify_all();	// conditionwait thread is run
	boost::mutex::scoped_lock	downlock( downthread_condition_mutex );//downstream state is lock
	downthread_state = EXIT;	//downstream state update [EXIT] -> thread exit mode
	downthread_condition.notify_all(); //condition wait thread is run.
}

}	//namespace l7vs
