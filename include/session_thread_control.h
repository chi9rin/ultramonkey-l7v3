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
	enum	condition_tag{
		INIT	= 0,
		WAIT,
		RUNNING,
		FINISHING,
		EXIT
	};

	thread_ptr		upthread;
	thread_ptr		downthread;
	boost::mutex	condition_mutex;
	session_ptr		session;
	void			upstream_run();
	void			downstream_run();
public:
	friend	class	session;
	friend	class	virtualservice;

	session_thread_control( session_ptr );
	void			startupstream();	// virtualserviceのmapに入れるときにstartを呼ぶ
	void			startdownstream();	// sessionからリクエストがあったときに呼ばれる
	void			stopupstream();		// thread_pool mapに入れるときにstopupstream()を呼ぶ
	void			stopdownstream();	// thread_pool mapに入れるときにstopdownstream()を呼ぶ
	void			join();		// 全てのthreadを停止状態にするときにjoin()を呼ぶ
};

}//	namespace l7vs

#endif	//SESSION_THREAD_CONTROL_H