/*!
 *	@file	tcp_thread_message_que.cpp
 *	@brief	tcp session thread message queue class
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

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
		rw_scoped_lock scope_lock(que_mutex);
		
		message_que.push(message);
	}

	//! front queue
	tcp_thread_message_que::tcp_thread_message_ptr tcp_thread_message_que::front(){
		rw_scoped_lock scope_lock(que_mutex);

		tcp_thread_message_ptr res;
		if(!message_que.empty()){
			res= message_que.front();
			message_que.pop();
		}
		
		return res;
	}

	//! queue check empty
	bool tcp_thread_message_que::empty(){
		return message_que.empty();
	}

	//! clear queue
	void tcp_thread_message_que::clear(){
		rw_scoped_lock scope_lock(que_mutex);
		
		while(message_que.empty() == false)
			message_que.pop();	
	
	}	
	
}// namespace l7vs
