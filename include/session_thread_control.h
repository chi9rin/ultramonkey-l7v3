//
//	@file	session thread controler.
//	@brief	session used 2 threads. thread pooling unit is 2 thread control.
//
//	copyright (c) sdy corporation. 2009
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	SESSION_THRAED_CONTROL_H
#define	SESSION_THREAD_CONTROL_H
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "session.h"

namespace l7vs{

//
//	session thread pooling utility class.
//
class	session_thread_control : private boost::noncopyable{
public:
	typedef	boost::shared_ptr< boost::thread >	thread_ptr;		//! shared_ptr thread typedef
	typedef	boost::shared_ptr< session >		session_ptr;	//! shared_ptr session typedef
protected:
	enum	state_tag{	//! upthread and down thread state enum
		WAIT	= 0,	//! thread pooling mode
		RUNNING,		//! thread running mode
		EXIT			//! thread exit mode
 	};
	thread_ptr			upthread;					//! upstream thread
	state_tag			upthread_state;				//! upstream thread state
	boost::mutex		upthread_condition_mutex;	//! upthread condition use mutex 
	boost::condition	upthread_condition;			//! upthread condition
	thread_ptr			downthread;					//! downstream thread
	state_tag			downthread_state;			//! downstream thread state
	boost::mutex		downthread_condition_mutex;	//! downstream condition use mutex
	boost::condition	downthread_condition;		//! downstream condition
	session_ptr			session;					//! session class shared pointer
	void				upstream_run();				//! upstream thread bind function
	void				downstream_run();			//! downstream thread bind function
public:
	//! constractor.
	session_thread_control( session_ptr ptr ) :
			session( ptr ),
			upthread_state( WAIT ),
			downthread_state( WAIT ){
		upthread.reset( new boost::thread( boost::bind( &session_thread_control::upstream_run, this ) ) );	//! upstream thread create
		downthread.reset( new boost::thread( boost::bind( &session_thread_control::downstream_run, this ) ) );//! downstream thread create
	}
	//! destractor
	~session_thread_control(){}
	//! session shared ptr getter
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
};

//
//	upstream thread bind function.
//
void	session_thread_control::upstream_run(){
	state_tag	state;
	{	// get first state from class upstream state.
		boost::mutex::scoped_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
	for(;;){	// thread loop
		if( state == WAIT ){	// after create or session end. this thread is pooling mode
			boost::mutex::scoepd_lock	lock( upthread_condition_mutex );
			upstream_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
		}
		else if( state == EXIT ){ // this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session_ptr->up_thread_run();	//session upstream thread looping.
		}
		boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
}
//
// downstream thread bind function,
//
void	session_thread_control::downstream_run(){
	state_tag	state;
	{
		boost::mutex::scoped_lock( downthread_condition_mutex );	//downstream state is lock
		state = downthread_state;	//thread local state is update.
	}
	for(;;){	//thread loop
		if( state == WAIT ){	//after create or session end. this thread is pooling mode
			boost::mutex::scoepd_lock	lock( downthread_condition_mutex );
			downstream_condition.wait( lock ); // thread is condition wait( start at notify_all() )
		}
		else if( state == EXIT ){// this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session_ptr->down_thread_run();//session downstream thread looping.
		}
		boost::mutex::scoped_lock	lock( downthread_condition_mutex );	//downstream state lock
		state = downthread_state;	// thread local sate is update.
	}
}
//
// start upstream function.
//
void	session_thread_control::startupstream(){
	boost::mutex::scoped_lock( downthread_condition_mutex );	//upstream state lock
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
}
//
// stop upstream function
//
void	session_thread_control::stopupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
	if( upthrad_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
}
//
// start downstream function
//
void	session_thread_control::startdownstream(){
	boost::mutex::scoped_lock( downthread_condition_mutex );		// downstream state lock
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
}
//
// stop downstream function.
//
void	session_thread_control::stopdownstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );	// downstream state lock
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
}
//
//	upstream and downstream threads finished function
//
void	session_thread_control::join(){
	boost::mutex::scoped_lock	uplock( upthread_condition_mutex );	//upstream state lock
	upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	upthread_condition.notify_all();	// conditionwait thread is run
	boost::mutex::scoped_lock	downlock( downthread_condition_mutex );//downstream state is lock
	downthread_state = EXIT;	//downstream state update [EXIT] -> thread exit mode
	downthread_condition.notify_all(); //condition wait thread is run.
}

}//	namespace l7vs
#endif	//SESSION_THREAD_CONTROL_H