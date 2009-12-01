/*
 * @file  protocol_module_sessionless.h
 * @brief protocol module sessionless header file.
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

#include <boost/thread/mutex.hpp>
#include "http_protocol_module_base.h"

#ifndef PROTOCOL_MODULE_SESSIONLESS_H
#define PROTOCOL_MODULE_SESSIONLESS_H

#define MAX_OPTION_SIZE 128
#define MAX_SESSIONLESS_MODULE_BUFFER_SIZE (4 * 1024* 1024 + MAX_BUFFER_SIZE)

namespace l7vs
{

class protocol_module_sessionless : public http_protocol_module_base
{
public:
	enum SEND_STATUS_TAG
    {
		SEND_OK= 0,
		SEND_NG,
		SEND_END,
		SEND_CONTINUE
    };
	struct edit_data {
        std::string data;
        size_t data_size;
        size_t insert_posission;
        size_t replace_size;
    } ;

    struct send_status {
        SEND_STATUS_TAG status;
        size_t send_end_size;
        size_t send_rest_size;
        size_t send_possible_size;
        size_t send_offset;
        size_t unsend_size;
        int edit_division;
        boost::asio::ip::tcp::endpoint send_endpoint;
        std::list<edit_data> edit_data_list;
    };

    struct recive_data {
        char* recive_buffer;
        char* recive_buffer1;
        char* recive_buffer2;
        size_t recive_buffer_max_size;
        size_t recive_buffer_rest_size;
        std::list<send_status>  send_status_list;
        recive_data()
        {
            recive_buffer1 = NULL;
            recive_buffer2 = NULL;
            recive_buffer = NULL;
            recive_buffer_max_size = 0;
            recive_buffer_rest_size = 0;
        }
        ~recive_data()
        {
            if (recive_buffer1 != NULL)
            {
                delete [] recive_buffer1;
                recive_buffer1 = NULL;
            }

            if (recive_buffer2 != NULL)
            {
                delete [] recive_buffer2;
                recive_buffer2 = NULL;
            }

            recive_buffer = NULL;
            recive_buffer_max_size = 0;
            recive_buffer_rest_size = 0;
        }
	};


    struct session_thread_data_sessionless {
        boost::thread::id thread_id;
        int thread_division;
        boost::thread::id pair_thread_id;
        std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_data_map;
        int end_flag;
        int accept_end_flag;
        int sorry_flag;
        int sorryserver_switch_flag;
        int realserver_switch_flag;
        boost::asio::ip::tcp::endpoint target_endpoint;
        boost::asio::ip::tcp::endpoint client_endpoint_tcp;
        EVENT_TAG last_status;
    };

   typedef std::list<send_status>::iterator send_status_it;
   typedef boost::shared_ptr<session_thread_data_sessionless> thread_data_ptr;
   typedef std::map<boost::thread::id, thread_data_ptr>::iterator session_thread_data_map_it;
   typedef std::map<boost::asio::ip::tcp::endpoint, recive_data>::iterator recive_data_map_it;
protected:
	int forwarded_for;
	boost:: array<char,MAX_OPTION_SIZE> sorry_uri ;
	std::map<boost::thread::id, thread_data_ptr> session_thread_data_map;
	boost::mutex	session_thread_data_map_mutex;

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

	static const int SORRYSERVER_SWITCH_FLAG_OFF;
	static const int SORRYSERVER_SWITCH_FLAG_ON;

	static const int REALSERVER_SWITCH_FLAG_OFF;
	static const int REALSERVER_SWITCH_FLAG_ON;

	static const int EDIT_DIVISION_NO_EDIT;
	static const int EDIT_DIVISION_EDIT;

	static const int FORWARDED_FOR_OFF;
	static const int FORWARDED_FOR_ON;

public:
	protocol_module_sessionless();
	~protocol_module_sessionless();

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
};

inline bool operator < (const protocol_module_sessionless::edit_data& lref, const protocol_module_sessionless::edit_data& rref)
{
	return lref.insert_posission < rref.insert_posission;
}

class data_send_possible
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_OK
				&&  send_status.send_possible_size > 0;
	}
};

class data_send_disable
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_NG
		|| send_status.send_rest_size > 0 ;
	}
};

class data_send_continue
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_CONTINUE;
	}
};

class data_send_ng
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_NG;
	}
};

class data_send_ok
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_OK;
	}
};

class data_send_repeated
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status_first,
	        const protocol_module_sessionless::send_status& send_status_second)
	{
		return send_status_first.status == protocol_module_sessionless::SEND_NG
		|| send_status_first.status == protocol_module_sessionless::SEND_CONTINUE;
	}
};

class data_send_list_incorrect
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status_first,
	        const protocol_module_sessionless::send_status& send_status_second)
	{
		return (send_status_first.status == protocol_module_sessionless::SEND_OK
		&& send_status_first.send_rest_size > 0)
		|| (send_status_first.status == protocol_module_sessionless::SEND_CONTINUE)
		|| (send_status_first.status == protocol_module_sessionless::SEND_NG);
	}
};

}

#endif  //PROTOCOL_MODULE_SESSIONLESS_H

