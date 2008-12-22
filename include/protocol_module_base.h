//
//	@file	protocol_module_base.h
//	@brief	shared object protocol module absctract class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	PROTOCOL_MODULE_BASE_H
#define	PROTOCOL_MODULE_BASE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "logger_enum.h"
#include "module_base.h"
#include "realserver.h"

namespace l7vsd
{

class protocol_module_base : public module_base{
public:
	typedef	list<realserver>	realserverlist_type;
	typedef	boost::function< realserverlist_type::iretarot( void ) > rs_list_itr_func_type;

	enum	EVENT_TAG
	{
		//use in upstream_thread
		CLIENT_RECV = 0,
		REALSERVER_SELECT,
		REALSERVER_CONNECT,
		SORRYSERVER_SELECT,
		SORRYSERVER_CONNECT,
		//use in downstream_thread
		REALSERVER_RECV,
		SORRYSERVER_RECV,
		CLIENT_CONNECTION_CHECK,
		CLIENT_SEND,
		//COMMON Status
		CLIENT_RESPOND_SEND,
		REALSERVER_DISCONNECT,
		SORRYSERVER_DISCONNECT,
		CLIENT_DISCONNECT,
		FINALIZE,
		STOP
	};

	//this class is POD
	struct check_message_result
	{
		bool		flag;
		std::string	message;
		bool		operator==( const check_message& in ){ return flag == in.flag; }
		bool		operator!=( const check_message& in ){ return flag != in.flag; }
	};
protected:
	rs_list_itr_func_type	rs_list_begin;
	rs_list_itr_func_type	rs_list_end;
	rs_list_itr_func_type	rs_list_next;

	boost::function< void ( const LOG_LEVEL_TAG, const std::string) >	logger;
	boost::function< void ( const std::string&, unsigned int* ) > replication_pay_memory;

	//scheduler_method
	boost::function< boost::asio::ip::basic_endpoint&(	const boost::thread::id,
														const boost::thread::id,
														rs_list_itr_func_type,
														rs_list_itr_func_type,
														rs_list_itr_func_type ) >	schedule;

	virtual	boost::function< void ( void ) > rs_list_lock;
	virtual	boost::function< void ( void ) > rs_list_unlock;

	boost::function< void( void ) >	replication_area_lock;
	boost::function< void( void ) >	replication_area_unlock;
public:

	protocol_module_base(
							boost::function< void ( const LOG_LEVEL_TAG, const std::string ) > inlog,
						) : logger( inlog ) = 0;
							
	virtual ~protocol_module_base() = 0;

	virtual	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock
							boost::function< void ( std::string&, unsigned int* ) >  inreplication_pay_memory
						) = 0;

	virtual	finalize();

	// event function
	virtual	bool	is_use_sorry() = 0;
	virtual	check_message_result&	check_parameter( const std::vector<std::string>& args ) const = 0;

	virtual	void	handle_rslist_update() = 0;

	virtual	check_message_result	set_parameter( const std::vector<std::string>& args ) = 0;
	virtual	check_message_result	add_parameter( const std::vector<std::string>& args ) = 0;

	virtual	void	register_schedule(
									boost::function< boost::asio::ip::basic_endpoint&(
																const boost::thread::id,
																const boost::thread::id,
																rs_list_itr_func_type,
																rs_list_itr_func_type,
																rs_list_itr_func_type ) > inschedule 
											) = 0;

	virtual	void	register_replication_area_lock(
									boost::function< void( void ) > inlock_func ) = 0;

	virtual	void	register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func ) = 0;

	//use in upstream_thread
	virtual	EVENT_TAG	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id ) = 0;
	virtual	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id ) = 0;
	virtual	EVENT_TAG	handle_accept(
									const boost::thread::id thread_id ) = 0;

	virtual	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen ) = 0;

	virtual	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;

	virtual	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BFFER_SIZE>& sendbuffer ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_send(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::basic_endpoint & sorry_endpoint ) = 0;

	virtual	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer ) = 0;

	virtual	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_send(
									const boost::thread::id thread_id ) = 0;

	//use in downstream_thread
	virtual	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen ) = 0;
	
	virtual EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint & recv_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer ) = 0;

	virtual	EVENT_TAG	handle_client_send(
									const boost::thread::id thread_id ) = 0;

	//use in upstream/downstream thread
	virtual	EVENT_TAG	handle_client_disconnect(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id ) = 0;

	virtual	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id ) = 0;

	virtual	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::basic_endpoint & sorry_endpoint ) = 0;

//	virtual	EVENT_TAG	handle_event( const pthread_t thread_id, EVENT_TAG prev_status,  ) = 0;
};

}; // namespace l7vsd

#endif	//PROTOCOL_MODULE_BASE_H
