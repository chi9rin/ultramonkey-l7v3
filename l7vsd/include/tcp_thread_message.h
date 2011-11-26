/*!
 *    @file    tcp_thread_message.h
 *    @brief    tcp session thread message class
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

#ifndef TCP_THREAD_MESSAGE_H
#define TCP_THREAD_MESSAGE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "tcp_session.h"

namespace l7vs
{

//!    @class    tcp_thread_message
//! @brief    this class is notify message in tcp session thread.
class tcp_thread_message : private boost::noncopyable
{
public:
#ifdef  DEBUG
        std::string                     func_tag_name;
#endif
        //! constructor
        tcp_thread_message() {}
        //! destructor
        ~tcp_thread_message() {}

        //! request call function object
        boost::function<void(tcp_session::TCP_PROCESS_TYPE_TAG)> message;
        //! function require endpoint
        boost::asio::ip::tcp::endpoint endpoint_info;

};// class tcp_thread_message
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
