#ifndef	L7VSD_THREAD_TEST_H
#define L7VSD_THREAD_TEST_H

#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/condition.hpp>

#include "tcp_session_base.h"
#include "tcp_session_socket_model.h"
#include "tcp_session_stream_model.h"
#include "atomic.h"
#include "lockfree_queue.h"
#include "lockfree_hashmap.h"

namespace l7vs{
boost::asio::io_service		dispatcher;			//!< dispatcher

class	session_thread_control : private boost::noncopyable{
public:
	typedef	boost::shared_ptr< boost::thread >	thread_ptr;		//! shared_ptr thread typedef
	typedef	boost::shared_ptr< tcp_session_base >	session_ptr;	//! shared_ptr session typedef
	typedef	boost::thread::id					thread_id_type;	//! thread id typedef
	enum	state_tag{	//! @enum state_tag upthread and down thread state enum
		WAIT	= 0,	//! thread pooling mode
		RUNNING,		//! thread running mode
		EXIT			//! thread exit mode
 	};
protected:
	thread_ptr			upthread;					//! upstream thread
	wr_mutex			upthread_state_mutex;		//! mutex for upstream thread status
	state_tag			upthread_state;				//! upstream thread state
	boost::mutex		upthread_condition_mutex;	//! upthread condition use mutex
	boost::condition	upthread_condition;			//! upthread condition
	boost::mutex		upthread_running_mutex;
	boost::mutex		upthread_joining_mutex;
	boost::condition	upthread_joining_condition;
	thread_ptr			downthread;					//! downstream thread
	wr_mutex			downthread_state_mutex;		//! mutex for downstream thread status
	state_tag			downthread_state;			//! downstream thread state
	boost::mutex		downthread_condition_mutex;	//! downstream condition use mutex
	boost::condition	downthread_condition;		//! downstream condition
	boost::mutex		downthread_running_mutex;
	boost::mutex		downthread_joining_mutex;
	boost::condition	downthread_joining_condition;
	session_ptr			session;					//! session class shared pointer
	void				upstream_run(){
		state_tag	state;
		upthread_running_mutex.lock();
		{	// get first state from class upstream state.
			rw_scoped_lock	upstate_lock( upthread_state_mutex );
			state = upthread_state;	//thread local state is update.
		}
		for(;;){	// thread loop
			if( state == WAIT ){	// after create or session end. this thread is pooling mode
				boost::mutex::scoped_lock	lock( upthread_condition_mutex );
	 			upthread_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
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
//				session->up_thread_run();	//session upstream thread looping.
				session->Run_main();
				session->Run_sub();

				stopupstream();
			}
			rw_scoped_lock	upstate_lock( upthread_state_mutex );
			state = upthread_state;	//thread local state is update.
		}
		upthread_running_mutex.unlock();
		boost::mutex::scoped_lock up_lk( upthread_joining_mutex );
		upthread_joining_condition.notify_all();
	}
	void				downstream_run(){
		state_tag	state;
		downthread_running_mutex.lock();
		{
			rw_scoped_lock	downstate_lock( downthread_state_mutex );
			state = downthread_state;	//thread local state is update.
		}
		for(;;){	//thread loop
			if( state == WAIT ){	//after create or session end. this thread is pooling mode
				boost::mutex::scoped_lock	lock( downthread_condition_mutex );
	 			downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
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
//				session->down_thread_run();//session downstream thread looping.
				session->Run_main();
				session->Run_sub();
				stopdownstream();
			}
			rw_scoped_lock	downstate_lock( downthread_state_mutex );
			state = downthread_state;	// thread local sate is update.
		}
		downthread_running_mutex.unlock();
		boost::mutex::scoped_lock down_lk( downthread_joining_mutex );
		downthread_joining_condition.notify_all();
	}

public:
	//! constractor.
	session_thread_control( tcp_session_base* ptr) :upthread_state( WAIT ),downthread_state( WAIT ){
		int			int_val;

		session.reset( ptr );
		upthread.reset( new boost::thread( &session_thread_control::upstream_run, this ) );	
		downthread.reset( new boost::thread( &session_thread_control::downstream_run, this ) );
	}
	~session_thread_control(){
	}
	session_ptr		get_session(){	return session; }
	void			startupstream(){
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
		upthread_condition.notify_all();							// conditionwait upstreamthread is run.
	}
	void			startdownstream(){
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
		downthread_condition.notify_all();								// condition wait thread is run.
	}
	void			stopupstream(){
		rw_scoped_lock	upstate_lock( upthread_state_mutex );
		if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
	}
	void			stopdownstream(){
		rw_scoped_lock	downstate_lock( downthread_state_mutex );
		if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
	}


