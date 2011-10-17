/*!
 *    @file    basic_tcp_socket.h
 *    @brief    tcp session basic socket class
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

#ifndef	BASIC_TCP_SOCKET_H
#define	BASIC_TCP_SOCKET_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "tcp_socket_option.h"
#include "wrlock.h"
#include "utility.h"
#include "logger.h"

namespace l7vs{

template< class SocketClass >
class basic_tcp_socket : private boost::noncopyable{
public:
	// typedefs
	typedef	boost::function< void( const boost::system::error_code&, std::size_t )> async_rw_handler_t; 

	//! constructor
	basic_tcp_socket( boost::asio::io_service& io_service, const tcp_socket_option_info set_option )
		:	io_service_( io_service ),
			opt_info( set_option ){
	}
	//! destructor
	virtual ~basic_tcp_socket(){} ;


	//! get reference control socket
	virtual SocketClass& get_socket(){
		return *my_socket;
	}

	//! option setting
	virtual void setoption( boost::system::error_code& error_code ) = 0;

	//! accept
	virtual void	accept(){
		boost::system::error_code	error_code;
		setoption( error_code );
	}	

	//! close
	virtual bool	close( boost::system::error_code& error_code ) = 0;

	//! write some
	virtual std::size_t	write_some( const boost::asio::const_buffers_1& buffer, boost::system::error_code& error_code ){
		std::size_t	write_size = 0;
		rd_scoped_lock	lock( close_mutex );
		if( is_open() ) write_size = my_socket->write_some( buffer, error_code );
		return write_size;
	}

	//! async_write_some
	virtual void async_write_some( const boost::asio::const_buffers_1& buffer, async_rw_handler_t& handle ){
		rd_scoped_lock	lock( close_mutex );
		my_socket->async_write_some( buffer, handle );
	}

	//! read some
	virtual std::size_t	read_some( const boost::asio::mutable_buffers_1& buffer, boost::system::error_code& error_code ){
		std::size_t	recv_size = 0;
		rd_scoped_lock  lock( close_mutex );
		if( is_open() ) recv_size = my_socket->read_some( buffer, error_code );
		return recv_size;
	}

	//! async read some
	virtual void async_read_some( const boost::asio::mutable_buffers_1& buffer, async_rw_handler_t& handle ){
		rd_scoped_lock	lock( close_mutex );
		my_socket->async_read_some( buffer, handle );
	}

	// is_open
	virtual bool	is_open() = 0;

protected:
	//! io_service reference
	boost::asio::io_service&		io_service_;
	//! control socket
	boost::shared_ptr<SocketClass>	my_socket;
	//! socket close mutex
	wr_mutex						close_mutex;
	//! socket options
	tcp_socket_option_info			opt_info;

	virtual void	set_quickack(  boost::system::error_code& error_code ) = 0;

};	//basic tcp socket

}	// namespace
#endif
