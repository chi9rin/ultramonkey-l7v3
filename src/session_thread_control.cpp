/*!
 *	@file	session_thread_control.cpp
 *	@brief	session used 2 threads. thread pooling unit is 2 thread control.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include <sched.h>
#include "session_thread_control.h"

#include "utility.h"

namespace l7vs{

//
//!	@brief upstream thread bind function.
//
void	session_thread_control::upstream_run(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::upstream_run()");
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 1, fmt.str(), __FILE__, __LINE__ );
	}

	cpu_set_t       mask;
	//numが1以上なら、その数だけCPU_SETする
	if( 0 < num_of_core_uses ){
		CPU_ZERO( &mask );
		for( int i = 0; i < num_of_core_uses; ++i ){
			CPU_SET( static_cast<int>( i ), &mask );
		}
		sched_setaffinity( 0, sizeof( cpu_set_t ), &mask );
	}
#ifdef	SCHED_SETAFFINITY
	sched_setaffinity( 0, sizeof( cpu_set_t ), &vsnic_cpumask );
#endif

	state_tag	state;
	upthread_running_mutex.lock();
	{	// get first state from class upstream state.
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
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		state = upthread_state;	//thread local state is update.
	}
	upthread_running_mutex.unlock();
	boost::mutex::scoped_lock up_lk( upthread_joining_mutex );
	upthread_joining_condition.notify_all();
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::upstream_run()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 3, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	downstream thread bind function,
//
void	session_thread_control::downstream_run(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::downstream_run()");
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 3, fmt.str(), __FILE__, __LINE__ );
	}
	cpu_set_t       mask;
	//numが1以上なら、その数だけCPU_SETする
	if( 0 < num_of_core_uses ){
		CPU_ZERO( &mask );
		for( int i = 1; i < num_of_core_uses; ++i ){
			CPU_SET( static_cast<int>( i ), &mask );
		}
		sched_setaffinity( 0, sizeof( cpu_set_t ), &mask );
	}
#ifdef	SCHED_SETAFFINITY
	sched_setaffinity( 0, sizeof( cpu_set_t ), &rsnic_cpumask );
#endif

	state_tag	state;
	downthread_running_mutex.lock();
	{
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
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		state = downthread_state;	// thread local sate is update.
	}
	downthread_running_mutex.unlock();
	boost::mutex::scoped_lock down_lk( downthread_joining_mutex );
	downthread_joining_condition.notify_all();
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::downstream_run()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 5, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start upstream function.
//
void	session_thread_control::startupstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::startupstream()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 6, fmt.str(), __FILE__, __LINE__ );
	}
	rw_scoped_lock	upstate_lock( upthread_state_mutex );
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::startupstream() :status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 7, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop upstream function
//
void	session_thread_control::stopupstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::stopupstream()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 8, fmt.str(), __FILE__, __LINE__ );
	}
	rw_scoped_lock	upstate_lock( upthread_state_mutex );
	if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::stopupstream() : status = %d");
 		fmt % upthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 9, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	start downstream function
//
void	session_thread_control::startdownstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::startdownstream()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 10, fmt.str(), __FILE__, __LINE__ );
	}
	rw_scoped_lock	downstate_lock( downthread_state_mutex );
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::startdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 11, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//! @brief	stop downstream function.
//
void	session_thread_control::stopdownstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::stopdownstream()");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 12, fmt.str(), __FILE__, __LINE__ );
	}
	rw_scoped_lock	downstate_lock( downthread_state_mutex );
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("out_function : void session_thread_control::stopdownstream() : status = %d");
		fmt % downthread_state;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 13, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//!	@brief	upstream and downstream threads finished function
//
void	session_thread_control::join(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format fmt("in_function : void session_thread_control::join() :");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 14, fmt.str(), __FILE__, __LINE__ );
	}

	boost::mutex::scoped_lock	up_lk( upthread_joining_mutex );
	boost::mutex::scoped_lock	down_lk( downthread_joining_mutex );

	{
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	}
	{
		boost::mutex::scoped_lock upthread_running_wait( upthread_running_mutex );
		upthread_condition.notify_all();	// conditionwait thread is run
	}

	{
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
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 15, fmt.str(), __FILE__, __LINE__ );
	}
}
//
//!	@brief	get upthread mutex object reference
//
boost::mutex&	session_thread_control::get_upthread_mutex(){
	return	upthread_running_mutex;
}
//
//!	@brief	get downthread mutex object reference
//
boost::mutex&	session_thread_control::get_downthread_mutex(){
	return	downthread_running_mutex;
}

}	//namespace l7vs
