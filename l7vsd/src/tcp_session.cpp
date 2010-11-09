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
        io(session_io),
        parent_service(vs),
        exit_flag(0),
        upthread_status(UPTHREAD_SLEEP),
        downthread_status(DOWNTHREAD_SLEEP),
        realserver_connect_status(false),
        protocol_module(NULL),
        client_socket(session_io, set_option),
        upstream_buffer_size(-1),
        downstream_buffer_size(-1),
        virtualservice_endpoint(listen_endpoint),
        access_log_flag(false),
        access_logger(set_access_logger),
        ssl_flag(ssl_mode),
        client_ssl_socket(session_io, set_ssl_context, set_option),
        ssl_context(set_ssl_context),
        ssl_cache_flag(set_ssl_cache_flag),
        ssl_handshake_timer_flag(false),
        ssl_handshake_time_out(set_ssl_handshake_time_out),
        ssl_handshake_time_out_flag(false),
        socket_opt_info(set_option)
{
        // sorryserver socket
        tcp_socket_ptr sorry_socket(new tcp_socket(session_io, socket_opt_info));
        sorryserver_socket.second = sorry_socket;

        // set up_thread_module_event_map
        std::pair<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG> add_up_thread_event;
        add_up_thread_event.first = protocol_module_base::ACCEPT;
        add_up_thread_event.second = UP_FUNC_CLIENT_ACCEPT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::CLIENT_DISCONNECT;
        add_up_thread_event.second = UP_FUNC_CLIENT_DISCONNECT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::CLIENT_RECV;
        add_up_thread_event.second = UP_FUNC_CLIENT_RECEIVE;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::CLIENT_RESPONSE_SEND;
        add_up_thread_event.second = UP_FUNC_CLIENT_RESPOND_SEND;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::REALSERVER_SELECT;
        add_up_thread_event.second = UP_FUNC_REALSERVER_GET_DEST_EVENT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::REALSERVER_CONNECT;
        add_up_thread_event.second = UP_FUNC_REALSERVER_CONNECT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::REALSERVER_SEND;
        add_up_thread_event.second = UP_FUNC_REALSERVER_SEND;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::REALSERVER_DISCONNECT;
        add_up_thread_event.second = UP_FUNC_REALSERVER_ALL_DISCONNECT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::SORRYSERVER_SELECT;
        add_up_thread_event.second = UP_FUNC_SORRYSERVER_GET_DEST;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::SORRYSERVER_CONNECT;
        add_up_thread_event.second = UP_FUNC_SORRYSERVER_CONNECT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::SORRYSERVER_SEND;
        add_up_thread_event.second = UP_FUNC_SORRYSERVER_SEND;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::SORRYSERVER_DISCONNECT;
        add_up_thread_event.second = UP_FUNC_SORRYSERVER_MOD_DISCONNECT;
        up_thread_module_event_map.insert(add_up_thread_event);
        add_up_thread_event.first = protocol_module_base::FINALIZE;
        add_up_thread_event.second = UP_FUNC_EXIT;
        up_thread_module_event_map.insert(add_up_thread_event);

        // set up_thread_function_array


        up_thread_function_array[UP_FUNC_CLIENT_ACCEPT] =
                std::make_pair(UP_FUNC_CLIENT_ACCEPT,  boost::bind(&tcp_session::up_thread_client_accept, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_ACCEPT_EVENT] =
                std::make_pair(UP_FUNC_CLIENT_ACCEPT_EVENT,  boost::bind(&tcp_session::up_thread_client_accept_event, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT] =
                std::make_pair(UP_FUNC_CLIENT_DISCONNECT, boost::bind(&tcp_session::up_thread_client_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT] =
                std::make_pair(UP_FUNC_CLIENT_DISCONNECT_EVENT,  boost::bind(&tcp_session::up_thread_client_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RECEIVE] =
                std::make_pair(UP_FUNC_CLIENT_RECEIVE, boost::bind(&tcp_session::up_thread_client_receive, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND] =
                std::make_pair(UP_FUNC_CLIENT_RESPOND_SEND, boost::bind(&tcp_session::up_thread_client_respond, this, _1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT] =
                std::make_pair(UP_FUNC_CLIENT_RESPOND_SEND_EVENT, boost::bind(&tcp_session::up_thread_client_respond_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_GET_DEST_EVENT] =
                std::make_pair(UP_FUNC_REALSERVER_GET_DEST_EVENT, boost::bind(&tcp_session::up_thread_realserver_get_destination_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT] =
                std::make_pair(UP_FUNC_REALSERVER_CONNECT, boost::bind(&tcp_session::up_thread_realserver_connect, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT] =
                std::make_pair(UP_FUNC_REALSERVER_CONNECT_EVENT, boost::bind(&tcp_session::up_thread_realserver_connect_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT] =
                std::make_pair(UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT, boost::bind(&tcp_session::up_thread_realserver_connection_fail_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_SEND] =
                std::make_pair(UP_FUNC_REALSERVER_SEND, boost::bind(&tcp_session::up_thread_realserver_send, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT] =
                std::make_pair(UP_FUNC_REALSERVER_DISCONNECT, boost::bind(&tcp_session::up_thread_realserver_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT] =
                std::make_pair(UP_FUNC_REALSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::up_thread_realserver_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_REALSERVER_ALL_DISCONNECT] =
                std::make_pair(UP_FUNC_REALSERVER_ALL_DISCONNECT, boost::bind(&tcp_session::up_thread_all_realserver_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_GET_DEST] =
                std::make_pair(UP_FUNC_SORRYSERVER_GET_DEST, boost::bind(&tcp_session::up_thread_sorryserver_get_destination_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT] =
                std::make_pair(UP_FUNC_SORRYSERVER_CONNECT, boost::bind(&tcp_session::up_thread_sorryserver_connect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT] =
                std::make_pair(UP_FUNC_SORRYSERVER_CONNECT_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_connect_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT] =
                std::make_pair(UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_connection_fail_event, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_SEND] =
                std::make_pair(UP_FUNC_SORRYSERVER_SEND, boost::bind(&tcp_session::up_thread_sorryserver_send, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT] =
                std::make_pair(UP_FUNC_SORRYSERVER_DISCONNECT, boost::bind(&tcp_session::up_thread_sorryserver_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_MOD_DISCONNECT] =
                std::make_pair(UP_FUNC_SORRYSERVER_MOD_DISCONNECT, boost::bind(&tcp_session::up_thread_sorryserver_mod_disconnect, this, _1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT] =
                std::make_pair(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event, this, _1));
        up_thread_function_array[UP_FUNC_EXIT] =
                std::make_pair(UP_FUNC_EXIT, boost::bind(&tcp_session::up_thread_exit, this, _1));

        // set up_thread_message_down_thread_function_map
        std::pair<DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func> add_up_thread_message_func;
        add_up_thread_message_func.first = DOWN_FUNC_CLIENT_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_disconnect_event, this, _1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_respond_event, this, _1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_REALSERVER_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_realserver_disconnect_event, this, _1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event, this, _1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_SORRY_ENABLE_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_sorry_enable_event, this, _1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);

        // set down_thread_module_event_map
        std::pair<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG> add_down_thread_event;
        add_down_thread_event.first = protocol_module_base::CLIENT_SEND;
        add_down_thread_event.second = DOWN_FUNC_CLIENT_SEND;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::CLIENT_DISCONNECT;
        add_down_thread_event.second = DOWN_FUNC_CLIENT_DISCONNECT;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::CLIENT_CONNECTION_CHECK;
        add_down_thread_event.second = DOWN_FUNC_CLIENT_CONNECTION_CHK;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::REALSERVER_RECV;
        add_down_thread_event.second = DOWN_FUNC_REALSERVER_RECEIVE;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::REALSERVER_DISCONNECT;
        add_down_thread_event.second = DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::SORRYSERVER_RECV;
        add_down_thread_event.second = DOWN_FUNC_SORRYSERVER_RECEIVE;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::SORRYSERVER_DISCONNECT;
        add_down_thread_event.second = DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT;
        down_thread_module_event_map.insert(add_down_thread_event);
        add_down_thread_event.first = protocol_module_base::FINALIZE;
        add_down_thread_event.second = DOWN_FUNC_EXIT;
        down_thread_module_event_map.insert(add_down_thread_event);

        // set down_thread_function_array
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT]    = std::make_pair(DOWN_FUNC_CLIENT_DISCONNECT, boost::bind(&tcp_session::down_thread_client_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_CLIENT_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_client_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_CONNECTION_CHK]    = std::make_pair(DOWN_FUNC_CLIENT_CONNECTION_CHK, boost::bind(&tcp_session::down_thread_client_connection_chk_event, this, _1));
        down_thread_function_array[DOWN_FUNC_CLIENT_SEND]    = std::make_pair(DOWN_FUNC_CLIENT_SEND, boost::bind(&tcp_session::down_thread_client_send, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE]    = std::make_pair(DOWN_FUNC_REALSERVER_RECEIVE, boost::bind(&tcp_session::down_thread_realserver_receive, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT]    = std::make_pair(DOWN_FUNC_REALSERVER_DISCONNECT, boost::bind(&tcp_session::down_thread_realserver_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_realserver_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_ALL_DISCONNECT]    = std::make_pair(DOWN_FUNC_REALSERVER_ALL_DISCONNECT, boost::bind(&tcp_session::down_thread_all_realserver_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE]    = std::make_pair(DOWN_FUNC_SORRYSERVER_RECEIVE, boost::bind(&tcp_session::down_thread_sorryserver_receive, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_DISCONNECT, boost::bind(&tcp_session::down_thread_sorryserver_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT, boost::bind(&tcp_session::down_thread_sorryserver_mod_disconnect, this, _1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event, this, _1));
        down_thread_function_array[DOWN_FUNC_EXIT]    = std::make_pair(DOWN_FUNC_EXIT, boost::bind(&tcp_session::down_thread_exit, this, _1));

        // set down_thread_message_up_thread_function_map
        std::pair<UP_THREAD_FUNC_TYPE_TAG, tcp_session_func> add_down_thread_message_func;
        add_down_thread_message_func.first = UP_FUNC_CLIENT_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_client_disconnect_event, this, _1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
        add_down_thread_message_func.first = UP_FUNC_REALSERVER_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_realserver_disconnect_event, this, _1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
        add_down_thread_message_func.first = UP_FUNC_SORRYSERVER_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event, this, _1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);

        // set virtual_service_message_up_thread_function_map
        std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func> add_up_thread_vs_message_func;
        add_up_thread_vs_message_func.first = SORRY_STATE_ENABLE;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_enable_event, this, _1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
        add_up_thread_vs_message_func.first = SORRY_STATE_DISABLE;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_disable_event, this, _1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
        add_up_thread_vs_message_func.first = SESSION_END;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_exit, this, _1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);

        // set virtual_service_message_down_thread_function_map
        std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func> add_down_thread_vs_message_func;
        add_down_thread_vs_message_func.first = SORRY_STATE_ENABLE;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_enable_event, this, _1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
        add_down_thread_vs_message_func.first = SORRY_STATE_DISABLE;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_disable_event, this, _1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
        add_down_thread_vs_message_func.first = SESSION_END;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_exit, this, _1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);

        // epoll implement
        up_client_epollfd = epoll_create(EVENT_NUM);
        up_realserver_epollfd = epoll_create(EVENT_NUM);
        up_sorryserver_epollfd = epoll_create(EVENT_NUM);
        down_client_epollfd = epoll_create(EVENT_NUM);
        down_realserver_epollfd = epoll_create(EVENT_NUM);
        down_sorryserver_epollfd = epoll_create(EVENT_NUM);
        up_client_epollfd_registered = false;
        up_realserver_epollfd_registered = false;
        up_sorryserver_epollfd_registered = false;
        down_client_epollfd_registered = false;
        down_realserver_epollfd_registered = false;
        down_sorryserver_epollfd_registered = false;
        is_epoll_edge_trigger = false;
        epoll_timeout = 50;
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
        // up_thread_message_down_thread_function_map
        up_thread_message_down_thread_function_map.clear();
        // down_thread_module_event_map
        down_thread_module_event_map.clear();
        // down_thread_message_down_thread_function_map
        down_thread_message_up_thread_function_map.clear();
        // virtual_service_message_up_thread_function_map
        virtual_service_message_up_thread_function_map.clear();
        // virtual_service_message_down_thread_function_map
        virtual_service_message_down_thread_function_map.clear();
        // up_thread_message_que
        tcp_thread_message *msg;
        while (1) {
                msg = up_thread_message_que.pop();
                if (msg) {
                        delete msg;
                        msg = NULL;
                } else
                        break;
        }
        // down_thread_message_que
        while (1) {
                msg = down_thread_message_que.pop();
                if (msg) {
                        delete msg;
                        msg = NULL;
                } else
                        break;
        }
        close(up_client_epollfd);
        close(up_realserver_epollfd);
        close(up_sorryserver_epollfd);
        close(down_client_epollfd);
        close(down_realserver_epollfd);
        close(down_sorryserver_epollfd);
}

//! initialize
session_result_message tcp_session::initialize()
{
        session_result_message msg;
        msg.flag = false;
        msg.message = "";
        exit_flag = 0;
        up_thread_id = boost::thread::id();
        down_thread_id = boost::thread::id();
        ssl_handshake_timer_flag = false;
        ssl_handshake_time_out_flag = false;
        upthread_status = UPTHREAD_SLEEP;
        downthread_status = DOWNTHREAD_SLEEP;
        realserver_connect_status = false;
        protocol_module = NULL;
        tcp_thread_message *tmp_msg;
        while (1) {
                tmp_msg = up_thread_message_que.pop();
                if (tmp_msg) {
                        delete tmp_msg;
                        tmp_msg = NULL;
                } else
                        break;
        }
        while (1) {
                tmp_msg = down_thread_message_que.pop();
                if (tmp_msg) {
                        delete tmp_msg;
                        tmp_msg = NULL;
                } else
                        break;
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

        int_val = param.get_int(PARAM_COMP_SESSION, PARAM_EPOLL_TRIGGER, vs_err);
        if ((likely(!vs_err)) && (int_val >= 0)) {
                if (int_val == 0) {
                        is_epoll_edge_trigger = false;
                } else {
                        is_epoll_edge_trigger = false;
                }
        }

        int_val = param.get_int(PARAM_COMP_SESSION, PARAM_EPOLL_TIMEOUT, vs_err);
        if ((likely(!vs_err)) && (int_val > 0)) {
                epoll_timeout = int_val;
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

        // Reset SSL structure to allow another connection.
        if (ssl_flag) {
                if (client_ssl_socket.is_handshake_error()) {
                        // remake socket
                        client_ssl_socket.clear_socket();
                } else {
                        if (ssl_cache_flag) {
                                if (unlikely(ssl_clear_keep_cache(client_ssl_socket.get_socket().impl()->ssl) == false)) {
                                        //Error ssl_clear_keep_cache
                                        std::stringstream buf;
                                        buf << "Thread ID[";
                                        buf << boost::this_thread::get_id();
                                        buf << "] ssl_clear_keep_cache failed";
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 110, buf.str(), __FILE__, __LINE__);
                                        msg.flag = true;
                                        msg.message = "ssl_clear_keep_cache failed";
                                } else {
                                        //----Debug log----------------------------------------------------------------------
                                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                                std::stringstream buf;
                                                buf << "Thread ID[";
                                                buf << boost::this_thread::get_id();
                                                buf << "] ssl_clear_keep_cache ok";
                                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 81,
                                                                    buf.str(),
                                                                    __FILE__, __LINE__);
                                        }
                                        //----Debug log----------------------------------------------------------------------
                                }
                        } else {
                                if (unlikely(SSL_clear(client_ssl_socket.get_socket().impl()->ssl) == 0)) {
                                        //Error SSL_clear
                                        std::stringstream buf;
                                        buf << "Thread ID[";
                                        buf << boost::this_thread::get_id();
                                        buf << "] SSL_clear failed";
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 111, buf.str(), __FILE__, __LINE__);
                                        msg.flag = true;
                                        msg.message = "SSL_clear failed";
                                } else {
                                        //----Debug log----------------------------------------------------------------------
                                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                                std::stringstream buf;
                                                buf << "Thread ID[";
                                                buf << boost::this_thread::get_id();
                                                buf << "] SSL_clear ok";
                                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 82,
                                                                    buf.str(),
                                                                    __FILE__, __LINE__);
                                        }
                                        //----Debug log----------------------------------------------------------------------
                                }
                        }
                }
        }

        up_client_epollfd_registered = false;
        up_realserver_epollfd_registered = false;
        up_sorryserver_epollfd_registered = false;
        down_client_epollfd_registered = false;
        down_realserver_epollfd_registered = false;
        down_sorryserver_epollfd_registered = false;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT ~tcp_session");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return msg;
}

//! ssl clear keep cache
//! @param[in] ssl object
//! @return    true is clear OK.
//! @return    false is not clear
bool tcp_session::ssl_clear_keep_cache(SSL *clear_ssl)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN ssl_clear_keep_cache");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        if (clear_ssl->method == NULL) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] clear ssl method is NULL";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 83,
                                            buf.str(),
                                            __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                return (false);
        }

        if (clear_ssl->new_session) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] clear ssl new_session is not NULL";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 84,
                                            buf.str(),
                                            __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                return (false);
        }

        if (clear_ssl->session != NULL) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] SSL_SESSION_free call";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 88,
                                            buf.str(),
                                            __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                SSL_SESSION_free(clear_ssl->session);
        }

        // init member
        clear_ssl->session = NULL;
        clear_ssl->type = 0;
        clear_ssl->error = 0;
        clear_ssl->hit = 0;
        clear_ssl->shutdown = 0;
        clear_ssl->version = clear_ssl->method->version;
        clear_ssl->client_version = clear_ssl->version;
        clear_ssl->rwstate = SSL_NOTHING;
        clear_ssl->rstate = SSL_ST_READ_HEADER;
        clear_ssl->state = SSL_ST_BEFORE | clear_ssl->server ? SSL_ST_ACCEPT : SSL_ST_CONNECT;

        // init_buf free
        if (clear_ssl->init_buf != NULL) {
                BUF_MEM_free(clear_ssl->init_buf);
                clear_ssl->init_buf = NULL;
        }

        // enc_read_ctx free
        if (clear_ssl->enc_read_ctx != NULL) {
                EVP_CIPHER_CTX_cleanup(clear_ssl->enc_read_ctx);
                OPENSSL_free(clear_ssl->enc_read_ctx);
                clear_ssl->enc_read_ctx = NULL;
        }

        // enc_write_ctx free
        if (clear_ssl->enc_write_ctx != NULL) {
                EVP_CIPHER_CTX_cleanup(clear_ssl->enc_write_ctx);
                OPENSSL_free(clear_ssl->enc_write_ctx);
                clear_ssl->enc_write_ctx = NULL;
        }

        // expand free
        if (clear_ssl->expand != NULL) {
                COMP_CTX_free(clear_ssl->expand);
                clear_ssl->expand = NULL;
        }

        // compress free
        if (clear_ssl->compress != NULL) {
                COMP_CTX_free(clear_ssl->compress);
                clear_ssl->compress = NULL;
        }

        clear_ssl->first_packet = 0;

        if (!clear_ssl->in_handshake && (clear_ssl->method != clear_ssl->ctx->method)) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] In ssl_clear_keep_cache() method changed";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 86,
                                            buf.str(),
                                            __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                clear_ssl->method->ssl_free(clear_ssl);
                clear_ssl->method = clear_ssl->ctx->method;
                if (!clear_ssl->method->ssl_new(clear_ssl)) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "Thread ID[";
                                buf << boost::this_thread::get_id();
                                buf << "] clear ssl method ssl_new error";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 85,
                                                    buf.str(),
                                                    __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        return (false);
                }
        } else {
                clear_ssl->method->ssl_clear(clear_ssl);
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT ssl_clear_keep_cache");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return (true);
}

//! get reference client side socket
//! @return            reference client side socket
boost::asio::ip::tcp::socket &tcp_session::get_client_socket()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN/OUT get_client_socket");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        return client_socket.get_socket();
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
void tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN set_virtual_service_message");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        switch (message) {
        case SESSION_PAUSE_ON: {
                {
                        boost::mutex::scoped_lock up_lock(upthread_status_mutex);
                        upthread_status = UPTHREAD_LOCK;
                }
                {
                        boost::mutex::scoped_lock down_lock(downthread_status_mutex);
                        downthread_status = DOWNTHREAD_LOCK;
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] FUNC OUT set_virtual_service_message message:[SESSION_PAUSE_ON]";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 12, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        return;
        case SESSION_PAUSE_OFF: {
                {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                        if (upthread_status == UPTHREAD_LOCK) {
	                        upthread_status = UPTHREAD_ACTIVE;
                                upthread_status_cond.notify_one();
                        }
                }
                {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                        if (downthread_status == DOWNTHREAD_LOCK) {
                                downthread_status = DOWNTHREAD_ACTIVE;
                                downthread_status_cond.notify_one();
                        }
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] FUNC OUT set_virtual_service_message message:[SESSION_PAUSE_OFF]";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 13, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        return;
        case ACCESS_LOG_ON: {
                rw_scoped_lock scope_lock(access_log_flag_mutex);
                access_log_flag = true;
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] FUNC OUT set_virtual_service_message message:[ACCESS_LOG_ON]";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 49, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        return;
        case ACCESS_LOG_OFF: {
                rw_scoped_lock scope_lock(access_log_flag_mutex);
                access_log_flag = false;
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] FUNC OUT set_virtual_service_message message:[ACCESS_LOG_OFF]";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 68, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        return;
        case SORRY_STATE_ENABLE:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] set_virtual_service_message message:[SORRY_STATE_ENABLE]";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 14, buf.str(), __FILE__, __LINE__);
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
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator up_func;
        up_func = virtual_service_message_up_thread_function_map.find(message);

        up_msg->message = up_func->second;
        while (!up_thread_message_que.push(up_msg)) {}

        tcp_thread_message *down_msg = new tcp_thread_message;
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator down_func;
        down_func = virtual_service_message_down_thread_function_map.find(message);
        down_msg->message = down_func->second;
        while (!down_thread_message_que.push(down_msg)) {}

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT set_virtual_service_message");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up stream thread main function
void tcp_session::up_thread_run()
{
        boost::system::error_code ec;

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 17, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        up_thread_id = boost::this_thread::get_id();
        {
                boost::mutex::scoped_lock lock1(upthread_status_mutex);
                boost::mutex::scoped_lock lock2(realserver_connect_mutex);
                upthread_status = UPTHREAD_ALIVE;
                realserver_connect_status = false;
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread down thread alive wait start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 18, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                while (downthread_status < DOWNTHREAD_ALIVE) {
                        to_time(LOCKTIMEOUT, xt);
                        downthread_status_cond.timed_wait(lock, xt);
		}
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread down thread alive wait end";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 19, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        if (likely(!exit_flag)) {
                bool bres = !ssl_flag ? client_socket.get_socket().lowest_layer().is_open()
                            : client_ssl_socket.get_socket().lowest_layer().is_open();
                if (unlikely(!bres)) {
                        // cannot get client socket
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "cannot get client socket";
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 9, buf.str(), __FILE__, __LINE__);
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                }
        }

        if (!ssl_flag)
                client_socket.accept();
        else
                client_ssl_socket.accept();

        if (likely(!exit_flag)) {
                client_endpoint = (!ssl_flag) ? client_socket.get_socket().lowest_layer().remote_endpoint(ec)
                                  : client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                if (unlikely(ec)) {
                        //cannot get client endpoint
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "cannot get client endpoint: ";
                        buf << ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 10, buf.str(), __FILE__, __LINE__);
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                }
        }
        if (likely(!exit_flag)) {
                bool bres = !ssl_flag ? client_socket.set_non_blocking_mode(ec)
                            : client_ssl_socket.set_non_blocking_mode(ec);
                if (unlikely(!bres)) {
                        // cannot set socket to non-blocking mode
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "cannot set socket to non-blocking mode:";
                        buf << ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 11, buf.str(), __FILE__, __LINE__);
                        __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                }
        }
        if (likely(!exit_flag)) {
                // set client socket options(receive buffer size)
                if (upstream_buffer_size > 0) {
                        boost::asio::socket_base::receive_buffer_size opt(upstream_buffer_size);

                        if (!ssl_flag)
                                client_socket.get_socket().lowest_layer().set_option(opt, ec);
                        else
                                client_ssl_socket.get_socket().lowest_layer().set_option(opt, ec);

                        if (unlikely(ec)) {
                                // cannot set socket option
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "cannot set client socket receive buffer size: ";
                                buf << ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 12, buf.str(), __FILE__, __LINE__);
                                __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                        }
                }
        }
        if (likely(!exit_flag)) {
                // set client socket options(send buffer size)
                if (downstream_buffer_size > 0) {
                        boost::asio::socket_base::send_buffer_size opt(downstream_buffer_size);

                        if (!ssl_flag)
                                client_socket.get_socket().lowest_layer().set_option(opt, ec);
                        else
                                client_ssl_socket.get_socket().lowest_layer().set_option(opt, ec);

                        if (unlikely(ec)) {
                                // cannot set socket option
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "cannot set client socket send buffer size: ";
                                buf << ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 13, buf.str(), __FILE__, __LINE__);
                                __sync_bool_compare_and_swap(&exit_flag, 0, 1);
                        }
                }
        }
        if (likely(!exit_flag)) {
                boost::asio::ip::udp::endpoint dummy_end;
                protocol_module_base::EVENT_TAG module_event;
                std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type;
                up_thread_function_pair func;
                module_event = protocol_module->handle_session_initialize(up_thread_id, down_thread_id, client_endpoint, dummy_end);
                func_type = up_thread_module_event_map.find(module_event);
                func = up_thread_function_array[func_type->second];
                up_thread_next_call_function = func;
        }
        if (likely(!exit_flag)) {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                upthread_status = UPTHREAD_ACTIVE;
                upthread_status_cond.notify_one();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread loop start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 20, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        while (!exit_flag) {
		{
			boost::mutex::scoped_lock lock(upthread_status_mutex);
	                while ( unlikely(upthread_status == UPTHREAD_LOCK) ) {
                                to_time(LOCKTIMEOUT, xt);
	                        upthread_status_cond.timed_wait(lock, xt);
	                }
		}
                tcp_thread_message *msg = up_thread_message_que.pop();
                if (unlikely(msg)) {
                        if (unlikely(UP_FUNC_EXIT == up_thread_next_call_function.first)) {
                                up_thread_next_call_function.second(LOCAL_PROC);
                        } else {
                                up_thread_message_data.set_endpoint(msg->endpoint_info);
                                msg->message(MESSAGE_PROC);
                        }
                        delete  msg;
                        msg = NULL;
                } else {
                        up_thread_next_call_function.second(LOCAL_PROC);
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread loop end";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 21, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        up_thread_all_socket_close();

        {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                upthread_status = UPTHREAD_COMPLETE;
                upthread_status_cond.notify_one();
        }

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down thread complete wait start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 22, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                while (downthread_status != DOWNTHREAD_COMPLETE && downthread_status != DOWNTHREAD_SLEEP) {
                        to_time(LOCKTIMEOUT, xt);
                        downthread_status_cond.timed_wait(lock, xt);
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down thread complete wait end";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 23, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        if (likely(protocol_module != NULL)) {
                protocol_module->handle_session_finalize(up_thread_id, down_thread_id);
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread called handle_session_finalize";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 24, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                upthread_status = UPTHREAD_SLEEP;
        }
        if ( !__sync_bool_compare_and_swap(&exit_flag, 1, 2) ) {
                parent_service.release_session(this);
        }

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 18, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
}

//! down stream thread main function
void tcp_session::down_thread_run()
{
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 27, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        down_thread_id = boost::this_thread::get_id();
        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                downthread_status = DOWNTHREAD_ALIVE;
                downthread_status_cond.notify_one();
        }

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down_thread_run up thread active wait start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 28, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                while (upthread_status < UPTHREAD_ACTIVE) {
                        to_time(LOCKTIMEOUT, xt);
                        upthread_status_cond.timed_wait(upthread_status_mutex, xt);
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down_thread_run up thread active wait end";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 29, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                downthread_status = DOWNTHREAD_ACTIVE;
                downthread_status_cond.notify_one();
        }

        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down thread loop start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 30, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        while (!exit_flag) {
                {
                        boost::mutex::scoped_lock lock(downthread_status_mutex);
                        while ( unlikely(downthread_status == DOWNTHREAD_LOCK) ) {
                                to_time(LOCKTIMEOUT, xt);
                                downthread_status_cond.timed_wait(lock, xt);
                        }
                }
                while (unlikely(!down_thread_connect_socket_list.empty())) {
                        socket_element push_rs_socket = down_thread_connect_socket_list.get_socket();
                        down_thread_receive_realserver_socket_list.push_back(push_rs_socket);
                        down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
                }

                tcp_thread_message *msg = down_thread_message_que.pop();
                if (unlikely(msg)) {
                        if (unlikely(DOWN_FUNC_EXIT == down_thread_next_call_function.first)) {
                                down_thread_next_call_function.second(LOCAL_PROC);
                        } else {
                                down_thread_message_data.set_endpoint(msg->endpoint_info);
                                msg->message(MESSAGE_PROC);
                        }
                        delete  msg;
                        msg = NULL;
                } else {
                        down_thread_next_call_function.second(LOCAL_PROC);
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "down thread loop end";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 31, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        down_thread_all_socket_close();
        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                downthread_status = DOWNTHREAD_COMPLETE;
                downthread_status_cond.notify_one();
        }

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread complete wait start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, /*XXX*/999, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
        {
                boost::mutex::scoped_lock lock(upthread_status_mutex);
                while (upthread_status != UPTHREAD_COMPLETE && upthread_status != UPTHREAD_SLEEP) {
                        to_time(LOCKTIMEOUT, xt);
                        upthread_status_cond.timed_wait(lock, xt);
                }
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                std::stringstream buf;
                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                buf << "up thread complete wait start";
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, /*XXX*/999, buf.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------

        {
                boost::mutex::scoped_lock lock(downthread_status_mutex);
                downthread_status = DOWNTHREAD_SLEEP;
                downthread_status_cond.notify_one();
        }
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 32, formatter.str(), __FILE__, __LINE__);
        }
        if ( !__sync_bool_compare_and_swap(&exit_flag, 1, 2) ) {
                parent_service.release_session(this);
        }

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_run");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log----------------------------------------------------------------------
}

//! endpoint data to string information
//! @param[in]        endpoint is target endpoint object
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
                ret << "[" << target_endpoint.address().to_string() << "]:" << target_endpoint.port();
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

//! up thread accept client side
//! @param[in]        process_type is process type
void tcp_session::up_thread_client_accept(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_client_accept");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        UP_THREAD_FUNC_TYPE_TAG func_tag;

        if (ssl_flag) {
                //SSL Mode
                rd_scoped_lock scoped_lock(ssl_handshake_time_out_flag_mutex);
                if (ssl_handshake_time_out_flag) {
                        // SSL handshake time out or timer error
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "handshake timer timeout " << ssl_handshake_time_out << "sec: ";
                        buf << "handshaking not end";
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 112, buf.str(), __FILE__, __LINE__);
                } else {
                        if (ssl_handshake_timer_flag == false) {
                                // set handshake timer
                                //register handshake timer event handler
                                ssl_handshake_timer.reset(new boost::asio::deadline_timer(io));
                                ssl_handshake_timer->expires_from_now(boost::posix_time::seconds(ssl_handshake_time_out));
                                ssl_handshake_timer->async_wait(boost::bind(&tcp_session::handle_ssl_handshake_timer,
                                                                this,
                                                                boost::asio::placeholders::error));
                                ssl_handshake_timer_flag = true;
                                //----Debug log----------------------------------------------------------------------
                                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                        std::stringstream buf;
                                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                        buf << "ssl session handshaking start: ";
                                        buf << "set handshake timer " << ssl_handshake_time_out << "sec";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 70, buf.str(), __FILE__, __LINE__);
                                }
                                //----Debug log----------------------------------------------------------------------
                        }
                        // try ssl handshake
                        boost::system::error_code ec;
                        client_ssl_socket.handshake(ec);

                        if (!ec) {
                                // handshake ok
                                ssl_handshake_timer->cancel();
                                func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;
                        } else {
                                // handshake fail
                                if (ec == boost::asio::error::try_again) {
                                        func_tag = UP_FUNC_CLIENT_ACCEPT;
                                        boost::this_thread::yield();
                                } else {
                                        //ERROR
                                        ssl_handshake_timer->cancel();
                                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 113, "ssl socket handshaking failed", __FILE__, __LINE__);
                                        //----Debug log----------------------------------------------------------------------
                                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                                std::stringstream buf;
                                                buf << "Thread ID[";
                                                buf << boost::this_thread::get_id();
                                                buf << "] tcp_ssl_socket::handshake [";
                                                buf << ec.message();
                                                buf << "]";
                                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 71, buf.str(), __FILE__, __LINE__);
                                        }
                                        //----Debug log----------------------------------------------------------------------
                                }
                        }
                }
        } else {
                func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_accept: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
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

        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_accept(up_thread_id);
        up_thread_data_client_side.set_endpoint(client_endpoint);
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);

        up_thread_function_pair func = up_thread_function_array[func_type->second];
        up_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_accept_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! handshake timer handler
//! @param[in]        error is timer operation result error code
void tcp_session::handle_ssl_handshake_timer(const boost::system::error_code &error)
{
        //----Debug log--------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN handle_ssl_handshake_timer");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 72, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log--------------------------------------------------------

        if (!error) {
                rw_scoped_lock scoped_lock(ssl_handshake_time_out_flag_mutex);
                ssl_handshake_time_out_flag = true;
        } else {
                if (error.value() == ECANCELED) {
                        //----Debug log----------------------------------------
                        if (unlikely(LOG_LV_DEBUG ==
                                     Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "handshake timer operation cancel: ";
                                buf << "handshaking normal end";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION,
                                                    87, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------
                } else {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "handshake timer operation failed: ";
                        buf << error.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION,
                                            116, buf.str(), __FILE__, __LINE__);
                }
        }

        //----Debug log--------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT handle_ssl_handshake_timer");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 73, formatter.str(), __FILE__, __LINE__);
        }
        //----Debug log--------------------------------------------------------
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

        struct epoll_event event;
        event.data.fd = !ssl_flag ? client_socket.get_socket().native()
                        : client_ssl_socket.get_socket().lowest_layer().native();

	if (event.data.fd == -1) {
	        up_thread_next_call_function  = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT];
		return;
	}

        // epoll wait codes
        if (is_epoll_edge_trigger) {
                event.events = EPOLLIN | EPOLLHUP | EPOLLET;
        } else {
                event.events = EPOLLIN | EPOLLHUP;
        }
        if (!up_client_epollfd_registered) {
                if (epoll_ctl(up_client_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "up_thread_client_receive: epoll_ctl EPOLL_CTL_ADD error: ";
                        buf << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                up_client_epollfd_registered = true;
        }

        int ret_fds = epoll_wait(up_client_epollfd, up_client_events, EVENT_NUM, epoll_timeout);
        if (ret_fds == 0) {
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format formatter("up_thread_client_receive: epoll_wait timeout %d msec");
                        formatter % epoll_timeout;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                up_thread_next_call_function = up_thread_function_array[UP_FUNC_CLIENT_RECEIVE];
                return;
        } else if (ret_fds < 0) {
                boost::format formatter("up_thread_client_receive: epoll_wait error: %s");
                formatter % strerror(errno);
                Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                up_thread_next_call_function = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT];
                return;
        }

        for (int i = 0; i < ret_fds; ++i) {
                if (up_client_events[i].data.fd == event.data.fd) {
                        if (up_client_events[i].events & EPOLLIN) {
                                break;
                        }
                        if (up_client_events[i].events & EPOLLHUP) {
                                up_thread_next_call_function = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT];
                                //----Debug log----------------------------------------------------------------------
                                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                        boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_receive EPOLLHUP: NEXT_FUNC[%s]");
                                        formatter % boost::this_thread::get_id() % func_tag_to_string(UP_FUNC_CLIENT_DISCONNECT);
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                                }
                                //----Debug log----------------------------------------------------------------------
                                return;
                        }
                }
        }

        recv_size = !ssl_flag ? client_socket.read_some(
                            boost::asio::buffer(data_buff, MAX_BUFFER_SIZE),
                            ec)
                    : client_ssl_socket.read_some(
                            boost::asio::buffer(data_buff, MAX_BUFFER_SIZE),
                            ec);

        if (!ec) {
                if (recv_size > 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                endpoint client_endpoint; // XXX redefined???
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
                        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
                        if (unlikely(func_type == up_thread_module_event_map.end())) {
                                //Error unknown protocol_module_base::EVENT_TAG return
                                boost::format formatter("Thread ID[%d] protocol_module returned illegal EVENT_TAG: %d");
                                formatter % boost::this_thread::get_id() % module_event;
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 20, formatter.str(), __FILE__, __LINE__);
                                up_thread_exit(process_type);
                                return;
                        }
                        func_tag = func_type->second;
                } else {
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                }
        } else {
                if (ec == boost::asio::error::eof) {
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                } else if (ec == boost::asio::error::try_again) {
                        func_tag = UP_FUNC_CLIENT_RECEIVE;
                } else {
                        boost::format formatter("Thread ID[%d] client read error: %s");
                        formatter % boost::this_thread::get_id() % ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
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
        tcp_thread_message *down_msg = new tcp_thread_message;
        up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT];

        up_msg->message = up_func.second;
        std::map<DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func>::iterator down_func
        = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
        down_msg->message = down_func->second;
        while (!up_thread_message_que.push(up_msg)) {}
        while (!down_thread_message_que.push(down_msg)) {}

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
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN func up_thread_client_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        boost::system::error_code ec;
        bool bres = !ssl_flag ? client_socket.close(ec)
                    : client_ssl_socket.close(ec);

        if (bres) {
                tcp_thread_message *up_msg = new tcp_thread_message;
                tcp_thread_message *down_msg = new tcp_thread_message;
                up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT];
                up_msg->message = up_func.second;
                down_thread_function_pair down_func = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT];
                down_msg->message = down_func.second;

                while (!up_thread_message_que.push(up_msg)) {}
                while (!down_thread_message_que.push(down_msg)) {}
        }
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC_OUT up_thread_client_disconnect up_func[%s] down_func[%s]");
                formatter % boost::this_thread::get_id()
                % func_tag_to_string(UP_FUNC_CLIENT_DISCONNECT_EVENT)
                % func_tag_to_string(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
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
        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_function_pair func = up_thread_function_array[func_type->second];
        up_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_client_disconnect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        boost::system::error_code ec;
        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint, tcp_socket_ptr>::iterator send_socket =  up_thread_send_realserver_socket_map.find(server_endpoint);
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        std::size_t data_size = up_thread_data_dest_side.get_size();
        std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
        std::size_t send_size;
        UP_THREAD_FUNC_TYPE_TAG func_tag;

        struct epoll_event event;
        event.data.fd = send_socket->second->get_socket().native();
        if (is_epoll_edge_trigger) {
                event.events = EPOLLOUT | EPOLLHUP | EPOLLET;
        } else {
                event.events = EPOLLOUT | EPOLLHUP;
        }
        if (!up_realserver_epollfd_registered) {
                if (epoll_ctl(up_realserver_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "up_thread_realserver_send: epoll_ctl EPOLL_CTL_ADD error: ";
                        buf << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                up_realserver_epollfd_registered = true;
        }

        int ret_fds = epoll_wait(up_realserver_epollfd, up_realserver_events, EVENT_NUM, epoll_timeout);
        if (ret_fds <= 0) {
                if (ret_fds == 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "up_thread_realserver_send: epoll_wait timeout " << epoll_timeout << " msec.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                } else {
                        std::stringstream buf;
                        buf << "up_thread_realserver_send: epoll_wait error: " << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                }
                //XXX no need to retry???
                up_thread_next_call_function = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT];
                return;
        }

        for (int i = 0; i < ret_fds; ++i) {
                if (up_realserver_events[i].data.fd == event.data.fd) {
                        if (up_realserver_events[i].events & EPOLLOUT) {
                                break;
                        } else if (up_realserver_events[i].events & EPOLLHUP) {
                                std::stringstream buf;
                                buf << "up_thread_realserver_send: epoll hung up event";
                                Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                                up_thread_next_call_function = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT];
                                return;
                        }
                }
        }

        send_size = send_socket->second->write_some(
                            boost::asio::buffer(
                                    data_buff.data() + send_data_size,
                                    data_size - send_data_size),
                            ec);
        if (!ec) {
                send_data_size += send_size;
                up_thread_data_dest_side.set_send_size(send_data_size);
                parent_service.update_up_send_size(send_size);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format formatter("Thread ID[%d] up_thread_realserver_send send data size[%d] for [%d]");
                        formatter % boost::this_thread::get_id() % send_size % server_endpoint;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 37, formatter.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                if (data_size > send_data_size) {
                        func_tag = UP_FUNC_REALSERVER_SEND;
                } else {
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_send(up_thread_id);
                        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
                        func_tag = func_type->second;
                }
        } else {
                if (ec == boost::asio::error::try_again) {
                        func_tag = UP_FUNC_REALSERVER_SEND;
                } else {
                        func_tag = UP_FUNC_REALSERVER_DISCONNECT;
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

        endpoint server_endpoint;

        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_select(up_thread_id, server_endpoint);
        up_thread_data_dest_side.set_endpoint(server_endpoint);

        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_function_pair func = up_thread_function_array[func_type->second];
        up_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_get_destination_event");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
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

        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint, tcp_socket_ptr>::iterator get_socket = up_thread_send_realserver_socket_map.find(server_endpoint);
        std::map<endpoint, tcp_socket_ptr>::iterator map_end = up_thread_send_realserver_socket_map.end();
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if (get_socket != map_end) {
                func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
        } else {
                tcp_socket_ptr new_socket(new tcp_socket(io, socket_opt_info));
                boost::system::error_code ec;
#ifdef IP_TRANSPARENT
                realserver_element::REALSERVER_FWDMODE_TAG fwdmode = realserver_element::FWD_NONE;
                std::vector<realserver_element> real_vec = parent_service.get_element().realserver_vector;
                for (std::vector<realserver_element>::iterator rs_itr = real_vec.begin();
                        rs_itr != real_vec.end(); ++rs_itr) {
                        if (rs_itr->tcp_endpoint == server_endpoint) {
                                fwdmode = rs_itr->fwdmode;
                                break;
                        }
                }
                if (fwdmode == realserver_element::FWD_TPROXY && (
                        ( server_endpoint.address().is_v4() && client_endpoint.address().is_v4() ) ||
                        ( server_endpoint.address().is_v6() && client_endpoint.address().is_v6() ) ) ) {
                        int ip_socket_level;
                        if (client_endpoint.address().is_v4()) {
                                ip_socket_level = SOL_IP;
                                new_socket->get_socket().open(boost::asio::ip::tcp::v4(), ec);
                        } else {
                                ip_socket_level = SOL_IPV6;
                                new_socket->get_socket().open(boost::asio::ip::tcp::v6(), ec);
                        }
                        if (unlikely(ec)) {
                                boost::format formatter("Thread ID[%d] realserver socket open error: %s");
                                formatter % boost::this_thread::get_id() % ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                goto out_tproxy;
                        }
                        // set IP_TRANSPARENT
                        int on = 1;
                        int err = ::setsockopt(new_socket->get_socket().native(),
                                ip_socket_level, IP_TRANSPARENT, &on, sizeof(on));
                        if (unlikely(err)) {
                                ec = boost::system::error_code(errno, boost::asio::error::get_system_category());
                                boost::format formatter("Thread ID[%d] realserver socket option(IP_TRANSPARENT) set failed: %s");
                                formatter % boost::this_thread::get_id() % ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                goto out_tproxy;
                        }
                        // bind client address
                        new_socket->get_socket().bind(client_endpoint, ec);
                        if (unlikely(ec)) {
                                if (ec == boost::asio::error::address_in_use) {
                                        boost::format formatter("Thread ID[%d] bind client address error(%s). retry by other port.");
                                        formatter % boost::this_thread::get_id() % ec.message();
                                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);

                                        unsigned short client_port = client_endpoint.port();
                                        client_endpoint.port(0);
                                        new_socket->get_socket().bind(client_endpoint, ec);
                                        client_endpoint.port(client_port);
                                }
                                if (unlikely(ec)) {
                                        boost::format formatter("Thread ID[%d] bind client address to realserver socket failed: %s");
                                        formatter % boost::this_thread::get_id() % ec.message();
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                }
                        }
                }
out_tproxy:
#endif
                bool bres = new_socket->connect(server_endpoint, ec);
                if (likely(bres)) {
                        {
                                rd_scoped_lock scoped_lock(access_log_flag_mutex);
                                if (access_log_flag && access_logger != NULL) {
                                        endpoint server_side_endpoint =
                                                new_socket->get_socket().local_endpoint(ec);
                                        if (!ec) {
                                                std::string reserv_msg("");
                                                access_logger->putLog(
                                                        endpoint_to_string(virtualservice_endpoint),
                                                        endpoint_to_string(client_endpoint),
                                                        endpoint_to_string(server_side_endpoint),
                                                        endpoint_to_string(server_endpoint),
                                                        reserv_msg);
                                        }
                                }
                        }
                        parent_service.connection_active(server_endpoint);
                        if (unlikely(!new_socket->set_non_blocking_mode(ec))) {
                                // socket set non-blocking mode error
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "set non blocking socket error: " << ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 34, buf.str(), __FILE__, __LINE__);
                                up_thread_exit(process_type);
                                return;
                        }

                        //set realserver_socket options(receive buffer size)
                        if (downstream_buffer_size > 0) {
                                boost::asio::socket_base::receive_buffer_size opt1(downstream_buffer_size);
                                new_socket->get_socket().set_option(opt1, ec);
                                if (unlikely(ec)) {
                                        // socket set non-blocking mode error
                                        std::stringstream buf;
                                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                        buf << "realserver socket receive buffer size error: " << ec.message();
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 35, buf.str(), __FILE__, __LINE__);
                                        up_thread_exit(process_type);
                                        return;
                                }
                        }
                        //set realserver_socket options(send buffer size)
                        if (upstream_buffer_size > 0) {
                                boost::asio::socket_base::send_buffer_size opt2(upstream_buffer_size);
                                new_socket->get_socket().set_option(opt2, ec);
                                if (unlikely(ec)) {
                                        // socket set non-blocking mode error
                                        std::stringstream buf;
                                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                        buf << "realserver socket send buffer size error: " << ec.message();
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 36, buf.str(), __FILE__, __LINE__);
                                        up_thread_exit(process_type);
                                        return;
                                }
                        }

                        socket_element push_element;
                        push_element.first = server_endpoint;
                        push_element.second = new_socket;
                        up_thread_send_realserver_socket_map.insert(push_element);
                        down_thread_connect_socket_list.push_back(push_element);
                        func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
                } else {
                        func_tag = UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT;
                        //connect socket error
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "connect socket error: " << ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 37, buf.str(), __FILE__, __LINE__);
                }
        }
        up_thread_function_pair func = up_thread_function_array[func_tag];
        boost::mutex::scoped_lock lock(realserver_connect_mutex);
        realserver_connect_status = true;
        realserver_connect_cond.notify_one();
        up_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
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

        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        up_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connect(up_thread_id, data_buff, data_size);
        up_thread_data_dest_side.set_endpoint(server_endpoint);
        up_thread_data_dest_side.set_size(data_size);
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connection_fail(up_thread_id, server_endpoint);
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_connection_fail_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint, tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.find(server_endpoint);
        boost::system::error_code ec;
        bool bres = close_socket->second->close(ec);
        if (bres) {
                parent_service.connection_inactive(server_endpoint);
                tcp_thread_message *up_msg = new tcp_thread_message;
                tcp_thread_message *down_msg = new tcp_thread_message;
                up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT];
                up_msg->endpoint_info = server_endpoint;
                up_msg->message = up_func.second;
                std::map<DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func>::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
                down_msg->endpoint_info = server_endpoint;
                down_msg->message = down_func->second;
                while (!up_thread_message_que.push(up_msg)) {}
                while (!down_thread_message_que.push(down_msg)) {}
        }

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
        endpoint server_endpoint = up_thread_message_data.get_endpoint();
        {
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(up_thread_id, server_endpoint);
        }
        up_thread_send_realserver_socket_map.erase(server_endpoint);

        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_realserver_disconnect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}

