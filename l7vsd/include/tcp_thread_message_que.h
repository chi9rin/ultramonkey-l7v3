/*!
 *	@file	tcp_thread_message_que.h
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

#ifndef TCP_THREAD_MESSAGE_QUE_H
#define TCP_THREAD_MESSAGE_QUE_H

#include <cstdlib>
#include <queue>
#include <boost/thread/mutex.hpp>

#include "wrlock.h"

namespace l7vs{
//!	@class	tcp_thread_message
//! @brief	tcp_thread_message class name define.
	class tcp_thread_message;
	
//!	@class	data_buff_base
//! @brief	this class is tcp_thread_message object queue.
	class tcp_thread_message_que : private boost::noncopyable{
		public:
			//!	queue message type
			typedef boost::shared_ptr<tcp_thread_message> tcp_thread_message_ptr;
			
			//! construcor
			tcp_thread_message_que();
			//! destructor
			~tcp_thread_message_que();
			//! push queue
			//! @param[in]	push message
			void push(tcp_thread_message_ptr message);
			//! front queue
			tcp_thread_message_ptr front();
			//! queue check empty
			bool empty();
			//! clear queue
			void clear();
		protected:
			//! message queue
			std::queue< tcp_thread_message_ptr > message_que;
			//! queue access mutex
			wr_mutex que_mutex;
			
	};// class tcp_thread_message_que
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