	void			join(){
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
	}


	thread_id_type	get_upthread_id(){ return upthread->get_id(); }
	thread_id_type	get_downthread_id(){ return downthread->get_id(); }
	boost::mutex&	get_upthread_mutex(){ return	upthread_running_mutex; }
	boost::mutex&	get_downthread_mutex(){ return	downthread_running_mutex; }

	void			session_stop(){}
//	void			session_stop(){ session->set_virtual_service_message( tcp_session::SESSION_END ); }
};







class	virtualservice_tcp{
public:
	typedef	lockfree_queue< session_thread_control >
								session_queue_type;
	typedef	lockfree_hashmap< tcp_session_base, session_thread_control >
								session_map_type;
	typedef	boost::shared_ptr< tcp_session_base >	session_ptr;	//! shared_ptr session typedef
protected:
	boost::asio::ip::tcp::acceptor acceptor_;
	session_queue_type			pool_sessions;
	session_map_type			waiting_sessions;
	session_map_type			active_sessions;


	void	handle_accept(const l7vs::session_thread_control* stc_ptr,const boost::system::error_code& err ){
		session_thread_control*		stc_ptr_noconst = const_cast<session_thread_control*>( stc_ptr );
		tcp_session_base*	tmp_session	= stc_ptr_noconst->get_session().get();

		active_sessions.insert( tmp_session, stc_ptr_noconst );
		waiting_sessions.erase( tmp_session );

		stc_ptr_noconst->startupstream();
		stc_ptr_noconst->startdownstream();

		//waiting, pool_sessions.size become over 1
		//pick up session from pool
		session_thread_control*		stc_ptr_register_accept;
		do{
			stc_ptr_register_accept = pool_sessions.pop();
		}
		while( unlikely( !stc_ptr_register_accept ) );
	
		waiting_sessions.insert( tmp_session, stc_ptr_register_accept );

		//regist accept event handler
		acceptor_.async_accept( stc_ptr_register_accept->get_session()->get_cl_socket(),
					boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr_register_accept, boost::asio::placeholders::error ) );
	
	}

public:
	int flg;
	boost::asio::ip::tcp::endpoint tcp_accept_endpoint;
	boost::asio::ip::tcp::endpoint tcp_rs_endpoint;

	virtualservice_tcp(int _flg,std::string ip_address,int port,std::string rs_ip_address,int rs_port)
						:flg(_flg),acceptor_( dispatcher ),
						tcp_accept_endpoint(boost::asio::ip::address::from_string(ip_address), port),
						tcp_rs_endpoint(boost::asio::ip::address::from_string(rs_ip_address), rs_port) {}

	~virtualservice_tcp(){}



	void	initialize(){

		boost::asio::ip::address	address	= tcp_accept_endpoint.address();

		//bind acceptor
		boost::system::error_code	acceptor_err;
		acceptor_.open( tcp_accept_endpoint.protocol(), acceptor_err );
		acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ), acceptor_err );
		acceptor_.bind( tcp_accept_endpoint, acceptor_err );
		//create session pool
		{
			for( int i = 0; i < 100; ++i ){
//				tcp_session*	sess	= new tcp_session( *this, dispatcher);
				if (flg == 1){
					tcp_session_socket_model*	sess	= new tcp_session_socket_model(this,dispatcher,tcp_rs_endpoint);
				}else{
					tcp_session_stream_model*	sess	= new  tcp_session_stream_model(this,dispatcher,tcp_rs_endpoint);
				}

//				session_result_message	result	= sess->initialize();
				session_thread_control*	p_stc = new session_thread_control( sess );
				while( !pool_sessions.push( p_stc ) ){}
			}
		}
	}


	void	finalize(){
		//stop main loop
		stop();
		while(active_sessions.size()){
			boost::this_thread::yield();
		}
		for(;;){
			tcp_session_base*				tmp_session	= NULL;
			session_thread_control*		tmp_stc		= NULL;
			waiting_sessions.pop( tmp_session, tmp_stc );
			if( !tmp_stc ){
				break;
			}
			for(;;){
				if( likely( pool_sessions.push( tmp_stc ) ) )break;
			}
		}
		//release sessions[i]->join();
		while( !pool_sessions.empty() ){
			session_thread_control*	stc = pool_sessions.pop();
			stc->join();
			{
				boost::mutex::scoped_lock upthread_wait( stc->get_upthread_mutex() );
				boost::mutex::scoped_lock downthread_wait( stc->get_downthread_mutex() );
			}
			delete	stc;
			stc = NULL;
		}
		active_sessions.clear();
//		release_virtual_service( element );
	}
