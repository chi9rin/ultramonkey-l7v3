//
//!	@file	tcp_session.cpp
//!	@brief	tcp session class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include "tcp_session.h"
#include "tcp_thread_message.h"
#include "virtualservice.h"
#include "logger.h"

#define UP_THREAD_ALIVE		std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001)
#define DOWN_THREAD_ALIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002)
#define UP_THREAD_ACTIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004)
#define DOWN_THREAD_ACTIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008)
#define UP_THREAD_LOCK 		std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010)
#define DOWN_THREAD_LOCK 	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020)

namespace l7vs{
	//! construcor
	//! @param[in/out]	vs is parent virtualservice object
	//! @param[in/out]	io is session use io service object
	tcp_session::tcp_session(virtualservice_tcp& vs,boost::asio::io_service& session_io):
		io(session_io),
		parent_service(vs),
		exit_flag(false),
		thread_state(0),
		protocol_module(NULL),
		session_pause_flag(false),
		client_socket(session_io){
		
		// sorryserver socket
		tcp_socket_ptr sorry_socket(new tcp_socket(session_io));
		sorryserver_socket.second = sorry_socket;
		
		// set up_thread_module_event_map
		std::pair<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG> add_up_thread_event;
		add_up_thread_event.first = protocol_module_base::ACCEPT;
		add_up_thread_event.second = UP_FUNC_CLIENT_ACCEPT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::CLIENT_DISCONNECT;
		add_up_thread_event.second = UP_FUNC_CLIENT_DISCONNECT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::CLIENT_RECV;
		add_up_thread_event.second = UP_FUNC_CLIENT_RECEIVE;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::CLIENT_RESPONSE_SEND;
		add_up_thread_event.second = UP_FUNC_CLIENT_RESPOND_SEND;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::REALSERVER_SELECT;
		add_up_thread_event.second = UP_FUNC_REALSERVER_GET_DEST_EVENT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::REALSERVER_CONNECT;
		add_up_thread_event.second = UP_FUNC_REALSERVER_CONNECT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::REALSERVER_SEND;
		add_up_thread_event.second = UP_FUNC_REALSERVER_SEND;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::REALSERVER_DISCONNECT;
		add_up_thread_event.second = UP_FUNC_REALSERVER_ALL_DISCONNECT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::SORRYSERVER_SELECT;
		add_up_thread_event.second = UP_FUNC_SORRYSERVER_GET_DEST;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::SORRYSERVER_CONNECT;
		add_up_thread_event.second = UP_FUNC_SORRYSERVER_CONNECT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::SORRYSERVER_SEND;
		add_up_thread_event.second = UP_FUNC_SORRYSERVER_SEND;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::SORRYSERVER_DISCONNECT;
		add_up_thread_event.second = UP_FUNC_SORRYSERVER_DISCONNECT;
		up_thread_module_event_map.insert(add_up_thread_event);
		add_up_thread_event.first = protocol_module_base::FINALIZE;
		add_up_thread_event.second = UP_FUNC_EXIT;
		up_thread_module_event_map.insert(add_up_thread_event);
		
		// set up_thread_function_map
		std::pair<UP_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_up_thread_func;
		add_up_thread_func.first = UP_FUNC_CLIENT_ACCEPT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_accept_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_CLIENT_DISCONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_disconnect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_CLIENT_DISCONNECT_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_disconnect_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_CLIENT_RECEIVE;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_receive,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_CLIENT_RESPOND_SEND;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_respond,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_CLIENT_RESPOND_SEND_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_client_respond_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_GET_DEST_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_get_destination_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_CONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_connect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_CONNECT_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_connect_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_connection_fail_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_SEND;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_send,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_DISCONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_disconnect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_DISCONNECT_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_realserver_disconnect_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_REALSERVER_ALL_DISCONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_all_realserver_disconnect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_GET_DEST;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_get_destination_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_CONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_connect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_CONNECT_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_connect_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_connection_fail_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_SEND;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_send,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_DISCONNECT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_disconnect,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_SORRYSERVER_DISCONNECT_EVENT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
		add_up_thread_func.first = UP_FUNC_EXIT;
		add_up_thread_func.second = boost::bind(&tcp_session::up_thread_exit,this,_1);
		up_thread_function_map.insert(add_up_thread_func);
	 	
		// set up_thread_message_down_thread_function_map
		std::pair<DOWN_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_up_thread_message_func;
		add_up_thread_message_func.first = DOWN_FUNC_CLIENT_DISCONNECT_EVENT;
		add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_disconnect_event,this,_1);
		up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
		add_up_thread_message_func.first = DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT;
		add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_respond_event,this,_1);
		up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
		add_up_thread_message_func.first = DOWN_FUNC_REALSERVER_DISCONNECT_EVENT;
		add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_realserver_disconnect_event,this,_1);
		up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
		add_up_thread_message_func.first = DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT;
		add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event,this,_1);
		up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
		
		// set down_thread_module_event_map
		std::pair<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG> add_down_thread_event;
		add_down_thread_event.first = protocol_module_base::CLIENT_SEND;
		add_down_thread_event.second = DOWN_FUNC_CLIENT_SEND;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::CLIENT_DISCONNECT;
		add_down_thread_event.second = DOWN_FUNC_CLIENT_DISCONNECT;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::CLIENT_CONNECTION_CHECK;
		add_down_thread_event.second = DOWN_FUNC_CLIENT_CONNECTION_CHK;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::REALSERVER_RECV;
		add_down_thread_event.second = DOWN_FUNC_REALSERVER_RECEIVE;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::REALSERVER_DISCONNECT;
		add_down_thread_event.second = DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::SORRYSERVER_RECV;
		add_down_thread_event.second = DOWN_FUNC_SORRYSERVER_RECEIVE;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::SORRYSERVER_DISCONNECT;
		add_down_thread_event.second = DOWN_FUNC_SORRYSERVER_DISCONNECT;
		down_thread_module_event_map.insert(add_down_thread_event);
		add_down_thread_event.first = protocol_module_base::FINALIZE;
		add_down_thread_event.second = DOWN_FUNC_EXIT;
		down_thread_module_event_map.insert(add_down_thread_event);
		
		// set down_thread_function_map
		std::pair<DOWN_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_down_thread_func;
		add_down_thread_func.first = DOWN_FUNC_CLIENT_DISCONNECT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_client_disconnect,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_CLIENT_DISCONNECT_EVENT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_client_disconnect_event,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_CLIENT_CONNECTION_CHK;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_client_connection_chk_event,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_CLIENT_SEND;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_client_send,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_REALSERVER_RECEIVE;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_realserver_receive,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_REALSERVER_DISCONNECT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_realserver_disconnect,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_REALSERVER_DISCONNECT_EVENT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_realserver_disconnect_event,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_all_realserver_disconnect,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_SORRYSERVER_RECEIVE;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_sorryserver_receive,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_SORRYSERVER_DISCONNECT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_sorryserver_disconnect,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		add_down_thread_func.first = DOWN_FUNC_EXIT;
		add_down_thread_func.second = boost::bind(&tcp_session::down_thread_exit,this,_1);
		down_thread_function_map.insert(add_down_thread_func);
		
		// set down_thread_message_up_thread_function_map
		std::pair<UP_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_down_thread_message_func;
		add_down_thread_message_func.first = UP_FUNC_CLIENT_DISCONNECT_EVENT;
		add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_client_disconnect_event,this,_1);
		down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
		add_down_thread_message_func.first = UP_FUNC_REALSERVER_DISCONNECT_EVENT;
		add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_realserver_disconnect_event,this,_1);
		down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
		add_down_thread_message_func.first = UP_FUNC_SORRYSERVER_DISCONNECT_EVENT;
		add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event,this,_1);
		down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
		
		// set virtual_service_message_up_thread_function_map
		std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG , tcp_session_func > add_up_thread_vs_message_func;
		add_up_thread_vs_message_func.first = SORRY_STATE_ENABLE;
		add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_enable_event,this,_1);
		virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
		add_up_thread_vs_message_func.first = SORRY_STATE_DISABLE;
		add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_disable_event,this,_1);
		virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
		add_up_thread_vs_message_func.first = SESSION_END;
		add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_exit,this,_1);
		virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
		
		// set virtual_service_message_down_thread_function_map
		std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG , tcp_session_func > add_down_thread_vs_message_func;
		add_down_thread_vs_message_func.first = SORRY_STATE_ENABLE;
		add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_enable_event,this,_1);
		virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
		add_down_thread_vs_message_func.first = SORRY_STATE_DISABLE;
		add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_disable_event,this,_1);
		virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
		add_down_thread_vs_message_func.first = SESSION_END;
		add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_exit,this,_1);
		virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
		
	}
	//! destructor
	tcp_session::~tcp_session(){
		// up_thread_module_event_map
		up_thread_module_event_map.clear();
		// up_thread_function_map
		up_thread_function_map.clear();
		// up_thread_message_down_thread_function_map
		up_thread_message_down_thread_function_map.clear();
		// down_thread_module_event_map
		down_thread_module_event_map.clear();
		// down_thread_function_map
		down_thread_function_map.clear();
		// down_thread_message_down_thread_function_map
		down_thread_message_up_thread_function_map.clear();
		// virtual_service_message_up_thread_function_map
		virtual_service_message_up_thread_function_map.clear();
		// virtual_service_message_down_thread_function_map
		virtual_service_message_down_thread_function_map.clear();
		// up_thread_message_que
		up_thread_message_que.clear();
		// down_thread_message_que
		down_thread_message_que.clear();
	}
	//! initialize
	session_result_message tcp_session::initialize(){
		session_result_message msg;
		msg.flag = false;
		msg.message = "";
		exit_flag = false;
		up_thread_id = boost::thread::id();
		down_thread_id = boost::thread::id();
		thread_state.reset();
		protocol_module = NULL;
		session_pause_flag = false;
		up_thread_message_que.clear();
		down_thread_message_que.clear();
		protocol_module = parent_service.get_protocol_module();
		if(protocol_module == NULL){
			//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			msg.flag = true;
			msg.message = "Not fond protocol module";
		}
		return msg;
	}
	//! get reference client side socket
	//! @return			reference client side socket
	boost::asio::ip::tcp::socket& tcp_session::get_client_socket()
	{
		return client_socket.get_socket();
	}
	//! is thread wait
	//! @return 		true is wait
	//! @return 		false is not wait
	bool tcp_session::is_thread_wait(){
		boost::mutex::scoped_lock scope_lock(thread_state_update_mutex);
		bool res = false;
		if(thread_state.test(4) & thread_state.test(5))
			res = true;
		return res;
	}
	//! message from parent virtualservice
	//! @param[in]		message is tcp virtualservice message type
	void tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
		switch(message){
			case SESSION_PAUSE_ON:
				session_pause_flag = true;
				return;
			case SESSION_PAUSE_OFF:
				session_pause_flag = false;
				return;
			case SORRY_STATE_ENABLE:
				break;
			case SORRY_STATE_DISABLE:
				break;
			case SESSION_END:
				break;
		}
		// set message
		boost::shared_ptr< tcp_thread_message > up_msg(new tcp_thread_message);
		std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator up_func;
		up_func = virtual_service_message_up_thread_function_map.find(message);
		if(up_func != virtual_service_message_up_thread_function_map.end()){
			up_msg->message = up_func->second;
			up_thread_message_que.push(up_msg);
		}else{
			//Error illegal virtualservice message type
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] up thread illegal virtualservice message type : ";
			buf << message;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
		}
		boost::shared_ptr< tcp_thread_message > down_msg(new tcp_thread_message);
		std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator down_func;
		down_func = virtual_service_message_down_thread_function_map.find(message);
		if(down_func != virtual_service_message_down_thread_function_map.end()){
			down_msg->message = down_func->second;
			down_thread_message_que.push(down_msg);
		}else{
			//Error illegal virtualservice message type
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] down thread illegal virtualservice message type : ";
			buf << message;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
		}
	}
	//! up stream thread main function
	void tcp_session::up_thread_run(void){
		
		up_thread_id = boost::this_thread::get_id(); 
		thread_state_update(UP_THREAD_ALIVE,true);
		while(!thread_state.test(1)){ // DOWN_THREAD_ALIVE
			//wait down thread get id
		}
		if(protocol_module == NULL){
			//Error!
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
			return;
		}
		boost::system::error_code ec;
		client_socket.accept();
		endpoint cl_end;
		boost::xtime wait_time;
		for(int i = 0;i < 5000;i++){
			cl_end = client_socket.get_socket().remote_endpoint(ec);
			if(!ec) break;
			
			boost::xtime_get(&wait_time, boost::TIME_UTC);
			wait_time.nsec += 1000000;
			boost::thread::sleep(wait_time);
		}
		
		if(ec){
			//client endpoint get Error!
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] client endpoint get false : ";
			buf << ec.message();
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
			client_socket.close(ec);
			return;
		}
		if(!client_socket.set_non_blocking_mode(ec)){
			// socket set nonblocking mode error
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] set non blocking socket error :";
			buf << ec.message();
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
			client_socket.close(ec);
			return;
		}
		boost::asio::ip::udp::endpoint dumy_end;
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_session_initialize(up_thread_id,down_thread_id,cl_end,dumy_end);
		std::map< protocol_module_base::EVENT_TAG , UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func =  up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
			return;
		}
		thread_state_update(UP_THREAD_ACTIVE,true);
		up_thread_next_call_function = func->second;
		while(!exit_flag){
			thread_state_update(UP_THREAD_LOCK,true);
			while(session_pause_flag){
				if(exit_flag) break;
			}
			thread_state_update(UP_THREAD_LOCK,false);
			if(exit_flag) break;
			bool is_msg_none = up_thread_message_que.empty();
			if(!is_msg_none){
				boost::shared_ptr<tcp_thread_message> msg = up_thread_message_que.front();
				up_thread_message_data.set_endpoint(msg->endpoint_info);
				msg->message(MESSAGE_PROC);
			}else{
				up_thread_next_call_function(LOCAL_PROC);
			}
		}
		up_thread_all_socket_close();
		thread_state_update(UP_THREAD_ACTIVE,false);
		while(thread_state.test(1)){ // DOWN_THREAD_ALIVE
			// wait down thread alive
		}
		protocol_module->handle_session_finalize(up_thread_id,down_thread_id);
		thread_state_update(UP_THREAD_ALIVE,false);
		parent_service.release_session(up_thread_id);
	}
	
	//! down stream thread main function
	void tcp_session::down_thread_run(void){
		
		down_thread_id = boost::this_thread::get_id(); 
		thread_state_update(DOWN_THREAD_ALIVE,true);
		while(!thread_state.test(2)){ // UP_THREAD_ACTIVE
			// wait up thread active
			if(exit_flag) break;
		}
		thread_state_update(DOWN_THREAD_ACTIVE,true);
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func =  down_thread_function_map.find(DOWN_FUNC_REALSERVER_RECEIVE);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_RECV";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			exit_flag = true;
		}else{
			down_thread_next_call_function = func->second;
		}
		while(!exit_flag){
			thread_state_update(DOWN_THREAD_LOCK,true);
			while(session_pause_flag){
				if(exit_flag) break;
			}
			thread_state_update(DOWN_THREAD_LOCK,false);
			if(exit_flag) break;
			while(!down_thread_connect_socket_list.empty()){
				std::pair<endpoint,tcp_socket_ptr > push_rs_socket = down_thread_connect_socket_list.get_socket();
				down_thread_receive_realserver_socket_list.push_back(push_rs_socket);
				down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
			}
			bool is_msg_none = down_thread_message_que.empty();
			if(!is_msg_none){
				boost::shared_ptr<tcp_thread_message> msg = down_thread_message_que.front();
				down_thread_message_data.set_endpoint(msg->endpoint_info);
				msg->message(MESSAGE_PROC);
			}else{
				down_thread_next_call_function(LOCAL_PROC);
			}
		}
		down_thread_all_socket_close();
		thread_state_update(DOWN_THREAD_ACTIVE,false);
		thread_state_update(DOWN_THREAD_ALIVE,false);
	}

	//! up and down thread state update
	//! @param[in]		thread_flag is regist or unregist bitset
	//! @param[in]		regist is regist or unregist flag
	void tcp_session::thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){
		boost::mutex::scoped_lock scope_lock(thread_state_update_mutex);
		if(regist){
			thread_state |= thread_flag;
		}else{
			std::bitset<TCP_SESSION_THREAD_STATE_BIT> ret_flag = thread_flag;
			ret_flag.flip();
			thread_state &= ret_flag;
		}
	}
	//! up thread raise module event of handle_accept
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){
		
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		protocol_module_base::EVENT_TAG module_event;
		module_event = protocol_module->handle_accept(up_thread_id);
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread receive client side and raise module event of handle_client_recv
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		unsigned long long qos = parent_service.get_qos_upstream();
		unsigned long long up_throughput = parent_service.get_throughput_upstream();
		if(qos < up_throughput) return; // try again
		up_thread_data_client_side.initialize();
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_client_side.get_data();
		boost::system::error_code ec;
		size_t recv_size = client_socket.read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE), ec);
		up_thread_data_client_side.set_size(recv_size);
		UP_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			if(recv_size > 0){
				parent_service.update_up_recv_size(recv_size);
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_recv(up_thread_id,data_buff,recv_size);
				std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
				if(func_type == up_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;	
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					up_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}else{
				func_tag = UP_FUNC_CLIENT_RECEIVE;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = UP_FUNC_CLIENT_RECEIVE;
			}else if(ec == boost::asio::error::eof){
				func_tag = UP_FUNC_CLIENT_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = UP_FUNC_CLIENT_DISCONNECT;
			}else{
				func_tag = UP_FUNC_CLIENT_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_tag);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise client respond send event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){
		boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
		boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = up_thread_function_map.find(UP_FUNC_CLIENT_RESPOND_SEND_EVENT);
		if(up_func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_RESPOND_SEND_EVENT";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_msg->message = up_func->second;
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
		if(down_func == up_thread_message_down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		down_msg->message = down_func->second;
		up_thread_message_que.push(up_msg);
		down_thread_message_que.push(down_msg);
	}
	//! up thread raise module event of handle_response_send_inform
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_response_send_inform_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_response_send_inform(up_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread close client socket and raise client disconnect event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::system::error_code ec;
		bool bres = client_socket.close(ec);
		if(bres){
			boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
			boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT_EVENT);
			if(up_func == up_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			up_msg->message = up_func->second;
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
			if(down_func == up_thread_message_down_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			down_msg->message = down_func->second;
			up_thread_message_que.push(up_msg);
			down_thread_message_que.push(down_msg);
		}
	}
	//! up thread raise module event of handle_client_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_client_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_client_disconnect(up_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread send realserver and raise module event of handle_client_recv
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		std::map<endpoint,tcp_socket_ptr>::iterator send_socket =  up_thread_send_realserver_socket_map.find(server_endpoint);
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
		std::size_t data_size = up_thread_data_dest_side.get_size();
		std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
		std::size_t send_size = send_socket->second->write_some(boost::asio::buffer(&data_buff+send_data_size,data_size-send_data_size),ec);
		UP_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			send_data_size += send_size;
			up_thread_data_dest_side.set_send_size(send_data_size);
			parent_service.update_up_send_size(send_size);
			if(data_size > send_data_size){
				func_tag = UP_FUNC_REALSERVER_SEND;
			}else{
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_send(up_thread_id);
				std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
				if(func_type == up_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;	
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					up_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = UP_FUNC_REALSERVER_SEND;
			}else if(ec == boost::asio::error::eof){
				func_tag = UP_FUNC_REALSERVER_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = UP_FUNC_REALSERVER_DISCONNECT;
			}else{
				func_tag = UP_FUNC_REALSERVER_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_tag);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_realserver_select
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint server_endpoint;
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_select(up_thread_id,server_endpoint);
		up_thread_data_dest_side.set_endpoint(server_endpoint);
		
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread connect realserver
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		std::map<endpoint,tcp_socket_ptr>::iterator get_socket =  up_thread_send_realserver_socket_map.find(server_endpoint);
		std::map<endpoint,tcp_socket_ptr>::iterator map_end =  up_thread_send_realserver_socket_map.end();
		UP_THREAD_FUNC_TYPE_TAG func_tag;
		if(get_socket != map_end){
			func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
		}else{
			tcp_socket_ptr new_socket(new tcp_socket(io));
			boost::system::error_code ec;
			bool bres = new_socket->connect(server_endpoint,ec);
			if(bres){
				parent_service.connection_active(server_endpoint);
				if(!new_socket->set_non_blocking_mode(ec)){
					// socket set nonblocking mode error
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] set non blocking socket error :";
					buf << ec.message();
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					up_thread_exit(process_type);
				}
				socket_element push_element;
				push_element.first = server_endpoint;
				push_element.second = new_socket;
				up_thread_send_realserver_socket_map.insert(push_element);
				down_thread_connect_socket_list.push_back(push_element);
				func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
			}else{
				func_tag = UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT;
				//connect socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] connect socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_tag);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_realserver_connect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		up_thread_data_dest_side.initialize();
		boost::array< char , MAX_BUFFER_SIZE >& data_buff = up_thread_data_dest_side.get_data();
		size_t data_size = 0;
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connect(up_thread_id,data_buff,data_size);
		up_thread_data_dest_side.set_endpoint(server_endpoint);
		up_thread_data_dest_side.set_size(data_size);
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_realserver_connection_fail
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connection_fail(up_thread_id,server_endpoint);
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread close realserver socket and raise realserver disconnect event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.find(server_endpoint);
		boost::system::error_code ec;
		bool bres = close_socket->second->close(ec);
		if(bres){
			parent_service.connection_inactive(server_endpoint);
			boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
			boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT_EVENT);
			if(up_func == up_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_REALSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			up_msg->endpoint_info = server_endpoint;
			up_msg->message = up_func->second;
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
			if(down_func == up_thread_message_down_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			down_msg->endpoint_info = server_endpoint;
			down_msg->message = down_func->second;
			up_thread_message_que.push(up_msg);
			down_thread_message_que.push(down_msg);
		}
	}
	//! up thread raise module event of handle_client_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		boost::asio::ip::tcp::endpoint server_endpoint = up_thread_message_data.get_endpoint();
		{
			boost::mutex::scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_realserver_disconnect(up_thread_id,server_endpoint);
		}
		up_thread_send_realserver_socket_map.erase(server_endpoint);
		
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread close all realserver socket and raise module event of handle_realserver_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
					//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
		std::map<endpoint,tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();
		
		protocol_module_base::EVENT_TAG module_event;
		bool realserver_fond = false;
		boost::asio::ip::tcp::endpoint server_endpoint;
		while(close_socket != list_end){
			realserver_fond = true;
			boost::system::error_code ec;
			bool bres = close_socket->second->close(ec);
			server_endpoint = close_socket->first;
			if(bres){
				parent_service.connection_inactive(server_endpoint);
			}
			{
				boost::mutex::scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
				module_event = protocol_module->handle_realserver_disconnect(up_thread_id,server_endpoint);
			}
			close_socket++;
		}
		if(!realserver_fond){
			//Error not fond realserver
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] close realserver not fond ";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_send_realserver_socket_map.clear();
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread send sorryserver and raise module event of handle_sorryserver_send
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
			//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
		std::size_t data_size = up_thread_data_dest_side.get_size();
		std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
		std::size_t send_size = sorryserver_socket.second->write_some(boost::asio::buffer(&data_buff+send_data_size,data_size-send_data_size),ec);
		UP_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			send_data_size += send_size;
			up_thread_data_dest_side.set_send_size(send_data_size);
			if(data_size > send_data_size){
				func_tag = UP_FUNC_SORRYSERVER_SEND;
			}else{
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_send(up_thread_id);
				std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
				if(func_type == up_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					up_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = UP_FUNC_SORRYSERVER_SEND;
			}else if(ec == boost::asio::error::eof){
				func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
			}else{
				func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_tag);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_sorryserver_select
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint server_endpoint;
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_select(up_thread_id,server_endpoint);
		up_thread_data_dest_side.set_endpoint(server_endpoint);
		
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread connect sorryserver
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
		boost::system::error_code ec;
		bool bres = sorryserver_socket.second->connect(sorry_endpoint,ec);
		UP_THREAD_FUNC_TYPE_TAG func_tag;
		if(bres){
			if(!sorryserver_socket.second->set_non_blocking_mode(ec)){
					// socket set nonblocking mode error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] set non blocking socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
			}
			sorryserver_socket.first = sorry_endpoint;
			func_tag = UP_FUNC_SORRYSERVER_CONNECT_EVENT;
		}else{
			func_tag = UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT;
			//connect socket error
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] connect socket error :";
			buf << ec.message();
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_tag);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_sorryserver_connect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
		up_thread_data_dest_side.initialize();
		boost::array< char , MAX_BUFFER_SIZE >& data_buff = up_thread_data_dest_side.get_data();
		size_t data_size = 0;
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connect(up_thread_id,data_buff,data_size);
		up_thread_data_dest_side.set_endpoint(sorry_endpoint);
		up_thread_data_dest_side.set_size(data_size);
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_sorryserver_connection_fail
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connection_fail(up_thread_id,server_endpoint);
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::system::error_code ec;
		bool bres = sorryserver_socket.second->close(ec);
		if(bres){
			boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
			boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			if(up_func == up_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_SORRYSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			up_msg->message = up_func->second;
			up_msg->endpoint_info = sorryserver_socket.first;
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			if(down_func == up_thread_message_down_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			down_msg->message = down_func->second;
			down_msg->endpoint_info = sorryserver_socket.first;
			up_thread_message_que.push(up_msg);
			down_thread_message_que.push(down_msg);
		}
	}
	
	//! up thread raise module event of handle_sorryserver_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_message_data.get_endpoint();
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id,sorry_endpoint);
		}
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_sorry_enable
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorry_enable_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorry_enable(up_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	//! up thread raise module event of handle_sorry_disable
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorry_disable_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				up_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorry_disable(up_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
		if(func_type == up_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = up_thread_function_map.find(func_type->second);
		if(func == up_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			up_thread_exit(process_type);
			return;
		}
		up_thread_next_call_function = func->second;
	}
	
	//! up thread exit main loop
	//! @param[in]		process_type is prosecess type
	void tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
		boost::mutex::scoped_lock scope_lock(exit_flag_update_mutex);
		exit_flag = true;
	}
	//! up thread close all socket
	void tcp_session::up_thread_all_socket_close(void){
		std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
		std::map<endpoint,tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();
		boost::system::error_code ec;
		while(close_socket != list_end){
			bool bres = close_socket->second->close(ec);
			if(bres)
				parent_service.connection_inactive(close_socket->first);
			close_socket++;
		}
		up_thread_send_realserver_socket_map.clear();
		down_thread_connect_socket_list.clear();
		client_socket.close(ec);
		sorryserver_socket.second->close(ec);
	}

	//! down thread receive from realserver and raise module event of handle_realserver_recv
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		bool is_emp = down_thread_receive_realserver_socket_list.empty();
		if(is_emp)return;
		unsigned long long qos = parent_service.get_qos_downstream();
		unsigned long long down_throughput = parent_service.get_throughput_downstream();
		if(qos < down_throughput) return;
		down_thread_data_dest_side.initialize();
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
		boost::system::error_code ec;
		size_t recv_size = down_thread_current_receive_realserver_socket->second->read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE),ec);
		down_thread_data_dest_side.set_size(recv_size);
		boost::asio::ip::tcp::endpoint server_endpoint = down_thread_current_receive_realserver_socket->first;
		down_thread_data_dest_side.set_endpoint(server_endpoint);
		DOWN_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			if(recv_size > 0){
				parent_service.update_down_recv_size(recv_size);
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_recv(down_thread_id,server_endpoint,data_buff,recv_size);
				std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
				if(func_type == down_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;	
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					down_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}else{
				func_tag = DOWN_FUNC_REALSERVER_RECEIVE;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = DOWN_FUNC_REALSERVER_RECEIVE;
			}else if(ec == boost::asio::error::eof){
				func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
			}else{
				func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_tag);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_current_receive_realserver_socket++;
		std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
		if(down_thread_current_receive_realserver_socket == list_end)
			down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
		down_thread_next_call_function = func->second;
	}
	//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::asio::ip::tcp::endpoint server_endpoint = down_thread_data_dest_side.get_endpoint();
		std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
		std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
		while(list_end != close_socket){
			boost::asio::ip::tcp::endpoint close_endpoint = close_socket->first;
			if(close_endpoint == server_endpoint){
				boost::system::error_code ec;
				bool bres = close_socket->second->close(ec);
				if(bres){
					parent_service.connection_inactive(server_endpoint);
					boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
					boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
					std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
					if(down_func == down_thread_function_map.end()){
						//Error not find function map
						std::stringstream buf;
						buf << "Thread ID[";
						buf << boost::this_thread::get_id();
						buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_DISCONNECT_EVENT";
						Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
						down_thread_exit(process_type);
						return;
					}
					down_msg->endpoint_info = server_endpoint;
					down_msg->message = down_func->second;
					std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT_EVENT);
					if(up_func == down_thread_message_up_thread_function_map.end()){
						//Error not find function map
						std::stringstream buf;
						buf << "Thread ID[";
						buf << boost::this_thread::get_id();
						buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : UP_FUNC_REALSERVER_DISCONNECT_EVENT";
						Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
						down_thread_exit(process_type);
						return;
					}
					up_msg->endpoint_info = server_endpoint;
					up_msg->message = up_func->second;
					up_thread_message_que.push(up_msg);
					down_thread_message_que.push(down_msg);
				}
				break;
			}
			close_socket++;
		}
	}
	
	//! down thread raise module event of handle_realserver_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		boost::asio::ip::tcp::endpoint server_endpoint = down_thread_message_data.get_endpoint();
		{
			boost::mutex::scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_realserver_disconnect(down_thread_id,server_endpoint);
		}
		
		std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
		std::list<socket_element>::iterator erase_socket = down_thread_receive_realserver_socket_list.begin();
		while(erase_socket != list_end){
			if(erase_socket->first == server_endpoint){
				down_thread_receive_realserver_socket_list.erase(erase_socket);
				down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
				break;
			}
			erase_socket++;
		}
		
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
					//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
		std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
		
		protocol_module_base::EVENT_TAG module_event;
		boost::asio::ip::tcp::endpoint server_endpoint;
		bool realserver_fond = false;
		while(close_socket != list_end){
			realserver_fond = true;
			boost::system::error_code ec;
			bool bres = close_socket->second->close(ec);
			server_endpoint = close_socket->first;
			if(bres)
				parent_service.connection_inactive(server_endpoint);
			{
				boost::mutex::scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
				module_event = protocol_module->handle_realserver_disconnect(down_thread_id,server_endpoint);
			}
			
			close_socket++;
		}
		if(!realserver_fond){
			//Error not fond realserver
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] close realserver not fond ";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_receive_realserver_socket_list.clear();
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread raise module event of handle_client_connection_check
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
					//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_data_client_side.initialize();
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
		std::size_t data_size;
		protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_connection_check(down_thread_id,data_buff,data_size);
		down_thread_data_client_side.set_size(data_size);
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	
	//! down thread raise module event of handle_response_send_inform
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_response_send_inform_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_response_send_inform(down_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread send for client and raise module event of handle_client_send
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
			//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		boost::system::error_code ec;
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
		std::size_t data_size = down_thread_data_client_side.get_size();
		std::size_t send_data_size = down_thread_data_client_side.get_send_size();
		std::size_t send_size = client_socket.write_some(boost::asio::buffer(&data_buff+send_data_size,data_size-send_data_size),ec);
		DOWN_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			send_data_size += send_size;
			down_thread_data_client_side.set_send_size(send_data_size);
			parent_service.update_down_send_size(send_size);
			if(data_size > send_data_size){
				func_tag = DOWN_FUNC_CLIENT_SEND;
			}else{
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_send(down_thread_id);
				std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
				if(func_type == down_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					down_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = DOWN_FUNC_CLIENT_SEND;
			}else if(ec == boost::asio::error::eof){
				func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
			}else{
				func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_tag);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread close client socket and raise client disconnect event message for up and down thread 
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::system::error_code ec;
		bool bres = client_socket.close(ec);
		if(bres){
			boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
			boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = down_thread_function_map.find(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
			if(down_func == down_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			down_msg->message = down_func->second;
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT_EVENT);
			if(up_func == down_thread_message_up_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			up_msg->message = up_func->second;
			down_thread_message_que.push(down_msg);
			up_thread_message_que.push(up_msg);
		}
	}
	
	//! down thread raise module event of handle_client_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_client_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_client_disconnect(down_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
		if(protocol_module == NULL){
				//Error protocol_module NULL
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module is NULL!";
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_data_dest_side.initialize();
		boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
		boost::system::error_code ec;
		size_t recv_size = sorryserver_socket.second->read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE),ec);
		down_thread_data_dest_side.set_size(recv_size);
		boost::asio::ip::tcp::endpoint sorry_endpoint = sorryserver_socket.first;
		down_thread_data_dest_side.set_endpoint(sorry_endpoint);
		DOWN_THREAD_FUNC_TYPE_TAG func_tag;
		if(!ec){
			if(recv_size > 0){
				protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_recv(down_thread_id,sorry_endpoint,data_buff,recv_size);
				std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
				if(func_type == down_thread_module_event_map.end()){
					//Error unknown protocol_module_base::EVENT_TAG return
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] protocol_module returnd illegal EVENT_TAG : ";
					buf << module_event;	
					Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
					down_thread_exit(process_type);
					return;
				}
				func_tag = func_type->second;
			}else{
				func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
			}
		}else{
			if(ec == boost::asio::error::try_again){
				func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
			}else if(ec == boost::asio::error::eof){
				func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
			}else if(ec == boost::asio::error::connection_reset){
				func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
			}else{
				func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
				//receive socket error
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] receive socket error :";
				buf << ec.message();
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_tag);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_tag;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread raise module event of handle_sorryserver_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
		boost::system::error_code ec;
		bool bres = sorryserver_socket.second->close(ec);
		if(bres){
			boost::shared_ptr<tcp_thread_message> up_msg(new tcp_thread_message);
			boost::shared_ptr<tcp_thread_message> down_msg(new tcp_thread_message);
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			if(down_func == down_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			down_msg->message = down_func->second;
			down_msg->endpoint_info = sorryserver_socket.first;
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			if(up_func == down_thread_message_up_thread_function_map.end()){
				//Error not find function map
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_SORRYSERVER_DISCONNECT_EVENT";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			up_msg->message = up_func->second;
			up_msg->endpoint_info = sorryserver_socket.first;
			up_thread_message_que.push(up_msg);
			down_thread_message_que.push(down_msg);
		}
	}
	//! down thread raise module event of handle_sorryserver_disconnect
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		boost::asio::ip::tcp::endpoint sorry_endpoint = down_thread_message_data.get_endpoint();
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id,sorry_endpoint);
		}
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread raise module event of handle_sorry_enable
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorry_enable_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorry_enable(down_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread raise module event of handle_sorry_disable
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
		protocol_module_base::EVENT_TAG module_event;
		{
			boost::mutex::scoped_lock scope_lock(module_function_sorry_disable_mutex);
			if(protocol_module == NULL){
				//Error protocol_module NULL
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] protocol_module is NULL!";
				Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				down_thread_exit(process_type);
				return;
			}
			module_event = protocol_module->handle_sorry_disable(down_thread_id);
		}
		std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
		if(func_type == down_thread_module_event_map.end()){
			//Error unknown protocol_module_base::EVENT_TAG return
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] protocol_module returnd illegal EVENT_TAG : ";
			buf << module_event;	
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator func = down_thread_function_map.find(func_type->second);
		if(func == down_thread_function_map.end()){
			//Error not find function map
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
			buf << func_type->second;
			Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			down_thread_exit(process_type);
			return;
		}
		down_thread_next_call_function = func->second;
	}
	//! down thread exit main loop
	//! @param[in]		process_type is prosecess type
	void tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
		boost::mutex::scoped_lock scope_lock(exit_flag_update_mutex);
		exit_flag = true;
	}
	//! down thread close all socket
	void tcp_session::down_thread_all_socket_close(void){
		std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
		std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
		boost::system::error_code ec;
		while(close_socket != list_end){
			bool bres = close_socket->second->close(ec);
			if(bres)
				parent_service.connection_inactive(close_socket->first);
			close_socket++;
		}
		down_thread_receive_realserver_socket_list.clear();
		client_socket.close(ec);
		sorryserver_socket.second->close(ec);
	}
	
}// namespace l7vs
