/*!
 *    @file    tcp_realserver_connect_socket_list.h
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

#ifndef TCP_REALSERVER_CONNECT_SOCKET_LIST_H
#define TCP_REALSERVER_CONNECT_SOCKET_LIST_H

#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "tcp_socket.h"

#include "wrlock.h"

namespace l7vs
{

//!    @class    tcp_realserver_connect_socket_list
//! @brief    this class is connected realsever list by up thread.
class tcp_realserver_connect_socket_list : private boost::noncopyable
{
public:
        //!    socket pointer type
        typedef boost::shared_ptr<tcp_socket> tcp_socket_ptr;
        //! realserver list element type first : connection endpoint second : socket pointer
        typedef std::pair<boost::asio::ip::tcp::endpoint, tcp_socket_ptr > list_element;

        //! construcor
        tcp_realserver_connect_socket_list();
        //! destructor
        ~tcp_realserver_connect_socket_list();
        //! push back list
        //! @param[in/out]    realserver_socket is realserver list push back element
        void push_back(list_element realserver_socket);
        //! get socket list
        //! @return            realserver list element
        list_element get_socket();
        //! check empty list
        bool empty();
        //! clear list
        void clear();

protected:
        //! realserver list
        std::list< list_element > connect_list;
        //! list access mutex
        wr_mutex list_mutex;
};// class tcp_realserver_connect_socket_list
}// namespace l7vs

#endif//TCP_REALSERVER_CONNECT_SOCKET_LIST_H


