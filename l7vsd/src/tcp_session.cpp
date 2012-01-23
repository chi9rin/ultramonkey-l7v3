/*!
 *    @file    tcp_session.cpp
 *    @brief    tcp session class
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
#include <boost/format.hpp>
#include <cerrno>
#include "tcp_session.h"
#include "tcp_thread_message.h"
#include "virtualservice.h"
#include "logger.h"
#include "logger_implement_access.h"
#include "parameter.h"
#include "utility.h"

namespace l7vs
{

//! constructor
//! @param[in/out]  vs is parent virtualservice object
//! @param[in/out]  session_io is session use io service object
//! @param[in]      set_option is session use set socket option info
//! @param[in]      listen_endpoint is virtualservice listen endpoint
//! @param[in]      ssl_mode is session use SSL flag
//! @param[in]      set_ssl_context is session use SSL context object
//! @param[in]      set_ssl_cache_flag is session use SSL session cache
//! @param[in]      set_ssl_handshake_time_out is session use SSL handshake timeout
//! @param[in]      set_access_logger is session use access logger
tcp_session::tcp_session(
        virtualservice_tcp &vs,
        boost::asio::io_service &session_io,
        const tcp_socket_option_info set_option,
        const endpoint listen_endpoint,
        const bool ssl_mode,
        boost::asio::ssl::context &set_ssl_context,
        const bool set_ssl_cache_flag,
        const int set_ssl_handshake_time_out,
        logger_implement_access *set_access_logger)
        :
        parent_dispatcher(session_io),
        parent_service(vs),
        exit_flag(0),
        upthread_status(UPTHREAD_SLEEP),
        downthread_status(DOWNTHREAD_SLEEP),
        protocol_module(NULL),
        client_socket(session_io, set_option),
        upstream_buffer_size(-1),
        downstream_buffer_size(-1),
        virtualservice_endpoint(listen_endpoint),
        access_log_flag(false),
        access_logger(set_access_logger),
        ssl_flag(ssl_mode),
        client_ssl_socket(session_io, set_option, set_ssl_context),
        ssl_context(set_ssl_context),
        ssl_cache_flag(set_ssl_cache_flag),
        ssl_handshake_time_out(set_ssl_handshake_time_out),
        socket_opt_info(set_option)
{

        // set up_thread_module_event_map
        up_thread_module_event_map[protocol_module_base::ACCEPT]                                = UP_FUNC_CLIENT_ACCEPT;
        up_thread_module_event_map[protocol_module_base::CLIENT_DISCONNECT]     = UP_FUNC_CLIENT_DISCONNECT;
        up_thread_module_event_map[protocol_module_base::CLIENT_RECV]                   = UP_FUNC_CLIENT_RECEIVE;
        up_thread_module_event_map[protocol_module_base::CLIENT_RESPONSE_SEND]  = UP_FUNC_CLIENT_RESPOND_SEND;
        up_thread_module_event_map[protocol_module_base::REALSERVER_SELECT]             = UP_FUNC_REALSERVER_GET_DEST_EVENT;
        up_thread_module_event_map[protocol_module_base::REALSERVER_CONNECT]    = UP_FUNC_REALSERVER_CONNECT;
        up_thread_module_event_map[protocol_module_base::REALSERVER_SEND]               = UP_FUNC_REALSERVER_SEND;
        up_thread_module_event_map[protocol_module_base::REALSERVER_DISCONNECT] = UP_FUNC_REALSERVER_ALL_DISCONNECT;
        up_thread_module_event_map[protocol_module_base::SORRYSERVER_SELECT]    = UP_FUNC_SORRYSERVER_GET_DEST;
        up_thread_module_event_map[protocol_module_base::SORRYSERVER_CONNECT]   = UP_FUNC_SORRYSERVER_CONNECT;
        up_thread_module_event_map[protocol_module_base::SORRYSERVER_SEND]              = UP_FUNC_SORRYSERVER_SEND;
        up_thread_module_event_map[protocol_module_base::SORRYSERVER_DISCONNECT] = UP_FUNC_SORRYSERVER_MOD_DISCONNECT;
        up_thread_module_event_map[protocol_module_base::FINALIZE]                              = UP_FUNC_EXIT;

        // set up_que_function_map
        up_que_function_map[UP_FUNC_CLIENT_DISCONNECT]          =
                boost::bind(&tcp_session::up_thread_client_disconnect, this, _1);
        up_que_function_map[UP_FUNC_CLIENT_DISCONNECT_EVENT]            =
                boost::bind(&tcp_session::up_thread_client_disconnect_event, this, _1);
        up_que_function_map[UP_FUNC_REALSERVER_DISCONNECT]                      =
                boost::bind(&tcp_session::up_thread_realserver_disconnect, this, _1);
        up_que_function_map[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT]       =
                boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event, this, _1);
        up_que_function_map[UP_FUNC_PAUSE_ON_EVENT]                                     =
                boost::bind(&tcp_session::up_thread_session_pause_on_event, this, _1);
        up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT]                            =
                boost::bind(&tcp_session::up_thread_session_pause_off_event, this, _1);
        up_que_function_map[UP_FUNC_CLIENT_ACCEPT_EVENT]                        =
                boost::bind(&tcp_session::up_thread_client_accept_event, this, _1);
        up_que_function_map[UP_FUNC_CLIENT_ACCEPT_FAIL_EVENT]           =
                boost::bind(&tcp_session::up_thread_client_accept_fail_event, this, _1);

        // set up_thread_function_array
        up_thread_function_array[UP_FUNC_CLIENT_ACCEPT] =
                std::make_pair(
                        UP_FUNC_CLIENT_ACCEPT,
                        boost::bind(&tcp_session::up_thread_client_accept, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_ACCEPT_EVENT] =
                std::make_pair(
                        UP_FUNC_CLIENT_ACCEPT_EVENT,
                        boost::bind(&tcp_session::up_thread_client_accept_event, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT] =
                std::make_pair(
                        UP_FUNC_CLIENT_DISCONNECT,
                        boost::bind(&tcp_session::up_thread_client_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT] =
                std::make_pair(
                        UP_FUNC_CLIENT_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::up_thread_client_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RECEIVE] =
                std::make_pair(
                        UP_FUNC_CLIENT_RECEIVE,
                        boost::bind(&tcp_session::up_thread_client_receive, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_HANDLE_ARECIEVE] =
                std::make_pair(
                        UP_FUNC_CLIENT_HANDLE_ARECIEVE,
                        boost::bind(&tcp_session::up_thread_client_handle_async_read_some, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND] =
                std::make_pair(
                        UP_FUNC_CLIENT_RESPOND_SEND,
                        boost::bind(&tcp_session::up_thread_client_respond, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT] =
                std::make_pair(
                        UP_FUNC_CLIENT_RESPOND_SEND_EVENT,
                        boost::bind(&tcp_session::up_thread_client_respond_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_GET_DEST_EVENT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_GET_DEST_EVENT,
                        boost::bind(&tcp_session::up_thread_realserver_get_destination_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_CONNECT,
                        boost::bind(&tcp_session::up_thread_realserver_connect, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_CONNECT_EVENT,
                        boost::bind(&tcp_session::up_thread_realserver_connect_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT,
                        boost::bind(&tcp_session::up_thread_realserver_connection_fail_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_SEND] =
                std::make_pair(
                        UP_FUNC_REALSERVER_SEND,
                        boost::bind(&tcp_session::up_thread_realserver_send, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_HANDLE_ASEND] =
                std::make_pair(
                        UP_FUNC_REALSERVER_HANDLE_ASEND,
                        boost::bind(&tcp_session::up_thread_realserver_handle_async_write_some, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_DISCONNECT,
                        boost::bind(&tcp_session::up_thread_realserver_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT] =
                std::make_pair(
                        UP_FUNC_REALSERVER_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::up_thread_realserver_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CHECK] =
                std::make_pair(
                        UP_FUNC_REALSERVER_CHECK,
                        boost::bind(&tcp_session::up_thread_realserver_check, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_GET_DEST] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_GET_DEST,
                        boost::bind(&tcp_session::up_thread_sorryserver_get_destination_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_CONNECT,
                        boost::bind(&tcp_session::up_thread_sorryserver_connect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_CONNECT_EVENT,
                        boost::bind(&tcp_session::up_thread_sorryserver_connect_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT,
                        boost::bind(&tcp_session::up_thread_sorryserver_connection_fail_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_SEND] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_SEND,
                        boost::bind(&tcp_session::up_thread_sorryserver_send, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_HANDLE_ASEND] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_HANDLE_ASEND,
                        boost::bind(&tcp_session::up_thread_sorryserver_handle_async_write_some, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_DISCONNECT,
                        boost::bind(&tcp_session::up_thread_sorryserver_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_MOD_DISCONNECT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_MOD_DISCONNECT,
                        boost::bind(&tcp_session::up_thread_sorryserver_mod_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT] =
                std::make_pair(
                        UP_FUNC_SORRYSERVER_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRY_ENABLE_EVENT] =
                std::make_pair(
                        UP_FUNC_SORRY_ENABLE_EVENT,
                        boost::bind(&tcp_session::up_thread_sorry_enable_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRY_DISABLE_EVENT] =
                std::make_pair(
                        UP_FUNC_SORRY_DISABLE_EVENT,
                        boost::bind(&tcp_session::up_thread_sorry_disable_event, this, _1));
        up_thread_function_array[UP_FUNC_PAUSE_ON_EVENT] =
                std::make_pair(
                        UP_FUNC_PAUSE_ON_EVENT,
                        boost::bind(&tcp_session::up_thread_session_pause_on_event, this, _1));
        up_thread_function_array[UP_FUNC_PAUSE_OFF_EVENT] =
                std::make_pair(
                        UP_FUNC_PAUSE_OFF_EVENT,
                        boost::bind(&tcp_session::up_thread_session_pause_off_event, this, _1));
        up_thread_function_array[UP_FUNC_EXIT] =
                std::make_pair(
                        UP_FUNC_EXIT,
                        boost::bind(&tcp_session::up_thread_exit, this, _1));


        // set down_thread_module_event_map
        down_thread_module_event_map[protocol_module_base::CLIENT_SEND]                         = DOWN_FUNC_CLIENT_SEND;
        down_thread_module_event_map[protocol_module_base::CLIENT_DISCONNECT]           = DOWN_FUNC_CLIENT_DISCONNECT;
        down_thread_module_event_map[protocol_module_base::CLIENT_CONNECTION_CHECK]     = DOWN_FUNC_CLIENT_CONNECTION_CHK;
        down_thread_module_event_map[protocol_module_base::REALSERVER_RECV]                     = DOWN_FUNC_REALSERVER_RECEIVE;
        down_thread_module_event_map[protocol_module_base::REALSERVER_DISCONNECT]       = DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
        down_thread_module_event_map[protocol_module_base::SORRYSERVER_RECV]            = DOWN_FUNC_SORRYSERVER_RECEIVE;
        down_thread_module_event_map[protocol_module_base::SORRYSERVER_DISCONNECT]      = DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT;
        down_thread_module_event_map[protocol_module_base::FINALIZE]                            = DOWN_FUNC_EXIT;

        // set down_thread_function_array
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT] =
                std::make_pair(
                        DOWN_FUNC_CLIENT_DISCONNECT,
                        boost::bind(&tcp_session::down_thread_client_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT] =
                std::make_pair(
                        DOWN_FUNC_CLIENT_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::down_thread_client_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_CONNECTION_CHK] =
                std::make_pair(
                        DOWN_FUNC_CLIENT_CONNECTION_CHK,
                        boost::bind(&tcp_session::down_thread_client_connection_chk_event, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_SEND] =
                std::make_pair(
                        DOWN_FUNC_CLIENT_SEND,
                        boost::bind(&tcp_session::down_thread_client_send, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_HANDLE_ASEND] =
                std::make_pair(
                        DOWN_FUNC_CLIENT_HANDLE_ASEND,
                        boost::bind(&tcp_session::down_thread_client_handle_async_write_some, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE] =
                std::make_pair(
                        DOWN_FUNC_REALSERVER_RECEIVE,
                        boost::bind(&tcp_session::down_thread_realserver_receive, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_HANDLE_ARECEIVE] =
                std::make_pair(
                        DOWN_FUNC_REALSERVER_HANDLE_ARECEIVE,
                        boost::bind(&tcp_session::down_thread_realserver_handle_async_read_some, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT] =
                std::make_pair(
                        DOWN_FUNC_REALSERVER_DISCONNECT,
                        boost::bind(&tcp_session::down_thread_realserver_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT] =
                std::make_pair(
                        DOWN_FUNC_REALSERVER_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::down_thread_realserver_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE] =
                std::make_pair(
                        DOWN_FUNC_SORRYSERVER_RECEIVE,
                        boost::bind(&tcp_session::down_thread_sorryserver_receive, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_HANDLE_ARECEIVE] =
                std::make_pair(
                        DOWN_FUNC_SORRYSERVER_HANDLE_ARECEIVE,
                        boost::bind(&tcp_session::down_thread_sorryserver_handle_async_read_some, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT] =
                std::make_pair(
                        DOWN_FUNC_SORRYSERVER_DISCONNECT,
                        boost::bind(&tcp_session::down_thread_sorryserver_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT] =
                std::make_pair(
                        DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT,
                        boost::bind(&tcp_session::down_thread_sorryserver_mod_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT] =
                std::make_pair(
                        DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT,
                        boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRY_ENABLE_EVENT] =
                std::make_pair(
                        DOWN_FUNC_SORRY_ENABLE_EVENT,
                        boost::bind(&tcp_session::down_thread_sorry_enable_event , this, _1));
        down_thread_function_array[DOWN_FUNC_SORRY_DISABLE_EVENT] =
                std::make_pair(
                        DOWN_FUNC_SORRY_DISABLE_EVENT,
                        boost::bind(&tcp_session::down_thread_sorry_disable_event , this, _1));
        down_thread_function_array[DOWN_FUNC_PAUSE_ON_EVENT] =
                std::make_pair(
                        DOWN_FUNC_PAUSE_ON_EVENT,
                        boost::bind(&tcp_session::down_thread_session_pause_on_event , this, _1));
        down_thread_function_array[DOWN_FUNC_PAUSE_OFF_EVENT] =
                std::make_pair(
                        DOWN_FUNC_PAUSE_OFF_EVENT,
                        boost::bind(&tcp_session::down_thread_session_pause_off_event , this, _1));
        down_thread_function_array[DOWN_FUNC_EXIT] =
                std::make_pair(
                        DOWN_FUNC_EXIT,
                        boost::bind(&tcp_session::down_thread_exit, this, _1));

        // set down_que_function_map
        down_que_function_map[DOWN_FUNC_CLIENT_DISCONNECT_EVENT] =
                boost::bind(&tcp_session::down_thread_client_disconnect_event, this, _1);
        down_que_function_map[DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT] =
                boost::bind(&tcp_session::down_thread_client_respond_event, this, _1);
        down_que_function_map[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT] =
                boost::bind(&tcp_session::down_thread_realserver_disconnect_event, this, _1);
        down_que_function_map[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT] =
                boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event, this, _1);
        down_que_function_map[DOWN_FUNC_SORRY_ENABLE_EVENT] =
                boost::bind(&tcp_session::down_thread_sorry_enable_event, this, _1);
        down_que_function_map[DOWN_FUNC_EXIT] =
                boost::bind(&tcp_session::down_thread_exit, this, _1);
        down_que_function_map[DOWN_FUNC_PAUSE_ON_EVENT] =
                boost::bind(&tcp_session::down_thread_session_pause_on_event, this, _1);
        down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT] =
                boost::bind(&tcp_session::down_thread_session_pause_off_event, this, _1);

        // set virtual_service_message_up_thread_function_map
        virtual_service_message_up_thread_function_map[SORRY_STATE_ENABLE]      = boost::bind(&tcp_session::up_thread_sorry_enable_event , this, _1);
        virtual_service_message_up_thread_function_map[SORRY_STATE_DISABLE] = boost::bind(&tcp_session::up_thread_sorry_disable_event , this, _1);
        virtual_service_message_up_thread_function_map[SESSION_END]                     = boost::bind(&tcp_session::up_thread_exit , this, _1);
        virtual_service_message_up_thread_function_map[REALSERVER_REMOVE]       = boost::bind(&tcp_session::up_thread_realserver_check , this, _1);

        // set virtual_service_message_down_thread_function_map
        virtual_service_message_down_thread_function_map[SORRY_STATE_ENABLE]    = boost::bind(&tcp_session::down_thread_sorry_enable_event , this, _1);
        virtual_service_message_down_thread_function_map[SORRY_STATE_DISABLE]   = boost::bind(&tcp_session::down_thread_sorry_disable_event , this, _1);
        virtual_service_message_down_thread_function_map[SESSION_END]                   = boost::bind(&tcp_session::down_thread_exit , this, _1);
        virtual_service_message_down_thread_function_map[REALSERVER_REMOVE]             = boost::bind(&tcp_session::down_thread_realserver_check , this, _1);
}

//! destructor
tcp_session::~tcp_session()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN ~tcp_session");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        // up_thread_module_event_map
        up_thread_module_event_map.clear();
        // down_que_function_map
        down_que_function_map.clear();
        // down_thread_module_event_map
        down_thread_module_event_map.clear();
        // down_thread_message_down_thread_function_map
        up_que_function_map.clear();
        // virtual_service_message_up_thread_function_map
        virtual_service_message_up_thread_function_map.clear();
        // virtual_service_message_down_thread_function_map
        virtual_service_message_down_thread_function_map.clear();
        // realserver socket clear
        realserver_socket.reset();
        // realserver endpoint clear
        realserver_endpoint = boost::asio::ip::tcp::endpoint();
        // connectiing socket clear
        connecting_socket.reset();
        // connecting endpoint clear
        connecting_endpoint = boost::asio::ip::tcp::endpoint();
        // up_thread_message_que
        tcp_thread_message *msg = NULL;
        msg = up_thread_message_que.pop();
        while (msg) {
                delete msg;
                msg = up_thread_message_que.pop();
        }
        // down_thread_message_que
        msg = down_thread_message_que.pop();
        while (msg) {
                delete msg;
                msg = down_thread_message_que.pop();
        }
}

//! initialize
session_result_message tcp_session::initialize()
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN initialize");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        session_result_message msg;
        msg.flag = false;
        msg.message = "";
        exit_flag = 0;
        up_thread_id = boost::thread::id();
        down_thread_id = boost::thread::id();
        upthread_status = UPTHREAD_SLEEP;
        downthread_status = DOWNTHREAD_SLEEP;
        protocol_module = NULL;
        tcp_thread_message *tmp_msg;
        tmp_msg = up_thread_message_que.pop();
        while (tmp_msg) {
                delete tmp_msg;
                tmp_msg = up_thread_message_que.pop();
        }

        tmp_msg = down_thread_message_que.pop();
        while (tmp_msg) {
                delete tmp_msg;
                tmp_msg = down_thread_message_que.pop();
        }

        //load parameter
        l7vs::Parameter param;
        l7vs::error_code vs_err;
        int int_val;

        int_val = param.get_int(PARAM_COMP_SESSION, PARAM_UP_BUFFER_SIZE, vs_err);
        if (likely(!vs_err) && (int_val > 0)) {
                upstream_buffer_size = int_val;
        }

        int_val = param.get_int(PARAM_COMP_SESSION, PARAM_DOWN_BUFFER_SIZE, vs_err);
        if ((likely(!vs_err)) && (int_val > 0)) {
                downstream_buffer_size = int_val;
        }
        protocol_module = parent_service.get_protocol_module();

        if (unlikely(protocol_module == NULL)) {
                //Error protocol_module NULL
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "protocol_module is NULL!";
                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 5, buf.str(), __FILE__, __LINE__);
                msg.flag = true;
                msg.message = "Protocol module not found";
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT initialize");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return msg;
}

//! get reference client side socket
//! @return            reference client side socket
tcp_socket &tcp_session::get_client_socket()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN/OUT get_client_socket");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return client_socket;
}
//! get reference client side ssl socket
//! @return            reference client side ssl socket
ssl_socket &tcp_session::get_client_ssl_socket()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN/OUT get_client_ssl_socket");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return client_ssl_socket.get_socket();
}
//! message from parent virtualservice
//! @param[in]        message is tcp virtualservice message type
void tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message, const boost::asio::ip::tcp::endpoint endpoint_)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN set_virtual_service_message");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        switch (message) {
        case SESSION_PAUSE_ON: {
                tcp_thread_message     *up_msg = new tcp_thread_message();
                up_msg->message = up_que_function_map[UP_FUNC_PAUSE_ON_EVENT];
#ifdef  DEBUG
                up_msg->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_ON_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_ON_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                while (!up_thread_message_que.push(up_msg)) {}
                upthread_status_cond.notify_one();

                tcp_thread_message *down_msg = new tcp_thread_message();
                down_msg->message = down_que_function_map[DOWN_FUNC_PAUSE_ON_EVENT];
#ifdef  DEBUG
                down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                while (!down_thread_message_que.push(down_msg)) {}
                downthread_status_cond.notify_one();

                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] FUNC OUT set_virtual_service_message SESSION_PAUSE_ON");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                return;
        }
        case SESSION_PAUSE_OFF: {
                tcp_thread_message *up_msg = new tcp_thread_message();
                up_msg->message = up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
                up_msg->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                while (!up_thread_message_que.push(up_msg)) {}
                upthread_status_cond.notify_one();

                tcp_thread_message *down_msg = new tcp_thread_message();
                down_msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
                down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                while (!down_thread_message_que.push(down_msg)) {}
                downthread_status_cond.notify_one();

                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] FUNC OUT set_virtual_service_message SESSION_PAUSE_OFF");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                return;
        }
        case ACCESS_LOG_ON: {
                rw_scoped_lock scope_lock(access_log_flag_mutex);
                access_log_flag = true;
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] FUNC OUT set_virtual_service_message ACCESS_LOG_ON");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                return;
        }
        case ACCESS_LOG_OFF: {
                rw_scoped_lock scope_lock(access_log_flag_mutex);
                access_log_flag = false;
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] FUNC OUT set_virtual_service_message ACCESS_LOG_OFF");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                return;
        }
        case REALSERVER_REMOVE:
                //---Debug Log------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] set_virtual_service_message message:[REALSERVER_REMOVE]");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                break;
        case SORRY_STATE_ENABLE:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] set_virtual_service_message message:[SORRY_STATE_ENABLE]");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 14, fmt.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                break;
        case SORRY_STATE_DISABLE:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] set_virtual_service_message message:[SORRY_STATE_DISABLE]";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 15, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                break;
        case SESSION_END:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] set_virtual_service_message message:[SESSION_END]";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 16, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                break;
        }
        // set message
        tcp_thread_message *up_msg = new tcp_thread_message;
        up_msg->message = virtual_service_message_up_thread_function_map[message];
        up_msg->endpoint_info = endpoint_;
#ifdef  DEBUG
        up_msg->func_tag_name = func_tag_to_string(message);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(message);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(up_msg)) {}
        upthread_status_cond.notify_one();

        tcp_thread_message *down_msg = new tcp_thread_message;
        down_msg->message = virtual_service_message_down_thread_function_map[message];
        down_msg->endpoint_info = endpoint_;
#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(message);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(message);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(down_msg)) {}
        downthread_status_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT set_virtual_service_message");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up stream thread main function
void tcp_session::up_thread_run()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 17, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       ec;
        boost::xtime                            xt;

        // down thread active waiting.
        up_thread_id = boost::this_thread::get_id();
        {
                boost::mutex::scoped_lock       lock(downthread_status_mutex);
                while (downthread_status < DOWNTHREAD_ALIVE) {
                        to_time(LOCKTIMEOUT, xt);
                        downthread_status_cond.timed_wait(lock, xt);
                }
        }

        // socket option check
        if (!exit_flag) {
                boost::system::error_code       error_code;
                !ssl_flag ? client_socket.setoption(error_code)   // error output in tcp_socket.h
                : client_ssl_socket.setoption(error_code);
        }

        if (!exit_flag) {
                // get client endpoint
                client_endpoint = !ssl_flag ? client_socket.get_socket().lowest_layer().remote_endpoint(ec)
                                  : client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                if (unlikely(ec)) {
                        boost::format   fmt("Thread ID[%d] cannot get client endpoint: %s");
                        fmt % boost::this_thread::get_id() % ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 10, fmt.str(), __FILE__, __LINE__);
                        !ssl_flag ? client_socket.close(ec) : client_ssl_socket.close(ec);
                        if (ec) {
                                boost::format   fmt2("Thread ID[%s] cannot close endpoint : %s");
                                fmt2 % boost::this_thread::get_id() % ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 11, fmt2.str(), __FILE__, __LINE__);
                        }
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1); // set exit flag true
                }
        }


        // set client socket options(receive buffer size)
        if (likely(!exit_flag) && likely(upstream_buffer_size > 0)) {
                boost::asio::socket_base::receive_buffer_size opt(upstream_buffer_size);

                !ssl_flag ? client_socket.get_socket().lowest_layer().set_option(opt, ec)
                : client_ssl_socket.get_socket().lowest_layer().set_option(opt, ec);

                if (unlikely(ec)) {
                        boost::format   fmt("Thread ID[%d] cannot set client socket receive buffer size : %s");
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 12, fmt.str(), __FILE__, __LINE__);
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                }
        }

        // set client socket options(send buffer size)
        if (likely(!exit_flag) && likely(downstream_buffer_size > 0)) {
                boost::asio::socket_base::send_buffer_size opt(downstream_buffer_size);
                !ssl_flag ? client_socket.get_socket().lowest_layer().set_option(opt, ec)
                : client_ssl_socket.get_socket().lowest_layer().set_option(opt, ec);
                if (unlikely(ec)) {
                        boost::format   fmt("Thread ID[%d] cannot set client socket send buffer size : %s");
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 13, fmt.str(), __FILE__, __LINE__);
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                }
        }

        // module initialize
        if (likely(!exit_flag)) {
                protocol_module_base::EVENT_TAG module_event;
                module_event = protocol_module->handle_session_initialize(up_thread_id,
                                down_thread_id,
                                client_endpoint,
                                boost::asio::ip::udp::endpoint());
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_session_initialize( upthread_id = %d , downthread_id = %d ) return: %s");
                        fmt % boost::this_thread::get_id()
                        % up_thread_id
                        % down_thread_id
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
                up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] first module event : %s");
                        fmt %   boost::this_thread::get_id()
                        %       func_tag_to_string(up_thread_next_call_function.first);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        upthread_status_mutex.lock();
        upthread_status = UPTHREAD_ACTIVE;
        upthread_status_mutex.unlock();
        upthread_status_cond.notify_one();
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread loop start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 20, buf.str(), __FILE__, __LINE__);
        }
        while (!exit_flag) {

                // LOCK WAIT MODE
                {
                        boost::mutex::scoped_lock lock(upthread_status_mutex);
                        while (unlikely(upthread_status == UPTHREAD_LOCK)) {
                                to_time(LOCKTIMEOUT, xt);
                                upthread_status_cond.timed_wait(lock, xt);
                                tcp_thread_message *msg = up_thread_message_que.pop();
                                if (msg) {      // message is alive.
                                        if (UP_FUNC_EXIT == up_thread_next_call_function.first) { // don't execute message when UP_FUNC_EXIT
                                                up_thread_next_call_function.second(LOCAL_PROC);
                                                delete msg;
                                                break;
                                        } else {
                                                up_thread_message_data.set_endpoint(msg->endpoint_info);
#ifdef  DEBUG
                                                boost::format   fmt("Thread ID[%d] message execute : %s");
                                                fmt % boost::this_thread::get_id() % msg->func_tag_name;
                                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                                                msg->message(MESSAGE_PROC);     // execute message
                                                delete msg;
                                                msg = NULL;
                                        }
                                        if (!(client_socket.is_open() || client_ssl_socket.is_open())) { //client connection check
                                                upthread_status = UPTHREAD_ACTIVE;
                                                up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
#ifdef  DEBUG
                                                boost::format   fmt("Thread ID[%d] client socket is close. goto finalize");
                                                fmt % boost::this_thread::get_id() ;
                                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                                        }
                                }       //message alive end.
                                if (ssl_flag && up_thread_next_call_function.first == UP_FUNC_CLIENT_ACCEPT_EVENT) {     //handshake timeout check
                                        boost::xtime    now_time;
                                        boost::xtime_get(&now_time, boost::TIME_UTC);
                                        if ((now_time.sec - start_handshake_time.sec) > ssl_handshake_time_out) {        // timeout detect.
                                                boost::system::error_code error_code;
                                                client_ssl_socket.close(error_code);
                                                if (error_code) {
                                                        boost::format   fmt("Thread ID[%d] client SSL socket close fail when handshake timeout : %s");
                                                        fmt % boost::this_thread::get_id() % error_code.message();
                                                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                                                }
                                        }
                                }
                        }
                }       // lockmode while loop end.

                // ACTIVE MODE
                tcp_thread_message *msg = up_thread_message_que.pop(); //message checking
                if (unlikely(msg)) {    //message execute mode
                        if (unlikely(UP_FUNC_EXIT == up_thread_next_call_function.first) ||
                            unlikely(UP_FUNC_CLIENT_DISCONNECT == up_thread_next_call_function.first)) {
                                up_thread_next_call_function.second(LOCAL_PROC);
                        } else {
#ifdef  DEBUG
                                boost::format   fmt("Thread ID[%d] message execute : %s");
                                fmt % boost::this_thread::get_id() % msg->func_tag_name;
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                                up_thread_message_data.set_endpoint(msg->endpoint_info);
                                msg->message(MESSAGE_PROC);     //execute message
                        }
                        delete  msg;
                        msg = NULL;
                } else { // function exec mode
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] up_thread_next_call_function : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(up_thread_next_call_function.first);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                        up_thread_next_call_function.second(LOCAL_PROC);
                }
        }       // thread main loop end.


        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] up thread loop end");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 21, fmt.str(), __FILE__, __LINE__);
        }

        {
                // down thread waiting start
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                while (downthread_status != DOWNTHREAD_COMPLETE && downthread_status != DOWNTHREAD_SLEEP) {
                        to_time(LOCKTIMEOUT, xt);
                        downthread_status_cond.timed_wait(lock, xt);

                        tcp_thread_message *down_msg = new tcp_thread_message();
                        down_msg->message = down_que_function_map[DOWN_FUNC_EXIT];
#ifdef  DEBUG
                        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_EXIT);
                        {
                                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_EXIT);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        }
#endif
                        while (!down_thread_message_que.push(down_msg)) {}
                        downthread_status_cond.notify_one();
                }
        }

        up_thread_all_socket_close();
        connecting_socket.reset();

        if (likely(protocol_module != NULL))    protocol_module->handle_session_finalize(up_thread_id, down_thread_id);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread called handle_session_finalize";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 24, buf.str(), __FILE__, __LINE__);
        }

        if (ssl_flag) {
                client_ssl_socket.wait_async_event_all_end();
                client_ssl_socket.clear_socket();
        }

        upthread_status = UPTHREAD_SLEEP;
        // sockets finalize
        realserver_socket_mutex.lock();
        if (realserver_socket) realserver_socket.reset();
        realserver_socket_mutex.unlock();
        realserver_endpoint = boost::asio::ip::tcp::endpoint();


        // clear messages
        tcp_thread_message     *msg = up_thread_message_que.pop();
        while (msg) {
                delete msg;
                msg = up_thread_message_que.pop();
        }

        msg = down_thread_message_que.pop();
        while (msg) {
                delete msg;
                msg = down_thread_message_que.pop();
        }

        if (!__sync_bool_compare_and_swap(&exit_flag, 1, 2)) {   // release session onece time.
                parent_service.release_session(this);
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 18, formatter.str(), __FILE__, __LINE__);
        }
}

//! down stream thread main function
void tcp_session::down_thread_run()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 27, formatter.str(), __FILE__, __LINE__);
        }

        down_thread_id = boost::this_thread::get_id();
        boost::xtime    xt;

        downthread_status_mutex.lock();
        downthread_status = DOWNTHREAD_LOCK;
        downthread_status_mutex.unlock();
        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];
        {
                boost::mutex::scoped_lock       lock(upthread_status_mutex);
                while (upthread_status < UPTHREAD_ACTIVE) {
                        to_time(LOCKTIMEOUT, xt);
                        upthread_status_cond.timed_wait(lock, xt);
                }
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Therad ID[%d] down thread loop start");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 30, fmt.str(), __FILE__, __LINE__);
        }

        while (!exit_flag) { // down thread main loop.

#ifdef  DEBUG
                down_thread_function_pair old_func;
#endif
                {
                        boost::mutex::scoped_lock lock(downthread_status_mutex);
                        while (unlikely(downthread_status == DOWNTHREAD_LOCK)) {   // waiting loop
                                to_time(LOCKTIMEOUT, xt);
                                downthread_status_cond.timed_wait(lock, xt);
                                tcp_thread_message *msg = down_thread_message_que.pop();
                                if (msg) {
                                        if (DOWN_FUNC_EXIT == down_thread_next_call_function.first) {
                                                down_thread_next_call_function.second(LOCAL_PROC);
                                                delete msg;
                                                msg = NULL;
                                                break;
                                        } else {
                                                down_thread_message_data.set_endpoint(msg->endpoint_info);
#ifdef  DEBUG
                                                boost::format   fmt("Thread ID[%d] message execute : %s");
                                                fmt % boost::this_thread::get_id() % msg->func_tag_name;
                                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                                                msg->message(MESSAGE_PROC);
                                                delete msg;
                                                msg = NULL;
                                        }
                                }
                        }       // waiting loop end
                }
                tcp_thread_message *msg = down_thread_message_que.pop();
                if (unlikely(msg)) {
                        if (unlikely(DOWN_FUNC_EXIT == down_thread_next_call_function.first)) {
                                down_thread_next_call_function.second(LOCAL_PROC);
                        } else {
#ifdef  DEBUG
                                boost::format   fmt("Thread ID[%d] message execute : %s");
                                fmt % boost::this_thread::get_id() % msg->func_tag_name;
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                                down_thread_message_data.set_endpoint(msg->endpoint_info);
                                msg->message(MESSAGE_PROC);
                        }
                        delete  msg;
                        msg = NULL;
                } else {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] down_thread_next_call_function : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(down_thread_next_call_function.first);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);

#endif
                        down_thread_next_call_function.second(LOCAL_PROC);
#ifdef  DEBUG
                        old_func = down_thread_next_call_function;
#endif
                }
        }       // down thread main loop end.

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] down thread loop end");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 31,  fmt.str(), __FILE__, __LINE__);
        }

        {
                // waiting down thread loop.
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] downthread_status = DOWNTHREAD_SLEEP");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 31,  fmt.str(), __FILE__, __LINE__);
                }
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                downthread_status = DOWNTHREAD_SLEEP;
                downthread_status_cond.notify_one();
        }

        // clear down thread message
        {
                tcp_thread_message *msg = down_thread_message_que.pop();
                while (msg) {
                        delete msg;
                        msg = down_thread_message_que.pop();
                }

        }

        if (!__sync_bool_compare_and_swap(&exit_flag, 1, 2)) {   // execute onetime on 2 threads.
                parent_service.release_session(this);
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
        }
}


//! up thread accept client side
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_accept(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_accept");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        UP_THREAD_FUNC_TYPE_TAG func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;

        boost::system::error_code       error_code;

        if (ssl_flag) {
                upthread_status = UPTHREAD_LOCK;
                // try ssl handshake
                boost::xtime_get(&start_handshake_time, boost::TIME_UTC);
                client_ssl_socket.setoption(error_code);
                client_ssl_socket.async_handshake(boost::bind(&tcp_session::up_thread_client_handshake_handle,
                                                  this,
                                                  boost::asio::placeholders::error));
        } else {
                upthread_status = UPTHREAD_ACTIVE;
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_accept: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up thread message after client handshake
void tcp_session::up_thread_client_handshake_handle(const boost::system::error_code &error_code)
{

        if (ssl_flag) {
                client_ssl_socket.decrement_handshake_con();
        }

        tcp_thread_message     *up_msg = new tcp_thread_message();
        if (!error_code) {
                up_msg->message = up_que_function_map[UP_FUNC_CLIENT_ACCEPT_EVENT];
#ifdef  DEBUG
                up_msg->func_tag_name = func_tag_to_string(UP_FUNC_CLIENT_ACCEPT_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_CLIENT_ACCEPT_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
        } else {
                handshake_error_code = error_code;
                up_msg->message = up_que_function_map[UP_FUNC_CLIENT_ACCEPT_FAIL_EVENT];
#ifdef  DEBUG
                up_msg->func_tag_name = func_tag_to_string(UP_FUNC_CLIENT_ACCEPT_FAIL_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s / error_code = %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_CLIENT_ACCEPT_FAIL_EVENT) % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        while (!up_thread_message_que.push(up_msg)) {}
        upthread_status_cond.notify_one();
}

//! up thread raise module event of handle_accept and do handshake
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_accept_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (ssl_flag) upthread_status = UPTHREAD_ACTIVE;

        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_accept(up_thread_id);
#ifdef  DEBUG
        boost::format   fmt("Thread ID[%d] protocol_module->handle_accept() return : %s");
        fmt % boost::this_thread::get_id()
        % func_tag_to_string(module_event);
        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
        up_thread_data_client_side.set_endpoint(client_endpoint);

        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_accept_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}


//! accept error event.
void tcp_session::up_thread_client_accept_fail_event(const TCP_PROCESS_TYPE_TAG proc)
{

        UP_THREAD_FUNC_TYPE_TAG func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;

        if (handshake_error_code == boost::asio::error::try_again) {
                upthread_status = UPTHREAD_LOCK;
                tcp_ssl_socket::async_handshake_handler_t   handler;
                handler = boost::bind(&tcp_session::up_thread_client_handshake_handle, this, boost::asio::placeholders::error);
                client_ssl_socket.async_handshake(handler);
        } else { //error
                func_tag = UP_FUNC_CLIENT_DISCONNECT;
                upthread_status = UPTHREAD_ACTIVE;
                tcp_thread_message *mes = new tcp_thread_message();
                mes->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
                mes->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif

                while (!down_thread_message_que.push(mes)) {}
                downthread_status_cond.notify_one();

                boost::format   fmt("Thread ID[%d] tcp_ssl_socket::handshake[%s]");
                fmt % boost::this_thread::get_id() % handshake_error_code.message();
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 71, fmt.str(), __FILE__, __LINE__);

        }
        up_thread_next_call_function = up_thread_function_array[func_tag];
}

//! real server remove
//! @param[in]        target endpoint
void tcp_session::realserver_remove(endpoint &target_endpoint)
{
        tcp_thread_message *up_msg = new tcp_thread_message;
        up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_REALSERVER_CHECK];
        up_msg->message = up_func.second;
        up_msg->endpoint_info = target_endpoint;
#ifdef  DEBUG
        up_msg->func_tag_name = func_tag_to_string(UP_FUNC_REALSERVER_CHECK);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_REALSERVER_CHECK);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(up_msg)) {}
        upthread_status_cond.notify_one();
}



//! up thread receive client side and raise module event of handle_client_recv
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_receive");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (unlikely(0 < parent_service.get_wait_upstream())) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "up_thread_client_receive qos wait active";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 35, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                return; // try again
        }
        up_thread_data_client_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_client_side.get_data();
        boost::system::error_code ec;
        std::size_t recv_size;
        UP_THREAD_FUNC_TYPE_TAG func_tag;

        recv_size = !ssl_flag ? client_socket.read_some(
                            boost::asio::buffer(data_buff),
                            ec)
                    : client_ssl_socket.read_some(
                            boost::asio::buffer(data_buff),
                            ec);
        if (!ec) {
                if (recv_size > 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                client_endpoint = !ssl_flag ? client_socket.get_socket().lowest_layer().remote_endpoint(ec)
                                                  : client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                                boost::format formatter("Thread ID[%d] up_thread_client_receive receive data size[%d] from [%d]");
                                formatter % boost::this_thread::get_id() % recv_size % client_endpoint;
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 36, formatter.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        up_thread_data_client_side.set_size(recv_size);
                        parent_service.update_up_recv_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_recv(up_thread_id, data_buff, recv_size);
#ifdef  DEBUG
                        {
                                boost::format   fmt("Thread ID[%d] protocol_module->handle_client_recv() return: %s");
                                fmt % boost::this_thread::get_id()
                                % func_tag_to_string(module_event);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
#endif
                        func_tag = up_thread_module_event_map[module_event];
                } else {
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                }
        } else {
                if (ec == boost::asio::error::eof) {
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                } else if (ec == boost::asio::error::try_again) {
                        upthread_status = UPTHREAD_LOCK;
                        func_tag = UP_FUNC_CLIENT_HANDLE_ARECIEVE;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler
                        = boost::bind(&tcp_session::up_thread_client_async_read_some_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred);
                        !ssl_flag ? client_socket.async_read_some(boost::asio::buffer(data_buff), handler)
                        : client_ssl_socket.async_read_some(boost::asio::buffer(data_buff), handler);

#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] async_read_some execute");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                } else {
                        boost::format formatter("Thread ID[%d] client read error: %s");
                        formatter % boost::this_thread::get_id() % ec.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                }
        }

        up_thread_next_call_function  = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_receive: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise client respond send event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_respond");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        tcp_thread_message *up_msg = new tcp_thread_message;
        up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT];
        up_msg->message = up_func.second;
#ifdef  DEBUG
        up_msg->func_tag_name = func_tag_to_string(UP_FUNC_CLIENT_RESPOND_SEND_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_CLIENT_RESPOND_SEND_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(up_msg)) {}
        upthread_status_cond.notify_one();

        tcp_thread_message *down_msg = new tcp_thread_message;
        down_msg->message = down_que_function_map[DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT];
#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(down_msg)) {}
        downthread_status_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_respond");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_response_send_inform
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_respond_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_response_send_inform_mutex);
                module_event = protocol_module->handle_response_send_inform(up_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_response_send_inform() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif

        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_function_pair func = up_thread_function_array[func_type->second];
        up_thread_next_call_function = func;
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_respond_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread close client socket and raise client disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        UP_THREAD_FUNC_TYPE_TAG func_tag;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN func up_thread_client_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code ec;

        if (!ssl_flag) {
                client_socket.shutdown(ec);
                if (ec) {
                        boost::format   fmt("Thrad ID[%d] client shutdown error : %s ");
                        fmt % boost::this_thread::get_id() % ec.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
                func_tag = UP_FUNC_CLIENT_DISCONNECT_EVENT;
        } else {
                client_ssl_socket.shutdown(ec);
                if (ec == boost::asio::error::try_again) {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] ssl_shutdown fail: %s");
                        fmt % boost::this_thread::get_id() % ec.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                } else if (ec == boost::asio::error::eof) {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] ssl_shutdown fail: %s");
                        fmt % boost::this_thread::get_id() % ec.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = UP_FUNC_CLIENT_DISCONNECT_EVENT;
                } else {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] ssl_shutdown fail: %s");
                        fmt % boost::this_thread::get_id() % ec.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = UP_FUNC_CLIENT_DISCONNECT_EVENT;
                }
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC_OUT up_thread_client_disconnect up_func[%s] down_func[%s]");
                formatter % boost::this_thread::get_id()
                % func_tag_to_string(UP_FUNC_CLIENT_DISCONNECT_EVENT)
                % func_tag_to_string(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
        upthread_status = UPTHREAD_ACTIVE;
        up_thread_next_call_function = up_thread_function_array[func_tag];
}
//! up thread raise module event of handle_client_disconnect
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_client_disconnect_mutex);
                module_event = protocol_module->handle_client_disconnect(up_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_client_disconnect() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif

        }
        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];


        tcp_thread_message     *mes = new tcp_thread_message();
        mes->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(mes)) {}

        upthread_status = UPTHREAD_ACTIVE;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_disconnect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(down_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread send realserver and raise module event of handle_client_recv
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_send");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        std::size_t data_size = up_thread_data_dest_side.get_size();
        std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
        std::size_t send_size = 0;
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if (realserver_socket)
                send_size = realserver_socket->write_some(
                                    boost::asio::const_buffers_1(
                                            data_buff.data() + send_data_size,
                                            data_size - send_data_size
                                    ),
                                    error_code
                            );
        else
                error_code = boost::asio::error::connection_reset;


        if (!error_code) {
                send_data_size += send_size;
                up_thread_data_dest_side.set_send_size(send_data_size);
                parent_service.update_up_send_size(send_size);
                if (data_size > send_data_size) {
                        func_tag = UP_FUNC_REALSERVER_SEND;
                } else {
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_send(up_thread_id);
#ifdef  DEBUG
                        {
                                boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_send() return: %s");
                                fmt % boost::this_thread::get_id()
                                % func_tag_to_string(module_event);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
#endif

                        func_tag = up_thread_module_event_map[module_event];
                }
        } else {
                if (error_code == boost::asio::error::try_again) {
                        upthread_status = UPTHREAD_LOCK;
                        func_tag = UP_FUNC_REALSERVER_HANDLE_ASEND;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::up_thread_realserver_async_write_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        if (realserver_socket)
                                realserver_socket->async_write_some(
                                        boost::asio::const_buffers_1(
                                                data_buff.data() + send_data_size,
                                                data_size - send_data_size
                                        ),
                                        handler
                                );
                        else
                                func_tag = UP_FUNC_REALSERVER_DISCONNECT;
                } else {
                        func_tag = UP_FUNC_REALSERVER_DISCONNECT;
                        boost::format   fmt("Thread ID[%d] realserver send error: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }

        up_thread_next_call_function = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_send: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

}

//! up thread raise module event of handle_realserver_select
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_get_destination_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_select(up_thread_id, connecting_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_select() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        if (unlikely(module_event == protocol_module_base::SORRYSERVER_SELECT)) {  //sorry_server mode change when realserver select fail.
                tcp_thread_message     *down_msg = new tcp_thread_message();
                down_msg->message = down_que_function_map[DOWN_FUNC_SORRY_ENABLE_EVENT];
#ifdef  DEBUG
                down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_SORRY_ENABLE_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_SORRY_ENABLE_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                while (!down_thread_message_que.push(down_msg)) {}
                downthread_status_cond.notify_one();
        }

        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_get_destination_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
void tcp_session::up_thread_realserver_connect_handler(
        const boost::system::error_code &error)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN up_thread_realserver_connect_handler");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }


        tcp_thread_message *msg = new tcp_thread_message();
        if (!error) {
                up_thread_function_pair func = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT];
                msg->message = func.second;
#ifdef  DEBUG
                msg->func_tag_name = func_tag_to_string(UP_FUNC_REALSERVER_CONNECT_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_REALSERVER_CONNECT_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
        } else {
                up_thread_function_pair func = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT];
                msg->message = func.second;
#ifdef  DEBUG
                msg->func_tag_name = func_tag_to_string(UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                boost::format   fmt("Thread ID[%d] connection error : %s");
                fmt % boost::this_thread::get_id() % error.message();
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
        while (!up_thread_message_que.push(msg)) {}
        upthread_status_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT up_thread_realserver_connect_handler");
                fmt % boost::this_thread::get_id() ;
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
}

//! up thread connect realserver
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_connect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;

        if (realserver_socket && realserver_socket->is_open()) {
                up_thread_data_dest_side.initialize();
                boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
                size_t data_size = 0;
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connect(up_thread_id, data_buff, data_size);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_connect() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
                up_thread_data_dest_side.set_size(data_size);
                // next call function setup.
                up_thread_next_call_function = up_thread_function_array[ up_thread_module_event_map[module_event] ];
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connect_event: NEXT_FUNC[%s]");
                        formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_module_event_map[module_event]);
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                }
                return ;
        }

        connecting_socket.reset(new tcp_socket(parent_dispatcher, socket_opt_info));

        upthread_status = UPTHREAD_LOCK;
        up_thread_next_call_function = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT];

        // down thread_pause on
        tcp_thread_message *msg = new tcp_thread_message();
        msg->message = down_que_function_map[DOWN_FUNC_PAUSE_ON_EVENT];

#ifdef  DEBUG
        msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(msg)) {}
        downthread_status_cond.notify_one();

        connecting_socket->open(connecting_endpoint);
        connecting_socket->setoption(error_code);

#ifdef IP_TRANSPARENT
        if (parent_service.is_realserver_transparent(connecting_endpoint)) {
                if ((connecting_endpoint.address().is_v4() && client_endpoint.address().is_v4()) ||
                    (connecting_endpoint.address().is_v6() && client_endpoint.address().is_v6())) {
                        connecting_socket->set_transparent(client_endpoint, error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] realserver[%s:%d] socket option(IP_TRANSPARENT) set failed:");
                                fmt % boost::this_thread::get_id()
                                % connecting_endpoint.address().to_string()
                                % connecting_endpoint.port();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }
#endif

        connecting_socket->async_connect(
                connecting_endpoint,
                boost::bind(
                        &tcp_session::up_thread_realserver_connect_handler,
                        this,
                        boost::asio::placeholders::error
                )
        );

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_realserver_connect
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_connect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;

        if (unlikely(error_code)) {
                boost::format   fmt("Thread ID[%d] realserver socket option error %s");
                fmt % boost::this_thread::get_id() % error_code.message();
                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 99, fmt.str(), __FILE__, __LINE__);
        }

        //set realserver socket receive buffer size
        if (likely(downstream_buffer_size > 0)) {
                boost::asio::socket_base::receive_buffer_size   buf_size(downstream_buffer_size);
                connecting_socket->get_socket().set_option(buf_size, error_code);
                if (unlikely(error_code)) {
                        boost::format   formatter("Thread ID[%d] realserver socket receive buffer size error: %s");
                        formatter % boost::this_thread::get_id % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 35, formatter.str(), __FILE__, __LINE__);

                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
                        upthread_status = UPTHREAD_ACTIVE;
                        return;
                }
        }

        //set realserver socket send buffer size
        if (likely(upstream_buffer_size > 0)) {
                boost::asio::socket_base::send_buffer_size      buf_size(upstream_buffer_size);
                connecting_socket->get_socket().set_option(buf_size, error_code);
                if (unlikely(error_code)) {
                        boost::format   formatter("Thread ID[%d] realserver socket send buffer size error: %s");
                        formatter % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 35, formatter.str(), __FILE__, __LINE__);

                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
                        upthread_status = UPTHREAD_ACTIVE;
                        return;
                }
        }

        //realserver socket alive.
        realserver_socket_mutex.lock();
        realserver_socket.swap(connecting_socket);
        realserver_socket_mutex.unlock();
        realserver_endpoint = connecting_endpoint;

        //set virtualservice connection active
        parent_service.connection_active(realserver_endpoint);

        // out put access log
        rd_scoped_lock scoped_lock(access_log_flag_mutex);
        if (access_log_flag && access_logger != NULL) {
                endpoint server_side_endpoint =
                        realserver_socket->get_socket().local_endpoint(error_code);
                std::string reserv_msg("");
                access_logger->putLog(
                        endpoint_to_string(virtualservice_endpoint),
                        endpoint_to_string(client_endpoint),
                        endpoint_to_string(server_side_endpoint),
                        endpoint_to_string(realserver_endpoint),
                        reserv_msg);
        }

        // up thread pause off
        upthread_status = UPTHREAD_ACTIVE;

        // down thread pause off
        tcp_thread_message     *msg = new tcp_thread_message();
        msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];

#ifdef  DEBUG
        msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(msg)) {}
        downthread_status_cond.notify_one();

        // dist_side data initialize
        up_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connect(up_thread_id, data_buff, data_size);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_connect() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        up_thread_data_dest_side.set_endpoint(realserver_endpoint);
        up_thread_data_dest_side.set_size(data_size);

        // connecting values reset.
        connecting_socket.reset();
        connecting_endpoint = boost::asio::ip::tcp::endpoint();

        // next call function setup.
        up_thread_next_call_function = up_thread_function_array[ up_thread_module_event_map[module_event] ];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_module_event_map[module_event]);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_realserver_connection_fail
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_connection_fail_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        // connecting socket clear
        connecting_socket.reset();
        connecting_endpoint = boost::asio::ip::tcp::endpoint();

        // module event call
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connection_fail(up_thread_id, connecting_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_connection_fail() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif

        up_thread_next_call_function = up_thread_function_array[ up_thread_module_event_map[module_event] ];


        // session pause off
        upthread_status = UPTHREAD_ACTIVE;

        tcp_thread_message     *down_msg = new tcp_thread_message();
        down_msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];

#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(down_msg)) {}
        downthread_status_cond.notify_one();


        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connection_fail_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_module_event_map[module_event]);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

}
//! up thread close realserver socket and raise realserver disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        realserver_socket_mutex.lock();
        if (realserver_socket) {
                boost::system::error_code error_code;
                if (realserver_socket->is_open()) {
                        realserver_socket->close(error_code);
                        parent_service.connection_inactive(realserver_endpoint);
                        if (unlikely(error_code)) {
                                boost::format   fmt("Thread ID[%d] realserver_socket close error: %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }
        realserver_socket_mutex.unlock();

        tcp_thread_message *down_msg = new tcp_thread_message;

        down_msg->endpoint_info = realserver_endpoint;
        down_msg->message = down_que_function_map[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT];
#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(down_msg)) {}
        downthread_status_cond.notify_one();

        up_thread_next_call_function = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_client_disconnect
//! @param[in]        process_type is process type
void tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_realserver_disconnect(up_thread_id, realserver_endpoint);
#ifdef  DEBUG
        boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_disconnect() return: %s");
        fmt % boost::this_thread::get_id()
        % func_tag_to_string(module_event);
        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif

        parent_service.connection_inactive(realserver_endpoint);

        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! check and up thread close client socket and raise client disconnect event message for up thread
void tcp_session::up_thread_realserver_check(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_check");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::asio::ip::tcp::endpoint  check_endpoint = up_thread_message_data.get_endpoint();

        if (check_endpoint != realserver_endpoint && check_endpoint != connecting_endpoint) return;

        realserver_socket_mutex.lock();
        if (realserver_socket) {
                boost::system::error_code       error_code;
                if (realserver_socket->is_open()) {
                        realserver_socket->close(error_code);
                        parent_service.connection_inactive(realserver_endpoint);
                }
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] realserver socket close fail: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        realserver_socket_mutex.unlock();

        if (connecting_socket) {
                boost::system::error_code       error_code;
                if (connecting_socket->is_open()) connecting_socket->close(error_code);
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] connecting socket close fail: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        //

        // up thread next call function set REALSERVER DISCONNECT
        up_thread_next_call_function = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT];

        // up thread pause off
        upthread_status  = UPTHREAD_ACTIVE;

        // down thread pause off
        tcp_thread_message *down_msg = new tcp_thread_message;
        down_msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(down_msg)) {}


        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_check");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up thread send sorryserver and raise module event of handle_sorryserver_send
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_send");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code ec;
        endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        std::size_t data_size = up_thread_data_dest_side.get_size();
        std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
        std::size_t send_size;
        UP_THREAD_FUNC_TYPE_TAG func_tag;

        send_size = sorryserver_socket->write_some(boost::asio::const_buffers_1(data_buff.data() + send_data_size, data_size - send_data_size), ec);
        if (!ec) {
                send_data_size += send_size;
                up_thread_data_dest_side.set_send_size(send_data_size);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "up_thread_sorryserver_send send data size[" << send_size << "] for [" << sorry_endpoint << "]";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 39, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                if (data_size > send_data_size) {
                        func_tag = UP_FUNC_SORRYSERVER_SEND;
                } else {
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_send(up_thread_id);
#ifdef  DEBUG
                        {
                                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_send() return: %s");
                                fmt % boost::this_thread::get_id()
                                % func_tag_to_string(module_event);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
#endif
                        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
                        if (unlikely(func_type == up_thread_module_event_map.end())) {
                                //Error unknown protocol_module_base::EVENT_TAG return
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "protocol_module returned illegal EVENT_TAG: " << module_event;
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 49, buf.str(), __FILE__, __LINE__);
                                up_thread_exit(process_type);
                                return;
                        }
                        func_tag = func_type->second;
                }
        } else {
                if (ec == boost::asio::error::try_again) {
                        upthread_status = UPTHREAD_LOCK;
                        func_tag = UP_FUNC_SORRYSERVER_HANDLE_ASEND;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::up_thread_sorryserver_async_write_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        sorryserver_socket->async_write_some(
                                boost::asio::const_buffers_1(data_buff.data() + send_data_size, data_size - send_data_size), handler);
                } else {
                        func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
                }
        }

        up_thread_next_call_function = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_send: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_sorryserver_select
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_get_destination_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        virtualservice_element &vs_element = parent_service.get_element();
        sorryserver_endpoint = vs_element.sorry_endpoint;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_select(up_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_select() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_get_destination_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread connect sorryserver
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_connect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        sorryserver_socket.reset(new tcp_socket(parent_dispatcher, socket_opt_info));
        boost::system::error_code       error_code;

        // up thread pause on
        upthread_status = UPTHREAD_LOCK;
        up_thread_next_call_function = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT];

        // down thread pause on
        tcp_thread_message *msg = new tcp_thread_message();
        msg->message = down_que_function_map[DOWN_FUNC_PAUSE_ON_EVENT];
#ifdef  DEBUG
        msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_ON_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(msg)) {}
        downthread_status_cond.notify_one();

        sorryserver_socket->open(sorryserver_endpoint);
        sorryserver_socket->setoption(error_code);


#ifdef IP_TRANSPARENT
        if (parent_service.get_element().sorry_fwdmode == virtualservice_element::FWD_TPROXY) {
                if ((sorryserver_endpoint.address().is_v4() && client_endpoint.address().is_v4()) ||
                    (sorryserver_endpoint.address().is_v6() && client_endpoint.address().is_v6())) {
                        sorryserver_socket->set_transparent(client_endpoint, error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] realserver[%s:%d] socket option(IP_TRANSPARENT) set failed:");
                                fmt % boost::this_thread::get_id()
                                % connecting_endpoint.address().to_string()
                                % connecting_endpoint.port();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }
#endif


        sorryserver_socket->async_connect(
                sorryserver_endpoint,
                boost::bind(
                        &tcp_session::up_thread_sorryserver_connect_handler,
                        this,
                        boost::asio::placeholders::error
                )
        );

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connect: next call function: %s");
                formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread sorryserver connect handle
//! @param[in] error_code is system error code
void tcp_session::up_thread_sorryserver_connect_handler(const boost::system::error_code &error_code)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN up_thread_sorryserver_connect_handler");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }

        tcp_thread_message *msg = new tcp_thread_message();
        if (!error_code) {
                up_thread_function_pair func = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT];
                msg->message = func.second;
#ifdef DEBUG
                msg->func_tag_name = func_tag_to_string(UP_FUNC_SORRYSERVER_CONNECT_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_SORRYSERVER_CONNECT_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
        } else {
                up_thread_function_pair func = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT];
                msg->message = func.second;
#ifdef DEBUG
                msg->func_tag_name = func_tag_to_string(UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT);
                {
                        boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
#endif
                boost::format   fmt("Thread ID[%d] connection error: %s");
                fmt % boost::this_thread::get_id() % error_code.message();
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
        while (!up_thread_message_que.push(msg)) {}
        upthread_status_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT up_thread_sorryserver_connect_handler");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
}


//! up thread raise module event of handle_sorryserver_connect
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_connect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;

        // set sorryserver socket receive buffer size
        if (likely(downstream_buffer_size > 0)) {
                boost::asio::socket_base::receive_buffer_size   buf_size(downstream_buffer_size);
                sorryserver_socket->get_socket().set_option(buf_size, error_code);
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] realserver socket receive buffer size error: %s");
                        fmt % boost::this_thread::get_id % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
                        upthread_status = UPTHREAD_ACTIVE;
                        return;
                }
        }

        // set sorryserver socet send buffer size
        if (likely(upstream_buffer_size > 0)) {
                boost::asio::socket_base::send_buffer_size  buf_size(upstream_buffer_size);
                connecting_socket->get_socket().set_option(buf_size, error_code);
                if (unlikely(error_code)) {
                        boost::format   fmt("Thread ID[%d] realserver socket send buffer size error: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 35, fmt.str(), __FILE__, __LINE__);
                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
                        upthread_status = UPTHREAD_ACTIVE;
                        return;
                }
        }

        // up thread pause off
        upthread_status = UPTHREAD_ACTIVE;

        // down thread pause off
        tcp_thread_message *msg = new tcp_thread_message();
        msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];

#ifdef  DEBUG
        msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif

        while (!down_thread_message_que.push(msg)) {}
        downthread_status_cond.notify_one();

        // dist side data initialize
        up_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connect(up_thread_id, data_buff, data_size);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_connect() return: %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif

        up_thread_data_dest_side.set_endpoint(sorryserver_endpoint);
        up_thread_data_dest_side.set_size(data_size);

        // next call function setup
        up_thread_next_call_function = up_thread_function_array[ up_thread_module_event_map[module_event] ];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_sorryserver_connection_fail
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_connection_fail_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connection_fail(up_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_connection_fail() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connection_fail_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        // up thread pause off
        upthread_status  = UPTHREAD_ACTIVE;

        //sorryserver socket clear
        sorryserver_socket.reset();
        sorryserver_endpoint = boost::asio::ip::tcp::endpoint();

}
//! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        // sorryserver socket clear
        sorryserver_socket.reset();
        sorryserver_endpoint = boost::asio::ip::tcp::endpoint();

        // module event call
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connection_fail(up_thread_id, realserver_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_connection_fail() return: %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif

        up_thread_next_call_function = up_thread_function_array[ up_thread_module_event_map[module_event] ];

        // up thread pause off
        upthread_status = UPTHREAD_ACTIVE;

        // down thread pause off
        tcp_thread_message *down_msg = new tcp_thread_message();
        down_msg->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        down_msg->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif

        while (!down_thread_message_que.push(down_msg)) {}
        downthread_status_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread close sorryserver socket and raise module sorryserver disconnect event
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_mod_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (sorryserver_socket) {
                boost::system::error_code       error_code;
                if (sorryserver_socket->is_open()) {
                        sorryserver_socket->close(error_code);
                        if (unlikely(error_code)) {
                                boost::format   fmt("Thread ID[%d] cannot close sorryserver socket");
                                fmt % boost::this_thread::get_id();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }


        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_disconnect() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif

        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_mod_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up thread raise module event of handle_sorryserver_disconnect
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_disconnect() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif

        up_thread_next_call_function = up_thread_function_array[up_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_disconnect_event NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(up_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_sorry_enable
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorry_enable_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_sorry_enable_mutex);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "handle_sorry_enable call";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 40, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                module_event = protocol_module->handle_sorry_enable(up_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_sorry_enable() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorry_enable_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread raise module event of handle_sorry_disable
//! @param[in]        process_type is process type
void tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorry_disable_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_sorry_disable_mutex);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "handle_sorry_disable call";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 41, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                module_event = protocol_module->handle_sorry_disable(up_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_sorry_disable() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorry_disable_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up thread exit main loop
//! @param[in]        process_type is process type
void tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN func up_thread_exit");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
        upthread_status = UPTHREAD_ACTIVE;
        __sync_bool_compare_and_swap(&exit_flag, 0, 1);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT func up_thread_exit");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread close all socket
void tcp_session::up_thread_all_socket_close()
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;

        realserver_socket_mutex.lock();
        if (realserver_socket) {
                if (realserver_socket->is_open()) {
                        realserver_socket->close(error_code);
                        parent_service.connection_inactive(realserver_endpoint);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] realserver socket close fail : %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }
        realserver_socket_mutex.unlock();

        if (connecting_socket) {
                if (connecting_socket->is_open()) {
                        connecting_socket->close(error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID{%d] connecting socket clise fail : %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
                        connecting_socket.reset();
                        connecting_endpoint = boost::asio::ip::tcp::endpoint();
                }
        }

        if (sorryserver_socket) {
                if (sorryserver_socket->is_open()) {
                        sorryserver_socket->close(error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] sorryserver socket close fail : %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }

        !ssl_flag ? client_socket.close(error_code) : client_ssl_socket.close(error_code);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up_thread_session_pause_on event

void tcp_session::up_thread_session_pause_on_event(const tcp_session::TCP_PROCESS_TYPE_TAG type)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN up_thread_session_pause_on_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }

        upthread_status = UPTHREAD_LOCK;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT up_thread_session_pause_on_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);

        }

}

//! up_thread_session_pause off event
void tcp_session::up_thread_session_pause_off_event(const tcp_session::TCP_PROCESS_TYPE_TAG type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN up_thread_session_pause_off_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }

        upthread_status = UPTHREAD_ACTIVE;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT up_thread_session_pause_off_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);

        }
}


//! down thread receive from realserver and raise module event of handle_realserver_recv
//! @param[in]        process_type is process type
void tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_realserver_receive");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (unlikely(0 < parent_service.get_wait_downstream())) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "down_thread_realserver_receive qos wait active";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 42, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                return;
        }
        if (unlikely(!realserver_socket)) return;

        DOWN_THREAD_FUNC_TYPE_TAG func_tag;

        down_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code error_code;
        size_t recv_size = 0;

        down_thread_data_dest_side.set_endpoint(realserver_endpoint);

        realserver_socket_mutex.lock();
        if (realserver_socket)
                recv_size = realserver_socket->read_some(boost::asio::buffer(data_buff, MAX_BUFFER_SIZE), error_code);
        else {
                error_code = boost::system::error_code(EBADF, boost::asio::error::get_system_category());
        }
        realserver_socket_mutex.unlock();

        if (!error_code) {
                if (recv_size > 0) {
                        down_thread_data_dest_side.set_size(recv_size);
                        parent_service.update_down_recv_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_recv(
                                                down_thread_id,
                                                realserver_endpoint,
                                                data_buff,
                                                recv_size);
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_recv() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = down_thread_module_event_map[module_event];
                } else {
                        func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                }
        } else {
                if (error_code == boost::asio::error::try_again) {
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_REALSERVER_HANDLE_ARECEIVE;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::down_thread_realserver_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        realserver_socket_mutex.lock();
                        if (realserver_socket)
                                realserver_socket->async_read_some(boost::asio::buffer(data_buff), handler);
                        else
                                func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                        realserver_socket_mutex.unlock();
                } else {
                        func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                }
        }

        down_thread_next_call_function = down_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_receive: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }


        tcp_thread_message *up_msg = new tcp_thread_message();
        up_msg->message = up_que_function_map[UP_FUNC_REALSERVER_DISCONNECT];
#ifdef  DEBUG
        up_msg->func_tag_name = func_tag_to_string(UP_FUNC_REALSERVER_DISCONNECT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_REALSERVER_DISCONNECT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(up_msg)) {}

        downthread_status = DOWNTHREAD_ACTIVE;

        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}


// ! down thread realserver check event.
//! @param[in]  process_type is process type
void tcp_session::down_thread_realserver_check(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN down_thread_realserver_check");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }

        downthread_status = DOWNTHREAD_ACTIVE;
        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_CLIENT_SEND];


        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT down_thread_realserver_check next_call_function : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(down_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
}

//! down thread raise module event of handle_realserver_disconnect
//! @param[in]        process_type is process type
void tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_realserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(down_thread_id, realserver_endpoint);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_realserver_disconnect() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }

        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread raise module event of handle_client_connection_check
//! @param[in]        process_type is process type
void tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_connection_chk_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        down_thread_data_client_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t data_size;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_connection_check(down_thread_id, data_buff, data_size);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_client_connection_check() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        down_thread_data_client_side.set_size(data_size);

        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_connection_chk_event:");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! down thread raise module event of handle_response_send_inform
//! @param[in]        process_type is process type
void tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_respond_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_response_send_inform_mutex);
                module_event = protocol_module->handle_response_send_inform(down_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_response_send_inform() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_respond_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread send for client and raise module event of handle_client_send
//! @param[in]        process_type is process type
void tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_send");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code error_code;
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t data_size = down_thread_data_client_side.get_size();
        std::size_t send_data_size = down_thread_data_client_side.get_send_size();
        std::size_t send_size;
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;

        send_size = !ssl_flag ? client_socket.write_some(
                            boost::asio::const_buffers_1(
                                    data_buff.data() + send_data_size,
                                    data_size - send_data_size
                            ),
                            error_code
                    )
                    : client_ssl_socket.write_some(
                            boost::asio::const_buffers_1(
                                    data_buff.data() + send_data_size,
                                    data_size - send_data_size),
                            error_code
                    );

        if (!error_code) {
                send_data_size += send_size;
                down_thread_data_client_side.set_send_size(send_data_size);
                parent_service.update_down_send_size(send_size);
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        client_endpoint = !ssl_flag ? client_socket.get_socket().lowest_layer().remote_endpoint(error_code)
                                          : client_ssl_socket.get_socket().lowest_layer().remote_endpoint(error_code);
                        boost::format formatter("Thread ID[%d] down_thread_client_send send data size[%d] for [%d]");
                        formatter % boost::this_thread::get_id() % send_size % client_endpoint;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 45, formatter.str(), __FILE__, __LINE__);
                }
                if (data_size > send_data_size) {
                        func_tag = DOWN_FUNC_CLIENT_SEND;
                } else {
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_send(down_thread_id);
#ifdef  DEBUG
                        {
                                boost::format   fmt("Thread ID[%d] protocol_module->handle_client_send() return: %s");
                                fmt % boost::this_thread::get_id()
                                % func_tag_to_string(module_event);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
#endif
                        func_tag = down_thread_module_event_map[module_event];
                }
        } else {
                if (error_code == boost::asio::error::try_again) {

                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_CLIENT_HANDLE_ASEND;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::down_thread_client_async_write_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        !ssl_flag ? client_socket.async_write_some(
                                boost::asio::const_buffers_1(
                                        data_buff.data() + send_data_size,
                                        data_size - send_data_size
                                ),
                                handler
                        )
                        : client_ssl_socket.async_write_some(
                                boost::asio::const_buffers_1(
                                        data_buff.data() + send_data_size,
                                        data_size - send_data_size
                                ),
                                handler
                        );

                } else {
                        func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
                }
        }

        down_thread_next_call_function = down_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_send: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! down thread close client socket and raise client disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        tcp_thread_message     *mes = new tcp_thread_message();
        mes->message = up_que_function_map[UP_FUNC_CLIENT_DISCONNECT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(UP_FUNC_CLIENT_DISCONNECT);
        boost::format   fmt("Thread ID[%d] up_queue.push : %s ");
        fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_CLIENT_DISCONNECT);
        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
#endif
        while (!up_thread_message_que.push(mes));
        upthread_status_cond.notify_one();

        downthread_status = DOWNTHREAD_LOCK;
        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! down thread raise module event of handle_client_disconnect
//! @param[in]        process_type is process type
void tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_client_disconnect_mutex);
                module_event = protocol_module->handle_client_disconnect(down_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_client_disconnect() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];


        tcp_thread_message     *mes = new tcp_thread_message();
        mes->message = up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(mes)) {}

        downthread_status = DOWNTHREAD_ACTIVE;


        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_exit");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_receive");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (unlikely(!sorryserver_socket)) {
                down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE];
                return;
        }

        if (unlikely(!sorryserver_socket->get_socket().lowest_layer().is_open())) {
                down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE];
                return;
        }

        down_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code error_code;
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;

        down_thread_data_dest_side.set_endpoint(sorryserver_endpoint);

        size_t recv_size = sorryserver_socket->read_some(boost::asio::buffer(data_buff, MAX_BUFFER_SIZE), error_code);
#ifdef DEBUG
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] down_thread_sorryserver_receive receive data size[%d] from [%s];");
                fmt % boost::this_thread::get_id() % recv_size % sorryserver_endpoint.address().to_string();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 46, fmt.str(), __FILE__, __LINE__);
        }
#endif
        if (!error_code) {
                if (recv_size > 0) {
                        down_thread_data_dest_side.set_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_recv(
                                                down_thread_id,
                                                sorryserver_endpoint,
                                                data_buff,
                                                recv_size);
#ifdef  DEBUG
                        {
                                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_recv() return: %s");
                                fmt % boost::this_thread::get_id() % func_tag_to_string(module_event);
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
#endif
                        func_tag = down_thread_module_event_map[module_event];
                } else {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] recv size == 0");
                        fmt % boost::this_thread::get_id();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                }
        } else {
                if (error_code == boost::asio::error::try_again) {
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_SORRYSERVER_HANDLE_ARECEIVE;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::down_thread_sorryserver_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        sorryserver_socket->async_read_some(boost::asio::buffer(data_buff), handler);
#ifdef  DEBUG
                        boost::format   fmt("Thraed ID[%d] try_agein async read some: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                } else {
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] recv error : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                }
        }

        down_thread_next_call_function = down_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_receive: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread raise module event of handle_sorryserver_disconnect
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        tcp_thread_message     *up_msg = new tcp_thread_message();
        up_msg->message = up_que_function_map[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT];
#ifdef  DEBUG
        up_msg->func_tag_name = func_tag_to_string(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(up_msg)) {}
        upthread_status_cond.notify_one();

        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread close sorryserver socket and raise module sorryserver disconnect event
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_mod_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;
        protocol_module_base::EVENT_TAG module_event;
        if (!sorryserver_socket) {
                if (!sorryserver_socket->is_open()) {
                        sorryserver_socket->close(error_code);
                }
        }
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_disconnect() return: %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }

        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_mod_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread raise module event of handle_sorryserver_disconnect
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id, sorryserver_endpoint);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_sorryserver_disconnect() return: %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_disconnect_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread raise module event of handle_sorry_enable
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorry_enable_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }


        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_sorry_enable(down_thread_id);
#ifdef  DEBUG
        {
                boost::format   fmt("Thread ID[%d] protocol_module->handle_sorry_enable() return: %s");
                fmt % boost::this_thread::get_id()
                % func_tag_to_string(module_event);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
        }
#endif
        down_thread_next_call_function = down_thread_function_array[down_thread_module_event_map[module_event]];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorry_enable_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(down_thread_next_call_function.first);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread raise module event of handle_sorry_disable
//! @param[in]        process_type is process type
void tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorry_disable_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        protocol_module_base::EVENT_TAG module_event;
        {
                rw_scoped_lock scope_lock(module_function_sorry_disable_mutex);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "handle_sorry_disable call";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 48, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                module_event = protocol_module->handle_sorry_disable(down_thread_id);
#ifdef  DEBUG
                {
                        boost::format   fmt("Thread ID[%d] protocol_module-> return:handle_sorry_disable() %s");
                        fmt % boost::this_thread::get_id()
                        % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                }
#endif
        }
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_function_pair func = down_thread_function_array[func_type->second];
        down_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorry_disable_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread exit main loop
//! @param[in]        process_type is process type
void tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_exit");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        downthread_status = DOWNTHREAD_ACTIVE;
        __sync_bool_compare_and_swap(&exit_flag, 0, 1);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_exit");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread close all socket
void tcp_session::down_thread_all_socket_close(void)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code       error_code;

        realserver_socket_mutex.lock();
        if (realserver_socket) {
                if (realserver_socket->is_open()) {
                        realserver_socket->close(error_code);
                        parent_service.connection_inactive(realserver_endpoint);
                        if (error_code) {
                                boost::format   fmt("Thread ID[%d] realserver socket close fail : %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
                }
        }
        realserver_socket_mutex.unlock();


        if (connecting_socket) {
                if (connecting_socket->is_open()) {
                        connecting_socket->close(error_code);
                        if (error_code) {
                                boost::format   fmt("Thread ID{%d] connecting socket clise fail : %s");
                                fmt % boost::this_thread::get_id() % error_code.message();
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
                        }
                        connecting_socket.reset();
                        connecting_endpoint = boost::asio::ip::tcp::endpoint();
                }
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! down_thread_session_pause_on_event
//!
//!
void tcp_session::down_thread_session_pause_on_event(tcp_session::TCP_PROCESS_TYPE_TAG type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN down_thread_session_pause_on_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
        downthread_status = DOWNTHREAD_LOCK;
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT down_thread_session_pause_on_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
}

//! down_thread_session_pause_off_event
//!
//!
void tcp_session::down_thread_session_pause_off_event(tcp_session::TCP_PROCESS_TYPE_TAG type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC IN down_thread_session_pause_off_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
        downthread_status = DOWNTHREAD_ACTIVE;
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format   fmt("Thread ID[%d] FUNC OUT down_thread_session_pause_off_event");
                fmt % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
}


//! up_thrad_client_async_raed_some_hander
//! this function call from other thread(io_service thread).
//!
void tcp_session::up_thread_client_async_read_some_handler(const boost::system::error_code &error_code , std::size_t len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_async_read_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (ssl_flag) {
                client_ssl_socket.decrement_read_con();
        }

        up_thread_data_client_side.set_error_code(error_code);
        up_thread_data_client_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] up_thread_client_async_read_some_handler canceled: %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        tcp_thread_message     *mes = new tcp_thread_message();
        mes->message = up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(mes)) {}

        upthread_status_cond.notify_one();
}

//! up_thread_client_handle_async_read_some
//! this function call form up_thread_run : next_call_function.
void tcp_session::up_thread_client_handle_async_read_some(const tcp_session::TCP_PROCESS_TYPE_TAG proc)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_handle_async_read_some : error=%s : recv byte= %d");
                formatter % boost::this_thread::get_id()
                % up_thread_data_client_side.get_error_code().message()
                % up_thread_data_client_side.get_async_len();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        UP_THREAD_FUNC_TYPE_TAG func_tag;
        boost::system::error_code error_code = up_thread_data_client_side.get_error_code();
        std::size_t     recv_size = up_thread_data_client_side.get_async_len();

        if (!error_code) { // no error
                if (recv_size > 0) {
                        up_thread_data_client_side.set_size(recv_size);
                        parent_service.update_up_recv_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_recv(
                                                up_thread_id,
                                                up_thread_data_client_side.get_data(),
                                                recv_size);
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_client_recv( upthread_id = %d, downthread_id = %d ) return %s");
                        fmt % up_thread_id % down_thread_id % func_tag_to_string(module_event);
#endif

                        func_tag = up_thread_module_event_map[module_event];
                } else { // recv zero byte
                        upthread_status = UPTHREAD_LOCK;
                        func_tag = UP_FUNC_CLIENT_HANDLE_ARECIEVE;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t handler =
                                boost::bind(&tcp_session::up_thread_client_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        !ssl_flag ? client_socket.async_read_some(boost::asio::buffer(up_thread_data_client_side.get_data()), handler)
                        : client_ssl_socket.async_read_some(boost::asio::buffer(up_thread_data_client_side.get_data()), handler);
                }
        } else { // error handling
                if (up_thread_data_client_side.get_error_code() == boost::asio::error::try_again) { // don't recv try again
                        upthread_status = UPTHREAD_LOCK;
                        func_tag = UP_FUNC_CLIENT_HANDLE_ARECIEVE;
                        basic_tcp_socket<boost::asio::ip::tcp>::async_rw_handler_t      handler =
                                boost::bind(&tcp_session::up_thread_client_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        !ssl_flag ? client_socket.async_read_some(boost::asio::buffer(up_thread_data_client_side.get_data()), handler)
                        : client_ssl_socket.async_read_some(boost::asio::buffer(up_thread_data_client_side.get_data()), handler);
                } else if (up_thread_data_client_side.get_error_code() == boost::asio::error::eof) { // hung up
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                } else { // other error.
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                        boost::format   fmt("Thread ID[%d] client read error: %s");
                        fmt % boost::this_thread::get_id() % up_thread_data_client_side.get_error_code().message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }

        up_thread_next_call_function = up_thread_function_array[func_tag];

}

//! up_thread_realserver_async_write_some_handler
//! this function called from other thread( io_service thread ).
//!
void tcp_session::up_thread_realserver_async_write_some_handler(const boost::system::error_code &error_code, std::size_t len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_async_write_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        up_thread_data_dest_side.set_error_code(error_code);
        up_thread_data_dest_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] up_thread_reaslserver_async_write_some_handler canceled : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        tcp_thread_message *mes = new tcp_thread_message();
        mes->message = up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(mes)) {}
        upthread_status_cond.notify_one();
}


void tcp_session::up_thread_realserver_handle_async_write_some(const tcp_session::TCP_PROCESS_TYPE_TAG proc)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_realserver_handle_async_write_some");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        UP_THREAD_FUNC_TYPE_TAG func_tag;

        std::size_t     data_size               = up_thread_data_dest_side.get_size();
        std::size_t     send_data_size  = up_thread_data_dest_side.get_send_size();

        if (!up_thread_data_dest_side.get_error_code()) {        // no error
                send_data_size += up_thread_data_dest_side.get_async_len();
                up_thread_data_dest_side.set_send_size(send_data_size);
                parent_service.update_up_send_size(up_thread_data_dest_side.get_async_len());
                if (data_size > send_data_size)
                        func_tag = UP_FUNC_REALSERVER_SEND;
                else
                        func_tag = up_thread_module_event_map[protocol_module->handle_realserver_send(up_thread_id)];
        } else { // error
                if (up_thread_data_dest_side.get_error_code() == boost::asio::error::try_again) {
                        func_tag = UP_FUNC_REALSERVER_HANDLE_ASEND;
                        upthread_status = UPTHREAD_LOCK;
                        boost::array<char, MAX_BUFFER_SIZE>& send_buff = up_thread_data_dest_side.get_data();
                        tcp_socket::async_rw_handler_t  handler = boost::bind(&tcp_session::up_thread_realserver_async_write_some_handler,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred);
                        if (realserver_socket)
                                realserver_socket->async_write_some(
                                        boost::asio::const_buffers_1(
                                                send_buff.data() + send_data_size,
                                                data_size - send_data_size
                                        ),
                                        handler);
			else
				func_tag = UP_FUNC_REALSERVER_DISCONNECT;
                } else {
                        func_tag = UP_FUNC_REALSERVER_DISCONNECT;
                        boost::format fmt("Thread ID[%d] realserver send error: %s");
                        fmt % boost::this_thread::get_id() % up_thread_data_dest_side.get_error_code().message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];

}

void tcp_session::up_thread_sorryserver_async_write_some_handler(const boost::system::error_code &error_code, std::size_t len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_async_write_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }


        if (ssl_flag) {
                client_ssl_socket.decrement_write_con();
        }
        up_thread_data_dest_side.set_error_code(error_code);
        up_thread_data_dest_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] up_thread_reaslserver_async_write_some_handler canceled : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        tcp_thread_message *mes = new tcp_thread_message();
        mes->message = up_que_function_map[UP_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] up_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!up_thread_message_que.push(mes)) {}
        upthread_status_cond.notify_one();
}

void tcp_session::up_thread_sorryserver_handle_async_write_some(const TCP_PROCESS_TYPE_TAG proc)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_sorryserver_handle_async_write_some");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        UP_THREAD_FUNC_TYPE_TAG func_tag;

        std::size_t     data_size               = up_thread_data_dest_side.get_size();
        std::size_t     send_data_size  = up_thread_data_dest_side.get_send_size();

        if (!up_thread_data_dest_side.get_error_code()) {        //no error
                send_data_size += up_thread_data_dest_side.get_async_len();
                up_thread_data_dest_side.set_send_size(send_data_size);
                if (data_size > send_data_size)
                        func_tag = UP_FUNC_SORRYSERVER_SEND;
                else
                        func_tag = up_thread_module_event_map[protocol_module->handle_realserver_send(up_thread_id)];
        } else {
                if (up_thread_data_dest_side.get_error_code() == boost::asio::error::try_again) {
                        func_tag = UP_FUNC_SORRYSERVER_HANDLE_ASEND;
                        upthread_status = UPTHREAD_LOCK;

                        boost::array<char, MAX_BUFFER_SIZE>& send_buff = up_thread_data_dest_side.get_data();
                        tcp_socket::async_rw_handler_t  handler = boost::bind(&tcp_session::up_thread_sorryserver_async_write_some_handler,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred);
                        sorryserver_socket->async_write_some(
                                boost::asio::const_buffers_1(send_buff.data() + send_data_size, data_size - send_data_size),
                                handler);
                } else {
                        func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
                        boost::format   fmt("Thread ID[%d] sorryserver send error: %s");
                        fmt % boost::this_thread::get_id() % up_thread_data_dest_side.get_error_code().message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];
}


void tcp_session::down_thread_realserver_async_read_some_handler(const boost::system::error_code &error_code, std::size_t len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_realserver_async_read_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        down_thread_data_dest_side.set_error_code(error_code);
        down_thread_data_dest_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) { //error
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] down_thread_realserver_async_read_some_handler candeld : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }


        tcp_thread_message *mes = new tcp_thread_message();
        mes->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(mes)) {}
        downthread_status_cond.notify_one();
}

void tcp_session::down_thread_realserver_handle_async_read_some(const tcp_session::TCP_PROCESS_TYPE_TAG proc)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_realserver_handle_async_raed_some : error = %s : recv_byte = %d");
                formatter % boost::this_thread::get_id()
                % down_thread_data_dest_side.get_error_code().message()
                % down_thread_data_dest_side.get_async_len();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        DOWN_THREAD_FUNC_TYPE_TAG       func_tag;
        boost::system::error_code       error_code = down_thread_data_dest_side.get_error_code();
        std::size_t     recv_size = down_thread_data_dest_side.get_async_len();

        if (!error_code) {       //no error
                if (down_thread_data_dest_side.get_async_len() > 0) { //recv success
                        down_thread_data_dest_side.set_size(recv_size);
                        parent_service.update_down_recv_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event =
                                protocol_module->handle_realserver_recv(down_thread_id,
                                                realserver_endpoint,
                                                down_thread_data_dest_side.get_data(),
                                                recv_size);
                        func_tag = down_thread_module_event_map[module_event];
                } else { // zero byte recv = try again.
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_REALSERVER_HANDLE_ARECEIVE;
                        tcp_socket::async_rw_handler_t handler = boost::bind(&tcp_session::down_thread_realserver_async_read_some_handler,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred);
                        realserver_socket_mutex.lock();
                        if (realserver_socket)
                                realserver_socket->async_read_some(boost::asio::buffer(down_thread_data_dest_side.get_data()), handler);
                        else
                                func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                        realserver_socket_mutex.unlock();
                }
        } else {        // error
                if (error_code == boost::asio::error::try_again) {
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_REALSERVER_HANDLE_ARECEIVE;
                        tcp_socket::async_rw_handler_t handler = boost::bind(&tcp_session::down_thread_realserver_async_read_some_handler,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred);
                        realserver_socket_mutex.lock();
                        if (realserver_socket)
                                realserver_socket->async_read_some(boost::asio::buffer(down_thread_data_dest_side.get_data()), handler);
                        else
                                func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                        realserver_socket_mutex.lock();
                } else { //recv error
                        func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                        boost::format   fmt("Thread ID[%d] down_thread_realserver_handle_async_read_some recv error:%s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }

        down_thread_next_call_function = down_thread_function_array[func_tag];

}

void tcp_session::down_thread_client_async_write_some_handler(const boost::system::error_code &error_code, std::size_t len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_async_write_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (ssl_flag) {
                client_ssl_socket.decrement_write_con();
        }

        down_thread_data_client_side.set_error_code(error_code);
        down_thread_data_client_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] down_thread_client_async_write_some_handle candeld : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }

        tcp_thread_message *mes = new tcp_thread_message();
        mes->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(mes)) {}
        downthread_status_cond.notify_one();

}

void tcp_session::down_thread_client_handle_async_write_some(tcp_session::TCP_PROCESS_TYPE_TAG proc)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_client_handle_async_write_some");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        DOWN_THREAD_FUNC_TYPE_TAG       func_tag;
        boost::system::error_code       error_code = down_thread_data_client_side.get_error_code();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t                                     data_size  = down_thread_data_client_side.get_size();
        std::size_t                                     send_data_size = down_thread_data_client_side.get_send_size();
        std::size_t                                     send_size  = down_thread_data_client_side.get_async_len();

        if (!error_code) {       //no error
                send_data_size += send_size;
                down_thread_data_client_side.set_send_size(send_data_size);
                parent_service.update_down_send_size(send_size);
                if (data_size > send_data_size) {
                        func_tag = DOWN_FUNC_CLIENT_SEND;
                } else {
                        protocol_module_base::EVENT_TAG module_event =
                                protocol_module->handle_client_send(down_thread_id);
#ifdef  DEBUG
                        boost::format   fmt("Thread ID[%d] protocol_module->handle_client_send() return: %s");
                        fmt % boost::this_thread::get_id() % func_tag_to_string(module_event);
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 0, fmt.str(), __FILE__, __LINE__);
#endif
                        func_tag = down_thread_module_event_map[module_event];
                }
        } else { //error
                if (error_code == boost::asio::error::try_again) {
                        func_tag = DOWN_FUNC_CLIENT_HANDLE_ASEND;
                        downthread_status = DOWNTHREAD_LOCK;
                        tcp_socket::async_rw_handler_t  handler = boost::bind(
                                                &tcp_session::down_thread_client_async_write_some_handler,
                                                this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred);
                        !ssl_flag ? client_socket.async_write_some(
                                boost::asio::const_buffers_1(data_buff.data() + send_data_size, data_size - send_data_size),
                                handler)
                        : client_ssl_socket.async_write_some(
                                boost::asio::const_buffers_1(data_buff.data() + send_data_size, data_size - send_data_size),
                                handler);
                } else {
                        func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
                }
        }
        down_thread_next_call_function = down_thread_function_array[func_tag];
}

void tcp_session::down_thread_sorryserver_async_read_some_handler(const boost::system::error_code &error_code, std::size_t      len)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_async_read_some_handler");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (ssl_flag) {
                client_ssl_socket.decrement_read_con();
        }

        down_thread_data_dest_side.set_error_code(error_code);
        down_thread_data_dest_side.set_async_len(len);

        if (error_code == boost::asio::error::operation_aborted) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format   fmt("Thread ID[%d] down_thread_realserver_async_read_some_handler candeld : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        tcp_thread_message *mes = new tcp_thread_message();
        mes->message = down_que_function_map[DOWN_FUNC_PAUSE_OFF_EVENT];
#ifdef  DEBUG
        mes->func_tag_name = func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
        {
                boost::format   fmt("Thread ID[%d] down_queue.push : %s");
                fmt % boost::this_thread::get_id() % func_tag_to_string(DOWN_FUNC_PAUSE_OFF_EVENT);
                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
        }
#endif
        while (!down_thread_message_que.push(mes)) {}
        downthread_status_cond.notify_one();
}

void tcp_session::down_thread_sorryserver_handle_async_read_some(tcp_session::TCP_PROCESS_TYPE_TAG)
{

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_sorryserver_handle_async_read_some");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        DOWN_THREAD_FUNC_TYPE_TAG       func_tag;
        boost::system::error_code       error_code = down_thread_data_dest_side.get_error_code();
        std::size_t     recv_size = down_thread_data_dest_side.get_async_len();

        if (!error_code) { // no error
                if (recv_size > 0) { //recv success
                        down_thread_data_dest_side.set_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event =
                                protocol_module->handle_sorryserver_recv(down_thread_id,
                                                sorryserver_endpoint,
                                                down_thread_data_dest_side.get_data(),
                                                recv_size);
                        func_tag = down_thread_module_event_map[module_event];
                } else { // zero byte recv try_again
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_SORRYSERVER_HANDLE_ARECEIVE;
                        tcp_socket::async_rw_handler_t handler =
                                boost::bind(&tcp_session::down_thread_sorryserver_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);
                        sorryserver_socket->async_read_some(boost::asio::buffer(down_thread_data_dest_side.get_data()), handler);
                }
        } else {
                if (error_code == boost::asio::error::try_again) {
                        downthread_status = DOWNTHREAD_LOCK;
                        func_tag = DOWN_FUNC_SORRYSERVER_HANDLE_ARECEIVE;
                        tcp_socket::async_rw_handler_t  handler =
                                boost::bind(&tcp_session::down_thread_sorryserver_async_read_some_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred);

                        sorryserver_socket->async_read_some(boost::asio::buffer(down_thread_data_dest_side.get_data()), handler);
                } else { //recv error
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                        boost::format   fmt("Thread ID[%d] down_thread_sorryserver_handle_async_read_some recv error : %s");
                        fmt % boost::this_thread::get_id() % error_code.message();
                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, fmt.str(), __FILE__, __LINE__);
                }
        }
        down_thread_next_call_function = down_thread_function_array[func_tag];
}


//! milliseconds to boost::xtime converter
void tcp_session::to_time(int in, boost::xtime &xt)
{
        boost::xtime_get(&xt, boost::TIME_UTC);
        xt.sec += (in / 1000);
        xt.nsec += (in % 1000) * 1000000;
        if (xt.nsec >= 1000000000) {
                xt.sec++;
                xt.nsec -= 1000000000;
        }
}

//! endpoint data to string information
////! @param[in]        endpoint is target endpoint object
std::string tcp_session::endpoint_to_string(
        const endpoint &target_endpoint)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN endpoint_to_string");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        std::stringstream ret;
        if (target_endpoint.address().is_v6()) {
                ret << "[" << target_endpoint.address().to_string() << "]:" << target_endpoint.port()
                    ;
        } else {
                ret << target_endpoint.address().to_string() << ":" << target_endpoint.port();
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT endpoint_to_string");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return ret.str();
}

}// namespace l7vs
