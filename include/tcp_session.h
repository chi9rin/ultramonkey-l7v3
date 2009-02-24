//
//!	@file	tcp_session.h
//!	@brief	tcp session class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef TCP_SESSION_H
#define TCP_SESSION_H

#include <bitset>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "protocol_module_base.h"
#include "session_result_message.h"
#include "tcp_socket.h"
#include "tcp_realserver_connect_socket_list.h"
#include "tcp_data.h"
#include "tcp_thread_message_que.h"

#define TCP_SESSION_THREAD_STATE_BIT 8

namespace l7vs{

//!	@class	virtualservice_tcp
//! @brief	this class is parent virtualservice class.
	class virtualservice_tcp;

//!	@class	tcp_session
//! @brief	this class is tcp session class.
	class tcp_session : private boost::noncopyable{
		public:
			//! process type
			enum TCP_PROCESS_TYPE_TAG{
				LOCAL_PROC = 0,
				MESSAGE_PROC
			};
			//! tcp virtualservice message type
			enum TCP_VIRTUAL_SERVICE_MESSAGE_TAG{
				SORRY_STATE_ENABLE = 0,
				SORRY_STATE_DISABLE,
				SESSION_END,
				SESSION_PAUSE_ON,
				SESSION_PAUSE_OFF
			};

			//! construcor
			//! @param[in/out]	vs is parent virtualservice object
			//! @param[in/out]	io is session use io service object
			tcp_session(virtualservice_tcp& vs,boost::asio::io_service& session_io);
			//! destructor
			virtual ~tcp_session();
			//! initialize
			session_result_message initialize();
			//! get reference client side socket
			//! @return			reference client side socket
			boost::asio::ip::tcp::socket& get_client_socket();
			//! is thread wait
			//! @return 		true is wait
			//! @return 		false is not wait
			bool is_thread_wait();
			//! message from parent virtualservice
			//! @param[in]		message is tcp virtualservice message type
			void set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message);
			//! up stream thread main function
			void up_thread_run();
			//! down stream thread main function
			void down_thread_run();
			
