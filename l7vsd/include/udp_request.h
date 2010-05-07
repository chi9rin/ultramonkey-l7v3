/*!
 *    @file    udp_request.h
 *    @brief    udp request class
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

#ifndef UDP_REQUEST_H
#define UDP_REQUEST_H

#include <bitset>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "protocol_module_base.h"
#include "udp_data.h"

#define UDP_REQUEST_THREAD_STATE_BIT 8


namespace l7vs
{
class virtual_service_udp;
class udp_session;

class udp_request : private boost::noncopyable
{
public:
        udp_request(l7vs::virtual_service_udp &vs, l7vs::udp_session &session, boost::asio::io_service &request_io);
        ~udp_request();

        void initialize(boost::asio::ip::udp::endpoint client_endpoint , boost::array<char , MAX_BUFFER_SIZE>& data, std::size_t data_size);
        void run(void);
        void stop(void);

protected:
        typedef boost::function< void(void) > udp_request_func;

        enum FUNC_TYPE_TAG {
                CLIENT_RECEIVE_EVENT = 0,
                REALSERVER_GET_DEST_EVENT,
                REALSERVER_SEND,
                CLIENT_GET_DEST_EVENT,
                CLIENT_SEND,
                REALSERVER_RECEIVE,
                REALSERVER_CLOSE,
                REALSERVER_CLOSE_EVENT,
                EXIT
        };

        boost::asio::io_service &io;
        l7vs::virtual_service_udp &parent_service;
        l7vs::udp_session &parent_session;
        boost::asio::ip::udp::socket realserver_socket;
        bool exit_flag;
        boost::mutex exit_flag_update_mutex;
        std::bitset<UDP_REQUEST_THREAD_STATE_BIT> thread_state;
        boost::thread::id thread_id;
        l7vs::protocol_module_base *protocol_module;
        l7vs::udp_data data_client_side;
        l7vs::udp_data data_dest_side;
        std::map<FUNC_TYPE_TAG, udp_request_func> function_map;
        std::map< protocol_module_base::EVENT_TAG , FUNC_TYPE_TAG> module_event_map;
        udp_request_func next_call_function;

        void client_receive_event(void);
        void realserver_get_destination_event(void);
        void realserver_send(void);
        void realserver_receive(void);
        void client_get_destination_event(void);
        void client_send(void);
        void realserver_close(void);
        void realserver_close_event(void);
        void thread_exit(void);
};// class udp_request
}// namespace l7vs

#endif//UDP_REQUES_H
