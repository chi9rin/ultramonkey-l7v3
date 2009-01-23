#include "udp_request.h"
#include "virtualservice.h"
#include "udp_session.h"

namespace l7vs{

	udp_request::udp_request(l7vs::virtual_service_udp& vs,l7vs::udp_session& session,boost::asio::io_service& request_io) : 
		io(request_io),
		parent_service(vs),
		parent_session(session),
		realserver_socket(request_io){

	}

	udp_request::~udp_request(){


	}

	void udp_request::initialize(boost::asio::ip::udp::endpoint client_endpoint , boost::array<char , MAX_BUFFER_SIZE>& data,std::size_t data_size){
	}

	void udp_request::run(void){
	}

	void udp_request::stop(void){
	}

	void udp_request::client_receive_event(void){
	}

	void udp_request::realserver_get_destination_event(void){
	}

	void udp_request::realserver_send(void){
	}

	void udp_request::realserver_receive(void){
	}

	void udp_request::client_get_destination_event(void){
	}

	void udp_request::client_send(void){
	}

	void udp_request::realserver_close(void){
	}

	void udp_request::realserver_close_event(void){
	}

	void udp_request::thread_exit(void){
	}

 }// namespace l7vs
