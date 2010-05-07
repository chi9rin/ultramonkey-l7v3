/*!
 *    @file    command_receiver.h
 *    @brief    l7vsadm message receiver class
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

#ifndef    COMMAND_RECEIVER_H
#define    COMMAND_RECEIVER_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include "command_session.h"
#include "l7vsd.h"

namespace l7vs
{
class    l7vsd;
class    command_session;

//!    @class    command_receiver
//!    @brief    l7vsadm message receiver class
class    command_receiver : private boost::noncopyable
{
protected:
        //! io dispatcher
        boost::asio::io_service                        &dispatcher;
        //! acceptor
        boost::asio::local::stream_protocol::acceptor    acceptor_;
        //! l7vsd refference
        l7vsd                                            &vsd;
        //! socketfile
        std::string                                sockfile;

        //!    @brief        accept handler
        //!    @param[in]    command session
        //!    @param[in]    error code
        void    handle_accept(boost::shared_ptr<command_session>, const boost::system::error_code &);

public:
        //!    @brief        constructor
        //!    @param[in]    io_service
        //!    @param[in]    socket file
        //!    @param[in]    l7vsd refernce
        command_receiver(boost::asio::io_service &, const std::string &, l7vsd &);

        //!    @brief        destructor
        ~command_receiver();
};

};    //namespace l7vsd
#endif    //COMMAND_RECEIVER_H
