//
//	@file	schedule_module_base.h
//	@brief	shared object schedule module abstract class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	SCHEDULE_MODULE_BASE_H
#define	SCHEDULE_MODULE_BASE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "logger_enum.h"
#include "module_base.h"
#include "realserver.h"

namespace l7vs{

class schedule_module_base
{
public:
	//logger function type
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const unsigned int, const std::string) >
									logger_func_type;
	//replication_pay_memory function type
	typedef	boost::function< void ( const std::string&, unsigned int* ) >
									replicationpaymemory_func_type;
	//
	typedef	boost::function< std::list<realserver>::iterator (void)>
									rslist_iterator_type;
protected:

	//! replication paymemory function object
	replicationpaymemory_func_type	replication_pay_memory;

	//! replication area lock function object
	boost::function< void( void ) >	replication_area_lock;
	//! replication area unlock function object
	boost::function< void( void ) >	replication_area_unlock;

public:
	schedule_module_base() : logger() {};
	virtual	~schedule_module_base(){};

	virtual	void	initialize();

	virtual	void	handle_schedule(
										boost::thread::id		thread_id,
										rslist_iterator_type	inlist_begin,
										rslist_iterator_type	inlist_end,
										rslist_iterator_type	inlist_next,
										boost::asio::ip::tcp::endpoint&	outendpoint ) = 0;

	virtual	void	handle_schedule(
										boost::thread::id		thread_id,
										rslist_iterator_type	inlist_begin,
										rslist_iterator_type	inlist_end,
										rslist_iterator_type	inlist_next,
										boost::asio::ip::udp::endpoint&	outendpoint ) = 0;

};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_BASE_H
