//
//!	@file	virtualservice.cpp
//!	@brief	VirtualService class implementations
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include <vector>

#include "virtualservice.h"
#include "logger_enum.h"
#include "logger.h"
#include "parameter.h"

#define	BPS_DEFAULT_INTERVAL_USEC	500000ULL

// imprementation for virtualservice_base
/*!
 * virtualservice_base class constructor.
 */
l7vs::virtualservice_base::virtualservice_base(	const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const l7vs::virtualservice_element& inelement)
												 :	vsd( invsd ),
													rep( inrep ),
													rs_list_ref_count( 0 ),
													recvsize_up( 0 ),
													current_up_recvsize( 0 ),
													sendsize_up( 0 ),
													recvsize_down( 0 ),
													current_down_recvsize( 0 ),
													sendsize_down( 0 ),
													throughput_up( 0 ),
													throughput_down( 0 ) {
	calc_bps_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	replication_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	rs_list.clear();
	rs_mutex_list.clear();
	element = inelement;
	protomod = NULL;
	schedmod = NULL;
};

/*!
 * load_parameter method call.(get parameter file's value)
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::load_parameter(){
	l7vs::error_code	vs_err;
	Parameter			param;
	int	int_val;
	//get session pool size value
	int_val	= param.get_int( l7vs::PARAM_COMP_VIRTUALSERVICE, PARAM_POOLSIZE_KEY_NAME, vs_err );
	if( vs_err )
		param_data.session_pool_size = SESSION_POOL_NUM_DEFAULT;
	else
		param_data.session_pool_size = int_val;
}

/*!
 * handle_protomod_replication method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_protomod_replication( const boost::system::error_code& ){
	if( NULL != protomod )
		protomod->replication_interrupt();
	else{
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, PROTOMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
	}
}

/*!
 * handle_schedmod_replication method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_schedmod_replication( const boost::system::error_code& ){
	if( NULL != schedmod )
		schedmod->replication_interrupt();
	else{
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, SCHEDMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
	}
}

/*!
 * handle_throughput_update method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_throughput_update( const boost::system::error_code& ){
	//calcurate time difference
	boost::xtime	current_time;
	boost::xtime	time_difference;
	boost::xtime_get( &current_time, boost::TIME_UTC );
	time_difference.sec = current_time.sec - last_calc_time.sec;
	time_difference.nsec = current_time.nsec - last_calc_time.nsec;
	if( 0 > time_difference.nsec ){
		--time_difference.sec;
		time_difference.nsec = 1000000000 + time_difference.nsec;
	}
	last_calc_time = current_time;

	{
		//mutex lock
		boost::mutex::scoped_lock throughput_up_lock( throughput_up_mutex );
		boost::mutex::scoped_lock recvsize_up_lock( recvsize_up_mutex );
		//calcurate throughput

		if( 0 < current_up_recvsize ){
			if( 0 < time_difference.sec ){
				//秒が0でなければ秒をベースにスループットを計算
				//bps = current_up_recvsize[bytes] * 8[8bit=1byte] / time_difference.sec
				throughput_up	= current_up_recvsize / time_difference.sec * 8;
			}else{
				throughput_up	= current_up_recvsize / time_difference.nsec / 1000000000 * 8;
			}
		}else throughput_up = 0ULL;
		current_up_recvsize = 0ULL;
	}
	{
		//mutex lock
		boost::mutex::scoped_lock throughput_down_lock( throughput_down_mutex );
		boost::mutex::scoped_lock recvsize_down_lock( recvsize_down_mutex );
		//calcurate throughput
		//bps = current_down_recvsize[bytes] * 8[8bit=1byte] / ( time_difference / 1000 )

		if( 0 < current_down_recvsize ){
			if( 0 < time_difference.sec ){
				throughput_down = current_down_recvsize / time_difference.sec * 8;
			}else{
				throughput_down = current_down_recvsize / time_difference.nsec / 1000000000 * 8;
			}
		}else throughput_down = 0ULL;
		current_down_recvsize = 0ULL;
	}
}

/*!
 * method of rs_list reference count increase.
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::rs_list_lock(){
	boost::mutex::scoped_lock refcnt_inc_lock( rs_list_ref_count_inc_mutex );
	boost::mutex::scoped_lock refcnt_lock( rs_list_ref_count_mutex );
	if( ULLONG_MAX > rs_list_ref_count )
		++rs_list_ref_count;
	else
		rs_list_ref_count = 0;
}

/*!
 * method of rs_list reference count decrease.
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::rs_list_unlock(){
	boost::mutex::scoped_lock refcnt_lock( rs_list_ref_count_mutex );
	if( 0 < rs_list_ref_count )
		--rs_list_ref_count;
	else
		rs_list_ref_count = ULLONG_MAX;
}

/*!
 * get virtual service element.
 *
 * @param   void
 * @return  virtual service element
 */
