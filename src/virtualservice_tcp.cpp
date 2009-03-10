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
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "virtualservice.h"
#include "logger_enum.h"
#include "logger.h"
#include "parameter.h"

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
l7vs::virtualservice_tcp::~virtualservice_tcp(){
}

/*!
 * replication interrupt event
 *
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& err ){
	boost::format	fmt("in_function : void virtualservice_tcp::handle_replication_interrupt( "
						"const boost::system::error_code& err ) : err = %s, err.message = %s");
	fmt % ( err ? "true" : "false") % err.message();
	Logger	functionlog( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );

	if( err ){
		//ERROR case
		Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );
		return;
	}

	bool	newdata	= true;
	replication&	rep_noconst = const_cast<replication&>( rep );

	if( replication::REPLICATION_SINGLE == rep_noconst.get_status() ){
		Logger::putLogInfo( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "replication mode is single.", __FILE__, __LINE__ );
		return;
	}

	std::stringstream	tmp_tcp_ep;
	tmp_tcp_ep		<< element.tcp_accept_endpoint;
	std::stringstream	tmp_udp_ep;
	tmp_udp_ep		<< element.udp_recv_endpoint;
	std::stringstream	tmp_sorry_ep;
	tmp_sorry_ep	<< element.sorry_endpoint;
	
	//get replication area
	unsigned int	rep_size = 0;
	replication_header*	rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
	if( (rep_header_ptr == NULL) || (0 == rep_size) ){
		l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& err )", __FILE__, __LINE__ );
		}
		return;
	}

	//check maxdatasize
	if( ( rep_size * DATA_SIZE ) <
		((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
		l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& err )", __FILE__, __LINE__ );
		}
		return;
	}

	//lock replication area
	rep_noconst.lock( REP_AREA_NAME );

	//read header value and set loop count
	unsigned int loop_cnt = rep_header_ptr->data_num;
	//if data_num over MAX_REPLICATION_DATA_NUM, data_num = 0
	if( static_cast<unsigned int>(MAX_REPLICATION_DATA_NUM) < loop_cnt )
		loop_cnt = 0;

	//set start pointer(pointer of replication_data)
	replication_data*	rep_data_ptr = reinterpret_cast<replication_data*>( ++rep_header_ptr );

	//find vs(loop)
	for( unsigned int i = 0; i < loop_cnt; ++i ){
		//check equal udpmode and tcp_accept_endpoint
		if( (rep_data_ptr->udpmode == element.udpmode )&&
			( 0 == strncmp( rep_data_ptr->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) ) ){
			newdata = false;
			break;
		}
		//increment data pointer
		++rep_data_ptr;
	}

	//if it is new data, increment data num.
	if( newdata ){
		rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
		++(rep_header_ptr->data_num);
	}
	//write replication data
	rep_data_ptr->udpmode		= element.udpmode;
	strncpy( rep_data_ptr->tcp_endpoint, tmp_tcp_ep.str().c_str(), 48 );
	strncpy( rep_data_ptr->udp_endpoint, tmp_udp_ep.str().c_str(), 48 );
	rep_data_ptr->sorry_maxconnection	= element.sorry_maxconnection;
	strncpy( rep_data_ptr->sorry_endpoint, tmp_sorry_ep.str().c_str(), 48 );
	rep_data_ptr->sorry_flag	= element.sorry_flag;
	rep_data_ptr->qos_up		= element.qos_upstream;
	rep_data_ptr->qos_down		= element.qos_downstream;

	//unlock replication area
	rep_noconst.unlock( REP_AREA_NAME );

	//register handle_replication_interrupt
	replication_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
	replication_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_replication_interrupt, 
											this, boost::asio::placeholders::error ) );
}

/*!
 * read replication data
 *
 * @param   read_replicationdata
 * @param	error_code
 * @return  void
 */
