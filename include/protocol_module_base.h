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

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "logger_enum.h"
#include "module_base.h"
#include "realserver.h"

namespace l7vs{

class protocol_module_base : public module_base {
public:
	//
	typedef	std::list<realserver>	realserverlist_type;
	typedef	boost::function< realserverlist_type::iterator( void ) >
								rs_list_itr_func_type;
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const unsigned int, const std::string) >
								logger_func_type;
	typedef	boost::function< void ( const std::string&, unsigned int* ) >
								replicationpaymemory_func_type;
	typedef	boost::function< void (		const boost::thread::id,
										const boost::thread::id,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										boost::asio::ip::tcp::endpoint& ) >
								tcp_schedule_func_type;
	typedef	boost::function< void (		const boost::thread::id,
										const boost::thread::id,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										boost::asio::ip::udp::endpoint& ) >
								udp_schedule_func_type;

	enum	EVENT_TAG{
		//use in upstream_thread
		CLIENT_RECV = 0,			// !< Receive from Client
		REALSERVER_SELECT,			// !< Select RealServer
		REALSERVER_CONNECT,			// !< Connect to RealServer
		REALSERVER_SEND,			// !< Send message to RealServer
		SORRYSERVER_SELECT,			// !< Select SorryServer
		SORRYSERVER_CONNECT,		// !< Connect to SorryServer
		SORRYSERVER_SEND,			// !< Send message to SorryServer
		//use in downstream_thread
		REALSERVER_RECV,			// !< Receive from RealServer
		SORRYSERVER_RECV,			// !< Receive from SorryServer
		CLIENT_CONNECTION_CHECK,	// !< Check ClientConnection available 
		CLIENT_SEND,				// !< Send message to Client
		//COMMON Status
		CLIENT_RESPONSE_SEND,		// !< Send UML7response message to Client
		REALSERVER_DISCONNECT,		// !< Disconnect RealServerConnection
		SORRYSERVER_DISCONNECT,		// !< Disconnect SorryServerConnection
		CLIENT_DISCONNECT,			// !< Disconnect ClientConnection
		REALSERVER_CLOSE,			// !< Close socket(RealServer side)
		FINALIZE,					// !< Do finalize
		STOP						// !< Stop SessionThread
	};

	//this class is POD
	struct check_message_result{
		bool		flag;
		std::string	message;
		bool		operator==( const check_message_result& in )
								{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
		bool		operator!=( const check_message_result& in )
								{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
		check_message_result() : flag(false){}
	};
protected:
	// this methos from session class event
	//

	// realserver list iterator begin method
	rs_list_itr_func_type	rs_list_begin;
	// realserver list iterator end method
	rs_list_itr_func_type	rs_list_end;
	// realserver list iterator next method
	rs_list_itr_func_type	rs_list_next;

	// logger method
	logger_func_type		logger;
	// replication memory peyment method
	replicationpaymemory_func_type
							replication_pay_memory;

	//scheduler_method
	tcp_schedule_func_type		schedule_tcp;
	udp_schedule_func_type		schedule_udp;

	// realserver list lock function object
	boost::function< void( void ) >	rs_list_lock;
	// realserver list unlock function object
	boost::function< void( void ) >	rs_list_unlock;

	// replication area lock function object
	boost::function< void( void ) >	replication_area_lock;
	// replication area unlock function object
	boost::function< void( void ) >	replication_area_unlock;

public:

	protocol_module_base( logger_func_type inlog ) : logger( inlog ) {};
							
	virtual	~protocol_module_base() = 0;

	virtual	void	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock,
							replicationpaymemory_func_type  inreplication_pay_memory ) = 0;

	virtual	void	finalize() = 0;

	// event function
	virtual	bool	is_use_sorry() = 0;
	virtual	check_message_result	check_parameter( const std::vector<std::string>& args ) = 0;

	virtual	void	handle_rslist_update() = 0;

	virtual	check_message_result	set_parameter( const std::vector<std::string>& args ) = 0;
	virtual	check_message_result	add_parameter( const std::vector<std::string>& args ) = 0;

	virtual	void	register_schedule( tcp_schedule_func_type inschedule ) = 0;
	virtual	void	register_schedule( udp_schedule_func_type inschedule ) = 0;

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

	virtual	EVENT_TAG	handle_accept( const boost::thread::id thread_id ) = 0;

	virtual	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen ) = 0;

	virtual	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint ) = 0;

	virtual	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ) = 0;

	virtual	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_send(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint ) = 0;

	virtual	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ) = 0;

	virtual	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ) = 0;

	//use in downstream_thread
	virtual	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen ) = 0;
	virtual	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen ) = 0;
	
	virtual	EVENT_TAG	handle_response_send_inform(
									const boost::thread::id thread_id ) = 0;

	virtual EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & cl_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ) = 0;

	virtual	EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ) = 0;

	virtual	EVENT_TAG	handle_client_send( const boost::thread::id thread_id ) = 0;

	//use in upstream/downstream thread
	virtual	EVENT_TAG	handle_client_disconnect(
									const boost::thread::id thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id ) = 0;

	virtual	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id ) = 0;

	virtual	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint ) = 0;

	virtual	EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint ) = 0;

//	virtual	EVENT_TAG	handle_event( const pthread_t thread_id, EVENT_TAG prev_status,  ) = 0;
};

} // namespace l7vsd

#endif	//PROTOCOL_MODULE_BASE_H