l7vs::virtualservice_element&		l7vs::virtualservice_base::get_element(){
	boost::mutex::scoped_lock lk( element_mutex );
	//update element
// 	rs_list_lock();
// 	element.realserver_vector.clear();
// 	for( std::list<realserver>::iterator itr = rs_list.begin();
// 		 itr != rs_list.end(); ++itr ){
// 		l7vs::realserver_element	rs_element;
// 		rs_element.nactive		= itr->get_active();
// 		rs_element.ninact		= itr->get_inact();
// 		rs_element.tcp_endpoint	= itr->tcp_endpoint;
// 		rs_element.udp_endpoint	= itr->udp_endpoint;
// 		rs_element.weight		= itr->weight;
// 		element.realserver_vector.push_back( rs_element );
// 	}
// 	rs_list_unlock();

	return element;
}

/*!
 * get upstream QoS threashold value.
 *
 * @param   void
 * @return  upstream QoS threashold value
 */
unsigned long long	l7vs::virtualservice_base::get_qos_upstream(){ return element.qos_upstream; }

/*!
 * get downstream QoS threashold value.
 *
 * @param   void
 * @return  downstream QoS threashold value
 */
unsigned long long	l7vs::virtualservice_base::get_qos_downstream(){ return element.qos_downstream; }

/*!
 * get upstream bit/sec value.
 *
 * @param   void
 * @return  upstream throughput[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_throughput_upstream(){
	boost::mutex::scoped_lock lock( throughput_up_mutex );
	return throughput_up;
}

/*!
 * get downstream bit/sec value.
 *
 * @param   void
 * @return  downstream throughput[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_throughput_downstream(){
	boost::mutex::scoped_lock lock( throughput_down_mutex );
	return throughput_down;
}

/*!
 * get upstream total receive size bit/sec value.
 *
 * @param   void
 * @return  upstream receive size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_up_recv_size(){
	boost::mutex::scoped_lock lock( recvsize_up_mutex );
	return recvsize_up;
}

/*!
 * get upstream total send size bit/sec value.
 *
 * @param   void
 * @return  upstream send size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_up_send_size(){
	boost::mutex::scoped_lock lock( sendsize_up_mutex );
	return sendsize_up;
}

/*!
 * get downstream total receive size bit/sec value.
 *
 * @param   void
 * @return  downstream receive size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_down_recv_size(){
	boost::mutex::scoped_lock lock( recvsize_down_mutex );
	return recvsize_down;
}

/*!
 * get downstream total send size bit/sec value.
 *
 * @param   void
 * @return  downstream send size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_down_send_size(){
	boost::mutex::scoped_lock lock( sendsize_down_mutex );
	return sendsize_down;
}


/*!
 * update upstream receive data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_up_recv_size( unsigned long long	datasize ){
	boost::mutex::scoped_lock lock( recvsize_up_mutex );

	if( (ULLONG_MAX - current_up_recvsize) < datasize )
		current_up_recvsize	= ULLONG_MAX;
	else
		current_up_recvsize	= current_up_recvsize + datasize;

	if( (ULLONG_MAX - recvsize_up) < datasize )
		recvsize_up = datasize - (ULLONG_MAX - recvsize_up);
	else
		recvsize_up = recvsize_up + datasize;
}

/*!
 * update upstream send data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_up_send_size( unsigned long long	datasize ){
	boost::mutex::scoped_lock lock( sendsize_up_mutex );

	if( (ULLONG_MAX - sendsize_up) < datasize )
		sendsize_up = datasize - ( ULLONG_MAX - sendsize_up );
	else
		sendsize_up = sendsize_up + datasize;
}

/*!
 * update downstream receive data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_down_recv_size( unsigned long long	datasize ){
	boost::mutex::scoped_lock lock( recvsize_down_mutex );

	if( (ULLONG_MAX - current_down_recvsize) < datasize )
		current_down_recvsize = ULLONG_MAX;
	else
		current_down_recvsize = current_down_recvsize + datasize;

	if( (ULLONG_MAX - recvsize_down) < datasize )
		recvsize_down = datasize - ( ULLONG_MAX - recvsize_down );
	else
		recvsize_down = recvsize_down + datasize;
}

/*!
 * update downstream send data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_down_send_size( unsigned long long	datasize ){
	boost::mutex::scoped_lock lock( sendsize_down_mutex );

	if( (ULLONG_MAX - sendsize_down) < datasize )
		sendsize_down = datasize - ( ULLONG_MAX - sendsize_down );
	else
		sendsize_down = sendsize_down + datasize;
}
	
/*!
 * get protocol module pointer(shared_ptr)
 *
 * @param   void
 * @return  protocol module pointer(shared_ptr)
 */
