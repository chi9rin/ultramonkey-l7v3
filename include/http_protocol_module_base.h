//
//	@file	http_protocol_module_base.h
//	@brief	shared object http protocol module absctract class
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	HTTP_PROTOCOL_MODULE_BASE_H
#define	HTTP_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"

namespace l7vs{

class http_protocol_module_base : public protocol_module_base {
protected:

	// http function
	int	check_http_method(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	int	check_http_version(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	int	check_status_code(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	bool	find_uri(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								int& uri_offset,
								int& uri_len );
	bool	find_status_code(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								int& status_code_offset,
								int& status_code_len );
	bool	find_http_header(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								const boost::array<char,MAX_BUFFER_SIZE>& http_header_name,
								const int http_header_name_len,
								int& http_header_offset,
								int& http_header_len );

public:

	http_protocol_module_base( logger_func_type inlog ) : logger( inlog );
							
	~http_protocol_module_base();

	initialize(
								rs_list_itr_func_type	inlist_begin,
								rs_list_itr_func_type	inlist_end,
								rs_list_itr_func_type	inlist_next,
								boost::function< void( void ) >	inlist_lock,
								boost::function< void( void ) >	inlist_unlock,
								replicationpaymemory_func_type  inreplication_pay_memory
							);

	finalize();

	// event function
	bool	is_use_sorry();
	check_message_result&	check_parameter( const std::vector<std::string>& args ) const;

	void	handle_rslist_update();

	check_message_result&	set_parameter( const std::vector<std::string>& args );
	check_message_result&	add_parameter( const std::vector<std::string>& args );

	void	register_schedule( tcp_schedule_func_type inschedule );

	void	register_replication_area_lock(
									boost::function< void( void ) > inlock_func );

	void	register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func );

	//use in upstream_thread
	EVENT_TAG	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id );
	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id );
	EVENT_TAG	handle_accept(
									const boost::thread::id thread_id );

	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen );

	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint );

	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer );
	
	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint );
	
	EVENT_TAG	handle_realserver_send(
									const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint );

	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer );

	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorryserver_send(
									const boost::thread::id thread_id );

	//use in downstream_thread
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen );
	
	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen );
	
	EVENT_TAG	handle_response_send_inform(
									const boost::thread::id thread_id );

	EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & recv_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer );

	EVENT_TAG	handle_client_send(
									const boost::thread::id thread_id );

	//use in upstream/downstream thread
	EVENT_TAG	handle_client_disconnect(
									const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id );

	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id );

	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint );
	
	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint );


};

} // namespace l7vsd

#endif	//HTTP_PROTOCOL_MODULE_BASE_H