void	l7vs::virtualservice_tcp::read_replicationdata(){
	Logger	funclog( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );

	replication&	rep_noconst = const_cast<replication&>( rep );

	if( replication::REPLICATION_SINGLE == rep_noconst.get_status() ){
		Logger::putLogInfo( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "replication mode is single.", __FILE__, __LINE__ );
		return;
	}

	unsigned int	rep_size = 0;
	replication_header*	rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
	if( (rep_header_ptr == NULL) || (0 == rep_size) ){
		l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
		}
		return;
	}

	//check maxdatasize
	if( ( rep_size * DATA_SIZE ) <
		((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
		l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
		}
		return;
	}
	//lock replication area
	rep_noconst.lock( REP_AREA_NAME );

	//read header value and set loop count
	unsigned int loop_cnt = rep_header_ptr->data_num;
	//if data_num over MAX_REPLICATION_DATA_NUM, set data_num = 0
	if( static_cast<unsigned int>(MAX_REPLICATION_DATA_NUM) < loop_cnt ){
		l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
		}
		rep_header_ptr->data_num = 0;
		return;
	}

	//set start pointer(pointer of replication_data)
	replication_data*	rep_data_ptr = reinterpret_cast<replication_data*>( ++rep_header_ptr );

	//find vs(loop)
	for( unsigned int i = 0; i < loop_cnt; ++i ){
		//get tcp endpointdata
		std::string		tmpstr = rep_data_ptr->tcp_endpoint;
		std::string		ipaddr;
		unsigned short	portno;
		try{
			portno	= boost::lexical_cast<unsigned short>( tmpstr.substr( (tmpstr.rfind( ":" )+1) ) );
		}
		catch( const boost::bad_lexical_cast& err ){
			portno	= 0;
		}
		if( tmpstr[0] == '[' ){
			//IPv6 endpoint
			ipaddr	= tmpstr.substr( 1, (tmpstr.rfind( "]" )-1) );
		}else{
			//IPv4 endpoint
			ipaddr	= tmpstr.substr( 0, tmpstr.rfind( ":" ) );
		}
		//check equal udpmode and tcp_accept_endpoint
		if( (rep_data_ptr->udpmode == element.udpmode )&&
			( tcp_endpoint_type( boost::asio::ip::address::from_string( ipaddr ), portno ) == element.tcp_accept_endpoint ) ){
				element.sorry_maxconnection	= rep_data_ptr->sorry_maxconnection;
			tmpstr	= rep_data_ptr->sorry_endpoint;
			try{
				portno	= boost::lexical_cast<unsigned short>( tmpstr.substr( (tmpstr.rfind( ":" )+1) ) );
			}
			catch( const boost::bad_lexical_cast& err ){
				portno	= 0;
			}
			if( tmpstr[0] == '[' ){
				//IPv6 endpoint
				ipaddr	= tmpstr.substr( 1, (tmpstr.rfind( "]" )-1) );
			}else{
				//IPv4 endpoint
				ipaddr	= tmpstr.substr( 0, tmpstr.rfind( ":" ) );
			}
			element.sorry_endpoint		= 
					tcp_endpoint_type( boost::asio::ip::address::from_string( ipaddr ), portno );
			element.sorry_flag			= rep_data_ptr->sorry_flag;
			element.qos_upstream		= rep_data_ptr->qos_up;
			element.qos_downstream		= rep_data_ptr->qos_down;
			break;
		}
		//increment data pointer
		++rep_data_ptr;
	}

	//unlock replication area
	rep_noconst.unlock( REP_AREA_NAME );

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
	}
}

/*!
 * accept event
 *
 * @param   handle_accept
 * @param	error_code
 * @return  void
 */