l7vs::protocol_module_base*
		l7vs::virtualservice_base::get_protocol_module(){ return protomod; }

/*!
 * get schedule module pointer(shared_ptr)
 *
 * @param   void
 * @return  schedule module pointer(shared_ptr)
 */
l7vs::schedule_module_base*
		l7vs::virtualservice_base::get_schedule_module(){ return schedmod; }



// imprementation for virtualservice_tcp
/*!
 * virtualservice_tcp class constructor.
 */
l7vs::virtualservice_tcp::virtualservice_tcp(	const l7vsd& invsd,
												const replication& inrep,
												const virtualservice_element& inelement )
												 :	virtualservice_base( invsd, inrep, inelement ),
													acceptor_( dispatcher ) {}
/*!
 * virtualservice_tcp class destructor.
 */
l7vs::virtualservice_tcp::~virtualservice_tcp(){}

void	l7vs::virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& in ){
}
bool	l7vs::virtualservice_tcp::read_replicationdata( l7vs::virtualservice_base::replication_data& out ){
	return true;
}

void	l7vs::virtualservice_tcp::handle_accept(	const l7vs::virtualservice_tcp::session_thread_control_ptr in_session,
													const boost::system::error_code& in_error ){

	//switch status runing, session_thread_control
	in_session->startupstream();
	in_session->startdownstream();

	//switch active a session
	boost::thread::id			t_id;
	session_thread_control_ptr	stc_ptr;
	t_id	= pool_sessions.begin()->first;
	stc_ptr	= pool_sessions.begin()->second;

	pool_sessions.erase( t_id );
	active_sessions.insert( session_map_pair_type( stc_ptr->get_upthread_id(), stc_ptr ) );

	//regist accept event handler
	acceptor_.async_accept( stc_ptr->get_session()->get_client_socket(),
							boost::bind( &l7vs::virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
}

/*!
 * initialize virtualservice(TCP)
 *
 * @param   erro_code
 * @return  void
 */
void	l7vs::virtualservice_tcp::initialize( l7vs::error_code& err ){
	//load parameter value
	load_parameter();

	//load protocol module
	try{
		protomod = l7vs::protocol_module_control::getInstance().load_module( element.protocol_module_name );
	}
	catch( ... ){ //bad alloc exception catch
		err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
		return;
	}
	if( NULL == protomod ){
		err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
		return;
	}
	//load schedule module	
	try{
		schedmod = l7vs::schedule_module_control::getInstance().load_module( element.schedule_module_name );
	}
	catch( ... ){ //bad alloc exception catch
		err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
		return;
	}
	if( NULL == schedmod ){
		err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
		return;
	}
	//create session pool
	for( int i = 0; i < param_data.session_pool_size; ++i ){
		l7vs::tcp_session*	sess;
		try{
			sess	= new l7vs::tcp_session( *this, dispatcher );
		}
		catch( ... ){ //bad alloc exception catch
			err.setter( true, "error, create session pool." );
			return;
		}
		boost::shared_ptr<session_thread_control>	stc( new l7vs::session_thread_control( sess ) );
		std::pair<session_map_type::iterator,bool> retval;
		retval	= pool_sessions.insert( session_map_pair_type(stc->get_upthread_id(), stc) );
		if( !retval.second ){
			err.setter( true, "error, create session pool." );
			return;
		}
	}

	err.setter( false, "" );
}

void		l7vs::virtualservice_tcp::finalize( l7vs::error_code& err ){
	err.setter( false, "" );
}

/*!
 * equal compare virtualservice(TCP)
 *
 * @param   virtualservice_base
 * @return  bool ( true = equal )
 */
bool	l7vs::virtualservice_tcp::operator==( const l7vs::virtualservice_base& in ){
	l7vs::virtualservice_base&	vs = const_cast<l7vs::virtualservice_base&>( in );
	return (	( element.tcp_accept_endpoint == vs.get_element().tcp_accept_endpoint )
			&&	( element.udpmode == vs.get_element().udpmode ) );
}

/*!
 * not-equal compare virtualservice(TCP)
 *
 * @param   virtualservice_base
 * @return  bool ( true = not-equal )
 */
bool	l7vs::virtualservice_tcp::operator!=( const l7vs::virtualservice_base& in ){
	l7vs::virtualservice_base&	vs = const_cast<l7vs::virtualservice_base&>( in );
	return (	( element.tcp_accept_endpoint != vs.get_element().tcp_accept_endpoint )
			||	( element.udpmode != vs.get_element().udpmode ) );
}

void	l7vs::virtualservice_tcp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}
void	l7vs::virtualservice_tcp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

/*!
 * add realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	//check equal virtualservice
	if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
		err.setter( true, "Virtual Service is not equal." );
		return;
	}	

	//lock rs_list_ref_count_inc_mutex
	boost::mutex::scoped_lock inc_lock( rs_list_ref_count_inc_mutex );

	//waiting, rs_list_ref_count become 0
	boost::xtime		interval;
	boost::mutex		mtx;
	boost::condition	cond;

	interval.nsec = virtualservice_base::REFCOUNT_WAIT_INTERVAL;
	boost::mutex::scoped_lock lk( mtx );
	while( 0ULL != rs_list_ref_count ){
		cond.timed_wait( lk, interval );
	}

	l7vs::virtualservice_element&	in_element = const_cast<l7vs::virtualservice_element&>( in );

	//check duplication realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		for( std::list<realserver>::iterator rs_itr = rs_list.begin();
			 rs_itr != rs_list.end(); ++rs_itr ){
			if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
				err.setter( true, "Real Server is already exist." );
				return;
			}
		}
	}
	//add realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		realserver	rs;
		rs.tcp_endpoint		= itr->tcp_endpoint;
		rs.udp_endpoint		= itr->udp_endpoint;
		rs.weight			= itr->weight;
		rs_list.push_back( rs );
		rs_mutex_list.insert( std::pair<tcp_endpoint_type,mutex_ptr>( rs.tcp_endpoint,
																		mutex_ptr( new boost::mutex ) ) );
	}
	if( rs_list.size() != rs_mutex_list.size() ){
		//fatal case
		err.setter( true, "internal error" );
		return;
	}
	err.setter( false, "" );
}

/*!
 * edit realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	//lock rs_list_ref_count_inc_mutex
	boost::mutex::scoped_lock inc_lock( rs_list_ref_count_inc_mutex );

	//waiting, rs_list_ref_count become 0

	l7vs::virtualservice_element&	in_element = const_cast<l7vs::virtualservice_element&>( in );

	//edit realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
	}
	err.setter( false, "" );
}
void	l7vs::virtualservice_tcp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	//check equal virtualservice
	if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
		err.setter( true, "Virtual Service is not equal." );
		return;
	}	

	//lock rs_list_ref_count_inc_mutex
	boost::mutex::scoped_lock inc_lock( rs_list_ref_count_inc_mutex );

	//waiting, rs_list_ref_count become 0
	boost::xtime		interval;
	boost::mutex		mtx;
	boost::condition	cond;

	interval.nsec = virtualservice_base::REFCOUNT_WAIT_INTERVAL;
	boost::mutex::scoped_lock lk( mtx );
	while( 0ULL != rs_list_ref_count ){
		cond.timed_wait( lk, interval );
	}

	l7vs::virtualservice_element&	in_element = const_cast<l7vs::virtualservice_element&>( in );

	//check is exist realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		bool	exist_flag = false;
		for( std::list<realserver>::iterator rs_itr = rs_list.begin();
			 rs_itr != rs_list.end(); ++rs_itr ){
			if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
				exist_flag = true;
			}
		}
		if( !exist_flag ){
			err.setter( true, "Real Server is not exist." );
			return;
		}
	}

	//del realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		for( std::list<realserver>::iterator rs_itr = rs_list.begin();
			 rs_itr != rs_list.end(); ++rs_itr ){
			if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
				rs_list.erase( rs_itr );
				rs_mutex_list.erase( rs_mutex_list.find( itr->tcp_endpoint ) );
				break;
			}
		}
	}
	if( rs_list.size() != rs_mutex_list.size() ){
		//fatal case
		err.setter( true, "" );
		return;
	}
	err.setter( false, "" );
}

/*!
 * main loop of virtualservice(TCP)
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_tcp::run(){
	//bind acceptor
	acceptor_.open( element.tcp_accept_endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
	acceptor_.bind( element.tcp_accept_endpoint );
	acceptor_.listen();

	//switch active a session
	boost::thread::id			t_id;
	session_thread_control_ptr	stc_ptr;
	t_id	= pool_sessions.begin()->first;
	stc_ptr	= pool_sessions.begin()->second;

	pool_sessions.erase( t_id );
	active_sessions.insert( session_map_pair_type( stc_ptr->get_upthread_id(), stc_ptr ) );

	//regist accept event handler
	acceptor_.async_accept( stc_ptr->get_session()->get_client_socket(),
							boost::bind( &l7vs::virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
	//regist timer event handler
// 	handle_replication_interrupt


	//run dispatcher(start io_service loop)
	dispatcher.run();
}

/*!
 * stop loop
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_tcp::stop(){
	boost::system::error_code	err;
	//close acceptor
	acceptor_.cancel( err );
	acceptor_.close();

	//stop dispatcher
	dispatcher.stop();

	//stop all active sessions
	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		//stop session
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_END );
		//stop session_thread_control
		itr->second->stopupstream();
		itr->second->stopdownstream();
	}
}

/*!
 * increment active-connection count
 *
 * @param   endpoint
 * @return  void
 */
void	l7vs::virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){
	rs_list_lock();
	for( std::list<realserver>::iterator itr = rs_list.begin();
		 itr != rs_list.end();
		 ++itr ){
		if( itr->tcp_endpoint == in ){
			itr->increment_active();
			break;
		}
	}
	rs_list_unlock();
}

