/*!
 *    @file    tcp_ssl_socket.cpp
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

#include <boost/thread/thread.hpp>

#include "tcp_ssl_socket.h"

namespace l7vs{

    //! connect socket
    //! @param[in]        connect_endpoint is connection endpoint
    //! @param[out]        ec is reference error code object
    //! @return         true is connect
    //! @return         false is connect failure 
    bool tcp_ssl_socket::connect(boost::asio::ip::tcp::endpoint connect_endpoint,
        boost::system::error_code& ec){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 4, "in_function : tcp_ssl_socket::connect", __FILE__, __LINE__ );
        }

        rw_scoped_lock scope_lock(close_mutex);
        
        if(likely(!open_flag)){
            my_socket.lowest_layer().connect(connect_endpoint,ec);
            if(unlikely(!ec)){
                open_flag = true;
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] tcp_ssl_socket::connect [";
                    buf << connect_endpoint;
                    buf << "]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 5, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                
                //set TCP_NODELAY
                if(opt_info.nodelay_opt){
                    boost::asio::ip::tcp::no_delay set_option(opt_info.nodelay_val);
                    my_socket.lowest_layer().set_option(set_option,ec);
                    if(unlikely(ec)){
                        //ERROR
                        Logger::putLogError( LOG_CAT_L7VSD_SESSION, 100, "socket option(TCP_NODELAY) set failed" , __FILE__, __LINE__ );
                    }
                }
                
                //set TCP_CORK
                if(opt_info.cork_opt){
                    int val = opt_info.cork_val;
                    size_t len = sizeof(val);
                    boost::asio::detail::socket_ops::setsockopt(my_socket.lowest_layer().native(),IPPROTO_TCP,TCP_CORK,&val,len,ec);
                    if(unlikely(ec)){
                        //ERROR
                        Logger::putLogError( LOG_CAT_L7VSD_SESSION, 101, "socket option(TCP_CORK) set failed" , __FILE__, __LINE__ );
                    }
                }
            }else{
                open_flag = false;
            }
        }
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 4, "out_function : tcp_ssl_socket::connect", __FILE__, __LINE__ );
        }
        return open_flag;
    }
    
    //! handshake socket
    //! @param[in]        handshake_type is handshaking as a server or client
    //! @return         true is handshaked
    //! @return         false is handshake failure
    bool tcp_ssl_socket::handshake(boost::asio::ssl::stream_base::handshake_type type)
    {
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
            Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, "in_function : tcp_ssl_socket::handshake", __FILE__, __LINE__);
        }

        rw_scoped_lock scope_lock(close_mutex);

        bool bres = false;
        bool retryed = false;
        boost::system::error_code ec;
        while (true) {
            my_socket.handshake(type, ec);
            if (ec) {
                if (ec == boost::asio::error::try_again) {
                    if (!retryed) {
                        retryed = true;
                        // retry handshake (try_again[Resource temporarily unavailable])
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                            std::stringstream buf;
                            buf << "Thread ID[";
                            buf << boost::this_thread::get_id();
                            buf << "] tcp_ssl_socket::handshake : retry handshake";
                            Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                    }
                } else {
                    // handshake NG
                    //ERROR
                    Logger::putLogError(LOG_CAT_L7VSD_SESSION, 999, "ssl socket handshaking failed" , __FILE__, __LINE__);
                    //----Debug log----------------------------------------------------------------------
//                    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] tcp_ssl_socket::handshake [";
                        buf << ec.message();
                        buf << "]";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
//                    }
                    //----Debug log----------------------------------------------------------------------
                    break;
                }
            } else {
                // handshake OK
                bres = true;
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] tcp_ssl_socket::handshake : handshake OK";
                    Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                break;
            }
            boost::this_thread::yield();
        }

        if (unlikely( LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
            Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, "out_function : tcp_ssl_socket::handshake", __FILE__, __LINE__);
        }
        return bres;
    }
    
    //! accept socket
    void tcp_ssl_socket::accept(){
        rw_scoped_lock scope_lock(close_mutex);

        open_flag = true;
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            boost::system::error_code ec;
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] tcp_ssl_socket::accept [";
            buf << my_socket.lowest_layer().remote_endpoint(ec);
            buf << "]";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 6, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        
        //set TCP_NODELAY
        if(opt_info.nodelay_opt){
            boost::system::error_code ec;
            boost::asio::ip::tcp::no_delay set_option(opt_info.nodelay_val);
            my_socket.lowest_layer().set_option(set_option,ec);
            if(unlikely(ec)){
                        //ERROR
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 102, "socket option(TCP_NODELAY) set failed" , __FILE__, __LINE__ );
            }
        }
                
        //set TCP_CORK
        if(opt_info.cork_opt){
            boost::system::error_code ec;
            int val = opt_info.cork_val;
            size_t len = sizeof(val);
            boost::asio::detail::socket_ops::setsockopt(my_socket.lowest_layer().native(),IPPROTO_TCP,TCP_CORK,&val,len,ec);
            if(unlikely(ec)){
                        //ERROR
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 103, "socket option(TCP_CORK) set failed" , __FILE__, __LINE__ );
            }
        }
    }

    //! close socket
    //! @param[out]        ec is reference error code object
    //! @return         true is socket close
    //! @return         false is not open socket
    bool tcp_ssl_socket::close(boost::system::error_code& ec){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 7, "in_function : tcp_ssl_socket::close", __FILE__, __LINE__ );
        }
        
        rw_scoped_lock scope_lock(close_mutex);

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            if(open_flag){
                boost::system::error_code ec;
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] tcp_ssl_socket::close [";
                buf << my_socket.lowest_layer().remote_endpoint(ec);
                buf << "]";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 8, buf.str(), __FILE__, __LINE__ );
            }
        }
        //----Debug log----------------------------------------------------------------------
        bool bres = false;
        if(likely(open_flag)){
            open_flag = false;
            bres = true;
        }
        my_socket.lowest_layer().close(ec);
        
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 7, "out_function : tcp_ssl_socket::close", __FILE__, __LINE__ );
        }
        return bres;
    }

    //! set non blocking mode of the socket 
    //! @return            ec is reference error code object
    bool tcp_ssl_socket::set_non_blocking_mode(boost::system::error_code& ec){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 9, "in_function : tcp_ssl_socket::set_non_blocking_mode", __FILE__, __LINE__ );
        }
        
        boost::asio::socket_base::non_blocking_io cmd(true);
        my_socket.lowest_layer().io_control(cmd,ec);
        
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 9, "out_function : tcp_ssl_socket::set_non_blocking_mode", __FILE__, __LINE__ );
        }
        return true;
    }

    //! write socket
    //! @param[in]        buffers is wite data buffer
    //! @param[out]        ec is reference error code object
    //! @return            write data size    
    std::size_t tcp_ssl_socket::write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 10, "in_function : tcp_ssl_socket::write_some", __FILE__, __LINE__ );
        }

        rd_scoped_lock scope_lock(close_mutex);
        std::size_t res_size = 0;
        res_size = my_socket.write_some(buffers,ec);
        if(unlikely(ec)){
            if (likely(!open_flag)) {
                res_size = 0;
                ec.clear();
            }
        }

        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 10, "out_function : tcp_ssl_socket::write_some", __FILE__, __LINE__ );
        }
        return res_size;
    }

    //! read socket
    //! @param[out]        buffers is read data buffer
    //! @param[out]        ec is reference error code object
    //! @return            read data size
    std::size_t tcp_ssl_socket::read_some(boost::asio::mutable_buffers_1 buffers,
        boost::system::error_code& ec){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
//            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 11, "in_function : tcp_ssl_socket::read_some", __FILE__, __LINE__ );
        }

        rd_scoped_lock scope_lock(close_mutex);
        std::size_t res_size = 0;
            if(unlikely(open_flag)){
                //set TCP_QUICKACK
                if(opt_info.quickack_opt){
                    int val = opt_info.quickack_val;
                    std::size_t len = sizeof(val);
                    boost::asio::detail::socket_ops::setsockopt(my_socket.lowest_layer().native(),IPPROTO_TCP,TCP_QUICKACK,&val,len,ec);
                    if (unlikely(!open_flag)) {
                        ec.clear();
                    }
                    if(unlikely(ec)){
                        //ERROR
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] socket option(TCP_QUICKACK) set failed : ";
                        buf << ec.message();
                        Logger::putLogError( LOG_CAT_L7VSD_SESSION, 104, buf.str() , __FILE__, __LINE__ );
                    }
                }
                boost::this_thread::yield();
                res_size = my_socket.read_some(buffers,ec);
std::stringstream buf;
buf << "my_socket.read_some(buffers,ec) res_size[";
buf << res_size;
buf << "]";
Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 999, buf.str() , __FILE__, __LINE__ );
                if(unlikely(ec)){
std::stringstream buf;
buf << "Thread ID[";
buf << boost::this_thread::get_id();
buf << "] tcp_ssl_socket::read_some [";
buf << ec.message();
buf << "]";
Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                    if (unlikely(!open_flag)) {
                        res_size = 0;
                        ec.clear();
                    }
                }

            }
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_SESSION ) ) ){
//            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 11, "out_function : tcp_ssl_socket::read_some", __FILE__, __LINE__ );
        }
        return res_size;
    }

}// namespace l7vs
 
