/*
 * @file  ip_replication_data_processor_test.cpp
 * @brief ip replication data processor test file.
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

#include "ip_to_be_test_file.h"

using namespace boost::unit_test;
using namespace l7vs;

#define SECTION_NUMBER    200
#define STRUCT_NUMBER    256

bool is_lock_function_called = false;
bool is_unlock_function_called = false;
bool is_put_into_temp_list_test_thread4_waiting = false;
bool is_put_into_temp_list_test_thread6_waiting = false;
bool is_put_into_temp_list_test_thread8_waiting = false;
bool is_put_into_temp_list_test_thread9_waiting = false;
bool is_put_into_temp_list_test_thread10_waiting = false;

// function for testing register_replication_area_lock
void lock_function()
{
        is_lock_function_called = true;
}

// function for testing register_replication_area_unlock
void unlock_function()
{
        is_unlock_function_called = true;
}

LOG_LEVEL_TAG replication_getloglevel()
{
        return LOG_LV_NONE;
}

void replication_putLogFatal(const unsigned int id, const std::string &message, const char *file_name, int line_number)
{
}

void replication_putLogError(const unsigned int id, const std::string &message, const char *file_name, int line_number)
{
}

void replication_putLogWarn(const unsigned int id, const std::string &message, const char *file_name, int line_number)
{
}

void replication_putLogInfo(const unsigned int id, const std::string &message, const char *file_name, int line_number)
{
}

void replication_putLogDebug(const unsigned int id, const std::string &message, const char *file_name, int line_number)
{
}
l7vs::ip_replication_data_processor::getloglevel_func_type replication_ingetloglevel = replication_getloglevel;
l7vs::ip_replication_data_processor::logger_func_type replication_inputLogFatal = replication_putLogFatal;
l7vs::ip_replication_data_processor::logger_func_type replication_inputLogError = replication_putLogError;
l7vs::ip_replication_data_processor::logger_func_type replication_inputLogWarn = replication_putLogWarn;
l7vs::ip_replication_data_processor::logger_func_type replication_inputLogInfo = replication_putLogInfo;
l7vs::ip_replication_data_processor::logger_func_type replication_inputLogDebug = replication_putLogDebug;

static int calc_hash(const boost::asio::ip::tcp::endpoint &cl_endpoint)
{
        unsigned int hash = 0;
        if (cl_endpoint.address().is_v4()) {
                hash = cl_endpoint.address().to_v4().to_ulong() * GOLDEN_RATIO_PRIME;
        } else {
                boost::asio::ip::address_v6::bytes_type v6_bytes = cl_endpoint.address().to_v6().to_bytes();
                boost::asio::ip::address_v4::bytes_type v4_bytes = {{v6_bytes[12], v6_bytes[13], v6_bytes[14], v6_bytes[15]}};
                boost::asio::ip::address_v4::address_v4 v4_address = boost::asio::ip::address_v4::address_v4(v4_bytes);
                hash = v4_address.to_ulong() * GOLDEN_RATIO_PRIME;
        }
        return hash >> (32 - HASH_TABLE_BITS);
}

// test class
class ip_replication_data_processor_test_class : public l7vs::ip_replication_data_processor
{
public:
        boost::mutex check_mutex;
        // ip_replication_data_processor_test_class
        ip_replication_data_processor_test_class(
                char *ip_replication_area_begain,
                int ip_replication_area_size,
                const boost::asio::ip::tcp::endpoint &virtual_service_endpoint,
                getloglevel_func_type    ingetloglevel,
                logger_func_type inputLogFatal,
                logger_func_type inputLogError,
                logger_func_type inputLogWarn,
                logger_func_type inputLogInfo,
                logger_func_type inputLogDebug) :
                ip_replication_data_processor(
                        ip_replication_area_begain,
                        ip_replication_area_size,
                        virtual_service_endpoint,
                        ingetloglevel,
                        inputLogFatal,
                        inputLogError,
                        inputLogWarn,
                        inputLogInfo,
                        inputLogDebug) {
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread1(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread2(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'U';
                test_data1.rs_endpoint = endpoint;
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread3(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread4(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;

                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                is_put_into_temp_list_test_thread4_waiting = true;
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread5() {

                l7vs::ip_replication_temp_data temp_data;
                this->get_from_temp_list(temp_data);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread6(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;

                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                is_put_into_temp_list_test_thread6_waiting = true;
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread7() {

                l7vs::ip_replication_temp_data temp_data;

                for (int i = 0; i < 4; i++) {
                        this->get_from_temp_list(temp_data);
                }
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread8(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;

                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data1);
                is_put_into_temp_list_test_thread8_waiting = true;
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread9(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;

                is_put_into_temp_list_test_thread9_waiting = true;
                this->put_into_temp_list(test_data1);
        }

        // put_into_temp_list test thread
        void put_into_temp_list_test_thread10(int ip_hash,
        boost::asio::ip::tcp::endpoint &endpoint, time_t time) {

                l7vs::ip_replication_temp_data test_data1;

                test_data1.ip_hash = ip_hash;
                test_data1.last_time = time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;

                is_put_into_temp_list_test_thread10_waiting = true;
                this->put_into_temp_list(test_data1);
        }

        // get_from_temp_list test thread
        void get_from_temp_list_thread1() {
                boost::asio::ip::tcp::endpoint endpoint;
                l7vs::ip_replication_temp_data get_data;
                this->get_from_temp_list(get_data);
                {
                        boost::mutex::scoped_lock sclock(check_mutex);
                        BOOST_CHECK_EQUAL(get_data.ip_hash, 111);
                        BOOST_CHECK_EQUAL(get_data.op_code, 'A');
                        BOOST_CHECK_EQUAL(get_data.rs_endpoint, endpoint);
                        BOOST_CHECK_EQUAL(get_data.last_time, 1000);
                }
        }

        // ip_replication_data_processor
        void ip_replication_data_processor_test(
                l7vs::ip_replication_data *expecting_replication_area,
                boost::asio::ip::tcp::endpoint expecting_virtual_service_endpoint,
                char *replication_data_area, int data_area_size, char *header_start_address,
                char *real_data_start_address) {

                boost::asio::ip::tcp::endpoint test_virtual_service_endpoint;
                test_virtual_service_endpoint.port(999);
                l7vs::ip_replication_data_header *first_header = reinterpret_cast<ip_replication_data_header *>(header_start_address);

                cout << "[1]--------------------------------------------- " << endl;
                // unit_test[1] メンバー変数が初期化されるチェックする。
                BOOST_CHECK_EQUAL(this->temp_list.size(), 0u);
                BOOST_CHECK_EQUAL(this->replication_area, expecting_replication_area);
                BOOST_CHECK_EQUAL(this->virtual_service_endpoint, expecting_virtual_service_endpoint);
                BOOST_CHECK_EQUAL(this->getloglevel, replication_getloglevel);
                BOOST_CHECK_EQUAL(this->putLogFatal, replication_putLogFatal);
                BOOST_CHECK_EQUAL(this->putLogError, replication_putLogError);
                BOOST_CHECK_EQUAL(this->putLogWarn, replication_putLogWarn);
                BOOST_CHECK_EQUAL(this->putLogInfo, replication_putLogInfo);
                BOOST_CHECK_EQUAL(this->putLogDebug, replication_putLogDebug);

                cout << "[2]--------------------------------------------- " << endl;
                // unit_test[2] replication_data_area が NULLの場合, 例外が発生しない。
                try {
                        ip_replication_data_processor test_object1(
                                NULL, SECTION_NUMBER, test_virtual_service_endpoint, replication_ingetloglevel,
                                replication_inputLogFatal, replication_inputLogError,
                                replication_inputLogWarn, replication_inputLogInfo,
                                replication_inputLogDebug);
                } catch (...) {
                        BOOST_ERROR("exception: ip_replication_data_processor");
                }

                cout << "[3]--------------------------------------------- " << endl;
                // unit_test[3] ip_replication_area_size < 0, 例外が発生する。
                bool exception_occured = false;
                try {
                        memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                        for (int i = 0; i < 128; i++) {
                                l7vs::ip_replication_data_header *temp = reinterpret_cast<ip_replication_data_header *>(replication_data_area) + i;
                                temp->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                        }
                        l7vs::ip_replication_data_processor test_object2(
                                replication_data_area, -1, test_virtual_service_endpoint,
                                replication_ingetloglevel, replication_inputLogFatal,
                                replication_inputLogError, replication_inputLogWarn,
                                replication_inputLogInfo, replication_inputLogDebug);
                } catch (...) {
                        exception_occured = true;
                }
                BOOST_CHECK(exception_occured);

                cout << "[4]--------------------------------------------- " << endl;
                // unit_test[4] ip_replication_area_sizeが0の場合, 例外が発生しない。
                try {
                        memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                        for (int i = 0; i < 128; i++) {
                                l7vs::ip_replication_data_header *temp = reinterpret_cast<ip_replication_data_header *>(replication_data_area) + i;
                                temp->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                        }
                        l7vs::ip_replication_data_processor test_object2(
                                replication_data_area, 0, test_virtual_service_endpoint,
                                replication_ingetloglevel, replication_inputLogFatal,
                                replication_inputLogError, replication_inputLogWarn,
                                replication_inputLogInfo, replication_inputLogDebug);
                } catch (...) {
                        BOOST_ERROR("exception: ip_replication_data_processor");
                }

                cout << "[5]--------------------------------------------- " << endl;
                // unit_test[5] virtual service が存在して、且つ一つvirtual serviceを追加して、追加したvirtual serviceのサイズをチェックして、offsetが正しいことを確認する場合、
                // unit_test[5] virtual service対応のreplicationデータヘッダーの情報を再設定する。
                std::string virtual_service_ip = "192.168.120.102";
                memset(replication_data_area, 0, data_area_size);
                test_virtual_service_endpoint.address(boost::asio::ip::address::from_string("192.168.120.103"));
                test_virtual_service_endpoint.port(80);
                memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                first_header->virtualserver_port = 80;
                first_header->size = 256 * sizeof(l7vs::ip_replication_data);
                first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                l7vs::ip_replication_data_processor test_object3(
                        replication_data_area,
                        SECTION_NUMBER, test_virtual_service_endpoint,
                        replication_ingetloglevel, replication_inputLogFatal,
                        replication_inputLogError, replication_inputLogWarn,
                        replication_inputLogInfo, replication_inputLogDebug);
                // the virtual service's size is resetted, check it
                size_t expecting_size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                size_t expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address + first_header->size);
                ip_replication_data *expecting_replication_data_area = reinterpret_cast<ip_replication_data *>(header_start_address + first_header->offset);
                BOOST_CHECK_EQUAL((first_header + 1)->size, expecting_size);
                BOOST_CHECK_EQUAL((first_header + 1)->offset, expecting_offset);
                BOOST_CHECK_EQUAL(this->replication_area, expecting_replication_area);

                cout << "[6]--------------------------------------------- " << endl;
                // unit_test[6] virtual service が存在して、且つvirtual serviceのデータエリアが実際サイズより小さいの場合
                // unit_test[6] 処理を行わない。
                virtual_service_ip = "192.168.120.102";
                memset(replication_data_area, 0, data_area_size);
                test_virtual_service_endpoint.address(boost::asio::ip::address::from_string("192.168.120.103"));
                test_virtual_service_endpoint.port(80);
                memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                first_header->virtualserver_port = 80;
                first_header->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                unsigned old_port = first_header->virtualserver_port;
                std::string old_virtual_service_ip(first_header->virtualserver_ip);
                size_t old_size = first_header->size;
                size_t old_offset = first_header->offset;
                ip_replication_data_header temp_old_data;
                memcpy(&temp_old_data, first_header, sizeof(ip_replication_data_header));
                l7vs::ip_replication_data_processor temp_test_object(
                        replication_data_area, 20,
                        test_virtual_service_endpoint, replication_ingetloglevel,
                        replication_inputLogFatal, replication_inputLogError,
                        replication_inputLogWarn, replication_inputLogInfo,
                        replication_inputLogDebug);
                // first_header not changed check
                std::string new_virtual_service_ip(first_header->virtualserver_ip);
                BOOST_CHECK_EQUAL(new_virtual_service_ip, old_virtual_service_ip);
                BOOST_CHECK_EQUAL(first_header->virtualserver_port, old_port);
                BOOST_CHECK_EQUAL(first_header->size, old_size);
                BOOST_CHECK_EQUAL(first_header->offset, old_offset);
                int compare_result = memcmp(first_header, &temp_old_data, sizeof(ip_replication_data_header));
                BOOST_CHECK_EQUAL(compare_result, 0);

                cout << "[7]--------------------------------------------- " << endl;
                // unit_test[7] virtual service対応のreplicationデータヘッダーの情報を再設定して、ヘッダーエリアがfullの場合,処理を行わない。
                try {
                        memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                        for (int i = 0; i < 128; i++) {
                                l7vs::ip_replication_data_header *temp = reinterpret_cast<ip_replication_data_header *>(replication_data_area) + i;
                                temp->size = 256 * sizeof(l7vs::ip_replication_data);
                        }
                        memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                        first_header->virtualserver_port = 80;
                        first_header->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                        first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                        l7vs::ip_replication_data_processor test_object4(
                                replication_data_area,
                                SECTION_NUMBER, test_virtual_service_endpoint,
                                replication_ingetloglevel, replication_inputLogFatal,
                                replication_inputLogError, replication_inputLogWarn,
                                replication_inputLogInfo, replication_inputLogDebug);
                } catch (...) {
                        BOOST_ERROR("exception: ip_replication_data_processor");
                }

                cout << "[8]--------------------------------------------- " << endl;
                // unit_test[8] virtual service対応のreplicationデータヘッダーの情報を再設定する時, ヘッダーエリアがfullでない場合
                // unit_test[8] replicationデータヘッダーの情報を変更する。
                memset(replication_data_area, 0, data_area_size);
                memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                first_header->virtualserver_port = 80;
                first_header->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                l7vs::ip_replication_data_processor test_object5(
                        replication_data_area,
                        SECTION_NUMBER, test_virtual_service_endpoint,
                        replication_ingetloglevel, replication_inputLogFatal,
                        replication_inputLogError, replication_inputLogWarn,
                        replication_inputLogInfo, replication_inputLogDebug);
                // the virtual service information's size is resized, get it
                expecting_size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                // the virtual service information's offset is resized, get it
                expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address + first_header->size);
                expecting_replication_data_area = reinterpret_cast<ip_replication_data *>(header_start_address + first_header->offset);
                // size check
                BOOST_CHECK_EQUAL((first_header + 1)->size, expecting_size);
                // offset check
                BOOST_CHECK_EQUAL((first_header + 1)->offset, expecting_offset);
                BOOST_CHECK_EQUAL(this->replication_area, expecting_replication_area);

                cout << "[9]--------------------------------------------- " << endl;
                // unit_test[9] virtual service対応のreplicationデータヘッダーが存在しなくて、且つヘッダーエリアがfullでなく、且つ
                // unit_test[9] replicationデータエリアの領域が足りない場合、処理を行わない。
                try {
                        memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                        std::string temp_virtual_service_ip = "255.255.255.255";
                        test_virtual_service_endpoint.address(boost::asio::ip::address::from_string("192.168.120.104"));
                        test_virtual_service_endpoint.port(90);
                        for (int i = 0; i < 127; i++) {
                                l7vs::ip_replication_data_header *temp = reinterpret_cast<ip_replication_data_header *>(replication_data_area) + i;
                                temp->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                                temp->virtualserver_port = 80;
                                memcpy(temp->virtualserver_ip, temp_virtual_service_ip.c_str(), temp_virtual_service_ip.length());
                        }
                        ip_replication_data_header old_data;
                        memcpy(&old_data, reinterpret_cast<ip_replication_data_header *>(replication_data_area) + 127, sizeof(ip_replication_data_header));
                        l7vs::ip_replication_data_processor test_object6(
                                replication_data_area,
                                20, test_virtual_service_endpoint,
                                replication_ingetloglevel, replication_inputLogFatal,
                                replication_inputLogError, replication_inputLogWarn,
                                replication_inputLogInfo, replication_inputLogDebug);
                        compare_result = memcmp(reinterpret_cast<ip_replication_data_header *>(replication_data_area) + 127, &old_data, sizeof(ip_replication_data_header));
                        BOOST_CHECK_EQUAL(compare_result, 0);
                } catch (...) {
                        BOOST_ERROR("exception: ip_replication_data_processor");
                }

                cout << "[10]--------------------------------------------- " << endl;
                // unit_test[10] virtual service対応のreplicationデータヘッダーが存在して、且つreplicationデータエリアの領域が足りる場合、
                // unit_test[10] ヘッダーエリアに、virtual serviceを追加する。
                memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                test_virtual_service_endpoint.address(boost::asio::ip::address::from_string("192.168.1.112"));
                test_virtual_service_endpoint.port(80);
                memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                first_header->virtualserver_port = 80;
                first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                first_header->size = 256 * sizeof(ip_replication_data);
                l7vs::ip_replication_data_processor test_object7(
                        replication_data_area,
                        SECTION_NUMBER, test_virtual_service_endpoint,
                        replication_ingetloglevel, replication_inputLogFatal,
                        replication_inputLogError, replication_inputLogWarn,
                        replication_inputLogInfo, replication_inputLogDebug);
                expecting_size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address) + 256 * sizeof(ip_replication_data);
                expecting_replication_data_area = reinterpret_cast<ip_replication_data *>(real_data_start_address);
                // ip information's size check
                BOOST_CHECK_EQUAL((first_header + 1)->size, expecting_size);
                // ip information's offset check
                BOOST_CHECK_EQUAL((first_header + 1)->offset, expecting_offset);
                // replication_area check
                BOOST_CHECK_EQUAL(replication_area, expecting_replication_area);

                cout << "[11]--------------------------------------------- " << endl;
                // unit_test[11] virtual serviceが存在しなくて、且つヘッダーエリアがfullの場合,処理を行わない。
                try {
                        memset(replication_data_area, 0, 128 * sizeof(l7vs::ip_replication_data_header));
                        std::string temp_virtual_service_ip = "255.255.255.255";
                        for (int i = 0; i < 128; i++) {
                                l7vs::ip_replication_data_header *temp = reinterpret_cast<ip_replication_data_header *>(replication_data_area) + i;
                                temp->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                                memcpy(temp->virtualserver_ip, temp_virtual_service_ip.c_str(), temp_virtual_service_ip.length());
                        }
                        l7vs::ip_replication_data_processor test_object8(
                                replication_data_area,
                                SECTION_NUMBER, test_virtual_service_endpoint,
                                replication_ingetloglevel, replication_inputLogFatal,
                                replication_inputLogError, replication_inputLogWarn,
                                replication_inputLogInfo, replication_inputLogDebug);
                } catch (...) {
                        BOOST_ERROR("exception: ip_replication_data_processor");
                }

                cout << "[12]--------------------------------------------- " << endl;
                // unit_test[12] virtual serviceのipがipv6で、且つ一つvirtual serviceを追加する場合
                // unit_test[12] virtual service対応のreplicationデータヘッダーの情報を再設定する。
                virtual_service_ip = "abcd:21d0:8936:4866:eefe:567d:3a4b:1230";
                memset(replication_data_area, 0, data_area_size);
                test_virtual_service_endpoint.address(boost::asio::ip::address::from_string("16.23.45.1"));
                test_virtual_service_endpoint.port(80);
                memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
                first_header->virtualserver_port = 80;
                first_header->size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
                l7vs::ip_replication_data_processor test_object9(
                        replication_data_area,
                        SECTION_NUMBER, test_virtual_service_endpoint,
                        replication_ingetloglevel, replication_inputLogFatal,
                        replication_inputLogError, replication_inputLogWarn,
                        replication_inputLogInfo, replication_inputLogDebug);
                // the virtual service's size is resetted, check it
                expecting_size = static_cast<size_t>(256 * sizeof(l7vs::ip_replication_data));
                expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address + first_header->size);
                expecting_replication_data_area = reinterpret_cast<ip_replication_data *>(real_data_start_address);
                BOOST_CHECK_EQUAL((first_header + 1)->size, expecting_size);
                BOOST_CHECK_EQUAL((first_header + 1)->offset, expecting_offset);
                BOOST_CHECK_EQUAL(this->replication_area, expecting_replication_area);
        }

        // put_into_temp_list_test
        void put_into_temp_list_test() {

                time_t last_time = time(0);
                boost::asio::ip::tcp::endpoint endpoint;
                l7vs::ip_replication_temp_data test_data1;
                l7vs::ip_replication_temp_data test_data2;
                l7vs::ip_replication_temp_data test_data3;

                cout << "[13]--------------------------------------------- " << endl;
                // unit_test[13] １つのアイテムの場合、temp_listにデータを追加する。
                test_data1.ip_hash = 1111;
                test_data1.last_time = last_time;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;
                this->temp_list.clear();
                this->put_into_temp_list(test_data1);
                l7vs::ip_replication_temp_data &put_into_data = this->temp_list.back();
                // the item which put into the temp_list check
                // ip_hash check
                BOOST_CHECK_EQUAL(put_into_data.ip_hash, test_data1.ip_hash);
                // last_time check
                BOOST_CHECK_EQUAL(put_into_data.last_time, test_data1.last_time);
                // op_code check
                BOOST_CHECK_EQUAL(put_into_data.op_code, 'A');
                // rs_endpoint check
                BOOST_CHECK_EQUAL(put_into_data.rs_endpoint, test_data1.rs_endpoint);

                cout << "[14]--------------------------------------------- " << endl;
                // unit_test[14] ２つのアイテムの場合、temp_listにデータを追加する。
                test_data1.ip_hash = 1111;
                test_data1.last_time = last_time;
                test_data1.op_code = 'U';
                test_data1.rs_endpoint = endpoint;
                test_data2.ip_hash = 2222;
                test_data2.last_time = last_time;
                test_data2.op_code = 'A';
                test_data2.rs_endpoint = endpoint;
                this->temp_list.clear();
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data2);
                l7vs::ip_replication_temp_data &first_put_into_data = this->temp_list.front();
                l7vs::ip_replication_temp_data &second_put_into_data = this->temp_list.back();
                // the items which put into the temp_list check
                // ip_hash check
                BOOST_CHECK_EQUAL(first_put_into_data.ip_hash, test_data1.ip_hash);
                BOOST_CHECK_EQUAL(second_put_into_data.ip_hash, test_data2.ip_hash);
                // last_time check
                BOOST_CHECK_EQUAL(first_put_into_data.last_time, test_data1.last_time);
                BOOST_CHECK_EQUAL(second_put_into_data.last_time, test_data2.last_time);
                // op_code check
                BOOST_CHECK_EQUAL(first_put_into_data.op_code, 'U');
                BOOST_CHECK_EQUAL(second_put_into_data.op_code, 'A');
                // rs_endpoint check
                BOOST_CHECK_EQUAL(first_put_into_data.rs_endpoint, test_data1.rs_endpoint);
                BOOST_CHECK_EQUAL(second_put_into_data.rs_endpoint, test_data2.rs_endpoint);

                cout << "[15]--------------------------------------------- " << endl;
                // unit_test[15] ３つのアイテムの場合、temp_listにデータを追加する。
                test_data1.ip_hash = 1111;
                test_data1.last_time = last_time;
                test_data1.op_code = 'U';
                test_data1.rs_endpoint = endpoint;
                test_data2.ip_hash = 222;
                test_data2.last_time = last_time;
                test_data2.op_code = 'A';
                test_data2.rs_endpoint = endpoint;
                test_data3.ip_hash = 333;
                test_data3.last_time = last_time;
                test_data3.op_code = 'A';
                test_data3.rs_endpoint = endpoint;
                this->temp_list.clear();
                this->put_into_temp_list(test_data1);
                this->put_into_temp_list(test_data2);
                this->put_into_temp_list(test_data3);
                first_put_into_data = this->temp_list[0];
                second_put_into_data = this->temp_list[1];
                l7vs::ip_replication_temp_data &third_put_into_data = this->temp_list[2];
                // the items which put into the temp_list check
                // ip_hash check
                BOOST_CHECK_EQUAL(first_put_into_data.ip_hash, test_data1.ip_hash);
                BOOST_CHECK_EQUAL(second_put_into_data.ip_hash, test_data2.ip_hash);
                BOOST_CHECK_EQUAL(third_put_into_data.ip_hash, test_data3.ip_hash);
                // last_time check
                BOOST_CHECK_EQUAL(first_put_into_data.last_time, test_data1.last_time);
                BOOST_CHECK_EQUAL(second_put_into_data.last_time, test_data2.last_time);
                BOOST_CHECK_EQUAL(third_put_into_data.last_time, test_data3.last_time);
                // op_code check
                BOOST_CHECK_EQUAL(first_put_into_data.op_code, 'U');
                BOOST_CHECK_EQUAL(second_put_into_data.op_code, 'A');
                BOOST_CHECK_EQUAL(third_put_into_data.op_code, 'A');
                // rs_endpoint check
                BOOST_CHECK_EQUAL(first_put_into_data.rs_endpoint, test_data1.rs_endpoint);
                BOOST_CHECK_EQUAL(second_put_into_data.rs_endpoint, test_data2.rs_endpoint);
                BOOST_CHECK_EQUAL(third_put_into_data.rs_endpoint, test_data3.rs_endpoint);

                cout << "[16]--------------------------------------------- " << endl;
                // unit_test[16] マルチスレッドの場合、temp_listにデータを正常追加する。
                try {
                        boost::thread_group thread_group;
                        this->temp_list.clear();
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread1,
                                this, 111, endpoint, last_time));
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread2,
                                this, 222, endpoint, last_time));
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread3,
                                this, 333, endpoint, last_time));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }
                BOOST_CHECK_EQUAL(this->temp_list.size(), 3u);
                // temp_list item check
                int count = static_cast<int>(this->temp_list.size());
                for (int i = 0; i < count; i++) {
                        ip_replication_temp_data &temp = this->temp_list[i];
                        if (temp.ip_hash == 111) {
                                BOOST_CHECK_EQUAL(temp.op_code, 'A');
                                BOOST_CHECK_EQUAL(temp.rs_endpoint, endpoint);
                                BOOST_CHECK_EQUAL(temp.last_time, last_time);
                        } else if (temp.ip_hash == 222) {
                                BOOST_CHECK_EQUAL(temp.op_code, 'U');
                                BOOST_CHECK_EQUAL(temp.rs_endpoint, endpoint);
                                BOOST_CHECK_EQUAL(temp.last_time, last_time);
                        } else if (temp.ip_hash == 333) {
                                BOOST_CHECK_EQUAL(temp.op_code, 'A');
                                BOOST_CHECK_EQUAL(temp.rs_endpoint, endpoint);
                                BOOST_CHECK_EQUAL(temp.last_time, last_time);
                        } else {
                                BOOST_ERROR("error: put_into_temp_list");
                        }
                }

                cout << "[17]--------------------------------------------- " << endl;
                // unit_test[17] マルチスレッド　がつ　temp_listのサイズが max_temp_listの場合、temp_listにデータを正常追加する。
                try {
                        boost::thread_group thread_group;
                        is_put_into_temp_list_test_thread4_waiting = false;
                        this->temp_list.clear();
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread4,
                                this, 111, endpoint, last_time));
                        while (!is_put_into_temp_list_test_thread4_waiting) {
                        }
                        thread_group.create_thread(boost::bind(&ip_replication_data_processor_test_class::put_into_temp_list_test_thread5, this));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }
                BOOST_CHECK_EQUAL(this->temp_list.size(), 5u);
                // temp_list item check
                count = static_cast<int>(this->temp_list.size());
                for (int i = 0; i < count; i++) {
                        ip_replication_temp_data &temp = this->temp_list[i];
                        BOOST_CHECK_EQUAL(temp.ip_hash, 111);
                        BOOST_CHECK_EQUAL(temp.rs_endpoint, endpoint);
                        BOOST_CHECK_EQUAL(temp.last_time, last_time);
                        BOOST_CHECK_EQUAL(temp.op_code, 'A');
                }

                cout << "[18]--------------------------------------------- " << endl;
                // unit_test[18] ルチスレッド　がつ　temp_list size > max_temp_listの場合、temp_listにデータを正常追加する。
                try {
                        boost::thread_group thread_group;
                        is_put_into_temp_list_test_thread6_waiting = false;
                        this->temp_list.clear();
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread6,
                                this, 111, endpoint, last_time));
                        while (!is_put_into_temp_list_test_thread6_waiting) {
                        }
                        thread_group.create_thread(boost::bind(&ip_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }
                BOOST_CHECK_EQUAL(this->temp_list.size(), 4u);
                // temp_list item check
                count = static_cast<int>(this->temp_list.size());
                for (int i = 0; i < count; i++) {
                        ip_replication_temp_data &temp = this->temp_list[i];
                        BOOST_CHECK_EQUAL(temp.ip_hash, 111);
                        BOOST_CHECK_EQUAL(temp.rs_endpoint, endpoint);
                        BOOST_CHECK_EQUAL(temp.last_time, last_time);
                        BOOST_CHECK_EQUAL(temp.op_code, 'A');
                }

                cout << "[19]--------------------------------------------- " << endl;
                // unit_test[19] temp_listがfullで、且１つのスレッドが待ち状態の場合、waitingフラグを待ちに設定する。
                try {
                        boost::thread_group thread_group;

                        is_put_into_temp_list_test_thread8_waiting = false;
                        this->temp_list.clear();

                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread8,
                                this, 111, endpoint, last_time));

                        while (!is_put_into_temp_list_test_thread8_waiting) {
                        }
                        // put_into_temp_list_test_thread8 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);

                        thread_group.create_thread(boost::bind(&ip_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }

                cout << "[20]--------------------------------------------- " << endl;
                // unit_test[20] temp_listがfullで、且２つのスレッドが待ち状態の場合、waitingフラグを待ちに設定する。
                try {
                        boost::thread_group thread_group;

                        is_put_into_temp_list_test_thread8_waiting = false;
                        is_put_into_temp_list_test_thread9_waiting = false;
                        this->temp_list.clear();

                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread8,
                                this, 111, endpoint, last_time));

                        while (!is_put_into_temp_list_test_thread8_waiting) {
                        }
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread9,
                                this, 111, endpoint, last_time));

                        while (!is_put_into_temp_list_test_thread9_waiting) {
                        }

                        // put_into_temp_list_test_thread8 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);
                        // put_into_temp_list_test_thread9 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread9_waiting, true);

                        thread_group.create_thread(boost::bind(&ip_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }

                cout << "[21]--------------------------------------------- " << endl;
                // unit_test[21] temp_listがfullで、且３つのスレッドが待ち状態の場合、waitingフラグを待ちに設定する。
                try {
                        boost::thread_group thread_group;

                        is_put_into_temp_list_test_thread8_waiting = false;
                        is_put_into_temp_list_test_thread9_waiting = false;
                        is_put_into_temp_list_test_thread10_waiting = false;
                        this->temp_list.clear();

                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread8,
                                this, 111, endpoint, last_time));
                        while (!is_put_into_temp_list_test_thread8_waiting) {
                        }
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread9,
                                this, 111, endpoint, last_time));
                        thread_group.create_thread(boost::bind(
                                &ip_replication_data_processor_test_class::put_into_temp_list_test_thread10,
                                this, 111, endpoint, last_time));
                        while (!is_put_into_temp_list_test_thread9_waiting) {
                        }
                        while (!is_put_into_temp_list_test_thread10_waiting) {
                        }

                        // put_into_temp_list_test_thread8 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);
                        // put_into_temp_list_test_thread9 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread9_waiting, true);
                        // put_into_temp_list_test_thread10 is waiting check
                        BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread10_waiting, true);

                        thread_group.create_thread(boost::bind(&ip_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
                        thread_group.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: put_into_temp_list");
                }
        }

        // write_replicaion_area_test
        void write_replicaion_area_test() {

                {
                        cout << "[22]--------------------------------------------- " << endl;
                        // unit_test[22] replication_area が NULLの場合、例外が発生しない。
                        ip_replication_data *old_replication_area = this->replication_area;
                        try {
                                this->temp_list.clear();
                                this->replication_area = NULL;
                                this->replication_area_lock = lock_function;
                                this->replication_area_unlock = unlock_function;
                                this->write_replication_area();
                        } catch (...) {
                                BOOST_ERROR("exception: write_replication_area");
                        }
                        this->replication_area = old_replication_area;
                }

                {
                        cout << "[23]--------------------------------------------- " << endl;
                        // unit_test[23] temp_listが空の場合、例外が発生しない。
                        try {
                                this->temp_list.clear();
                                this->replication_area_lock = lock_function;
                                this->replication_area_unlock = unlock_function;
                                this->write_replication_area();
                        } catch (...) {
                                BOOST_ERROR("exception: write_replication_area");
                        }

                }

                {
                        cout << "[24]--------------------------------------------- " << endl;
                        // unit_test[24] op_codeが「A」で、且つreplication_areaに１つのデータが存在する場合、新データを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        time_t time2 = time(NULL) - 2;
                        struct ip_replication_data replication_data1 = { "172.16.58.66", 80, time1, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.168.58.66:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("172.168.58.67:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        int ip_hash2 = calc_hash(cl_endpoint2);
                        struct ip_replication_temp_data temp_data2 = { 'A', ip_hash2, time2, rs_endpoint2 };
                        this->temp_list.push_back(temp_data2);
                        this->replication_area[ip_hash1] = replication_data1;
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash2].realserver_ip, rs_endpoint2.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].last_time, time2);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].valid, 1);
                }

                {
                        cout << "[25]--------------------------------------------- " << endl;
                        // unit_test[25] op_codeが「A」で、且つreplication_areaに２つのデータが存在して、且つ１つ目データのvalidフラグが０の場合、新のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        time_t time2 = time(NULL) - 2;
                        time_t time3 = time(NULL) - 3;
                        struct ip_replication_data replication_data1 = { "172.16.58.66", 80, time1, 1 };
                        struct ip_replication_data replication_data2 = { "172.16.58.67", 80, time2, 0 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.168.58.66:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("172.168.58.67:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("172.168.58.68:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        int ip_hash2 = calc_hash(cl_endpoint2);
                        int ip_hash3 = calc_hash(cl_endpoint3);
                        struct ip_replication_temp_data temp_data3 = { 'A', ip_hash3, time3, rs_endpoint3 };
                        this->temp_list.push_back(temp_data3);
                        this->replication_area[ip_hash1] = replication_data1;
                        this->replication_area[ip_hash2] = replication_data2;
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash3].realserver_ip, rs_endpoint3.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].last_time, time3);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].valid, 1);
                }

                {
                        cout << "[26]--------------------------------------------- " << endl;
                        // unit_test[26] op_codeが「A」で、且つreplication_areaにデータが存在しない場合、新のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[27]--------------------------------------------- " << endl;
                        // unit_test[27] op_codeが「U」で、且つセッションIDが存在している場合、該当データを更新する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1_new = time(NULL) - 1;
                        time_t time1_old = time(NULL) - 10;
                        struct ip_replication_data replication_data1 = { "172.16.58.66", 80, time1_old, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'U', ip_hash1, time1_new, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->replication_area[ip_hash1] = replication_data1;
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1_new);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[28]--------------------------------------------- " << endl;
                        // unit_test[28] op_codeが「A」,「U」以外の場合、データを変更しない。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        time_t time2 = time(NULL) - 2;
                        time_t time3 = time(NULL) - 3;
                        time_t time4 = time(NULL) - 4;
                        struct ip_replication_data replication_data1 = { "172.16.58.66", 80, time1, 1 };
                        struct ip_replication_data replication_data2 = { "172.16.58.67", 80, time2, 1 };
                        struct ip_replication_data replication_data3 = { "172.16.58.68", 80, time3, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
                        boost::asio::ip::tcp::endpoint cl_endpoint4 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.103:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.67:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.68:80");
                        boost::asio::ip::tcp::endpoint rs_endpoint4 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.69:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        int ip_hash2 = calc_hash(cl_endpoint2);
                        int ip_hash3 = calc_hash(cl_endpoint3);
                        int ip_hash4 = calc_hash(cl_endpoint4);
                        struct ip_replication_temp_data temp_data = { 'Z', ip_hash4, time4, rs_endpoint4 };
                        this->temp_list.push_back(temp_data);
                        this->replication_area[ip_hash1] = replication_data1;
                        this->replication_area[ip_hash2] = replication_data2;
                        this->replication_area[ip_hash3] = replication_data3;
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash2].realserver_ip, rs_endpoint2.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].last_time, time2);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash2].valid, 1);
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash3].realserver_ip, rs_endpoint3.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].realserver_port, (unsigned short)80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].last_time, time3);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash3].valid, 1);
                }

                {
                        cout << "[29]--------------------------------------------- " << endl;
                        // unit_test[29] データを追加するの場合、replication_area_lock関数を呼び出す。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();
                        is_lock_function_called = false;

                        this->replication_area_lock = lock_function;
                        this->replication_area_unlock = unlock_function;
                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK(is_lock_function_called);
                }

                {
                        cout << "[30]--------------------------------------------- " << endl;
                        // unit_test[30] データを追加するの場合、replication_area_unlock関数を呼び出す。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();
                        is_unlock_function_called = false;

                        this->replication_area_lock = lock_function;
                        this->replication_area_unlock = unlock_function;
                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK(is_unlock_function_called);
                }

                {
                        cout << "[31]--------------------------------------------- " << endl;
                        // unit_test[31] endpointがipv6で、replicationエリアにデータがなくて、該当ipv6のendpointを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"), 80);
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[32]--------------------------------------------- " << endl;
                        // unit_test[32] op_codeが「U」で、endpointがipv6で、replicationエリアにデータがある場合、該当ipv6のendpointを更新する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1_old = time(NULL) - 1;
                        time_t time1_new = time(NULL) - 2;
                        struct ip_replication_data replication_data1 = { "abcd:21d0:8936:4866:eefe:567d:3a4b:1230", 80, time1_old, 1};
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"), 80);
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'U', ip_hash1, time1_new, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->replication_area[ip_hash1] = replication_data1;
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 80);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1_new);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[33]--------------------------------------------- " << endl;
                        // unit_test[33] op_codeが「A」で、ipが"0.0.0.0"で、該当のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("0.0.0.0:0");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("0.0.0.0:0");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[34]--------------------------------------------- " << endl;
                        // unit_test[34] op_codeが「A」で、ipが"255.255.255.255"で、該当のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1 = time(NULL) - 1;
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("255.255.255.255:65535");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("255.255.255.255:65535");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'A', ip_hash1, time1, rs_endpoint1 };
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 65535);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[35]--------------------------------------------- " << endl;
                        // unit_test[35] op_codeが「U」で、ipが"0.0.0.0"で、該当のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1_old = time(NULL) - 1;
                        time_t time1_new = time(NULL) - 2;
                        struct ip_replication_data replication_data1 = { "0.0.0.0", 0, time1_old, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("0.0.0.0:0");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("0.0.0.0:0");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'U', ip_hash1, time1_new, rs_endpoint1 };
                        this->replication_area[ip_hash1] = replication_data1;
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1_new);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[36]--------------------------------------------- " << endl;
                        // unit_test[36] op_codeが「U」で、ipが"255.255.255.255"で、該当のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1_old = time(NULL) - 1;
                        time_t time1_new = time(NULL) - 2;
                        struct ip_replication_data replication_data1 = { "255.255.255.255", 65535, time1_old, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("255.255.255.255:65535");
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("255.255.255.255:65535");
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'U', ip_hash1, time1_new, rs_endpoint1 };
                        this->replication_area[ip_hash1] = replication_data1;
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 65535);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1_new);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }

                {
                        cout << "[37]--------------------------------------------- " << endl;
                        // unit_test[37] op_codeが「U」で、ipが"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"で、該当のデータを追加する。
                        memset(this->replication_area, 0, 256 * sizeof(ip_replication_data));
                        this->temp_list.clear();

                        time_t time1_old = time(NULL) - 1;
                        time_t time1_new = time(NULL) - 2;
                        struct ip_replication_data replication_data1 = { "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 65535, time1_old, 1 };
                        boost::asio::ip::tcp::endpoint cl_endpoint1 = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"), 65535);
                        boost::asio::ip::tcp::endpoint rs_endpoint1 = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"), 65535);
                        int ip_hash1 = calc_hash(cl_endpoint1);
                        struct ip_replication_temp_data temp_data = { 'U', ip_hash1, time1_new, rs_endpoint1 };
                        this->replication_area[ip_hash1] = replication_data1;
                        this->temp_list.push_back(temp_data);
                        this->write_replication_area();
                        BOOST_CHECK_EQUAL(strcmp(this->replication_area[ip_hash1].realserver_ip, rs_endpoint1.address().to_string().c_str()), 0);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].realserver_port, 65535);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].last_time, time1_new);
                        BOOST_CHECK_EQUAL(this->replication_area[ip_hash1].valid, 1);
                }
        }

        // register_replication_area_lock_test
        void register_replication_area_lock_test() {

                boost::function<void(void)> register_function = &lock_function;

                cout << "[38]--------------------------------------------- " << endl;
                // unit_test[38] register_replication_area_lock（）関数のパラメータがNULLの場合、replication_area_lockがNULLある。
                this->register_replication_area_lock(NULL);
                BOOST_CHECK_EQUAL(this->replication_area_lock.empty(), true);

                cout << "[39]--------------------------------------------- " << endl;
                // unit_test[39] register_replication_area_lock（）関数のパラメータがNULLでない場合、replication_area_lockがパラメータと一致する。
                this->register_replication_area_lock(register_function);
                // function registered correctly check;
                BOOST_CHECK_EQUAL(this->replication_area_lock, lock_function);
        }

        // register_replication_area_unlock_test
        void register_replication_area_unlock_test() {

                boost::function<void(void)> register_function = &unlock_function;

                cout << "[40]--------------------------------------------- " << endl;
                // unit_test[40] register_replication_area_unlock（）関数のパラメータがNULLの場合、replication_area_unlockがNULLある。
                this->register_replication_area_unlock(NULL);
                // function registered correctly check;
                BOOST_CHECK_EQUAL(this->replication_area_unlock.empty(), true);

                cout << "[41]--------------------------------------------- " << endl;
                // unit_test[41] register_replication_area_unlock（）関数のパラメータがNULLでない場合、replication_area_unlockがパラメータと一致する。
                this->register_replication_area_unlock(register_function);
                // function registered correctly check;
                BOOST_CHECK_EQUAL(this->replication_area_unlock, unlock_function);
        }

        // get_from_temp_list_test
        void get_from_temp_list_test() {

                int result = 10;
                l7vs::ip_replication_temp_data test_data1;
                l7vs::ip_replication_temp_data test_data2;
                l7vs::ip_replication_temp_data get_data;
                boost::asio::ip::tcp::endpoint endpoint;

                cout << "[42]--------------------------------------------- " << endl;
                // unit_test[42] temp_listのサイズが0の場合、戻り値が失敗（-１）で設定する。
                this->temp_list.clear();
                result = this->get_from_temp_list(get_data);
                BOOST_CHECK_EQUAL(result, -1);

                cout << "[43]--------------------------------------------- " << endl;
                // unit_test[43] temp_listのサイズが1の場合、戻り値が正常（0）で設定する。
                test_data1.ip_hash = 111;
                test_data1.op_code = 'A';
                test_data1.rs_endpoint = endpoint;
                test_data1.last_time = 1000;
                this->temp_list.clear();
                this->temp_list.push_back(test_data1);
                result = this->get_from_temp_list(get_data);
                BOOST_CHECK_EQUAL(result, 0);

                cout << "[44]--------------------------------------------- " << endl;
                // unit_test[44] temp_listのサイズが3の場合、戻り値が正常（0）で設定する。
                this->temp_list.clear();
                this->temp_list.push_back(test_data1);
                this->temp_list.push_back(test_data1);
                this->temp_list.push_back(test_data1);
                result = this->get_from_temp_list(get_data);
                BOOST_CHECK_EQUAL(result, 0);

                cout << "[45]--------------------------------------------- " << endl;
                // unit_test[45] マルチスレッドの場合、temp_listにデータを正常取得する。
                try {
                        this->temp_list.clear();
                        this->temp_list.push_back(test_data1);
                        this->temp_list.push_back(test_data1);
                        this->temp_list.push_back(test_data1);
                        this->temp_list.push_back(test_data1);
                        boost::thread_group thread_group1;
                        thread_group1.create_thread(boost::bind(&ip_replication_data_processor_test_class::get_from_temp_list_thread1, this));
                        thread_group1.create_thread(boost::bind(&ip_replication_data_processor_test_class::get_from_temp_list_thread1, this));
                        thread_group1.create_thread(boost::bind(&ip_replication_data_processor_test_class::get_from_temp_list_thread1, this));
                        thread_group1.join_all();
                } catch (...) {
                        BOOST_ERROR("exception: get_from_temp_list");
                }

                cout << "[46]--------------------------------------------- " << endl;
                // unit_test[46] temp_listにデータがある場合、１つ目のデータを削除する。
                test_data1.ip_hash = 111;
                test_data1.op_code = 'U';
                test_data1.rs_endpoint = endpoint;
                test_data1.last_time = time(0);
                test_data2.ip_hash = 222;
                test_data2.op_code = 'A';
                test_data2.rs_endpoint = endpoint;
                test_data2.last_time = time(0);
                this->temp_list.clear();
                this->temp_list.push_back(test_data1);
                this->temp_list.push_back(test_data2);
                this->get_from_temp_list(get_data);
                l7vs::ip_replication_temp_data &leave_data = this->temp_list.back();
                // first item removed check,so only one item leave, check it
                BOOST_CHECK_EQUAL(this->temp_list.size(), 1u);
                // ip check
                BOOST_CHECK_EQUAL(leave_data.ip_hash, test_data2.ip_hash);
                // op_code check
                BOOST_CHECK_EQUAL(leave_data.op_code, test_data2.op_code);
                // realserver_addr check
                BOOST_CHECK_EQUAL(leave_data.rs_endpoint, test_data2.rs_endpoint);
                // last_time check
                BOOST_CHECK_EQUAL(leave_data.last_time, test_data2.last_time);

                cout << "[47]--------------------------------------------- " << endl;
                // unit_test[47] temp_listに１つデータがある場合、get_from_temp_list（）で取得したデータがtemp_listの内容と一致する。
                test_data1.op_code = 'A';
                this->temp_list.clear();
                this->temp_list.push_back(test_data1);
                this->get_from_temp_list(get_data);
                // get data check
                // ip check
                BOOST_CHECK_EQUAL(get_data.ip_hash, test_data1.ip_hash);
                // op_code check
                BOOST_CHECK_EQUAL(get_data.op_code, test_data1.op_code);
                // realserver_addr check
                BOOST_CHECK_EQUAL(get_data.rs_endpoint, test_data1.rs_endpoint);
                // last_time check
                BOOST_CHECK_EQUAL(get_data.last_time, test_data1.last_time);

                cout << "[48]--------------------------------------------- " << endl;
                // unit_test[48] temp_listに２つデータがある場合、get_from_temp_list（）で取得したデータがtemp_listの１つ目の内容と一致する。
                test_data1.op_code = 'U';
                test_data2.op_code = 'A';
                this->temp_list.clear();
                this->temp_list.push_back(test_data1);
                this->temp_list.push_back(test_data2);
                this->get_from_temp_list(get_data);
                // got data check
                // ip check
                BOOST_CHECK_EQUAL(get_data.ip_hash, test_data1.ip_hash);
                // op_code check
                BOOST_CHECK_EQUAL(get_data.op_code, test_data1.op_code);
                // realserver_addr check
                BOOST_CHECK_EQUAL(get_data.rs_endpoint, test_data1.rs_endpoint);
                // last_time check
                BOOST_CHECK_EQUAL(get_data.last_time, test_data1.last_time);
        }

        // get_replication_area_test
        void get_replication_area_test(ip_replication_data *expecting_ip_replication_data) {

                cout << "[49]--------------------------------------------- " << endl;
                // unit_test[49] get_replication_area（）関数の戻り値はコンストラクタが正常に生成する値と一致する。
                ip_replication_data *get_data = this->get_replication_area();
                BOOST_CHECK_EQUAL(get_data, expecting_ip_replication_data);
        }
};

// function ip_replication_data_processor test
void ip_replication_data_processor_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        char *header_start_address = replication_data_area;
        char *real_data_start_address = header_start_address + 128 * sizeof(struct l7vs::ip_replication_data_header);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;
        memset(replication_data_area, 0, data_area_size);
        virtual_service_endpoint.port(999);
        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);
        test_object.ip_replication_data_processor_test(
                reinterpret_cast<l7vs::ip_replication_data *>(real_data_start_address),
                virtual_service_endpoint, replication_data_area, data_area_size,
                header_start_address, real_data_start_address);
        delete []replication_data_area;
}

// function put_into_temp_list test
void put_into_temp_list_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);

        test_object.put_into_temp_list_test();
        delete []replication_data_area;
}

// function write_replicaion_area test
void write_replicaion_area_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);

        test_object.write_replicaion_area_test();
        delete []replication_data_area;
}

// function register_replication_area_lock test
void register_replication_area_lock_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);

        test_object.register_replication_area_lock_test();
        delete []replication_data_area;
}

// function register_replication_area_unlock test
void register_replication_area_unlock_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);

        test_object.register_replication_area_unlock_test();
        delete []replication_data_area;
}

// function get_from_temp_list test
void get_from_temp_list_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);
        l7vs::ip_replication_temp_data test_data1;
        l7vs::ip_replication_temp_data test_data2;
        l7vs::ip_replication_temp_data get_data;
        boost::asio::ip::tcp::endpoint endpoint;

        test_object.get_from_temp_list_test();
        delete []replication_data_area;
}

// get_replication_area_test
void get_replication_area_test()
{

        int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
        char *replication_data_area = new char[data_area_size];
        bool bret = (replication_data_area != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;

        memset(replication_data_area, 0, data_area_size);

        ip_replication_data_processor_test_class test_object(
                replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
                replication_ingetloglevel, replication_inputLogFatal,
                replication_inputLogError, replication_inputLogWarn,
                replication_inputLogInfo, replication_inputLogDebug);

        ip_replication_data *expecting_result;
        expecting_result = reinterpret_cast<ip_replication_data *>(replication_data_area
        + 128 * sizeof(struct l7vs::ip_replication_data_header));

        test_object.get_replication_area_test(expecting_result);
        delete []replication_data_area;
}

void ip_replication_data_processor_test_main()
{
        // create unit test suite
        test_suite *ts = BOOST_TEST_SUITE("ip_replication_data_processor");

        // add test case to test suite
        ts->add(BOOST_TEST_CASE(&ip_replication_data_processor_test));
        ts->add(BOOST_TEST_CASE(&put_into_temp_list_test));
        ts->add(BOOST_TEST_CASE(&write_replicaion_area_test));
        ts->add(BOOST_TEST_CASE(&register_replication_area_lock_test));
        ts->add(BOOST_TEST_CASE(&register_replication_area_unlock_test));
        ts->add(BOOST_TEST_CASE(&get_from_temp_list_test));
        ts->add(BOOST_TEST_CASE(&get_replication_area_test));

        framework::master_test_suite().add(ts);
}