//! up thread close all realserver socket and raise module event of handle_realserver_disconnect
//! @param[in]        process_type is process type
void tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_all_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        std::map<endpoint, tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
        std::map<endpoint, tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();

        protocol_module_base::EVENT_TAG module_event;
        bool realserver_found = false;
        endpoint server_endpoint;
        while (close_socket != list_end) {
                realserver_found = true;
                boost::system::error_code ec;
                bool bres = close_socket->second->close(ec);
                server_endpoint = close_socket->first;
                if (bres) {
                        parent_service.connection_inactive(server_endpoint);
                }
                {
                        rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                        module_event = protocol_module->handle_realserver_disconnect(up_thread_id, server_endpoint);
                }
                close_socket++;
        }
        if (!realserver_found) {
                // realserver not found
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "close realserver not fond";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 38, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(up_thread_id, endpoint());
        }
        up_thread_send_realserver_socket_map.clear();
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        //allrealserver_disconnect.
        boost::mutex::scoped_lock lock(realserver_connect_mutex);
        realserver_connect_status = true;
        realserver_connect_cond.notify_one();

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_all_realserver_disconnect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        struct epoll_event event;
        event.data.fd = sorryserver_socket.second->get_socket().native();
        if (is_epoll_edge_trigger) {
                event.events = EPOLLET | EPOLLOUT | EPOLLHUP;
        } else {
                event.events = EPOLLOUT | EPOLLHUP;
        }
        if (!up_sorryserver_epollfd_registered) {
                if (epoll_ctl(up_sorryserver_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "up_thread_sorryserver_send: epoll_ctl EPOLL_CTL_ADD error: ";
                        buf << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                up_sorryserver_epollfd_registered = true;
        }

        while (true) {
                {
                        rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                        if (unlikely(exit_flag)) {
                                func_tag = UP_FUNC_EXIT;
                                break;
                        }
                }

                int ret_fds = epoll_wait(up_sorryserver_epollfd, up_sorryserver_events, EVENT_NUM, epoll_timeout);
                if (ret_fds <= 0) {
                        if (ret_fds == 0) {
                                //----Debug log----------------------------------------------------------------------
                                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                        std::stringstream buf;
                                        buf << "up_thread_sorryserver_send: epoll_wait timeout " << epoll_timeout << " msec.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                                }
                                //----Debug log----------------------------------------------------------------------
                        } else {
                                std::stringstream buf;
                                buf << "up_thread_sorryserver_send: epoll_wait error: ";
                                buf << strerror(errno);
                                Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        }
                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT];
                        return;
                }

                for (int i = 0; i < ret_fds; ++i) {
                        if (up_sorryserver_events[i].data.fd == event.data.fd) {
                                if (up_sorryserver_events[i].events & EPOLLOUT) {
                                        break;
                                } else if (up_sorryserver_events[i].events & EPOLLHUP) {
                                        up_thread_next_call_function = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT];
                                        return;
                                }
                        }
                }

                send_size = sorryserver_socket.second->write_some(boost::asio::buffer(data_buff.data() + send_data_size, data_size - send_data_size), ec);

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
                        break;
                } else {
                        if (ec == boost::asio::error::try_again) {
                                //func_tag = UP_FUNC_SORRYSERVER_SEND;
                                //boost::this_thread::yield();
                        } else {
                                func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
                                break;
                        }
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

        endpoint server_endpoint;
        virtualservice_element &vs_element = parent_service.get_element();
        server_endpoint = vs_element.sorry_endpoint;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_select(up_thread_id, server_endpoint);
        up_thread_data_dest_side.set_endpoint(server_endpoint);

        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_get_destination_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        boost::system::error_code ec;
        if (sorryserver_socket.second->get_socket().lowest_layer().is_open()) {
                func_tag = UP_FUNC_SORRYSERVER_CONNECT_EVENT;
        } else {
#ifdef IP_TRANSPARENT
                if (parent_service.get_element().sorry_fwdmode == virtualservice_element::FWD_TPROXY && (
                        ( sorry_endpoint.address().is_v4() && client_endpoint.address().is_v4() ) ||
                        ( sorry_endpoint.address().is_v6() && client_endpoint.address().is_v6() ) ) ) {
                        int ip_socket_level;
                        if (client_endpoint.address().is_v4()) {
                                ip_socket_level = SOL_IP;
                                sorryserver_socket.second->get_socket().open(boost::asio::ip::tcp::v4(), ec);
                        } else {
                                ip_socket_level = SOL_IPV6;
                                sorryserver_socket.second->get_socket().open(boost::asio::ip::tcp::v6(), ec);
                        }
                        if (unlikely(ec)) {
                                boost::format formatter("Thread ID[%d] sorryserver socket open error: %s");
                                formatter % boost::this_thread::get_id() % ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                goto out_tproxy;
                        }
                        // set IP_TRANSPARENT
                        int on = 1;
                        int err = ::setsockopt(sorryserver_socket.second->get_socket().native(),
                                ip_socket_level, IP_TRANSPARENT, &on, sizeof(on));
                        if (unlikely(err)) {
                                ec = boost::system::error_code(errno, boost::asio::error::get_system_category());
                                boost::format formatter("Thread ID[%d] sorryserver socket option(IP_TRANSPARENT) set failed: %s");
                                formatter % boost::this_thread::get_id() % ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                goto out_tproxy;
                        }
                        // bind client address
                        sorryserver_socket.second->get_socket().bind(client_endpoint, ec);
                        if (unlikely(ec)) {
                                if (ec == boost::asio::error::address_in_use) {
                                        boost::format formatter("Thread ID[%d] bind client address error(%s). retry by other port.");
                                        formatter % boost::this_thread::get_id() % ec.message();
                                        Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);

                                        unsigned short client_port = client_endpoint.port();
                                        client_endpoint.port(0);
                                        sorryserver_socket.second->get_socket().bind(client_endpoint, ec);
                                        client_endpoint.port(client_port);
                                }
                                if (unlikely(ec)) {
                                        boost::format formatter("Thread ID[%d] bind client address to sorryserver socket failed: %s");
                                        formatter % boost::this_thread::get_id() % ec.message();
                                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, /*XXX*/999, formatter.str(), __FILE__, __LINE__);
                                }
                        }
                }
