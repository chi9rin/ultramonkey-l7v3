/*
 * @file  protocol_module_ip.h
 * @brief protocol module ip header file.
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
 *register_replication_area_unlock
 **********************************************************************/

#include <boost/thread/mutex.hpp>
#include "ip_protocol_module_base.h"
#include "ip_session_data_processor.h"
#include "ip_replication_data_processor.h"

#ifndef PROTOCOL_MODULE_IP_H
#define PROTOCOL_MODULE_IP_H

#define MAX_FORWARD_FOR_SIZE 36
#define MAX_OPTION_SIZE 128
#define MAX_IP_MODULE_BUFFER_SIZE (8190 + MAX_BUFFER_SIZE)
/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME 0x9e370001UL

namespace l7vs
{

    class protocol_module_ip : public ip_protocol_module_base
    {
	public:
	    enum DATA_STATE_TAG
	    {
		HTTP_START = 0,
		HTTP_HEADER,
		HTTP_BODY,
		UNKNOWN
	    };

	    typedef std::deque<std::pair<char*, size_t> > buffer_deque;
	    struct session_thread_data_ip
	    {
		boost::thread::id thread_id;
		boost::thread::id pair_thread_id;
		int thread_division;
		int end_flag;
		int accept_end_flag;
		int sorry_flag;
		int switch_flag;
		size_t data_buffer_size;
		size_t data_length;
		size_t data_offset;
		size_t current_message_rest_size;
		char* data_buffer;
		boost::array<char, MAX_FORWARD_FOR_SIZE> forwarded_for_buffer;
		int ip_hash;
		boost::asio::ip::tcp::endpoint client_endpoint;
		DATA_STATE_TAG data_state;
		EVENT_TAG last_status;
		buffer_deque buffer_sequence;
	    };

	    typedef boost::shared_ptr<session_thread_data_ip> thread_data_ptr;
	    typedef std::map<boost::thread::id, thread_data_ptr>::iterator session_thread_data_map_it;
	protected:
	    int forwarded_for;
	    int reschedule;
	    int timeout;
	    int realserver_connect_failed_max_count;
	    boost:: array<char,MAX_OPTION_SIZE> sorry_uri ;
	    std::map<boost::thread::id, thread_data_ptr> session_thread_data_map;
	    boost::mutex session_data_mutex;
	    boost::mutex session_thread_data_map_mutex;

	    ip_replication_data_processor* replication_data_processor;
	    ip_session_data_processor* ip_data_processor;

	    unsigned int l7vs_ip_service_calc_hash(const boost::asio::ip::tcp::endpoint& cl_endpoint)
	    {
		unsigned int hash = 0;
		if (cl_endpoint.address().is_v4())
		{
		    hash= cl_endpoint.address().to_v4().to_ulong() * GOLDEN_RATIO_PRIME;
		}
		else
		{
		    boost::asio::ip::address_v6::bytes_type v6_bytes = cl_endpoint.address().to_v6().to_bytes();
		    boost::asio::ip::address_v4::bytes_type v4_bytes = {{v6_bytes[12],v6_bytes[13],v6_bytes[14],v6_bytes[15]}};
		    boost::asio::ip::address_v4::address_v4 v4_address = boost::asio::ip::address_v4::address_v4(v4_bytes);
		    hash= v4_address.to_ulong() * GOLDEN_RATIO_PRIME;

		}
		return hash >> 32 - HASH_TABLE_BITS;
	    }


	public:
	    static const std::string MODULE_NAME;

	    static const int THREAD_DIVISION_UP_STREAM;
	    static const int THREAD_DIVISION_DOWN_STREAM;

	    static const int END_FLAG_OFF;
	    static const int END_FLAG_ON;

	    static const int ACCEPT_END_FLAG_OFF;
	    static const int ACCEPT_END_FLAG_ON;

	    static const int SORRY_FLAG_ON;
	    static const int SORRY_FLAG_OFF;

	    static const int SWITCH_FLAG_OFF;
	    static const int SWITCH_FLAG_ON;

	    static const int FORWARDED_FOR_OFF;
	    static const int FORWARDED_FOR_ON;

	public:
	    protocol_module_ip();
	    ~protocol_module_ip();
	    bool is_tcp();
	    bool is_udp();
	    void replication_interrupt();
	    void initialize(rs_list_itr_func_type	inlist_begin,
		    rs_list_itr_func_type	inlist_end,
		    rs_list_itr_next_func_type	inlist_next,
		    boost::function< void( void ) >	inlist_lock,
		    boost::function< void( void ) >	inlist_unlock);
	    void finalize();
	    bool is_use_sorry();
	    check_message_result check_parameter(const std::vector<std::string>& args);
	    check_message_result set_parameter(const std::vector<std::string>& args);
	    check_message_result add_parameter(const std::vector<std::string>& args);
            void get_option_info(std::string& option);
	    void handle_rslist_update();
	    void register_schedule(tcp_schedule_func_type inschedule);
	    void register_schedule(udp_schedule_func_type inschedule);
	    EVENT_TAG handle_session_initialize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, const boost::asio::ip::udp::endpoint& client_endpoint_udp);
	    EVENT_TAG handle_session_finalize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id);
	    EVENT_TAG handle_accept(const boost::thread::id thread_id);
	    EVENT_TAG handle_client_recv(const boost::thread::id thread_id, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	    EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& rs_endpoint);
	    EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    EVENT_TAG handle_realserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint);
	    EVENT_TAG handle_realserver_send(const boost::thread::id thread_id);
	    EVENT_TAG handle_sorryserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint);
	    EVENT_TAG handle_sorryserver_connect(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    EVENT_TAG handle_sorryserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint);
	    EVENT_TAG handle_sorryserver_send(const boost::thread::id thread_id);
	    EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	    EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	    EVENT_TAG handle_sorryserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	    EVENT_TAG handle_response_send_inform(const boost::thread::id thread_id);
	    EVENT_TAG handle_client_connection_check(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    EVENT_TAG handle_client_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    EVENT_TAG handle_client_send(const boost::thread::id thread_id);
	    EVENT_TAG handle_client_disconnect(const boost::thread::id thread_id);
	    EVENT_TAG handle_sorry_enable(const boost::thread::id thread_id);
	    EVENT_TAG handle_sorry_disable(const boost::thread::id thread_id);
	    EVENT_TAG handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint);
	    EVENT_TAG handle_sorryserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint);
	    EVENT_TAG handle_realserver_close(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint);
	    bool get_data_from_recvbuffer(thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	    bool put_data_into_sendbuffer(thread_data_ptr data_ptr, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	    bool create_x_forwarded_for(const std::string& client_endpoint,  const char* buffer,
		    const size_t buffer_len, size_t& x_forwarded_for_insert_pos,
		    std::string& x_forwarded_for_context);

    };

}//namespace l7vs





#endif  //PROTOCOL_MODULE_IP_H

