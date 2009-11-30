/*
 * @file  ip_ip_session_data_processor_test.cpp
 * @brief ip data processor test file.
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

#define SECTION_NUMBER	200
#define GOLDEN_RATIO_PRIME 0x9e370001UL

bool clear_function_return_fail = false;

LOG_LEVEL_TAG getloglevel_test()
{
	return LOG_LV_NONE;
}

void putLogFatal_test(const unsigned int id, const std::string& message, const char* file_name, int line_number) {}
void putLogError_test(const unsigned int id, const std::string& message, const char* file_name, int line_number) {}
void putLogWarn_test(const unsigned int id, const std::string& message, const char* file_name, int line_number) {}
void putLogInfo_test(const unsigned int id, const std::string& message, const char* file_name, int line_number) {}
void putLogDebug_test(const unsigned int id, const std::string& message, const char* file_name, int line_number) {}

l7vs::ip_replication_data_processor::getloglevel_func_type ingetloglevel1 = getloglevel_test;
l7vs::ip_replication_data_processor::logger_func_type inputLogFatal1 = putLogFatal_test;
l7vs::ip_replication_data_processor::logger_func_type inputLogError1 = putLogError_test;
l7vs::ip_replication_data_processor::logger_func_type inputLogWarn1 = putLogWarn_test;
l7vs::ip_replication_data_processor::logger_func_type inputLogInfo1 = putLogInfo_test;
l7vs::ip_replication_data_processor::logger_func_type inputLogDebug1 = putLogDebug_test;
l7vs::ip_session_data_processor::getloglevel_func_type ingetloglevel2 = getloglevel_test;
l7vs::ip_session_data_processor::logger_func_type inputLogFatal2 = putLogFatal_test;
l7vs::ip_session_data_processor::logger_func_type inputLogError2 = putLogError_test;
l7vs::ip_session_data_processor::logger_func_type inputLogWarn2 = putLogWarn_test;
l7vs::ip_session_data_processor::logger_func_type inputLogInfo2 = putLogInfo_test;
l7vs::ip_session_data_processor::logger_func_type inputLogDebug2 = putLogDebug_test;

class ip_replication_data_processor_replacement: public ip_replication_data_processor{
public:
	ip_replication_data_processor_replacement(
	            char* ip_replication_area_begain,
	            int ip_replication_area_size,
	            const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
	            getloglevel_func_type	ingetloglevel,
	            logger_func_type inputLogFatal,
	            logger_func_type inputLogError,
	            logger_func_type inputLogWarn,
	            logger_func_type inputLogInfo,
	            logger_func_type inputLogDebug ) :
			ip_replication_data_processor(
	                ip_replication_area_begain,
	                ip_replication_area_size,
	                virtual_service_endpoint,
	                ingetloglevel,
	                inputLogFatal,
	                inputLogError,
	                inputLogWarn,
	                inputLogInfo,
	                inputLogDebug ) {
		}

	// to call the get_from_temp_list function
	void to_get_from_temp_list( ip_replication_temp_data& data ){
		this->get_from_temp_list(data);
	}

	// to get the temp_list
	std::deque<ip_replication_temp_data>& get_temp_list(){
		return this->temp_list;
	}
	
};

// test class
class ip_session_data_processor_test_class : public ip_session_data_processor{
public:
	boost::mutex check_mutex;
	// ip_session_data_processor_test_class
	ip_session_data_processor_test_class(int timeout,
			ip_replication_data_processor* replication_data_processor,
			getloglevel_func_type ingetloglevel,
			logger_func_type inputLogFatal, logger_func_type inputLogError,
			logger_func_type inputLogWarn, logger_func_type inputLogInfo,
			logger_func_type inputLogDebug) :
				ip_session_data_processor(timeout,
				replication_data_processor, ingetloglevel, inputLogFatal,
				inputLogError, inputLogWarn, inputLogInfo, inputLogDebug) {

	}

	// get_endpoint_from_session_data_test thread
	void get_endpoint_from_session_data_thread(
            int ip_hash,
            boost::asio::ip::tcp::endpoint& expecting_endpoint,
	    int ret){

		int result = 10;
		boost::asio::ip::tcp::endpoint get_endpoint;

		result = this->get_endpoint_from_session_data(ip_hash, get_endpoint);
		{
			boost::mutex::scoped_lock sclock(check_mutex);
			BOOST_CHECK_EQUAL(result, ret);
			BOOST_CHECK_EQUAL(get_endpoint, expecting_endpoint);
		}
	}

	// ip_session_data_processor_test
	void ip_session_data_processor_test(int expecting_timeout, ip_replication_data_processor* expecting_replication_data_processor) {

		cout << "[1]--------------------------------------------- " << endl;
		// unit_test[1] メンバー変数が初期化されるチェックする。
		// timeout check
		BOOST_CHECK_EQUAL(this->timeout, expecting_timeout);
		// replication_data_processor check
		BOOST_CHECK_EQUAL(this->replication_data_processor, expecting_replication_data_processor);
		// getloglevel check
		BOOST_CHECK_EQUAL(this->getloglevel, getloglevel_test);
		// putLogFatal check
		BOOST_CHECK_EQUAL(this->putLogFatal, putLogFatal_test);
		// putLogError check
		BOOST_CHECK_EQUAL(this->putLogError, putLogError_test);
		// putLogWarn check
		BOOST_CHECK_EQUAL(this->putLogWarn, putLogWarn_test);
		// putLogInfo check
		BOOST_CHECK_EQUAL(this->putLogInfo, putLogInfo_test);
		// putLogDebug check
		BOOST_CHECK_EQUAL(this->putLogDebug, putLogDebug_test);

		cout << "[2]--------------------------------------------- " << endl;
		// unit_test[2] timeout < 0の場合, 例外が発生する。
		bool exception_occur = false;
		try{
			l7vs::ip_session_data_processor(-2, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			exception_occur = true;
		}
		BOOST_CHECK(exception_occur);

		cout << "[3]--------------------------------------------- " << endl;
		// unit_test[3] timeout が０の場合, 例外が発生しない。
		try{
			l7vs::ip_session_data_processor(0, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			BOOST_ERROR("exception: ip_session_data_processor");
		}

		cout << "[4]--------------------------------------------- " << endl;
		// unit_test[4] replication_data_processorがNULLの場合, 例外が発生する。
		exception_occur = false;
		try{
			l7vs::ip_session_data_processor(2, NULL,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			exception_occur = true;
		}
		BOOST_CHECK(exception_occur);

	}

	// get_endpoint_from_session_data_test
	void get_endpoint_from_session_data_test(){

		cout << "[5]--------------------------------------------- " << endl;
		// unit_test[5] rs_endpoint と lasttimeが空の場合、戻り値が失敗（１）で設定する。
		{
			int ret = -2;
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint;
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 1);
		}

		cout << "[6]--------------------------------------------- " << endl;
		// unit_test[6] rs_endpointが空で,lasttimeが空でない場合、戻り値が失敗（１）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint;
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret , 1);
		}

		cout << "[7]--------------------------------------------- " << endl;
		// unit_test[7] lasttimeが空で,rs_endpointが空でない場合、戻り値が失敗（１）で設定する。
		{
			int ret = -2;
			time_t nul_time = (time_t) 0;
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.222:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint;
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { nul_time, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 1);
		}

		cout << "[8]--------------------------------------------- " << endl;
		// unit_test[8] １つのデータがsession_tableに存在して、タイムアウトしない場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			this->timeout = 3600;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(ret_endpoint, rs_endpoint);
		}

		cout << "[9]--------------------------------------------- " << endl;
		// unit_test[9] １つのデータがsession_tableに存在して、タイムアウトの場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL) - 11;
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			boost::asio::ip::tcp::endpoint init_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			this->timeout = (time_t)10;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 1);
			BOOST_CHECK_EQUAL(ret_endpoint, init_endpoint);
		}

		cout << "[10]--------------------------------------------- " << endl;
		// unit_test[10] session_tableに3つデータがあり、且つ３つデータが全部タイムアウトしない場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash1 = calc_hash(cl_endpoint1);
			unsigned int ip_hash2 = calc_hash(cl_endpoint2);
			unsigned int ip_hash3 = calc_hash(cl_endpoint3);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash1] = session_entry;
			this->session_table[ip_hash2] = session_entry;
			this->session_table[ip_hash3] = session_entry;
			this->timeout = 3600;
			ret = this->get_endpoint_from_session_data(ip_hash1, ret_endpoint);
			BOOST_CHECK_EQUAL(ret , 0);
			BOOST_CHECK_EQUAL(ret_endpoint, rs_endpoint);
		}

		cout << "[11]--------------------------------------------- " << endl;
		// unit_test[11] session_tableに3つデータがあり、且つ２つデータがタイムアウトしなくて,１つのデータだけタイムアウトの場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			time_t time_out = time(NULL) - 10000;
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash1 = calc_hash(cl_endpoint1);
			unsigned int ip_hash2 = calc_hash(cl_endpoint2);
			unsigned int ip_hash3 = calc_hash(cl_endpoint3);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			struct ip_session_table_entry session_timeout_entry = { time_out, rs_endpoint };
			this->session_table[ip_hash1] = session_entry;
			this->session_table[ip_hash2] = session_entry;
			this->session_table[ip_hash3] = session_timeout_entry;
			this->timeout = 3600;
			ret = this->get_endpoint_from_session_data(ip_hash1, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(ret_endpoint, rs_endpoint);
		}

		cout << "[12]--------------------------------------------- " << endl;
		// unit_test[12] session_tableに3つデータがあり、且つ３つデータが全部タイムアウトの場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash1 = calc_hash(cl_endpoint1);
			unsigned int ip_hash2 = calc_hash(cl_endpoint2);
			unsigned int ip_hash3 = calc_hash(cl_endpoint3);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash1] = session_entry;
			this->session_table[ip_hash2] = session_entry;
			this->session_table[ip_hash3] = session_entry;
			this->timeout = (time_t)0;
			ret = this->get_endpoint_from_session_data(ip_hash1, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(ret_endpoint, rs_endpoint);
		}

		cout << "[13]--------------------------------------------- " << endl;
		// unit_test[13] メンバー関数timeoutが0でなくて、且つ検索対象データがタイムアウトの場合、戻り値が正常（1）で設定する。
		{
			int ret = -2;
			time_t time_out = time(NULL) - 10000;
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { time_out, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			this->timeout = 10;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 1);
		}

		cout << "[14]--------------------------------------------- " << endl;
		// unit_test[14] endpointがipv6で、１つのデータが存在して、且つタイムアウトしない場合、戻り値が正常（０）で設定する。
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"), 1234);
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash = calc_hash(cl_endpoint);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			this->session_table[ip_hash] = session_entry;
			this->timeout = 3600;
			ret = this->get_endpoint_from_session_data(ip_hash, ret_endpoint);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(ret_endpoint, rs_endpoint);
		}

		cout << "[15]--------------------------------------------- " << endl;
		// unit_test[15] マルチスレッドの場合、データを正常取得する。
		{
			time_t now = time(NULL);
			time_t timeout = time(NULL) - 10000;
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint3 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.102:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint null_endpoint;
			boost::asio::ip::tcp::endpoint ret_endpoint;
			unsigned int ip_hash1 = calc_hash(cl_endpoint1);
			unsigned int ip_hash2 = calc_hash(cl_endpoint2);
			unsigned int ip_hash3 = calc_hash(cl_endpoint3);
			struct ip_session_table_entry session_entry = { now, rs_endpoint };
			struct ip_session_table_entry session_timeout_entry = { timeout, rs_endpoint };
			this->session_table[ip_hash1] = session_entry;
			this->session_table[ip_hash2] = session_timeout_entry;
			this->session_table[ip_hash3] = session_timeout_entry;
			this->timeout = 3600;
			boost::thread_group thread_group;
			thread_group.create_thread(boost::bind(&ip_session_data_processor_test_class::get_endpoint_from_session_data_thread, this, ip_hash1, rs_endpoint, 0));
			thread_group.create_thread(boost::bind(&ip_session_data_processor_test_class::get_endpoint_from_session_data_thread, this, ip_hash2, null_endpoint, 1));
			thread_group.create_thread(boost::bind(&ip_session_data_processor_test_class::get_endpoint_from_session_data_thread, this, ip_hash3, null_endpoint, 1));
			thread_group.join_all();
		}

	}

	// write_session_data_test
	void write_session_data_test(){

		cout << "[16]--------------------------------------------- " << endl;
		// unit_test[16] Session_tableが空、endpointが初期値でないの場合、ip_replication_temp_data.op_code='A'、 return 0
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->get_temp_list().clear();
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			int ip_hash = (int)calc_hash(cl_endpoint);
			this->timeout = 3600;
			ret = this->write_session_data(ip_hash, rs_endpoint, now);
			ip_replication_temp_data temp_data;
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->to_get_from_temp_list(temp_data);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash].last_time, now);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash].rs_endpoint, rs_endpoint);
			BOOST_CHECK_EQUAL(temp_data.op_code, 'A');
			BOOST_CHECK_EQUAL(temp_data.ip_hash, ip_hash);
			BOOST_CHECK_EQUAL(temp_data.last_time, now);
			BOOST_CHECK_EQUAL(temp_data.rs_endpoint, rs_endpoint);
		}

		cout << "[17]--------------------------------------------- " << endl;
		// unit_test[17] Session_tableが空、endpointが初期値の場合、ip_replication_temp_data.op_code='U' return 0
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->get_temp_list().clear();
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint;
			int ip_hash = (int)calc_hash(cl_endpoint);
			this->timeout = 3600;
			ret = this->write_session_data(ip_hash, rs_endpoint, now);
			ip_replication_temp_data temp_data;
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->to_get_from_temp_list(temp_data);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash].last_time, now);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash].rs_endpoint, rs_endpoint);
			BOOST_CHECK_EQUAL(temp_data.op_code, 'U');
			BOOST_CHECK_EQUAL(temp_data.ip_hash, ip_hash);
			BOOST_CHECK_EQUAL(temp_data.last_time, now);
			BOOST_CHECK_EQUAL(temp_data.rs_endpoint, rs_endpoint);
		}

		cout << "[18]--------------------------------------------- " << endl;
		// unit_test[18] Session_tableが空でわない、endpointが初期値でないの場合、ip_replication_temp_data.op_code='A' return 0
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->get_temp_list().clear();
			int ret = -2;
			time_t time1 = time(NULL) - 1;
			time_t time2 = time(NULL) - 2;
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint rs_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.67:80");
			int ip_hash1 = (int)calc_hash(cl_endpoint1);
			int ip_hash2 = (int)calc_hash(cl_endpoint2);
			struct ip_session_table_entry session_entry1 = { time1, rs_endpoint1 };
			this->session_table[ip_hash1] = session_entry1;
			this->timeout = 3600;
			ret = this->write_session_data(ip_hash2, rs_endpoint2, time2);
			ip_replication_temp_data temp_data;
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->to_get_from_temp_list(temp_data);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].last_time, time1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].rs_endpoint, rs_endpoint1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].last_time, time2);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].rs_endpoint, rs_endpoint2);
			BOOST_CHECK_EQUAL(temp_data.op_code, 'A');
			BOOST_CHECK_EQUAL(temp_data.ip_hash, ip_hash2);
			BOOST_CHECK_EQUAL(temp_data.last_time, time2);
			BOOST_CHECK_EQUAL(temp_data.rs_endpoint, rs_endpoint2);			
		}

		cout << "[19]--------------------------------------------- " << endl;
		// unit_test[19] Session_tableが空でわない、endpointが初期値の場合、ip_replication_temp_data.op_code='U' return 0
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->get_temp_list().clear();
			int ret = -2;
			time_t time1 = time(NULL) - 1;
			time_t time2 = time(NULL) - 2;
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint rs_endpoint2;
			int ip_hash1 = (int)calc_hash(cl_endpoint1);
			int ip_hash2 = (int)calc_hash(cl_endpoint2);
			struct ip_session_table_entry session_entry1 = { time1, rs_endpoint1 };
			this->session_table[ip_hash1] = session_entry1;
			this->timeout = 3600;
			ret = this->write_session_data(ip_hash2, rs_endpoint2, time2);
			ip_replication_temp_data temp_data;
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->to_get_from_temp_list(temp_data);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].last_time, time1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].rs_endpoint, rs_endpoint1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].last_time, time2);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].rs_endpoint, rs_endpoint2);
			BOOST_CHECK_EQUAL(temp_data.op_code, 'U');
			BOOST_CHECK_EQUAL(temp_data.ip_hash, ip_hash2);
			BOOST_CHECK_EQUAL(temp_data.last_time, time2);
			BOOST_CHECK_EQUAL(temp_data.rs_endpoint, rs_endpoint2);
		}

		cout << "[20]--------------------------------------------- " << endl;
		// unit_test[20] Session_tableが空でわない、エンドポイントが存在するの場合、ip_replication_temp_data.op_code='U' return 0
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->get_temp_list().clear();
			int ret = -2;
			time_t time1 = time(NULL) - 1;
			time_t time2_old = time(NULL) - 2;
			time_t time2_new = time(NULL) - 3;
			boost::asio::ip::tcp::endpoint cl_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.100:1234");
			boost::asio::ip::tcp::endpoint cl_endpoint2 = string_to_endpoint <boost::asio::ip::tcp> ("192.168.1.101:1234");
			boost::asio::ip::tcp::endpoint rs_endpoint1 = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.66:80");
			boost::asio::ip::tcp::endpoint rs_endpoint2_old = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.67:80");
			boost::asio::ip::tcp::endpoint rs_endpoint2_new = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.68:80");
			int ip_hash1 = (int)calc_hash(cl_endpoint1);
			int ip_hash2 = (int)calc_hash(cl_endpoint2);
			struct ip_session_table_entry session_entry1 = { time1, rs_endpoint1 };
			struct ip_session_table_entry session_entry2 = { time2_old, rs_endpoint2_old };
			this->session_table[ip_hash1] = session_entry1;
			this->session_table[ip_hash2] = session_entry2;
			this->timeout = 3600;
			ret = this->write_session_data(ip_hash2, rs_endpoint2_new, time2_new);
			ip_replication_temp_data temp_data;
			(dynamic_cast<ip_replication_data_processor_replacement *>(this->replication_data_processor))->to_get_from_temp_list(temp_data);
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].last_time, time1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash1].rs_endpoint, rs_endpoint1);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].last_time, time2_new);
			BOOST_CHECK_EQUAL(this->session_table[ip_hash2].rs_endpoint, rs_endpoint2_new);
			BOOST_CHECK_EQUAL(temp_data.op_code, 'A');
			BOOST_CHECK_EQUAL(temp_data.ip_hash, ip_hash2);
			BOOST_CHECK_EQUAL(temp_data.last_time, time2_new);
			BOOST_CHECK_EQUAL(temp_data.rs_endpoint, rs_endpoint2_new);
		}

		cout << "[21]--------------------------------------------- " << endl;
		// unit_test[21] ip_hash異常の場合, return 1
		{
			int ret = -2;
			time_t now = time(NULL);
			boost::asio::ip::tcp::endpoint rs_endpoint;
			int ip_hash = 0xFFFFFFFF;
			ret = this->write_session_data(ip_hash, rs_endpoint, now);
			BOOST_CHECK_EQUAL(ret, 1);
		}
  	}

	// read_session_data_from_replication_area_test
	void read_session_data_from_replication_area_test(){
		
		cout << "[22]--------------------------------------------- " << endl;
		// unit_test[22] replication area初期値でないの場合
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			memset(this->replication_data_processor->get_replication_area(), 0, 128 * 256 * sizeof(struct l7vs::ip_replication_data));
			int ret = -2;
			ret = this->read_session_data_from_replication_area(this->replication_data_processor->get_replication_area());
			BOOST_CHECK_EQUAL(ret, 0);
		}

		cout << "[23]--------------------------------------------- " << endl;
		// unit_test[23] replication area初期値の場合
		{
			memset(this->session_table.c_array(), 0, MAX_IP_SESSION_TBL_SIZE);
			memset(this->replication_data_processor->get_replication_area(), 0, 128 * 256 * sizeof(struct l7vs::ip_replication_data));
			int ret = -2;
			time_t time1 = time(NULL) - 1;
			time_t time2 = time(NULL) - 2;
			time_t time3 = time(NULL) - 3;
			struct ip_replication_data replication_data1 = { "172.16.58.66", 1234, time1, 1};
			struct ip_replication_data replication_data2 = { "172.16.58.67", 1235, time2, 1};
			struct ip_replication_data replication_data3 = { "172.16.58.68", 1236, time3, 1};
			this->replication_data_processor->get_replication_area()[0] = replication_data1;
			this->replication_data_processor->get_replication_area()[1] = replication_data2;
			this->replication_data_processor->get_replication_area()[2] = replication_data3;
			ret = this->read_session_data_from_replication_area(this->replication_data_processor->get_replication_area());
			BOOST_CHECK_EQUAL(ret, 0);
			BOOST_CHECK_EQUAL(this->session_table[0].last_time, time1);
			BOOST_CHECK_EQUAL(strcmp(this->session_table[0].rs_endpoint.address().to_string().c_str(), "172.16.58.66"), 0);
			BOOST_CHECK_EQUAL(this->session_table[0].rs_endpoint.port(), (unsigned short)1234);
			BOOST_CHECK_EQUAL(this->session_table[1].last_time, time2);
			BOOST_CHECK_EQUAL(strcmp(this->session_table[1].rs_endpoint.address().to_string().c_str(), "172.16.58.67"), 0);
			BOOST_CHECK_EQUAL(this->session_table[1].rs_endpoint.port(), (unsigned short)1235);
			BOOST_CHECK_EQUAL(this->session_table[2].last_time, time3);
			BOOST_CHECK_EQUAL(strcmp(this->session_table[2].rs_endpoint.address().to_string().c_str(), "172.16.58.68"), 0);
			BOOST_CHECK_EQUAL(this->session_table[2].rs_endpoint.port(), (unsigned short)1236);
		}

		cout << "[24]--------------------------------------------- " << endl;
		// unit_test[24] replication_areaがNULLの場合, return 1
		{
			int ret = -2;
			ret = this->read_session_data_from_replication_area(NULL);
			BOOST_CHECK_EQUAL(ret, -1);
		}
	}

};

// ip_session_data_processor test
void ip_session_data_processor_test(){

	int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
	char *replication_data_area = new char[data_area_size];
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;
	memset(replication_data_area, 0, data_area_size);
	l7vs::ip_replication_data_processor replication_data_processor(
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	ip_session_data_processor_test_class test_object(5,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.ip_session_data_processor_test(5, &replication_data_processor);

	delete []replication_data_area;

}

// get_endpoint_from_session_data test
void get_endpoint_from_session_data_test(){

	int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
	char *replication_data_area = new char[data_area_size];
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	ip_replication_data_processor_replacement replication_data_processor(
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	ip_session_data_processor_test_class test_object(0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.get_endpoint_from_session_data_test();
	delete []replication_data_area;

}

// write_session_data test
void write_session_data_test(){

	int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
	char *replication_data_area = new char[data_area_size];
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	ip_replication_data_processor_replacement replication_data_processor(
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	ip_session_data_processor_test_class test_object(0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.write_session_data_test();
	delete []replication_data_area;

}

// read_session_data_from_replication_area test
void read_session_data_from_replication_area_test(){

	int data_area_size = 128 * sizeof(struct l7vs::ip_replication_data_header) + 128 * 256 * sizeof(struct l7vs::ip_replication_data);
	char *replication_data_area = new char[data_area_size];
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	l7vs::ip_replication_data_processor replication_data_processor(
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	ip_session_data_processor_test_class test_object(0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.read_session_data_from_replication_area_test();

	delete []replication_data_area;

}


void ip_session_data_processor_test_main()
{

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "ip_session_data_processor" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &ip_session_data_processor_test ) );
	ts->add( BOOST_TEST_CASE( &get_endpoint_from_session_data_test ) );
	ts->add( BOOST_TEST_CASE( &write_session_data_test ) );
	ts->add( BOOST_TEST_CASE( &read_session_data_from_replication_area_test ) );

	framework::master_test_suite().add( ts );
}