out_tproxy:
#endif
                bool bres = sorryserver_socket.second->connect(sorry_endpoint, ec);
                if (likely(bres)) {
                        if (unlikely(!sorryserver_socket.second->set_non_blocking_mode(ec))) {
                                // socket set non-blocking mode error
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "set non blocking socket error: " << ec.message();
                                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 53, buf.str(), __FILE__, __LINE__);
                                up_thread_exit(process_type);
                        }
                        sorryserver_socket.first = sorry_endpoint;
                        func_tag = UP_FUNC_SORRYSERVER_CONNECT_EVENT;
                } else {
                        func_tag = UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT;
                        //connect socket error
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "connect socket error: " << ec.message();
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 54, buf.str(), __FILE__, __LINE__);
                }
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_tag);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
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

        endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        up_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connect(up_thread_id, data_buff, data_size);
        up_thread_data_dest_side.set_endpoint(sorry_endpoint);
        up_thread_data_dest_side.set_size(data_size);
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        // protocol module sorry mode change
        tcp_thread_message *down_msg = new tcp_thread_message;
        std::map<DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func>::iterator
        down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRY_ENABLE_EVENT);
        down_msg->message = down_func->second;
        while (!down_thread_message_que.push(down_msg)) {}

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connection_fail(up_thread_id, server_endpoint);
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_connection_fail_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
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

        boost::system::error_code ec;
        bool bres = sorryserver_socket.second->close(ec);
        if (bres) {
                tcp_thread_message *up_msg = new tcp_thread_message;
                tcp_thread_message *down_msg = new tcp_thread_message;
                up_thread_function_pair up_func = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT];
                up_msg->message = up_func.second;
                up_msg->endpoint_info = sorryserver_socket.first;
                std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT);
                down_msg->message = down_func->second;
                down_msg->endpoint_info = sorryserver_socket.first;
                while (!up_thread_message_que.push(up_msg)) {}
                while (!down_thread_message_que.push(down_msg)) {}
        }

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

        protocol_module_base::EVENT_TAG module_event;

        boost::system::error_code ec;
        endpoint sorry_endpoint = sorryserver_socket.first;
        bool bres = sorryserver_socket.second->close(ec);
        if (!bres) {
                sorryserver_socket.first = endpoint();
        }
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id, sorry_endpoint);
        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_mod_disconnect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        endpoint sorry_endpoint = up_thread_message_data.get_endpoint();
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id, sorry_endpoint);
        }
        std::map<protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG>::iterator func_type = up_thread_module_event_map.find(module_event);
        up_thread_next_call_function = up_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_sorryserver_disconnect_event NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        boost::mutex::scoped_lock down_thread_cond_lock(upthread_status_mutex);
        boost::mutex::scoped_lock realserver_status_lock(realserver_connect_mutex);
        upthread_status_cond.notify_one();
        realserver_connect_cond.notify_one();
        realserver_connect_status = true;
        __sync_bool_compare_and_swap(&exit_flag, 0, 1);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT func up_thread_client_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! up thread close all socket
