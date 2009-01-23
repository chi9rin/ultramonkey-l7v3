
#include "udp_request_thread_control.h"

namespace l7vs{

	udp_request_thread_control::udp_request_thread_control(request_ptr request) : 
		my_request(request){
	}

	udp_request_thread_control::~udp_request_thread_control(void){
	}

	void udp_request_thread_control::start(void){
	}

	void udp_request_thread_control::stop(void){
	}

	void udp_request_thread_control::join(void){
	}

	boost::thread::id udp_request_thread_control::get_thread_id(){
		return request_thread->get_id();
	}

	boost::shared_ptr< l7vs::udp_request > udp_request_thread_control::get_request(){
		return my_request;
	}

	void udp_request_thread_control::run(void){
	}

}// namespace l7vs
