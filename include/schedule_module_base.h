//
//!	@file	schedule_module_base.h
//!	@brief	shared object schedule module abstract class
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

//
//! @class schedule_module_base
//!	@brief	schedule module abstract base class.
//! @brief	all schedule module is extened this class.
class schedule_module_base : public module_base{
public:
	//! logger function type
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const unsigned int, const std::string) >
									logger_func_type;
	//! replication_pay_memory function type
	typedef	boost::function< void ( const std::string&, unsigned int* ) >
									replicationpaymemory_func_type;
	//! realserver list iterator type
	typedef	boost::function< std::list<realserver>::iterator (void)>
									rslist_iterator_func_type;
public:
	//!	constractor
	schedule_module_base( std::string in_modulename ) : module_base( in_modulename ) {};
	//! destractor
	virtual	~schedule_module_base(){};
	//!	initialize function
	virtual	void	initialize() = 0;
	//! handle schedule called then schedule function for TCP/IP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled TCP/IP endpoint
	virtual	void	handle_schedule(
										boost::thread::id,
										rslist_iterator_func_type,
										rslist_iterator_func_type,
										rslist_iterator_func_type,
										boost::asio::ip::tcp::endpoint&) = 0;

	//! handle schedule calles then schedule function for UDP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled UDP endpoint
	virtual	void	handle_schedule(
										boost::thread::id,
										rslist_iterator_func_type,
										rslist_iterator_func_type,
										rslist_iterator_func_type,
										boost::asio::ip::udp::endpoint&) = 0;

};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_BASE_H
