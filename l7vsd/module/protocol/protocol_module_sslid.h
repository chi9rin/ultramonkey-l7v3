/*
 * @file  protocol_module_sslid.h
 * @brief protocol module sslid header file
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

#ifndef PROTOCOL_MODULE_SSLID_H
#define PROTOCOL_MODULE_SSLID_H

#include "ssl_protocol_module_base.h"
#include "sslid_replication_data_processor.h"
#include "sslid_session_data_processor.h"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

#define MAX_SSLID_BUFFER_SIZE (MAX_BUFFER_SIZE + 76)
namespace l7vs
{

struct session_thread_data_sslid
{
        int thread_division;
        boost::thread::id pair_thread_id;
        int end_flag;
        int accept_flag;
        int sorry_flag;
        boost::asio::ip::tcp::endpoint selected_realserver;
        boost::array<char, MAX_SSLID_BUFFER_SIZE> data_buffer;
        size_t data_begain_offset;
        size_t data_size;
        size_t current_record_rest_size;
        bool hello_message_flag;
        protocol_module_base::EVENT_TAG last_status;
};

class protocol_module_sslid : public ssl_protocol_module_base
{
    public:
        protocol_module_sslid();

        virtual ~protocol_module_sslid();

        bool is_tcp();

        bool is_udp();

        void initialize( rs_list_itr_func_type    inlist_begin,
                         rs_list_itr_func_type    inlist_end,
                         rs_list_itr_next_func_type inlist_next,
                         boost::function< void( void ) >    inlist_lock,
                         boost::function< void( void ) >    inlist_unlock );

        void finalize();

        bool is_use_sorry();

        check_message_result check_parameter( const std::vector<std::string>& args );

        check_message_result set_parameter( const std::vector<std::string>& args );

        check_message_result add_parameter( const std::vector<std::string>& args );

    void get_option_info( std::string& option );

        void handle_rslist_update();

        void register_schedule( tcp_schedule_func_type inschedule );

        void register_schedule( udp_schedule_func_type inschedule );

        EVENT_TAG handle_session_initialize(
                                const boost::thread::id up_thread_id,
                                const boost::thread::id down_thread_id,
                                const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
                                const boost::asio::ip::udp::endpoint& client_endpoint_udp );

        EVENT_TAG handle_session_finalize(
                                const boost::thread::id up_thread_id,
                                const boost::thread::id down_thread_id );

        EVENT_TAG handle_accept( const boost::thread::id thread_id );

        EVENT_TAG handle_client_recv(
                                const boost::thread::id thread_id,
                                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen );

        EVENT_TAG handle_realserver_select(
                                const boost::thread::id thread_id,
                                boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_realserver_select(
                                const boost::thread::id thread_id,
                                boost::asio::ip::udp::endpoint& rs_endpoint,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_realserver_connect(
                                const boost::thread::id thread_id,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_realserver_connection_fail(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_realserver_send( const boost::thread::id thread_id );

        EVENT_TAG handle_sorryserver_select(
                                const boost::thread::id thread_id,
                                boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_sorryserver_connect(
                                const boost::thread::id thread_id,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_sorryserver_connection_fail(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_sorryserver_send( const boost::thread::id thread_id );

        EVENT_TAG handle_realserver_recv(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& rs_endpoint,
                                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen );

        EVENT_TAG handle_realserver_recv(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::udp::endpoint& rs_endpoint,
                                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen );

        EVENT_TAG handle_sorryserver_recv(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& sorry_endpoint,
                                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen );

        EVENT_TAG handle_response_send_inform( const boost::thread::id thread_id );

        EVENT_TAG handle_client_connection_check(
                                const boost::thread::id thread_id,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_client_select(
                                const boost::thread::id thread_id,
                                boost::asio::ip::udp::endpoint& cl_endpoint,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_client_send( const boost::thread::id thread_id );

        EVENT_TAG handle_client_disconnect( const boost::thread::id thread_id );

        EVENT_TAG handle_sorry_enable( const boost::thread::id thread_id );

        EVENT_TAG handle_sorry_disable( const boost::thread::id thread_id );

        EVENT_TAG handle_realserver_disconnect(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_sorryserver_disconnect(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_realserver_close(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::udp::endpoint& rs_endpoint );

        void replication_interrupt();

        bool realserver_selected( const boost::asio::ip::tcp::endpoint& rs_endpoint );

    protected:
        int put_data_to_sendbuffer(
                                const boost::thread::id& thread_id,
                                boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen);
        class rs_list_scoped_lock {
            protected: 
                boost::function< void( void ) >    rs_list_unlock;
            public:
                rs_list_scoped_lock(boost::function< void( void ) >    inlist_lock,
                                    boost::function< void( void ) >    inlist_unlock) 
                {
                    inlist_lock();
                    rs_list_unlock = inlist_unlock; 
                }
                ~rs_list_scoped_lock() { rs_list_unlock(); }
        };
    protected:
        int timeout;
        int maxlist;
        int reschedule;
        typedef boost::shared_ptr<session_thread_data_sslid> thread_data_ptr;
        typedef std::map<boost::thread::id, thread_data_ptr> session_thread_data_map_type;
        session_thread_data_map_type session_thread_data_map;
        boost::mutex session_thread_data_map_mutex;
        int realserver_connect_failed_max_count;
        sslid_replication_data_processor* replication_data_processor;
        sslid_session_data_processor* session_data_processor;
    public:
        static const std::string MODULE_NAME;
        static const int REALSERVER_CONNECT_FAILED_MAX_COUNT;
        static const int THREAD_DIVISION_UP_STREAM;      // up thread
        static const int THREAD_DIVISION_DOWN_STREAM;    // down thread
        static const int END_FLAG_ON;                    // end flag ON
        static const int END_FLAG_OFF;                   // end flag OFF
        static const int ACCEPT_END_FLAG_OFF;            // accept flag off
        static const int ACCEPT_END_FLAG_ON;             // accept flag on
        static const int SORRY_FLAG_ON;                  // sorry flag on
        static const int SORRY_FLAG_OFF;                 // sorry flag off
};

}
#endif // PROTOCOL_MODULE_SSLID_H
