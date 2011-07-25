/*
 *    @file    protocol_module_base.h
 *    @brief    shared object protocol module abstract class
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
#ifndef    PROTOCOL_MODULE_BASE_H
#define    PROTOCOL_MODULE_BASE_H

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "logger_enum.h"
#include "module_base.h"
#include "realserver.h"
#include "atomic.h"
#include <boost/format.hpp>

#define VS_CONTACT_CLASS_NORMAL (0)
#define VS_CONTACT_CLASS_SSL (0x00000001)

namespace l7vs
{

//!    @class    stats_base
//! @brief    this class is base class of all statistic information classes.
class stats_base
{
public:
        //! @enum    STATS_MODE_TAG
        //! @brief    statistic mode type.
        enum STATS_MODE_TAG {
          MODE_BASE = 0,
          MODE_HTTP
        };

        //!    constructor
        stats_base() : mode(MODE_BASE) {};

        //!    constructor
        stats_base(STATS_MODE_TAG m) : mode(m) {};

        //! destructor
        ~stats_base() {};

        //! get statistic mode
        //! @return    statistic mode.
        STATS_MODE_TAG get_mode() const {
                return mode;
        };
protected:
        //! statistic mode
        STATS_MODE_TAG mode;
};

//!    @class    stats_base
//! @brief    this class is a class of http statistic information classes.
class http_stats : public stats_base
{
public:
        //!    constructor
        http_stats() : stats_base(MODE_HTTP) {
                http_requests = 0;
                http_get_requests = 0;
                http_post_requests = 0;
        };

        //! destructor
        ~http_stats(){};

        //! http request counts
        atomic<unsigned long long> http_requests;

        //! http get request counts
        atomic<unsigned long long> http_get_requests;

        //! http post request counts
        atomic<unsigned long long> http_post_requests;
};

//!    @class    protocol_module_base
//! @brief    this class is base class of all protocol module classes.
class protocol_module_base : public module_base
{
public:
        //!    realserver list type
        typedef    std::list<realserver>    realserverlist_type;
        //! realserver iterator function object type
        typedef    boost::function< realserverlist_type::iterator(void) >
        rs_list_itr_func_type;
        typedef    boost::function< realserverlist_type::iterator(realserverlist_type::iterator) >
        rs_list_itr_next_func_type;
        //! TCP/IP scheduler function object type.
        typedef    boost::function < void (const boost::thread::id,
                                           rs_list_itr_func_type,
                                           rs_list_itr_func_type,
                                           rs_list_itr_next_func_type,
                                           boost::asio::ip::tcp::endpoint &) >
        tcp_schedule_func_type;
        //! UDP scheduler function object type.
        typedef    boost::function < void (const boost::thread::id,
                                           rs_list_itr_func_type,
                                           rs_list_itr_func_type,
                                           rs_list_itr_next_func_type,
                                           boost::asio::ip::udp::endpoint &) >
        udp_schedule_func_type;

        //! @enum    EVENT_TAG
        //! @brief    event tag is return to session.
        enum    EVENT_TAG {
                //use in upstream_thread
                INITIALIZE = 0,                //!< Do Initialize
                ACCEPT,                        //!< Accept to Client
                CLIENT_RECV,                //!< Receive from Client
                REALSERVER_SELECT,            //!< Select RealServer
                REALSERVER_CONNECT,            //!< Connect to RealServer
                REALSERVER_SEND,            //!< Send message to RealServer
                SORRYSERVER_SELECT,            //!< Select SorryServer
                SORRYSERVER_CONNECT,        //!< Connect to SorryServer
                SORRYSERVER_SEND,            //!< Send message to SorryServer
                //use in downstream_thread
                REALSERVER_RECV,            //!< Receive from RealServer
                SORRYSERVER_RECV,            //!< Receive from SorryServer
                CLIENT_SELECT,                //!< Select Client
                CLIENT_CONNECTION_CHECK,    //!< Check ClientConnection available
                CLIENT_SEND,                //!< Send message to Client
                //COMMON Status
                CLIENT_RESPONSE_SEND,        //!< Send UML7response message to Client
                REALSERVER_DISCONNECT,        //!< Disconnect RealServerConnection
                SORRYSERVER_DISCONNECT,        //!< Disconnect SorryServerConnection
                CLIENT_DISCONNECT,            //!< Disconnect ClientConnection
                REALSERVER_CLOSE,            //!< Close socket(RealServer side)
                FINALIZE,                    //!< Do finalize
                STOP                        //!< Stop SessionThread
        };

        //! @struct check_message_result
        //!    @brief    this class is POD. this class used then return multi value
        struct check_message_result {
                bool        flag;        //! return flag
                std::string    message;    //! return message
                //!    == operator
                //! @param[in]    check_message_result& is equal check object.
                //! @return true is equal
                //! @return false is not equal
                bool        operator==(const check_message_result &in) {
                        return ((flag == in.flag) && (message == in.message));
                }
                //! @param[in]    check_message_result& is not equal check object.
                //! @return true is not equal
                //! @return false is equal
                bool        operator!=(const check_message_result &in) {
                        return ((flag != in.flag) || (message != in.message));
                }
                //! constructor
                check_message_result() : flag(false) {}
        };
protected:
        //! realserver list iterator begin function object
        rs_list_itr_func_type    rs_list_begin;
        //! realserver list iterator end function object
        rs_list_itr_func_type    rs_list_end;
        //! realserver list iterator next function object
        rs_list_itr_next_func_type
        rs_list_next;

        //! realserver list lock function object
        boost::function< void(void) >    rs_list_lock;
        //! realserver list unlock function object
        boost::function< void(void) >    rs_list_unlock;

        tcp_schedule_func_type        schedule_tcp;    //!< tcp_scheduler_function object
        udp_schedule_func_type        schedule_udp;    //!< udp_scheduler function object

        stats_base                    stats;           //!< base statistic object
        bool                           statistic;       //!< collect statistic flag

public:
        //! constructor
        protocol_module_base(std::string in_modulename) : module_base(in_modulename), statistic(false) {};
        //! destructor
        virtual    ~protocol_module_base() {};
        //! initialize function. called from module control. module loaded call
        //! @param[in]    realserver list iterator begin function object type
        //!    @param[in]    realserver list iterator end function object type
        //! @param[in]    realserver list iterator next function object type
        //! @param[in]    realserver list mutex lock function object type.
        //! @param[in]    realserver list mutex unlock function object type
        virtual    void    initialize(
                rs_list_itr_func_type    inlist_begin,
                rs_list_itr_func_type    inlist_end,
                rs_list_itr_next_func_type    inlist_next,
                boost::function< void(void) >    inlist_lock,
                boost::function< void(void) >    inlist_unlock) = 0;

        //! finalize called from module control. module unloaded call.
        virtual    void    finalize() = 0;

        //! sorry support check
        //! @return true sorry mode is supported.
        //! @return false sorry mode is unsupported.
        virtual    bool    is_use_sorry() = 0;

        //! realserver list update event
        virtual    void    handle_rslist_update() = 0;


        //! module parameter check.used by l7vsadm
        //! @param[in]    module parameter string list
        //! @return    result.flag true is parameter is no problem.
        //! @return result.flag false is parameter is problem.
        virtual    check_message_result    check_parameter(const std::vector<std::string>& args) = 0;
        //! parameter set
        //! @param[in] module parameter string list
        //! @return    result.flag true is parameter is no problem.
        //! @return result.flag false is parameter is problem.
        virtual    check_message_result    set_parameter(const std::vector<std::string>& args) = 0;
        //! parameter add
        //! @param[in] module parameter string list
        //! @return    result.flag true is parameter is no problem.
        //! @return result.flag false is parameter is problem.
        virtual    check_message_result    add_parameter(const std::vector<std::string>& args) = 0;
        //! get option info
        //! @param[out] module parameter string
        virtual void                    get_option_info(std::string &option) = 0;
        //! TCP/IP scheduled function registration.
        //! @param[in] schedule module TCP/IP scheduled function object type
        virtual    void    register_schedule(tcp_schedule_func_type inschedule) = 0;
        //! UDP scheduled function registration
        //! @param[in] schedule module UDP scheduled function object type
        virtual    void    register_schedule(udp_schedule_func_type inschedule) = 0;

        //! called from session initialize use in upstream_thread
        //! @param[in]    upstream thread id.
        //! @param[in]    downstream thread id
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_session_initialize(
                const boost::thread::id up_thread_id,
                const boost::thread::id down_thread_id,
                const boost::asio::ip::tcp::endpoint &client_endpoint_tcp,
                const boost::asio::ip::udp::endpoint &client_endpoint_udp) = 0;

        //! called from session finalize use in upstream thread.
        //! @param[in]    upstream thread id.
        //! @param[in]    downstream thread id
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_session_finalize(
                const boost::thread::id up_thread_id,
                const boost::thread::id down_thread_id) = 0;

        //! called from after session accept.in client socket use in upstream thread.
        //! @param[in]    upstream thread id.
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_accept(const boost::thread::id thread_id) = 0;

        //! called from after session recv in client socket. use in upstream thread.
        //! @param[in]    upstream thread id
        //! @param[in]    receive buffer reference.
        //! @param[in]    receive length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_client_recv(
                const boost::thread::id thread_id,
                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen) = 0;

        //! called from after realserver select.use in upstream thread.
        //! @param[in]    upstream thread id
        //! @param[out]    realserver TCP endpoint
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_select(
                const boost::thread::id thread_id,
                boost::asio::ip::tcp::endpoint &rs_endpoint) = 0;

        //! called from after realserver select
        //! @param[in]    upstream thread id
        //! @param[out]    realserver UDP endpoint
        //! @param[out]    send buffer reference
        //! @param[out]    send data length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_select(
                const boost::thread::id thread_id,
                boost::asio::ip::udp::endpoint &rs_endpoint,
                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                size_t &datalen) = 0;

        //! called from after realserver connect
        //! @param[in]    upstream thread id
        //! @param[out]    send buffer reference
        //! @param[out]    send data length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_connect(
                const boost::thread::id thread_id,
                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                size_t &datalen) = 0;

        //! called from after realserver connection fail
        //! @param[in]    upstream thread id
        //! @param[in]    fail realserver endpoint reference
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_connection_fail(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &rs_endpoint) = 0;

        //! called from after realserver send.
        //! @param[in]    upstream thread id
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_send(
                const boost::thread::id thread_id) = 0;

        //! called from after sorryserver select
        //! @param[in]    upstream thread id
        //! @param[in]    sorryserver endpoint reference
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_sorryserver_select(
                const boost::thread::id thread_id,
                boost::asio::ip::tcp::endpoint &sorry_endpoint) = 0;

        //! called from after sorryserver connect
        //!    @param[in]    upstream thread id
        //! @param[out]    send buffer reference.
        //! @param[out]    send length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_sorryserver_connect(
                const boost::thread::id thread_id,
                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                size_t &datalen) = 0;

        //! called from after sorryserver connection fail
        //! @param[in]    upstream thread id
        //! @param[in]    sorryserver endpoint reference.
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_sorryserver_connection_fail(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &sorry_endpoint) = 0;

        //! called from after sorryserver send
        //! @param[in]    upstream thread id
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_sorryserver_send(const boost::thread::id thread_id) = 0;

        //! called from after realserver receive for TCP/IP
        //! @param[in]    downstream thread id
        //! @param[in]    realserver TCP/IP endpoint reference
        //! @param[in]    realserver receive buffer reference.
        //! @param[in]    recv data length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &rs_endpoint,
                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen) = 0;
        //! called from after realserver receive.for UDP
        //! @param[in]    downstream thread id
        //! @param[in]    realserver UDP endpoint reference
        //! @param[in]    receive from realserver buffer reference
        //! @param[in]    recv data length
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::udp::endpoint &rs_endpoint,
                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen) = 0;

        //! called from after sorryserver receive
        //! @param[in]    downstream thread id
        //! @param[in]    sorryserver endpoint reference
        //! @param[in]    receive from realserver buffer reference.
        //! @param[in]    recv data length
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_sorryserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &sorry_endpoint,
                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen) = 0;

        //! called from UPSTREAM thread. make module original message.
        //! @param[in]    downstream thread id.
        //! @return         session use EVENT mode
        virtual    EVENT_TAG    handle_response_send_inform(
                const boost::thread::id thread_id) = 0;

        //! called from after client connection check. use TCP/IP only. create client send message.
        //! @param[in]    downstream thread id
        //! @param[out]    send buffer reference
        //! @param[out]    send data length
        //! @return     session use EVENT mode
        virtual EVENT_TAG    handle_client_connection_check(
                const boost::thread::id thread_id,
                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                size_t &datalen) = 0;

        //! called from after client select. use UDP only
        //! @param[in]    downstream thread id
        //!    @param[in]    client udp endpoint
        //! @param[out]    send buffer reference
        //! @param[out]    send data length
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_client_select(
                const boost::thread::id thread_id,
                boost::asio::ip::udp::endpoint &cl_endpoint,
                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                size_t &datalen) = 0;

        //!    called from after client send
        //!    @param[in]    downstream thread id
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_client_send(const boost::thread::id thread_id) = 0;

        //! call from client disconnect event. use upstream thread and downstream thread.
        //! @param[in]    upstream and downstream thread id( check! one thread one event! )
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_client_disconnect(
                const boost::thread::id thread_id) = 0;

        //! call from sorry mode event. use upstream thread and downstream thread
        //! @param[in]    upstream and downstream thread id( check! one thread one event and first time call pattern )
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_sorry_enable(const boost::thread::id thread_id) = 0;

        //! call from sorry mode disable. use upstream thread and downstream thread.
        //! @param[in]    upstream and downstream thread id( check! one thread one event )
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_sorry_disable(const boost::thread::id thread_id) = 0;

        //! call from realserver disconnect. use upstream thread and downstream thread
        //! @param[in]    upstream and downstream thread id( check! one thread one event )
        //! @param[in]    disconnected realserver endpoint.
        //! @return     session use EVENT mode
        virtual    EVENT_TAG    handle_realserver_disconnect(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &rs_endpoint) = 0;

        //! call from sorry server disconnect. use upstream thread and downstream thread
        //! @param[in]    upstream and downstream thread id( check! one thread one event )
        //! @param[in]    disconnect sorryserver endpoint
        //! @return        session use EVENT mode
        virtual    EVENT_TAG    handle_sorryserver_disconnect(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint &sorry_endpoint) = 0;

        //! call from realserver disconnect. use upstream thread and downstream thread.
        //! @param[in]    upstream and downstream thread id( check! one thread one event )
        //! @param[in]    disconnect realserver endpoint
        //! @return        session use EVENT mode.
        virtual    EVENT_TAG    handle_realserver_close(
                const boost::thread::id thread_id,
                const boost::asio::ip::udp::endpoint &rs_endpoint) = 0;
        //! call from
        virtual bool        is_exec_OK(unsigned int vs_attr)  {
                return true;
        }

        //! format dump data.
        //! @param[in]    data want to format
        //! @param[in]    data size
        //! @param[out]    format string
        //! @return
        static void  dump_memory(
                const char *data,
                const size_t data_size,
                std::string &data_dump) {
                if (data == NULL || data_size == 0) {
                        return;
                }

                boost::format formatter("%02X");
                for (size_t i = 0; i < data_size; i++) {
                        if (i % 32 == 0) {
                                data_dump += "\n";
                        } else if (i != 0 && i % 2 == 0) {
                                data_dump += " ";
                        }

                        formatter % static_cast<unsigned short>(static_cast<unsigned char>(data[i]));
                        data_dump += formatter.str();
                }
                data_dump += "\n";
        }

        //! get base statistic object.
        //! @return        base statistic object.
        virtual stats_base& get_stats()
        {
                return stats;
        }
};

} // namespace l7vsd

#endif    //PROTOCOL_MODULE_BASE_H
