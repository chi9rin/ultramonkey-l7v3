/*
 * @file  protocol_module_url.h
 * @brief protocol module url header file.
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
#include <boost/lexical_cast.hpp>
#include "http_protocol_module_base.h"

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/function.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/xpressive/xpressive.hpp>
#include "error_code.h"

#ifndef PROTOCOL_MODULE_URL_H
#define PROTOCOL_MODULE_URL_H

#define MAX_OPTION_SIZE 128
#define MAX_URL_MODULE_BUFFER_SIZE (8190 + MAX_BUFFER_SIZE)

namespace l7vs
{

class protocol_module_url : public http_protocol_module_base
{
public:
        enum SEND_STATUS_TAG {
                SEND_OK = 0,
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

        struct receive_data {
                char *receive_buffer;
                char *receive_buffer1;
                char *receive_buffer2;
                size_t receive_buffer_max_size;
                size_t receive_buffer_rest_size;
                std::list<send_status>  send_status_list;
                receive_data() {
                        receive_buffer1 = NULL;
                        receive_buffer2 = NULL;
                        receive_buffer = NULL;
                        receive_buffer_max_size = 0;
                        receive_buffer_rest_size = 0;
                }
                ~receive_data() {
                        if (receive_buffer1 != NULL) {
                                delete [] receive_buffer1;
                                receive_buffer1 = NULL;
                        }

                        if (receive_buffer2 != NULL) {
                                delete [] receive_buffer2;
                                receive_buffer2 = NULL;
                        }

                        receive_buffer = NULL;
                        receive_buffer_max_size = 0;
                        receive_buffer_rest_size = 0;
                }
        };


        struct session_thread_data_url {
                boost::thread::id thread_id;
                int thread_division;
                boost::thread::id pair_thread_id;
                std::map<boost::asio::ip::tcp::endpoint, receive_data> receive_data_map;
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
        typedef boost::shared_ptr<session_thread_data_url> thread_data_ptr;
        typedef std::map<boost::thread::id, thread_data_ptr>::iterator session_thread_data_map_it;
        typedef std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator receive_data_map_it;
protected:
        int forwarded_for;
	boost::array<char, MAX_OPTION_SIZE> sorry_uri ;
        std::map<boost::thread::id, thread_data_ptr> session_thread_data_map;
        boost::mutex    session_thread_data_map_mutex;

	typedef std::map< boost::array< char, MAX_OPTION_SIZE >, std::list< boost::asio::ip::tcp::endpoint > > pattern_endpointlist_map;
	typedef std::map< boost::array< char, MAX_OPTION_SIZE >, std::list< boost::asio::ip::tcp::endpoint > >::iterator pattern_endpointlist_map_it;
	pattern_endpointlist_map ptn_eplist_map;
	// for keep order '-P/--pattern-match' option value set & convert pattern to sregex
	typedef std::pair< boost::array< char, MAX_OPTION_SIZE >, boost::xpressive::sregex > pattern_sregex_converter_pair;
	typedef std::list< pattern_sregex_converter_pair > pattern_sregex_converter_pairlist;
	typedef std::list< pattern_sregex_converter_pair >::iterator pattern_sregex_converter_pairlist_it;
	pattern_sregex_converter_pairlist ptnsrgx_converter_pairlist;

private: 
	template < class T >typename T::endpoint stringtoEndpoint( const std::string &str, error_code &err );
	template < class T > void checkEndpoint( typename T::endpoint &ep, bool allow_any_address, error_code &err );
	bool checkOptionURIregex( const std::string &optstr, check_message_result &check_result );
	template < class T >
	bool setRealserverEndpoint( std::vector<std::string>::const_iterator &vsit,
				    const std::vector<std::string>::const_iterator &vsit_end,
				    std::list< typename T::endpoint > &ep_list,
				    check_message_result &check_result 
				  );

	// for use 'schedule_tcp' function
	std::list< std::list< realserver>::iterator >  rslist_it_list;
	std::list< std::list< realserver>::iterator >::iterator rslist_it_list_it;
	std::list< realserver >::iterator begin_rslist_it();
	std::list< realserver >::iterator end_rslist_it();
	std::list< realserver >::iterator next_rslist_it( std::list< realserver >::iterator in_itr );
	void useScheduleTCP( const boost::thread::id thread_id, std::list< std::list<realserver>::iterator >& in_rslist_it_list, boost::asio::ip::tcp::endpoint &rs_endpoint );

	// for debug 
	void dumpKeyary_endpointlist( int num );
	void dumpRslist_it_list( void );
	void dumpOptionString( const std::vector<std::string>& args );
	void dumpPtnsrgx_converter_pairlist( void );
	void testVolume( void );
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

        static const int COLLECT_STATS_OFF;
        static const int COLLECT_STATS_ON;

public:
        protocol_module_url();
        ~protocol_module_url();

        bool is_tcp();
        bool is_udp();
        void replication_interrupt();
        void initialize(rs_list_itr_func_type    inlist_begin,
                        rs_list_itr_func_type    inlist_end,
                        rs_list_itr_next_func_type    inlist_next,
                        boost::function< void(void) >    inlist_lock,
                        boost::function< void(void) >    inlist_unlock);
        void finalize();
        bool is_use_sorry();
        check_message_result check_parameter(const std::vector<std::string>& args);
        check_message_result set_parameter(const std::vector<std::string>& args);
        check_message_result add_parameter(const std::vector<std::string>& args);
        void get_option_info(std::string &option);
        void handle_rslist_update();
        void register_schedule(tcp_schedule_func_type inschedule);
        void register_schedule(udp_schedule_func_type inschedule);
        EVENT_TAG handle_session_initialize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint &client_endpoint_tcp, const boost::asio::ip::udp::endpoint &client_endpoint_udp);
        EVENT_TAG handle_session_finalize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id);
        EVENT_TAG handle_accept(const boost::thread::id thread_id);
        EVENT_TAG handle_client_recv(const boost::thread::id thread_id, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
        EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint &rs_endpoint);
        EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint &rs_endpoint, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen);
        EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen);
        EVENT_TAG handle_realserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint);
        EVENT_TAG handle_realserver_send(const boost::thread::id thread_id);
        EVENT_TAG handle_sorryserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint &sorry_endpoint);
        EVENT_TAG handle_sorryserver_connect(const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen);
        EVENT_TAG handle_sorryserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint);
        EVENT_TAG handle_sorryserver_send(const boost::thread::id thread_id);
        EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
        EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint &rs_endpoint, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
        EVENT_TAG handle_sorryserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
        EVENT_TAG handle_response_send_inform(const boost::thread::id thread_id);
        EVENT_TAG handle_client_connection_check(const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen);
        EVENT_TAG handle_client_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint &cl_endpoint, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen);
        EVENT_TAG handle_client_send(const boost::thread::id thread_id);
        EVENT_TAG handle_client_disconnect(const boost::thread::id thread_id);
        EVENT_TAG handle_sorry_enable(const boost::thread::id thread_id);
        EVENT_TAG handle_sorry_disable(const boost::thread::id thread_id);
        EVENT_TAG handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint);
        EVENT_TAG handle_sorryserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint);
        EVENT_TAG handle_realserver_close(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint &rs_endpoint);
};

inline bool operator < (const protocol_module_url::edit_data &lref, const protocol_module_url::edit_data &rref)
{
        return lref.insert_posission < rref.insert_posission;
}

class data_send_possible
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status) {
                return send_status.status == protocol_module_url::SEND_OK
                       &&  send_status.send_possible_size > 0;
        }
};

class data_send_disable
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status) {
                return send_status.status == protocol_module_url::SEND_NG
                       || send_status.send_rest_size > 0 ;
        }
};

class data_send_continue
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status) {
                return send_status.status == protocol_module_url::SEND_CONTINUE;
        }
};

class data_send_ng
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status) {
                return send_status.status == protocol_module_url::SEND_NG;
        }
};

class data_send_ok
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status) {
                return send_status.status == protocol_module_url::SEND_OK;
        }
};

class data_send_repeated
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status_first,
                               const protocol_module_url::send_status &send_status_second) {
                return send_status_first.status == protocol_module_url::SEND_NG
                       || send_status_first.status == protocol_module_url::SEND_CONTINUE;
        }
};

class data_send_list_incorrect
{
public:
        inline bool operator()(const protocol_module_url::send_status &send_status_first,
                               const protocol_module_url::send_status &send_status_second) {
                return (send_status_first.status == protocol_module_url::SEND_OK
                        && send_status_first.send_rest_size > 0)
                       || (send_status_first.status == protocol_module_url::SEND_CONTINUE)
                       || (send_status_first.status == protocol_module_url::SEND_NG);
        }
};

class rs_list_scoped_lock
{
protected:
        boost::function< void(void) >    rs_list_unlock;
public:
        rs_list_scoped_lock(boost::function< void(void) >    inlist_lock,
                            boost::function< void(void) >    inlist_unlock) {
                inlist_lock();
                rs_list_unlock = inlist_unlock;
        }
        ~rs_list_scoped_lock() {
                rs_list_unlock();
        }
};
}

#endif  //PROTOCOL_MODULE_URL_H

