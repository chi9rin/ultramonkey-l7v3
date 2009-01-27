//
//!	@file	tcp_thread_message_que.h
//!	@brief	tcp session thread message queue class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef TCP_THREAD_MESSAGE_QUE_H
#define TCP_THREAD_MESSAGE_QUE_H

#include <cstdlib>
#include <queue>
#include <boost/thread/mutex.hpp>

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
			boost::mutex que_mutex;
			
	};// class tcp_thread_message_que
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
