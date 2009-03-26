//
//!	@file	virtualservice_base.cpp
//!	@brief	VirtualService base class implementations
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

// #include <new>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "virtualservice.h"
#include "logger_enum.h"
#include "logger.h"
#include "parameter.h"

#include "utility.h"

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
													throughput_down( 0 ),
													wait_count_up( 0 ),
													wait_count_down( 0 ) {
	calc_bps_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	replication_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	protomod_rep_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	schedmod_rep_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	rs_list.clear();
	rs_mutex_list.clear();
	protomod = NULL;
	schedmod = NULL;
	this_id = boost::this_thread::get_id();
	//
	element = inelement;
	if( LLONG_MAX == inelement.sorry_maxconnection )
		element.sorry_maxconnection = 0LL;
	if( INT_MAX == inelement.sorry_flag )
		element.sorry_flag = 0;
	if( inelement.sorry_endpoint ==
			boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "255.255.255.255" ), (0)) ){
		element.sorry_endpoint			= boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) );
		element.sorry_maxconnection	= 0LL;
		element.sorry_flag			= false;
	}
};

/*!
 * load_parameter method call.(get parameter file's value)
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::load_parameter( l7vs::error_code& err ){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "in_function : void virtualservice_base::load_parameter()", __FILE__, __LINE__ );
	}

	l7vs::error_code	vs_err;
	Parameter			param;
	int					int_val;
	std::string			str_val;
	//get realserver_side networkI/F card name
	str_val	= param.get_string( l7vs::PARAM_COMP_VIRTUALSERVICE, PARAM_RS_SIDE_NIC_NAME, vs_err );
	if( !vs_err )
		param_data.nic_realserver_side	= str_val;
	else{
		err.setter( true, "nic_realserver_side parameter does not exist." );
		return;
	}
	//get session pool size value
	int_val	= param.get_int( l7vs::PARAM_COMP_VIRTUALSERVICE, PARAM_POOLSIZE_KEY_NAME, vs_err );
	if( !vs_err )
		param_data.session_pool_size = int_val;
	//get bps calc interval
	int_val	= param.get_int( l7vs::PARAM_COMP_VIRTUALSERVICE, PARAM_BPS_CALC_INTERVAL, vs_err );
	if( !vs_err )
		param_data.bps_interval = int_val;
	//get task schedule algorithm
	int_val	= param.get_int( l7vs::PARAM_COMP_VIRTUALSERVICE, PARAM_SCHED_ALGORITHM, vs_err );
	if( !vs_err ){
		if( (SCHED_FIFO == int_val) || (SCHED_RR == int_val) || (SCHED_OTHER == int_val) || (SCHED_BATCH == int_val) )
			param_data.schedule_algorithm = int_val;
	}
	//get replication interval
	int_val	= param.get_int( l7vs::PARAM_COMP_REPLICATION, PARAM_REP_INTERVAL, vs_err );
	if( !vs_err )
		param_data.rep_interval = int_val;

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_base::load_parameter()", __FILE__, __LINE__ );
	}
}

/*!
 * handle_protomod_replication method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_protomod_replication( const boost::system::error_code& err ){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format formatter("in_function : void virtualservice_base::handle_protomod_replication( "
								"const boost::system::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		if( NULL != protomod ){
			protomod->replication_interrupt();
			//register handle_protomod_replication
			protomod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
			protomod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_protomod_replication, 
													this, boost::asio::placeholders::error ) );
		}else{
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, PROTOMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_base::handle_protomod_replication( const boost::system::error_code& err )", __FILE__, __LINE__ );
	}
}

/*!
 * handle_schedmod_replication method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_schedmod_replication( const boost::system::error_code& err ){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format formatter("in_function : void virtualservice_base::handle_schedmod_replication( "
								"const boost::system::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		if( NULL != schedmod ){
			schedmod->replication_interrupt();
			//register handle_schedmod_replication
			schedmod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
			schedmod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_schedmod_replication, 
													this, boost::asio::placeholders::error ) );
		}else{
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, SCHEDMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_base::handle_schedmod_replication( const boost::system::error_code& err )", __FILE__, __LINE__ );
	}
}

/*!
 * handle_throughput_update method call.(deadline_timer event)
 *
 * @param   error_code
 * @return  void
 */