void	l7vs::virtualservice_tcp::handle_accept( const l7vs::session_thread_control* stc_ptr, const boost::system::error_code& err ){
	boost::format funclog_fmt("in_function : void virtualservice_tcp::handle_accept( "
							"const boost::shared_ptr<session_thread_control> , "
							"const boost::system::error_code& err ) : err = %s, err.message = %s");
	funclog_fmt % ( err ? "true" : "false") % err.message();
	l7vs::Logger	funclog( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, funclog_fmt.str(), __FILE__, __LINE__ );

	session_thread_control*		stc_ptr_noconst = const_cast<session_thread_control*>( stc_ptr );

	if( err ){
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );
		return;
	}

	bool	sorry_status = false;
	//check sorry status
	if( ( 0 < element.sorry_maxconnection ) && 
		( ( active_sessions.size() - sorry_sessions.size() ) >= static_cast<size_t>( element.sorry_maxconnection ) ) ||
		( ( 0 != element.sorry_flag ) && ( INT_MAX != element.sorry_flag ) ) ||
		( ( 0 == rs_list.size() ) && ( boost::asio::ip::tcp::endpoint() != element.sorry_endpoint ) ) ) {
		sorry_status = true;
		if( LOG_LV_ERROR == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt( "Connection switch Sorry mode. : active_session.size = %d / sorry_session.size = %d" );
			fmt % active_sessions.size() % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}

	{
		boost::mutex::scoped_lock actives_lk( active_sessions_mutex );
		active_sessions.insert( std::make_pair( stc_ptr_noconst->get_upthread_id(), stc_ptr_noconst ) );
		if( sorry_status ){
			boost::mutex::scoped_lock sorrys_lk( sorry_sessions_mutex );
			sorry_sessions.insert( std::make_pair( stc_ptr_noconst->get_upthread_id(), stc_ptr_noconst ) );
			stc_ptr_noconst->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
		}
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt1( "active session thread id = %d" );
			fmt1 % stc_ptr_noconst->get_upthread_id();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
			boost::format	fmt2( "pool_session.size   = %d" );
			fmt2 % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
			boost::format	fmt3( "active_session.size = %d" );
			fmt3 % active_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt3.str(), __FILE__, __LINE__ );
			boost::format	fmt4( "sorry_session.size = %d" );
			fmt4 % sorry_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt4.str(), __FILE__, __LINE__ );
		}
	}

	session_map_type::iterator itr = waiting_sessions.find( stc_ptr_noconst->get_upthread_id() );
	waiting_sessions.erase( itr );

	stc_ptr_noconst->startupstream();
	stc_ptr_noconst->startdownstream();

	//waiting, pool_sessions.size become over 1
	while( pool_sessions.empty() ){
		boost::this_thread::yield();
	}

	//pick up session from pool
	session_thread_control*		stc_ptr_register_accept;
	{
		boost::mutex::scoped_lock	pool_lk( pool_sessions_mutex );
		stc_ptr_register_accept = pool_sessions.front();
		pool_sessions.erase( pool_sessions.begin() );
	}

	//session add wait_sessions
	boost::mutex::scoped_lock	up_wait_lk( stc_ptr_register_accept->get_upthread_mutex() );
	boost::mutex::scoped_lock	down_wait_lk( stc_ptr_register_accept->get_downthread_mutex() );
	boost::mutex::scoped_lock	wait_lk( waiting_sessions_mutex );
	waiting_sessions.insert( std::make_pair( stc_ptr_register_accept->get_upthread_id(), stc_ptr_register_accept ) );

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format	fmt1( "active session thread id = %d" );
		fmt1 % stc_ptr_register_accept->get_upthread_id();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
		boost::format	fmt2( "pool_session.size   = %d" );
		fmt2 % pool_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
		boost::format	fmt3( "active_session.size = %d" );
		fmt3 % active_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt3.str(), __FILE__, __LINE__ );
		boost::format	fmt4( "sorry_session.size = %d" );
		fmt4 % sorry_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt4.str(), __FILE__, __LINE__ );
	}

	//regist accept event handler
	acceptor_.async_accept( stc_ptr_register_accept->get_session()->get_client_socket(),
					boost::bind( &l7vs::virtualservice_tcp::handle_accept, this, stc_ptr_register_accept, boost::asio::placeholders::error ) );
}

/*!
 * initialize virtualservice(TCP)
 *
 * @param   erro_code
 * @return  void
 */
