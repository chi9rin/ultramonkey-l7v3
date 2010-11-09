/*!
 *    @file    tcp_session.h
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

#ifndef TCP_SESSION_H
#define TCP_SESSION_H

#include <boost/thread/condition.hpp>
#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <sys/epoll.h>

#include "wrlock.h"
#include "protocol_module_base.h"
#include "session_result_message.h"
#include "tcp_socket.h"
#include "tcp_ssl_socket.h"
#include "tcp_realserver_connect_socket_list.h"
#include "tcp_data.h"
#include "lockfree_queue.h"
#include "virtualservice_element.h"

#define TCP_SESSION_THREAD_STATE_BIT    8
#define PARAM_UP_BUFFER_SIZE            "upstream_buffer_size"
#define PARAM_DOWN_BUFFER_SIZE          "downstream_buffer_size"
#define PARAM_EPOLL_TRIGGER             "epoll_trigger"
#define PARAM_EPOLL_TIMEOUT             "epoll_timeout"
#define EVENT_NUM                       2

namespace l7vs
{

//! @class    virtualservice_tcp
//! @brief    this class is parent virtualservice class.
class virtualservice_tcp;

//! @class    tcp_thread_message
//! @brief    tcp_thread_message class name define.
class tcp_thread_message;

//! @class    logger_implement_access
//! @brief    logger_implement_access class name define.
class logger_implement_access;

//! @class    tcp_session
//! @brief    this class is tcp session class.
class tcp_session : private boost::noncopyable
{
public:
        //! process type
        enum TCP_PROCESS_TYPE_TAG {
                LOCAL_PROC = 0,
                MESSAGE_PROC
        };
        //! tcp virtualservice message type
        enum TCP_VIRTUAL_SERVICE_MESSAGE_TAG {
                SORRY_STATE_ENABLE = 0,
                SORRY_STATE_DISABLE,
                SESSION_END,
                SESSION_PAUSE_ON,
                SESSION_PAUSE_OFF,
                ACCESS_LOG_ON,
                ACCESS_LOG_OFF
        };

        //! constructor
        //! @param[in/out]    vs is parent virtualservice object
        //! @param[in/out]    io is session use io service object
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
        tcp_session(virtualservice_tcp &vs,
                    boost::asio::io_service &session_io,
                    const tcp_socket_option_info set_option,
                    const boost::asio::ip::tcp::endpoint listen_endpoint,
                    const bool ssl_mode,
                    boost::asio::ssl::context &set_ssl_context,
                    const bool set_ssl_cache_flag,
                    const int set_ssl_handshake_time_out,
                    logger_implement_access *set_access_logger);

        //! destructor
        virtual ~tcp_session();
        //! initialize
        session_result_message initialize();
        //! get reference client side socket
        //! @return            reference client side socket
        boost::asio::ip::tcp::socket &get_client_socket();
        //! get reference client side ssl socket
        //! @return            reference client side ssl socket
        ssl_socket &get_client_ssl_socket();
        //! message from parent virtualservice
        //! @param[in]        message is tcp virtualservice message type
        void set_virtual_service_message(
                const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message);
        //! up stream thread main function
        void up_thread_run();
        //! down stream thread main function
        void down_thread_run();

protected:
        typedef boost::asio::ip::tcp::endpoint endpoint;
        typedef boost::shared_ptr< tcp_socket > tcp_socket_ptr;
        typedef std::pair< endpoint, tcp_socket_ptr> socket_element;
        typedef boost::function< void(TCP_PROCESS_TYPE_TAG) > tcp_session_func;
        //! up thread call function type
        enum UP_THREAD_FUNC_TYPE_TAG {
                UP_FUNC_CLIENT_ACCEPT = 0,                  //! up_thread_client_accept function
                UP_FUNC_CLIENT_ACCEPT_EVENT,                //! up_thread_client_accept_event function
                UP_FUNC_CLIENT_DISCONNECT,                  //! up_thread_client_disconnect function
                UP_FUNC_CLIENT_DISCONNECT_EVENT,            //! up_thread_client_disconnect_event function
                UP_FUNC_CLIENT_RECEIVE,                     //! up_thread_client_receive function
                UP_FUNC_CLIENT_RESPOND_SEND,                //! up_thread_client_respond function
                UP_FUNC_CLIENT_RESPOND_SEND_EVENT,          //! up_thread_client_respond_event function
                UP_FUNC_REALSERVER_GET_DEST_EVENT,          //! up_thread_realserver_get_destination_event function
                UP_FUNC_REALSERVER_CONNECT,                 //! up_thread_realserver_connect function
                UP_FUNC_REALSERVER_CONNECT_EVENT,           //! up_thread_realserver_connect_event function
                UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT,      //! up_thread_realserver_connection_fail_event function
                UP_FUNC_REALSERVER_SEND,                    //! up_thread_realserver_send function
                UP_FUNC_REALSERVER_DISCONNECT,              //! up_thread_realserver_disconnect function
                UP_FUNC_REALSERVER_DISCONNECT_EVENT,        //! up_thread_realserver_disconnect_event function
                UP_FUNC_REALSERVER_ALL_DISCONNECT,          //! up_thread_all_realserver_disconnect function
                UP_FUNC_SORRYSERVER_GET_DEST,               //! up_thread_sorryserver_get_destination_event function
                UP_FUNC_SORRYSERVER_CONNECT,                //! up_thread_sorryserver_connect function
                UP_FUNC_SORRYSERVER_CONNECT_EVENT,          //! up_thread_sorryserver_connect_event function
                UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT,     //! up_thread_sorryserver_connection_fail_event function
                UP_FUNC_SORRYSERVER_SEND,                   //! up_thread_sorryserver_send function
                UP_FUNC_SORRYSERVER_DISCONNECT,             //! up_thread_sorryserver_disconnect function
                UP_FUNC_SORRYSERVER_MOD_DISCONNECT,         //! up_thread_sorryserver_mod_disconnect function
                UP_FUNC_SORRYSERVER_DISCONNECT_EVENT,       //! up_thread_sorryserver_disconnect_event function
                UP_FUNC_SORRY_ENABLE_EVENT,                 //! up_thread_sorry_enable_event function
                UP_FUNC_SORRY_DISABLE_EVENT,                //! up_thread_sorry_disable_event function
                UP_FUNC_EXIT                                //! up_thread_exit function
        };
        //! down thread call function type
        enum DOWN_THREAD_FUNC_TYPE_TAG {
                DOWN_FUNC_CLIENT_DISCONNECT = 0,            //! down_thread_client_disconnect function
                DOWN_FUNC_CLIENT_DISCONNECT_EVENT,          //! down_thread_client_disconnect_event function
                DOWN_FUNC_CLIENT_CONNECTION_CHK,            //! down_thread_client_connection_chk_event function
                DOWN_FUNC_CLIENT_SEND,                      //! down_thread_client_send function
                DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT,        //! down_thread_client_respond_event function
                DOWN_FUNC_REALSERVER_RECEIVE,               //! down_thread_realserver_receive function
                DOWN_FUNC_REALSERVER_DISCONNECT,            //! down_thread_realserver_disconnect function
                DOWN_FUNC_REALSERVER_DISCONNECT_EVENT,      //! down_thread_realserver_disconnect_event function
                DOWN_FUNC_REALSERVER_ALL_DISCONNECT,        //! down_thread_all_realserver_disconnect function
                DOWN_FUNC_SORRYSERVER_RECEIVE,              //! down_thread_sorryserver_receive function
                DOWN_FUNC_SORRYSERVER_DISCONNECT,           //! down_thread_sorryserver_disconnect function
                DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT,       //! down_thread_sorryserver_mod_disconnect function
                DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT,     //! down_thread_sorryserver_disconnect_event function
                DOWN_FUNC_SORRY_ENABLE_EVENT,               //! down_thread_sorry_enable_event function
                DOWN_FUNC_SORRY_DISABLE_EVENT,              //! down_thread_sorry_disable_event function
                DOWN_FUNC_EXIT                              //! down_thread_exit function
        };

        typedef std::pair< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >
        up_thread_function_pair;
        typedef std::pair< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >
        down_thread_function_pair;
        //! tcp_session use io service
        boost::asio::io_service &io;
        //! parent virtualservice
        virtualservice_tcp &parent_service;
        //! thread main loop exit flag
        volatile int exit_flag;
        //! thread main loop exit flag update mutex
        wr_mutex exit_flag_update_mutex;

        //! upthread_status_tag
        enum UPTHREAD_STATUS_TAG {
                UPTHREAD_SLEEP = 0,
                UPTHREAD_ALIVE,
                UPTHREAD_ACTIVE,
                UPTHREAD_LOCK,
                UPTHREAD_COMPLETE
        };
        //! downthread_status tag
        enum DOWNTHREAD_STATUS_TAG {
                DOWNTHREAD_SLEEP = 0,
                DOWNTHREAD_ALIVE,
                DOWNTHREAD_ACTIVE,
                DOWNTHREAD_LOCK,
                DOWNTHREAD_COMPLETE
        };
        UPTHREAD_STATUS_TAG     upthread_status;
        boost::mutex            upthread_status_mutex;
        boost::condition        upthread_status_cond;
        DOWNTHREAD_STATUS_TAG   downthread_status;
        boost::mutex            downthread_status_mutex;
        boost::condition        downthread_status_cond;
        bool                    realserver_connect_status;
        boost::mutex            realserver_connect_mutex;
        boost::condition        realserver_connect_cond;

        //! up thread id
        boost::thread::id up_thread_id;
        //! down thread id
        boost::thread::id down_thread_id;
        //! pointer of protocol module
        protocol_module_base *protocol_module;
        //! client socket
        tcp_socket client_socket;
        //! sorryserver socket
        socket_element sorryserver_socket;
        //! up thread use realserver socket map
        std::map<endpoint, tcp_socket_ptr>
        up_thread_send_realserver_socket_map;
        //! connect realserver list
        tcp_realserver_connect_socket_list down_thread_connect_socket_list;
        //! down thread use realserver socket list
        std::list<socket_element>
        down_thread_receive_realserver_socket_list;
        //! down thread next receive realserver
        std::list<socket_element>::iterator
        down_thread_current_receive_realserver_socket;
        //! module function mutex (handle_response_send_inform)
        wr_mutex module_function_response_send_inform_mutex;
        //! module function mutex (handle_client_disconnect)
        wr_mutex module_function_client_disconnect_mutex;
        //! module function mutex (handle_realserver_disconnect)
        wr_mutex module_function_realserver_disconnect_mutex;
        //! module function mutex (handle_sorryserver_disconnect)
        wr_mutex module_function_sorryserver_disconnect_mutex;
        //! module function mutex (handle_sorry_enable)
        wr_mutex module_function_sorry_enable_mutex;
        //! module function mutex (handle_sorry_disable)
        wr_mutex module_function_sorry_disable_mutex;
        //! up thread call function object array
        up_thread_function_pair up_thread_function_array[UP_FUNC_EXIT+1];
        //! up thread recept module event convert to up thread function type map
        std::map < protocol_module_base::EVENT_TAG ,
            UP_THREAD_FUNC_TYPE_TAG > up_thread_module_event_map;
        //! up thread raise message map for down thread
        std::map < DOWN_THREAD_FUNC_TYPE_TAG,
            tcp_session_func > up_thread_message_down_thread_function_map;
        //! up thread next call function object
        up_thread_function_pair up_thread_next_call_function;
        //! up thread use client side buffer
        tcp_data up_thread_data_client_side;
        //! up thread use destination side buffer
        tcp_data up_thread_data_dest_side;
        //! message data queue for up thread
        lockfree_queue< tcp_thread_message > up_thread_message_que;
        //! up thread use message data
        tcp_data up_thread_message_data;
        //! down thread call function object array
        down_thread_function_pair
        down_thread_function_array[DOWN_FUNC_EXIT+1];
        //! down thread recept module event convert to down thread function type map
        std::map < protocol_module_base::EVENT_TAG ,
            DOWN_THREAD_FUNC_TYPE_TAG > down_thread_module_event_map;
        //! down thread raise message map for up thread
        std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >
        down_thread_message_up_thread_function_map;
        //! down thread next call function object
        down_thread_function_pair down_thread_next_call_function;
        //! down thread use client side buffer
        tcp_data down_thread_data_client_side;
        //! down thread use destination side buffer
        tcp_data down_thread_data_dest_side;
        //! message data queue for down thread
        lockfree_queue< tcp_thread_message > down_thread_message_que;
        //! up thread use message data
        tcp_data down_thread_message_data;
        //! upstream socket buffer size
        int upstream_buffer_size;
        //! downstream socket buffer size
        int downstream_buffer_size;
        //! virtual service message convert to up thread function object map
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>
        virtual_service_message_up_thread_function_map;
        //! virtual service message convert to down thread function object map
        std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>
        virtual_service_message_down_thread_function_map;
        //! client connection src endpoint
        boost::asio::ip::tcp::endpoint client_endpoint;
        //! virtualservice accept endpoint
        boost::asio::ip::tcp::endpoint virtualservice_endpoint;
        //! access log out put flag
        bool access_log_flag;
        //! access log out put flag mutex
        wr_mutex access_log_flag_mutex;
        //! access logger
        logger_implement_access *access_logger;
        //! ssl mode flag
        bool ssl_flag;
        //! client ssl socket
        tcp_ssl_socket client_ssl_socket;
        //! ssl context
        boost::asio::ssl::context &ssl_context;
        //! ssl session cache flag
        bool ssl_cache_flag;
        //! ssl handshake timer
        typedef boost::shared_ptr<boost::asio::deadline_timer>
        deadline_timer_ptr_type;
        deadline_timer_ptr_type ssl_handshake_timer;
        //! ssl handshake timer flag
        bool ssl_handshake_timer_flag;
        //! ssl handshake timeout
        int ssl_handshake_time_out;
        //! ssl handshake timeout flag
        bool ssl_handshake_time_out_flag;
        //! ssl handshake timeout flag mutex
        wr_mutex ssl_handshake_time_out_flag_mutex;
        //! socket option
        tcp_socket_option_info socket_opt_info;

        // epoll using member
        struct epoll_event  up_client_events[EVENT_NUM];
        struct epoll_event  up_realserver_events[EVENT_NUM];
        struct epoll_event  up_sorryserver_events[EVENT_NUM];
        struct epoll_event  down_client_events[EVENT_NUM];
        struct epoll_event  down_realserver_events[EVENT_NUM];
        struct epoll_event  down_sorryserver_events[EVENT_NUM];
        int    up_client_epollfd;
        int    up_realserver_epollfd;
        int    up_sorryserver_epollfd;
        int    down_client_epollfd;
        int    down_realserver_epollfd;
        int    down_sorryserver_epollfd;
        bool   up_client_epollfd_registered;
        bool   up_realserver_epollfd_registered;
        bool   up_sorryserver_epollfd_registered;
        bool   down_client_epollfd_registered;
        bool   down_realserver_epollfd_registered;
        bool   down_sorryserver_epollfd_registered;
        bool   is_epoll_edge_trigger;
        int    epoll_timeout;

        //! handshake timer handler
        //! @param[in]        error is error code object
        virtual void handle_ssl_handshake_timer(
                const boost::system::error_code &error);
        //! reset ssl object for reuse
        //! @param[in/out]    clear_ssl is clear target SSL structure pointer
        virtual bool ssl_clear_keep_cache(SSL *clear_ssl);
        //! endpoint data to string information
        //! @param[in]        endpoint is target endpoint object
        virtual std::string endpoint_to_string(
                const boost::asio::ip::tcp::endpoint &target_endpoint);

        //! up thread accept client side
        //! @param[in]        process_type is process type
        virtual void up_thread_client_accept(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_accept
        //! @param[in]        process_type is process type
        virtual void up_thread_client_accept_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread receive client side and raise module event of handle_client_recv
        //! @param[in]        process_type is process type
        virtual void up_thread_client_receive(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise client respond send event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void up_thread_client_respond(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_response_send_inform
        //! @param[in]        process_type is process type
        virtual void up_thread_client_respond_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close client socket and raise client disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void up_thread_client_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_client_disconnect
        //! @param[in]        process_type is process type
        virtual void up_thread_client_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread send realserver and raise module event of handle_client_recv
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_send(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_realserver_select
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_get_destination_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread connect realserver
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_connect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_realserver_connect
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_connect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_realserver_connection_fail
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_connection_fail_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close realserver socket and raise realserver disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_realserver_disconnect
        //! @param[in]        process_type is process type
        virtual void up_thread_realserver_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close all realserver socket and raise module event of handle_realserver_disconnect
        //! @param[in]        process_type is process type
        virtual void up_thread_all_realserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread send sorryserver and raise module event of handle_sorryserver_send
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_send(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorryserver_select
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_get_destination_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread connect sorryserver
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_connect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorryserver_connect
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_connect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorryserver_connection_fail
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_connection_fail_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close sorryserver socket and raise module sorryserver disconnect event
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_mod_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorryserver_disconnect
        //! @param[in]        process_type is process type
        virtual void up_thread_sorryserver_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorry_enable
        //! @param[in]        process_type is process type
        virtual void up_thread_sorry_enable_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread raise module event of handle_sorry_disable
        //! @param[in]        process_type is process type
        virtual void up_thread_sorry_disable_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread exit main loop
        //! @param[in]        process_type is process type
        virtual void up_thread_exit(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! up thread close all socket
        virtual void up_thread_all_socket_close(void);

        //! down thread receive from realserver and raise module event of handle_realserver_recv
        //! @param[in]        process_type is process type
        virtual void down_thread_realserver_receive(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close realserver socket and raise realserver disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void down_thread_realserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_realserver_disconnect
        //! @param[in]        process_type is process type
        virtual void down_thread_realserver_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close all realserver socket and raise module event of handle_realserver_disconnect
        //! @param[in]        process_type is process type
        virtual void down_thread_all_realserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_client_connection_check
        //! @param[in]        process_type is process type
        virtual void down_thread_client_connection_chk_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_response_send_inform
        //! @param[in]        process_type is process type
        virtual void down_thread_client_respond_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread send for client and raise module event of handle_client_send
        //! @param[in]        process_type is process type
        virtual void down_thread_client_send(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close client socket and raise client disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void down_thread_client_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_client_disconnect
        //! @param[in]        process_type is process type
        virtual void down_thread_client_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
        //! @param[in]        process_type is process type
        virtual void down_thread_sorryserver_receive(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
        //! @param[in]        process_type is process type
        virtual void down_thread_sorryserver_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close sorryserver socket and raise module sorryserver disconnect event
        //! @param[in]        process_type is process type
        virtual void down_thread_sorryserver_mod_disconnect(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_sorryserver_disconnect
        //! @param[in]        process_type is process type
        virtual void down_thread_sorryserver_disconnect_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_sorry_enable
        //! @param[in]        process_type is process type
        virtual void down_thread_sorry_enable_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread raise module event of handle_sorry_disable
        //! @param[in]        process_type is process type
        virtual void down_thread_sorry_disable_event(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread exit main loop
        //! @param[in]        process_type is process type
        virtual void down_thread_exit(
                const TCP_PROCESS_TYPE_TAG process_type);
        //! down thread close all socket
        virtual void down_thread_all_socket_close(void);

        //! millisecond to boost::xtime converter
        virtual void to_time(int in, boost::xtime &xt);
        //! using xtime
        boost::xtime xt;
        static const int LOCKTIMEOUT = 50;
        //! functag to string
        std::string func_tag_to_string(UP_THREAD_FUNC_TYPE_TAG in) {
                return 
                        in == UP_FUNC_CLIENT_ACCEPT                  ? "UP_FUNC_CLIENT_ACCEPT"
                      : in == UP_FUNC_CLIENT_ACCEPT_EVENT            ? "UP_FUNC_CLIENT_ACCEPT_EVENT"
                      : in == UP_FUNC_CLIENT_DISCONNECT              ? "UP_FUNC_CLIENT_DISCONNECT"
                      : in == UP_FUNC_CLIENT_DISCONNECT_EVENT        ? "UP_FUNC_CLIENT_DISCONNECT_EVENT"
                      : in == UP_FUNC_CLIENT_RECEIVE                 ? "UP_FUNC_CLIENT_RECEIVE"
                      : in == UP_FUNC_CLIENT_RESPOND_SEND            ? "UP_FUNC_CLIENT_RESPOND_SEND"
                      : in == UP_FUNC_CLIENT_RESPOND_SEND_EVENT      ? "UP_FUNC_CLIENT_RESPOND_SEND_EVENT"
                      : in == UP_FUNC_REALSERVER_GET_DEST_EVENT      ? "UP_FUNC_REALSERVER_GET_DEST_EVENT"
                      : in == UP_FUNC_REALSERVER_CONNECT             ? "UP_FUNC_REALSERVER_CONNECT"
                      : in == UP_FUNC_REALSERVER_CONNECT_EVENT       ? "UP_FUNC_REALSERVER_CONNECT_EVENT"
                      : in == UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT  ? "UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT"
                      : in == UP_FUNC_REALSERVER_SEND                ? "UP_FUNC_REALSERVER_SEND"
                      : in == UP_FUNC_REALSERVER_DISCONNECT          ? "UP_FUNC_REALSERVER_DISCONNECT"
                      : in == UP_FUNC_REALSERVER_DISCONNECT_EVENT    ? "UP_FUNC_REALSERVER_DISCONNECT_EVENT"
                      : in == UP_FUNC_REALSERVER_ALL_DISCONNECT      ? "UP_FUNC_REALSERVER_ALL_DISCONNECT"
                      : in == UP_FUNC_SORRYSERVER_GET_DEST           ? "UP_FUNC_SORRYSERVER_GET_DEST"
                      : in == UP_FUNC_SORRYSERVER_CONNECT            ? "UP_FUNC_SORRYSERVER_CONNECT"
                      : in == UP_FUNC_SORRYSERVER_CONNECT_EVENT      ? "UP_FUNC_SORRYSERVER_CONNECT_EVENT"
                      : in == UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT ? "UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT"
                      : in == UP_FUNC_SORRYSERVER_SEND               ? "UP_FUNC_SORRYSERVER_SEND"
                      : in == UP_FUNC_SORRYSERVER_DISCONNECT         ? "UP_FUNC_SORRYSERVER_DISCONNECT"
                      : in == UP_FUNC_SORRYSERVER_MOD_DISCONNECT     ? "UP_FUNC_SORRYSERVER_MOD_DISCONNECT"
                      : in == UP_FUNC_SORRYSERVER_DISCONNECT_EVENT   ? "UP_FUNC_SORRYSERVER_DISCONNECT_EVENT"
                      : in == UP_FUNC_SORRY_ENABLE_EVENT             ? "UP_FUNC_SORRY_ENABLE_EVENT"
                      : in == UP_FUNC_SORRY_DISABLE_EVENT            ?  "UP_FUNC_SORRY_DISABLE_EVENT"
                      : in == UP_FUNC_EXIT                           ? "UP_FUNC_EXIT"
                      :                                                "NOT_FOUND"
                      ;
        }
        std::string func_tag_to_string(DOWN_THREAD_FUNC_TYPE_TAG in) {
                return
                        in == DOWN_FUNC_CLIENT_DISCONNECT            ? "DOWN_FUNC_CLIENT_DISCONNECT"
                      : in == DOWN_FUNC_CLIENT_DISCONNECT_EVENT      ? "DOWN_FUNC_CLIENT_DISCONNECT_EVENT"
                      : in == DOWN_FUNC_CLIENT_CONNECTION_CHK        ? "DOWN_FUNC_CLIENT_CONNECTION_CHK"
                      : in == DOWN_FUNC_CLIENT_SEND                  ? "DOWN_FUNC_CLIENT_SEND"
                      : in == DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT    ? "DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT"
                      : in == DOWN_FUNC_REALSERVER_RECEIVE           ? "DOWN_FUNC_REALSERVER_RECEIVE"
                      : in == DOWN_FUNC_REALSERVER_DISCONNECT        ? "DOWN_FUNC_REALSERVER_DISCONNECT"
                      : in == DOWN_FUNC_REALSERVER_DISCONNECT_EVENT  ? "DOWN_FUNC_REALSERVER_DISCONNECT_EVENT"
                      : in == DOWN_FUNC_REALSERVER_ALL_DISCONNECT    ? "DOWN_FUNC_REALSERVER_ALL_DISCONNECT"
                      : in == DOWN_FUNC_SORRYSERVER_RECEIVE          ? "DOWN_FUNC_SORRYSERVER_RECEIVE"
                      : in == DOWN_FUNC_SORRYSERVER_DISCONNECT       ? "DOWN_FUNC_SORRYSERVER_DISCONNECT"
                      : in == DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT   ? "DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT"
                      : in == DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT ? "DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT"
                      : in == DOWN_FUNC_SORRY_ENABLE_EVENT           ? "DOWN_FUNC_SORRY_ENABLE_EVENT"
                      : in == DOWN_FUNC_SORRY_DISABLE_EVENT          ? "DOWN_FUNC_SORRY_DISABLE_EVENT"
                      : in == DOWN_FUNC_EXIT                         ? "DOWN_FUNC_EXIT"
                      :                                                "NOT_FOUND"
                      ;
        }

};// class tcp_session
}// namespace l7vs

#endif//TCP_SESSION_H