		protected:
			typedef boost::asio::ip::tcp::endpoint endpoint;
			typedef boost::shared_ptr< tcp_socket > tcp_socket_ptr;
			typedef std::pair< endpoint, tcp_socket_ptr> socket_element;
			typedef boost::function< void(TCP_PROCESS_TYPE_TAG) > tcp_session_func;
			//! up thread call function type
			enum UP_THREAD_FUNC_TYPE_TAG{
				UP_FUNC_CLIENT_ACCEPT = 0,					//! up_thread_client_accept_event function
				UP_FUNC_CLIENT_DISCONNECT,					//! up_thread_client_disconnect function
				UP_FUNC_CLIENT_DISCONNECT_EVENT,			//! up_thread_client_disconnect_event function
				UP_FUNC_CLIENT_RECEIVE,						//! up_thread_client_receive function
				UP_FUNC_CLIENT_RESPOND_SEND,				//! up_thread_client_respond function
				UP_FUNC_CLIENT_RESPOND_SEND_EVENT,			//! up_thread_client_respond_event function
				UP_FUNC_REALSERVER_GET_DEST_EVENT,			//! up_thread_realserver_get_distination_event function
				UP_FUNC_REALSERVER_CONNECT,					//! up_thread_realserver_connect function
				UP_FUNC_REALSERVER_CONNECT_EVENT,			//! up_thread_realserver_connect_event function
				UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT,		//! up_thread_realserver_connection_fail_event function
				UP_FUNC_REALSERVER_SEND,					//! up_thread_realserver_send function
				UP_FUNC_REALSERVER_DISCONNECT,				//! up_thread_realserver_disconnect function
				UP_FUNC_REALSERVER_DISCONNECT_EVENT,		//! up_thread_realserver_disconnect_event function
				UP_FUNC_REALSERVER_ALL_DISCONNECT,			//! up_thread_all_realserver_disconnect function
				UP_FUNC_SORRYSERVER_GET_DEST,				//! up_thread_sorryserver_get_destination_event function
				UP_FUNC_SORRYSERVER_CONNECT,				//! up_thread_sorryserver_connect function
				UP_FUNC_SORRYSERVER_CONNECT_EVENT,			//! up_thread_sorryserver_connect_event function
				UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT,		//! up_thread_sorryserver_connection_fail_event function
				UP_FUNC_SORRYSERVER_SEND,					//! up_thread_sorryserver_send function
				UP_FUNC_SORRYSERVER_DISCONNECT,				//! up_thread_sorryserver_disconnect function
				UP_FUNC_SORRYSERVER_MOD_DISCONNECT,			//! up_thread_sorryserver_mod_disconnect function
				UP_FUNC_SORRYSERVER_DISCONNECT_EVENT,		//! up_thread_sorryserver_disconnect_event function
				UP_FUNC_SORRY_ENABLE_EVENT,					//! up_thread_sorry_enable_event function
				UP_FUNC_SORRY_DISABLE_EVENT,				//! up_thread_sorry_disable_event function
				UP_FUNC_EXIT								//! up_thread_exit function
			};
			//! down thread call function type
			enum DOWN_THREAD_FUNC_TYPE_TAG{
				DOWN_FUNC_CLIENT_DISCONNECT = 0,			//! down_thread_client_disconnect function
				DOWN_FUNC_CLIENT_DISCONNECT_EVENT,			//! down_thread_client_disconnect_event function
				DOWN_FUNC_CLIENT_CONNECTION_CHK,			//! down_thread_client_connection_chk_event function
				DOWN_FUNC_CLIENT_SEND,						//! down_thread_client_send function
				DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT,		//! down_thread_client_respond_event function
				DOWN_FUNC_REALSERVER_RECEIVE,				//! down_thread_realserver_receive function
				DOWN_FUNC_REALSERVER_DISCONNECT,			//! down_thread_realserver_disconnect function
				DOWN_FUNC_REALSERVER_DISCONNECT_EVENT,		//! down_thread_realserver_disconnect_event function
				DOWN_FUNC_REALSERVER_ALL_DISCONNECT,		//! down_thread_all_realserver_disconnect function
				DOWN_FUNC_SORRYSERVER_RECEIVE,				//! down_thread_sorryserver_receive function
				DOWN_FUNC_SORRYSERVER_DISCONNECT,			//! down_thread_sorryserver_disconnect function
				DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT,		//! down_thread_sorryserver_mod_disconnect function
				DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT,		//! down_thread_sorryserver_disconnect_event function
				DOWN_FUNC_SORRY_ENABLE_EVENT,				//! down_thread_sorry_enable_event function
				DOWN_FUNC_SORRY_DISABLE_EVENT,				//! down_thread_sorry_disable_event function
				DOWN_FUNC_EXIT								//! down_thread_exit function
			};
			//! tcp_session use io service
			boost::asio::io_service& io;
			//! parent virtualservice
			virtualservice_tcp& parent_service;
			
