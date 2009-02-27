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
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::upstream_run()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	sched_setaffinity( 0, sizeof( cpu_set_t ), &vsnic_cpumask );
	state_tag	state;
	{	// get first state from class upstream state.
		boost::mutex::scoped_lock upcond_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
	for(;;){	// thread loop
		if( state == WAIT ){	// after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( upthread_condition_mutex );
// 			upthread_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
			boost::xtime	wait;
			boost::xtime_get( &wait, boost::TIME_UTC );
			wait.sec += 1;
			downthread_condition.timed_wait( lock, wait ); // thread is condition wait( start at notify_all() )
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
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::upstream_run()");
// 		fmt % upthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	downstream thread bind function,
//
void	session_thread_control::downstream_run(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::downstream_run()");
//		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	sched_setaffinity( 0, sizeof( cpu_set_t ), &rsnic_cpumask );
	state_tag	state;
	{
		boost::mutex::scoped_lock downcond_lock( downthread_condition_mutex );	//downstream state is lock
		state = downthread_state;	//thread local state is update.
	}
	for(;;){	//thread loop
		if( state == WAIT ){	//after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( downthread_condition_mutex );
// 			downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
			boost::xtime	wait;
			boost::xtime_get( &wait, boost::TIME_UTC );
			wait.sec += 1;
			downthread_condition.timed_wait( lock, wait ); // thread is condition wait( start at notify_all() )
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
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::downstream_run()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start upstream function.
//
void	session_thread_control::startupstream(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::startupstream()");
//		fmt % upthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	//upstream state lock
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::startupstream() :status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop upstream function
//
void	session_thread_control::stopupstream(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::stopupstream()");
// 		fmt % upthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
	if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::stopupstream() : status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start downstream function
//
void	session_thread_control::startdownstream(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::startdownstream()");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );		// downstream state lock
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::startdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop downstream function.
//
void	session_thread_control::stopdownstream(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::stopdownstream()");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );	// downstream state lock
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::stopdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//!	@brief	upstream and downstream threads finished function
//
void	session_thread_control::join(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("in_function : void session_thread_control::join() :");
// 		fmt % downthread->native_handle();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	boost::mutex::scoped_lock	uplock( upthread_condition_mutex );	//upstream state lock
	upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	upthread_condition.notify_all();	// conditionwait thread is run
	boost::mutex::scoped_lock	downlock( downthread_condition_mutex );//downstream state is lock
	downthread_state = EXIT;	//downstream state update [EXIT] -> thread exit mode
	downthread_condition.notify_all(); //condition wait thread is run.
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format fmt("out_function : void session_thread_control::stopdownstream() : up_status = %d / down_status = %d");
		fmt % upthread_state % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}

}	//namespace l7vs