void tcp_session::up_thread_all_socket_close(void)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN up_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        std::map<endpoint, tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
        std::map<endpoint, tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();
        boost::system::error_code ec;
        while (close_socket != list_end) {
                bool bres = close_socket->second->close(ec);
                if (bres)
                        parent_service.connection_inactive(close_socket->first);
                close_socket++;
        }
        up_thread_send_realserver_socket_map.clear();
        down_thread_connect_socket_list.clear();

        if (!ssl_flag)
                client_socket.close(ec);
        else
                client_ssl_socket.close(ec);

        sorryserver_socket.second->close(ec);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT up_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
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

        if (down_thread_receive_realserver_socket_list.empty()) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_receive");
                        formatter % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                }
                boost::mutex::scoped_lock lock(realserver_connect_mutex);
                if (!realserver_connect_status) {
                        to_time(LOCKTIMEOUT, xt);
                        realserver_connect_cond.timed_wait(lock, xt);
                }
                down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];
                return;
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
        down_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code ec;
        size_t recv_size;
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;

        struct epoll_event event;
        event.data.fd = down_thread_current_receive_realserver_socket->second->get_socket().native();
        if (is_epoll_edge_trigger) {
                event.events = EPOLLIN | EPOLLHUP | EPOLLET;
        } else {
                event.events = EPOLLIN | EPOLLHUP;
        }
        if (!down_realserver_epollfd_registered) {
                if (epoll_ctl(down_realserver_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "down_thread_realserver_receive: epoll_ctl EPOLL_CTL_ADD error: " << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                down_realserver_epollfd_registered = true;
        }

        int ret_fds = epoll_wait(down_realserver_epollfd, down_realserver_events, EVENT_NUM, epoll_timeout);
        if (ret_fds <= 0) {
                if (ret_fds == 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                boost::format formatter("down_thread_realserver_receive: epoll_wait timeout %d msec.");
                                formatter % epoll_timeout;
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];
                        return;
                } else {
                        boost::format formatter("down_thread_realserver_receive: epoll_wait error: %d");
                        formatter % strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT];
                        return;
                }
        }

        for (int i = 0; i < ret_fds; ++i) {
                if (down_realserver_events[i].data.fd == event.data.fd) {
                        if (down_realserver_events[i].events & EPOLLIN) {
                                break;
                        } else if (down_realserver_events[i].events & EPOLLHUP) {
                                down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT];
                                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                        boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_receive: EPOLL_HUP");
                                        formatter % boost::this_thread::get_id();
                                }
                                return;
                        }
                }
        }

        endpoint server_endpoint = down_thread_current_receive_realserver_socket->first;
        down_thread_data_dest_side.set_endpoint(server_endpoint);

        recv_size = down_thread_current_receive_realserver_socket->second->read_some(boost::asio::buffer(data_buff, MAX_BUFFER_SIZE), ec);
        if (!ec) {
                if (recv_size > 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "down_thread_realserver_receive receive data size[" << recv_size << "] from [" << server_endpoint << "]";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 43, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        down_thread_data_dest_side.set_size(recv_size);
                        parent_service.update_down_recv_size(recv_size);

                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_recv(down_thread_id, server_endpoint, data_buff, recv_size);

                        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
                        func_tag = func_type->second;
                } else {
                        func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                }
        } else {
                if (ec == boost::asio::error::try_again) {
                        func_tag = DOWN_FUNC_REALSERVER_RECEIVE;
                } else {
                        func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
                }
        }

        down_thread_function_pair func = down_thread_function_array[func_tag];
        down_thread_current_receive_realserver_socket++;
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        if (down_thread_current_receive_realserver_socket == list_end) {
                down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
        }
        down_thread_next_call_function = func;

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

        endpoint server_endpoint = down_thread_data_dest_side.get_endpoint();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        while (list_end != close_socket) {
                endpoint close_endpoint = close_socket->first;
                if (close_endpoint == server_endpoint) {
                        boost::system::error_code ec;
                        bool bres = close_socket->second->close(ec);
                        if (bres) {
                                parent_service.connection_inactive(server_endpoint);
                                tcp_thread_message *up_msg = new tcp_thread_message;
                                tcp_thread_message *down_msg = new tcp_thread_message;
                                down_thread_function_pair down_func = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT];
                                down_msg->endpoint_info = server_endpoint;
                                down_msg->message = down_func.second;
                                std::map<UP_THREAD_FUNC_TYPE_TAG, tcp_session_func>::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT_EVENT);
                                up_msg->endpoint_info = server_endpoint;
                                up_msg->message = up_func->second;
                                while (!up_thread_message_que.push(up_msg)) {}
                                while (!down_thread_message_que.push(down_msg)) {}
                        }
                        break;
                }
                close_socket++;
        }

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
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
        endpoint server_endpoint = down_thread_message_data.get_endpoint();
        {
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(down_thread_id, server_endpoint);
        }

        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        std::list<socket_element>::iterator erase_socket = down_thread_receive_realserver_socket_list.begin();
        while (likely(erase_socket != list_end)) {
                if (erase_socket->first == server_endpoint) {
                        down_thread_receive_realserver_socket_list.erase(erase_socket);
                        down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
                        break;
                }
                erase_socket++;
        }

        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_next_call_function = down_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_realserver_disconnect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
}
//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
//! @param[in]        process_type is process type
void tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type)
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC IN down_thread_all_realserver_disconnect");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }

        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();

        protocol_module_base::EVENT_TAG module_event;
        endpoint server_endpoint;
        bool realserver_fond = false;
        while (close_socket != list_end) {
                realserver_fond = true;
                boost::system::error_code ec;
                bool bres = close_socket->second->close(ec);
                server_endpoint = close_socket->first;
                if (bres) {
                        parent_service.connection_inactive(server_endpoint);
                }
                {
                        rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                        module_event = protocol_module->handle_realserver_disconnect(down_thread_id, server_endpoint);
                }

                close_socket++;
        }
        if (!realserver_fond) {
                //not fond realserver
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                        buf << "close realserver not fond";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 44, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(down_thread_id, endpoint());
        }
        down_thread_receive_realserver_socket_list.clear();
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_next_call_function = down_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_all_realserver_disconnect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_type->second;
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
        down_thread_data_client_side.set_size(data_size);
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_next_call_function = down_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_connection_chk_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        }
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_next_call_function = down_thread_function_array[func_type->second];

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_client_respond_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        boost::system::error_code ec;
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t data_size = down_thread_data_client_side.get_size();
        std::size_t send_data_size = down_thread_data_client_side.get_send_size();
        std::size_t send_size;
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;

        struct epoll_event event;
        event.data.fd = !ssl_flag ? client_socket.get_socket().native()
                        : client_ssl_socket.get_socket().lowest_layer().native();

        if (is_epoll_edge_trigger) {
                event.events = EPOLLOUT | EPOLLHUP | EPOLLET;
        } else {
                event.events = EPOLLOUT | EPOLLHUP;
        }
        if (!down_client_epollfd_registered) {
                if (epoll_ctl(down_client_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "down_thread_client_send: epoll_ctl EPOLL_CTL_ADD error: ";
                        buf << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                down_client_epollfd_registered = true;
        }

        while (true) {
                {
                        rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                        if (unlikely(exit_flag)) {
                                func_tag = DOWN_FUNC_EXIT;
                                break;
                        }
                }

                int ret_fds = epoll_wait(down_client_epollfd, down_client_events, EVENT_NUM, epoll_timeout);
                if (ret_fds <= 0) {
                        if (ret_fds == 0) {
                                std::stringstream buf;
                                buf << "down_thread_client_send: epoll_wait timeout ";
                                buf << epoll_timeout;
                                buf << "mS";
                                Logger::putLogInfo(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        } else {
                                std::stringstream buf;
                                buf << "down_thread_client_send: epoll_wait error: ";
                                buf << strerror(errno);
                                Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        }
                        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT];
                        return;
                }

                for (int i = 0; i < ret_fds; ++i) {
                        if (down_client_events[i].data.fd == event.data.fd) {
                                if (down_client_events[i].events & EPOLLOUT) {
                                        break;
                                } else if (down_client_events[i].events & EPOLLHUP) {
                                        down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT];
                                        return;
                                }
                        }
                }

                send_size = !ssl_flag ? client_socket.write_some(
                                    boost::asio::buffer(
                                            data_buff.data() + send_data_size,
                                            data_size - send_data_size),
                                    ec)
                            : client_ssl_socket.write_some(
                                    boost::asio::buffer(
                                            data_buff.data() + send_data_size,
                                            data_size - send_data_size),
                                    ec);
                if (!ec) {
                        send_data_size += send_size;
                        down_thread_data_client_side.set_send_size(send_data_size);
                        parent_service.update_down_send_size(send_size);
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                endpoint client_endpoint // XXX redefined???
                                = !ssl_flag ? client_socket.get_socket().lowest_layer().remote_endpoint(ec)
                                  : client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                                boost::format formatter("Thread ID[%d] down_thread_client_send send data size[%d] for [%d]");
                                formatter % boost::this_thread::get_id() % send_size % client_endpoint;
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 45, formatter.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        if (data_size > send_data_size) {
                                func_tag = DOWN_FUNC_CLIENT_SEND;
                        } else {
                                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_send(down_thread_id);
                                std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
                                func_tag = func_type->second;
                        }
                        break;
                } else {
                        if (ec == boost::asio::error::try_again) {
                                //func_tag = DOWN_FUNC_CLIENT_SEND;
                                //boost::this_thread::yield();
                        } else {
                                func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
                                break;
                        }
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
        boost::system::error_code ec;
        bool bres = !ssl_flag ? client_socket.close(ec)
                    : client_ssl_socket.close(ec);
        if (bres) {
                tcp_thread_message *up_msg = new tcp_thread_message;
                tcp_thread_message *down_msg = new tcp_thread_message;
                down_thread_function_pair down_func = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT];
                down_msg->message = down_func.second;
                std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT_EVENT);
                up_msg->message = up_func->second;
                while (!down_thread_message_que.push(down_msg)) {}
                while (!up_thread_message_que.push(up_msg)) {}
        }

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
        }
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type =
                down_thread_module_event_map.find(module_event);
        down_thread_function_pair func = down_thread_function_array[func_type->second];
        down_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_exit: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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

        if (unlikely(!sorryserver_socket.second->get_socket().lowest_layer().is_open())) {
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_receive");
                        formatter % boost::this_thread::get_id();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
                }
                down_thread_next_call_function = down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE];
                return;
        }

        down_thread_data_dest_side.initialize();
        boost::array<char, MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code ec;
        size_t recv_size;
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;
        endpoint sorry_endpoint;
        int ret_fds;

        struct epoll_event event;
        event.data.fd = sorryserver_socket.second->get_socket().native();
        if (!down_sorryserver_epollfd_registered) {
                if (is_epoll_edge_trigger) {
                        event.events = EPOLLIN | EPOLLHUP | EPOLLET;
                } else {
                        event.events = EPOLLIN | EPOLLHUP;
                }
                if (epoll_ctl(down_sorryserver_epollfd, EPOLL_CTL_ADD, event.data.fd, &event) < 0) {
                        std::stringstream buf;
                        buf << "down_thread_sorryserver_receive: epoll_ctl EPOLL_CTL_ADD error: ";
                        buf << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        boost::this_thread::yield();
                        return;
                }
                down_sorryserver_epollfd_registered = true;
        }

        ret_fds = epoll_wait(down_sorryserver_epollfd, down_sorryserver_events, EVENT_NUM, epoll_timeout);
        if (ret_fds <= 0) {
                if (ret_fds == 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "down_thread_sorryserver_receive: epoll_wait timeout " << epoll_timeout << " msec.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
                } else {
                        std::stringstream buf;
                        buf << "down_thread_sorryserver_receive: epoll_wait error: " << strerror(errno);
                        Logger::putLogWarn(LOG_CAT_L7VSD_SESSION, 999, buf.str(), __FILE__, __LINE__);
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                }
                goto down_thread_sorryserver_receive_out;
        }

        for (int i = 0; i < ret_fds; ++i) {
                if (down_sorryserver_events[i].data.fd == event.data.fd) {
                        if (down_sorryserver_events[i].events & EPOLLIN) {
                                break;
                        } else if (down_sorryserver_events[i].events & EPOLLHUP) {
                                func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                                goto down_thread_sorryserver_receive_out;
                        }
                }
        }

        sorry_endpoint = sorryserver_socket.first;
        down_thread_data_dest_side.set_endpoint(sorry_endpoint);

        recv_size = sorryserver_socket.second->read_some(boost::asio::buffer(data_buff, MAX_BUFFER_SIZE), ec);
        if (!ec) {
                if (recv_size > 0) {
                        //----Debug log----------------------------------------------------------------------
                        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                                std::stringstream buf;
                                buf << "Thread ID[" << boost::this_thread::get_id() << "] ";
                                buf << "down_thread_sorryserver_receive receive data size[" << recv_size << "] from [" << sorry_endpoint << "]";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 46, buf.str(), __FILE__, __LINE__);
                        }
                        //----Debug log----------------------------------------------------------------------
                        down_thread_data_dest_side.set_size(recv_size);
                        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_recv(down_thread_id, sorry_endpoint, data_buff, recv_size);
                        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
                        func_tag = func_type->second;
                } else {
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                }
        } else {
                if (ec == boost::asio::error::try_again) {
                        func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
                } else {
                        func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
                }
        }

