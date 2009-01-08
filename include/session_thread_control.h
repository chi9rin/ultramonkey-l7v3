#ifndef	SESSION_THRAED_CONTROL_H
#define	SESSION_THREAD_CONTROL_H
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "session.h"

namespace l7vs{

class	session_thread_control : private boost::noncopyable{
public:
	typedef	boost::shared_ptr< boost::thread >	thread_ptr;
	typedef	boost::shared_ptr< session >		session_ptr;
protected:
	enum	state_tag{
		WAIT	= 0,
		RUNNING,
		EXIT
 	};
	thread_ptr			upthread;
	thread_ptr			downthread;
	state_tag			upthread_state;
	boost::mutex		upthread_condition_mutex;
	boost::condition	upthread_condition;
	state_tag			downthread_state;
	boost::mutex		downthread_condition_mutex;
	boost::condition	downthread_condition;
	session_ptr			session;
	void				upstream_run();
	void				downstream_run();
public:
	session_thread_control( session_ptr ptr ) : session( ptr ), upthread_state( WAIT ), downthread_state( WAIT ){
		upthread.reset( new boost::thread( boost::bind( &session_thread_control::upstream_run, this ) ) );
		downthread.reset( new boost::thread( boost::bind( &session_thread_control::downstream_run, this ) ) );
	}
	~session_thread_control(){}
	session_ptr		get_session(){	return session; }
	void			startupstream();	// virtualserviceのmapに入れるときにstartを呼ぶ
	void			startdownstream();	// sessionからリクエストがあったときに呼ばれる
	void			stopupstream();		// thread_pool mapに入れるときにstopupstream()を呼ぶ
	void			stopdownstream();	// thread_pool mapに入れるときにstopdownstream()を呼ぶ
	void			join();		// 全てのthreadを停止状態にするときにjoin()を呼ぶ
};

void	session_thread_control::upstream_run(){
	state_tag	state;
	{
		boost::mutex::scoped_lock( upthread_condition_mutex );
		state = upthread_state;
	}
	for(;;){
		if( state == WAIT ){
			boost::mutex::scoepd_lock	lock( upthread_condition_mutex );
			upstream_condition.wait( lock );
			boost::mutex::scoped_lock( upthread_condition_mutex ){
			}
		}
		else if( state == EXIT ){
			break;
		}
		else{	//state RUNNING
			session_ptr->up_thread_run();
		}
		{
			boost::mutex::scoped_lock	lock( upthread_condition_mutex );
			state = upthread_state;
		}
	}
}
void	session_thread_control::downstream_run(){
	state_tag	state;
	{
		boost::mutex::scoped_lock( downthread_condition_mutex );
		state = downthread_state;
	}
	for(;;){
		if( state == WAIT ){
			boost::mutex::scoepd_lock	lock( downthread_condition_mutex );
			downstream_condition.wait( lock );
			boost::mutex::scoped_lock( downthread_condition_mutex ){
			}
		}
		else if( state == EXIT ){
			break;
		}
		else{	//state RUNNING
			session_ptr->down_thread_run();
		}
		{
			boost::mutex::scoped_lock	lock( downthread_condition_mutex );
			state = downthread_state;
		}
	}
}

void	session_thread_control::startupstream(){
	boost::mutex::scoped_lock( downthread_condition_mutex );
	upthread_state = RUNNING;
	upthread_condition.notify_all();
}

void	session_thread_control::stopupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );
	upthread_state = WAIT;
}
void	session_thread_control::startdownstream(){
	boost::mutex::scoped_lock( downthread_condition_mutex );
	downthread_state = RUNNING;
	downthread_condition.notify_all();
}

void	session_thread_control::stopupstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );
	downthread_state = WAIT;
}
void	session_thread_control::join(){
	boost::mutex::scoped_lock	uplock( upthread_condition_mutex );
	upthread_state = EXIT;
	upthread_condition.notify_all();
	boost::mutex::scoped_lock	downlock( downthread_condition_mutex );
	downthread_state = EXIT;
	downthread_condition.notify_all();
}


}//	namespace l7vs

#endif	//SESSION_THREAD_CONTROL_H