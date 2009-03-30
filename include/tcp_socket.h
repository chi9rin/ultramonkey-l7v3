/*!
 *	@file	tcp_socket.h
 *	@brief	tcp session socket class
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

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "wrlock.h"

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
            wr_mutex close_mutex;
			//! socket open flag
			bool open_flag;

	};// class tcp_socket
}// namespace l7vs

#endif//TCP_SOCKET_H
 
