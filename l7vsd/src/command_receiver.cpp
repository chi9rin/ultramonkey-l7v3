/*!
 *    @file    command_receiver.cpp
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

#include "logger.h"
#include "command_receiver.h"

namespace    l7vs
{

//!    @brief        constructor
//!    @param[in]    io_service
//!    @param[in]    socket file
//!    @param[in]    l7vsd reference
command_receiver::command_receiver(boost::asio::io_service &io_service, const std::string &file, l7vsd &parent)
        :    dispatcher(io_service),
             acceptor_(io_service, boost::asio::local::stream_protocol::endpoint(file)),
             vsd(parent)
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 99999, "command_receiver::command_receiver", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_COMMAND)) {
                std::stringstream    debugstr;
                debugstr << "command_receiver::command_receiver arguments:";
                debugstr << boost::format("file=%s") % file;
                Logger::putLogDebug(LOG_CAT_L7VSD_COMMAND, 99999, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        sockfile = file;

        // create command_session for first accept.
        command_session::command_session_ptr session(new command_session(dispatcher, vsd));
        // start an asynchronous accept for first.
        acceptor_.async_accept(session->socket(),
                               boost::bind(&command_receiver::handle_accept,
                                           this,
                                           session,
                                           boost::asio::placeholders::error));
}

//!    @brief        destructor
command_receiver::~command_receiver()
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 99999, "command_receiver::~command_receiver", __FILE__, __LINE__);

        unlink(sockfile.c_str());
}

//!    @brief        accept handler
//!    @param[in]    command session
//!    @param[in]    error code
void    command_receiver::handle_accept(command_session::command_session_ptr session, const boost::system::error_code &err)
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 99999, "command_receiver::handle_accept", __FILE__, __LINE__);

        // check async_accept() result.
        if (!err) {
                // command_session start.
                session->start();
                // create command_session for next accept.
                session.reset(new command_session(dispatcher, vsd));
                // start an asynchronous accept for next.
                acceptor_.async_accept(session->socket(),
                                       boost::bind(&command_receiver::handle_accept,
                                                   this,
                                                   session,
                                                   boost::asio::placeholders::error));

        } else {
                // start an asynchronous accept for retry.
                acceptor_.async_accept(session->socket(),
                                       boost::bind(&command_receiver::handle_accept,
                                                   this,
                                                   session,
                                                   boost::asio::placeholders::error));
        }
}

}    //namespace    l7vs
