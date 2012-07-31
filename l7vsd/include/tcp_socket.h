/*!
 *    @file    tcp_socket.h
 *    @brief    tcp session socket class
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

#include "basic_tcp_socket.h"

namespace l7vs
{

class   tcp_socket : public basic_tcp_socket< boost::asio::ip::tcp::socket>
{
public:
        //typedefs
        typedef boost::function< void(const boost::system::error_code &)> connect_handler_t;

        // constractor
        tcp_socket(boost::asio::io_service &io_service, const tcp_socket_option_info set_option)
                : basic_tcp_socket< boost::asio::ip::tcp::socket>(io_service, set_option) {
                my_socket.reset(new boost::asio::ip::tcp::socket(io_service_));
        }

        // destractor
        ~tcp_socket() {}

        // set option
        virtual void setoption(boost::system::error_code &error_code) {
                // non blocking mode
                boost::asio::socket_base::non_blocking_io       cmd(true);
                my_socket->io_control(cmd, error_code);
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] socket option(NON_BLOCKING_MODE) set fail : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 100, fmt.str(), __FILE__, __LINE__);
                }
                // set TCP_NODELAY option
                if (opt_info.nodelay_opt) {
                        boost::asio::ip::tcp::no_delay  option(opt_info.nodelay_val);
                        my_socket->set_option(option, error_code);
                        if (unlikely(error_code)) {
                                boost::format   fmt("Thread ID[%d] socket option(TCP_NODELAY) set failed");
                                fmt % boost::this_thread::get_id();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 100, fmt.str(), __FILE__, __LINE__);
                        }
                }
                // set TCP_CORK option
                if (opt_info.cork_opt) {
                        int     val = opt_info.cork_val;
                        int err = ::setsockopt(my_socket->native(), IPPROTO_TCP, TCP_CORK, &val, sizeof(int));
                        if (unlikely(err)) {
                                error_code = boost::system::error_code(errno, boost::asio::error::get_system_category());
                                boost::format   fmt("Thread ID[%d] socket option(TCP_CORK) set failed");
                                fmt % boost::this_thread::get_id();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 101, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }

        // connect
        virtual bool connect(const boost::asio::ip::tcp::endpoint &connect_endpoint, boost::system::error_code &error_code) {
                rd_scoped_lock  lock(close_mutex);
                my_socket->connect(connect_endpoint, error_code);
                setoption(error_code);
                return error_code ? false : true;
        }

        // async connect
        virtual void async_connect(const boost::asio::ip::tcp::endpoint &connect_endpoint, connect_handler_t handler) {
                rd_scoped_lock  lock(close_mutex);
                my_socket->async_connect(connect_endpoint, handler);
        }

        virtual bool shutdown(boost::system::error_code &error_code) {
                rd_scoped_lock  lock(close_mutex);
                my_socket->cancel(error_code);
                my_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error_code);
                return error_code ? false : true;
        }

        // close
        virtual bool close(boost::system::error_code &error_code) {
                rd_scoped_lock  lock(close_mutex);
                if (is_open()){
                        my_socket->cancel(error_code);
                        my_socket->close(error_code);
                }
                return error_code ? false : true;
        }

        // readsome
        virtual std::size_t read_some(const boost::asio::mutable_buffers_1 &buffer, boost::system::error_code &error_code) {
                if (opt_info.quickack_opt) set_quickack(error_code);
                return basic_tcp_socket< boost::asio::ip::tcp::socket >::read_some(buffer, error_code);
        }

        // async_read_some
        virtual void async_read_some(const boost::asio::mutable_buffers_1 &buffer, async_rw_handler_t &handle) {
                boost::system::error_code error_code;
                if (opt_info.quickack_opt) set_quickack(error_code);
                basic_tcp_socket< boost::asio::ip::tcp::socket >::async_read_some(buffer, handle);
        }

        // is_open
        virtual bool is_open() {
                return my_socket->is_open();
        }

        // open
        virtual void open(const boost::asio::ip::tcp::endpoint &endpoint) {
                boost::system::error_code       error_code;
                endpoint.address().is_v4()      ? my_socket->open(boost::asio::ip::tcp::v4(), error_code)
                : my_socket->open(boost::asio::ip::tcp::v6(), error_code);
                if (error_code) {
                        boost::format   fmt("Thread ID[%d] open error : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 100, fmt.str(), __FILE__, __LINE__);
                }
        }

#ifdef  IP_TRANSPARENT
        virtual void    set_transparent(const boost::asio::ip::tcp::endpoint &client_endpoint, boost::system::error_code &error_code) {
                int ip_socket_level;
                client_endpoint.address().is_v4()
                ? ip_socket_level = SOL_IP
                                    : ip_socket_level = SOL_IPV6;
                // set IP_TRANSPARENT
                int on = 1;
                int err = ::setsockopt(my_socket->native(), ip_socket_level, IP_TRANSPARENT, &on, sizeof(on));
                if (err) {
                        error_code = boost::system::error_code(errno, boost::asio::error::get_system_category());
                        return;
                }

                // bind client address
                my_socket->bind(client_endpoint, error_code);
        }
#endif
protected:
        virtual void set_quickack(boost::system::error_code &error_code) {
                int err = ::setsockopt(my_socket->native(), IPPROTO_TCP, TCP_QUICKACK, &opt_info.quickack_val, sizeof(opt_info.quickack_val));
                if (err) error_code = boost::system::error_code(errno, boost::asio::error::get_system_category());
        }

};      //tcp_socket

}       // namespace l7vs
#endif//TCP_SOCKET_H