			//! thread main loop exit flag
			bool exit_flag;
			//! thread main loop exit flag update mutex
			boost::mutex exit_flag_update_mutex;
			//! up and down thread status bit frag
			std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_state;
			//! thread_state update mutex
			boost::mutex thread_state_update_mutex;
			//! up thread id
			boost::thread::id up_thread_id;
			//! down thread id
			boost::thread::id down_thread_id;
			//! pointer of protocol module
			protocol_module_base* protocol_module;
			//! up and down thread wait flag
			bool session_pause_flag;
			//! client socket
			tcp_socket client_socket;
			//! sorryserver socket
			socket_element sorryserver_socket;
			//! up thread use realserver socket map
			std::map<endpoint,tcp_socket_ptr> up_thread_send_realserver_socket_map;
			//! connect realserver list
			tcp_realserver_connect_socket_list down_thread_connect_socket_list;
			//! down thread use realserver socket list
			std::list<socket_element> down_thread_receive_realserver_socket_list;
			//! down thread next receive realserver
			std::list<socket_element>::iterator down_thread_current_receive_realserver_socket;
			//! module function mutex (handle_response_send_inform) 
			boost::mutex module_function_response_send_inform_mutex;
			//! module function mutex (handle_client_disconnect) 
			boost::mutex module_function_client_disconnect_mutex;
			//! module function mutex (handle_realserver_disconnect) 
			boost::mutex module_function_realserver_disconnect_mutex;
			//! module function mutex (handle_sorryserver_disconnect) 
			boost::mutex module_function_sorryserver_disconnect_mutex;
			//! module function mutex (handle_sorry_enable) 
			boost::mutex module_function_sorry_enable_mutex;
			//! module function mutex (handle_sorry_disable) 
			boost::mutex module_function_sorry_disable_mutex;
			//! up thread call function object map
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func > up_thread_function_map;
			//! up thread recept module event convert to up thread function type map
			std::map< protocol_module_base::EVENT_TAG , UP_THREAD_FUNC_TYPE_TAG > up_thread_module_event_map;
			//! up thread raise message map for down thread
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func > up_thread_message_down_thread_function_map;
			//! up thread next call function object
			tcp_session_func up_thread_next_call_function;
			//! up thread use client side buffer
			tcp_data up_thread_data_client_side;
			//! up thread use destination side buffer
			tcp_data up_thread_data_dest_side;
			//! message data queue for up thread
			tcp_thread_message_que up_thread_message_que;
			//! up thread use message data
			tcp_data up_thread_message_data;
			//! down thread call function object map
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func > down_thread_function_map;
			//! down thread recept module event convert to down thread function type map
			std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG > down_thread_module_event_map;
			//! down thread raise message map for up thread
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func > down_thread_message_up_thread_function_map;
			//! down thread next call function object
			tcp_session_func down_thread_next_call_function;
			//! down thread use client side buffer
			tcp_data down_thread_data_client_side;
			//! down thread use destination side buffer
			tcp_data down_thread_data_dest_side;
			//! message data queue for down thread
			tcp_thread_message_que down_thread_message_que;
			//! up thread use message data
			tcp_data down_thread_message_data;
			
			//! virtual service message convert to up thread function object map
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>  virtual_service_message_up_thread_function_map;
			//! virtual service message convert to down thread function object map
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>  virtual_service_message_down_thread_function_map;

			//! up and down thread state update
			//! @param[in]		thread_flag is regist or unregist bitset
			//! @param[in]		regist is regist or unregist flag
			virtual void thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist);

			//! up thread raise module event of handle_accept
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread receive client side and raise module event of handle_client_recv
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise client respond send event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_response_send_inform
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close client socket and raise client disconnect event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_client_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread send realserver and raise module event of handle_client_recv
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_realserver_select
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread connect realserver
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_realserver_connect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_realserver_connection_fail
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close realserver socket and raise realserver disconnect event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_realserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close all realserver socket and raise module event of handle_realserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread send sorryserver and raise module event of handle_sorryserver_send
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorryserver_select
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread connect sorryserver
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorryserver_connect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorryserver_connection_fail
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close sorryserver socket and raise module sorryserver disconnect event
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorryserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorry_enable
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread raise module event of handle_sorry_disable
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread exit main loop
			//! @param[in]		process_type is prosecess type
			virtual void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
			//! up thread close all socket
			virtual void up_thread_all_socket_close(void);

			//! down thread receive from realserver and raise module event of handle_realserver_recv
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_realserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close all realserver socket and raise module event of handle_realserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_client_connection_check
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_response_send_inform
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread send for client and raise module event of handle_client_send
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close client socket and raise client disconnect event message for up and down thread 
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_client_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close sorryserver socket and raise module sorryserver disconnect event
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_sorryserver_disconnect
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_sorry_enable
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread raise module event of handle_sorry_disable
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread exit main loop
			//! @param[in]		process_type is prosecess type
			virtual void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
			//! down thread close all socket
			virtual void down_thread_all_socket_close(void);

	};// class tcp_session
}// namespace l7vs

#endif//TCP_SESSION_H
 

