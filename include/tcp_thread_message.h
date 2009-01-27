//
//!	@file	tcp_thread_message.h
//!	@brief	tcp session thread message class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef TCP_THREAD_MESSAGE_H
#define TCP_THREAD_MESSAGE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "tcp_session.h"

namespace l7vs{

//!	@class	tcp_thread_message
//! @brief	this class is notify message in tcp session thread.
	class tcp_thread_message : private boost::noncopyable{
		public:
			//! construcor 
			tcp_thread_message();
			//! destructor
			~tcp_thread_message();
			
			//! request call function object
			boost::function<void(tcp_session::TCP_PROCESS_TYPE_TAG)> message;
			//! function require endpoint 
			boost::asio::ip::tcp::endpoint endpoint_info;
			
	};// class tcp_thread_message
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
