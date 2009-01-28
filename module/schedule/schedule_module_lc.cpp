//
//	@file	schedule_module_lc.h
//	@brief	shared object schedule module class
//
//	copyright (c) xxx corporation. 2009
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include "schedule_module_lc.h"

namespace l7vs{

//!	constractor
schedule_module_least_connection::schedule_module_least_connection() : schedule_module_base( "schedule least connection" ){
}

//! destractor
schedule_module_least_connection::~schedule_module_least_connection(){}

//!	initialize function
void	schedule_module_least_connection::initialize(){
	if ( !putLogInfo.empty() )
	{
		std::string msg("function was initialized.");
		putLogInfo( 1, msg, __FILE__, __LINE__);
	}
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	schedule_module_least_connection::is_tcp(){ return true; }

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool	schedule_module_least_connection::is_udp(){ return false; }

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled TCP/IP endpoint
void	schedule_module_least_connection::handle_schedule(
							boost::thread::id				thread_id,
							rslist_iterator_func_type		inlist_begin,
							rslist_iterator_func_type		inlist_end,
							rslist_iterator_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint ){
	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	int							active = INT_MAX;
	rslist_type::iterator			itr;

	//! set clear data as NULL
	outendpoint = tcp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			std::string msg("iterator function is empty.");
			putLogFatal( 1, msg, __FILE__, __LINE__);
		}
		return;
	}

	for ( itr = inlist_begin(); itr != inlist_end(); itr++ ){
		if ( itr->weight > 0 && active > itr->get_active() ){
			//! set found data
			outendpoint = itr->tcp_endpoint;
			active = itr->get_active();
		}
	}

	if ( active == INT_MAX ){
		//! no data
		if ( !putLogError.empty() )
		{
			std::string msg("there is no realserver on list.");
			putLogError( 1, msg, __FILE__, __LINE__);
		}
		return;
	}
}

//! handle schedule calles then schedule function for UDP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled UDP endpoint
void	schedule_module_least_connection::handle_schedule(
							boost::thread::id				thread_id,
							rslist_iterator_func_type		inlist_begin,
							rslist_iterator_func_type		inlist_end,
							rslist_iterator_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&	outendpoint ){
	if ( !putLogWarn.empty() )
	{
		std::string msg("udp function is invalid.");
		putLogWarn( 1, msg, __FILE__, __LINE__);
	}
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void	schedule_module_least_connection::replication_interrupt(){}

}	//namespace l7vs

extern "C" l7vs::schedule_module_base*
create_module(){
	return dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_least_connection());
}

extern "C" void
destroy_module( l7vs::schedule_module_base* in ){
	delete in;
}