/*!
 * increment in-active-connection (and decrement active-connection count)
 *
 * @param   endpoint
 * @return  void
 */
void	l7vs::virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
	rs_list_lock();
	for( std::list<realserver>::iterator itr = rs_list.begin();
		 itr != rs_list.end();
		 ++itr ){
		if( itr->tcp_endpoint == in ){
			itr->decrement_active();
			itr->increment_inact();
			break;
		}
	}
	rs_list_unlock();
}

void	l7vs::virtualservice_tcp::release_session( const boost::thread::id thread_id ){}



// imprementation for virtualservice_udp
/*!
 * virtualservice_udp class constructor.
 */
l7vs::virtualservice_udp::virtualservice_udp(	const l7vs::l7vsd& invsd,
												const l7vs::replication& inrep,
												const l7vs::virtualservice_element& inelement) : 
													l7vs::virtualservice_base( invsd, inrep, inelement ),
													session( *this, dispatcher ){}
l7vs::virtualservice_udp::~virtualservice_udp(){}

void	l7vs::virtualservice_udp::handle_replication_interrupt( const boost::system::error_code& in ){
}
bool	l7vs::virtualservice_udp::read_replicationdata( l7vs::virtualservice_base::replication_data& out ){
	return true;
}

void	l7vs::virtualservice_udp::initialize( l7vs::error_code& err ){
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::finalize( l7vs::error_code& err ){
	err.setter( true, "" );
}

bool	l7vs::virtualservice_udp::operator==( const l7vs::virtualservice_base& in ){
	return true;
}
bool	l7vs::virtualservice_udp::operator!=( const l7vs::virtualservice_base& in ){
	return true;
}

void	l7vs::virtualservice_udp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( true, "" );
}

