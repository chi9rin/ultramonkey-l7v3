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

#include "tcp_session.h"
#include "tcp_thread_message.h"
#include "virtualservice.h"
#include "logger.h"
#include "logger_implement_access.h"
#include "parameter.h"
#include "utility.h"

#define UP_THREAD_ALIVE        std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001)
#define DOWN_THREAD_ALIVE    std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002)
#define UP_THREAD_ACTIVE    std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004)
#define DOWN_THREAD_ACTIVE    std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008)
#define UP_THREAD_LOCK         std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010)
#define DOWN_THREAD_LOCK     std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020)

namespace l7vs{

    //! construcor
    //! @param[in/out]    vs is parent virtualservice object
    //! @param[in/out]    session_io is session use io service object
    //! @param[in]        set_option is
    //!                     session use set socket option info
    //! @param[in]        listen_endpoint is
    //!                     virtualservice listen endpoint
    //! @param[in]        ssl_mode is session use SSL flag
    //! @param[in]        set_ssl_context is
    //!                     session use SSL context object
    //! @param[in]        set_ssl_cache_flag is
    //!                     session use SSL session cache
    //! @param[in]        set_ssl_handshake_time_out is
    //!                     session use SSL handshake timeout
    //! @param[in]        set_access_logger is
    //!                     session use access logger
    tcp_session::tcp_session(virtualservice_tcp& vs,
            boost::asio::io_service& session_io,
            const tcp_socket_option_info set_option,
            const boost::asio::ip::tcp::endpoint listen_endpoint,
            const bool ssl_mode,
            boost::asio::ssl::context& set_ssl_context,
            const bool set_ssl_cache_flag,
            const int set_ssl_handshake_time_out,
            logger_implement_access* set_access_logger)
        :
        io(session_io),
        parent_service(vs),
        exit_flag(false),
        thread_state(0),
        protocol_module(NULL),
        session_pause_flag(false),
        client_socket(session_io,set_option),
        upstream_buffer_size(8192),
        downstream_buffer_size(8192),
        virtualservice_endpoint(listen_endpoint),
        access_log_flag(false),
        access_logger(set_access_logger),
        ssl_flag(ssl_mode),
        client_ssl_socket(session_io, set_ssl_context,set_option),
        ssl_context(set_ssl_context),
        ssl_cache_flag(set_ssl_cache_flag),
        ssl_handshake_timer_flag(false),
        ssl_handshake_time_out(set_ssl_handshake_time_out),
        ssl_handshake_time_out_flag(false),
        socket_opt_info(set_option){

        // sorryserver socket
        tcp_socket_ptr sorry_socket(new tcp_socket(session_io,socket_opt_info));
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
            std::make_pair(UP_FUNC_CLIENT_ACCEPT,  boost::bind(&tcp_session::up_thread_client_accept,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_ACCEPT_EVENT] =
            std::make_pair(UP_FUNC_CLIENT_ACCEPT_EVENT,  boost::bind(&tcp_session::up_thread_client_accept_event,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT] =
            std::make_pair(UP_FUNC_CLIENT_DISCONNECT, boost::bind(&tcp_session::up_thread_client_disconnect,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT] =
            std::make_pair(UP_FUNC_CLIENT_DISCONNECT_EVENT,  boost::bind(&tcp_session::up_thread_client_disconnect_event,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_RECEIVE] =
            std::make_pair(UP_FUNC_CLIENT_RECEIVE, boost::bind(&tcp_session::up_thread_client_receive,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND] =
            std::make_pair(UP_FUNC_CLIENT_RESPOND_SEND, boost::bind(&tcp_session::up_thread_client_respond,this,_1));
        up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT] =
            std::make_pair(UP_FUNC_CLIENT_RESPOND_SEND_EVENT, boost::bind(&tcp_session::up_thread_client_respond_event,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_GET_DEST_EVENT] =
            std::make_pair(UP_FUNC_REALSERVER_GET_DEST_EVENT, boost::bind(&tcp_session::up_thread_realserver_get_destination_event,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT] =
            std::make_pair(UP_FUNC_REALSERVER_CONNECT, boost::bind(&tcp_session::up_thread_realserver_connect,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT] =
            std::make_pair(UP_FUNC_REALSERVER_CONNECT_EVENT, boost::bind(&tcp_session::up_thread_realserver_connect_event,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT] =
            std::make_pair(UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT, boost::bind(&tcp_session::up_thread_realserver_connection_fail_event,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_SEND] =
            std::make_pair(UP_FUNC_REALSERVER_SEND, boost::bind(&tcp_session::up_thread_realserver_send,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT] =
            std::make_pair(UP_FUNC_REALSERVER_DISCONNECT, boost::bind(&tcp_session::up_thread_realserver_disconnect,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT] =
            std::make_pair(UP_FUNC_REALSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::up_thread_realserver_disconnect_event,this,_1));
        up_thread_function_array[UP_FUNC_REALSERVER_ALL_DISCONNECT] =
            std::make_pair(UP_FUNC_REALSERVER_ALL_DISCONNECT, boost::bind(&tcp_session::up_thread_all_realserver_disconnect,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_GET_DEST] =
            std::make_pair(UP_FUNC_SORRYSERVER_GET_DEST, boost::bind(&tcp_session::up_thread_sorryserver_get_destination_event,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT] =
            std::make_pair(UP_FUNC_SORRYSERVER_CONNECT, boost::bind(&tcp_session::up_thread_sorryserver_connect,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT] =
            std::make_pair(UP_FUNC_SORRYSERVER_CONNECT_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_connect_event,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT] =
            std::make_pair(UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_connection_fail_event,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_SEND] =
            std::make_pair(UP_FUNC_SORRYSERVER_SEND, boost::bind(&tcp_session::up_thread_sorryserver_send,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT] =
            std::make_pair(UP_FUNC_SORRYSERVER_DISCONNECT, boost::bind(&tcp_session::up_thread_sorryserver_disconnect,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_MOD_DISCONNECT] =
            std::make_pair(UP_FUNC_SORRYSERVER_MOD_DISCONNECT, boost::bind(&tcp_session::up_thread_sorryserver_mod_disconnect,this,_1));
        up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT] =
            std::make_pair(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event,this,_1));
        up_thread_function_array[UP_FUNC_EXIT] =
            std::make_pair(UP_FUNC_EXIT, boost::bind(&tcp_session::up_thread_exit,this,_1));

        // set up_thread_message_down_thread_function_map
        std::pair<DOWN_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_up_thread_message_func;
        add_up_thread_message_func.first = DOWN_FUNC_CLIENT_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_disconnect_event,this,_1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_client_respond_event,this,_1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_REALSERVER_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_realserver_disconnect_event,this,_1);
        up_thread_message_down_thread_function_map.insert(add_up_thread_message_func);
        add_up_thread_message_func.first = DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT;
        add_up_thread_message_func.second = boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event,this,_1);
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
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT]    = std::make_pair(DOWN_FUNC_CLIENT_DISCONNECT, boost::bind(&tcp_session::down_thread_client_disconnect,this,_1));
        down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_CLIENT_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_client_disconnect_event,this,_1));
        down_thread_function_array[DOWN_FUNC_CLIENT_CONNECTION_CHK]    = std::make_pair(DOWN_FUNC_CLIENT_CONNECTION_CHK, boost::bind(&tcp_session::down_thread_client_connection_chk_event,this,_1));
        down_thread_function_array[DOWN_FUNC_CLIENT_SEND]    = std::make_pair(DOWN_FUNC_CLIENT_SEND, boost::bind(&tcp_session::down_thread_client_send,this,_1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE]    = std::make_pair(DOWN_FUNC_REALSERVER_RECEIVE, boost::bind(&tcp_session::down_thread_realserver_receive,this,_1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT]    = std::make_pair(DOWN_FUNC_REALSERVER_DISCONNECT, boost::bind(&tcp_session::down_thread_realserver_disconnect,this,_1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_realserver_disconnect_event,this,_1));
        down_thread_function_array[DOWN_FUNC_REALSERVER_ALL_DISCONNECT]    = std::make_pair(DOWN_FUNC_REALSERVER_ALL_DISCONNECT, boost::bind(&tcp_session::down_thread_all_realserver_disconnect,this,_1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE]    = std::make_pair(DOWN_FUNC_SORRYSERVER_RECEIVE, boost::bind(&tcp_session::down_thread_sorryserver_receive,this,_1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_DISCONNECT, boost::bind(&tcp_session::down_thread_sorryserver_disconnect,this,_1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT, boost::bind(&tcp_session::down_thread_sorryserver_mod_disconnect,this,_1));
        down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT]    = std::make_pair(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT, boost::bind(&tcp_session::down_thread_sorryserver_disconnect_event,this,_1));
        down_thread_function_array[DOWN_FUNC_EXIT]    = std::make_pair(DOWN_FUNC_EXIT, boost::bind(&tcp_session::down_thread_exit,this,_1));

        // set down_thread_message_up_thread_function_map
        std::pair<UP_THREAD_FUNC_TYPE_TAG , tcp_session_func > add_down_thread_message_func;
        add_down_thread_message_func.first = UP_FUNC_CLIENT_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_client_disconnect_event,this,_1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
        add_down_thread_message_func.first = UP_FUNC_REALSERVER_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_realserver_disconnect_event,this,_1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);
        add_down_thread_message_func.first = UP_FUNC_SORRYSERVER_DISCONNECT_EVENT;
        add_down_thread_message_func.second = boost::bind(&tcp_session::up_thread_sorryserver_disconnect_event,this,_1);
        down_thread_message_up_thread_function_map.insert(add_down_thread_message_func);

        // set virtual_service_message_up_thread_function_map
        std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG , tcp_session_func > add_up_thread_vs_message_func;
        add_up_thread_vs_message_func.first = SORRY_STATE_ENABLE;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_enable_event,this,_1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
        add_up_thread_vs_message_func.first = SORRY_STATE_DISABLE;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_sorry_disable_event,this,_1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);
        add_up_thread_vs_message_func.first = SESSION_END;
        add_up_thread_vs_message_func.second = boost::bind(&tcp_session::up_thread_exit,this,_1);
        virtual_service_message_up_thread_function_map.insert(add_up_thread_vs_message_func);

        // set virtual_service_message_down_thread_function_map
        std::pair<TCP_VIRTUAL_SERVICE_MESSAGE_TAG , tcp_session_func > add_down_thread_vs_message_func;
        add_down_thread_vs_message_func.first = SORRY_STATE_ENABLE;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_enable_event,this,_1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
        add_down_thread_vs_message_func.first = SORRY_STATE_DISABLE;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_sorry_disable_event,this,_1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);
        add_down_thread_vs_message_func.first = SESSION_END;
        add_down_thread_vs_message_func.second = boost::bind(&tcp_session::down_thread_exit,this,_1);
        virtual_service_message_down_thread_function_map.insert(add_down_thread_vs_message_func);

    }
    //! destructor
    tcp_session::~tcp_session(){
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
        tcp_thread_message*    msg;
        while(1){
            msg = up_thread_message_que.pop();
            if( msg )
                delete msg;
            else
                break;
        }
        // down_thread_message_que
        while(1){
            msg = down_thread_message_que.pop();
            if( msg )
                delete msg;
            else
                break;
        }
    }
    //! initialize
    session_result_message tcp_session::initialize(){
        session_result_message msg;
        msg.flag = false;
        msg.message = "";
        exit_flag = false;
        up_thread_id = boost::thread::id();
        down_thread_id = boost::thread::id();
        ssl_handshake_timer_flag = false;
        ssl_handshake_time_out_flag = false;
        thread_state.reset();
        protocol_module = NULL;
        session_pause_flag = false;
        tcp_thread_message* tmp_msg;
        while(1){
            tmp_msg = up_thread_message_que.pop();
            if( tmp_msg )
                delete tmp_msg;
            else
                break;
        }
        while(1){
            tmp_msg = down_thread_message_que.pop();
            if( tmp_msg )
                delete tmp_msg;
            else
                break;
        }

        //load parameter
        l7vs::Parameter        param;
        l7vs::error_code    vs_err;
        int                    int_val;

        int_val    = param.get_int( PARAM_COMP_SESSION, PARAM_UP_BUFFER_SIZE, vs_err );
        if(likely( !vs_err )){
            upstream_buffer_size    = int_val;
        }else{
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 3, "up buffer param error set default 8192" , __FILE__, __LINE__ );    
        }

        int_val    = param.get_int( PARAM_COMP_SESSION, PARAM_DOWN_BUFFER_SIZE, vs_err );
        if(likely( !vs_err )){
            downstream_buffer_size    = int_val;
        }else{
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 4, "down buffer param error set default 8192" , __FILE__, __LINE__ );    
        }

        protocol_module = parent_service.get_protocol_module();

        if(unlikely( protocol_module == NULL )){
            //Error protocol_module NULL
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module is NULL!";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 5, buf.str(), __FILE__, __LINE__ );
            msg.flag = true;
            msg.message = "Not fond protocol module";
        }

        // Reset SSL structure to allow another connection.
        if ( ssl_flag ) {
            if ( ssl_cache_flag ) {
                if (unlikely(ssl_clear_keep_cache(client_ssl_socket.get_socket().impl()->ssl) == 0)) {
                    //Error ssl_clear_keep_cache
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] ssl_clear_keep_cache failed";
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 110, buf.str(), __FILE__, __LINE__ );
                    msg.flag = true;
                    msg.message = "ssl_clear_keep_cache failed";
                } else {
                    //----Debug log----------------------------------------------------------------------
                    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] ssl_clear_keep_cache ok";
                        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                                    buf.str(),
                                    __FILE__, __LINE__ );
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
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 111, buf.str(), __FILE__, __LINE__ );
                    msg.flag = true;
                    msg.message = "SSL_clear failed";
                }else{
                    //----Debug log----------------------------------------------------------------------
                    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                        std::stringstream buf;
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] SSL_clear ok";
                        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                                    buf.str(),
                                    __FILE__, __LINE__ );
                    }
                    //----Debug log----------------------------------------------------------------------
                }
            }
        }

