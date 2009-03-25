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

#include <sched.h>
#include "session_thread_control.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace l7vs{

//
//!	@brief upstream thread bind function.
//
void	session_thread_control::upstream_run(){
	l7vs::Logger	funcLog( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in_function : void session_thread_control::upstream_run()", __FILE__, __LINE__ );

#ifdef	SCHED_SETAFFINITY
	sched_setaffinity( 0, sizeof( cpu_set_t ), &vsnic_cpumask );
#endif

	int				ret_val, sched_policy;
	sched_param		scheduler_param;
	ret_val			= sched_getparam( 0, &scheduler_param );
	sched_policy	= sched_getscheduler( 0 );
	if( SCHED_FIFO == sched_algorithm ){
		scheduler_param.__sched_priority	= 20;
		sched_policy	= SCHED_FIFO;
	}else if( SCHED_RR == sched_algorithm ){
		scheduler_param.__sched_priority	= 20;
		sched_policy	= SCHED_RR;
	}else if( SCHED_BATCH == sched_algorithm ){
		sched_policy	= SCHED_BATCH;
	}
	ret_val			= sched_setscheduler( 0, sched_policy, &scheduler_param );
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "upstream_run : parameter of task scheduler algorithm : priority = %d / algorithm : %d" );
		fmt % scheduler_param.__sched_priority % sched_policy;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	if( 0 > ret_val ){
		boost::format	errlog_fmt( "error at upstream_run, set task scheduler algorithm. : %d" );
		errlog_fmt % errno;
		Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, errlog_fmt.str(), __FILE__, __LINE__ );
	}

	state_tag	state;
	upthread_running_mutex.lock();
	{	// get first state from class upstream state.
// 		boost::mutex::scoped_lock upcond_lock( upthread_condition_mutex );	// upstream state lock
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		state = upthread_state;	//thread local state is update.
	}
	for(;;){	// thread loop
		if( state == WAIT ){	// after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( upthread_condition_mutex );
// 			upthread_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
			boost::xtime	wait;
			boost::xtime_get( &wait, boost::TIME_UTC );
			wait.sec += 1;
			upthread_running_mutex.unlock();
			upthread_condition.timed_wait( lock, wait ); // thread is condition wait( start at notify_all() )
			upthread_running_mutex.lock();
		}
		else if( state == EXIT ){ // this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->up_thread_run();	//session upstream thread looping.
			stopupstream();
		}
// 		boost::mutex::scoped_lock	upcond_lock( upthread_condition_mutex );	// upstream state lock
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		state = upthread_state;	//thread local state is update.
	}
	upthread_running_mutex.unlock();
	boost::mutex::scoped_lock up_lk( upthread_joining_mutex );
	upthread_joining_condition.notify_all();
}
//
//! @brief	downstream thread bind function,
//
void	session_thread_control::downstream_run(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::downstream_run()");
//		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
#ifdef	SCHED_SETAFFINITY
	sched_setaffinity( 0, sizeof( cpu_set_t ), &rsnic_cpumask );
#endif

	int				ret_val, sched_policy;
	sched_param		scheduler_param;
	ret_val			= sched_getparam( 0, &scheduler_param );
	sched_policy	= sched_getscheduler( 0 );
	if( SCHED_FIFO == sched_algorithm ){
		scheduler_param.__sched_priority	= 20;
		sched_policy	= SCHED_FIFO;
	}else if( SCHED_RR == sched_algorithm ){
		scheduler_param.__sched_priority	= 20;
		sched_policy	= SCHED_RR;
	}else if( SCHED_BATCH == sched_algorithm ){
		sched_policy	= SCHED_BATCH;
	}
	ret_val			= sched_setscheduler( 0, sched_policy, &scheduler_param );
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "upstream_run : parameter of task scheduler algorithm : priority = %d / algorithm : %d" );
		fmt % scheduler_param.__sched_priority % sched_policy;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	if( 0 > ret_val ){
		boost::format	errlog_fmt( "error at upstream_run, set task scheduler algorithm. : %d" );
		errlog_fmt % errno;
		Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, errlog_fmt.str(), __FILE__, __LINE__ );
	}

	state_tag	state;
	downthread_running_mutex.lock();
	{
// 		boost::mutex::scoped_lock downcond_lock( downthread_condition_mutex );	//downstream state is lock
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		state = downthread_state;	//thread local state is update.
	}
	for(;;){	//thread loop
		if( state == WAIT ){	//after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( downthread_condition_mutex );
// 			downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
			boost::xtime	wait;
			boost::xtime_get( &wait, boost::TIME_UTC );
			wait.sec += 1;
			downthread_running_mutex.unlock();
			downthread_condition.timed_wait( lock, wait ); // thread is condition wait( start at notify_all() )
			downthread_running_mutex.lock();
		}
		else if( state == EXIT ){// this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->down_thread_run();//session downstream thread looping.
			stopdownstream();
		}
// 		boost::mutex::scoped_lock	downcond_lock( downthread_condition_mutex );	//downstream state lock
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		state = downthread_state;	// thread local sate is update.
	}
	downthread_running_mutex.unlock();
	boost::mutex::scoped_lock down_lk( downthread_joining_mutex );
	downthread_joining_condition.notify_all();
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::downstream_run()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start upstream function.
//
void	session_thread_control::startupstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::startupstream()");
//		fmt % upthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
// 	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	//upstream state lock
	rw_scoped_lock	upstate_lock( upthread_state_mutex );
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::startupstream() :status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop upstream function
//
void	session_thread_control::stopupstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::stopupstream()");
// 		fmt % upthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
// 	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
	rw_scoped_lock	upstate_lock( upthread_state_mutex );
	if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::stopupstream() : status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start downstream function
//
void	session_thread_control::startdownstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::startdownstream()");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
// 	boost::mutex::scoped_lock	lock( downthread_condition_mutex );		// downstream state lock
	rw_scoped_lock	downstate_lock( downthread_state_mutex );
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::startdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop downstream function.
//
void	session_thread_control::stopdownstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::stopdownstream()");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
// 	boost::mutex::scoped_lock	lock( downthread_condition_mutex );	// downstream state lock
	rw_scoped_lock	downstate_lock( downthread_state_mutex );
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::stopdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//!	@brief	upstream and downstream threads finished function
//
void	session_thread_control::join(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::join() :");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	boost::mutex::scoped_lock	up_lk( upthread_joining_mutex );
	boost::mutex::scoped_lock	down_lk( downthread_joining_mutex );

	{
// 		boost::mutex::scoped_lock	uplock( upthread_condition_mutex );	//upstream state lock
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	}
	{
		boost::mutex::scoped_lock upthread_running_wait( upthread_running_mutex );
		upthread_condition.notify_all();	// conditionwait thread is run
	}

	{
// 		boost::mutex::scoped_lock	downlock( downthread_condition_mutex );//downstream state is lock
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		downthread_state = EXIT;	//downstream state update [EXIT] -> thread exit mode
	}
	{
		boost::mutex::scoped_lock downthread_running_wait( downthread_running_mutex );
		downthread_condition.notify_all(); //condition wait thread is run.
	}

	upthread_joining_condition.wait( up_lk );
	downthread_joining_condition.wait( down_lk );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::stopdownstream() : up_status = %d / down_status = %d");
		fmt % upthread_state % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}

boost::mutex&	session_thread_control::get_upthread_mutex(){
	return	upthread_running_mutex;
}

boost::mutex&	session_thread_control::get_downthread_mutex(){
	return	downthread_running_mutex;
}

}	//namespace l7vs
