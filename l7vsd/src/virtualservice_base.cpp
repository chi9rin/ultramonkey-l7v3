/*!
 *	@file	virtualservice_base.cpp
 *	@brief	VirtualService base class implementations
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
													rep( inrep ) {
	rs_list_ref_count = 0;

	recvsize_up	= 0;
	current_up_recvsize	= 0;
	sendsize_up	= 0;
	recvsize_down	= 0;
	current_down_recvsize	= 0;
	sendsize_down	= 0;
	throughput_up	= 0;
	throughput_down	= 0;
	wait_count_up	= 0;
	wait_count_down	= 0;

	calc_bps_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	replication_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	protomod_rep_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	schedmod_rep_timer.reset( new boost::asio::deadline_timer( dispatcher ) );
	rs_list.clear();
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
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 1, "in_function : void virtualservice_base::load_parameter()", __FILE__, __LINE__ );
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
	//get replication interval
	int_val	= param.get_int( l7vs::PARAM_COMP_REPLICATION, PARAM_REP_INTERVAL, vs_err );
	if( !vs_err )
		param_data.rep_interval = int_val;

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 2, "out_function : void virtualservice_base::load_parameter()", __FILE__, __LINE__ );
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
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 3, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		if( likely( NULL != protomod ) ){
			protomod->replication_interrupt();
			//register handle_protomod_replication
			protomod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
			protomod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_protomod_replication, 
													this, boost::asio::placeholders::error ) );
		}else{
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 1, PROTOMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 2, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 4, "out_function : void virtualservice_base::handle_protomod_replication( const boost::system::error_code& err )", __FILE__, __LINE__ );
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
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 5, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		if( likely( NULL != schedmod ) ){
			schedmod->replication_interrupt();
			//register handle_schedmod_replication
			schedmod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
			schedmod_rep_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_schedmod_replication, 
													this, boost::asio::placeholders::error ) );
		}else{
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 3, SCHEDMOD_NOTLOAD_ERROR_MSG, __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 4, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 6, "out_function : void virtualservice_base::handle_schedmod_replication( const boost::system::error_code& err )", __FILE__, __LINE__ );
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
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 7, formatter.str(), __FILE__, __LINE__ );
	}
	if( likely( !err ) ){
		// decrease wait count
		if( wait_count_up != 0 )	wait_count_up--;
		if( wait_count_down != 0 )	wait_count_down--;

		if( current_up_recvsize != 0 ){
			if( 0 != element.qos_upstream ){
				// wait_count = ( current_throughput(byte/s)(=(recvsize / interval(ms)) * 1000 ) / qos_limit_throughput ) - 1(thistime)
				wait_count_up = ( ( ( current_up_recvsize.get() / param_data.bps_interval ) * 1000 ) / element.qos_upstream );
				if( wait_count_up > 0 )	wait_count_up--;
			}
			// throughput = recvsize / ( (thistime(1) + all_wait_time) * interval(ms) ) * 1000
			throughput_up = ( current_up_recvsize.get() / ( param_data.bps_interval * ( wait_count_up.get() + 1 ) ) ) * 1000;
			current_up_recvsize = 0ULL;
		}

		if( current_down_recvsize != 0 ){

			if( 0 != element.qos_downstream ){
				// wait_count = ( current_throughput(byte/s)(=(recvsize / interval(ms)) * 1000 ) / qos_limit_throughput ) - 1(thistime)
				wait_count_down = ( ( ( current_down_recvsize.get() / param_data.bps_interval ) * 1000 ) / element.qos_downstream );
				if( wait_count_down > 0 )	wait_count_down--;
			}
			// throughput = recvsize / ( (thistime(1) + all_wait_time) * interval(ms) ) * 1000
			throughput_down = ( current_down_recvsize.get() / ( param_data.bps_interval * ( wait_count_down.get() + 1 ) ) ) * 1000;
			current_down_recvsize = 0ULL;
		}

		//register timer event
		calc_bps_timer->expires_from_now( boost::posix_time::milliseconds( param_data.bps_interval ) );
		calc_bps_timer->async_wait( boost::bind( &l7vs::virtualservice_tcp::handle_throughput_update, 
												this, boost::asio::placeholders::error ) );
		if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
			boost::format formatter1("throughput(upstream) dump   : %d");
			formatter1 % throughput_up.get();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 8, formatter1.str(), __FILE__, __LINE__ );
			boost::format formatter2("throughput(downstream) dump : %d");
			formatter2 % throughput_down.get();
			l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 9, formatter2.str(), __FILE__, __LINE__ );
		}
	}else
		//ERROR case
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 5, err.message(), __FILE__, __LINE__ );

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD ) ) ){
		boost::format formatter("out_function : void virtualservice_base::handle_throughput_update( "
								"const boost::system::error_code& err )");
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD, 10, formatter.str(), __FILE__, __LINE__ );
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
 * get virtual service element.
 *
 * @param   void
 * @return  virtual service element
 */
l7vs::virtualservice_element&		l7vs::virtualservice_base::get_element(){
	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 16, "in_function : l7vs::virtualservice_element& virtualservice_base::get_element()", __FILE__, __LINE__ );
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
	element.throughput_upstream		= throughput_up.get();
	element.throughput_downstream	= throughput_down.get();

	if( unlikely( LOG_LV_DEBUG == l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
		boost::format element_dump( "element struct data, "
								 "udpmode = %s, tcp_accept_endpoint = %s, "
								 "udp_recv_endpoint = %s, "
								 "realserver_vector:size = %d, "
								 "protocol_module_name = %s, schedule_module_name = %s, "
								 "protocol_args.size = %d, sorry_maxconnection = %d, "
								 "sorry_endpoint = %s, sorry_flag = %d, qos_upstream = %d, "
								 "qos_downstream = %d, throughput_upstream = %d, throughput_downstream = %d, "
								 "ssl_flag = %d, ssl_conf_filename = %s" );
		element_dump % ( ( element.udpmode == 0 ) ? "TCP" : "UDP" ) % element.tcp_accept_endpoint \
						% element.udp_recv_endpoint % element.realserver_vector.size() % element.protocol_module_name \
						% element.schedule_module_name % element.protocol_args.size() % element.sorry_maxconnection \
						% element.sorry_endpoint % element.sorry_flag % element.qos_upstream % element.qos_downstream \
						% element.throughput_upstream % element.throughput_downstream \
						% element.ssl_flag % element.ssl_conf_filename;
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 17, element_dump.str(), __FILE__, __LINE__ );
		l7vs::Logger::putLogDebug( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 18, "out_function : l7vs::virtualservice_element& virtualservice_base::get_element()", __FILE__, __LINE__ );
	}
	return element;
}