        return msg;
    }

    //! reset ssl object for reuse (keep SSL session cache)
    //! this function based from SSL_clear(SSL *s) in OpenSSL.
    //! keeping SSL session cache it by not executing ssl_clear_bad_session(s). 
    //! OpenSSL ssl_lib.c code is GNU Public Licence.
    //! @param[in]        ssl object
    //! @return         1 is clear OK
    //! @return         0 is clear NG
    int tcp_session::ssl_clear_keep_cache(SSL *s)
    {
        if (s->method == NULL) {
            SSLerr(SSL_F_SSL_CLEAR,SSL_R_NO_METHOD_SPECIFIED);
            return(0);
        }

        // not execute ssl_clear_bad_session(s) and SSL_SESSION_free(s->session)
        // set NULL only
        s->session=NULL;

        s->error=0;
        s->hit=0;
        s->shutdown=0;

        if (s->new_session) {
            SSLerr(SSL_F_SSL_CLEAR,ERR_R_INTERNAL_ERROR);
            return 0;
        }

        s->type=0;

        s->state=SSL_ST_BEFORE|((s->server)?SSL_ST_ACCEPT:SSL_ST_CONNECT);

        s->version=s->method->version;
        s->client_version=s->version;
        s->rwstate=SSL_NOTHING;
        s->rstate=SSL_ST_READ_HEADER;

        if (s->init_buf != NULL) {
            BUF_MEM_free(s->init_buf);
            s->init_buf=NULL;
        }

        // ssl_clear_cipher_ctx(SSL *s)
        if (s->enc_read_ctx != NULL) {
            EVP_CIPHER_CTX_cleanup(s->enc_read_ctx);
            OPENSSL_free(s->enc_read_ctx);
            s->enc_read_ctx=NULL;
        }
        if (s->enc_write_ctx != NULL) {
            EVP_CIPHER_CTX_cleanup(s->enc_write_ctx);
            OPENSSL_free(s->enc_write_ctx);
            s->enc_write_ctx=NULL;
        }
        if (s->expand != NULL) {
            COMP_CTX_free(s->expand);
            s->expand=NULL;
        }
        if (s->compress != NULL) {
            COMP_CTX_free(s->compress);
            s->compress=NULL;
        }

        s->first_packet=0;

        /* Check to see if we were changed into a different method, if
         * so, revert back if we are not doing session-id reuse. */
        if (!s->in_handshake && (s->session == NULL) && (s->method != s->ctx->method)) {
//            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 69, "In ssl_clear_keep_cache() method changed", __FILE__, __LINE__ );
            s->method->ssl_free(s);
            s->method=s->ctx->method;
            if (!s->method->ssl_new(s)) {
                return(0);
            }
        } else {
            s->method->ssl_clear(s);
        }
        return(1);
    }

    //! get reference client side socket
    //! @return            reference client side socket
    boost::asio::ip::tcp::socket& tcp_session::get_client_socket()
    {
        return client_socket.get_socket();
    }
    //! get reference client side ssl socket
    //! @return            reference client side ssl socket
    ssl_socket& tcp_session::get_client_ssl_socket()
    {
        return client_ssl_socket.get_socket();
    }
    //! is thread wait
    //! @return         true is wait
    //! @return         false is not wait
    bool tcp_session::is_thread_wait(){
        bool res = false;
        rd_scoped_lock scope_lock(thread_state_update_mutex);
        if(thread_state.test(4) & thread_state.test(5))
            res = true;
        return res;
    }
    //! message from parent virtualservice
    //! @param[in]        message is tcp virtualservice message type
    void tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
        switch(message){
            case SESSION_PAUSE_ON:
                {
                    rw_scoped_lock scope_lock(session_pause_flag_mutex);
                    session_pause_flag = true;
                }
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[SESSION_PAUSE_ON]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 12, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                return;
            case SESSION_PAUSE_OFF:
                {
                    rw_scoped_lock scope_lock(session_pause_flag_mutex);
                    session_pause_flag = false;
                }
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[SESSION_PAUSE_OFF]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 13, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                return;
            case ACCESS_LOG_ON:
                {
                    rw_scoped_lock scope_lock(access_log_flag_mutex);
                    access_log_flag = true;
                }
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[ACCESS_LOG_ON]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 49, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                return;
            case ACCESS_LOG_OFF:
                {
                    rw_scoped_lock scope_lock(access_log_flag_mutex);
                    access_log_flag = false;
                }
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[ACCESS_LOG_OFF]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 68, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                return;
            case SORRY_STATE_ENABLE:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[SORRY_STATE_ENABLE]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 14, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                break;
            case SORRY_STATE_DISABLE:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[SORRY_STATE_DISABLE]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 15, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                break;
            case SESSION_END:
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set_virtual_service_message message:[SESSION_END]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 16, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                break;
        }
        // set message
        tcp_thread_message*    up_msg    = new tcp_thread_message;
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator up_func;
        up_func = virtual_service_message_up_thread_function_map.find(message);
        if(up_func != virtual_service_message_up_thread_function_map.end()){
            up_msg->message = up_func->second;
            while(!up_thread_message_que.push(up_msg)){}
        }else{
            //Error illegal virtualservice message type
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread illegal virtualservice message type : ";
            buf << message;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 6, buf.str(), __FILE__, __LINE__ );
        }
        tcp_thread_message*    down_msg    = new tcp_thread_message;
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>::iterator down_func;
        down_func = virtual_service_message_down_thread_function_map.find(message);
        if(down_func != virtual_service_message_down_thread_function_map.end()){
            down_msg->message = down_func->second;
            while(!down_thread_message_que.push(down_msg)){}
        }else{
            //Error illegal virtualservice message type
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down thread illegal virtualservice message type : ";
            buf << message;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 7, buf.str(), __FILE__, __LINE__ );
        }
    }
    //! up stream thread main function
    void tcp_session::up_thread_run(void){
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up_thread_run start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 17, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        up_thread_id = boost::this_thread::get_id(); 
        thread_state_update(UP_THREAD_ALIVE,true);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread down thread alive wait start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 18, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        while(true){ 
            //wait down thread get id
            {
                rd_scoped_lock scope_lock(thread_state_update_mutex);
                if(unlikely( thread_state.test(1) )){// DOWN_THREAD_ALIVE
                    break;
                }
            }
            boost::this_thread::yield();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread down thread alive wait end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 19, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        if(unlikely( protocol_module == NULL )){
            //Error!
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module is NULL!";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 8, buf.str(), __FILE__, __LINE__ );
            {
                rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                exit_flag = true;
            }
        }
        bool is_exit;
        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely( !is_exit )){
            bool bres;
            if (!ssl_flag) {
                bres = client_socket.get_socket().lowest_layer().is_open();
            } else {
                bres = client_ssl_socket.get_socket().lowest_layer().is_open();
            }
            if(unlikely( !bres )){
                //client socket not open Error!
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] client socket not open!";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 9, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }
        }
        
        boost::system::error_code ec;
        if (!ssl_flag) {
            client_socket.accept();
        } else {
            client_ssl_socket.accept();
        }
        endpoint cl_end;
        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely( !is_exit )){
            if (!ssl_flag) {
                cl_end = client_socket.get_socket().lowest_layer().remote_endpoint(ec);
            } else {
                cl_end = client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
            }
            if(unlikely( ec )){
                //client endpoint get Error!
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] client endpoint get false : ";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 10, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }
        }
        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely( !is_exit )){
            bool bres;
            if (!ssl_flag) {
                bres = client_socket.set_non_blocking_mode(ec);
            } else {
                bres = client_ssl_socket.set_non_blocking_mode(ec);
            }
            if(unlikely( !bres )){
                // socket set nonblocking mode error
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] set non blocking socket error :";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 11, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }
        }

        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely( !is_exit )){
            //set client_socket options(recieve buffer size)
            boost::asio::socket_base::receive_buffer_size    opt1( upstream_buffer_size );
            if (!ssl_flag) {
                client_socket.get_socket().lowest_layer().set_option( opt1 ,ec);
            } else {
                client_ssl_socket.get_socket().lowest_layer().set_option( opt1 ,ec);
            }
            if(unlikely( ec )){
                //client socket Error!
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] client socket recieve buffer size error : ";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 12, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }
        }

        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely( !is_exit )){
            //set client_socket options(send buffer size)
            boost::asio::socket_base::send_buffer_size        opt2( downstream_buffer_size );
            if (!ssl_flag) {
                client_socket.get_socket().lowest_layer().set_option( opt2 ,ec);
            } else {
                client_ssl_socket.get_socket().lowest_layer().set_option( opt2 ,ec);
            }
            if(unlikely( ec )){
                //client socket Error!
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] client socket send buffer size error : ";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 13, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }
        }

        boost::asio::ip::udp::endpoint dumy_end;
        protocol_module_base::EVENT_TAG module_event;
        std::map< protocol_module_base::EVENT_TAG , UP_THREAD_FUNC_TYPE_TAG >::iterator func_type;
        up_thread_function_pair    func;
        {
            rd_scoped_lock scoped_lock(exit_flag_update_mutex);
            is_exit = exit_flag;
        }
        if(likely(!is_exit)){
            module_event = protocol_module->handle_session_initialize(up_thread_id,down_thread_id,cl_end,dumy_end);
            func_type = up_thread_module_event_map.find(module_event);
            if(unlikely( func_type == up_thread_module_event_map.end() )){
                //Error unknown protocol_module_base::EVENT_TAG return
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] protocol_module returnd illegal EVENT_TAG : ";
                buf << module_event;    
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 14, buf.str(), __FILE__, __LINE__ );
                {
                    rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                    exit_flag = true;
                }
            }else{
                func = up_thread_function_array[func_type->second];
                if(unlikely( !func.second )){
                    //Error not find function map
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
                    buf << func_type->second;
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 15, buf.str(), __FILE__, __LINE__ );
                    {
                        rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                        exit_flag = true;
                    }
                }else{
                    up_thread_next_call_function = func;
                }
            }
        }
        thread_state_update(UP_THREAD_ACTIVE,true);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread loop start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 20, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        bool is_pause;
        while(true){
            {
                rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                if(unlikely(exit_flag)) break;
            }

            {
                rd_scoped_lock scope_lock(session_pause_flag_mutex);
                is_pause = session_pause_flag;
            }
            if(unlikely(is_pause)){
                thread_state_update(UP_THREAD_LOCK,true);
                while(true){
                    {
                        rd_scoped_lock scope_lock(session_pause_flag_mutex);
                        if(!session_pause_flag) break;
                    }
                    {
                        rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                        if(exit_flag) break;
                    }
                }
                thread_state_update(UP_THREAD_LOCK,false);
                {
                    rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                    if(exit_flag) break;
                }
            }

            tcp_thread_message*    msg    = up_thread_message_que.pop();
            if(unlikely( msg )){
                if(unlikely( UP_FUNC_EXIT == up_thread_next_call_function.first )){
                    up_thread_next_call_function.second(LOCAL_PROC);
                }else{
                    up_thread_message_data.set_endpoint(msg->endpoint_info);
                    msg->message(MESSAGE_PROC);
                }
                delete  msg;
                msg = NULL;
            }else{
                up_thread_next_call_function.second(LOCAL_PROC);
            }

            boost::this_thread::yield();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread loop end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 21, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        
        up_thread_all_socket_close();
        thread_state_update(UP_THREAD_ACTIVE,false);

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread down thread dead wait start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 22, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        while(true){ 
            // wait down thread alive
            {
                rd_scoped_lock scope_lock(thread_state_update_mutex);
                if(unlikely( !thread_state.test(1) )){// DOWN_THREAD_ALIVE
                    break;
                }
            }
            boost::this_thread::yield();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread down thread dead wait end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 23, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        if(likely( protocol_module != NULL ))
            protocol_module->handle_session_finalize(up_thread_id,down_thread_id);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread call protocol_module_base handle_session_finalize() return";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 24, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        thread_state_update(UP_THREAD_ALIVE,false);
        parent_service.release_session(this);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up thread call parent service release_session() return";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 25, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] up_thread_run end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 26, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
    }
    
    //! down stream thread main function
    void tcp_session::down_thread_run(void){
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down_thread_run start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 27, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        down_thread_id = boost::this_thread::get_id(); 
        thread_state_update(DOWN_THREAD_ALIVE,true);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down_thread_run up thread active wait start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 28, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        while(true){ // UP_THREAD_ACTIVE
            // wait up thread active
            {
                rd_scoped_lock scope_lock(thread_state_update_mutex);
                if(unlikely( thread_state.test(2) )){
                    break;
                }
            }
            {
                rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                if(unlikely(  exit_flag )) break;
            }
            boost::this_thread::yield();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down_thread_run up thread active wait end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 29, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        thread_state_update(DOWN_THREAD_ACTIVE,true);
        down_thread_function_pair    func    = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_RECV";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 16, buf.str(), __FILE__, __LINE__ );
            {
                rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                exit_flag = true;
            }
        }else{
            down_thread_next_call_function = func;
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down thread loop start";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 30, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------

        if(unlikely( protocol_module == NULL )){
            //Error!
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module is NULL!";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 17, buf.str(), __FILE__, __LINE__ );
            {
                rw_scoped_lock scoped_lock(exit_flag_update_mutex);
                exit_flag = true;
            }
        }
        bool is_pause;
        while(true){
            {
                rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                if(unlikely(exit_flag)) break;
            }

            {
                rd_scoped_lock scope_lock(session_pause_flag_mutex);
                is_pause = session_pause_flag;
            }
            if(unlikely(is_pause)){
                thread_state_update(DOWN_THREAD_LOCK,true);
                while(true){
                    {
                        rd_scoped_lock scope_lock(session_pause_flag_mutex);
                        if(!session_pause_flag) break;
                    }
                    {
                        rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                        if(exit_flag) break;
                    }
                }
                thread_state_update(DOWN_THREAD_LOCK,false);
                {
                    rd_scoped_lock scoped_lock(exit_flag_update_mutex);
                    if(exit_flag) break;
                }
            }
            while(unlikely( !down_thread_connect_socket_list.empty() )){
                std::pair<endpoint,tcp_socket_ptr > push_rs_socket = down_thread_connect_socket_list.get_socket();
                down_thread_receive_realserver_socket_list.push_back(push_rs_socket);
                down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
            }

            tcp_thread_message*    msg    = down_thread_message_que.pop();
            if(unlikely( msg )){
                if(unlikely( DOWN_FUNC_EXIT == down_thread_next_call_function.first )){
                    down_thread_next_call_function.second(LOCAL_PROC);
                }else{
                    down_thread_message_data.set_endpoint(msg->endpoint_info);
                    msg->message(MESSAGE_PROC);
                }
                delete  msg;
                msg = NULL;
            }else{
                down_thread_next_call_function.second(LOCAL_PROC);
            }
            boost::this_thread::yield();
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down thread loop end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 31, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        down_thread_all_socket_close();
        thread_state_update(DOWN_THREAD_ACTIVE,false);
        thread_state_update(DOWN_THREAD_ALIVE,false);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] down_thread_run end";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 32, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
    }

    //! up and down thread state update
    //! @param[in]        thread_flag is regist or unregist bitset
    //! @param[in]        regist is regist or unregist flag
    void tcp_session::thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){
        rw_scoped_lock scope_lock(thread_state_update_mutex);
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] thread_state_update";
            buf << " thread_flag[";
            buf << thread_flag;
            buf << "] regist[";
            buf << regist;
            buf << "] thread_state[";
            buf << thread_state;
            buf << "]";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 33, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
        if(regist){
            thread_state |= thread_flag;
        }else{
            std::bitset<TCP_SESSION_THREAD_STATE_BIT> ret_flag = thread_flag;
            ret_flag.flip();
            thread_state &= ret_flag;
        }
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] thread_state_update";
            buf << " update thread_state[";
            buf << thread_state;
            buf << "]";
            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 34, buf.str(), __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
    }

    //! up thread accept client side
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_accept(const TCP_PROCESS_TYPE_TAG process_type){

        UP_THREAD_FUNC_TYPE_TAG func_tag;

        if( ssl_flag ){
            //SSL Mode
            rd_scoped_lock scoped_lock(ssl_handshake_time_out_flag_mutex);
            if( ssl_handshake_time_out_flag ){
                // SSL handshake time out or timer error
                func_type = UP_FUNC_CLIENT_DISCONNECT;
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] handshake timer timeout ";
                buf << ssl_handshake_time_out;
                buf << " sec : handshaking not end";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 112, buf.str(), __FILE__, __LINE__ );
            }else{
                if( ssl_handshake_timer_flag == false){
                    // set handshake timer
                    //regist handshake timer event handler
                    ssl_handshake_timer.reset(new boost::asio::deadline_timer(io));
                    ssl_handshake_timer->expires_from_now(boost::posix_time::seconds(ssl_handshake_time_out));
                    ssl_handshake_timer->async_wait(boost::bind(&tcp_session::handle_ssl_handshake_timer, 
                                                            this,
                                                            boost::asio::placeholders::error));
                    ssl_handshake_timer_flag = true;
                    //----Debug log----------------------------------------------------------------------
                    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
                            std::stringstream buf;
                            buf << "Thread ID[";
                            buf << boost::this_thread::get_id();
                            buf << "] ssl session handshaking start : ";
                            buf << "set handshake timer [";
                            buf << handshake_timeout;
                            buf << "]";
                            Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 70, buf.str(), __FILE__, __LINE__ );
                    }
                    //----Debug log----------------------------------------------------------------------
                }
                // try ssl handshake
                boost::system::error_code ec;
                client_ssl_socket.handshake( ec );
                
                if( !ec ){
                    // handshake ok
                    ssl_handshake_timer->cancel();
                    func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;
                }else{
                    // handshake fail
                    if(ec == boost::asio::error::try_again){
                        func_tag = UP_FUNC_CLIENT_ACCEPT;
                        boost::this_thread::yield();
                    }else{
                        //ERROR
                        ssl_handshake_timer->cancel();
                        func_tag = UP_FUNC_CLIENT_DISCONNECT;
                        Logger::putLogError(LOG_CAT_L7VSD_SESSION, 113, "ssl socket handshaking failed" , __FILE__, __LINE__);
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
        }else{
            func_tag = UP_FUNC_CLIENT_ACCEPT_EVENT;
        }
        up_thread_next_call_function = up_thread_function_array[func_tag];
    }

    //! up thread raise module event of handle_accept and do handshake
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){
        
        protocol_module_base::EVENT_TAG module_event;
        module_event = protocol_module->handle_accept(up_thread_id);
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely(func_type == up_thread_module_event_map.end())){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 18, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }

        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 19, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }

    //! handshake timer handler
    //! @param[in]        error is timer operation result error code
    void tcp_session::handle_ssl_handshake_timer() {
        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
            Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 72, "in_function : tcp_session::handle_ssl_handshake_timer", __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------

        rw_scoped_lock scoped_lock(ssl_handshake_time_out_flag_mutex);
        ssl_handshake_time_out_flag = true;

        //----Debug log----------------------------------------------------------------------
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))) {
            Logger::putLogDebug(LOG_CAT_L7VSD_SESSION, 73, "out_function : tcp_session::handle_ssl_handshake_timer", __FILE__, __LINE__ );
        }
        //----Debug log----------------------------------------------------------------------
    }

    //! up thread receive client side and raise module event of handle_client_recv
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){
        
        if(unlikely(0 < parent_service.get_wait_upstream())){
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] up_thread_client_receive qos wait active";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 35, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            return; // try again
        }
        up_thread_data_client_side.initialize();
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_client_side.get_data();
        boost::system::error_code ec;
        std::size_t recv_size;
        if (!ssl_flag) {
            recv_size = client_socket.read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE), ec);
        } else {
            recv_size = client_ssl_socket.read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE), ec);
        }
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if(!ec){
            if(recv_size > 0){
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    boost::asio::ip::tcp::endpoint client_endpoint;
                    if (!ssl_flag) {
                        client_endpoint = client_socket.get_socket().lowest_layer().remote_endpoint(ec);
                    } else {
                        client_endpoint = client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                    }
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] up_thread_client_receive";
                    buf << " receive data size[";
                    buf << recv_size;
                    buf << "] from [";
                    buf << client_endpoint;
                    buf << "]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 36, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                up_thread_data_client_side.set_size(recv_size);
                parent_service.update_up_recv_size(recv_size);
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_recv(up_thread_id,data_buff,recv_size);
                std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
                if(unlikely( func_type == up_thread_module_event_map.end() )){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 20, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }else{
                func_tag = UP_FUNC_CLIENT_RECEIVE;
                boost::this_thread::yield();
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = UP_FUNC_CLIENT_RECEIVE;
                boost::this_thread::yield();
            }else{
                func_tag = UP_FUNC_CLIENT_DISCONNECT;
            }
        }
        up_thread_function_pair    func    = up_thread_function_array[func_tag];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 21, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise client respond send event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){
        tcp_thread_message*        up_msg        = new tcp_thread_message;
        tcp_thread_message*        down_msg    = new tcp_thread_message;
        up_thread_function_pair    up_func    = up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT];
        if(unlikely( !up_func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_RESPOND_SEND_EVENT";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 22, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_msg->message = up_func.second;
        std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
        if(unlikely( down_func == up_thread_message_down_thread_function_map.end() )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT";
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 23, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        down_msg->message = down_func->second;
        while(!up_thread_message_que.push(up_msg)){}
        while(!down_thread_message_que.push(down_msg)){}
    }
    //! up thread raise module event of handle_response_send_inform
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_response_send_inform_mutex);
            module_event = protocol_module->handle_response_send_inform(up_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 24, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 25, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread close client socket and raise client disconnect event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        bool bres;
        if (!ssl_flag) {
            bres = client_socket.close(ec);
        } else {
            bres = client_ssl_socket.close(ec);
        }
        if(bres){
            tcp_thread_message*        up_msg        = new tcp_thread_message;
            tcp_thread_message*        down_msg    = new tcp_thread_message;
            up_thread_function_pair    up_func    = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT];
            if(unlikely( !up_func.second )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 26, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            up_msg->message = up_func.second;
            std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
            if(unlikely( down_func == up_thread_message_down_thread_function_map.end() )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 27, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            down_msg->message = down_func->second;
            while(!up_thread_message_que.push(up_msg)){}
            while(!down_thread_message_que.push(down_msg)){}
        }
    }
    //! up thread raise module event of handle_client_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_client_disconnect_mutex);
            module_event = protocol_module->handle_client_disconnect(up_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if( unlikely( func_type == up_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 28, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 29, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread send realserver and raise module event of handle_client_recv
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint,tcp_socket_ptr>::iterator send_socket =  up_thread_send_realserver_socket_map.find(server_endpoint);
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        std::size_t data_size = up_thread_data_dest_side.get_size();
        std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
        std::size_t send_size = send_socket->second->write_some(boost::asio::buffer(data_buff.data()+send_data_size,data_size-send_data_size),ec);
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if( !ec ){
            send_data_size += send_size;
            up_thread_data_dest_side.set_send_size(send_data_size);
            parent_service.update_up_send_size(send_size);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] up_thread_realserver_send";
                buf << " send data size[";
                buf << send_size;
                buf << "] for [";
                buf << server_endpoint;
                buf << "]";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 37, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            if(data_size > send_data_size){
                func_tag = UP_FUNC_REALSERVER_SEND;
            }else{
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_send(up_thread_id);
                std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
                if( unlikely( func_type == up_thread_module_event_map.end() )){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;    
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 30, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = UP_FUNC_REALSERVER_SEND;
            }else{
                func_tag = UP_FUNC_REALSERVER_DISCONNECT;
            }
        }
        up_thread_function_pair    func    = up_thread_function_array[func_tag];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 31, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_realserver_select
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint server_endpoint;
        
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_select(up_thread_id,server_endpoint);
        up_thread_data_dest_side.set_endpoint(server_endpoint);
        
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if( unlikely( func_type == up_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 32, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 33, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread connect realserver
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint,tcp_socket_ptr>::iterator get_socket =  up_thread_send_realserver_socket_map.find(server_endpoint);
        std::map<endpoint,tcp_socket_ptr>::iterator map_end =  up_thread_send_realserver_socket_map.end();
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if(get_socket != map_end){
            func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
        }else{
            tcp_socket_ptr new_socket(new tcp_socket(io,socket_opt_info));
            boost::system::error_code ec;
            bool bres = new_socket->connect(server_endpoint,ec);
            if(likely( bres )){
                parent_service.connection_active(server_endpoint);
                if(unlikely( !new_socket->set_non_blocking_mode(ec) )){
                    // socket set nonblocking mode error
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] set non blocking socket error :";
                    buf << ec.message();
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 34, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }

                //set realserver_socket options(recieve buffer size)
                boost::asio::socket_base::receive_buffer_size    opt1( downstream_buffer_size );
                new_socket->get_socket().set_option(opt1 , ec);
                if(unlikely( ec )){
                    // socket set nonblocking mode error
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] realserver socket recieve buffer size error : ";
                    buf << ec.message();
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 35, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }
                //set realserver_socket options(send buffer size)
                boost::asio::socket_base::send_buffer_size        opt2( upstream_buffer_size );
                new_socket->get_socket().set_option(opt2 , ec);
                if(unlikely( ec )){
                    // socket set nonblocking mode error
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] realserver socket send buffer size error : ";
                    buf << ec.message();
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 36, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }

                socket_element push_element;
                push_element.first = server_endpoint;
                push_element.second = new_socket;
                up_thread_send_realserver_socket_map.insert(push_element);
                down_thread_connect_socket_list.push_back(push_element);
                func_tag = UP_FUNC_REALSERVER_CONNECT_EVENT;
            }else{
                func_tag = UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT;
                //connect socket error
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] connect socket error :";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 37, buf.str(), __FILE__, __LINE__ );
            }
        }
        up_thread_function_pair    func    = up_thread_function_array[func_tag];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 38, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_realserver_connect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        up_thread_data_dest_side.initialize();
        boost::array< char , MAX_BUFFER_SIZE >& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connect(up_thread_id,data_buff,data_size);
        up_thread_data_dest_side.set_endpoint(server_endpoint);
        up_thread_data_dest_side.set_size(data_size);
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 39, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 40, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_realserver_connection_fail
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_connection_fail(up_thread_id,server_endpoint);
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely(func_type == up_thread_module_event_map.end())){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 41, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 42, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread close realserver socket and raise realserver disconnect event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.find(server_endpoint);
        boost::system::error_code ec;
        bool bres = close_socket->second->close(ec);
        if(bres){
            parent_service.connection_inactive(server_endpoint);
            tcp_thread_message*        up_msg        = new tcp_thread_message;
            tcp_thread_message*        down_msg    = new tcp_thread_message;
            up_thread_function_pair    up_func    = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT];
            if(unlikely( !up_func.second )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_REALSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 43, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            up_msg->endpoint_info = server_endpoint;
            up_msg->message = up_func.second;
            std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
            if(unlikely( down_func == up_thread_message_down_thread_function_map.end() )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 44, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            down_msg->endpoint_info = server_endpoint;
            down_msg->message = down_func->second;
            while(!up_thread_message_que.push(up_msg)){}
            while(!down_thread_message_que.push(down_msg)){}
        }
    }
    //! up thread raise module event of handle_client_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        boost::asio::ip::tcp::endpoint server_endpoint = up_thread_message_data.get_endpoint();
        {
            rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
            module_event = protocol_module->handle_realserver_disconnect(up_thread_id,server_endpoint);
        }
        up_thread_send_realserver_socket_map.erase(server_endpoint);
        
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 45, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 46, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread close all realserver socket and raise module event of handle_realserver_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
        std::map<endpoint,tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();
        
        protocol_module_base::EVENT_TAG module_event;
        bool realserver_fond = false;
        boost::asio::ip::tcp::endpoint server_endpoint;
        while(close_socket != list_end){
            realserver_fond = true;
            boost::system::error_code ec;
            bool bres = close_socket->second->close(ec);
            server_endpoint = close_socket->first;
            if(bres){
                parent_service.connection_inactive(server_endpoint);
            }
            {
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(up_thread_id,server_endpoint);
            }
            close_socket++;
        }
        if(!realserver_fond){
            //not fond realserver
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] close realserver not fond";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 38, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
            module_event = protocol_module->handle_realserver_disconnect(up_thread_id,boost::asio::ip::tcp::endpoint());
        }
        up_thread_send_realserver_socket_map.clear();
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 47, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 48, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread send sorryserver and raise module event of handle_sorryserver_send
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = up_thread_data_dest_side.get_data();
        std::size_t data_size = up_thread_data_dest_side.get_size();
        std::size_t send_data_size = up_thread_data_dest_side.get_send_size();
        std::size_t send_size = sorryserver_socket.second->write_some(boost::asio::buffer(data_buff.data()+send_data_size,data_size-send_data_size),ec);
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if(!ec){
            send_data_size += send_size;
            up_thread_data_dest_side.set_send_size(send_data_size);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] up_thread_sorryserver_send";
                buf << " send data size[";
                buf << send_size;
                buf << "] for [";
                buf << sorry_endpoint;
                buf << "]";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 39, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            if(data_size > send_data_size){
                func_tag = UP_FUNC_SORRYSERVER_SEND;
            }else{
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_send(up_thread_id);
                std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
                if(unlikely( func_type == up_thread_module_event_map.end() )){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 49, buf.str(), __FILE__, __LINE__ );
                    up_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = UP_FUNC_SORRYSERVER_SEND;
            }else{
                func_tag = UP_FUNC_SORRYSERVER_DISCONNECT;
            }
        }
        up_thread_function_pair    func    = up_thread_function_array[func_tag];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 50, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorryserver_select
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint server_endpoint;
        virtualservice_element& vs_element = parent_service.get_element();
        server_endpoint = vs_element.sorry_endpoint;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_select(up_thread_id,server_endpoint);
        up_thread_data_dest_side.set_endpoint(server_endpoint);
        
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely(func_type == up_thread_module_event_map.end())){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 51, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 52, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread connect sorryserver
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        boost::system::error_code ec;
        bool bres = sorryserver_socket.second->connect(sorry_endpoint,ec);
        UP_THREAD_FUNC_TYPE_TAG func_tag;
        if(likely( bres )){
            if(unlikely( !sorryserver_socket.second->set_non_blocking_mode(ec) )){
                    // socket set nonblocking mode error
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] set non blocking socket error :";
                buf << ec.message();
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 53, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
            }
            sorryserver_socket.first = sorry_endpoint;
            func_tag = UP_FUNC_SORRYSERVER_CONNECT_EVENT;
        }else{
            func_tag = UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT;
            //connect socket error
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] connect socket error :";
            buf << ec.message();
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 54, buf.str(), __FILE__, __LINE__ );
        }
        up_thread_function_pair    func    = up_thread_function_array[func_tag];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 55, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorryserver_connect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
        endpoint sorry_endpoint = up_thread_data_dest_side.get_endpoint();
        up_thread_data_dest_side.initialize();
        boost::array< char , MAX_BUFFER_SIZE >& data_buff = up_thread_data_dest_side.get_data();
        size_t data_size = 0;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connect(up_thread_id,data_buff,data_size);
        up_thread_data_dest_side.set_endpoint(sorry_endpoint);
        up_thread_data_dest_side.set_size(data_size);
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 56, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 57, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorryserver_connection_fail
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
        endpoint server_endpoint = up_thread_data_dest_side.get_endpoint();
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_connection_fail(up_thread_id,server_endpoint);
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 58, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 59, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        bool bres = sorryserver_socket.second->close(ec);
        if(bres){
            tcp_thread_message*        up_msg        = new tcp_thread_message;
            tcp_thread_message*        down_msg    = new tcp_thread_message;
            up_thread_function_pair    up_func    = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT];
            if(unlikely( !up_func.second )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_SORRYSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 60, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            up_msg->message = up_func.second;
            up_msg->endpoint_info = sorryserver_socket.first;
            std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator down_func = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT);
            if(unlikely( down_func == up_thread_message_down_thread_function_map.end() )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 61, buf.str(), __FILE__, __LINE__ );
                up_thread_exit(process_type);
                return;
            }
            down_msg->message = down_func->second;
            down_msg->endpoint_info = sorryserver_socket.first;
            while(!up_thread_message_que.push(up_msg)){}
            while(!down_thread_message_que.push(down_msg)){}
        }
    }
    //! up thread close sorryserver socket and raise module sorryserver disconnect event
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        
        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint sorry_endpoint = sorryserver_socket.first;
        bool bres = sorryserver_socket.second->close(ec);
        if(!bres){
            sorryserver_socket.first = boost::asio::ip::tcp::endpoint();
        }
        {
            rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
            module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id,sorry_endpoint);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 62, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 63, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorryserver_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        boost::asio::ip::tcp::endpoint sorry_endpoint = up_thread_message_data.get_endpoint();
        {
            rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
            module_event = protocol_module->handle_sorryserver_disconnect(up_thread_id,sorry_endpoint);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 64, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 65, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorry_enable
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_sorry_enable_mutex);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] handle_sorry_enable call";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 40, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            module_event = protocol_module->handle_sorry_enable(up_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if(unlikely( func_type == up_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 66, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 67, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    //! up thread raise module event of handle_sorry_disable
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_sorry_disable_mutex);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] handle_sorry_disable call";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 41, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            module_event = protocol_module->handle_sorry_disable(up_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,UP_THREAD_FUNC_TYPE_TAG >::iterator func_type = up_thread_module_event_map.find(module_event);
        if( unlikely( func_type == up_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 68, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_function_pair    func    = up_thread_function_array[func_type->second];
        if( unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 69, buf.str(), __FILE__, __LINE__ );
            up_thread_exit(process_type);
            return;
        }
        up_thread_next_call_function = func;
    }
    
    //! up thread exit main loop
    //! @param[in]        process_type is prosecess type
    void tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
        rw_scoped_lock scoped_lock(exit_flag_update_mutex);
        exit_flag = true;
    }
    //! up thread close all socket
    void tcp_session::up_thread_all_socket_close(void){
        std::map<endpoint,tcp_socket_ptr>::iterator close_socket = up_thread_send_realserver_socket_map.begin();
        std::map<endpoint,tcp_socket_ptr>::iterator list_end = up_thread_send_realserver_socket_map.end();
        boost::system::error_code ec;
        while(close_socket != list_end){
            bool bres = close_socket->second->close(ec);
            if(bres)
                parent_service.connection_inactive(close_socket->first);
            close_socket++;
        }
        up_thread_send_realserver_socket_map.clear();
        down_thread_connect_socket_list.clear();
        if (!ssl_flag) {
            client_socket.close(ec);
        } else {
            client_ssl_socket.close(ec);
        }
        sorryserver_socket.second->close(ec);
    }

    //! down thread receive from realserver and raise module event of handle_realserver_recv
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
        bool is_emp = down_thread_receive_realserver_socket_list.empty();
        if(is_emp){
            boost::this_thread::yield();
            return;
        }

        if(unlikely( 0 < parent_service.get_wait_downstream() ) ){
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] down_thread_realserver_receive qos wait active";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 42, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
             return;
        }
        down_thread_data_dest_side.initialize();
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code ec;
        size_t recv_size = down_thread_current_receive_realserver_socket->second->read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE),ec);
        boost::asio::ip::tcp::endpoint server_endpoint = down_thread_current_receive_realserver_socket->first;
        down_thread_data_dest_side.set_endpoint(server_endpoint);
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;
        if(!ec){
            if(recv_size > 0){
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] down_thread_realserver_receive";
                    buf << " receive data size[";
                    buf << recv_size;
                    buf << "] from [";
                    buf << server_endpoint;
                    buf << "]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 43, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                down_thread_data_dest_side.set_size(recv_size);
                parent_service.update_down_recv_size(recv_size);
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_realserver_recv(down_thread_id,server_endpoint,data_buff,recv_size);
                std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
                if(unlikely( func_type == down_thread_module_event_map.end() )){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;    
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 70, buf.str(), __FILE__, __LINE__ );
                    down_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }else{
                func_tag = DOWN_FUNC_REALSERVER_RECEIVE;
                boost::this_thread::yield();
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = DOWN_FUNC_REALSERVER_RECEIVE;
                boost::this_thread::yield();
            }else{
                func_tag = DOWN_FUNC_REALSERVER_DISCONNECT;
            }
        }
        down_thread_function_pair    func    = down_thread_function_array[func_tag];
        if(unlikely( !func.second ) ){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 71, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_current_receive_realserver_socket++;
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        if(down_thread_current_receive_realserver_socket == list_end)
            down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
        down_thread_next_call_function = func;
    }
    //! down thread close realserver socket and raise realserver disconnect event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::asio::ip::tcp::endpoint server_endpoint = down_thread_data_dest_side.get_endpoint();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        while(list_end != close_socket){
            boost::asio::ip::tcp::endpoint close_endpoint = close_socket->first;
            if(close_endpoint == server_endpoint){
                boost::system::error_code ec;
                bool bres = close_socket->second->close(ec);
                if(bres){
                    parent_service.connection_inactive(server_endpoint);
                    tcp_thread_message*        up_msg        = new tcp_thread_message;
                    tcp_thread_message*        down_msg    = new tcp_thread_message;
                    down_thread_function_pair    down_func    = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT];
                    if( unlikely( !down_func.second ) ){
                        //Error not find function map
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_REALSERVER_DISCONNECT_EVENT";
                        Logger::putLogError( LOG_CAT_L7VSD_SESSION, 72, buf.str(), __FILE__, __LINE__ );
                        down_thread_exit(process_type);
                        return;
                    }
                    down_msg->endpoint_info = server_endpoint;
                    down_msg->message = down_func.second;
                    std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT_EVENT);
                    if( unlikely( up_func == down_thread_message_up_thread_function_map.end() ) ){
                        //Error not find function map
                        std::stringstream buf;
                        buf << "Thread ID[";
                        buf << boost::this_thread::get_id();
                        buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : UP_FUNC_REALSERVER_DISCONNECT_EVENT";
                        Logger::putLogError( LOG_CAT_L7VSD_SESSION, 73, buf.str(), __FILE__, __LINE__ );
                        down_thread_exit(process_type);
                        return;
                    }
                    up_msg->endpoint_info = server_endpoint;
                    up_msg->message = up_func->second;
                    while(!up_thread_message_que.push(up_msg)){}
                    while(!down_thread_message_que.push(down_msg)){}
                }
                break;
            }
            close_socket++;
        }
    }
    
    //! down thread raise module event of handle_realserver_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        boost::asio::ip::tcp::endpoint server_endpoint = down_thread_message_data.get_endpoint();
        {
            rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
            module_event = protocol_module->handle_realserver_disconnect(down_thread_id,server_endpoint);
        }
        
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        std::list<socket_element>::iterator erase_socket = down_thread_receive_realserver_socket_list.begin();
        while(likely( erase_socket != list_end )){
            if(erase_socket->first == server_endpoint){
                down_thread_receive_realserver_socket_list.erase(erase_socket);
                down_thread_current_receive_realserver_socket = down_thread_receive_realserver_socket_list.begin();
                break;
            }
            erase_socket++;
        }
        
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 74, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 75, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread close realserver socket and raise realserver disconnect event message for up and down thread
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        
        protocol_module_base::EVENT_TAG module_event;
        boost::asio::ip::tcp::endpoint server_endpoint;
        bool realserver_fond = false;
        while(close_socket != list_end){
            realserver_fond = true;
            boost::system::error_code ec;
            bool bres = close_socket->second->close(ec);
            server_endpoint = close_socket->first;
            if(bres)
                parent_service.connection_inactive(server_endpoint);
            {
                rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
                module_event = protocol_module->handle_realserver_disconnect(down_thread_id,server_endpoint);
            }
            
            close_socket++;
        }
        if(!realserver_fond){
            //not fond realserver
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] close realserver not fond";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 44, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            rw_scoped_lock scope_lock(module_function_realserver_disconnect_mutex);
            module_event = protocol_module->handle_realserver_disconnect(down_thread_id,boost::asio::ip::tcp::endpoint());
        }
        down_thread_receive_realserver_socket_list.clear();
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 76, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 77, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread raise module event of handle_client_connection_check
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){
        down_thread_data_client_side.initialize();
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t data_size;
        protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_connection_check(down_thread_id,data_buff,data_size);
        down_thread_data_client_side.set_size(data_size);
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 78, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 79, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    
    //! down thread raise module event of handle_response_send_inform
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_response_send_inform_mutex);
            module_event = protocol_module->handle_response_send_inform(down_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 80, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 81, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread send for client and raise module event of handle_client_send
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){
        
        boost::system::error_code ec;
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_client_side.get_data();
        std::size_t data_size = down_thread_data_client_side.get_size();
        std::size_t send_data_size = down_thread_data_client_side.get_send_size();
        std::size_t send_size;
        if (!ssl_flag) {
            send_size = client_socket.write_some(boost::asio::buffer(data_buff.data()+send_data_size,data_size-send_data_size),ec);
        } else {
            send_size = client_ssl_socket.write_some(boost::asio::buffer(data_buff.data()+send_data_size,data_size-send_data_size),ec);
        }
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;
        if(!ec){
            send_data_size += send_size;
            down_thread_data_client_side.set_send_size(send_data_size);
            parent_service.update_down_send_size(send_size);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                boost::asio::ip::tcp::endpoint client_endpoint;
                if (!ssl_flag) {
                    client_endpoint = client_socket.get_socket().lowest_layer().remote_endpoint(ec);
                } else {
                    client_endpoint = client_ssl_socket.get_socket().lowest_layer().remote_endpoint(ec);
                }
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] down_thread_client_send";
                buf << " send data size[";
                buf << send_size;
                buf << "] for [";
                buf << client_endpoint;
                buf << "]";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 45, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            if(data_size > send_data_size){
                func_tag = DOWN_FUNC_CLIENT_SEND;
            }else{
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_client_send(down_thread_id);
                std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
                if(unlikely( func_type == down_thread_module_event_map.end() ) ){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 82, buf.str(), __FILE__, __LINE__ );
                    down_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = DOWN_FUNC_CLIENT_SEND;
            }else{
                func_tag = DOWN_FUNC_CLIENT_DISCONNECT;
            }
        }
        down_thread_function_pair    func    = down_thread_function_array[func_tag];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 83, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread close client socket and raise client disconnect event message for up and down thread 
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        bool bres;
        if (!ssl_flag) {
            bres = client_socket.close(ec);
        } else {
            bres = client_ssl_socket.close(ec);
        }
        if(bres){
            tcp_thread_message*        up_msg        = new tcp_thread_message;
            tcp_thread_message*        down_msg    = new tcp_thread_message;
            down_thread_function_pair    down_func    = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT];
            if(unlikely( !down_func.second )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_CLIENT_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 84, buf.str(), __FILE__, __LINE__ );
                down_thread_exit(process_type);
                return;
            }
            down_msg->message = down_func.second;
            std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT_EVENT);
            if(unlikely( up_func == down_thread_message_up_thread_function_map.end() )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_CLIENT_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 85, buf.str(), __FILE__, __LINE__ );
                down_thread_exit(process_type);
                return;
            }
            up_msg->message = up_func->second;
            while(!down_thread_message_que.push(down_msg)){}
            while(!up_thread_message_que.push(up_msg)){}
        }
    }
    
    //! down thread raise module event of handle_client_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_client_disconnect_mutex);
            module_event = protocol_module->handle_client_disconnect(down_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end())){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 86, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 87, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
        down_thread_data_dest_side.initialize();
        boost::array<char,MAX_BUFFER_SIZE>& data_buff = down_thread_data_dest_side.get_data();
        boost::system::error_code ec;
        size_t recv_size = sorryserver_socket.second->read_some(boost::asio::buffer(data_buff,MAX_BUFFER_SIZE),ec);
        boost::asio::ip::tcp::endpoint sorry_endpoint = sorryserver_socket.first;
        down_thread_data_dest_side.set_endpoint(sorry_endpoint);
        DOWN_THREAD_FUNC_TYPE_TAG func_tag;
        if(!ec){
            if(recv_size > 0){
                //----Debug log----------------------------------------------------------------------
                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] down_thread_sorryserver_receive";
                    buf << " receive data size[";
                    buf << recv_size;
                    buf << "] from [";
                    buf << sorry_endpoint;
                    buf << "]";
                    Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 46, buf.str(), __FILE__, __LINE__ );
                }
                //----Debug log----------------------------------------------------------------------
                down_thread_data_dest_side.set_size(recv_size);
                protocol_module_base::EVENT_TAG module_event = protocol_module->handle_sorryserver_recv(down_thread_id,sorry_endpoint,data_buff,recv_size);
                std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
                if(unlikely( func_type == down_thread_module_event_map.end() )){
                    //Error unknown protocol_module_base::EVENT_TAG return
                    std::stringstream buf;
                    buf << "Thread ID[";
                    buf << boost::this_thread::get_id();
                    buf << "] protocol_module returnd illegal EVENT_TAG : ";
                    buf << module_event;    
                    Logger::putLogError( LOG_CAT_L7VSD_SESSION, 88, buf.str(), __FILE__, __LINE__ );
                    down_thread_exit(process_type);
                    return;
                }
                func_tag = func_type->second;
            }else{
                func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
                boost::this_thread::yield();
            }
        }else{
            if(ec == boost::asio::error::try_again){
                func_tag = DOWN_FUNC_SORRYSERVER_RECEIVE;
                boost::this_thread::yield();
            }else{
                func_tag = DOWN_FUNC_SORRYSERVER_DISCONNECT;
            }
        }
        down_thread_function_pair    func    = down_thread_function_array[func_tag];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_tag;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 89, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread raise module event of handle_sorryserver_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        boost::system::error_code ec;
        bool bres = sorryserver_socket.second->close(ec);
        if(bres){
            tcp_thread_message*        up_msg        = new tcp_thread_message;
            tcp_thread_message*        down_msg    = new tcp_thread_message;
            down_thread_function_pair    down_func    = down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT];
            if(unlikely( !down_func.second )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 90, buf.str(), __FILE__, __LINE__ );
                down_thread_exit(process_type);
                return;
            }
            down_msg->message = down_func.second;
            down_msg->endpoint_info = sorryserver_socket.first;
            std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator up_func = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
            if(unlikely( up_func == down_thread_message_up_thread_function_map.end() )){
                //Error not find function map
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] not find function map UP_THREAD_FUNC_TYPE_TAG : UP_FUNC_SORRYSERVER_DISCONNECT_EVENT";
                Logger::putLogError( LOG_CAT_L7VSD_SESSION, 91, buf.str(), __FILE__, __LINE__ );
                down_thread_exit(process_type);
                return;
            }
            up_msg->message = up_func->second;
            up_msg->endpoint_info = sorryserver_socket.first;
            while(!up_thread_message_que.push(up_msg)){}
            while(!down_thread_message_que.push(down_msg)){}
        }
    }
    //! down thread close sorryserver socket and raise module sorryserver disconnect event
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;

        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint sorry_endpoint = sorryserver_socket.first;
        bool bres = sorryserver_socket.second->close(ec);
        if(!bres){
            sorryserver_socket.first = boost::asio::ip::tcp::endpoint();
        }
        {
            rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
            module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id,sorry_endpoint);
        }

        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 92, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 93, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread raise module event of handle_sorryserver_disconnect
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        boost::asio::ip::tcp::endpoint sorry_endpoint = down_thread_message_data.get_endpoint();
        {
            rw_scoped_lock scope_lock(module_function_sorryserver_disconnect_mutex);
            module_event = protocol_module->handle_sorryserver_disconnect(down_thread_id,sorry_endpoint);
        }
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() ) ){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 94, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 95, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread raise module event of handle_sorry_enable
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_sorry_enable_mutex);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] handle_sorry_enable call";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 47, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            module_event = protocol_module->handle_sorry_enable(down_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 96, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 97, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread raise module event of handle_sorry_disable
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
        protocol_module_base::EVENT_TAG module_event;
        {
            rw_scoped_lock scope_lock(module_function_sorry_disable_mutex);
            //----Debug log----------------------------------------------------------------------
            if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION))){
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] handle_sorry_disable call";
                Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 48, buf.str(), __FILE__, __LINE__ );
            }
            //----Debug log----------------------------------------------------------------------
            module_event = protocol_module->handle_sorry_disable(down_thread_id);
        }
        std::map< protocol_module_base::EVENT_TAG ,DOWN_THREAD_FUNC_TYPE_TAG >::iterator func_type = down_thread_module_event_map.find(module_event);
        if(unlikely( func_type == down_thread_module_event_map.end() )){
            //Error unknown protocol_module_base::EVENT_TAG return
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] protocol_module returnd illegal EVENT_TAG : ";
            buf << module_event;    
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 98, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_function_pair    func    = down_thread_function_array[func_type->second];
        if(unlikely( !func.second )){
            //Error not find function map
            std::stringstream buf;
            buf << "Thread ID[";
            buf << boost::this_thread::get_id();
            buf << "] not find function map DOWN_THREAD_FUNC_TYPE_TAG : ";
            buf << func_type->second;
            Logger::putLogError( LOG_CAT_L7VSD_SESSION, 99, buf.str(), __FILE__, __LINE__ );
            down_thread_exit(process_type);
            return;
        }
        down_thread_next_call_function = func;
    }
    //! down thread exit main loop
    //! @param[in]        process_type is prosecess type
    void tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
        rw_scoped_lock scoped_lock(exit_flag_update_mutex);
        exit_flag = true;
    }
    //! down thread close all socket
    void tcp_session::down_thread_all_socket_close(void){
        std::list<socket_element>::iterator close_socket = down_thread_receive_realserver_socket_list.begin();
        std::list<socket_element>::iterator list_end = down_thread_receive_realserver_socket_list.end();
        boost::system::error_code ec;
        while(close_socket != list_end){
            bool bres = close_socket->second->close(ec);
            if(bres)
                parent_service.connection_inactive(close_socket->first);
            close_socket++;
        }
        down_thread_receive_realserver_socket_list.clear();
        if (!ssl_flag) {
            client_socket.close(ec);
        } else {
            client_ssl_socket.close(ec);
        }
        sorryserver_socket.second->close(ec);
    }
    
}// namespace l7vs
