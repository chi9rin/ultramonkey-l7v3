/*
 *	@file	schedule_module_wrr.h
 *	@brief	shared object schedule module class
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
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint );

	//! handle schedule calles then schedule function for UDP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled UDP endpoint
	void	handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&		outendpoint );

	//! replication interval interrrupt
	//! timer thread call this function. from virtualservice.
	void	replication_interrupt();

protected:
	int		sched_wrr_service_init(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next );
	int		sched_wrr_getMaxWeight(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next );
	int		sched_wrr_gcd( int a, int b );
	int		sched_wrr_getGCD(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next );
};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_WRR_H
