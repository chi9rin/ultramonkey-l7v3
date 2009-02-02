//
//	@file	schedule_module_wrr.cpp
//	@brief	shared object schedule module class
//
//	copyright (c) xxx corporation. 2009
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include "schedule_module_wrr.h"

namespace l7vs{

//!	constractor
schedule_module_weighted_round_robin::schedule_module_weighted_round_robin() : schedule_module_base( "schedule weighted round robin" ){
}

//! destractor
schedule_module_weighted_round_robin::~schedule_module_weighted_round_robin(){}

//!	initialize function
void	schedule_module_weighted_round_robin::initialize(){
	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	boost::asio::ip::udp::endpoint	udp_local_endpoint ;

	tcp_endpoint = tcp_local_endpoint ;
	udp_endpoint = udp_local_endpoint ;

	vs_weights.currentWeight = 0;
	vs_weights.maxWeight = 0;
	vs_weights.gcd = 0;

	if ( !putLogInfo.empty() )
	{
		putLogInfo( 1, "Saved endpoint, weight and gcd were initialized.", __FILE__, __LINE__);
	}
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	schedule_module_weighted_round_robin::is_tcp(){ return true; }

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool	schedule_module_weighted_round_robin::is_udp(){ return true; }

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled TCP/IP endpoint
void	schedule_module_weighted_round_robin::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint ){
	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	rslist_type::iterator			itr;

	//! set clear data as NULL
	outendpoint = tcp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() || inlist_next.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			putLogFatal( 1, "Iterator function is empty.", __FILE__, __LINE__);
		}
		return;
	}

	if ( -1 == sched_wrr_service_init ( inlist_begin, inlist_end, inlist_next ) ){
		//! init error( no data )
		if ( !putLogError.empty() )
		{
			putLogError( 1, "There is no realserver on list.", __FILE__, __LINE__);
		}
		return;
	}

	//! first time
	if ( tcp_local_endpoint == tcp_endpoint ){
		vs_weights.currentWeight = vs_weights.maxWeight;

		itr = inlist_begin();
	} else {
		if ( vs_weights.currentWeight > vs_weights.maxWeight ) {
			vs_weights.currentWeight = vs_weights.maxWeight;
		}

		for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
			if ( itr->weight > 0 ){
				//! prev endpoint
				if ( tcp_endpoint == itr->tcp_endpoint ){
					itr = inlist_next( itr );
					break;
				}
			}
		}
		if ( itr == inlist_end() ){
			itr = inlist_begin();
		}
	}

	for ( ; ; ){
		for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
			if ( itr->weight >= vs_weights.currentWeight ){
				//! set found data
				tcp_endpoint = outendpoint = itr->tcp_endpoint;
				return ;
			}
		}
		if ( itr == inlist_end() ){
			itr = inlist_begin();
			vs_weights.currentWeight -= vs_weights.gcd;
			if ( vs_weights.currentWeight <= 0 ) {
				vs_weights.currentWeight = vs_weights.maxWeight;
			}
		}
	}
}

//! handle schedule calles then schedule function for UDP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled UDP endpoint
void	schedule_module_weighted_round_robin::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&		outendpoint ){
	boost::asio::ip::udp::endpoint	udp_local_endpoint ;
	rslist_type::iterator			itr;

	//! set clear data as NULL
	outendpoint = udp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() || inlist_next.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			putLogFatal( 1, "Iterator function is empty.", __FILE__, __LINE__);
		}
		return;
	}

	if ( -1 == sched_wrr_service_init ( inlist_begin, inlist_end, inlist_next ) ){
		//! init error( no data )
		if ( !putLogError.empty() )
		{
			putLogError( 1, "There is no realserver on list.", __FILE__, __LINE__);
		}
		return;
	}

	//! first time
	if ( udp_local_endpoint == udp_endpoint ){
		vs_weights.currentWeight = vs_weights.maxWeight;

		itr = inlist_begin();
	} else {
		if ( vs_weights.currentWeight > vs_weights.maxWeight ) {
			vs_weights.currentWeight = vs_weights.maxWeight;
		}

		for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
			if ( itr->weight > 0 ){
				//! prev endpoint
				if ( udp_endpoint == itr->udp_endpoint ){
					itr = inlist_next( itr );
					break;
				}
			}
		}
		if ( itr == inlist_end() ){
			itr = inlist_begin();
		}
	}

	for ( ; ; ){
		for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
			if ( itr->weight >= vs_weights.currentWeight ){
				//! set found data
				udp_endpoint = outendpoint = itr->udp_endpoint;
				return ;
			}
		}
		if ( itr == inlist_end() ){
			itr = inlist_begin();
			vs_weights.currentWeight -= vs_weights.gcd;
			if ( vs_weights.currentWeight <= 0 ) {
				vs_weights.currentWeight = vs_weights.maxWeight;
			}
		}
	}
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void	schedule_module_weighted_round_robin::replication_interrupt(){}


int		schedule_module_weighted_round_robin::sched_wrr_service_init(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){

	vs_weights.maxWeight = sched_wrr_getMaxWeight( inlist_begin, inlist_end, inlist_next );
	if ( vs_weights.maxWeight <= 0 ){
		return -1;
	}

	vs_weights.gcd = sched_wrr_getGCD( inlist_begin, inlist_end, inlist_next );
	if ( vs_weights.gcd <= 0 ){
		return -1;
	}
	return 0;
}


int		schedule_module_weighted_round_robin::sched_wrr_getMaxWeight(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
	int					weight = 0;
	rslist_type::iterator	itr;

	for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
		//! keep max weight of list
		if ( itr->weight > weight ){
			weight = itr->weight;
		}
	}

	return ( weight );
}

int		schedule_module_weighted_round_robin::sched_wrr_gcd( int a, int b ){
	if ( a == 0 || b == 0 ){
		return 0;
	}

	while ( a != b ){
		if ( a > b ){
			a -= b;
		} else {
			b -= a;
		}
	}

	return ( a );	
}

int		schedule_module_weighted_round_robin::sched_wrr_getGCD(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
	int					current_gcd = 1;
	rslist_type::iterator	itr;

	for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
		//! keep first data of list
		if ( itr->weight > 0 ){
			current_gcd = itr->weight;
			break;
		}
	}
	if ( itr == inlist_end() ){
		//! no data
		return -1;
	}

	for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
		if ( itr->weight > 0 ){
			current_gcd = sched_wrr_gcd( current_gcd, itr->weight );
		}
	}

	return ( current_gcd );
}

}	//namespace l7vs


extern "C" l7vs::schedule_module_base*
create_module(){
	return dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_weighted_round_robin());
}

extern "C" void
destroy_module( l7vs::schedule_module_base* in ){
	delete in;
}
