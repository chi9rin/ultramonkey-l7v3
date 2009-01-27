//
//	@file	schedule_module_rr.cpp
//	@brief	shared object schedule module class
//
//	copyright (c) xxx corporation. 2009
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include "schedule_module_rr.h"

namespace l7vs{

//!	constractor
schedule_module_round_robin::schedule_module_round_robin() : schedule_module_base( "schedule round robin" ){
}

//! destractor
schedule_module_round_robin::~schedule_module_round_robin(){}

//!	initialize function
void	schedule_module_round_robin::initialize(){
	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	boost::asio::ip::udp::endpoint	udp_local_endpoint ;

	tcp_endpoint = tcp_local_endpoint ;
	udp_endpoint = udp_local_endpoint ;
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	schedule_module_round_robin::is_tcp(){ return true; }

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool	schedule_module_round_robin::is_udp(){ return true; }

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled TCP/IP endpoint
void	schedule_module_round_robin::handle_schedule(
							boost::thread::id				thread_id,
							rslist_iterator_func_type		inlist_begin,
							rslist_iterator_func_type		inlist_end,
							rslist_iterator_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint ){
	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	rslist_type::iterator itr;

	//! set clear data as NULL
	outendpoint = tcp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() ){
		//! invalid iterator function
		return;
	}

	for ( itr = inlist_begin(); itr != inlist_end(); itr++ ){
		//! keep first data of list
		if ( itr->weight > 0 ){
			outendpoint = itr->tcp_endpoint;
			break;
		}
	}
	if ( itr == inlist_end() ){
		//! no data
		return;
	}

	//! first time
	if ( tcp_local_endpoint == tcp_endpoint ){
		//! set first data
		tcp_endpoint = outendpoint;
		return;
	}

	for ( ; itr != inlist_end(); itr++ ){
		if ( itr->weight > 0 ){
			//! prev endpoint
			if ( tcp_endpoint == itr->tcp_endpoint ){
				itr++;
				break;
			}
		}
	}
	for ( ; itr != inlist_end(); itr++ ){
		if ( itr->weight > 0 ){
			//! set found data
			outendpoint = itr->tcp_endpoint;
			break ;
		}
	}

	//! set found or first data
	tcp_endpoint = outendpoint;
}

//! handle schedule calles then schedule function for UDP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled UDP endpoint
void	schedule_module_round_robin::handle_schedule(
							boost::thread::id				thread_id,
							rslist_iterator_func_type		inlist_begin,
							rslist_iterator_func_type		inlist_end,
							rslist_iterator_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&	outendpoint ){
	boost::asio::ip::udp::endpoint	udp_local_endpoint ;
	rslist_type::iterator itr;

	//! set clear data as NULL
	outendpoint = udp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() ){
		//! invalid iterator function
		return;
	}

	for ( itr = inlist_begin(); itr != inlist_end(); itr++ ){
		//! keep first data of list
		if ( itr->weight > 0 ){
			outendpoint = itr->udp_endpoint;
			break;
		}
	}
	if ( itr == inlist_end() ){
		//! no data
		return;
	}

	//! first time
	if ( udp_local_endpoint == udp_endpoint ){
		//! set first data
		udp_endpoint = outendpoint;
		return;
	}

	for ( ; itr != inlist_end(); itr++ ){
		if ( itr->weight > 0 ){
			//! prev endpoint
			if ( udp_endpoint == itr->udp_endpoint ){
				itr++;
				break;
			}
		}
	}
	for ( ; itr != inlist_end(); itr++ ){
		if ( itr->weight > 0 ){
			//! set found data
			outendpoint = itr->udp_endpoint;
			break ;
		}
	}

	//! set found or first data
	udp_endpoint = outendpoint;
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void	schedule_module_round_robin::replication_interrupt(){}

}	//namespace l7vs

extern "C" l7vs::schedule_module_base*
create_module(){
	return dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_round_robin());
}

extern "C" void
destroy_module( l7vs::schedule_module_base* in ){
	delete in;
}