void	l7vs::virtualservice_tcp::initialize( l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::initialize( "
								"l7vs::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
	if( pool_sessions.size() != 0 ){
		err.setter( true, "VirtualService already initialized." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}

	//load parameter value
	load_parameter( err );
	if( err ){
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}

	//get cpumask
	boost::asio::ip::address	address	= element.tcp_accept_endpoint.address();
#ifdef	SCHED_SETAFFINITY
	vsnic_cpumask	= get_cpu_mask( address );
	rsnic_cpumask	= get_cpu_mask( param_data.nic_realserver_side );
#endif

	//bind acceptor
	boost::system::error_code	acceptor_err;
	acceptor_.open( element.tcp_accept_endpoint.protocol(), acceptor_err );
	if( acceptor_err ){
		l7vs::Logger::putLogError( 
			l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, acceptor_err.message(), __FILE__, __LINE__ );
		err.setter( true, acceptor_err.message() );
		return;
	}
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ), acceptor_err );
	if( acceptor_err ){
		l7vs::Logger::putLogError( 
			l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, acceptor_err.message(), __FILE__, __LINE__ );
		err.setter( true, acceptor_err.message() );
		return;
	}
	acceptor_.bind( element.tcp_accept_endpoint, acceptor_err );
	if( acceptor_err ){
		l7vs::Logger::putLogError( 
			l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, acceptor_err.message(), __FILE__, __LINE__ );
		err.setter( true, acceptor_err.message() );
		return;
	}

	//read replication data
	read_replicationdata();

	//load protocol module
	protomod = l7vs::protocol_module_control::getInstance().load_module( element.protocol_module_name );
	if( NULL == protomod ){
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, PROTOMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
		err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}
	//Protocol Module Initialize
	protomod->init_logger_functions(
					boost::bind( &l7vs::Logger::getLogLevel, l7vs::LOG_CAT_PROTOCOL ),
					boost::bind( &l7vs::Logger::putLogFatal, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogError, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogWarn, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogInfo, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogDebug, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4 ) );
	protomod->initialize(
					boost::bind( &l7vs::virtualservice_tcp::rs_list_begin, this ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_end, this ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_next, this, _1 ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_lock, this ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_unlock, this ) );
	protomod->init_replication_functions(
					boost::bind( &l7vs::virtualservice_tcp::replication_pay_memory, this, _1, _2 ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_lock, this ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_unlock, this ),
					element.tcp_accept_endpoint,
					element.udp_recv_endpoint );
	tcp_schedule_func_type	sched_rs_func	= boost::bind( &l7vs::virtualservice_tcp::schedule_rs, this, _1, _2, _3, _4, _5 );
	protomod->register_schedule( sched_rs_func );

	l7vs::protocol_module_base::check_message_result pm_result;
	pm_result	=	protomod->check_parameter( element.protocol_args );
	if( !pm_result.flag ){
		err.setter( true, "Protocol Module argument error." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}
	pm_result	=	protomod->set_parameter( element.protocol_args );
	if( !pm_result.flag ){
		err.setter( true, "Protocol Module argument error." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}

	//load schedule module	
	schedmod = l7vs::schedule_module_control::getInstance().load_module( element.schedule_module_name );
	if( NULL == schedmod ){
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, SCHEDMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
		err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::initialize( "
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}
	//Schedule Module Initialize
	schedmod->init_logger_functions(
					boost::bind( &l7vs::Logger::getLogLevel, l7vs::LOG_CAT_SCHEDULE ),
					boost::bind( &l7vs::Logger::putLogFatal, l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogError, l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogWarn, l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogInfo, l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
					boost::bind( &l7vs::Logger::putLogDebug, l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 ) );
	schedmod->initialize();
	schedmod->init_replication_functions(
					boost::bind( &l7vs::virtualservice_tcp::replication_pay_memory, this, _1, _2 ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_lock, this ),
					boost::bind( &l7vs::virtualservice_tcp::rs_list_unlock, this ),
					element.tcp_accept_endpoint,
					element.udp_recv_endpoint );

	//create session pool
	{
		boost::mutex::scoped_lock lk( pool_sessions_mutex );
		for( int i = 0; i < param_data.session_pool_size; ++i ){
			try{
				tcp_session*	sess	= new tcp_session( *this, dispatcher );
				l7vs::session_result_message	result	= sess->initialize();
				if( result.flag == true ){
					err.setter( result.flag, result.message );
					if( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
						boost::format formatter("out_function : void virtualservice_tcp::initialize( "
												"l7vs::error_code& err ) : err = %s, err.message = %s");
						formatter % ( err ? "true" : "false") % err.get_message();
						Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
					}
					return;
				}
				session_thread_control*	p_stc = new session_thread_control( sess, vsnic_cpumask, rsnic_cpumask );
				pool_sessions.push_back( p_stc );
			}
			catch( std::bad_alloc ex ){
				Logger::putLogFatal( 
					l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "error, create session.", __FILE__, __LINE__ );
				if( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
					boost::format formatter("out_function : void virtualservice_tcp::initialize( "
											"l7vs::error_code& err ) : err = %s, err.message = %s");
					formatter % ( err ? "true" : "false") % err.get_message();
					Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
				}
				return;
			}
		}
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt1( "pool_session.size   = %d" );
			fmt1 % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
			boost::format	fmt2( "active_session.size = %d" );
			fmt2 % active_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
		}
	}

	err.setter( false, "" );

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::initialize( "
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * finalize virtualservice(TCP)
 *
 * @param   erro_code
 * @return  void
 */
void		l7vs::virtualservice_tcp::finalize( l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::finalize( "
								"l7vs::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );

		boost::format	fmt1( "pool_session.size   = %d" );
		fmt1 % pool_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
		boost::format	fmt2( "active_session.size = %d" );
		fmt2 % active_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
	}

	//stop main loop
	stop();

	while(1){
		{
			if( 0 == active_sessions.size() )
				break;
		}
	}

	while( !waiting_sessions.empty() ){
		session_map_type::iterator itr = waiting_sessions.begin();
		boost::mutex::scoped_lock lk1( waiting_sessions_mutex );
		session_thread_control*	stc = itr->second;
		waiting_sessions.erase( itr );
		boost::mutex::scoped_lock lk2( pool_sessions_mutex );
		pool_sessions.push_back( stc );
	}

	//release sessions[i]->join();
	while( !pool_sessions.empty() ){
		boost::mutex::scoped_lock lk( pool_sessions_mutex );
		pool_sessions.front()->join();
		{
			boost::mutex::scoped_lock upthread_wait( pool_sessions.front()->get_upthread_mutex() );
			boost::mutex::scoped_lock downthread_wait( pool_sessions.front()->get_downthread_mutex() );
		}
		session_thread_control*	stc = pool_sessions.front();
		pool_sessions.erase( pool_sessions.begin() );
		delete	stc;
		stc = NULL;
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt( "join pool sessin : pool_sessions.size = %d" );
			fmt % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}

	//unload ProtocolModule
	if( protomod ){
		//finalize ProtocolModule
		protomod->finalize();
		//unload ProtocolModule
		l7vs::protocol_module_control::getInstance().unload_module( protomod );
		protomod = NULL;
	}
	//unload ScheduleModule
	if( schedmod ){
		l7vs::schedule_module_control::getInstance().unload_module( schedmod );
		schedmod = NULL;
	}

	active_sessions.clear();
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format	fmt1( "pool_session.size   = %d" );
		fmt1 % pool_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
		boost::format	fmt2( "active_session.size = %d" );
		fmt2 % active_sessions.size();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
	}

	//clear replication_data
	bool	replication_status	= true;
	replication&	rep_noconst = const_cast<replication&>( rep );

	if( replication::REPLICATION_SINGLE != rep_noconst.get_status() ){
		unsigned int	rep_size = 0;
		replication_header*	rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
		if( (rep_header_ptr == NULL) || (0 == rep_size) ){
			err.setter( true, REP_BLOCK_SIZE_ERR_MSG );
			l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
			replication_status = false;
		}
	
		//check maxdatasize
		if( ( rep_size * DATA_SIZE ) <
			((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
			err.setter( true, REP_AREA_SIZE_ERR_MSG );
			l7vs::Logger::putLogWarn( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
			replication_status = false;
		}
		if( (NULL != rep_header_ptr) && (replication_status) ){
			//lock replication area
			rep_noconst.lock( REP_AREA_NAME );
			//set data_num = 0
			rep_header_ptr->data_num = 0;
			//unlock replication area
			rep_noconst.unlock( REP_AREA_NAME );
		}
	}

	vsd.release_virtual_service( element );

	err.setter( false, "" );

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::finalize( "
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * equal compare virtualservice(TCP)
 *
 * @param   virtualservice_base
 * @return  bool ( true = equal )
 */
bool	l7vs::virtualservice_tcp::operator==( const l7vs::virtualservice_base& in ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in/out_function : void virtualservice_tcp::operator==( "
								"const l7vs::virtualservice_base& in )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
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
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in/out_function : void virtualservice_tcp::operator!=( "
								"const l7vs::virtualservice_base& in )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
	l7vs::virtualservice_base&	vs = const_cast<l7vs::virtualservice_base&>( in );
	return (	( element.tcp_accept_endpoint != vs.get_element().tcp_accept_endpoint )
			||	( element.udpmode != vs.get_element().udpmode ) );
}

/*!
 * add VirtualService( not-imprement )
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

/*!
 * edit VirtualService
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::edit_virtualservice( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err ) : dump in ");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}

	l7vs::virtualservice_element&	elem = const_cast<l7vs::virtualservice_element&>( in );
	//パラメータがVirtualServiceに一致するか検査
	//udpmodeとtcp_accept_endpointとprotocol_module_nameが一致すること
	if( ( element.udpmode != elem.udpmode ) ||
		( element.tcp_accept_endpoint != elem.tcp_accept_endpoint ) ||
		( element.protocol_module_name != elem.protocol_module_name ) ){
		err.setter( true, "Virtual Service does not exist." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}

	//if change ScueduleModule Name, unload old ScheduleModule and load new ScheduleModule
	if( ( element.schedule_module_name != elem.schedule_module_name ) && ( "" != elem.schedule_module_name ) ){
		schedule_module_control::getInstance().unload_module( schedmod );
		schedmod = schedule_module_control::getInstance().load_module( elem.schedule_module_name );
		if( NULL == schedmod ){
			//ERROR case
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, SCHEDMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
			err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
			if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
				boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
										"const l7vs::virtualservice_element& in,"
										"l7vs::error_code& err ) : err = %s, err.message = %s");
				formatter % ( err ? "true" : "false") % err.get_message();
				l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
			}
			return;
		}
		element.schedule_module_name = elem.schedule_module_name;
	}

	//additional PM options(for protomod_url)
	l7vs::protocol_module_base::check_message_result result;
	if( NULL != protomod ){
		result = protomod->add_parameter( elem.protocol_args );
		if( result.flag ){
			for( size_t i = 0; i < elem.protocol_args.size(); ++i ){
				element.protocol_args.push_back( elem.protocol_args[i] );
			}
		}else{
			err.setter( true, result.message );
			if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
				boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
										"const l7vs::virtualservice_element& in,"
										"l7vs::error_code& err ) : err = %s, err.message = %s");
				formatter % ( err ? "true" : "false") % err.get_message();
				l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
			}
			return;
		}
	}else{
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, PROTOMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
		err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}

	//update values
	//qos_upstream;
	if( ULLONG_MAX == elem.qos_upstream )
		element.qos_upstream	= 0ULL;
	else if( 0ULL != elem.qos_upstream )
		element.qos_upstream	= elem.qos_upstream;
	//qos_downstream;
	if( ULLONG_MAX == elem.qos_downstream )
		element.qos_downstream	= 0ULL;
	else if( 0ULL != elem.qos_downstream )
		element.qos_downstream	= elem.qos_downstream;
	//sorry_maxconnection;
	if( LLONG_MAX == elem.sorry_maxconnection )
		element.sorry_maxconnection	= 0;
	else if( 0 != elem.sorry_maxconnection )
		element.sorry_maxconnection	= elem.sorry_maxconnection;
	//sorry_flag;
	if( INT_MAX == elem.sorry_flag )
		element.sorry_flag			= 0;
	else if( 0 != elem.sorry_flag )
		element.sorry_flag			= 1;
	//sorry_endpoint;
	//if endpoint of SorryServer equal 255.255.255.255:0,not update
	//if equal endpoint 255.255.255.255:0, clear sorry parameters
	if( elem.sorry_endpoint ==
			boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "255.255.255.255" ), (0)) ){
		element.sorry_endpoint			= boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) );
		element.sorry_maxconnection	= 0LL;
		element.sorry_flag			= false;
		boost::mutex::scoped_lock lk( active_sessions_mutex );
		for( session_map_type::iterator itr = active_sessions.begin();
			itr != active_sessions.end();
			++itr ){
			itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );
		}
	}else{
		if( elem.sorry_endpoint !=
				boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) ) )
			element.sorry_endpoint		= elem.sorry_endpoint;

		boost::mutex::scoped_lock lk( active_sessions_mutex );
		for( session_map_type::iterator itr = active_sessions.begin();
			itr != active_sessions.end();
			++itr ){
			if( INT_MAX == elem.sorry_flag )
				itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );
			else if( 0 != elem.sorry_flag )
				itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
		}
	}

	err.setter( false, "" );

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::set_virtualservice( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * add realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::add_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}

	l7vs::virtualservice_element&	in_element = const_cast<l7vs::virtualservice_element&>( in );

	//check equal virtualservice
	if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
		err.setter( true, "Virtual Service does not exist." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
		return;
	}	
	//check rs num
	if( in.realserver_vector.size() == 0U ){
		err.setter( true, "." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
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

	//check duplication realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		for( std::list<realserver>::iterator rs_itr = rs_list.begin();
			 rs_itr != rs_list.end(); ++rs_itr ){
			if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
				err.setter( true, "Real Server already exist." );
				if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
					boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
											"const l7vs::virtualservice_element& in,"
											"l7vs::error_code& err ) : err = %s, err.message = %s");
					formatter % ( err ? "true" : "false") % err.get_message();
					l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
				}
				return;
			}
		}
	}

	//pause active sessions
	boost::mutex::scoped_lock session_lk( active_sessions_mutex );

	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
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
		l7vs::Logger::putLogFatal( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "RealServer information data error", __FILE__, __LINE__ );
		err.setter( true, "RealServer information data error" );
	}else
		err.setter( false, "" );

	//run active sessions
	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
	}

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * edit realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::edit_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
	//check equal virtualservice
	if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
		err.setter( true, "Virtual Service does not exist." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
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
			err.setter( true, "Real Server does not exist." );
			if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
				boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
										"const l7vs::virtualservice_element& in,"
										"l7vs::error_code& err ) : err = %s, err.message = %s");
				formatter % ( err ? "true" : "false") % err.get_message();
				l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
			}
			return;
		}
	}

	//pause active sessions
	boost::mutex::scoped_lock session_lk( active_sessions_mutex );

	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	}

	//edit realserver
	for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		for( std::list<realserver>::iterator rs_itr = rs_list.begin();
			 rs_itr != rs_list.end(); ++rs_itr ){
			if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
				rs_itr->udp_endpoint	= itr->udp_endpoint;
				rs_itr->weight			= itr->weight;
			}
		}
	}

	//run active sessions
	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
	}

	err.setter( false, "" );
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * delete realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void	l7vs::virtualservice_tcp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::del_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}

	//check equal virtualservice
	if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
		err.setter( true, "Virtual Service is not equal." );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
									"const l7vs::virtualservice_element& in,"
									"l7vs::error_code& err ) : err = %s, err.message = %s");
			formatter % ( err ? "true" : "false") % err.get_message();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
		}
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
			err.setter( true, "Real Server does not exist." );
			if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
				boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
										"const l7vs::virtualservice_element& in,"
										"l7vs::error_code& err ) : err = %s, err.message = %s");
				formatter % ( err ? "true" : "false") % err.get_message();
				l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
			}
			return;
		}
	}

	//pause active sessions
	boost::mutex::scoped_lock session_lk( active_sessions_mutex );

	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
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
		l7vs::Logger::putLogFatal( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "RealServer information data error", __FILE__, __LINE__ );
		err.setter( true, "RealServer information data error" );
	}else
		err.setter( false, "" );

	//run active sessions
	for( session_map_type::iterator itr = active_sessions.begin();
		 itr != active_sessions.end();
		 ++itr ){
		itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
	}

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
								"const l7vs::virtualservice_element& in,"
								"l7vs::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.get_message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

/*!
 * main loop of virtualservice(TCP)
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_tcp::run(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in_function : void virtualservice_tcp::run()", __FILE__, __LINE__ );
	}

	if( pool_sessions.size() == 0 ){
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "VirtualService not initialized.", __FILE__, __LINE__ );
		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_tcp::run()", __FILE__, __LINE__ );
		}
		return;
	}

	//start listen
	acceptor_.listen();

	//switch active a session
	session_thread_control*	stc_ptr;
	{
		boost::mutex::scoped_lock	lk( pool_sessions_mutex );
		if( pool_sessions.empty() ){
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "VirtualService not initialize.", __FILE__, __LINE__ );
			if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
				l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_tcp::run()", __FILE__, __LINE__ );
			}
			return;
		}
		//regist accept event handler
		stc_ptr = pool_sessions.front();
		pool_sessions.erase( pool_sessions.begin() );
		boost::mutex::scoped_lock	waiting_pool_lock( waiting_sessions_mutex );
		waiting_sessions.insert( std::make_pair( stc_ptr->get_upthread_id(), stc_ptr ) );
	}
	acceptor_.async_accept( stc_ptr->get_session()->get_client_socket(),
					boost::bind( &l7vs::virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
	//regist timer event handler
	calc_bps_timer->expires_from_now( boost::posix_time::milliseconds( param_data.bps_interval ) );
	calc_bps_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_throughput_update, 
											 this, boost::asio::placeholders::error ) );

	//register handle_replication_interrupt
	replication_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
	replication_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_replication_interrupt, 
											 this, boost::asio::placeholders::error ) );
	//register handle_protomod_replication
	protomod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
	protomod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_protomod_replication, 
											 this, boost::asio::placeholders::error ) );
	//register handle_schedmod_replication
	schedmod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
	schedmod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_schedmod_replication, 
											 this, boost::asio::placeholders::error ) );

	//run dispatcher(start io_service loop)
	dispatcher.run();

	//stop all active sessions
	{
		boost::mutex::scoped_lock lk( active_sessions_mutex );
		for( session_map_type::iterator itr = active_sessions.begin();
			itr != active_sessions.end();
			++itr ){
			//stop session
			itr->second->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_END );
		}
	}

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_tcp::run()", __FILE__, __LINE__ );
	}
}

/*!
 * stop loop
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_tcp::stop(){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in_function : void virtualservice_tcp::stop()", __FILE__, __LINE__ );
	}

	boost::system::error_code	err;

	acceptor_.close( err );
	if( err ){
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );
	}

	//stop dispatcher
	dispatcher.stop();



	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_tcp::stop()", __FILE__, __LINE__ );
	}
}

/*!
 * increment active-connection count
 *
 * @param   endpoint
 * @return  void
 */
void	l7vs::virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::connection_active( "
								"const boost::asio::ip::tcp::endpoint& in ) : in = %s");
		formatter % in;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
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

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_tcp::connection_active()", __FILE__, __LINE__ );
	}
}

/*!
 * increment in-active-connection (and decrement active-connection count)
 *
 * @param   endpoint
 * @return  void
 */
void	l7vs::virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format formatter("in_function : void virtualservice_tcp::connection_inactive( "
								"const boost::asio::ip::tcp::endpoint& in ) : in = %s");
		formatter % in;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, formatter.str(), __FILE__, __LINE__ );
	}
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

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_tcp::connection_inactive()", __FILE__, __LINE__ );
	}
}

/*!
 * release_session
 *
 * @param   thread_id
 * @return  void
 */
void	l7vs::virtualservice_tcp::release_session( const boost::thread::id thread_id ){
	
	boost::format funclog_fmt("in_function : void virtualservice_tcp::release_session( "
							"const boost::thread::id thread_id ) : thread_id = %d");
	funclog_fmt % thread_id;
	l7vs::Logger	func_log( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, funclog_fmt.str(), __FILE__, __LINE__ );

	boost::mutex::scoped_lock actives_lk( active_sessions_mutex );
	session_map_type::iterator	itr = active_sessions.find( thread_id );
	if( itr != active_sessions.end() ){
		session_thread_control*	stc_ptr = itr->second;

		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt1( "pool_session.size   = %d" );
			fmt1 % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
			boost::format	fmt2( "active_session.size = %d" );
			fmt2 % active_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
			boost::format	fmt3( "sorry_session.size = %d" );
			fmt3 % sorry_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt3.str(), __FILE__, __LINE__ );
		}
		active_sessions.erase( itr );
		boost::mutex::scoped_lock pools_lk( pool_sessions_mutex );
		stc_ptr->get_session()->initialize();
		pool_sessions.push_back( stc_ptr );

		{
			boost::mutex::scoped_lock sorrys_lk( sorry_sessions_mutex );
			session_map_type::iterator sorry_itr = sorry_sessions.find( thread_id );
			if( sorry_itr != sorry_sessions.end() ){
				sorry_sessions.erase( sorry_itr );
			}
		}

		if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
			boost::format	fmt1( "pool_session.size   = %d" );
			fmt1 % pool_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt1.str(), __FILE__, __LINE__ );
			boost::format	fmt2( "active_session.size = %d" );
			fmt2 % active_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt2.str(), __FILE__, __LINE__ );
			boost::format	fmt3( "sorry_session.size = %d" );
			fmt3 % sorry_sessions.size();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt3.str(), __FILE__, __LINE__ );
		}
	}else if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format	fmt( "session release fail : active_sessions.find( const boost::thread::id thread_id = %d )");
		fmt % thread_id;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
}
