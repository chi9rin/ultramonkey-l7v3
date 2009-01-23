
#include "udp_session.h"
#include "virtualservice.h"

namespace l7vs{

	udp_session::udp_session(virtual_service_udp& vs, boost::asio::io_service& session_io) : 
		io(session_io),
		parent_service(vs),
		client_side_socket(session_io){

	}
	udp_session::~udp_session(){

	}

	session_result_message udp_session::initialize(const udp_endpoint listen_end){
		session_result_message res;
		return res;
	}

	void udp_session::set_virtual_service_message(UDP_VIRTUAL_SERVICE_MESSAGE_TAG message){
	}

	void udp_session::run(void){
	}

	bool udp_session::client_send(const udp_endpoint client_endpoint,const udp_session_buff& data_buff,const std::size_t data_size,boost::system::error_code& ec){
		return true;
	}

	void udp_session::release_request(const boost::thread::id release_thread_id){
	}

	void udp_session::make_request_thread(const int max_count){
	}

	void udp_session::active_request_thread(const udp_endpoint client_endpoint , const udp_session_buff receive_data,const std::size_t receive_size){
	}

	void udp_session::all_active_request_stop(void){
	}

}// namespace l7vs
