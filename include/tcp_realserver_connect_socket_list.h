//
//!	@file	tcp_realserver_connect_socket_list.h
//!	@brief	tcp session connect realserver socket list class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef TCP_REALSERVER_CONNECT_SOCKET_LIST_H
#define TCP_REALSERVER_CONNECT_SOCKET_LIST_H

#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "tcp_socket.h"

namespace l7vs{

//!	@class	tcp_realserver_connect_socket_list
//! @brief	this class is connected realsever list by up thread.
	class tcp_realserver_connect_socket_list : private boost::noncopyable{
		public:
			//!	socket pointer type
			typedef boost::shared_ptr<tcp_socket> tcp_socket_ptr;
			//! realserver list element type first : connection endpoint second : socket pointer
			typedef std::pair<boost::asio::ip::tcp::endpoint,tcp_socket_ptr > list_element;
			
			//! construcor
			tcp_realserver_connect_socket_list();
			//! destructor
			~tcp_realserver_connect_socket_list();
			//! push back list
			//! @param[in/out]	realserver_socket is realserver list push back element 
			void push_back(list_element realserver_socket);
			//! get socket list
			//! @return			realserver list element
			list_element get_socket();
			//! check empty list
			bool empty();
			//! clear list
			void clear();
			
		protected:
			//! realserver list
			std::list< list_element > connect_list;
			//! list access mutex
			boost::mutex list_mutex;
	};// class tcp_realserver_connect_socket_list
}// namespace l7vs

#endif//TCP_REALSERVER_CONNECT_SOCKET_LIST_H
 

