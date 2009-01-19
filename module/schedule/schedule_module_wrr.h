//
//	@file	schedule_module_wrr.h
//	@brief	shared object schedule module class
//
//	copyright (c) xxx corporation. 2008
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	SCHEDULE_MODULE_WRR_H
#define	SCHEDULE_MODULE_WRR_H

#include "schedule_module_base.h"

namespace l7vs{

class schedule_module_weighted_round_robin : public schedule_module_base
{
protected:
	boost::asio::ip::tcp::endpoint	tcp_endpoint ;
	boost::asio::ip::udp::endpoint	udp_endpoint ;

	struct wrr_weights {
	    int currentWeight;  /*! Current Weight */
    	int maxWeight;      /*! Max of Weight */
    	int gcd;            /*! GCD of RealServer list */
		wrr_weights() : currentWeight(0), maxWeight(0), gcd(0) {}
	} vs_weights;
public:
	//!	constractor
	schedule_module_weighted_round_robin();
	//! destractor
	~schedule_module_weighted_round_robin();

	//!	initialize function
	void	initialize();

	//! tcp protocol support check
	//! @return tcp support is true
	//! @return tcp not-support is false
	bool	is_tcp();
	//! udp protocol support check
	//! @return udp support is true
	//! @return udp not-support is false
	bool	is_udp();

	//! handle schedule called then schedule function for TCP/IP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled TCP/IP endpoint
	void	handle_schedule(
							boost::thread::id		thread_id,
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint );

	//! handle schedule calles then schedule function for UDP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled UDP endpoint
	void	handle_schedule(
							boost::thread::id		thread_id,
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next,
							boost::asio::ip::udp::endpoint&	outendpoint );

	//! replication interval interrrupt
	//! timer thread call this function. from virtualservice.
	void	replication_interrupt();

protected:
	int		sched_wrr_service_init(
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next );
	int		sched_wrr_getMaxWeight(
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next );
	int		sched_wrr_gcd( int a, int b );
	int		sched_wrr_getGCD(
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next );
	
};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_WRR_H