void	l7vs::virtualservice_base::handle_throughput_update( const boost::system::error_code& err ){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format formatter("in_function : void virtualservice_base::handle_throughput_update( "
								"const boost::system::error_code& err ) : err = %s, err.message = %s");
		formatter % ( err ? "true" : "false") % err.message();
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		// decrease wait count
		{
			rw_scoped_lock				wait_up_lock( wait_count_up_mutex );
			if( 0 != wait_count_up )	wait_count_up -= 1;
		}
		{
			rw_scoped_lock				wait_down_lock( wait_count_down_mutex );
			if( 0 != wait_count_down )	wait_count_down -= 1;
		}

		if( 0 != current_up_recvsize ){
			rw_scoped_lock				throughput_up_lock( throughput_up_mutex );
			rd_scoped_lock				recvsize_up_lock( recvsize_up_mutex );
			rw_scoped_lock				wait_up_lock( wait_count_up_mutex );

			if( 0 != element.qos_upstream ){
				// wait_count = ( current_throughput(byte/s)(=(recvsize / interval(ms)) * 1000 ) / qos_limit_throughput ) - 1(thistime)
				wait_count_up = ( ( ( current_up_recvsize / param_data.bps_interval ) * 1000 ) / element.qos_upstream );
				if( wait_count_up > 0 )	wait_count_up -= 1;
			}
			// throughput = recvsize / ( (thistime(1) + all_wait_time) * interval(ms) ) * 1000
			throughput_up = ( current_up_recvsize / ( param_data.bps_interval * ( wait_count_up + 1 ) ) ) * 1000;
			current_up_recvsize = 0ULL;
		}

		if( 0 != current_down_recvsize ){
			rw_scoped_lock				throughput_down_lock( throughput_down_mutex );
			rd_scoped_lock				recvsize_down_lock( recvsize_down_mutex );
			rw_scoped_lock				wait_down_lock( wait_count_down_mutex );

			if( 0 != element.qos_downstream ){
				// wait_count = ( current_throughput(byte/s)(=(recvsize / interval(ms)) * 1000 ) / qos_limit_throughput ) - 1(thistime)
				wait_count_down = ( ( ( current_down_recvsize / param_data.bps_interval ) * 1000 ) / element.qos_downstream );
				if( wait_count_down > 0 )	wait_count_down -= 1;
			}
			// throughput = recvsize / ( (thistime(1) + all_wait_time) * interval(ms) ) * 1000
			throughput_down = ( current_down_recvsize / ( param_data.bps_interval * ( wait_count_down + 1 ) ) ) * 1000;
			current_down_recvsize = 0ULL;
		}

		//register timer event
		calc_bps_timer->expires_from_now( boost::posix_time::milliseconds( param_data.bps_interval ) );
		calc_bps_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_throughput_update, 
												this, boost::asio::placeholders::error ) );
		if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
			boost::format formatter1("throughput(upstream) dump   : %d");
			formatter1 % throughput_up;
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter1.str(), __FILE__, __LINE__ );
			boost::format formatter2("throughput(downstream) dump : %d");
			formatter2 % throughput_down;
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter2.str(), __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format formatter("out_function : void virtualservice_base::handle_throughput_update( "
								"const boost::system::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, formatter.str(), __FILE__, __LINE__ );
	}
}

//!	@brief	get machine using nics
//!	@param[out]	nic_vec
//!	@return		void
void	l7vs::virtualservice_base::get_nic_list( std::vector< std::string >& nic_vec ){
	using namespace boost;
	nic_vec.clear();
	std::string	buff;
	std::ifstream	ifs( "/proc/net/dev" );
	if( ifs )	ifs.seekg( 200 );
	else	return;
	while( std::getline( ifs, buff ) ){
		std::string nic = buff.substr( 0, 6 );
		algorithm::trim( nic );
		nic_vec.push_back( nic );
	}	
}

