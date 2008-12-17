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
protected:
	boost::function< void ( const LOG_LEVEL_TAG, const std::string) >	logger;
	boost::function< void ( const std::string&, unsigned int* ) > replication_pay_memory;
public:
	schedule_module_base(	boost::function< void ( const LOG_LEVEL_TAG, const std::string ) > inlog,
							boost::function< void ( std::string&, unsigned int* ) >  inreplication_pay_memory
							) : logger( inlog ),
								replication_pay_memory( inreplication_pay_memory ) = 0;
	virtual	~schedule_module_base() = 0;

	virtual	boost::function<void(void)>	table_lock;
	virtual	boost::function<void(void)>	table_unlock;

	virtual	boost::asio::ip::basic_endpoint&	handle_schedule(
									boost::thread::id	thread_id,
									boost::function< std::list<realserver>::iterator (void)> inlist_begin,
									boost::function< std::list<realserver>::iterator (void)> inlist_end,
									boost::function< std::list<realserver>::iterator (void)> inlist next ) = 0;

	virtual	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id ) = 0;

	virtual	handle_session_finalize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id ) = 0;
};

};	//namespace l7vsd

#endif //SCHEDULE_MODULE_BASE_H
