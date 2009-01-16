//
//	@file	schedule_module_wrr.h
//	@brief	shared object schedule module abstract class
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
	schedule_module_weighted_round_robin();
	~schedule_module_weighted_round_robin();

	void	initialize();

	void	handle_schedule(
							boost::thread::id		thread_id,
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint );

	void	handle_schedule(
							boost::thread::id		thread_id,
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next,
							boost::asio::ip::udp::endpoint&	outendpoint );

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