void	l7vs::virtualservice_udp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( true, "" );
}

void	l7vs::virtualservice_udp::run(){}
void	l7vs::virtualservice_udp::stop(){}

void	l7vs::virtualservice_udp::release_session( const boost::thread::id thread_id ){}


l7vs::virtual_service::virtual_service(	const l7vs::l7vsd& invsd,
										const l7vs::replication& inrep,
										const l7vs::virtualservice_element& inelement ){
	if( inelement.udpmode )
		vs = boost::shared_ptr<l7vs::virtualservice_base>(
				dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_udp( invsd, inrep, inelement ) ) );
	else
		vs = boost::shared_ptr<l7vs::virtualservice_base>(
				dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_tcp( invsd, inrep, inelement ) ) );
}

l7vs::virtual_service::~virtual_service(){
}

void	l7vs::virtual_service::initialize( l7vs::error_code& err ){
	if( NULL != vs )
		vs->initialize( err );
	else{
		err.setter( false, "Fail, create VirtualService" );
	}
}
void	l7vs::virtual_service::finalize( l7vs::error_code& err ){
	vs->finalize( err );
}

bool	l7vs::virtual_service::operator==( const l7vs::virtualservice_base& in ){
	return vs->operator==( in );
}
bool	l7vs::virtual_service::operator!=( const l7vs::virtualservice_base& in ){
	return vs->operator!=( in );
}

