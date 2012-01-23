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

#include <boost/asio/ssl.hpp>
#include "basic_tcp_socket.h"

namespace l7vs
{

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>  ssl_socket;

class   tcp_ssl_socket : public basic_tcp_socket< ssl_socket >
{
public:
        // typedef
        typedef boost::function< void (const boost::system::error_code &) > async_handshake_handler_t;

        // constructor
        tcp_ssl_socket(
                boost::asio::io_service        &io_service,
                const tcp_socket_option_info set_option,
                boost::asio::ssl::context      &context)
                :       basic_tcp_socket< ssl_socket >(io_service, set_option),
                        context_(context),
                        ssl_strand(io_service),
                        handshake_con(0),
                        read_con(0),
                        write_con(0) {
                my_socket.reset(new ssl_socket(io_service_, context_));
        }
        //destructor
        ~tcp_ssl_socket() {}

        // handshake
        virtual bool handshake(boost::system::error_code &error_code) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                my_socket->handshake(boost::asio::ssl::stream_base::server, error_code);
                return error_code ? false : true;
        }
        //async handshake
        virtual void async_handshake(async_handshake_handler_t  handler) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                handshake_con++;
                my_socket->async_handshake(boost::asio::ssl::stream_base::server, ssl_strand.wrap(handler));
        }

        // option setting
        virtual void setoption(boost::system::error_code &error_code) {
                // non blocking mode
                boost::asio::socket_base::non_blocking_io       cmd(true);
                my_socket->lowest_layer().io_control(cmd, error_code);
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] socket option(NON_BLOCKING_MODE) set fail : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 100, fmt.str(), __FILE__, __LINE__);
                }
                // set TCP_NODELAY option
                if (opt_info.nodelay_opt) {
                        boost::asio::ip::tcp::no_delay cmd(opt_info.nodelay_val);
                        my_socket->lowest_layer().set_option(cmd, error_code);
                        if (unlikely(error_code)) {
                                boost::format   fmt("Thread ID[%d] socket option(TCP_NODELAY) set failed");
                                fmt % boost::this_thread::get_id();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 100, fmt.str(), __FILE__, __LINE__);
                        }
                }
                // set TCP_CORK
                if (opt_info.cork_opt) {
                        int     val = opt_info.cork_val;
                        int err = ::setsockopt(my_socket->lowest_layer().native(), IPPROTO_TCP, TCP_CORK, &val, sizeof(int));
                        if (unlikely(err)) {
                                error_code = boost::system::error_code(errno, boost::asio::error::get_system_category());
                                boost::format   fmt("Thread ID[%d] socket option(TCP_CORK) set failed");
                                fmt % boost::this_thread::get_id();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 101, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }

        // close
        virtual bool close(boost::system::error_code   &error_code) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                if (my_socket->lowest_layer().is_open()) {
                        my_socket->lowest_layer().close(error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] ssl_socket lowest_layer close fail: %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 102, fmt.str(), __FILE__, __LINE__);
                        }
                }
                return error_code ? false : true;
        }


        virtual bool shutdown(boost::system::error_code &error_code) {
                boost::mutex::scoped_lock        lock(ssl_mutex);
                my_socket->shutdown(error_code);
                return error_code ? false : true;
        }

        virtual std::size_t     read_some(const boost::asio::mutable_buffers_1 &buffers, boost::system::error_code &error_code) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                if (write_con > 0) {
                        error_code = boost::asio::error::try_again;
                        return 0;
                }
                if (opt_info.quickack_opt) set_quickack(error_code);
                return my_socket->read_some(buffers, error_code);
        }

        virtual void    async_read_some(const boost::asio::mutable_buffers_1 &buffer, async_rw_handler_t &handle) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                boost::system::error_code       error_code;
                read_con++;
                if (opt_info.quickack_opt) set_quickack(error_code);
                my_socket->async_read_some(buffer, ssl_strand.wrap(handle));
        }

        virtual size_t   write_some(const boost::asio::const_buffers_1 &buffer, boost::system::error_code &error_code) {
                boost::mutex::scoped_lock       lock(ssl_mutex);

                if (read_con > 0) {
                        error_code = boost::asio::error::try_again;
                        return 0;
                }
                return my_socket->write_some(buffer, error_code);
        }

        virtual void    async_write_some(const boost::asio::const_buffers_1 &buffer, tcp_socket::async_rw_handler_t &handle) {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                write_con++;
                my_socket->async_write_some(buffer, ssl_strand.wrap(handle));
        }

        // socket remake
        virtual void clear_socket() {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                my_socket.reset(new ssl_socket(io_service_, context_));

        }

        // get socket
        virtual ssl_socket &get_socket() {
                return *my_socket;
        }
        virtual bool is_open() {
                return my_socket->lowest_layer().is_open();
        }

        void decrement_handshake_con() {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                handshake_con--;
                ssl_cond.notify_one();
        }

        int get_handshake_con() {
                return handshake_con;
        }

        void decrement_read_con() {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                read_con--;
                ssl_cond.notify_one();
        }

        int get_read_con() {
                return read_con;
        }

        void decrement_write_con() {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                write_con--;
                ssl_cond.notify_one();
        }

        int get_write_con() {
                return write_con;
        }

        void wait_async_event_all_end() {
                boost::mutex::scoped_lock       lock(ssl_mutex);
                while (handshake_con > 0 || read_con > 0 || write_con > 0) {
                        boost::format   fmt("handshake_con : %d read_con = %d write_con = %d ");
                        fmt % handshake_con % read_con % write_con ;
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);

                        ssl_cond.wait(lock);
                }
        }

protected:
        //! ssl context reference
        boost::asio::ssl::context      &context_;
        //! ssl mutex
        boost::mutex                            ssl_mutex;
        boost::condition                        ssl_cond;
        boost::asio::strand                     ssl_strand;
        //! set quick ack
        int handshake_con;
        int read_con;
        int write_con;

        virtual void set_quickack(boost::system::error_code &error_code) {
                int err = ::setsockopt(my_socket->lowest_layer().native(), IPPROTO_TCP, TCP_QUICKACK, &opt_info.quickack_val, sizeof(opt_info.quickack_val));
                if (err) error_code = boost::system::error_code(errno, boost::asio::error::get_system_category());
        }

};      //class tcp_ssl_socket

}       //namespace l7vs
#endif  //TCP_SSL_SOCKET_H
