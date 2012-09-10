/*
 * @file  protocol_module_simple.cpp
 * @brief protocol module of any protocol.
 * @brief this module never keep session persistence.
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
#include <boost/xpressive/xpressive.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/format.hpp>
#include "protocol_module_simple.h"
#include "utility.h"

namespace l7vs
{
const std::string protocol_module_simple::MODULE_NAME = "simple";
const int protocol_module_simple::THREAD_DIVISION_UP_STREAM = 0;
const int protocol_module_simple::THREAD_DIVISION_DOWN_STREAM = 1;

const int protocol_module_simple::END_FLAG_OFF = 0;
const int protocol_module_simple::END_FLAG_ON = 1;

const int protocol_module_simple::ACCEPT_END_FLAG_OFF = 0;
const int protocol_module_simple::ACCEPT_END_FLAG_ON = 1;

const int protocol_module_simple::SORRY_FLAG_ON = 1;
const int protocol_module_simple::SORRY_FLAG_OFF = 0;

const int protocol_module_simple::SORRYSERVER_SWITCH_FLAG_OFF = 0;
const int protocol_module_simple::SORRYSERVER_SWITCH_FLAG_ON = 1;

const int protocol_module_simple::REALSERVER_SWITCH_FLAG_OFF = 0;
const int protocol_module_simple::REALSERVER_SWITCH_FLAG_ON = 1;

const int protocol_module_simple::EDIT_DIVISION_NO_EDIT = 0;
const int protocol_module_simple::EDIT_DIVISION_EDIT = 1;

const int protocol_module_simple::FORWARDED_FOR_OFF = 0;
const int protocol_module_simple::FORWARDED_FOR_ON = 1;

const int protocol_module_simple::COLLECT_STATS_OFF = 0;
const int protocol_module_simple::COLLECT_STATS_ON = 1;

//! constructor
protocol_module_simple::protocol_module_simple() :
        http_protocol_module_base(MODULE_NAME), forwarded_for(FORWARDED_FOR_OFF)
{
        sorry_uri.assign('\0');
        sorry_uri[0] = '/';
}
//! destructor
protocol_module_simple::~protocol_module_simple()
{
}
//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool protocol_module_simple::is_tcp()
{
        return true;
}

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool protocol_module_simple::is_udp()
{
        return false;
}

//! replication interval interrupt
//! timer thread call this function. from virtualservice.
void protocol_module_simple::replication_interrupt()
{
}
//! initialize function. called from module control. module loaded call
//! @param[in]    realserver list iterator begin function object type
//!    @param[in]    realserver list iterator end function object type
//! @param[in]    realserver list iterator next function object type
//! @param[in]    realserver list mutex lock function object type.
//! @param[in]    realserver list mutex unlock function object type
void protocol_module_simple::initialize(rs_list_itr_func_type    inlist_begin,
                rs_list_itr_func_type    inlist_end,
                rs_list_itr_next_func_type    inlist_next,
                boost::function< void(void) >    inlist_lock,
                boost::function< void(void) >    inlist_unlock)
{
        //RealServer list begin function
        rs_list_begin = inlist_begin;
        //RealServer list end function
        rs_list_end = inlist_end;
        //RealServer list next function
        rs_list_next = inlist_next;
        //RealServer list lock function
        rs_list_lock = inlist_lock;
        //RealServer list unlock function
        rs_list_unlock = inlist_unlock;
}

//! finalize called from module control. module unloaded call.
void protocol_module_simple::finalize()
{
        //RealServer list functions initialization
        //RealServer list begin function
        rs_list_begin.clear();
        //RealServer list end function
        rs_list_end.clear();
        //RealServer list next function
        rs_list_next.clear();
        //RealServer list lock function
        rs_list_lock.clear();
        //RealServer list unlock function
        rs_list_unlock.clear();

        //Replication functions initialization
        //component memory allocate function
        replication_pay_memory.clear();
        //component memory lock function
        replication_area_lock.clear();
        //component memory unlock function
        replication_area_unlock.clear();

        //ScheduleModule's functions initialization
        schedule_tcp.clear();

        //logger functions initialization
        //log level getting function
        getloglevel.clear();
        //logger(Fatal)
        putLogFatal.clear();
        //logger(Error)
        putLogError.clear();
        //logger(Warn)
        putLogWarn.clear();
        //logger(Info)
        putLogInfo.clear();
        //logger(Debug)
        putLogDebug.clear();
}

//! sorry support check
//! @return true sorry mode is supported.
//! @return false sorry mode is unsupported.
bool protocol_module_simple::is_use_sorry()
{
        return true;
}

//! realserver list update event
void protocol_module_simple::handle_rslist_update()
{
}

//! module parameter check.used by l7vsadm
//! @param[in]    module parameter string list
//! @return    result.flag true is parameter is no problem.
//! @return result.flag false is parameter is problem.
protocol_module_base::check_message_result protocol_module_simple::check_parameter(const std::vector <
                std::string > & args)
{
        using namespace boost::xpressive;
        //set check result true
        check_message_result check_result;
        check_result.flag = true;

        typedef std::vector<std::string>::const_iterator vec_str_it;

        try {
                vec_str_it it = args.begin();
                vec_str_it it_end = args.end();
                //loop option strings
                for (; it != it_end; ++it) {
                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = "Option error.";
                                putLogError(100005, check_result.message, __FILE__, __LINE__);
                                //loop break
                                break;
                }
        } catch (...) {
                check_result.flag = false;
        }

        return check_result;
}

//! parameter set
//! @param[in] module parameter string list
//! @return    result.flag true is parameter is no problem.
//! @return result.flag false is parameter is problem.
protocol_module_base::check_message_result protocol_module_simple::set_parameter(const std::vector <
                std::string > & args)
{
        using namespace boost::xpressive;
        //set check result flag true
        check_message_result check_result;
        check_result.flag = true;

        typedef std::vector<std::string>::const_iterator vec_str_it;

        try {
                vec_str_it it = args.begin();
                vec_str_it it_end = args.end();
                for (; it != it_end; ++it) {
                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = "Option error.";

                                putLogError(100013, check_result.message, __FILE__,
                                            __LINE__);

                                break;
                }
        } catch (...) {
                check_result.flag = false;
        }

        return check_result;
}

//! parameter add
//! @param[in] module parameter string list
//! @return    result.flag true is parameter is no problem.
//! @return result.flag false is parameter is problem.
protocol_module_base::check_message_result protocol_module_simple::add_parameter(const std::vector <
                std::string > & args)
{
        check_message_result check_result;
        //set check result flag true
        check_result.flag = true;

        //param list is not empty
        if (!args.empty()) {
                //set check result flag false
                check_result.flag = false;
                //set check result message
                check_result.message = "Cannot add option.";
                putLogError(100016, check_result.message, __FILE__, __LINE__);
        }

        return check_result;
}

//! get option info
//! @param[out] module parameter string
void protocol_module_simple::get_option_info(std::string &option)
{
}

//! TCP/IP scheduled function registration.
//! @param[in] schedule module TCP/IP scheduled function object type
void protocol_module_simple::register_schedule(tcp_schedule_func_type inschedule)
{
        schedule_tcp = inschedule;
}

//! UDP scheduled function registration
//! @param[in] schedule module UDP scheduled function object type
void protocol_module_simple::register_schedule(udp_schedule_func_type inschedule)
{
}

//! called from session initialize use in upstream_thread
//! @param[in]    upstream thread id.
//! @param[in]    downstream thread id
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_session_initialize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id,
        const boost::asio::ip::tcp::endpoint &client_endpoint_tcp,
        const boost::asio::ip::udp::endpoint &client_endpoint_udp)
{
        EVENT_TAG status = FINALIZE;

        //session thread initialization
        try {
                thread_data_ptr p_up(new session_thread_data_simple);
                p_up->thread_id = up_thread_id;
                p_up->thread_division = THREAD_DIVISION_UP_STREAM;
                p_up->pair_thread_id = down_thread_id;
                p_up->accept_end_flag = ACCEPT_END_FLAG_OFF;
                p_up->end_flag = END_FLAG_OFF;
                p_up->sorry_flag = SORRY_FLAG_OFF;
                p_up->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                p_up->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                p_up->last_status = INITIALIZE;
                p_up->client_endpoint_tcp = client_endpoint_tcp;

                receive_data recv_data;
                p_up->receive_data_map[client_endpoint_tcp] = recv_data;

                thread_data_ptr p_down(new session_thread_data_simple);
                p_down->thread_id = down_thread_id;
                p_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
                p_down->pair_thread_id = up_thread_id;
                p_down->accept_end_flag = ACCEPT_END_FLAG_OFF;
                p_down->end_flag = END_FLAG_OFF;
                p_down->sorry_flag = SORRY_FLAG_OFF;
                p_down->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                p_down->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                p_down->last_status = INITIALIZE;
                p_down->client_endpoint_tcp = client_endpoint_tcp;

                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_data_map[up_thread_id] = p_up;
                session_thread_data_map[down_thread_id] = p_down;

                status = ACCEPT;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}
//! called from session finalize use in upstream thread.
//! @param[in]    upstream thread id.
//! @param[in]    downstream thread id
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_session_finalize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id)
{
        EVENT_TAG status = STOP;

        //session thread free
        try {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_data_map_it	session_thread_data_it = session_thread_data_map.find(up_thread_id);
                if (session_thread_data_it != session_thread_data_map.end()) {
                        thread_data_ptr p_up = session_thread_data_it->second;
                        session_thread_data_map.erase(up_thread_id);
                }

                session_thread_data_it = session_thread_data_map.find(down_thread_id);
                if (session_thread_data_it != session_thread_data_map.end()) {
			thread_data_ptr p_down = session_thread_data_it->second;
                        session_thread_data_map.erase(down_thread_id);
                }

                status = STOP;
        } catch (...) {
                status = STOP;
        }

        return status;
}

//! called from after session accept.in client socket use in upstream thread.
//! @param[in]    upstream thread id.
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_accept(const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                        boost::format formatter("Invalid thread id. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100022, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                session_data = session_thread_it->second;

                //set accept end flag ON
                session_data->accept_end_flag = ACCEPT_END_FLAG_ON;

		//sorry flag on
                if (session_data->sorry_flag == SORRY_FLAG_ON) {
                        //set return status
                        status = SORRYSERVER_SELECT;
                }
                //sorry flag off
                else {
                        //set return status
                        status = REALSERVER_SELECT;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after session recv in client socket. use in upstream thread.
//! @param[in]    upstream thread id
//! @param[in]    receive buffer reference.
//! @param[in]    receive length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_client_recv(const boost::thread::id thread_id,
                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t buffer_size = 0;
        std::string str_value;
        thread_data_ptr session_data;
        char *buffer1 = NULL;
        char *buffer2 = NULL;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

	// hibari add for debug
	boost::format formatter("handle_client_recv() =>  recvbuffer = %s");
                formatter % recvbuffer.data() ;
                putLogFatal(9999, formatter.str(), __FILE__, __LINE__);
	// hibari add end


        //parameter check
        if (recvlen > recvbuffer.size()) {
                std::cerr << "protocol_module_simple::handle_client_recv() : Data size bigger than buffer size." << std::endl;
                boost::format formatter("Data size bigger than buffer size. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100025, formatter.str(), __FILE__, __LINE__);
                return FINALIZE;
        }

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100026, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                //end flag on
                if (session_data->end_flag == END_FLAG_ON) {
                        status = CLIENT_RECV;
                }
                //end flag off
                else {
                        receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
                        if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                                boost::format formatter("Invalid endpoint. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100027, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        receive_data &recv_data = receive_data_it->second;

                        send_status_it it = recv_data.send_status_list.begin();
                        send_status_it it_end = recv_data.send_status_list.end();

                        //status list check
                        it = std::find_if(it, it_end, data_send_ok());
                        if (unlikely(it != it_end)) {
                                boost::format formatter("Sending data is not correct. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100028, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        //status list check
                        it = recv_data.send_status_list.begin();
                        it = std::adjacent_find(it, it_end, data_send_repeated());
                        if (unlikely(it != it_end)) {
                                boost::format formatter("Sending data is not correct. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100029, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        it = recv_data.send_status_list.begin();
                        //get original status info
                        while (it != it_end) {
                                //item status is SEND_END
                                if (it->status == SEND_END) {
                                        //erase from list
                                        recv_data.send_status_list.erase(it++);
                                        continue;
                                }
                                //item status is SEND_CONTINUE
                                else if (it->status == SEND_CONTINUE) {
                                        it->send_offset += it->send_end_size;
                                        data_remain_start = it->send_offset;
                                        break;
                                }
                                //item status is SEND_NG
                                else {
                                        data_remain_start = it->send_offset;
                                        data_remain_size = it->unsend_size;
                                        break;
                                }

                                ++it;
                        }

                        //receive buffer process
                        //buffer rest size < request size
                        if (recv_data.receive_buffer_rest_size < recvlen) {
                                //buffer max size < remain size + request size
                                //buffer is need reallocate
                                if (recv_data.receive_buffer_max_size < data_remain_size + recvlen) {
                                        //the buffer's size that will be allocated is exceed the upper limit value
                                        if (MAX_SIMPLE_MODULE_BUFFER_SIZE < data_remain_size + recvlen) {
                                                std::cerr << "protocol_module_simple::handle_client_recv() : the buffer's size that will be allocated is exceed the upper limit value." << std::endl;
                                                boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                                                formatter % boost::this_thread::get_id();
                                                putLogError(100030, formatter.str(), __FILE__, __LINE__);
                                                return FINALIZE;
                                        }

                                        buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                                        //receive_buffer1's memory allocate and initialization
                                        buffer1 = new char[buffer_size];
                                        memset(buffer1, 0, buffer_size);
                                        //receive_buffer2's memory allocate and initialization
                                        buffer2 = new char[buffer_size];
                                        memset(buffer2, 0, buffer_size);

                                        //copy data from old buffer to new buffer
                                        memcpy(buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                        memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                        //free old buffer1 and old buffer2
                                        if (recv_data.receive_buffer1 != NULL) {
                                                delete[] recv_data.receive_buffer1;
                                                recv_data.receive_buffer1 = NULL;
                                        }

                                        if (recv_data.receive_buffer2 != NULL) {
                                                delete[] recv_data.receive_buffer2;
                                                recv_data.receive_buffer2 = NULL;
                                        }

                                        //set new buffer pointer
                                        recv_data.receive_buffer1 = buffer1;
                                        recv_data.receive_buffer2 = buffer2;
                                        recv_data.receive_buffer = recv_data.receive_buffer1;
                                        //set new buffer's max size
                                        recv_data.receive_buffer_max_size = buffer_size;
                                }
                                //buffer's max size >= remain data size + request size
                                //buffer isn't need reallocate, but switch
                                else {
                                        //pointer valid check
                                        if (unlikely(recv_data.receive_buffer1 == NULL || recv_data.receive_buffer2 == NULL)) {
                                                boost::format formatter("Invalid pointer. thread id : %d.");
                                                formatter % boost::this_thread::get_id();
                                                putLogError(100031, formatter.str(), __FILE__, __LINE__);
                                                throw - 1;
                                        }
                                        //using buffer is buffer1
                                        if (recv_data.receive_buffer == recv_data.receive_buffer1) {
                                                //buffer2 initialization
                                                memset(recv_data.receive_buffer2, 0, recv_data.receive_buffer_max_size);
                                                //copy data from buffer1 to buffer2
                                                memcpy(recv_data.receive_buffer2, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                                memcpy(recv_data.receive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                                                //set buffer2 as using buffer
                                                recv_data.receive_buffer = recv_data.receive_buffer2;
                                        }
                                        //using buffer is buffer2
                                        else {
                                                //buffer1 initialization
                                                memset(recv_data.receive_buffer1, 0, recv_data.receive_buffer_max_size);
                                                //copy data from buffer2 to buffer1
                                                memcpy(recv_data.receive_buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                                memcpy(recv_data.receive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                                //set buffer1 as using buffer
                                                recv_data.receive_buffer = recv_data.receive_buffer1;
                                        }
                                }

                                //set buffer's rest size
                                recv_data.receive_buffer_rest_size = recv_data.receive_buffer_max_size - data_remain_size - recvlen;

                                //remain_size recalc
                                data_remain_size += recvlen;

                                send_status_it it_begin = recv_data.send_status_list.begin();
                                send_status_it it_end = recv_data.send_status_list.end();

                                //offset recalc
                                for (; it_begin != it_end; ++it_begin) {
                                        it_begin->send_offset -= data_remain_start;
                                }
                        }
                        //buffer's rest size >= request size
                        else {
                                //pointer valid check
                                if (unlikely(recv_data.receive_buffer == NULL)) {
                                        boost::format formatter("Invalid pointer. thread id : %d.");
                                        formatter % boost::this_thread::get_id();
                                        putLogError(100032, formatter.str(), __FILE__, __LINE__);
                                        throw - 1;
                                }
                                //copy data from parameter to using buffer
                                memcpy(recv_data.receive_buffer + recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size,
                                       recvbuffer.data(), recvlen);
                                //buffer's rest size recalc
                                recv_data.receive_buffer_rest_size -= recvlen;
                                //remain data size recalc
                                data_remain_size += recvlen;
                        }

                        it = recv_data.send_status_list.begin();
                        it_end = recv_data.send_status_list.end();

                        //set request rest size
                        request_data_remain_size = recvlen;

                        //original status process
                        for (; it != it_end; ++it) {
                                //status is SEND_CONTINUE
                                if (it->status == SEND_CONTINUE) {
                                        //send rest size > request size
                                        if (it->send_rest_size > request_data_remain_size) {
                                                //send possible size recalc
                                                it->send_possible_size = request_data_remain_size;
                                                //send rest size recalc
                                                it->send_rest_size -= request_data_remain_size;
                                                //send end size recalc
                                                it->send_end_size = 0;
                                                //request size recalc
                                                request_data_remain_size = 0;
                                        }
                                        //send rest size <= request size
                                        else {
                                                //send possible size recalc
                                                it->send_possible_size = it->send_rest_size;
                                                //send rest size recalc
                                                request_data_remain_size -= it->send_rest_size;
                                                //send end size recalc
                                                it->send_end_size = 0;
                                                //request size recalc
                                                it->send_rest_size = 0;
                                        }
                                        //set edit_division flag off
                                        it->edit_division = EDIT_DIVISION_NO_EDIT;
                                        //set status SEND_OK
                                        it->status = SEND_OK;
                                }
                                //status is SEND_NG
                                else if (it->status == SEND_NG) {
                                        //set edit_division flag off
                                        it->edit_division = EDIT_DIVISION_NO_EDIT;
                                        //send_rest_size recalc
                                        it->send_rest_size = it->unsend_size + request_data_remain_size;

                                        //recalc fields value according to send_rest_size and request rest size
                                        if (it->send_rest_size > it->unsend_size + request_data_remain_size) {
                                                it->send_possible_size = it->unsend_size + request_data_remain_size;
                                                it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                                                it->send_end_size = 0;
                                                it->unsend_size = 0;
                                                request_data_remain_size = 0;
                                        } else {
                                                it->send_possible_size = it->send_rest_size;
                                                request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                                                it->send_end_size = 0;
                                                it->unsend_size = 0;
                                                it->send_rest_size = 0;
                                        }

                                        //change status from SEND_NG to SEND_OK
                                        it->status = SEND_OK;
                                }
                                //no request rest data to process
                                if (request_data_remain_size <= 0) {
                                        break;
                                }
                        }
                        //there are still rest data need to process
                        //new status created and add to status list
                        while (request_data_remain_size > 0) {
                                //new status created
                                send_status new_send_state;

                                new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
                                new_send_state.send_end_size = 0;
                                new_send_state.send_offset = 0;
                                new_send_state.send_possible_size = 0;
                                new_send_state.unsend_size = 0;
                                new_send_state.send_rest_size = 0;
                                //status initialize to SEND_NG
                                new_send_state.status = SEND_NG;
                                //add new status to status_list
                                recv_data.send_status_list.push_back(new_send_state);
                                std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
                                //calc offset
                                new_send_it->send_offset = recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size
                                                           - request_data_remain_size;

                                new_send_it->edit_division = EDIT_DIVISION_NO_EDIT;
                                new_send_it->send_rest_size = request_data_remain_size;

                                //recalc fields value according to send_rest_size and request rest size
                                if (new_send_it->send_rest_size > request_data_remain_size) {
                                        new_send_it->send_possible_size = request_data_remain_size;
                                        new_send_it->send_rest_size -= request_data_remain_size;
                                        new_send_it->send_end_size = 0;
                                        request_data_remain_size = 0;
                                } else {
                                        new_send_it->send_possible_size = new_send_it->send_rest_size;
                                        request_data_remain_size -= new_send_it->send_rest_size;
                                        new_send_it->send_end_size = 0;
                                        new_send_it->send_rest_size = 0;
                                }

                                //change status from SEND_NG to SEND_OK
                                new_send_it->status = SEND_OK;
                        }

                        //search for send_possible item in status list
                        send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                         data_send_possible());
                        //the data that can be sent possible is exist
                        if (it_find != recv_data.send_status_list.end()) {
                                //sorry flag is on
                                if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                        status = SORRYSERVER_CONNECT;
                                }
                                //sorry flag is off
                                else {
                                        status = REALSERVER_CONNECT;
                                }
                        }
                        //the data that can be sent possible is not exist
                        else {
                                status = CLIENT_RECV;
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after realserver select.use in upstream thread.
//! @param[in]    upstream thread id
//! @param[out]    realserver TCP endpoint
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint &rs_endpoint)
{
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint tmp_endpoint;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        session_thread_data_map_it session_thread_it_end;
        receive_data_map_it receive_data_it;

        if (schedule_tcp.empty()) {
                std::cerr << "protocol_module_simple::handle_realserver_select() : Schedule_tcp function is empty." << std::endl;
                boost::format formatter("Schedule_tcp function is empty. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100037, formatter.str(), __FILE__, __LINE__);
                return FINALIZE;
        }

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100038, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                //call schedule_module's schedule function, get realserver endpoint
                {
                        rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                        schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);
                }

                //endpoint decide
                if (rs_endpoint != tmp_endpoint) {
                        //save rs endpoint
                        session_data->target_endpoint = rs_endpoint;
                        status = REALSERVER_CONNECT;
                } else {
                        //set end flag on
                        session_data->sorry_flag = SORRY_FLAG_ON;
                        status = SORRYSERVER_SELECT;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after realserver select
//! @param[in]    upstream thread id
//! @param[out]    realserver UDP endpoint
//! @param[out]    sendbuffer reference
//! @param[out]    send data length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint &rs_endpoint, boost::array < char,
        MAX_BUFFER_SIZE > & sendbuffer, size_t &datalen)
{
        return STOP;
}
//! called from after realserver connect
//! @param[in]    upstream thread id
//! @param[out]    sendbuffer reference
//! @param[out]    send data length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen)
{
        EVENT_TAG status = FINALIZE;
        size_t send_buffer_remian_size = 0;
        size_t copy_size = 0;
        const int send_buffer_end_size = sendbuffer.max_size();
        thread_data_ptr session_data;

        try {
			session_thread_data_map_mutex.lock();

			//thread id check
			session_thread_data_map_it	session_thread_it = session_thread_data_map.find(thread_id);
			if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
				boost::format formatter("Invalid thread id. thread id : %d.");
				formatter % boost::this_thread::get_id();
				putLogError(100042, formatter.str(), __FILE__, __LINE__);
				session_thread_data_map_mutex.unlock();
				throw - 1;
			}

			session_data = session_thread_it->second;

			//endpoint check
			receive_data_map_it	receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
			if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
				boost::format formatter("Invalid endpoint. thread id : %d.");
				formatter % boost::this_thread::get_id();
				putLogError(100043, formatter.str(), __FILE__, __LINE__);
				session_thread_data_map_mutex.unlock();
				throw - 1;
			}
	
			//receive_buffer pointer check
			 receive_data &recv_data = receive_data_it->second;
			if (unlikely(recv_data.receive_buffer == NULL)) {
				session_thread_data_map_mutex.unlock();
				return CLIENT_RECV;
			}

			//send list check
			send_status_it it = recv_data.send_status_list.begin();
			send_status_it it_end = recv_data.send_status_list.end();
			it = find_if(it, it_end, data_send_possible());
			if (unlikely(it == it_end)) {
				boost::format formatter("Sending possible data is not existed. thread id : %d.");
				formatter % boost::this_thread::get_id();
				putLogError(100045, formatter.str(), __FILE__, __LINE__);
				session_thread_data_map_mutex.unlock();
				throw - 1;
			}
			session_thread_data_map_mutex.unlock();

                //send buffer rest size initialization
                send_buffer_remian_size = send_buffer_end_size;

                //copy data as large as possible
                //send_possible_size is larger
                if (send_buffer_remian_size >= it->send_possible_size) {
                                copy_size = it->send_possible_size;
                                //copy data by send_possible size
                                memcpy(sendbuffer.data(), recv_data.receive_buffer + it->send_offset, copy_size);
                                it->send_end_size = copy_size;
                                it->send_possible_size = 0;
                                send_buffer_remian_size -= copy_size;
                        }
                        //buffer rest size is larger
                        else {
                                //copy data by buffer rest size
                                memcpy(sendbuffer.data(), recv_data.receive_buffer + it->send_offset, send_buffer_remian_size);
                                it->send_end_size = send_buffer_remian_size;
                                it->send_possible_size -= send_buffer_remian_size;
                                send_buffer_remian_size = 0;
                        }

                //set copied data length
                datalen = send_buffer_end_size - send_buffer_remian_size;
                status = REALSERVER_SEND;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after realserver connection fail
//! @param[in]    upstream thread id
//! @param[in]    fail realserver endpoint reference
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                        boost::format formatter("Invalid thread id. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100049, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                session_data = session_thread_it->second;

                //set end flag ON
                session_data->end_flag = END_FLAG_ON;
                status = CLIENT_DISCONNECT;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}
//! called from after realserver send.
//! @param[in]    upstream thread id
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_send(
        const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                        //thread_id check
                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100052, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                //endpoint check
                receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100053, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                receive_data &recv_data = receive_data_it->second;

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                //status list check
                it = std::adjacent_find(it, it_end, data_send_list_incorrect());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending possible data is invalid. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100054, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //status list check
                it = recv_data.send_status_list.begin();
                it = std::find_if(it, it_end, data_send_ok());
                if (unlikely(it == it_end)) {
                        boost::format formatter("Sending possible data is not existed. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100055, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //sending possible data is exist
                if (it->send_possible_size > 0) {
                        //status remain SEND_OK
                        it->status = SEND_OK;
                        //offset recalc
                        it->send_offset += it->send_end_size;

                        //insert_position recalc
                        for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
                             != it->edit_data_list.end(); ++list_it) {
                                list_it->insert_posission -= it->send_end_size;
                        }

                        //send_end_size recalc
                        it->send_end_size = 0;
                }
                //sending possible data is not exist
                else {
                        //can receive from client continue
                        if (it->send_rest_size > 0) {
                                //change status from SEND_OK to SEND_CONTINUE
                                it->status = SEND_CONTINUE;
                        }
                        //can not receive from client continue
                        else {
                                //change status from SEND_OK to SEND_END
                                it->status = SEND_END;
                        }
                }

                it = recv_data.send_status_list.begin();
                it = find_if(it, it_end, data_send_ok());
                //send_ok item is exist
                if (it != it_end) {
                        status = REALSERVER_CONNECT;
                }
                //send_ok item is exist
                else {
                        status = CLIENT_RECV;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after sorryserver select
//! @param[in]    upstream thread id
//! @param[in]    sorryserver endpoint reference
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint &sorry_endpoint)
{
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint client_endpoint;

        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                        boost::format formatter("Invalid thread id. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100058, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                session_data = session_thread_it->second;
                //set sorry_endpoint
                session_data->target_endpoint = sorry_endpoint;

                //endpoint check
                receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100059, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                status = SORRYSERVER_CONNECT;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after sorryserver connect
//!    @param[in]    upstream thread id
//! @param[out]    send buffer reference.
//! @param[out]    send length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen)
{
        EVENT_TAG status = FINALIZE;
        size_t send_buffer_remian_size = 0;
        size_t copy_size = 0;
        const int send_buffer_end_size = sendbuffer.max_size();
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                        //thread id check
                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100063, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                //endpoint check
                receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
                if (unlikely(receive_data_it
                             == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100064, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //receive_buffer pointer check
                receive_data &recv_data = receive_data_it->second;
                if (unlikely(recv_data.receive_buffer == NULL)) {
                        status = CLIENT_RECV;
                        goto handle_sorryserver_connect_out;
                }

                //send list check
                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                it = find_if(it, it_end, data_send_possible());
                if (unlikely(it == it_end)) {
                        boost::format formatter("Sending possible data is not existed. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100066, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //send buffer rest size initialization
                send_buffer_remian_size = send_buffer_end_size;

                        //copy data as large as possible
                        //send_possible_size is larger
                        if (send_buffer_remian_size >= it->send_possible_size) {
                                copy_size = it->send_possible_size;
                                //copy data by send_possible size
                                memcpy(sendbuffer.data(), recv_data.receive_buffer
                                       + it->send_offset, copy_size);
                                it->send_end_size = it->send_possible_size;
                                it->send_possible_size = 0;
                                send_buffer_remian_size -= copy_size;
                        }
                        //buffer rest size is larger
                        else {
                                //copy data by buffer rest size
                                memcpy(sendbuffer.data(), recv_data.receive_buffer
                                       + it->send_offset, send_buffer_remian_size);
                                it->send_end_size = send_buffer_remian_size;
                                it->send_possible_size -= send_buffer_remian_size;
                                send_buffer_remian_size = 0;
                        }

                //set copied data length
                datalen = send_buffer_end_size - send_buffer_remian_size;

                status = SORRYSERVER_SEND;
        } catch (...) {
                status = FINALIZE;
        }

handle_sorryserver_connect_out:
        return status;
}

//! called from after sorryserver connection fail
//! @param[in]    upstream thread id
//! @param[in]    sorryserver endpoint reference.
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                        boost::format formatter("Invalid thread id. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100070, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                session_data = session_thread_it->second;

                //set end flag on
                session_data->end_flag = END_FLAG_ON;

                status = CLIENT_DISCONNECT;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after sorryserver send
//! @param[in]    upstream thread id
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_send(
        const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                        //thread_id check
                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100073, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                //endpoint check
                receive_data_it = session_data->receive_data_map.find(session_data->client_endpoint_tcp);
                if (unlikely(receive_data_it
                             == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100074, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                receive_data &recv_data = receive_data_it->second;

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();
                //status list check
                it = std::adjacent_find(it, it_end, data_send_list_incorrect());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending possible data is invalid. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100075, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //status list check
                it = recv_data.send_status_list.begin();
                it = find_if(it, it_end, data_send_ok());
                if (unlikely(it == it_end)) {
                        boost::format formatter("Sending possible data is not existed. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100076, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //sending possible data is exist
                if (it->send_possible_size > 0) {
                        //status remain SEND_OK
                        it->status = SEND_OK;
                        //offset recalc
                        it->send_offset += it->send_end_size;

                        //insert_position recalc
                        for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
                             != it->edit_data_list.end(); ++list_it) {
                                list_it->insert_posission -= it->send_end_size;
                        }

                        //send_end_size recalc
                        it->send_end_size = 0;
                }
                //sending possible data is not exist
                else {
                        //can receive from client continue
                        if (it->send_rest_size > 0) {
                                //change status from SEND_OK to SEND_CONTINUE
                                it->status = SEND_CONTINUE;
                        }
                        //can not receive from client continue
                        else {
                                //change status from SEND_OK to SEND_END
                                it->status = SEND_END;
                        }
                }

                it = recv_data.send_status_list.begin();
                it = find_if(it, it_end, data_send_ok());
                //send_ok item is exist
                if (it != it_end) {
                        status = SORRYSERVER_CONNECT;
                }
                //send_ok item is exist
                else {
                        status = CLIENT_RECV;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after realserver receive.for UDP
//! @param[in]    downstream thread id
//! @param[in]    realserver UDP endpoint reference
//! @param[in]    receive from realserver buffer reference
//! @param[in]    recv data length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint &rs_endpoint, const boost::array < char,
        MAX_BUFFER_SIZE > & recvbuffer, const size_t recvlen)
{
        return STOP;
}

//! called from after realserver receive for TCP/IP
//! @param[in]    downstream thread id
//! @param[in]    realserver TCP/IP endpoint reference
//! @param[in]    realserver receive buffer reference.
//! @param[in]    recv data length
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint, const boost::array < char,
        MAX_BUFFER_SIZE > & recvbuffer, const size_t recvlen)
{
        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t buffer_size = 0;
        thread_data_ptr session_data;
        char *buffer1 = NULL;
        char *buffer2 = NULL;
        std::string str_value;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        //parameter check
        if (recvlen > recvbuffer.size()) {
                std::cerr << "protocol_module_simple::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
                boost::format formatter("Data size bigger than buffer size. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100079, formatter.str(), __FILE__, __LINE__);
                return FINALIZE;
        }

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end()
                                     || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100080, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                receive_data_it = session_data->receive_data_map.find(rs_endpoint);
                if (receive_data_it == session_data->receive_data_map.end()) {
                        receive_data recv_data;
                        session_data->receive_data_map[rs_endpoint] = recv_data;
                }

                session_data->target_endpoint = rs_endpoint;

                receive_data &recv_data = session_data->receive_data_map[rs_endpoint];

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                //status list check
                it = std::find_if(it, it_end, data_send_ok());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending data is not correct. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100081, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //status list check
                it = recv_data.send_status_list.begin();
                it = std::adjacent_find(it, it_end, data_send_repeated());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending data is not correct. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100082, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                it = recv_data.send_status_list.begin();
                //get original status info
                while (it != it_end) {
                        //item status is SEND_END
                        if (it->status == SEND_END) {
                                //erase from list
                                recv_data.send_status_list.erase(it++);
                                continue;
                        }
                        //item status is SEND_CONTINUE
                        else if (it->status == SEND_CONTINUE) {
                                it->send_offset += it->send_end_size;
                                data_remain_start = it->send_offset;
                                break;
                        }
                        //item status is SEND_NG
                        else {
                                data_remain_start = it->send_offset;
                                data_remain_size = it->unsend_size;
                                break;
                        }

                        ++it;
                }
                //receive buffer process
                //buffer rest size < request size
                if (recv_data.receive_buffer_rest_size < recvlen) {
                        //buffer max size < remain size + request size
                        //buffer is need reallocate
                        if (recv_data.receive_buffer_max_size < data_remain_size + recvlen) {
                                //the buffer's size that will be allocated is exceed the upper limit value
                                if (MAX_SIMPLE_MODULE_BUFFER_SIZE < data_remain_size + recvlen) {
                                        std::cerr << "protocol_module_simple::handle_realserver_recv() : the buffer's size that will be allocated is exceed the upper limit value." << std::endl;
                                        boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                                        formatter % boost::this_thread::get_id();
                                        putLogError(100083, formatter.str(), __FILE__, __LINE__);

                                        return FINALIZE;
                                }
                                //receive_buffer1's memory allocate and initialization
                                buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                                buffer1 = new char[buffer_size];
                                memset(buffer1, 0, buffer_size);
                                //receive_buffer2's memory allocate and initialization
                                buffer2 = new char[buffer_size];
                                memset(buffer2, 0, buffer_size);

                                //copy data from old buffer to new buffer
                                memcpy(buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                //free old buffer1 and old buffer2
                                if (recv_data.receive_buffer1 != NULL) {
                                        delete[] recv_data.receive_buffer1;
                                        recv_data.receive_buffer1 = NULL;
                                }

                                if (recv_data.receive_buffer2 != NULL) {
                                        delete[] recv_data.receive_buffer2;
                                        recv_data.receive_buffer2 = NULL;
                                }

                                //set new buffer pointer
                                recv_data.receive_buffer1 = buffer1;
                                recv_data.receive_buffer2 = buffer2;
                                recv_data.receive_buffer = recv_data.receive_buffer1;
                                //set new buffer's max size
                                recv_data.receive_buffer_max_size = buffer_size;
                        }
                        //buffer's max size >= remain data size + request size
                        //buffer isn't need reallocate, but switch
                        else {
                                //pointer valid check
                                if (unlikely(recv_data.receive_buffer1 == NULL || recv_data.receive_buffer2 == NULL)) {
                                        boost::format formatter("Invalid pointer. thread id : %d.");
                                        formatter % boost::this_thread::get_id();
                                        putLogError(100084, formatter.str(), __FILE__, __LINE__);
                                        throw - 1;
                                }
                                //using buffer is buffer1
                                if (recv_data.receive_buffer == recv_data.receive_buffer1) {
                                        //buffer2 initialization
                                        memset(recv_data.receive_buffer2, 0, recv_data.receive_buffer_max_size);
                                        //copy data from buffer1 to buffer2
                                        memcpy(recv_data.receive_buffer2, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                        memcpy(recv_data.receive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                                        //set buffer2 as using buffer
                                        recv_data.receive_buffer = recv_data.receive_buffer2;
                                }
                                //using buffer is buffer2
                                else {
                                        //buffer1 initialization
                                        memset(recv_data.receive_buffer1, 0, recv_data.receive_buffer_max_size);
                                        //copy data from buffer2 to buffer1
                                        memcpy(recv_data.receive_buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                        memcpy(recv_data.receive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                        //set buffer1 as using buffer
                                        recv_data.receive_buffer = recv_data.receive_buffer1;
                                }
                        }

                        //set buffer's rest size
                        recv_data.receive_buffer_rest_size = recv_data.receive_buffer_max_size - data_remain_size - recvlen;

                        //remain_size recalc
                        data_remain_size += recvlen;

                        send_status_it it_begin = recv_data.send_status_list.begin();
                        send_status_it it_end = recv_data.send_status_list.end();
                        //offset recalc
                        for (; it_begin != it_end; ++it_begin) {
                                it_begin->send_offset -= data_remain_start;
                        }
                }
                //buffer's rest size >= request size
                //copy directly
                else {
                        //pointer valid check
                        if (unlikely(recv_data.receive_buffer == NULL)) {
                                boost::format formatter("Invalid pointer. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100085, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        //copy data from parameter to using buffer
                        memcpy(recv_data.receive_buffer + recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size,
                               recvbuffer.data(), recvlen);
                        //buffer's rest size recalc
                        recv_data.receive_buffer_rest_size -= recvlen;
                        //remain data size recalc
                        data_remain_size += recvlen;
                }

                it = recv_data.send_status_list.begin();
                it_end = recv_data.send_status_list.end();
                //request rest size initialization
                request_data_remain_size = recvlen;
                //original status process
                for (; it != it_end; ++it) {
                        //status is SEND_CONTINUE
                        if (it->status == SEND_CONTINUE) {
                                //send rest size > request size
                                if (it->send_rest_size > request_data_remain_size) {
                                        //send possible size recalc
                                        it->send_possible_size = request_data_remain_size;
                                        //send rest size recalc
                                        it->send_rest_size -= request_data_remain_size;
                                        //send end size recalc
                                        it->send_end_size = 0;
                                        //request size recalc
                                        request_data_remain_size = 0;
                                }
                                //send rest size <= request size
                                else {
                                        //send possible size recalc
                                        it->send_possible_size = it->send_rest_size;
                                        //send rest size recalc
                                        request_data_remain_size -= it->send_rest_size;
                                        //send end size recalc
                                        it->send_end_size = 0;
                                        //request size recalc
                                        it->send_rest_size = 0;
                                }
                                //change status from SEND_CONTINUE to SEND_OK
                                it->status = SEND_OK;
                        }
                        //status is SEND_NG
                        else if (it->status == SEND_NG) {
                                        //send_rest_size recalc
                                        it->send_rest_size = it->unsend_size + request_data_remain_size;

                                //recalc fields value according to send_rest_size and request rest size
                                if (it->send_rest_size > it->unsend_size + request_data_remain_size) {
                                        it->send_possible_size = it->unsend_size + request_data_remain_size;
                                        it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                                        it->send_end_size = 0;
                                        it->unsend_size = 0;
                                        request_data_remain_size = 0;
                                } else {
                                        it->send_possible_size = it->send_rest_size;
                                        request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                                        it->send_end_size = 0;
                                        it->unsend_size = 0;
                                        it->send_rest_size = 0;
                                }

                                //change status from SEND_NG to SEND_OK
                                it->status = SEND_OK;
                        }
                        //no request rest data to process
                        if (request_data_remain_size <= 0) {
                                break;
                        }
                }

                //there are still rest data need to process
                //new status created and add to status list
                while (request_data_remain_size > 0) {
                        //new status created
                        send_status new_send_state;
                        new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
                        new_send_state.send_end_size = 0;
                        new_send_state.send_offset = 0;
                        new_send_state.send_possible_size = 0;
                        new_send_state.unsend_size = 0;
                        new_send_state.send_rest_size = 0;
                        //status initialize to SEND_NG
                        new_send_state.status = SEND_NG;
                        //add new status to status_list
                        recv_data.send_status_list.push_back(new_send_state);
                        std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
                        //calc offset
                        new_send_it->send_offset = recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size
                                                   - request_data_remain_size;

                        new_send_it->send_rest_size = request_data_remain_size;

                        //recalc fields value according to send_rest_size and request rest size
                        if (new_send_it->send_rest_size > request_data_remain_size) {
                                new_send_it->send_possible_size = request_data_remain_size;
                                new_send_it->send_rest_size -= request_data_remain_size;
                                new_send_it->send_end_size = 0;
                                new_send_it->send_end_size = 0;
                                request_data_remain_size = 0;
                        } else {
                                new_send_it->send_possible_size = new_send_it->send_rest_size;
                                request_data_remain_size -= new_send_it->send_rest_size;
                                new_send_it->send_end_size = 0;
                                new_send_it->send_rest_size = 0;
                        }

                        //change status from SEND_NG to SEND_OK
                        new_send_it->status = SEND_OK;
                }

                //search for send_possible item in status list
                send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                 data_send_possible());
                //the data that can be sent possible is exist
                if (it_find != recv_data.send_status_list.end()) {
                        status = CLIENT_CONNECTION_CHECK;
                }
                //the data that can be sent possible is not exist
                else {
                        status = REALSERVER_RECV;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}



//! called from after sorryserver receive
//! @param[in]    downstream thread id
//! @param[in]    sorryserver endpoint reference
//! @param[in]    receive from realserver buffer reference.
//! @param[in]    recv data length
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint, const boost::array <
        char, MAX_BUFFER_SIZE > & recvbuffer, const size_t recvlen)
{
        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t buffer_size = 0;
        thread_data_ptr session_data;
        char *buffer1 = NULL;
        char *buffer2 = NULL;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        //parameter check
        if (recvlen > recvbuffer.size()) {
                std::cerr << "protocol_module_simple::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
                boost::format formatter("Data size bigger than buffer size. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100090, formatter.str(), __FILE__,
                            __LINE__);
                return FINALIZE;
        }

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100091, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                receive_data_it = session_data->receive_data_map.find(sorry_endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        receive_data recv_data;
                        session_data->receive_data_map[sorry_endpoint] = recv_data;
                }

                session_data->target_endpoint = sorry_endpoint;

                receive_data &recv_data = session_data->receive_data_map[sorry_endpoint];

                //status list check
                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();
                it = std::find_if(it, it_end, data_send_ok());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending data is invalid. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100092, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //status list check
                it = recv_data.send_status_list.begin();
                it = std::adjacent_find(it, it_end, data_send_repeated());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending data is invalid. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100093, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                it = recv_data.send_status_list.begin();
                //get original status info
                while (it != it_end) {
                        //item status is SEND_END
                        if (it->status == SEND_END) {
                                //erase from list
                                recv_data.send_status_list.erase(it++);
                                continue;
                        }
                        //item status is SEND_CONTINUE
                        else if (it->status == SEND_CONTINUE) {
                                it->send_offset += it->send_end_size;
                                data_remain_start = it->send_offset;
                                break;
                        }
                        //item status is SEND_NG
                        else {
                                data_remain_start = it->send_offset;
                                data_remain_size = it->unsend_size;
                                break;
                        }

                        ++it;
                }
                //receive buffer process
                //buffer rest size < request size
                if (recv_data.receive_buffer_rest_size < recvlen) {
                        //buffer max size < remain size + request size
                        //buffer is need reallocate
                        if (recv_data.receive_buffer_max_size < data_remain_size + recvlen) {
                                //the buffer's size that will be allocated is exceed the upper limit value
                                if (MAX_SIMPLE_MODULE_BUFFER_SIZE < data_remain_size + recvlen) {
                                        std::cerr << "protocol_module_simple::handle_sorryserver_recv() : the buffer's size that will be allocated is exceed the upper limit value." << std::endl;
                                        boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                                        formatter % boost::this_thread::get_id();
                                        putLogError(100094, formatter.str(), __FILE__, __LINE__);
                                        return FINALIZE;
                                }
                                //receive_buffer1's memory allocate and initialization
                                buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                                buffer1 = new char[buffer_size];
                                memset(buffer1, 0, buffer_size);
                                //receive_buffer2's memory allocate and initialization
                                buffer2 = new char[buffer_size];
                                memset(buffer2, 0, buffer_size);

                                //copy data from old buffer to new buffer
                                memcpy(buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                //free old buffer1 and old buffer2
                                if (recv_data.receive_buffer1 != NULL) {
                                        delete[] recv_data.receive_buffer1;
                                        recv_data.receive_buffer1 = NULL;
                                }

                                if (recv_data.receive_buffer2 != NULL) {
                                        delete[] recv_data.receive_buffer2;
                                        recv_data.receive_buffer2 = NULL;
                                }

                                //set new buffer pointer
                                recv_data.receive_buffer1 = buffer1;
                                recv_data.receive_buffer2 = buffer2;
                                recv_data.receive_buffer = recv_data.receive_buffer1;
                                //set new buffer's max size
                                recv_data.receive_buffer_max_size = buffer_size;
                        }
                        //buffer's max size >= remain data size + request size
                        //buffer isn't need reallocate, but switch
                        else {
                                //pointer valid check
                                if (unlikely(recv_data.receive_buffer1 == NULL || recv_data.receive_buffer2 == NULL)) {
                                        boost::format formatter("Invalid pointer. thread id : %d.");
                                        formatter % boost::this_thread::get_id();
                                        putLogError(100095, formatter.str(), __FILE__, __LINE__);
                                        throw - 1;
                                }
                                //using buffer is buffer1
                                if (recv_data.receive_buffer == recv_data.receive_buffer1) {
                                        //buffer2 initialization
                                        memset(recv_data.receive_buffer2, 0, recv_data.receive_buffer_max_size);
                                        //copy data from buffer1 to buffer2
                                        memcpy(recv_data.receive_buffer2, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                        memcpy(recv_data.receive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                                        //set buffer2 as using buffer
                                        recv_data.receive_buffer = recv_data.receive_buffer2;
                                }
                                //using buffer is buffer2
                                else {
                                        //buffer1 initialization
                                        memset(recv_data.receive_buffer1, 0, recv_data.receive_buffer_max_size);
                                        //copy data from buffer2 to buffer1
                                        memcpy(recv_data.receive_buffer1, recv_data.receive_buffer + data_remain_start, data_remain_size);
                                        memcpy(recv_data.receive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                                        //set buffer1 as using buffer
                                        recv_data.receive_buffer = recv_data.receive_buffer1;
                                }
                        }

                        //set buffer's rest size
                        recv_data.receive_buffer_rest_size = recv_data.receive_buffer_max_size - data_remain_size - recvlen;

                        //remain_size recalc
                        data_remain_size += recvlen;

                        send_status_it it_begin = recv_data.send_status_list.begin();
                        send_status_it it_end = recv_data.send_status_list.end();
                        //offset recalc
                        for (; it_begin != it_end; ++it_begin) {
                                it_begin->send_offset -= data_remain_start;
                        }
                }
                //buffer's rest size >= request size
                //copy directly
                else {
                        //pointer valid check
                        if (unlikely(recv_data.receive_buffer == NULL)) {
                                boost::format formatter("Invalid pointer. thread id : %d");
                                formatter % boost::this_thread::get_id();
                                putLogError(100096, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        //copy data from parameter to using buffer
                        memcpy(recv_data.receive_buffer + recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size,
                               recvbuffer.data(), recvlen);
                        //buffer's rest size recalc
                        recv_data.receive_buffer_rest_size -= recvlen;
                        //remain data size recalc
                        data_remain_size += recvlen;
                }

                it = recv_data.send_status_list.begin();
                it_end = recv_data.send_status_list.end();
                //request rest size initialization
                request_data_remain_size = recvlen;
                //original status process
                for (; it != it_end; ++it) {
                        //status is SEND_CONTINUE
                        if (it->status == SEND_CONTINUE) {
                                //
                                if (it->send_rest_size > request_data_remain_size) {
                                        it->send_possible_size = request_data_remain_size;
                                        it->send_rest_size -= request_data_remain_size;
                                        it->send_end_size = 0;
                                        request_data_remain_size = 0;
                                } else {
                                        it->send_possible_size = it->send_rest_size;
                                        request_data_remain_size -= it->send_rest_size;
                                        it->send_end_size = 0;
                                        it->send_rest_size = 0;
                                }

                                //change status from SEND_CONTINUE to SEND_OK
                                it->status = SEND_OK;
                        }
                        //status is SEND_NG
                        else if (it->status == SEND_NG) {
                                it->send_rest_size = it->unsend_size + request_data_remain_size;

                                //recalc fields value according to send_rest_size and request rest size
                                if (it->send_rest_size > it->unsend_size + request_data_remain_size) {
                                        it->send_possible_size = it->unsend_size + request_data_remain_size;
                                        it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                                        it->send_end_size = 0;
                                        it->unsend_size = 0;
                                        request_data_remain_size = 0;
                                } else {
                                        it->send_possible_size = it->send_rest_size;
                                        request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                                        it->send_end_size = 0;
                                        it->unsend_size = 0;
                                        it->send_rest_size = 0;
                                }

                                //change status from SEND_NG to SEND_OK
                                it->status = SEND_OK;
                        }
                        //no request rest data to process
                        if (request_data_remain_size <= 0) {
                                break;
                        }
                }

                //there are still rest data need to process
                //new status created and add to status list
                while (request_data_remain_size > 0) {
                        //new status created
                        send_status new_send_state;
                        new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
                        new_send_state.send_end_size = 0;
                        new_send_state.send_offset = 0;
                        new_send_state.send_possible_size = 0;
                        new_send_state.unsend_size = 0;
                        new_send_state.send_rest_size = 0;
                        //status initialize to SEND_NG
                        new_send_state.status = SEND_NG;
                        //add new status to status_list
                        recv_data.send_status_list.push_back(new_send_state);
                        std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
                        //calc offset
                        new_send_it->send_offset = recv_data.receive_buffer_max_size - recv_data.receive_buffer_rest_size
                                                   - request_data_remain_size;

                        new_send_it->send_rest_size = request_data_remain_size;

                        //recalc fields value according to send_rest_size and request rest size
                        if (new_send_it->send_rest_size > request_data_remain_size) {
                                new_send_it->send_possible_size = request_data_remain_size;
                                new_send_it->send_rest_size -= request_data_remain_size;
                                new_send_it->send_end_size = 0;
                                request_data_remain_size = 0;
                        } else {
                                new_send_it->send_possible_size = new_send_it->send_rest_size;
                                request_data_remain_size -= new_send_it->send_rest_size;
                                new_send_it->send_end_size = 0;
                                new_send_it->send_rest_size = 0;
                        }

                        //change status from SEND_NG to SEND_OK
                        new_send_it->status = SEND_OK;
                }

                //search for send_possible item in status list
                send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                 data_send_possible());
                //the data that can be sent possible is exist
                if (it_find != recv_data.send_status_list.end()) {
                        status = CLIENT_CONNECTION_CHECK;
                }
                //the data that can be sent possible is not exist
                else {
                        status = SORRYSERVER_RECV;
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from UPSTREAM thread. make module original message.
//! @param[in]    downstream thread id.
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_response_send_inform(
        const boost::thread::id thread_id)
{
        return STOP;
}

//! called from after client connection check. use TCP/IP only. create client send message.
//! @param[in]    downstream thread id
//! @param[out]    send buffer reference
//! @param[out]    send data length
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_client_connection_check(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t &datalen)
{
        EVENT_TAG status = FINALIZE;
        size_t send_buffer_size = sendbuffer.max_size();
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100101, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                receive_data_it = session_data->receive_data_map.find(session_data->target_endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100102, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                receive_data &recv_data = receive_data_it->second;

                //get the data that can be sent possible
                send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                            data_send_possible());
                if (unlikely(it == recv_data.send_status_list.end())) {
                        boost::format formatter("Sending possible data is not existed. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100103, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //buffer size >= sending_possible size
                if (send_buffer_size > it->send_possible_size) {
                        //copy data from receive_buffer to sendbuffer by sending_possible size
                        memcpy(sendbuffer.data(), recv_data.receive_buffer + it->send_offset, it->send_possible_size);
                        //send_end_size recalc
                        it->send_end_size = it->send_possible_size;
                        //set copied data length
                        datalen = it->send_possible_size;
                        //sending_possible size recalc
                        it->send_possible_size = 0;
                }
                //buffer size < sending_possible size
                else {
                        //copy data from receive_buffer to sendbuffer by buffer size
                        memcpy(sendbuffer.data(), recv_data.receive_buffer + it->send_offset, send_buffer_size);
                        //send_end_size recalc
                        it->send_end_size = send_buffer_size;
                        //sending_possible size recalc
                        it->send_possible_size -= send_buffer_size;
                        //set copied data length
                        datalen = send_buffer_size;
                }

                status = CLIENT_SEND;
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! called from after client select. use UDP only
//! @param[in]    downstream thread id
//!    @param[in]    client udp endpoint
//! @param[out]    send buffer reference
//! @param[out]    send data length
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_client_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint &cl_endpoint, boost::array < char,
        MAX_BUFFER_SIZE > & sendbuffer, size_t &datalen)
{
        return STOP;
}

//!    called from after client send
//!    @param[in]    downstream thread id
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_client_send(
        const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        //thread_id check
                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100106, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }
                        session_data = session_thread_it->second;
                }
                //endpoint check
                receive_data_it = session_data->receive_data_map.find(session_data->target_endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100107, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                receive_data &recv_data = receive_data_it->second;

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                //check status list
                it = std::adjacent_find(it, it_end, data_send_list_incorrect());
                if (unlikely(it != it_end)) {
                        boost::format formatter("Sending possible data is invalid. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100108, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }
                //status list check
                it = recv_data.send_status_list.begin();
                it = find_if(it, it_end, data_send_ok());
                if (unlikely(it == it_end)) {
                        boost::format formatter("Sending possible data is not existed. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100109, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                //sending possible data is exist
                if (it->send_possible_size > 0) {
                        //status remain SEND_OK
                        it->status = SEND_OK;
                        //offset recalc
                        it->send_offset += it->send_end_size;
                        //send_end_size recalc
                        it->send_end_size = 0;
                }
                //sending possible data is not exist
                else {
                        //can receive from client continue
                        if (it->send_rest_size > 0) {
                                //change status from SEND_OK to SEND_CONTINUE
                                it->status = SEND_CONTINUE;
                        }
                        //can not receive from client continue
                        else {
                                //change status from SEND_OK to SEND_END
                                it->status = SEND_END;
                        }
                }

                it = recv_data.send_status_list.begin();
                it = find_if(it, it_end, data_send_ok());
                //send_ok item is exist
                if (it != it_end) {
                        status = CLIENT_CONNECTION_CHECK;
                }
                //send_ok item is not exist
                else {
                        //end flag is on
                        if (session_data->end_flag == END_FLAG_ON) {
                                status = CLIENT_DISCONNECT;
                        }
                        //end flag is off
                        else {
                                //sorry flag is on
                                if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                        status = SORRYSERVER_RECV;
                                }
                                //sorry flag is off
                                else {
                                        status = REALSERVER_RECV;
                                }
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! call from client disconnect event. use upstream thread and downstream thread.
//! @param[in]    upstream and downstream thread id( check! one thread one event! )
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_client_disconnect(
        const boost::thread::id thread_id)
{
        return FINALIZE;
}

//! call from sorry mode event. use upstream thread and downstream thread
//! @param[in]    upstream and downstream thread id( check! one thread one event and first time call pattern )
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorry_enable(
        const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint endpoint;
        bool send_possible = false;
        bool send_continue = false;
        bool send_disable = false;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        receive_data_map_it receive_data_it;

        try {
                {
                        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
                        //check thread_id
                        session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100112, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }
                //check endpoint
                endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                           : session_data->target_endpoint;
                receive_data_it = session_data->receive_data_map.find(endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        //must be down thread
                        if (unlikely(session_data->thread_division == THREAD_DIVISION_UP_STREAM)) {
                                boost::format formatter("Invalid endpoint. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100113, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }
                        session_data->sorry_flag = SORRY_FLAG_ON;
                        status = SORRYSERVER_RECV;
                } else {
                        receive_data &recv_data = receive_data_it->second;

                        //get this thread sending possible data
                        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                    data_send_possible());
                        if (it != recv_data.send_status_list.end()) {
                                send_possible = true;
                        }

                        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
                        if (it != recv_data.send_status_list.end()) {
                                send_continue = true;
                        }

                        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
                        if (it != recv_data.send_status_list.end()) {
                                send_disable = true;
                        }

                        //up thread
                        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM) {
                                //accept_end_flag is off
                                if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF) {
                                        //set sorry flag on
                                        session_data->sorry_flag = SORRY_FLAG_ON;
                                        status = ACCEPT;
                                }
                                //accept_end_flag is on
                                else {
                                        //set sorry flag on
                                        if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                                if (send_possible) {
                                                        status = SORRYSERVER_CONNECT;
                                                } else {
                                                        status = SORRYSERVER_SELECT;
                                                }

                                        }
                                        //set sorry flag off
                                        else {
                                                //the data that can be sent continue is exist
                                                if (send_continue) {
                                                        //set end flag on
                                                        session_data->end_flag = END_FLAG_ON;
                                                        status = REALSERVER_DISCONNECT;
                                                }
                                                //the data that can be sent continue is not exist
                                                else {
                                                        //set sorryserver_switch_flag on
                                                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
                                                        //set sorry_flag on
                                                        session_data->sorry_flag = SORRY_FLAG_ON;
                                                        status = REALSERVER_DISCONNECT;
                                                }
                                        }
                                }
                        }
                        //down thread
                        else {
                                //sorry_flag is on
                                if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                        //sending possible data is exist
                                        if (send_possible) {
                                                status = CLIENT_CONNECTION_CHECK;
                                        }
                                        //sending possible data is not exist
                                        else {
                                                status = SORRYSERVER_RECV;
                                        }
                                }
                                //sorry_flag is off
                                else {
                                        //set sorry_flag on
                                        session_data->sorry_flag = SORRY_FLAG_ON;
                                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;

                                        //sending NG data is exist or send_rest_size > 0
                                        if (send_disable) {
                                                //set end flag on
                                                session_data->end_flag = END_FLAG_ON;
                                                status = REALSERVER_DISCONNECT;
                                        }
                                        else {
                                                //sending possible data is exist
                                                if (send_possible) {
                                                        status = CLIENT_CONNECTION_CHECK;
                                                }
                                                //sending possible data is not exist
                                                else {
                                                        status = SORRYSERVER_RECV;
                                                }
                                        }
                                }
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! call from sorry mode disable. use upstream thread and downstream thread.
//! @param[in]    upstream and downstream thread id( check! one thread one event )
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorry_disable(
        const boost::thread::id thread_id)
{
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint endpoint;
        bool send_possible = false;
        bool send_disable = false;
        bool send_continue = false;
        thread_data_ptr session_data;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        //check thread_id
                        session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100116, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }
                        //check pointer
                        session_data = session_thread_it->second;
                }
                //check endpoint
                endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                           : session_data->target_endpoint;
                receive_data_map_it receive_data_it = session_data->receive_data_map.find(endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        //must be down thread
                        if (unlikely(session_data->thread_division == THREAD_DIVISION_UP_STREAM)) {
                                boost::format formatter("Invalid endpoint. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100117, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data->sorry_flag = SORRY_FLAG_OFF;
                        status = REALSERVER_RECV;
                } else {
                        receive_data &recv_data = receive_data_it->second;

                        //get this thread sending possible data
                        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                    data_send_possible());
                        if (it != recv_data.send_status_list.end()) {
                                send_possible = true;
                        }

                        //sending NG data is exist or send_rest_size > 0
                        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
                        if (it != recv_data.send_status_list.end()) {
                                send_disable = true;
                        }

                        //the data that can be sent continue is exist
                        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
                        if (it != recv_data.send_status_list.end()) {
                                send_continue = true;
                        }

                        //up thread
                        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM) {
                                //accept_end_flag is off
                                if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF) {
                                        //set sorry flag off
                                        session_data->sorry_flag = SORRY_FLAG_OFF;
                                        status = ACCEPT;
                                }
                                //accept_end_flag is on
                                else {
                                        //sorry flag is on
                                        if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                                //the data that can be sent continue is exist
                                                if (send_continue) {
                                                        //set end flag on
                                                        session_data->end_flag = END_FLAG_ON;
                                                        status = SORRYSERVER_DISCONNECT;
                                                }
                                                //the data that can be sent continue is not exist
                                                else {
                                                        //set realserver_switch_flag on
                                                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
                                                        //set sorry_flag off
                                                        session_data->sorry_flag = SORRY_FLAG_OFF;
                                                        status = SORRYSERVER_DISCONNECT;
                                                }
                                        }
                                        //sorry flag is off
                                        else {
                                                if (send_possible) {
                                                        status = REALSERVER_CONNECT;
                                                } else {
                                                        status = REALSERVER_SELECT;
                                                }
                                        }
                                }
                        }
                        //down thread
                        else {
                                //sorry_flag is on
                                if (session_data->sorry_flag == SORRY_FLAG_ON) {
                                        //set sorry_flag off
                                        session_data->sorry_flag = SORRY_FLAG_OFF;
                                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
                                        //sending NG data is exist or send_rest_size > 0
                                        if (send_disable) {
                                                //set end flag on
                                                session_data->end_flag = END_FLAG_ON;
                                                status = SORRYSERVER_DISCONNECT;
                                        }
                                        else {
                                                //sending possible data is exist
                                                if (send_possible) {
                                                        status = CLIENT_CONNECTION_CHECK;
                                                }
                                                //sending possible data is not exist
                                                else {
                                                        status = REALSERVER_RECV;
                                                }
                                        }
                                }
                                //sorry_flag is off
                                else {
                                        //sending possible data is exist
                                        if (send_possible) {
                                                status = CLIENT_CONNECTION_CHECK;
                                        }
                                        //sending possible data is not exist
                                        else {
                                                status = REALSERVER_RECV;
                                        }
                                }
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! call from realserver disconnect. use upstream thread and downstream thread
//! @param[in]    upstream and downstream thread id( check! one thread one event )
//! @param[in]    disconnected realserver endpoint.
//! @return     session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &rs_endpoint)
{
        EVENT_TAG status = FINALIZE;
        bool possible_flag = false;
        thread_data_ptr session_data;
        boost::asio::ip::tcp::endpoint endpoint;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                        session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100120, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                           : session_data->target_endpoint;
                receive_data_map_it receive_data_it = session_data->receive_data_map.find(endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        return FINALIZE;
                }

                receive_data &recv_data = receive_data_it->second;

                //the data that can be sent possible is exist
                send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                            data_send_possible());
                if (it != recv_data.send_status_list.end()) {
                        possible_flag = true;
                }

                //up thread
                if (session_data->thread_division == THREAD_DIVISION_UP_STREAM) {
                        //end flag is on
                        if (session_data->end_flag == END_FLAG_ON) {
                                status = CLIENT_RECV;
                        }
                        //end flag is off
                        else {
                                //sorryserver_switch_flag is on
                                if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON) {
                                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                                        status = SORRYSERVER_SELECT;
                                }
                                //sorryserver_switch_flag is off
                                else {
                                        //set end flag on
                                        session_data->end_flag = END_FLAG_ON;
                                        status = CLIENT_RECV;
                                }
                        }
                }
                //down thread
                else {
                        if (session_data->end_flag == END_FLAG_ON) {
                                status = CLIENT_DISCONNECT;
                        } else {
                                if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON) {
                                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                                        status = SORRYSERVER_RECV;
                                } else {
                                        //set end flag on
                                        session_data->end_flag = END_FLAG_ON;
                                        status = CLIENT_DISCONNECT;
                                }
                        }

                        //the data that can be sent possible is exist
                        if (possible_flag) {
                                status = CLIENT_CONNECTION_CHECK;
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! call from sorry server disconnect. use upstream thread and downstream thread
//! @param[in]    upstream and downstream thread id( check! one thread one event )
//! @param[in]    disconnect sorryserver endpoint
//! @return        session use EVENT mode
//! @return        session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_simple::handle_sorryserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &sorry_endpoint)
{
        EVENT_TAG status = FINALIZE;
        bool possible_flag = false;
        thread_data_ptr session_data;
        boost::asio::ip::tcp::endpoint endpoint;

        try {
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                        session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                        if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)) {
                                boost::format formatter("Invalid thread id. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100124, formatter.str(), __FILE__, __LINE__);
                                throw - 1;
                        }

                        session_data = session_thread_it->second;
                }

                endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                           : session_data->target_endpoint;
                receive_data_map_it receive_data_it = session_data->receive_data_map.find(endpoint);
                if (unlikely(receive_data_it == session_data->receive_data_map.end())) {
                        boost::format formatter("Invalid endpoint(%s). thread id: %d.");
                        formatter % endpoint % boost::this_thread::get_id();
                        putLogError(100125, formatter.str(), __FILE__, __LINE__);
                        throw - 1;
                }

                receive_data &recv_data = receive_data_it->second;

                //the data that can be sent possible is exist
                send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                            data_send_possible());
                if (it != recv_data.send_status_list.end()) {
                        possible_flag = true;
                }

                //up thread
                if (session_data->thread_division == THREAD_DIVISION_UP_STREAM) {
                        //end flag is on
                        if (session_data->end_flag == END_FLAG_ON) {
                                status = CLIENT_RECV;
                        }
                        //end flag is off
                        else {
                                //realserver_switch_flag is on
                                if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON) {
                                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                                        status = REALSERVER_SELECT;
                                }
                                //realserver_switch_flag is off
                                else {
                                        //set end flag on
                                        session_data->end_flag = END_FLAG_ON;
                                        status = CLIENT_RECV;
                                }
                        }
                }
                //down thread
                else {
                        if (session_data->end_flag == END_FLAG_ON) {
                                status = CLIENT_DISCONNECT;
                        } else {
                                if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON) {
                                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                                        status = REALSERVER_RECV;
                                } else {
                                        session_data->end_flag = END_FLAG_ON;
                                        status = CLIENT_DISCONNECT;
                                }
                        }

                        //the data that can be sent possible is exist
                        if (possible_flag) {
                                status = CLIENT_CONNECTION_CHECK;
                        }
                }
        } catch (...) {
                status = FINALIZE;
        }

        return status;
}

//! call from realserver disconnect. use upstream thread and downstream thread.
//! @param[in]    upstream and downstream thread id( check! one thread one event )
//! @param[in]    disconnect realserver endpoint
//! @return        session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_simple::handle_realserver_close(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint &rs_endpoint)
{
        return STOP;
}

}

extern "C" l7vs::protocol_module_base*
create_module()
{
        return dynamic_cast<l7vs::protocol_module_base *>(new l7vs::protocol_module_simple());
}

extern "C" void
destroy_module(l7vs::protocol_module_base *in)
{
        delete in;
}
