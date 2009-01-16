//
//	@file	schedule_module_lc.h
//	@brief	shared object schedule module abstract class
//
//	copyright (c) xxx corporation. 2008
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	SCHEDULE_MODULE_LC_H
#define	SCHEDULE_MODULE_LC_H

#include "schedule_module_base.h"

namespace l7vs{

class schedule_module_least_connection : public schedule_module_base
{
public:
	schedule_module_least_connection();
	~schedule_module_least_connection();

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
							boost::asio::ip::udp::endpoint&	outendpoint ) {};

};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_LC_H
