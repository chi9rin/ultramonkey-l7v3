
#include "tcp_session.h"
#include "virtualservice.h"

namespace l7vs{

	tcp_session::tcp_session(
		virtual_service_tcp& vs,
		boost::asio::io_service& session_io):
		io(session_io),
		parent_service(vs),
		client_socket(session_io){
		
			tcp_socket_ptr sorry_socket(new tcp_socket(session_io));
			sorryserver_socket.second = sorry_socket;
	}
	
	tcp_session::~tcp_session(){
	}
	session_result_message initialize(){
		session_result_message msg;
		return msg;
	}
	
	boost::asio::ip::tcp::socket& tcp_session::get_client_socket()
	{
		return client_socket.get_socket();
	}
	
	bool tcp_session::is_thread_wait()
	{
		return true;
	}
	
	void tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
	}
	
	void tcp_session::up_thread_run(void){
	}
	
	void tcp_session::down_thread_run(void){
	}

	void tcp_session::thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){
	}
	
	void tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_get_distination_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::up_thread_all_socket_close(void){
	}

	void tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
	}
	
	void tcp_session::down_thread_all_socket_close(void){
	}
	
}// namespace l7vs
