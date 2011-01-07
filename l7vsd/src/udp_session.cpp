/*!
 *    @file    udp_session.cpp
 *    @brief    udp session class
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

#include "udp_session.h"
#include "virtualservice.h"

namespace l7vs
{

udp_session::udp_session(virtualservice_udp &vs, boost::asio::io_service &session_io) :
        io(session_io),
        parent_service(vs),
        client_side_socket(session_io)
{

}
udp_session::~udp_session()
{

}

session_result_message udp_session::initialize(const udp_endpoint listen_end)
{
        session_result_message res;
        return res;
}

void udp_session::set_virtual_service_message(UDP_VIRTUAL_SERVICE_MESSAGE_TAG message)
{
}

void udp_session::run(void)
{
}

bool udp_session::client_send(const udp_endpoint client_endpoint, const udp_session_buff &data_buff, const std::size_t data_size, boost::system::error_code &ec)
{
        return true;
}

void udp_session::release_request(const boost::thread::id release_thread_id)
{
}

void udp_session::make_request_thread(const int max_count)
{
}

void udp_session::active_request_thread(const udp_endpoint client_endpoint , const udp_session_buff receive_data, const std::size_t receive_size)
{
}

void udp_session::all_active_request_stop(void)
{
}

}// namespace l7vs
