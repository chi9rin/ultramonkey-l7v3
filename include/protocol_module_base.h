//
//!	@file	protocol_module_base.h
//!	@brief	shared object protocol module absctract class
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

//!	@class	protocol_module_base
//! @brief	this class is base class of all protocol module classes.
class protocol_module_base : public module_base {
public:
	//!	realserver list type
	typedef	std::list<realserver>	realserverlist_type;
	//! realserver iterator function object type
	typedef	boost::function< realserverlist_type::iterator( void ) >
								rs_list_itr_func_type;
	//! TCP/IP scheduler function object type.
	typedef	boost::function< void (		const boost::thread::id,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										boost::asio::ip::tcp::endpoint& ) >
								tcp_schedule_func_type;
	//! UDP scheduler function object type.
	typedef	boost::function< void (		const boost::thread::id,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										boost::asio::ip::udp::endpoint& ) >
								udp_schedule_func_type;

	//! @enum	EVENT_TAG
	//! @brief	event tag is return to session.
	enum	EVENT_TAG{
		//use in upstream_thread
		CLIENT_RECV = 0,			//!< Receive from Client
		REALSERVER_SELECT,			//!< Select RealServer
		REALSERVER_CONNECT,			//!< Connect to RealServer
		REALSERVER_SEND,			//!< Send message to RealServer
		SORRYSERVER_SELECT,			//!< Select SorryServer
		SORRYSERVER_CONNECT,		//!< Connect to SorryServer
		SORRYSERVER_SEND,			//!< Send message to SorryServer
		//use in downstream_thread
		REALSERVER_RECV,			//!< Receive from RealServer
		SORRYSERVER_RECV,			//!< Receive from SorryServer
		CLIENT_SELECT,				//!< Select Client
		CLIENT_CONNECTION_CHECK,	//!< Check ClientConnection available
		CLIENT_SEND,				//!< Send message to Client
		//COMMON Status
		CLIENT_RESPONSE_SEND,		//!< Send UML7response message to Client
		REALSERVER_DISCONNECT,		//!< Disconnect RealServerConnection
		SORRYSERVER_DISCONNECT,		//!< Disconnect SorryServerConnection
		CLIENT_DISCONNECT,			//!< Disconnect ClientConnection
		REALSERVER_CLOSE,			//!< Close socket(RealServer side)
		FINALIZE,					//!< Do finalize
		STOP						//!< Stop SessionThread
	};

	//! @struct check_message_result
	//!	@brief	this class is POD. this class used then return multi value
	struct check_message_result{
		bool		flag;		//! return flasg
		std::string	message;	//! return message
		//!	== operator
		//! @param[in]	check_message_result& is equal check object.
		//! @return true is equal
		//! @return false is not equal
		bool		operator==( const check_message_result& in )
								{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
		//! @param[in]	check_message_result& is not equal check object.
		//! @return true is not equal
		//! @return false is equal
		bool		operator!=( const check_message_result& in )
								{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
		//! constractor
		check_message_result() : flag(false){}
	};
protected:
	//! realserver list iterator begin function object
	rs_list_itr_func_type	rs_list_begin;
	//! realserver list iterator end function object
	rs_list_itr_func_type	rs_list_end;
	//! realserver list iterator next function object
	rs_list_itr_func_type	rs_list_next;

	//! realserver list lock function object
	boost::function< void( void ) >	rs_list_lock;
	//! realserver list unlock function object
	boost::function< void( void ) >	rs_list_unlock;

	tcp_schedule_func_type		schedule_tcp;	//!< tcp_scheduler_function object
	udp_schedule_func_type		schedule_udp;	//!< udp_scheduler function object

public:
	//! constractor
	protocol_module_base( std::string in_modulename ) : module_base( in_modulename ){};
	//! destractor
	virtual	~protocol_module_base(){};
	//! initialize function. called from module control. module loaded call
	//! @param[in]	realserver list iterator begin function object type
	//!	@param[in]	realserver list iterator end function object type
	//! @param[in]	realserver list iterator next function object type
	//! @param[in]	realserver list mutex lock function object type.
	//! @param[in]	realserver list mutex unlock function object type
	virtual	void	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock ) = 0;

	//! finalize called from module control. module unloaded call.
	virtual	void	finalize() = 0;

	//! sorry support check
	//! @return true sorry mode is supported.
	//! @return false sorry mode is unsupported.
	virtual	bool	is_use_sorry() = 0;

	//! realserver list update event
	virtual	void	handle_rslist_update() = 0;


	//! module parameter check.used by l7vsadm
	//! @param[in]	module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	virtual	check_message_result	check_parameter( const std::vector<std::string>& args ) = 0;
	//! parameter set
	//! @param[in] module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	virtual	check_message_result	set_parameter( const std::vector<std::string>& args ) = 0;
	//! parameter add
	//! @param[in] module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	virtual	check_message_result	add_parameter( const std::vector<std::string>& args ) = 0;

