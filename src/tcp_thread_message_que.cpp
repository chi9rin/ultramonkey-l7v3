//
//!	@file	tcp_thread_message_que.cpp
//!	@brief	tcp session thread message queue class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "tcp_thread_message_que.h"
#include "tcp_thread_message.h"
#include "logger.h"
 
namespace l7vs{

	//! construcor
	tcp_thread_message_que::tcp_thread_message_que(){
	}

	//! destructor
	tcp_thread_message_que::~tcp_thread_message_que(){
	}

	//! push queue
	//! @param[in]	push message
	void tcp_thread_message_que::push(tcp_thread_message_ptr message){
		boost::mutex::scoped_lock scope_lock(que_mutex);
		
		message_que.push(message);
	}

	//! front queue
	tcp_thread_message_que::tcp_thread_message_ptr tcp_thread_message_que::front(){
		boost::mutex::scoped_lock scope_lock(que_mutex);

		tcp_thread_message_ptr res;
		if(!message_que.empty()){
			res= message_que.front();
			message_que.pop();
		}
		
		return res;
	}

	//! queue check empty
	bool tcp_thread_message_que::empty(){
		boost::mutex::scoped_lock scope_lock(que_mutex);
		
		bool bres = message_que.empty();
		
		return bres;
	}

	//! clear queue
	void tcp_thread_message_que::clear(){
		boost::mutex::scoped_lock scope_lock(que_mutex);
		
		while(message_que.empty() == false)
			message_que.pop();	
	
	}	
	
}// namespace l7vs