down_thread_sorryserver_receive_out:
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

        boost::system::error_code ec;
        bool bres = sorryserver_socket.second->close(ec);
        if (bres) {
                tcp_thread_message *up_msg = new tcp_thread_message;
                tcp_thread_message *down_msg = new tcp_thread_message;
                down_thread_function_pair down_func = down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT];
                down_msg->message = down_func.second;
                down_msg->endpoint_info = sorryserver_socket.first;
                std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
                up_msg->message = up_func->second;
                up_msg->endpoint_info = sorryserver_socket.first;
                while (!up_thread_message_que.push(up_msg)) {}
                while (!down_thread_message_que.push(down_msg)) {}
        }

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

        protocol_module_base::EVENT_TAG module_event;

        boost::system::error_code ec;
        endpoint sorry_endpoint = sorryserver_socket.first;
        bool bres = sorryserver_socket.second->close(ec);
        if (!bres) {
                sorryserver_socket.first = endpoint();
        }
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id, sorry_endpoint);
        }

        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_function_pair func = down_thread_function_array[func_type->second];
        down_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_mod_disconnect: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        endpoint sorry_endpoint = down_thread_message_data.get_endpoint();
        {
                rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
                module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id, sorry_endpoint);
        }
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_function_pair func = down_thread_function_array[func_type->second];
        down_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorryserver_disconnect_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        {
                rw_scoped_lock scope_lock(module_function_sorry_enable_mutex);
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] handle_sorry_enable call";
                        Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 47, buf.str(), __FILE__, __LINE__);
                }
                //----Debug log----------------------------------------------------------------------
                module_event = protocol_module->handle_sorry_enable(down_thread_id);
        }
        std::map<protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG>::iterator func_type = down_thread_module_event_map.find(module_event);
        down_thread_function_pair func = down_thread_function_array[func_type->second];
        down_thread_next_call_function = func;

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_sorry_enable_event: NEXT_FUNC[%s]");
                formatter % boost::this_thread::get_id() % func_tag_to_string(func_type->second);
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
        boost::mutex::scoped_lock status_scoped_lock(downthread_status_mutex);
        downthread_status_cond.notify_one();
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

        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        boost::system::error_code ec;
        while (close_socket != list_end) {
                bool bres = close_socket->second->close(ec);
                if (bres)
                        parent_service.connection_inactive(close_socket->first);
                close_socket++;
        }
        down_thread_receive_realserver_socket_list.clear();
        if (!ssl_flag)
                client_socket.close(ec);
        else
                client_ssl_socket.close(ec);
        sorryserver_socket.second->close(ec);

        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                boost::format formatter("Thread ID[%d] FUNC OUT down_thread_all_socket_close");
                formatter % boost::this_thread::get_id();
                Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 999, formatter.str(), __FILE__, __LINE__);
        }
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

}// namespace l7vs
