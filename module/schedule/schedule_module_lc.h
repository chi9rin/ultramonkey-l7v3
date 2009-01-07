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
	schedule_module_least_connection() {};
	~schedule_module_least_connection() {};

	void	initialize() {};

	void	init_logger_functions(
							getloglevel_func_type	ingetloglevel,
							logger_func_type		inputLogFatal,
							logger_func_type		inputLogError,
							logger_func_type		inputLogWarn,
							logger_func_type		inputLogInfo,
							logger_func_type		inputLogDebug ) :
							getloglevel( ingetloglevel ),
							putLogFatal( inputLogFatal ),
							putLogError( inputLogError ),
							putLogWarn( inputLogWarn ),
							putLogInfo( inputLogInfo ),
							putLogDebug( inputLogDebug ) {};

	void	init_replication_functions(
							replicationpaymemory_func_type  inreplication_pay_memory,
							boost::function< void( void ) > inlock_func,
							boost::function< void( void ) > inunlock_func ) :
							replication_pay_memory( inreplication_pay_memory ),
							replication_area_lock( inlock_func ),
							replication_area_unlock( inunlock_func ) {};

	void	handle_schedule(
							boost::thread::id		thread_id,
							rslist_iterator_type	inlist_begin,
							rslist_iterator_type	inlist_end,
							rslist_iterator_type	inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint );

};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_LC_H