//	void						set_virtualservice( const virtualservice_element&, error_code& );

	void	run(){
		boost::asio::socket_base::receive_buffer_size option(8192 * 192);
		acceptor_.set_option(option);
		//start listen
		acceptor_.listen();
		//switch active a session
		session_thread_control*	stc_ptr;
		{
			stc_ptr = pool_sessions.pop();
			waiting_sessions.insert( stc_ptr->get_session().get(), stc_ptr );
		}
		acceptor_.async_accept( stc_ptr->get_session()->get_cl_socket(),
						boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
		dispatcher.run();

		//stop all active sessions
		{
			active_sessions.do_all( boost::bind( &session_thread_control::session_stop, _1 ) );
		}
	}

	void	stop(){
		boost::system::error_code	err;
		acceptor_.close( err );
		dispatcher.stop();
	}

/*
	void	connection_active( const boost::asio::ip::tcp::endpoint& in ){
		for( std::list<realserver>::iterator itr = rs_list.begin();itr != rs_list.end();++itr ){
			if( itr->tcp_endpoint == in ){
				itr->decrement_active();
				itr->increment_inact();
				break;
			}
		}
	}

	void	connection_inactive(boost::asio::ip::tcp::endpoint& in ){
		session_thread_control*	stc_ptr = active_sessions.find( session_ptr );
	
		active_sessions.erase( session_ptr );
		stc_ptr->get_session()->initialize();
		for(;;){
			if( likely( pool_sessions.push( stc_ptr ) ) ){
				break;
			}
		}
	}
*/
	void	release_session(tcp_session_base* session_ptr ){
		session_thread_control*		stc_ptr = active_sessions.find( session_ptr );
		active_sessions.erase( session_ptr );
//		stc_ptr->get_session()->initialize();
		for(;;){
			if( likely( pool_sessions.push( stc_ptr ) ) ){
				break;
			}
		}
	}

};




class virtual_service;

class l7vsd : private boost::noncopyable{
public:
	typedef	boost::shared_ptr< virtual_service >	virtualservice_ptr;		//!< shared_ptr	virtualservice typedef

	l7vsd():exit_requested(0),
			received_sig(0){}
	~l7vsd(){}

protected:
//	boost::thread_group			vs_threads;			//!< virtual_service thread group

//	mutable	vslist_type			vslist;				//!< virtual_service list

	boost::mutex				command_mutex;		//!< command execute mutex
	boost::mutex				vslist_mutex;		//!< virtual service list mutex

	bool						help;				//!< help mode
	bool						debug;				//!< debug mode

	boost::posix_time::ptime	starttime;			//!< l7vsd startup time

	volatile	sig_atomic_t	exit_requested;		//!< signal exit flag
	volatile	sig_atomic_t	received_sig;		//!< received signal



	//! option parse function object type.
	typedef	boost::function< bool ( int&, int, char*[] ) >
			parse_opt_func_type;
	//! option string - parse function object map type
	typedef	std::map< std::string, parse_opt_func_type >
			parse_opt_map_type;
	//! list option function map dictionary.
	parse_opt_map_type	option_dic;

public:

	void	sig_exit_handler(){
		sigset_t	sigmask;
		sigemptyset( &sigmask );
		sigaddset( &sigmask, SIGHUP );
		sigaddset( &sigmask, SIGINT );
		sigaddset( &sigmask, SIGQUIT );
		sigaddset( &sigmask, SIGTERM );
		
		int	sig;
		sigwait( &sigmask, &sig );
	
		received_sig = sig;
		exit_requested = 1;
	}

	int		run( int flg ) {

		int rett = daemon(0,0);

//		struct rlimit lim;
//		lim.rlim_cur = 65535;
//		lim.rlim_max = 65535;
//		int ret;
//		ret = setrlimit( RLIMIT_NOFILE, &lim );
		// signal handler thread start
		boost::thread	sigthread( boost::bind( &l7vsd::sig_exit_handler, this ) );
		sigthread.detach();



		virtualservice_tcp vs(flg,"127.0.0.1",7000,"127.0.0.1",7000);
		vs.initialize();
		vs.run();
		// main loop
		dispatcher.poll();

		for(;;){
			if( unlikely( exit_requested ) ){
				break;
			}
			timespec	wait_val;
			wait_val.tv_sec		= 0;
			wait_val.tv_nsec	= 10;
			nanosleep( &wait_val, NULL );
			boost::this_thread::yield();
		}
		return 0;

	}

};

}	//namespace l7vs
#endif
