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


namespace l7vsd{

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
	logger_func_type				logger;
	replicationpaymemory_func_type	replication_pay_memory;
public:
	schedule_module_base( logger_func_type inlog ) : logger( inlog ) = 0;
	virtual	~schedule_module_base() = 0;

	virtual	void	initialize( replicationpaymemory_func_type inpaymemory_func );

	virtual	boost::asio::ip::basic_endpoint&	handle_schedule(
										boost::thread::id		thread_id,
										rslist_iterator_type	inlist_begin,
										rslist_iterator_type	inlist_end,
										rslist_iterator_type	inlist next
									) = 0;

};

};	//namespace l7vsd

#endif //SCHEDULE_MODULE_BASE_H
