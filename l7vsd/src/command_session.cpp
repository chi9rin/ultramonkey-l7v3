/*!
 *    @file    command_session.cpp
 *    @brief    l7vsadm message session class
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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "logger.h"
#include "command_session.h"

namespace    l7vs
{

//!    @brief        constructor
//!    @param[in]    io_service
//!    @param[in]    l7vsd refernce
command_session::command_session(boost::asio::io_service &io_service, l7vsd &parent)
        :    unixsocket(io_service),
             vsd(parent)
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 5, "command_session::command_session", __FILE__, __LINE__);

        // buffer initialize
        request_buffer.assign(0x00);

        // command handler map initialize
        command_handler_map[l7vsadm_request::CMD_LIST]
        = boost::bind(&l7vsd::list_virtual_service, &vsd,
                      &response_data.virtualservice_status_list, _1);
        command_handler_map[l7vsadm_request::CMD_LIST_KEY]
        = boost::bind(&l7vsd::list_virtual_service, &vsd,
                      &response_data.virtualservice_status_list, _1);
        command_handler_map[l7vsadm_request::CMD_LIST_VERBOSE]
        = boost::bind(&l7vsd::list_virtual_service_verbose, &vsd,
                      &response_data, _1);
        command_handler_map[l7vsadm_request::CMD_ADD_VS]
        = boost::bind(&l7vsd::add_virtual_service, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_DEL_VS]
        = boost::bind(&l7vsd::del_virtual_service, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_EDIT_VS]
        = boost::bind(&l7vsd::edit_virtual_service, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_ADD_RS]
        = boost::bind(&l7vsd::add_real_server, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_DEL_RS]
        = boost::bind(&l7vsd::del_real_server, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_EDIT_RS]
        = boost::bind(&l7vsd::edit_real_server, &vsd, &request_data.vs_element, _1);
        command_handler_map[l7vsadm_request::CMD_FLUSH_VS]
        = boost::bind(&l7vsd::flush_virtual_service, &vsd, _1);
        command_handler_map[l7vsadm_request::CMD_REPLICATION]
        = boost::bind(&l7vsd::replication_command, &vsd,
                      &request_data.replication_command, _1);
        command_handler_map[l7vsadm_request::CMD_LOG]
        = boost::bind(&l7vsd::set_loglevel, &vsd,
                      &request_data.log_category,
                      &request_data.log_level, _1);
        command_handler_map[l7vsadm_request::CMD_SNMP]
        = boost::bind(&l7vsd::set_snmp_info, &vsd,
                      &request_data.snmpinfo, _1);
        command_handler_map[l7vsadm_request::CMD_PARAMETER]
        = boost::bind(&l7vsd::reload_parameter, &vsd, &request_data.reload_param, _1);

        // command status map initialize
        command_status_map[l7vsadm_request::CMD_LIST]            = l7vsd_response::RESPONSE_LIST_ERROR;
        command_status_map[l7vsadm_request::CMD_LIST_VERBOSE]    = l7vsd_response::RESPONSE_LIST_VERBOSE_ERROR;
        command_status_map[l7vsadm_request::CMD_LIST_KEY]        = l7vsd_response::RESPONSE_LIST_KEY_ERROR;
        command_status_map[l7vsadm_request::CMD_ADD_VS]            = l7vsd_response::RESPONSE_ADD_VS_ERROR;
        command_status_map[l7vsadm_request::CMD_DEL_VS]            = l7vsd_response::RESPONSE_DEL_VS_ERROR;
        command_status_map[l7vsadm_request::CMD_EDIT_VS]        = l7vsd_response::RESPONSE_EDIT_VS_ERROR;
        command_status_map[l7vsadm_request::CMD_FLUSH_VS]        = l7vsd_response::RESPONSE_FLUSH_VS_ERROR;
        command_status_map[l7vsadm_request::CMD_ADD_RS]            = l7vsd_response::RESPONSE_ADD_RS_ERROR;
        command_status_map[l7vsadm_request::CMD_DEL_RS]            = l7vsd_response::RESPONSE_DEL_RS_ERROR;
        command_status_map[l7vsadm_request::CMD_EDIT_RS]        = l7vsd_response::RESPONSE_EDIT_RS_ERROR;
        command_status_map[l7vsadm_request::CMD_REPLICATION]    = l7vsd_response::RESPONSE_REPLICATION_ERROR;
        command_status_map[l7vsadm_request::CMD_LOG]            = l7vsd_response::RESPONSE_LOG_ERROR;
        command_status_map[l7vsadm_request::CMD_SNMP]            = l7vsd_response::RESPONSE_SNMP_ERROR;
        command_status_map[l7vsadm_request::CMD_PARAMETER]        = l7vsd_response::RESPONSE_PARAMETER_ERROR;
}

//!    @brief        read handler
//!    @param[in]    error code
//!    @param[in]    read size
void    command_session::handle_read(const boost::system::error_code &err, size_t size)
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 6, "command_session::handle_read", __FILE__, __LINE__);

        if (!err) {
                // execute received command
                execute_command();
                // async write response to unixsocket
                boost::asio::async_write(unixsocket,
                                         boost::asio::buffer(response_stream.str()),
                                         boost::bind(&command_session::handle_write,
                                                     shared_from_this(),
                                                     boost::asio::placeholders::error));
        } else {
                // retry async read request from unixsocket.
                unixsocket.async_read_some(boost::asio::buffer(request_buffer),
                                           boost::bind(&command_session::handle_read,
                                                       shared_from_this(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
        }
}

//!    @brief        write handler
//!    @param[in]    error code
void    command_session::handle_write(const boost::system::error_code &err)
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 7, "command_session::handle_write", __FILE__, __LINE__);

        if (err) {
                std::stringstream buf;
                buf << "handle_write error:" << err;
                Logger::putLogError(LOG_CAT_L7VSD_COMMAND, 1, buf.str(), __FILE__, __LINE__);
        }
}

//!    @brief        execute request command
void    command_session::execute_command()
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 8, "command_session::execute_command", __FILE__, __LINE__);

        // deserialize requestdata
        std::stringstream    ss;
        ss << &(request_buffer[0]);
        boost::archive::text_iarchive   ia(ss);
        ia >> request_data;

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_COMMAND)) {
                std::stringstream    debugstr;
                debugstr << boost::format("command_session_recv_request:%s") % request_data;
                Logger::putLogDebug(LOG_CAT_L7VSD_COMMAND, 9, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        // execute command
        command_handler_map_type::iterator itr = command_handler_map.find(request_data.command);
        if (itr != command_handler_map.end()) {
                error_code err;
                // execute command
                itr->second(err);
                if (!err) {        // command succeed
                        response_data.status = l7vsd_response::RESPONSE_OK;
                } else {          // command failed
                        response_data.status = command_status_map[ request_data.command ];
                        response_data.message = err.get_message();
                }
        } else { // command not found
                response_data.status = l7vsd_response::RESPONSE_ERROR;
                response_data.message = "command not found.";
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_COMMAND)) {
                std::stringstream    debugstr;
                debugstr << boost::format("command_session_send_response:%s") % response_data;
                Logger::putLogDebug(LOG_CAT_L7VSD_COMMAND, 10, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        // serialize responsedata
        boost::archive::text_oarchive    oa(response_stream);
        oa << (const l7vsd_response &) response_data;
}

//!    @brief        session start
void    command_session::start()
{
        Logger    logger(LOG_CAT_L7VSD_COMMAND, 11, "command_session::start", __FILE__, __LINE__);

        // start async read requestdata from unixsocket.
        unixsocket.async_read_some(boost::asio::buffer(request_buffer),
                                   boost::bind(&command_session::handle_read,
                                               shared_from_this(),
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
}
}    //namespace    l7vs
