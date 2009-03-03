//
//!	@file	tcp_socket.h
//!	@brief	tcp session socket class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

namespace l7vs{

//!	@class	tcp_socket
//! @brief	this class is tcp session object use socket.
	class tcp_socket : private boost::noncopyable{
		public:

			//! construcor
			//! @param[in/out]	socket use io service object
			tcp_socket(boost::asio::io_service& io);
			//! destructor
			~tcp_socket();
			//! get reference control socket
			//! @return			reference control socket
			boost::asio::ip::tcp::socket& get_socket();
			//! connect socket
			//! @param[in]		connect_endpoint is connection endpoint
			//! @param[out]		ec is reference error code object
			bool connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec);
			//! accept
			void accept();
			//! close socket
			//! @param[out]		ec is reference error code object
			//! @return 		true is socket close
			//! @return 		false is not open socket
			bool close(boost::system::error_code& ec);
			//! set non blocking mode of the socket 
			//! @return			ec is reference error code object
			bool set_non_blocking_mode(boost::system::error_code& ec);
			//! write socket
			//! @param[in]		buffers is wite data buffer
			//! @param[out]		ec is reference error code object
			//! @return			write data size
			std::size_t write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);
			//! read socket
			//! @param[out]		buffers is read data buffer
			//! @param[out]		ec is reference error code object
			//! @return			read data size
			std::size_t read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);
			//! is open
			//! @return 		true is open
			//! @return 		false is close
			bool is_open();

		protected:
			//! control socket
			boost::asio::ip::tcp::socket my_socket;
			//! socket close mutex
			boost::mutex read_mutex;
			boost::mutex write_mutex;
			//! socket open flag
			bool open_flag;

	};// class tcp_socket
}// namespace l7vs

#endif//TCP_SOCKET_H
 