	//! TCP/IP scheduled function registation.
	//! @param[in] schedule module TCP/IP scheduled function object type
	virtual	void	register_schedule( tcp_schedule_func_type inschedule ) = 0;
	//! UDP scheduled function registation
	//! @param[in] schedule module UDP scheduled funtion object type
	virtual	void	register_schedule( udp_schedule_func_type inschedule ) = 0;

	//! called from session initialzie use in upstream_thread
	//! @param[in]	upstream thread id.
	//! @param[in]	downstream thread id
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id ) = 0;

	//! called from session finalize use in upstream thread.
	//! @param[in]	upstream thread id.
	//! @param[in]	downstream thread id
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id ) = 0;

	//! called from after session accept.in client socket use in upstream thread.
	//! @param[in]	upstream thread id.
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_accept( const boost::thread::id thread_id ) = 0;

	//! called from after session recv in client socket. use in upstream thread.
	//! @param[in]	upstream thread id
	//! @param[in]	recive buffer refarence.
	//! @param[in]	recive length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ) = 0;

	//! called from after realserver select.use in upstream thread. 
	//! @param[in]	upstream thread id
	//! @param[out]	realserver TCP endpoint
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint ) = 0;

	//! called from after realserver select
	//! @param[in]	upstream thread id
	//! @param[out]	realserver UDP endpoint
	//! @param[out]	sendbudffer reference
	//! @param[out]	send data length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ) = 0;

	//! called from after realserver connect
	//! @param[in]	upstream thread id
	//! @param[out]	sendbuffer reference
	//! @param[out]	send data length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ) = 0;

	//! called from after realserver connection fail
	//! @param[in]	upstream thread id
	//! @param[in]	fail realserver endpoint reference
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint ) = 0;

	//! called from after realserver send.
	//! @param[in]	upstream thread id
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_send(
									const boost::thread::id thread_id ) = 0;

	//! called from after sorryserver select
	//! @param[in]	upstream thread id
	//! @param[in]	sorryserver endppiont reference
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& sorry_endpoint ) = 0;

	//! called from after sorryserver connect
	//!	@param[in]	upstream thread id
	//! @param[out]	send buffer reference.
	//! @param[out]	send length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ) = 0;

	//! called from after sorryserver connection fail
	//! @param[in]	upstream thread id
	//! @param[in]	sorryserver endpoint reference.
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint ) = 0;

	//! called from after sorryserver send
	//! @param[in]	upstream thread id
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ) = 0;

	//! called from after realserver recvive for TCP/IP
	//! @param[in]	downstream thread id
	//! @param[in]	realserver TCP/IP endpoint reference
	//! @param[in]	realserver recive buffer reference.
	//! @param[in]	recv data length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ) = 0;
	//! called from after realserver recive.for UDP
	//! @param[in]	downstream thread id
	//! @param[in]	realserver UDP endpoint reference
	//! @param[in]	recive from realserver buffer reference
	//! @param[in]	recv data length
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint& rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ) = 0;

	//! called from after sorryserver recive
	//! @param[in]	downstream thread id
	//! @param[in]	sorryserver endpoint reference
	//! @param[in]	recive from realserver buffer reference.
	//! @param[in]	recv data length
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ) = 0;

	//! called from UPSTEEARM thread. make module original message.
	//! @param[in]	downstream thread id.
	//! @return 		session use EVENT mode
	virtual	EVENT_TAG	handle_response_send_inform(
									const boost::thread::id thread_id ) = 0;

	//! called from after client connection check. use TCP/IP only. create client send message.
	//! @param[in]	downstream thread id
	//! @param[out]	send budffer reference
	//! @param[out]	send data length
	//! @return 	session use EVENT mode
	virtual EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ) = 0;

	//! called from after client select. use UDP only
	//! @param[in]	downstream thread id
	//!	@param[in]	client udp endpoint
	//! @param[out]	send buffer reference
	//! @param[out]	send data length
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& cl_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ) = 0;

	//!	called from after client send
	//!	@param[in]	downstream thread id
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_client_send( const boost::thread::id thread_id ) = 0;

	//! call from client disconnect event. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event! )
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_client_disconnect(
									const boost::thread::id thread_id ) = 0;

	//! call from sorry mode event. use upstream thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event and first time call pattern )	
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id ) = 0;

	//! call from sorry mode disable. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id ) = 0;

	//! call from realserver disconnect. use upstream thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnected realserver endpoint.
	//! @return 	session use EVENT mode
	virtual	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint ) = 0;

	//! call from sorry server disconnect. use upstraem thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnect sorryserver endpoint
	//! @return		session use EVENT mode	
	virtual	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint ) = 0;

	//! call from realserver disconnect. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnect realserver endpoint
	//! @return		session use EVENT mode.
	virtual	EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint& rs_endpoint ) = 0;
};

} // namespace l7vsd

#endif	//PROTOCOL_MODULE_BASE_H
