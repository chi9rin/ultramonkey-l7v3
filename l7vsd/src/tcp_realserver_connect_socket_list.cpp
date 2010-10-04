/*!
 *    @file    tcp_realserver_connect_socket_list.cpp
 *    @brief    tcp session connect realserver socket list class
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

#include "tcp_realserver_connect_socket_list.h"
#include "logger.h"

namespace l7vs
{

//! constructor
tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list()
{
}
//! destructor
tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list()
{
}

//! push back list
//! @param[in/out]    realserver_socket is realserver list push back element
void tcp_realserver_connect_socket_list::push_back(list_element realserver_socket)
{
        rw_scoped_lock scope_lock(list_mutex);

        connect_list.push_back(realserver_socket);
}

//! get socket list
//! @return            realserver list element
tcp_realserver_connect_socket_list::list_element tcp_realserver_connect_socket_list::get_socket()
{
        rw_scoped_lock scope_lock(list_mutex);

        list_element sock;
        if (!connect_list.empty()) {
                sock = *(connect_list.begin());
                connect_list.pop_front();
        }
        return sock;
}

//! check empty list
bool tcp_realserver_connect_socket_list::empty()
{
        return connect_list.empty();
}

//! clear list
void tcp_realserver_connect_socket_list::clear()
{
        rw_scoped_lock scope_lock(list_mutex);
        connect_list.clear();
}

}// namespace l7vs