//! @brief	get cpusetmask from NIC.
//!	@param[in]	NIC name
//!	@return		cpu_set_t
cpu_set_t	l7vs::virtualservice_base::get_cpu_mask( std::string	nic_name ){
	using namespace std;
	using namespace boost;

	vector< string > 		split_vec;
	map< size_t, string >	cpu_nic_map;
	string					buff;
	ifstream				ifs( "/proc/interrupts" );
	unsigned int			target_interrupt;
	size_t					target_cpuid = 0;
	cpu_set_t				mask;
	sched_getaffinity( 0, sizeof( cpu_set_t ), &mask );

	//read cpu lists.
	if( !ifs ) return mask;
	getline( ifs, buff );
	algorithm::split( split_vec, buff, algorithm::is_any_of( " " ) );
	for( vector< string >::iterator itr = split_vec.begin();
		 itr != split_vec.end();
		 ++itr ){
		algorithm::trim( *itr );
		if( itr->size() ) cpu_nic_map.insert( make_pair( cpu_nic_map.size(), "" ) );
	}
	// read interrupts.
	while( getline( ifs,  buff ) ){
		if( string::npos == buff.find( nic_name ) ) continue;
		//割り込みIDを取得
		algorithm::split( split_vec, buff, algorithm::is_any_of( ":" ));
		if( !split_vec.size() ) return mask;	//interrupt分割不可
		algorithm::trim( split_vec[0] );
		target_interrupt = lexical_cast<unsigned int>( split_vec[0] );
		for( size_t i = 0; i < cpu_nic_map.size(); ++i ){
			size_t	start_position = 4 + ( i * 11 );
			size_t	end_position = 11;
			map< size_t, string>::iterator itr = cpu_nic_map.find( i );
			if( itr == cpu_nic_map.end() ) return mask;
			itr->second = buff.substr( start_position, end_position );
			algorithm::trim( itr->second );
		}
		unsigned long long max_events = 0;
		for( map< size_t, string >::iterator itr = cpu_nic_map.begin();
			 itr != cpu_nic_map.end();
			 ++itr ){
			if( lexical_cast< unsigned long long >( itr->second ) > max_events ){
				target_cpuid = itr->first;
				max_events = lexical_cast< unsigned long long >( itr->second );
			}		 
		}
		break;
	}
	CPU_ZERO( &mask );
	CPU_SET( static_cast<int>( target_cpuid ), &mask );
	return mask;
}

//! @brief get cpumask from address name.
//! @brief set current cpumask which address is not found
//! @param[in] address object
//! @return cpu_set_t cpu mask set.
cpu_set_t	l7vs::virtualservice_base::get_cpu_mask( boost::asio::ip::address& address ){
	using namespace boost;
	cpu_set_t	mask;

	std::vector< std::string > nic_vector;
	get_nic_list( nic_vector );
	for( std::vector< std::string >::iterator itr = nic_vector.begin();
		 itr != nic_vector.end();
		 ++itr ){
		struct ifreq	ifr;
		memset( &ifr, 0, sizeof( struct ifreq ) );
		if( address.is_v4() )	ifr.ifr_addr.sa_family = AF_INET;
		else					ifr.ifr_addr.sa_family = AF_INET6;
		strncpy( ifr.ifr_name, itr->c_str(), IFNAMSIZ -1 );
		int fd = socket( AF_INET, SOCK_STREAM, 0 );
		if( ioctl( fd, SIOCGIFADDR, &ifr ) != 0 ){
			close( fd );
			continue;
		}
		struct sockaddr_in* ifaddr = reinterpret_cast<struct sockaddr_in*>( &( ifr.ifr_addr ) );
		asio::ip::address	add = asio::ip::address::from_string( inet_ntoa( ifaddr->sin_addr ) );
		if( add == address ){
			mask = get_cpu_mask( *itr );
			close( fd );
			return mask;
		}
		close( fd );
	}
	return mask;
}

