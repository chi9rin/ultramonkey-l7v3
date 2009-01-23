 
#include "tcp_thread_message_que.h"
#include "tcp_thread_message.h"
 
namespace l7vs{

	tcp_thread_message_que::tcp_thread_message_que(){
	}
	tcp_thread_message_que::~tcp_thread_message_que(){
	}
	
	void tcp_thread_message_que::push(tcp_thread_message_ptr message){
	}
	tcp_thread_message_que::tcp_thread_message_ptr tcp_thread_message_que::front(){
		tcp_thread_message_ptr res;
		return res;
	}
	bool tcp_thread_message_que::empty(){
		return true;
	}
	void tcp_thread_message_que::clear(){
		
	}	
	
}// namespace l7vs