void	l7vs::virtual_service::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->set_virtualservice( in, err );
}
void	l7vs::virtual_service::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->edit_virtualservice( in, err );
}

void	l7vs::virtual_service::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->add_realserver( in, err );
}
void	l7vs::virtual_service::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->edit_realserver( in, err );
}
void	l7vs::virtual_service::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->del_realserver( in, err );
}

l7vs::virtualservice_element&	l7vs::virtual_service::get_element(){
	return vs->get_element();
}

void		l7vs::virtual_service::run(){
	vs->run();
}
void		l7vs::virtual_service::stop(){
	vs->stop();
}

void		l7vs::virtual_service::connection_active( const boost::asio::ip::tcp::endpoint& in ){
	vs->connection_active( in );
}
void		l7vs::virtual_service::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
	vs->connection_inactive( in );
}
void		l7vs::virtual_service::release_session( const boost::thread::id thread_id ){
	vs->release_session( thread_id );
}

unsigned long long		l7vs::virtual_service::get_qos_upstream(){
	return vs->get_qos_upstream();
}
unsigned long long		l7vs::virtual_service::get_qos_downstream(){
	return vs->get_qos_downstream();
}
unsigned long long		l7vs::virtual_service::get_throughput_upstream(){
	return vs->get_throughput_upstream();
}
unsigned long long		l7vs::virtual_service::get_throughput_downstream(){
	return vs->get_throughput_downstream();
}

unsigned long long		l7vs::virtual_service::get_up_recv_size(){
	return vs->get_up_recv_size();
}

unsigned long long		l7vs::virtual_service::get_up_send_size(){
	return vs->get_up_send_size();
}

unsigned long long		l7vs::virtual_service::get_down_recv_size(){
	return vs->get_down_recv_size();
}

unsigned long long		l7vs::virtual_service::get_down_send_size(){
	return vs->get_down_send_size();
}

void		l7vs::virtual_service::update_up_recv_size( unsigned long long	datasize ){
	vs->update_up_recv_size( datasize );
}
void		l7vs::virtual_service::update_up_send_size( unsigned long long	datasize ){
	vs->update_up_send_size( datasize );
}
void		l7vs::virtual_service::update_down_recv_size( unsigned long long	datasize ){
	vs->update_down_recv_size( datasize );
}
void		l7vs::virtual_service::update_down_send_size( unsigned long long	datasize ){
	vs->update_down_send_size( datasize );
}
	
l7vs::protocol_module_base*
			l7vs::virtual_service::get_protocol_module(){
	return vs->get_protocol_module();
}
l7vs::schedule_module_base*
			l7vs::virtual_service::get_schedule_module(){
	return vs->get_schedule_module();
}