/*!
 * method of rs_list reference count increase.
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::rs_list_lock(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "in_function : void virtualservice_base::rs_list_lock()", __FILE__, __LINE__ );
	}

	{
		rw_scoped_lock			refcnt_inc_lock( rs_list_ref_count_inc_mutex );
	}
	rw_scoped_lock	refcnt_lock( rs_list_ref_count_mutex );
	if( ULLONG_MAX > rs_list_ref_count )
		++rs_list_ref_count;
	else
		rs_list_ref_count = 0;

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_base::rs_list_lock()", __FILE__, __LINE__ );
	}
}

/*!
 * method of rs_list reference count decrease.
 *
 * @param   void
 * @return  void
 */
void	l7vs::virtualservice_base::rs_list_unlock(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "in_function : void virtualservice_base::rs_list_unlock()", __FILE__, __LINE__ );
	}

	rw_scoped_lock	refcnt_lock( rs_list_ref_count_mutex );
	if( 0 < rs_list_ref_count )
		--rs_list_ref_count;
	else
		rs_list_ref_count = ULLONG_MAX;

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, "out_function : void virtualservice_base::rs_list_unlock()", __FILE__, __LINE__ );
	}
}

/*!
 * get virtual service element.
 *
 * @param   void
 * @return  virtual service element
 */
l7vs::virtualservice_element&		l7vs::virtualservice_base::get_element(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in_function : l7vs::virtualservice_element& virtualservice_base::get_element()", __FILE__, __LINE__ );
	}

	boost::mutex::scoped_lock lk( element_mutex );
	//update element
	rs_list_lock();
	element.realserver_vector.clear();
	for( std::list<realserver>::iterator itr = rs_list.begin();
		 itr != rs_list.end(); ++itr ){
		l7vs::realserver_element	rs_element;
		rs_element.set_active( itr->get_active() );
		rs_element.set_inact( itr->get_inact() );
		rs_element.tcp_endpoint	= itr->tcp_endpoint;
		rs_element.udp_endpoint	= itr->udp_endpoint;
		rs_element.weight		= itr->weight;
		element.realserver_vector.push_back( rs_element );
	}
	rs_list_unlock();
	rd_scoped_lock				up_lock( throughput_up_mutex );
	element.throughput_upstream		= throughput_up;
	rd_scoped_lock				down_lock( throughput_down_mutex );
	element.throughput_downstream	= throughput_down;

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format element_dump( "element struct data, "
								 "udpmode = %s, tcp_accept_endpoint = %s, "
								 "udp_recv_endpoint = %s, "
								 "realserver_vector:size = %d, "
								 "protocol_module_name = %s, schedule_module_name = %s, "
								 "protocol_args.size = %d, sorry_maxconnection = %d, "
								 "sorry_endpoint = %s, sorry_flag = %d, qos_upstream = %d"
								 "qos_downstream = %d, throughput_upstream = %d, throughput_downstream = %d" );
		element_dump % ( ( element.udpmode == 0 ) ? "TCP" : "UDP" ) % element.tcp_accept_endpoint \
						% element.udp_recv_endpoint % element.realserver_vector.size() % element.protocol_module_name \
						% element.schedule_module_name % element.protocol_args.size() % element.sorry_maxconnection \
						% element.sorry_endpoint % element.sorry_flag % element.qos_upstream % element.qos_downstream \
						% element.throughput_upstream % element.throughput_downstream;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, element_dump.str(), __FILE__, __LINE__ );
	}
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : l7vs::virtualservice_element& virtualservice_base::get_element()", __FILE__, __LINE__ );
	}
	return element;
}

/*!
 * get upstream QoS threashold value.
 *
 * @param   void
 * @return  upstream QoS threashold value
 */
