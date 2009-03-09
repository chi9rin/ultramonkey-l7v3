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
#ifndef	SESSION_THREAD_CONTROL_H
#define	SESSION_THREAD_CONTROL_H
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "tcp_session.h"

namespace l7vs{

class	tcp_session;

//!
//!	@brief	session thread pooling utility class.
//! @class	session_thread_control is session thread pool utirity.
class	session_thread_control : private boost::noncopyable{
public:
	typedef	boost::shared_ptr< boost::thread >	thread_ptr;		//! shared_ptr thread typedef
	typedef	boost::shared_ptr< tcp_session >	session_ptr;	//! shared_ptr session typedef
	typedef	boost::thread::id					thread_id_type;	//! thread id typedef
	enum	state_tag{	//! @enum state_tag upthread and down thread state enum
		WAIT	= 0,	//! thread pooling mode
		RUNNING,		//! thread running mode
		EXIT			//! thread exit mode
 	};
protected:
	thread_ptr			upthread;					//! upstream thread
	state_tag			upthread_state;				//! upstream thread state
	boost::mutex		upthread_condition_mutex;	//! upthread condition use mutex
	boost::condition	upthread_condition;			//! upthread condition
	boost::mutex		upthread_running_mutex;
	boost::mutex		upthread_joining_mutex;
	boost::condition	upthread_joining_condition;
	thread_ptr			downthread;					//! downstream thread
	state_tag			downthread_state;			//! downstream thread state
	boost::mutex		downthread_condition_mutex;	//! downstream condition use mutex
	boost::condition	downthread_condition;		//! downstream condition
	boost::mutex		downthread_running_mutex;
	boost::mutex		downthread_joining_mutex;
	boost::condition	downthread_joining_condition;
	session_ptr			session;					//! session class shared pointer
	void				upstream_run();				//! upstream thread bind function
	void				downstream_run();			//! downstream thread bind function

	cpu_set_t			vsnic_cpumask;
	cpu_set_t			rsnic_cpumask;
public:
	//! constractor.
	//! @param session_ptr	session class shared ptr
	session_thread_control( tcp_session* ptr, cpu_set_t in_upcpu, cpu_set_t in_downcpu ) :
			upthread_state( WAIT ),
			downthread_state( WAIT ),
			vsnic_cpumask( in_upcpu ),
			rsnic_cpumask( in_downcpu ){
		session.reset( ptr );
		upthread.reset( new boost::thread( &session_thread_control::upstream_run, this ) );	//! upstream thread create
		downthread.reset( new boost::thread( &session_thread_control::downstream_run, this ) );//! downstream thread create
	}
	//! destractor
	~session_thread_control(){
	}
	//! session shared ptr getter
	//! @return session shared ptr
	session_ptr		get_session(){	return session; }
	//! upstream thread start function
	void			startupstream();
	//! downstream thread start function
	void			startdownstream();
	//! upstream thread stop function
	void			stopupstream();
	//! downstream thread stop funtion
	void			stopdownstream();
	//! all thread destory function.
	void			join();
	//! upstream-thread id getter
	//! @return thread_id_type	upstream thread id
	thread_id_type	get_upthread_id(){ return upthread->get_id(); }
	//! downstream-thread id getter
	//! @return thread_id_type	downstream thread id
	thread_id_type	get_downthread_id(){ return downthread->get_id(); }

	boost::mutex&	get_upthread_mutex();
	boost::mutex&	get_downthread_mutex();
};

}//	namespace l7vs
#endif	//SESSION_THREAD_CONTROL_H
