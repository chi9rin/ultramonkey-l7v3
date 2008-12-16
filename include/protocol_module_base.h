#ifndef	PROTOCOL_MODULE_BASE_H
#define	PROTOCOL_MODULE_BASE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "module_base.h"

namespace l7vsd{

class protocol_module_base : public module_base{
public:
	enum	EVENT_TAG
	{
		//use in upstream_thread
		CLIENT_RECV = 0,
		CLIENT_RESPOND_SEND,
		REALSERVER_CONNECT,
		SORRYSERVER_CONNECT,
		//use in downstream_thread
		REALSERVER_RECV,
		SORRYSERVER_RECV,
		CLIENT_RESPOND_SEND,
		CLIENT_SEND,
		//COMMON Status
		REALSERVER_DISCONNECT,
		SORRYSERVER_DISCONNECT,
		CLIENT_DISCONNECT,
		FINALIZE,
		STOP
	};
	

	//this class is POD
	struct check_message_result{
		bool	flag;
		string	message;
		bool	operator==( const check_message& in ){ return flag == in.flag; }
		bool	operator!=( const check_message& in ){ return flag != in.flag; }
	};

public:

	// event function
	protocol_module_base() = 0;
	virtual ~protocol_module_base() = 0;
	virtual	bool	is_use_sorry() = 0;
	virtual	const check_message_result&	check_parameter( const std::vector<std::string>& args ) =0;

	virtual boost::function< void ( void ) > rs_list_begin;
	virtual boost::function< void ( void ) > rs_list_end;
	virtual boost::function< void ( void ) > rs_list_next;

	virtual	handle_rslist_update() = 0;

	virtual	boost::function< void ( void ) > rs_list_lock;
	virtual	boost::function< void ( void ) > rs_list_unlock;

	virtual	void	set_parameter( const std::vector<std::string>& args ) = 0;
	virtual	void	add_parameter( const std::vector<std::string>& args ) = 0;

	virtual	void	handle_sorry_enable() = 0;
	virtual	void	handle_sorry_disable() = 0;

	//use in upstream_thread
	virtual	EVENT_TAG	handle_accept(
									const pthread_t upstream_thread_id,
									const pthread_t downstream_thread_id ) = 0;

	virtual	EVENT_TAG	handle_client_recv(
									const pthread_t thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recv_len,
									const boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;

	virtual	EVENT_TAG	handle_realserver_connect(
									const pthread_t thread_id,
									const boost::array<char,MAX_BFFER_SIZE>& sendbuffer ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_connection_fail(
									const pthread_t thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_send(
									const pthread_t thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_connect(
									const pthread_t thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer ) = 0;

	virtual	EVENT_TAG	handle_sorryserver_connection_fail(
									const pthread_t thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_send(
									const pthread_t thread_id ) = 0;

	//use in downstream_thread
	virtual	EVENT_TAG	handle_realserver_recv(
									const pthread_t thread_id,
									const boost::asio::ip::basic_endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recv_len ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_recv(
									const pthread_t thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recv_len ) = 0;
	
	virtual	EVENT_TAG	handle_clientrespondmessage_get(
									const pthread_t thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& sendbuffer ) = 0;

	virtual EVENT_TAG	handle_client_connection_check(
									const pthread_t,
									const boost::asio::ip::basic_endpoint& client_endpoint = 0);

	virtual	EVENT_TAG	handle_client_send(
									const pthread_t thread_id ) = 0;

	//use in upstream/downstream thread
	virtual	EVENT_TAG	handle_client_disconnect(
									const pthread_t thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_realserver_disconnect(
									const pthread_t thread_id,
									const boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;
	
	virtual	EVENT_TAG	handle_sorryserver_disconnect(
									const pthread_t thread_id ) = 0;
	
	virtual	EVENT_TAG	handle_finalize(
									const pthread_t thread_id ) = 0;

//	virtual	EVENT_TAG	handle_event( const pthread_t thread_id, EVENT_TAG prev_status,  ) = 0;
};

}; // namespace l7vsd

#endif	//PROTOCOL_MODULE_BASE_H
