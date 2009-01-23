 
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

	class virtual_service_tcp;

	class tcp_session : private boost::noncopyable{
		public:
			enum TCP_PROCESS_TYPE_TAG{
				LOCAL_PROC = 0,
				MESSAGE_PROC
			};
			enum TCP_VIRTUAL_SERVICE_MESSAGE_TAG{
				SORRY_STATE_ENABLE = 0,
				SORRY_STATE_DISABLE,
				SESSION_END,
				SESSION_PAUSE_ON,
				SESSION_PAUSE_OFF
			};
			
			tcp_session(virtual_service_tcp& vs,boost::asio::io_service& session_io);
			~tcp_session(void);
			session_result_message initialize(void);
			boost::asio::ip::tcp::socket& get_client_socket(void);
			bool is_thread_wait(void);
			void set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message);
			void up_thread_run(void);
			void down_thread_run(void);
			
		protected:
			typedef boost::asio::ip::tcp::endpoint endpoint;
			typedef boost::shared_ptr< tcp_socket > tcp_socket_ptr;
			typedef std::pair< endpoint, tcp_socket_ptr> socket_element;
			typedef boost::function< void(TCP_PROCESS_TYPE_TAG) > tcp_session_func;
			enum UP_THREAD_FUNC_TYPE_TAG{
				UP_FUNC_CLIENT_ACCEPT = 0,
				UP_FUNC_CLIENT_DISCONNECT,
				UP_FUNC_CLIENT_DISCONNECT_EVENT,
				UP_FUNC_CLIENT_RECEIVE,
				UP_FUNC_CLIENT_RESPOND_SEND,
				UP_FUNC_CLIENT_RESPOND_SEND_EVENT,
				UP_FUNC_REALSERVER_GET_DEST_EVENT,
				UP_FUNC_REALSERVER_CONNECT,
				UP_FUNC_REALSERVER_CONNECT_EVENT,
				UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT,
				UP_FUNC_REALSERVER_SEND,
				UP_FUNC_REALSERVER_DISCONNECT,
				UP_FUNC_REALSERVER_DISCONNECT_EVENT,
				UP_FUNC_REALSERVER_ALL_DISCONNECT,
				UP_FUNC_SORRYSERVER_GET_DEST,
				UP_FUNC_SORRYSERVER_CONNECT,
				UP_FUNC_SORRYSERVER_CONNECT_EVENT,
				UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT,
				UP_FUNC_SORRYSERVER_SEND,
				UP_FUNC_SORRYSERVER_DISCONNECT,
				UP_FUNC_SORRYSERVER_DISCONNECT_EVENT,
				UP_FUNC_SORRY_ENABLE_EVENT,
				UP_FUNC_SORRY_DISABLE_EVENT,
				UP_FUNC_EXIT
			};
			enum DOWN_THREAD_FUNC_TYPE_TAG{
				DOWN_FUNC_CLIENT_DISCONNECT = 0,
				DOWN_FUNC_CLIENT_DISCONNECT_EVENT,
				DOWN_FUNC_CLIENT_CONNECTION_CHK,
				DOWN_FUNC_CLIENT_SEND,
				DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT,
				DOWN_FUNC_REALSERVER_RECEIVE,
				DOWN_FUNC_REALSERVER_DISCONNECT,
				DOWN_FUNC_REALSERVER_DISCONNECT_EVENT,
				DOWN_FUNC_REALSERVER_ALL_DISCONNECT,
				DOWN_FUNC_SORRYSERVER_RECEIVE,
				DOWN_FUNC_SORRYSERVER_DISCONNECT,
				DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT,
				DOWN_FUNC_SORRY_ENABLE_EVENT,
				DOWN_FUNC_SORRY_DISABLE_EVENT,
				DOWN_FUNC_EXIT
			};
			boost::asio::io_service& io;
			virtual_service_tcp& parent_service;
			bool exit_flag;
			boost::mutex exit_flag_update_mutex;
			std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_state;
			boost::mutex thread_state_update_mutex;
			boost::thread::id up_thread_id;
			boost::thread::id down_thread_id;
			protocol_module_base* protocol_module;
			bool session_pause_flag;
			tcp_socket client_socket;
			socket_element sorryserver_socket;
			std::map<endpoint,tcp_socket_ptr> up_thread_send_realserver_socket_map;
			tcp_realserver_connect_socket_list down_thread_connect_socket_list;
			std::list<socket_element> down_thread_receive_realserver_socket_list;
			std::list<socket_element>::iterator down_thread_current_receive_realserver_socket;
			boost::mutex module_function_response_send_inform_mutex;
			boost::mutex module_function_client_disconnect_mutex;
			boost::mutex module_function_realserver_disconnect_mutex;
			boost::mutex module_function_sorryserver_disconnect_mutex;
			boost::mutex module_function_sorry_enable_mutex;
			boost::mutex module_function_sorry_disable_mutex;
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func > up_thread_function_map;
			std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG > up_thread_module_event_map;
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func > up_thread_message_down_thread_function_map;
			tcp_session_func up_thread_next_call_function;
			tcp_data up_thread_data_client_side;
			tcp_data up_thread_data_dest_side;
			tcp_thread_message_que up_thread_message_que;
			tcp_data up_thread_message_data;

			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func > down_thread_function_map;
			std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG > down_thread_module_event_map;
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func > down_thread_message_up_thread_function_map;
			tcp_session_func down_thread_next_call_function;
			tcp_data down_thread_data_client_side;
			tcp_data down_thread_data_dest_side;
			tcp_thread_message_que down_thread_message_que;
			tcp_data down_thread_message_data;
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>  virtual_service_message_up_thread_function_map;
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>  virtual_service_message_down_thread_function_map;


			void thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist);

			void up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_get_distination_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
			void up_thread_all_socket_close(void);

			void down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
			void down_thread_all_socket_close(void);

	};// class tcp_session
}// namespace l7vs

#endif//TCP_SESSION_H
 

