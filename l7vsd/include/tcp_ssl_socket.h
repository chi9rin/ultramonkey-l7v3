/*!
 *    @file    tcp_ssl_socket.h
 *    @brief    tcp ssl session socket class
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

#ifndef TCP_SSL_SOCKET_H
#define TCP_SSL_SOCKET_H

#include <bitset>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread/mutex.hpp>

#include "tcp_socket_option.h"
#include "wrlock.h"
#include "utility.h"
#include "logger.h"

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>  ssl_socket;

namespace l7vs{

//!    @class    tcp_ssl_socket
//! @brief    this class is tcp session object use socket.
    class tcp_ssl_socket : private boost::noncopyable{
        public:
            //! construcor
            //! @param[in/out]    socket use io service object
            //! @param[in]        set socket option info 
            tcp_ssl_socket(boost::asio::io_service& io,
                       boost::asio::ssl::context& context,
                       const tcp_socket_option_info set_option)
                       :
                       my_socket(io, context),
                       open_flag(false),
                       opt_info(set_option)
            {
                if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 1, "tcp_ssl_socket::tcp_ssl_socket", __FILE__, __LINE__ );
                }
            }
            //! destructor
            ~tcp_ssl_socket(){
                if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 2, "tcp_ssl_socket::~tcp_ssl_socket", __FILE__, __LINE__ );
                }
            }
            
            //! get reference control socket
            //! @return            reference control socket
            ssl_socket& get_socket(){
                if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 3, "tcp_ssl_socket::get_socket", __FILE__, __LINE__ );
                }
                return my_socket;
            }

            //! handshake socket
            //! @param[in]        handshake_type is handshaking as a server or client
            bool handshake(boost::system::error_code& ec);
            //! accept
            void accept();
            //! close socket
            //! @param[out]        ec is reference error code object
            //! @return         true is socket close
            //! @return         false is not open socket
            bool close(boost::system::error_code& ec);
            //! set non blocking mode of the socket 
            //! @return            ec is reference error code object
            bool set_non_blocking_mode(boost::system::error_code& ec);
            //! write socket
            //! @param[in]        buffers is wite data buffer
            //! @param[out]        ec is reference error code object
            //! @return            write data size
            std::size_t write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);
            //! read socket
            //! @param[out]        buffers is read data buffer
            //! @param[out]        ec is reference error code object
            //! @return            read data size
            std::size_t read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);
            //! is open
            //! @return         true is open
            //! @return         false is close
            bool is_open(){
                return open_flag;
            }

        protected:
            //! control socket
            ssl_socket my_socket;
            //! socket close mutex
            wr_mutex close_mutex;
            //! socket open flag
            bool open_flag;
            //! socket option 
            tcp_socket_option_info opt_info;
    };// class tcp_ssl_socket
}// namespace l7vs

#endif//TCP_SSL_SOCKET_H
 