unsigned long long	l7vs::virtualservice_base::get_qos_upstream(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		if( boost::this_thread::get_id() == this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_qos_upstream() : ret = %d" );
			fmt % element.qos_upstream;
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		if( boost::this_thread::get_id() != this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_qos_upstream() : ret = %d" );
			fmt % element.qos_upstream;
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	return element.qos_upstream;
}

/*!
 * get downstream QoS threashold value.
 *
 * @param   void
 * @return  downstream QoS threashold value
 */
unsigned long long	l7vs::virtualservice_base::get_qos_downstream(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		if( boost::this_thread::get_id() == this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_qos_downstream() : ret = %d" );
			fmt % element.qos_downstream;
			Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		if( boost::this_thread::get_id() != this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_qos_downstream() : ret = %d" );
			fmt % element.qos_downstream;
			Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	return element.qos_downstream;
}

/*!
 * get upstream bit/sec value.
 *
 * @param   void
 * @return  upstream throughput[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_throughput_upstream(){
	rd_scoped_lock	lock( throughput_up_mutex );

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		if( boost::this_thread::get_id() == this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_throughput_upstream() : ret = %d" );
			fmt % throughput_up;
			Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		if( boost::this_thread::get_id() != this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_throughput_upstream() : ret = %d" );
			fmt % throughput_up;
			Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}

	return throughput_up;
}

/*!
 * get downstream bit/sec value.
 *
 * @param   void
 * @return  downstream throughput[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_throughput_downstream(){
	rd_scoped_lock	lock( throughput_down_mutex );

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		if( boost::this_thread::get_id() == this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_throughput_downstream() : ret = %d" );
			fmt % throughput_down;
			Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		if( boost::this_thread::get_id() != this_id ){
			boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_throughput_downstream() : ret = %d" );
			fmt % throughput_down;
			Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
		}
	}

	return throughput_down;
}

/*!
 * get upstream wait_count value.
 *
 * @param   void
 * @return  upstream wait_count value
 */
unsigned long long	l7vs::virtualservice_base::get_wait_upstream(){
	rd_scoped_lock	lock( wait_count_up_mutex );
	if(	unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_wait_upstream() : ret = %d" );
		fmt % wait_count_up;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return wait_count_up;
}

/*!
 * get downstream wait_count value.
 *
 * @param   void
 * @return  downstream wait_count value
 */
unsigned long long	l7vs::virtualservice_base::get_wait_downstream(){
	rd_scoped_lock	lock( wait_count_down_mutex );
	if(	unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_wait_downstream() : ret = %d" );
		fmt % wait_count_down;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return wait_count_down;
}

/*!
 * get upstream total receive size bit/sec value.
 *
 * @param   void
 * @return  upstream receive size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_up_recv_size(){
	rd_scoped_lock	lock( recvsize_up_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_up_recv_size() : ret = %d" );
		fmt % recvsize_up;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return recvsize_up;
}

/*!
 * get upstream total send size bit/sec value.
 *
 * @param   void
 * @return  upstream send size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_up_send_size(){
	rd_scoped_lock	lock( sendsize_up_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_up_send_size() : ret = %d" );
		fmt % sendsize_up;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return sendsize_up;
}

/*!
 * get downstream total receive size bit/sec value.
 *
 * @param   void
 * @return  downstream receive size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_down_recv_size(){
	rd_scoped_lock	lock( recvsize_down_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_down_recv_size() : ret = %d" );
		fmt % recvsize_down;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return recvsize_down;
}

/*!
 * get downstream total send size bit/sec value.
 *
 * @param   void
 * @return  downstream send size[bit/sec] value
 */
unsigned long long	l7vs::virtualservice_base::get_down_send_size(){
	rd_scoped_lock	lock( sendsize_down_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in/out_function : unsigned long long virtualservice_base::get_down_send_size() : ret = %d" );
		fmt % sendsize_down;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}
	return sendsize_down;
}


/*!
 * update upstream receive data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_up_recv_size( unsigned long long	datasize ){
	rw_scoped_lock	lock( recvsize_up_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in_function : void virtualservice_base::update_up_recv_size("
							 "unsigned long long datasize) : datasize = %d" );
		fmt % datasize;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	if( (ULLONG_MAX - current_up_recvsize) < datasize )
		current_up_recvsize	= ULLONG_MAX;
	else
		current_up_recvsize	= current_up_recvsize + datasize;

	if( (ULLONG_MAX - recvsize_up) < datasize )
		recvsize_up = datasize - (ULLONG_MAX - recvsize_up);
	else
		recvsize_up = recvsize_up + datasize;

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "update members : current_up_recvsize = %d, recvsize_up = %d" );
		fmt % current_up_recvsize % recvsize_up;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_base::update_up_recv_size(unsigned long long datasize)", __FILE__, __LINE__ );
	}
}

/*!
 * update upstream send data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_up_send_size( unsigned long long	datasize ){
	rw_scoped_lock	lock( sendsize_up_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in_function : void virtualservice_base::update_up_send_size("
							 "unsigned long long datasize) : datasize = %d" );
		fmt % datasize;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	if( (ULLONG_MAX - sendsize_up) < datasize )
		sendsize_up = datasize - ( ULLONG_MAX - sendsize_up );
	else
		sendsize_up = sendsize_up + datasize;

	if( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ){
		boost::format	fmt( "update members : sendsize_up = %d" );
		fmt % sendsize_up;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_base::update_up_send_size(unsigned long long datasize)", __FILE__, __LINE__ );
	}
}

/*!
 * update downstream receive data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_down_recv_size( unsigned long long	datasize ){
	rw_scoped_lock	lock( recvsize_down_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in_function : void virtualservice_base::update_down_recv_size("
							 "unsigned long long datasize) : datasize = %d" );
		fmt % datasize;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	if( (ULLONG_MAX - current_down_recvsize) < datasize )
		current_down_recvsize = ULLONG_MAX;
	else
		current_down_recvsize = current_down_recvsize + datasize;

	if( (ULLONG_MAX - recvsize_down) < datasize )
		recvsize_down = datasize - ( ULLONG_MAX - recvsize_down );
	else
		recvsize_down = recvsize_down + datasize;

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "update members : current_down_recvsize = %d, recvsize_down = %d" );
		fmt % current_down_recvsize % recvsize_down;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_base::update_down_recv_size(unsigned long long datasize)", __FILE__, __LINE__ );
	}
}

/*!
 * update downstream send data size
 *
 * @param   datasize
 * @return  void
 */
void	l7vs::virtualservice_base::update_down_send_size( unsigned long long	datasize ){
	rw_scoped_lock	lock( sendsize_down_mutex );
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "in_function : void virtualservice_base::update_down_send_size("
							 "unsigned long long datasize) : datasize = %d" );
		fmt % datasize;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
	}

	if( (ULLONG_MAX - sendsize_down) < datasize )
		sendsize_down = datasize - ( ULLONG_MAX - sendsize_down );
	else
		sendsize_down = sendsize_down + datasize;

	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format	fmt( "update members : sendsize_down = %d" );
		fmt % sendsize_down;
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, fmt.str(), __FILE__, __LINE__ );
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "out_function : void virtualservice_base::update_down_send_size(unsigned long long datasize)", __FILE__, __LINE__ );
	}
}
	
/*!
 * get protocol module pointer(shared_ptr)
 *
 * @param   void
 * @return  protocol module pointer(shared_ptr)
 */
l7vs::protocol_module_base*		l7vs::virtualservice_base::get_protocol_module(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in/out_function : l7vs::protocol_module_base* virtualservice_base::get_schedule_module()", __FILE__, __LINE__ );
	}
	return protomod;
}

/*!
 * get schedule module pointer(shared_ptr)
 *
 * @param   void
 * @return  schedule module pointer(shared_ptr)
 */
l7vs::schedule_module_base*		l7vs::virtualservice_base::get_schedule_module(){
	if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 0, "in/out_function : l7vs::schedule_module_base* virtualservice_base::get_schedule_module()", __FILE__, __LINE__ );
	}
	return schedmod;
}
