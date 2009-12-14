/*
 * @file  ssl_protocol_module_base_test.cpp
 * @brief ssl protocol module base test file.
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

#include "sslid_to_be_test_file.h"

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

class ssl_protocol_module_base_test_class: public ssl_protocol_module_base {
public:
    ssl_protocol_module_base_test_class() : ssl_protocol_module_base("sslid") {
    }

    ~ssl_protocol_module_base_test_class() {
    }

    bool is_tcp() {
        return true;
    }

    bool is_udp() {
        return false;
    }

    void replication_interrupt() {
    }

    void initialize(rs_list_itr_func_type inlist_begin,
                    rs_list_itr_func_type inlist_end,
                    rs_list_itr_next_func_type inlist_next,
                    boost::function<void(void)> inlist_lock,
                    boost::function<void(void)> inlist_unlock) {
    }

    void finalize() {
    }

    bool is_use_sorry() {
        return false;
    }

    void handle_rslist_update() {
    }

    check_message_result check_parameter(const std::vector<std::string>& args) {
        check_message_result result;
        return result;
    }

    check_message_result set_parameter(const std::vector<std::string>& args) {
        check_message_result result;
        return result;
    }

    check_message_result add_parameter(const std::vector<std::string>& args) {
        check_message_result result;
        return result;
    }

    void get_option_info(std::string& option) {
    }

    void register_schedule(tcp_schedule_func_type inschedule) {
    }

    void register_schedule(udp_schedule_func_type inschedule) {
    }

    EVENT_TAG handle_session_initialize(const boost::thread::id up_thread_id,
                                        const boost::thread::id down_thread_id,
                                        const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
                                        const boost::asio::ip::udp::endpoint& client_endpoint_udp) {
        return STOP;
    }

    EVENT_TAG handle_session_finalize(const boost::thread::id up_thread_id,
                                    const boost::thread::id down_thread_id) {
        return STOP;
    }

    EVENT_TAG handle_accept(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_client_recv(const boost::thread::id thread_id,
                                const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen) {
        return STOP;
    }

    EVENT_TAG handle_realserver_select(const boost::thread::id thread_id,
                                    boost::asio::ip::tcp::endpoint& rs_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_realserver_select(const boost::thread::id thread_id,
                                    boost::asio::ip::udp::endpoint& rs_endpoint,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen) {
        return STOP;
    }

    EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id,
                                        boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                        size_t& datalen) {
        return STOP;
    }

    EVENT_TAG handle_realserver_connection_fail(const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& rs_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_realserver_send(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_select(const boost::thread::id thread_id,
                                        boost::asio::ip::tcp::endpoint& sorry_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_connect(const boost::thread::id thread_id,
                                        boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                        size_t& datalen) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_connection_fail(const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& sorry_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_send(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint& rs_endpoint,
                                    const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                    const size_t recvlen) {
        return STOP;
    }

    EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id,
                                    const boost::asio::ip::udp::endpoint& rs_endpoint,
                                    const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                    const size_t recvlen) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_recv(const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint& sorry_endpoint,
                                    const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                    const size_t recvlen) {
        return STOP;
    }

    EVENT_TAG handle_response_send_inform(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_client_connection_check(const boost::thread::id thread_id,
                                            boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                            size_t& datalen) {
        return STOP;
    }

    EVENT_TAG handle_client_select(const boost::thread::id thread_id,
                                boost::asio::ip::udp::endpoint& cl_endpoint,
                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen) {
        return STOP;
    }

    EVENT_TAG handle_client_send(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_client_disconnect(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_sorry_enable(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_sorry_disable(const boost::thread::id thread_id) {
        return STOP;
    }

    EVENT_TAG handle_realserver_disconnect(const boost::thread::id thread_id,
            const boost::asio::ip::tcp::endpoint& rs_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_sorryserver_disconnect(const boost::thread::id thread_id,
            const boost::asio::ip::tcp::endpoint& sorry_endpoint) {
        return STOP;
    }

    EVENT_TAG handle_realserver_close(const boost::thread::id thread_id,
            const boost::asio::ip::udp::endpoint& rs_endpoint) {
        return STOP;
    }

    //get_ssl_session_id
    void get_ssl_session_id_test() {

        std::string session_id;
        int ret = 0;
        char *record_data;

    cout << "[1]------------------------------------------" << endl;
        // unit_test[1] recv_length が 0で, 且つrecord_data が NULLの場合
        // unit_test[1] 戻り値が-1(error)で設定する。
        record_data = NULL;
        ret = this->get_ssl_session_id(NULL, 0, session_id);
        BOOST_CHECK_EQUAL(ret, -1);

    cout << "[2]------------------------------------------" << endl;
        // unit_test[2] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つrecord_data != NULLの場合
        // unit_test[2] 戻り値が0で設定する。
        record_data = new char[HELLO_MSG_HEADER_LENGTH];
        record_data[SESSION_ID_BEGAIN_OFFSET-1] = 0x20;
        ret = this->get_ssl_session_id(record_data, HELLO_MSG_HEADER_LENGTH, session_id);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[3]------------------------------------------" << endl;
        // unit_test[3] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つrecord_data != NULLの場合
        // unit_test[3] 戻り値が1で設定する。
        record_data = new char[HELLO_MSG_HEADER_LENGTH];
        record_data[SESSION_ID_BEGAIN_OFFSET-1] = 0x00;
        ret = this->get_ssl_session_id(record_data, HELLO_MSG_HEADER_LENGTH, session_id);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[4]------------------------------------------" << endl;
        // unit_test[4] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つrecord_data != NULLの場合
        // unit_test[4] 戻り値が0で設定する。
        record_data = new char[HELLO_MSG_HEADER_LENGTH+1];
        record_data[SESSION_ID_BEGAIN_OFFSET-1] = 0x20;
        ret = this->get_ssl_session_id(record_data, HELLO_MSG_HEADER_LENGTH+1, session_id);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[5]------------------------------------------" << endl;
        // unit_test[5] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つrecord_data != NULLの場合
        // unit_test[5] 戻り値が1で設定する。
        record_data = new char[HELLO_MSG_HEADER_LENGTH+1];
        record_data[SESSION_ID_BEGAIN_OFFSET-1] = 0x00;
        ret = this->get_ssl_session_id(record_data, HELLO_MSG_HEADER_LENGTH+1, session_id);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;
    }

    //check_ssl_record_sendable
    void check_ssl_record_sendable_test() {

        bool is_message_from_client;
        char* record_data;
        size_t recv_length;
        size_t all_length;
        bool is_hello_message;
        int ret;

    cout << "[6]------------------------------------------" << endl;
        // unit_test[6] recv_length が 0で, 且つrecord_data が NULLで, 且つis_message_from_client が falseの場合
        // unit_test[6] 戻り値が-1(error)で設定する。
        recv_length = 0u;
        record_data = NULL;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);

    cout << "[7]------------------------------------------" << endl;
        // unit_test[7] 0 < recv_length < 6で, 且つrecord_data != NULLで, 且つis_message_from_client が falseの場合
        // unit_test[7] 戻り値が1(送信不可)で設定する。
        recv_length = 5u;
        record_data = new char[recv_length];
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[8]------------------------------------------" << endl;
        // unit_test[8] recv_length < 6で, 且つrecord_data != NULLで, 且つis_message_from_client が trueの場合
        // unit_test[8] 戻り値が1(送信不可)で設定する。
        recv_length = 5u;
        record_data = new char[recv_length];
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[9]------------------------------------------" << endl;
        // unit_test[9] recv_length が 6で, 且つrecord_data != NULLで, 且つis_message_from_client が trueの場合
        // unit_test[9] 戻り値が-1(異常)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x00;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[10]------------------------------------------" << endl;
        // unit_test[10] recv_length が 6で, 且つrecord_data != NULLで, 且つis_message_from_client が falseの場合
        // unit_test[10] 戻り値が-1(異常)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x00;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[11]------------------------------------------" << endl;
        // unit_test[11] recv_length が 6で, 且つis_message_from_client が trueで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[11] 戻り値が0(送信可能)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[12]------------------------------------------" << endl;
        // unit_test[12] recv_length が 6で, 且つis_message_from_client が trueで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[12] 戻り値が0(送信可能)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x14;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[13]------------------------------------------" << endl;
        // unit_test[13] recv_length が 6で, 且つis_message_from_client が trueで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[13] 戻り値が0(送信可能)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x15;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[14]------------------------------------------" << endl;
        // unit_test[14] recv_length が 6で, 且つis_message_from_client が falseで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[14] 戻り値が0(送信可能)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[15]------------------------------------------" << endl;
        // unit_test[15] recv_length が 6で, 且つis_message_from_client が trueで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[15] 戻り値が0(送信可能)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[16]------------------------------------------" << endl;
        // unit_test[16] recv_length が 6で, 且つis_message_from_client が falseで, record_dataがsslレコード(最小サイズ)で、且つhello messageでない場合
        // unit_test[16] 戻り値が0(送信可能)で設定する
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        all_length = 0u;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[17]------------------------------------------" << endl;
        // unit_test[17] recv_length が 6で, 且つis_message_from_client が trueで ,record_dataがsslレコード(最小サイズ)で、且つhello messageの場合
        // unit_test[17] 戻り値が1(送信不可)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[18]------------------------------------------" << endl;
        // unit_test[18] recv_length が 6で, 且つis_message_from_client が falseで ,record_dataがsslレコード(最小サイズ)で、且つhello messageの場合
        // unit_test[18] 戻り値が1(送信不可)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[19]------------------------------------------" << endl;
        // unit_test[19] recv_length が 6で, 且つis_message_from_client が trueで ,record_dataがsslレコード(最小サイズ)で、且つhello messageの場合
        // unit_test[19] 戻り値が1(送信不可)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[20]------------------------------------------" << endl;
        // unit_test[20] recv_length が 6で,　且つis_message_from_client が falseで ,record_dataがsslレコード(最小サイズ)で、且つhello messageの場合
        // unit_test[20] 戻り値が1(送信不可)で設定する。
        recv_length = 6u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[21]------------------------------------------" << endl;
        // unit_test[21] recv_length > 6で, 且つis_message_from_client が trueで,record_dataがsslレコードでない場合
        // unit_test[21] 戻り値が-1(異常)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x00;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[22]------------------------------------------" << endl;
        // unit_test[22] recv_length > 6で, 且つis_message_from_client が falseで,record_dataがsslレコードでない場合
        // unit_test[22] 戻り値が-1(異常)で設定する
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x00;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[23]------------------------------------------" << endl;
        // unit_test[23] recv_length > 6で, 且つis_message_from_client が trueで ,record_dataがsslレコードで、且つhello messageでない場合
        // unit_test[23] 戻り値が0(送信可能)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        is_hello_message = true;
        is_message_from_client = true;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[24]------------------------------------------" << endl;
        // unit_test[24] recv_length > 6で, 且つis_message_from_client が falseで,record_dataがsslレコードで、且つhello messageでない場合
        // unit_test[24] 戻り値が0(送信可能)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        is_hello_message = true;
        is_message_from_client = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[25]------------------------------------------" << endl;
        // unit_test[25] recv_length > 6で, 且つis_message_from_client が trueで, record_dataがsslレコードで、且つhello messageでない場合
        // unit_test[25] 戻り値が0(送信可能)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        is_hello_message = true;
        is_message_from_client = true;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[26]------------------------------------------" << endl;
        // unit_test[26] recv_length > 6で, 且つis_message_from_client が falseで, record_dataがsslレコードで、且つhello messageでない場合
        // unit_test[26] 戻り値が0(送信可能)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x17;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x00;
        is_hello_message = true;
        is_message_from_client = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[27]------------------------------------------" << endl;
        // unit_test[27] recv_length > 6で, 且つrecv_length < HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_clientが trueで,record_dataがsslレコードで、且つhello messageの場合
        // unit_test[27] 戻り値が1(送信不可)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[28]------------------------------------------" << endl;
        // unit_test[28] recv_length > 6で, 且つrecv_length < HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_clientが falseで,record_dataがsslレコードで、且つhello messageの場合
        // unit_test[28] 戻り値が1(送信不可)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[29]------------------------------------------" << endl;
        // unit_test[29] recv_length > 6で, 且つrecv_length < HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が falseで,record_dataがsslレコードで、且つhello messageの場合
        // unit_test[29] 戻り値が1(送信不可)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[30]------------------------------------------" << endl;
        // unit_test[30] recv_length > 6で, 且つrecv_length < HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が trueで, record_dataがsslレコードで、且つhello messageの場合
        // unit_test[30] 戻り値が1(送信不可)で設定する。
        recv_length = 10u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, 1);
        delete[] record_data;

    cout << "[31]------------------------------------------" << endl;
        // unit_test[31] recv_length が HELLO_MSG_HEADER_LENGTHで,且つis_message_from_client が trueで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[31] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[32]------------------------------------------" << endl;
        // unit_test[32] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が falseで,record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[32] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[33]------------------------------------------" << endl;
        // unit_test[33] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つrecord_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[33] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[34]------------------------------------------" << endl;
        // unit_test[34] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が trueで,record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[34] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[35]------------------------------------------" << endl;
        // unit_test[35] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が falseで,record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[35] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[36]------------------------------------------" << endl;
        // unit_test[36] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[36] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[37]------------------------------------------" << endl;
        // unit_test[37] recv_length が HELLO_MSG_HEADER_LENGTHで, 且つrecord_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[37] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[38]------------------------------------------" << endl;
        // unit_test[38] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[38] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[39]------------------------------------------" << endl;
        // unit_test[39] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[39] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[40]------------------------------------------" << endl;
        // unit_test[40] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[40] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[41]------------------------------------------" << endl;
        // unit_test[41] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[41] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[42]------------------------------------------" << endl;
        // unit_test[42] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[42] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[43]------------------------------------------" << endl;
        // unit_test[43] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[43] 戻り値が-1(異常)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[44]------------------------------------------" << endl;
        // unit_test[44] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[44] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[45]------------------------------------------" << endl;
        // unit_test[45] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[45] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[46]------------------------------------------" << endl;
        // unit_test[46] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[46] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[47]------------------------------------------" << endl;
        // unit_test[47] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[47] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[48]------------------------------------------" << endl;
        // unit_test[48] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[48] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[49]------------------------------------------" << endl;
        // unit_test[49] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[49] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[50]------------------------------------------" << endl;
        // unit_test[50] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[50] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[51]------------------------------------------" << endl;
        // unit_test[51] recv_length が HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[51] 戻り値が0(送信可能)で設定する。
        recv_length = HELLO_MSG_HEADER_LENGTH;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[52]------------------------------------------" << endl;
        // unit_test[52] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が trueで,record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[52] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[53]------------------------------------------" << endl;
        // unit_test[53] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が falseで,record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[53] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_hello_message = true;
        is_message_from_client = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[54]------------------------------------------" << endl;
        // unit_test[54] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が trueで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[54] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[55]------------------------------------------" << endl;
        // unit_test[55] recv_length > HELLO_MSG_HEADER_LENGTHで, 且つis_message_from_client が falseで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[55] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01;
        record_data[9] = 0x00;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[56]------------------------------------------" << endl;
        // unit_test[56] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[56] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[57]------------------------------------------" << endl;
        // unit_test[57] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[57] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[58]------------------------------------------" << endl;
        // unit_test[58] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[58] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[59]------------------------------------------" << endl;
        // unit_test[59] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[59] 戻る値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[60]------------------------------------------" << endl;
        // unit_test[60] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[60] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[61]------------------------------------------" << endl;
        // unit_test[61] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[61] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[62]------------------------------------------" << endl;
        // unit_test[62] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[62] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[63]------------------------------------------" << endl;
        // unit_test[63] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,誤りのhello messageの場合
        // unit_test[63] 戻り値が-1(異常)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = true;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(!is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[64]------------------------------------------" << endl;
        // unit_test[64] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[64] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[65]------------------------------------------" << endl;
        // unit_test[65] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[65] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[66]------------------------------------------" << endl;
        // unit_test[66] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[66] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[67]------------------------------------------" << endl;
        // unit_test[67] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つclient hello messageの場合
        // unit_test[67] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = true;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[68]------------------------------------------" << endl;
        // unit_test[68] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[68] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[69]------------------------------------------" << endl;
        // unit_test[69] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[69] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x01;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[70]------------------------------------------" << endl;
        // unit_test[70] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[70] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[71]------------------------------------------" << endl;
        // unit_test[71] recv_length > HELLO_MSG_HEADER_LENGTHで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[71] 戻り値が0(送信可能)で設定する。
        recv_length = 100u;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[72]------------------------------------------" << endl;
        // unit_test[72] recv_length が MAX_SSLID_BUFFER_SIZEで, record_dataがsslレコードで,且つserver hello messageの場合
        // unit_test[72] 戻り値が0(送信可能)で設定する。
        recv_length = MAX_SSLID_BUFFER_SIZE;
        record_data = new char[recv_length];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x00;
        record_data[3] = 0x00;
        record_data[4] = 0x9e;
        record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
        record_data[9] = 0x03;
        record_data[10] = 0x00;
        record_data[43] = 0x20;
        is_message_from_client = false;
        is_hello_message = false;
        all_length = 0u;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 163u);
        BOOST_CHECK_EQUAL(ret, 0);
        delete[] record_data;

    cout << "[73]------------------------------------------" << endl;
        // unit_test[73] record_data が NULLの場合
        // unit_test[73] 戻り値が-1(異常)で設定する。
        record_data = NULL;
        ret = this->check_ssl_record_sendable(is_message_from_client,
                record_data, 0u, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(ret, -1);
        delete[] record_data;

    cout << "[74]------------------------------------------" << endl;
        // unit_test[74] recv_lengthがSESSION_ID_BEGAIN_OFFSETで, record_dataがsslレコードで,セッションIDがない場合
        // uint_test[74] 戻り値が0(送信可能)で設定する。
        recv_length = SESSION_ID_BEGAIN_OFFSET;
        record_data = new char[SESSION_ID_BEGAIN_OFFSET];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x27;
        record_data[5] = 0x01;
        record_data[43] = 0x00;
        is_message_from_client = true;
        is_hello_message = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
            record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 44u);
        delete[] record_data;

    cout << "[75]------------------------------------------" << endl;
            // unit_test[75] recv_length>SESSION_ID_BEGAIN_OFFSETで, record_dataがsslレコードで,セッションIDがない場合
        // uint_test[75] 戻り値が0(送信可能)で設定する。
        recv_length = SESSION_ID_BEGAIN_OFFSET+1;
        record_data = new char[SESSION_ID_BEGAIN_OFFSET];
        record_data[0] = 0x16;
        record_data[1] = 0x03;
        record_data[2] = 0x01;
        record_data[3] = 0x00;
        record_data[4] = 0x28;
        record_data[5] = 0x01;
        record_data[43] = 0x00;
        is_message_from_client = true;
        is_hello_message = false;
        ret = this->check_ssl_record_sendable(is_message_from_client,
            record_data, recv_length, all_length, is_hello_message);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK(is_hello_message);
        BOOST_CHECK_EQUAL(all_length, 45u);
        delete[] record_data;

    }

    void dump_session_id_test(){
    struct check_condition
    {
        char memory[256];
        size_t memory_size;
        std::string out_string;
    };

    // テスト条件（正常系）：
    check_condition condition_empty_string[12] =
    {
        {"a", 0, ""},
        {"a", 1, "61"},
        {"ab", 2, "6162"},
        {"abc", 3, "616263"},
        {"abcd", 4, "61626364"},
        {"abcde", 5, "6162636465"},
        {"abcdefghijklmnopqrstuvwxyzABCDEF", 32,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A414243444546"},
        {"abcdefghijklmnopqrstuvwxyzABCDEFG", 33,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A41424344454647"},
        {"abcdefghijklmnopqrstuvwxyzABCDEFGH", 34,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A4142434445464748"},
        {"abcdefghijklmnopqrstuvwxyzABCDEFGHI", 35,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A414243444546474849"},
        {"abcdefghijklmnopqrstuvwxyzABCDEFabcdefghijklmnopqrstuvwxyzABCDEF", 64,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A414243444546"
        "6162636465666768696A6B6C6D6E6F707172737475767778797A414243444546"},
        {"abcdefghijklmnopqrstuvwxyzABCDEFabcdefghijklmnopqrstuvwxyzABCDEFG", 65,
        "6162636465666768696A6B6C6D6E6F707172737475767778797A414243444546"
        "6162636465666768696A6B6C6D6E6F707172737475767778797A41424344454647"},
    };

    // unit_test[1]　ヌルポイントのテスト（異常系）
    std::string out_string = "123";
    std::string out_string_src(out_string);
    protocol_module_base::dump_memory(NULL, 100, out_string);
    BOOST_CHECK_EQUAL(out_string, out_string_src);
    std::cout << "out_string = \n" <<  out_string << std::endl;
    std::cout << "unit_test[1]-----------------------------\n" << std::endl;

    // unit_test[2]　ポイントの内容のサイズ　＜　データサイズのテスト（異常系）
    out_string = "";
    char* less_data = "abcdefghij";
    this->dump_session_id(less_data, 65, out_string);
    BOOST_CHECK_EQUAL(out_string.size(), 130u );
    std::cout << "out_string = \n" <<  out_string << std::endl;
    std::cout << "unit_test[2]-----------------------------\n" << std::endl;

    // 正常系テスト
    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 12; j++)
        {
            out_string_src = "";

            if (i == 0)
            {
                out_string = "";
            }
            else
            {
                out_string = "test";
                out_string_src = out_string;
            }
            this->dump_session_id(condition_empty_string[j].memory,
                                              condition_empty_string[j].memory_size,
                                              out_string);
            BOOST_CHECK_EQUAL(out_string, out_string_src + condition_empty_string[j].out_string);

            std::cout << "condition[" << j << "].memory = " <<  condition_empty_string[j].memory << std::endl;
            std::cout << "condition[" << j << "].memory_size = " <<  condition_empty_string[j].memory_size << std::endl;
            std::cout << "condition[" << j << "].out_string = " <<  condition_empty_string[j].out_string << std::endl;

            std::cout << "out_string = " <<  out_string << std::endl;

            if ((j + 12 * i) >= 10)
            {
                std::cout << "unit_test[" << j + 12 * i + 3 << "]------------------------------------\n" << std::endl;
            }
            else
            {
                std::cout << "unit_test[" << j + 12 * i + 3 << "]-------------------------------------\n" << std::endl;
            }
        }
    }
    }
void is_exec_OK_test()
{
    std::cout << "[1]--------------------------------------------- " << std::endl;
    //unit_test[1] SSL以外モード trueを返す
    BOOST_CHECK_EQUAL(this->is_exec_OK(VS_CONTACT_CLASS_NORMAL), true);
    std::cout << "[2]--------------------------------------------- " << std::endl;
    //unit_test[2] SSLモード falseを返す
    BOOST_CHECK_EQUAL(this->is_exec_OK(VS_CONTACT_CLASS_SSL), false);
}

};

void get_ssl_session_id_test() {
    ssl_protocol_module_base_test_class obj;
    obj.get_ssl_session_id_test();
}

void check_ssl_record_sendable_test() {
    ssl_protocol_module_base_test_class obj;
    obj.check_ssl_record_sendable_test();
}

void dump_session_id_test() {
    ssl_protocol_module_base_test_class obj;
    obj.dump_session_id_test();
}

void is_exec_OK_test() {
    ssl_protocol_module_base_test_class obj;
    obj.is_exec_OK_test();
}

void ssl_protocol_module_base_test_main() {
    test_suite* ts = BOOST_TEST_SUITE( "ssl_protocol_module_base_ut" );
    ts->add(BOOST_TEST_CASE( &get_ssl_session_id_test ));
    ts->add(BOOST_TEST_CASE( &check_ssl_record_sendable_test ));
    ts->add(BOOST_TEST_CASE( &dump_session_id_test ));
    ts->add(BOOST_TEST_CASE( &is_exec_OK_test ));
    framework::master_test_suite().add(ts);
}
