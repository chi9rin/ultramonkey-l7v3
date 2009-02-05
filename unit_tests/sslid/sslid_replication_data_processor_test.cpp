/*
 * sslid_replication_data_processor_test.cpp
 *
 *  Created on: Jan 20, 2009
 *      Author: root
 */
#include "sslid_to_be_test_file.h"

using namespace boost::unit_test;
using namespace l7vs;

#define SECTION_NUMBER	200
#define STRUCT_NUMBER	256

bool is_lock_function_called = false;
bool is_unlock_function_called = true;
bool is_put_into_temp_list_test_thread8_waiting = false;
bool is_put_into_temp_list_test_thread9_waiting = false;
bool is_put_into_temp_list_test_thread10_waiting = false;

// function for testing register_replication_area_lock
void lock_function(){
	is_lock_function_called = true;
}

// function for testing register_replication_area_unlock
void unlock_function(){
	is_unlock_function_called = true;
}

LOG_LEVEL_TAG replication_getloglevel(){
	return LOG_LV_NONE;
}

void replication_putLogFatal(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void replication_putLogError(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void replication_putLogWarn(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void replication_putLogInfo(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void replication_putLogDebug(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}
l7vs::sslid_replication_data_processor::getloglevel_func_type replication_ingetloglevel = replication_getloglevel;
l7vs::sslid_replication_data_processor::logger_func_type replication_inputLogFatal = replication_putLogFatal;
l7vs::sslid_replication_data_processor::logger_func_type replication_inputLogError = replication_putLogError;
l7vs::sslid_replication_data_processor::logger_func_type replication_inputLogWarn = replication_putLogWarn;
l7vs::sslid_replication_data_processor::logger_func_type replication_inputLogInfo = replication_putLogInfo;
l7vs::sslid_replication_data_processor::logger_func_type replication_inputLogDebug = replication_putLogDebug;

// test class
class sslid_replication_data_processor_test_class : public l7vs::sslid_replication_data_processor {
public:
	// sslid_replication_data_processor_test_class
	sslid_replication_data_processor_test_class(int maxlist,
            char* sslid_replication_area_begain,
            int sslid_replication_area_size,
            const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
            getloglevel_func_type	ingetloglevel,
            logger_func_type inputLogFatal,
            logger_func_type inputLogError,
            logger_func_type inputLogWarn,
            logger_func_type inputLogInfo,
            logger_func_type inputLogDebug ) :
		sslid_replication_data_processor(maxlist,
                sslid_replication_area_begain,
                sslid_replication_area_size,
                virtual_service_endpoint,
                ingetloglevel,
                inputLogFatal,
                inputLogError,
                inputLogWarn,
                inputLogInfo,
                inputLogDebug ) {
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread1(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time) {

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;
		this->put_into_temp_list(test_data1);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread2(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'U';
		test_data1.realserver_addr = endpoint;
		this->put_into_temp_list(test_data1);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread3(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'D';
		test_data1.realserver_addr = endpoint;
		this->put_into_temp_list(test_data1);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread4(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;

		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread5(){

		l7vs::sslid_replication_temp_data temp_data;
		this->get_from_temp_list(temp_data);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread6(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;

		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread7(){

		l7vs::sslid_replication_temp_data temp_data;

		for(int i = 0; i < 4; i++){
			this->get_from_temp_list(temp_data);
		}
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread8(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;

		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data1);
		is_put_into_temp_list_test_thread8_waiting = true;
		this->put_into_temp_list(test_data1);
		is_put_into_temp_list_test_thread8_waiting = false;
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread9(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;

		is_put_into_temp_list_test_thread9_waiting = true;
		this->put_into_temp_list(test_data1);
		is_put_into_temp_list_test_thread9_waiting = false;
	}

	// put_into_temp_list test thread
	void put_into_temp_list_test_thread10(std::string& session,
			boost::asio::ip::tcp::endpoint& endpoint, time_t time){

		l7vs::sslid_replication_temp_data test_data1;

		test_data1.session_id = session;
		test_data1.last_time = time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;

		is_put_into_temp_list_test_thread10_waiting = true;
		this->put_into_temp_list(test_data1);
		is_put_into_temp_list_test_thread10_waiting = false;
	}

	// get_from_temp_list test thread
	void get_from_temp_list_thread1(){

		l7vs::sslid_replication_temp_data test_data1;
		boost::asio::ip::tcp::endpoint endpoint;

		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;
		test_data1.last_time = time(0);
		this->temp_list.push_back(test_data1);
	}

	// get_from_temp_list test thread
	void get_from_temp_list_thread2() {
		l7vs::sslid_replication_temp_data get_data;
		this->get_from_temp_list(get_data);
	}

	// sslid_replication_data_processor
	void sslid_replication_data_processor_test(int expecting_maxlist,
			l7vs::sslid_replication_data* expecting_replication_area,
			boost::asio::ip::tcp::endpoint expecting_virtual_service_endpoint) {

		// unit_test[1] member variable initialization test
		BOOST_CHECK_EQUAL(this->maxlist, expecting_maxlist);
		BOOST_CHECK_EQUAL(this->max_temp_list, expecting_maxlist * 2);
		BOOST_CHECK_EQUAL(this->temp_list.size(), 0u);
		BOOST_CHECK_EQUAL(this->replication_area, expecting_replication_area);
		BOOST_CHECK_EQUAL(this->getloglevel, replication_getloglevel);
		BOOST_CHECK_EQUAL(this->putLogFatal, replication_putLogFatal);
		BOOST_CHECK_EQUAL(this->putLogError, replication_putLogError);
		BOOST_CHECK_EQUAL(this->putLogWarn, replication_putLogWarn);
		BOOST_CHECK_EQUAL(this->putLogInfo, replication_putLogInfo);
		BOOST_CHECK_EQUAL(this->putLogDebug, replication_putLogDebug);
	}

	// put_into_temp_list_test
	void put_into_temp_list_test(){

		time_t last_time = time(0);
		boost::asio::ip::tcp::endpoint endpoint;
		std::string session_id1 = "test_id123456789abcdefghijklmnop";
		std::string session_id2 = "test_id2abcdefghijklmnop23456789";
		std::string session_id3 = "test_id3333333333333333333333333";
		l7vs::sslid_replication_temp_data test_data1;
		l7vs::sslid_replication_temp_data test_data2;
		l7vs::sslid_replication_temp_data test_data3;

		// unit_test[2] put into temp_list test(put into 1 item)
		test_data1.session_id = session_id1;
		test_data1.last_time = last_time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = endpoint;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		l7vs::sslid_replication_temp_data& put_into_data = this->temp_list.back();
		// the item which put into the temp_list check
		// session_id check
		BOOST_CHECK_EQUAL(put_into_data.session_id, test_data1.session_id);
		// last_time check
		BOOST_CHECK_EQUAL(put_into_data.last_time, test_data1.last_time);
		// op_code check
		BOOST_CHECK_EQUAL(put_into_data.op_code, 'A');
		// realserver_addr check
		BOOST_CHECK_EQUAL(put_into_data.realserver_addr, test_data1.realserver_addr);

		// unit_test[3] put into temp_list test(put into 2 items)
		test_data1.session_id = session_id1;
		test_data1.last_time = last_time;
		test_data1.op_code = 'U';
		test_data1.realserver_addr = endpoint;
		test_data2.session_id = session_id2;
		test_data2.last_time = last_time;
		test_data2.op_code = 'D';
		test_data2.realserver_addr = endpoint;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data2);
		l7vs::sslid_replication_temp_data& first_put_into_data = this->temp_list.front();
		l7vs::sslid_replication_temp_data& second_put_into_data = this->temp_list.back();
		// the items which put into the temp_list check
		// session_id check
		BOOST_CHECK_EQUAL(first_put_into_data.session_id, test_data1.session_id);
		BOOST_CHECK_EQUAL(second_put_into_data.session_id, test_data2.session_id);
		// last_time check
		BOOST_CHECK_EQUAL(first_put_into_data.last_time, test_data1.last_time);
		BOOST_CHECK_EQUAL(second_put_into_data.last_time, test_data2.last_time);
		// op_code check
		BOOST_CHECK_EQUAL(first_put_into_data.op_code, 'U');
		BOOST_CHECK_EQUAL(second_put_into_data.op_code, 'D');
		// realserver_addr check
		BOOST_CHECK_EQUAL(first_put_into_data.realserver_addr, test_data1.realserver_addr);
		BOOST_CHECK_EQUAL(second_put_into_data.realserver_addr, test_data2.realserver_addr);

		// unit_test[4] put into temp_list test(put into 3 items)
		test_data1.session_id = session_id1;
		test_data1.last_time = last_time;
		test_data1.op_code = 'U';
		test_data1.realserver_addr = endpoint;
		test_data2.session_id = session_id2;
		test_data2.last_time = last_time;
		test_data2.op_code = 'D';
		test_data2.realserver_addr = endpoint;
		test_data3.session_id = session_id3;
		test_data3.last_time = last_time;
		test_data3.op_code = 'T';
		test_data3.realserver_addr = endpoint;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		this->put_into_temp_list(test_data2);
		this->put_into_temp_list(test_data3);
		first_put_into_data = this->temp_list[0];
		second_put_into_data = this->temp_list[1];
		l7vs::sslid_replication_temp_data& third_put_into_data = this->temp_list[2];
		// the items which put into the temp_list check
		// session_id check
		BOOST_CHECK_EQUAL(first_put_into_data.session_id, test_data1.session_id);
		BOOST_CHECK_EQUAL(second_put_into_data.session_id, test_data2.session_id);
		BOOST_CHECK_EQUAL(third_put_into_data.session_id, test_data3.session_id);
		// last_time check
		BOOST_CHECK_EQUAL(first_put_into_data.last_time, test_data1.last_time);
		BOOST_CHECK_EQUAL(second_put_into_data.last_time, test_data2.last_time);
		BOOST_CHECK_EQUAL(third_put_into_data.last_time, test_data3.last_time);
		// op_code check
		BOOST_CHECK_EQUAL(first_put_into_data.op_code, 'U');
		BOOST_CHECK_EQUAL(second_put_into_data.op_code, 'D');
		BOOST_CHECK_EQUAL(third_put_into_data.op_code, 'T');
		// realserver_addr check
		BOOST_CHECK_EQUAL(first_put_into_data.realserver_addr, test_data1.realserver_addr);
		BOOST_CHECK_EQUAL(second_put_into_data.realserver_addr, test_data2.realserver_addr);
		BOOST_CHECK_EQUAL(third_put_into_data.realserver_addr, test_data3.realserver_addr);

		// unit_test[5] multi-threads test
		try{
			boost::thread_group thread_group;
			this->temp_list.clear();
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread1,
						this, session_id1, endpoint, last_time));
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread2,
						this, session_id2, endpoint, last_time));
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread3,
						this, session_id3, endpoint, last_time));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}
		BOOST_CHECK_EQUAL(this->temp_list.size(), 3u);
		// temp_list item check
		int count = static_cast<int>(this->temp_list.size());
		for(int i = 0; i < count; i++){
			sslid_replication_temp_data& temp = this->temp_list[i];
			if(temp.op_code == 'A'){
				BOOST_CHECK_EQUAL(temp.session_id, session_id1);
				BOOST_CHECK_EQUAL(temp.realserver_addr, endpoint);
				BOOST_CHECK_EQUAL(temp.last_time, last_time);
			} else if(temp.op_code == 'U'){
				BOOST_CHECK_EQUAL(temp.session_id, session_id2);
				BOOST_CHECK_EQUAL(temp.realserver_addr, endpoint);
				BOOST_CHECK_EQUAL(temp.last_time, last_time);
			} else if(temp.op_code == 'D'){
				BOOST_CHECK_EQUAL(temp.session_id, session_id3);
				BOOST_CHECK_EQUAL(temp.realserver_addr, endpoint);
				BOOST_CHECK_EQUAL(temp.last_time, last_time);
			} else {
				BOOST_ERROR("error: put_into_temp_list");
			}
		}

		// unit_test[6] while temp_list size = max_temp_list, test it
		try{
			boost::thread_group thread_group;
			this->temp_list.clear();
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread4,
						this, session_id1, endpoint, last_time));
			sleep(1);
			thread_group.create_thread(boost::bind(&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread5, this));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}
		BOOST_CHECK_EQUAL(this->temp_list.size(), 5u);
		// temp_list item check
		count = static_cast<int>(this->temp_list.size());
		for(int i = 0; i < count; i++){
			sslid_replication_temp_data& temp = this->temp_list[i];
			BOOST_CHECK_EQUAL(temp.session_id, session_id1);
			BOOST_CHECK_EQUAL(temp.realserver_addr, endpoint);
			BOOST_CHECK_EQUAL(temp.last_time, last_time);
			BOOST_CHECK_EQUAL(temp.op_code, 'A');
		}

		// unit_test[7] while temp_list size > max_temp_list, test it
		try{
			boost::thread_group thread_group;
			this->temp_list.clear();
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread6,
						this, session_id1, endpoint, last_time));
			sleep(1);
			thread_group.create_thread(boost::bind(&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}
		BOOST_CHECK_EQUAL(this->temp_list.size(), 4u);
		// temp_list item check
		count = static_cast<int>(this->temp_list.size());
		for(int i = 0; i < count; i++){
			sslid_replication_temp_data& temp = this->temp_list[i];
			BOOST_CHECK_EQUAL(temp.session_id, session_id1);
			BOOST_CHECK_EQUAL(temp.realserver_addr, endpoint);
			BOOST_CHECK_EQUAL(temp.last_time, last_time);
			BOOST_CHECK_EQUAL(temp.op_code, 'A');
		}

		// unit_test[8] while temp_list is full and one thread is waiting, test
		try{
			boost::thread_group thread_group;

			is_put_into_temp_list_test_thread8_waiting = false;
			this->temp_list.clear();

			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread8,
						this, session_id1, endpoint, last_time));

			sleep(1);
			// put_into_temp_list_test_thread8 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);

			thread_group.create_thread(boost::bind(&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}

		// unit_test[9] while temp_list is full and two threads are waiting, test
		try{
			boost::thread_group thread_group;

			is_put_into_temp_list_test_thread8_waiting = false;
			is_put_into_temp_list_test_thread9_waiting = false;
			this->temp_list.clear();

			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread8,
						this, session_id1, endpoint, last_time));

			sleep(1);

			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread9,
						this, session_id1, endpoint, last_time));

			sleep(1);

			// put_into_temp_list_test_thread8 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);
			// put_into_temp_list_test_thread9 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread9_waiting, true);

			thread_group.create_thread(boost::bind(&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}

		// unit_test[10] while temp_list is full and 3 threads are waiting, test
		try{
			boost::thread_group thread_group;

			is_put_into_temp_list_test_thread8_waiting = false;
			is_put_into_temp_list_test_thread9_waiting = false;
			is_put_into_temp_list_test_thread10_waiting = false;
			this->temp_list.clear();

			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread8,
						this, session_id1, endpoint, last_time));
			sleep(1);
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread9,
						this, session_id1, endpoint, last_time));
			thread_group.create_thread(boost::bind(
						&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread10,
						this, session_id1, endpoint, last_time));
			sleep(1);

			// put_into_temp_list_test_thread8 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread8_waiting, true);
			// put_into_temp_list_test_thread9 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread9_waiting, true);
			// put_into_temp_list_test_thread10 is waiting check
			BOOST_CHECK_EQUAL(is_put_into_temp_list_test_thread10_waiting, true);

			thread_group.create_thread(boost::bind(&sslid_replication_data_processor_test_class::put_into_temp_list_test_thread7, this));
			thread_group.join_all();
		} catch(...){
			BOOST_ERROR("exception: put_into_temp_list");
		}
	}

	// write_replicaion_area_test
	void write_replicaion_area_test(){

		unsigned short port = 999;
		time_t last_time = time(0);
		time_t update_time = last_time - 10;
		time_t delete_time = last_time - 10;
		std::string temp_session_id;
		boost::asio::ip::tcp::endpoint realserver_addr;
		boost::function<void(void)> function_pointer;
		l7vs::sslid_replication_data temp_data;
		l7vs::sslid_replication_temp_data test_data1;

		this->replication_area_lock = lock_function;
		this->replication_area_unlock = unlock_function;

		// unit_test[11] when temp_list is empty, test it
		try {
			this->temp_list.clear();
			boost::thread test_thread1(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
			sleep(1);
			test_thread1.interrupt();
		} catch(...) {
			BOOST_ERROR("exception: write_replicaion_area");
		}

		// unit_test[12] data add test(when data area has one sslip_replication_data)
		temp_session_id = "temp_id1rtrrrrtttttteeeeeeemmmmp";
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		temp_data.last_time = time(0);
		memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
		temp_data.valid = 1;
		memcpy(this->replication_area, &temp_data, sizeof(struct l7vs::sslid_replication_data));
		// set the added data
		realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
		realserver_addr.port(80);
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.last_time = last_time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		// get the existing data
		l7vs::sslid_replication_data old_data;
		memset(&old_data, 0, sizeof(sslid_replication_data));
		memcpy(&old_data, this->replication_area, sizeof(sslid_replication_data));
		boost::thread test_thread2(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread2.interrupt();
		// data add position: after the old existing data, check it
		// get the added data information
		char session_id_array[SSLID_LENGTH + 1];
		memcpy(session_id_array, (this->replication_area + 1)->session_id, SSLID_LENGTH);
		temp_session_id[SSLID_LENGTH] = 0;
		std::string added_session_id(session_id_array);
		std::string added_address_ip((this->replication_area + 1)->realserver_ip);
		unsigned short added_address_port = (this->replication_area + 1)->realserver_port;
		// the old existing data does not changed, check it
		int compare_result = memcmp(&old_data, this->replication_area, sizeof(sslid_replication_data));
		BOOST_CHECK_EQUAL(compare_result, 0);
		// added session_id check
		BOOST_CHECK_EQUAL(added_session_id, test_data1.session_id);
		// added last_time check
		BOOST_CHECK_EQUAL((this->replication_area + 1)->last_time, test_data1.last_time);
		// added valid flag check
		BOOST_CHECK_EQUAL((this->replication_area + 1)->valid, 1);
		// added realserver_addr ip check
		BOOST_CHECK_EQUAL(added_address_ip, test_data1.realserver_addr.address().to_string());
		// added realserver_addr port check
		BOOST_CHECK_EQUAL(added_address_port, test_data1.realserver_addr.port());

		// unit_test[13] data add test(when data area has two sslip_replication_data,and the first sslip_replication_data's valid flag is unused)
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		temp_session_id = "temp_id2eeeeeetttteeeeeeemmmmuui";
		temp_data.last_time = time(0);
		memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
		temp_data.valid = 0;
		memcpy(this->replication_area, &temp_data, sizeof(struct l7vs::sslid_replication_data));
		temp_session_id = "temp_id3333333333333333333333333";
		temp_data.last_time = time(0);
		memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
		temp_data.valid = 1;
		memcpy(this->replication_area + 1, &temp_data, sizeof(struct l7vs::sslid_replication_data));
		// set the added data
		realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
		realserver_addr.port(80);
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.last_time = last_time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		// get the second existing data
		memset(&old_data, 0, sizeof(sslid_replication_data));
		memcpy(&old_data, this->replication_area + 1, sizeof(sslid_replication_data));
		boost::thread test_thread3(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread3.interrupt();
		// data add position: the first existing data position, check it
		// get the added data information
		memset(session_id_array, 0, SSLID_LENGTH + 1);
		memcpy(session_id_array, this->replication_area->session_id, SSLID_LENGTH);
		added_session_id = std::string(session_id_array);
		added_address_ip = std::string(this->replication_area->realserver_ip);
		added_address_port = this->replication_area->realserver_port;
		// the second existing data not changed, check it
		compare_result = memcmp(&old_data, this->replication_area + 1, sizeof(sslid_replication_data));
		BOOST_CHECK_EQUAL(compare_result, 0);
		// added session_id check
		BOOST_CHECK_EQUAL(added_session_id, test_data1.session_id);
		// added last_time check
		BOOST_CHECK_EQUAL((this->replication_area)->last_time, test_data1.last_time);
		// added valid flag check
		BOOST_CHECK_EQUAL((this->replication_area)->valid, 1);
		// added realserver_addr ip check
		BOOST_CHECK_EQUAL(added_address_ip, test_data1.realserver_addr.address().to_string());
		// added realserver_addr port check
		BOOST_CHECK_EQUAL(added_address_port, test_data1.realserver_addr.port());

		// unit_test[14] data add test(when data area has no sslip_replication_data)
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
		realserver_addr.port(80);
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.last_time = last_time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread4(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread4.interrupt();
		// get added data information
		memset(session_id_array, 0, SSLID_LENGTH + 1);
		memcpy(session_id_array, this->replication_area->session_id, SSLID_LENGTH);
		std::string saved_session_id(session_id_array);
		std::string saved_address_ip(this->replication_area->realserver_ip);
		unsigned short saved_address_port = this->replication_area->realserver_port;
		// session_id is saved, check it
		BOOST_CHECK_EQUAL(saved_session_id, std::string("test_id123456789abcdefghijklmnop"));
		// last_time is saved,  check it
		BOOST_CHECK_EQUAL(this->replication_area->last_time, test_data1.last_time);
		// valid flag check
		BOOST_CHECK_EQUAL(this->replication_area->valid, 1);
		// realserver_addr ip check
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("192.168.120.102"));
		// realserver_addr port check
		BOOST_CHECK_EQUAL(saved_address_port, 80);

		// unit_test[15] data update test
		// update the added data
		realserver_addr.address(boost::asio::ip::address::from_string("255.255.255.255"));
		realserver_addr.port(port);
		test_data1.op_code = 'U';
		test_data1.last_time = update_time;
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread5(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread5.interrupt();
		// get saved information
		memset(session_id_array, 0, SSLID_LENGTH + 1);
		memcpy(session_id_array, this->replication_area->session_id, SSLID_LENGTH);
		saved_session_id = std::string(session_id_array);
		saved_address_ip = std::string(this->replication_area->realserver_ip);
		saved_address_port = this->replication_area->realserver_port;
		// session_id is not changed, check it
		BOOST_CHECK_EQUAL(saved_session_id, std::string("test_id123456789abcdefghijklmnop"));
		// last_time is updated , check it
		BOOST_CHECK_EQUAL(this->replication_area->last_time, update_time);
		// valid flag is not changed ,check it
		BOOST_CHECK_EQUAL(this->replication_area->valid, 1);
		// realserver_addr ip is changed, check it
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("255.255.255.255"));
		// realserver_addr port is changed, check it
		BOOST_CHECK_EQUAL(saved_address_port, port);

		// unit_test[16] data delete test
		realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
		realserver_addr.port(80);
		test_data1.op_code = 'D';
		test_data1.realserver_addr = realserver_addr;
		test_data1.last_time = delete_time;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread6(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread6.interrupt();
		// get saved information
		memset(session_id_array, 0, SSLID_LENGTH + 1);
		memcpy(session_id_array, this->replication_area->session_id, SSLID_LENGTH);
		saved_session_id = std::string(session_id_array);
		saved_address_ip = std::string(this->replication_area->realserver_ip);
		saved_address_port = this->replication_area->realserver_port;
		// sslid_replication_data is deleted logically, valid flag is setted to 0, check it
		BOOST_CHECK_EQUAL(this->replication_area->valid, 0);
		// last_time is not updated , check it
		BOOST_CHECK_EQUAL(this->replication_area->last_time, update_time);
		// realserver_addr ip is not changed, check it
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("255.255.255.255"));
		// realserver_addr port is not changed, check it
		BOOST_CHECK_EQUAL(saved_address_port, port);

		// unit_test[17] when op_code is out of 'A','U','D', old data is not changed test
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
		realserver_addr.port(port);
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.op_code = 'T';
		test_data1.last_time = last_time;
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		// get old session data
		l7vs::sslid_replication_data old_session_data[3];
		memcpy(&old_session_data, this->replication_area, 3*sizeof(sslid_replication_data));
		boost::thread test_thread7(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread7.interrupt();
		// test_data1 is not saved, so old data is not changed, check it
		// old data not changed check
		compare_result = memcmp(&old_session_data, this->replication_area, 3*sizeof(sslid_replication_data));
		BOOST_CHECK_EQUAL(compare_result, 0);

		// unit_test[18] replication_area_lock function is called test
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		test_data1.op_code = 'A';
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		is_lock_function_called = false;
		this->replication_area_lock = lock_function;
		this->replication_area_unlock = unlock_function;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		try{
			boost::thread test_thread8(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
			sleep(1);
			test_thread8.interrupt();
		} catch(...) {
			BOOST_ERROR("exception: write_replicaion_area_test");
		}
		// replication_area_lock function called check
		BOOST_CHECK(is_lock_function_called);

		// unit_test[19] replication_area_unlock function is called test
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		test_data1.op_code = 'A';
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		is_unlock_function_called = false;
		this->replication_area_lock = lock_function;
		this->replication_area_unlock = unlock_function;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		try{
			boost::thread test_thread9(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
			sleep(1);
			test_thread9.interrupt();
		} catch(...) {
			BOOST_ERROR("exception: write_replicaion_area_test");
		}
		// replication_area_unlock function called check
		BOOST_CHECK(is_unlock_function_called);

		// unit_test[20] ipv6 test(condition: while data area has no sslip_replication_data and one data will add to to the data area)
		memset(this->replication_area, 0, this->maxlist*sizeof(struct l7vs::sslid_replication_data));
		realserver_addr.address(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"));
		realserver_addr.port(80);
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.last_time = last_time;
		test_data1.op_code = 'A';
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread10(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread10.interrupt();
		memset(session_id_array, 0, SSLID_LENGTH + 1);
		memcpy(session_id_array, this->replication_area->session_id, SSLID_LENGTH);
		saved_session_id = std::string(session_id_array);
		saved_address_ip = std::string(this->replication_area->realserver_ip);
		saved_address_port = this->replication_area->realserver_port;
		// session_id is saved, check it
		BOOST_CHECK_EQUAL(saved_session_id, std::string("test_id123456789abcdefghijklmnop"));
		// last_time is saved,  check it
		BOOST_CHECK_EQUAL(this->replication_area->last_time, test_data1.last_time);
		// valid flag check
		BOOST_CHECK_EQUAL(this->replication_area->valid, 1);
		// realserver_addr ip check
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"));
		// realserver_addr port check
		BOOST_CHECK_EQUAL(saved_address_port, 80);

		// unit_test[21] ipv6 test(condition: while update data)
		realserver_addr.address(boost::asio::ip::address::from_string("1:21d0:1:4866:1:1:3a4b:1230"));
		realserver_addr.port(port);
		test_data1.op_code = 'U';
		test_data1.last_time = update_time;
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread11(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread11.interrupt();
		// get saved information
		saved_address_ip = std::string(this->replication_area->realserver_ip);
		saved_address_port = this->replication_area->realserver_port;
		// realserver_addr ip is updated, check it
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("1:21d0:1:4866:1:1:3a4b:1230"));
		// realserver_addr port check
		BOOST_CHECK_EQUAL(saved_address_port, port);

		// unit_test[22] ipv6 test(condition: while delete data)
		realserver_addr.address(boost::asio::ip::address::from_string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"));
		test_data1.op_code = 'D';
		test_data1.last_time = delete_time;
		test_data1.realserver_addr = realserver_addr;
		this->temp_list.clear();
		this->put_into_temp_list(test_data1);
		boost::thread test_thread12(boost::bind(&sslid_replication_data_processor_test_class::write_replicaion_area, this));
		sleep(1);
		test_thread12.interrupt();
		// get saved information
		saved_address_ip = std::string(this->replication_area->realserver_ip);
		saved_address_port = this->replication_area->realserver_port;
		// sslid_replication_data is deleted logically, valid flag is setted to 0, check it
		BOOST_CHECK_EQUAL(this->replication_area->valid, 0);
		// last_time is not updated , check it
		BOOST_CHECK_EQUAL(this->replication_area->last_time, update_time);
		// realserver_addr ip is not changed, check it
		BOOST_CHECK_EQUAL(saved_address_ip, std::string("1:21d0:1:4866:1:1:3a4b:1230"));
		// realserver_addr port is not changed, check it
		BOOST_CHECK_EQUAL(saved_address_port, port);
	}

	// register_replication_area_lock_test
	void register_replication_area_lock_test(){

		boost::function<void(void)> register_function = &lock_function;

		// unit_test[23] registered function is NULL test
		this->register_replication_area_lock(NULL);
		BOOST_CHECK_EQUAL(this->replication_area_lock.empty(), true);

		// unit_test[24] while registered replication_area_lock function is not NULL test
		this->register_replication_area_lock(register_function);
		// function registered correctly check;
		BOOST_CHECK_EQUAL(this->replication_area_lock, lock_function);
	}

	// register_replication_area_unlock_test
	void register_replication_area_unlock_test(){

		boost::function<void(void)> register_function = &unlock_function;

		// unit_test[25] registered function is NULL test
		this->register_replication_area_unlock(NULL);
		// function registered correctly check;
		BOOST_CHECK_EQUAL(this->replication_area_unlock.empty(), true);

		// unit_test[26] while registered replication_area_unlock function is not NULL, test
		this->register_replication_area_unlock(register_function);
		// function registered correctly check;
		BOOST_CHECK_EQUAL(this->replication_area_unlock, unlock_function);
	}

	// get_from_temp_list_test
	void get_from_temp_list_test(){

		l7vs::sslid_replication_temp_data test_data1;
		l7vs::sslid_replication_temp_data test_data2;
		l7vs::sslid_replication_temp_data get_data;
		boost::asio::ip::tcp::endpoint endpoint;

		// unit_test[27] when temp_list is empty, test
		try{
			this->temp_list.clear();
			boost::thread test_thread(boost::bind(&sslid_replication_data_processor_test_class::get_from_temp_list, this, get_data));
			test_thread.timed_join(boost::posix_time::milliseconds(100));
			this->temp_list.push_back(test_data1);
			sleep(1);
		} catch(...) {
			BOOST_ERROR("exception: get_from_temp_list");
		}

		// unit_test[28] muliti-threads test
		try {
			this->temp_list.clear();
			boost::thread_group thread_group1;
			thread_group1.create_thread(boost::bind(&sslid_replication_data_processor_test_class::get_from_temp_list_thread1, this));
			thread_group1.create_thread(boost::bind(&sslid_replication_data_processor_test_class::get_from_temp_list_thread2, this));
			thread_group1.join_all();
		} catch(...) {
			BOOST_ERROR("exception: get_from_temp_list");
		}

		// unit_test[29] first element of temp_list is removed test
		test_data1.session_id = "test_id123456789abcdefghijklmnop";
		test_data1.op_code = 'T';
		test_data1.realserver_addr = endpoint;
		test_data1.last_time = time(0);
		test_data2.session_id = "test_id2abcdefghijklmnop23456789";
		test_data2.op_code = 'A';
		test_data2.realserver_addr = endpoint;
		test_data2.last_time = time(0);
		this->temp_list.clear();
		this->temp_list.push_back(test_data1);
		this->temp_list.push_back(test_data2);
		this->get_from_temp_list(get_data);
		l7vs::sslid_replication_temp_data& leave_data = this->temp_list.back();
		// first item removed check,so only one item leave, check it
		BOOST_CHECK_EQUAL(this->temp_list.size(), 1u);
		// session_id check
		BOOST_CHECK_EQUAL(leave_data.session_id, test_data2.session_id);
		// op_code check
		BOOST_CHECK_EQUAL(leave_data.op_code, test_data2.op_code);
		// realserver_addr check
		BOOST_CHECK_EQUAL(leave_data.realserver_addr, test_data2.realserver_addr);
		// last_time check
		BOOST_CHECK_EQUAL(leave_data.last_time, test_data2.last_time);

		// unit_test[30] get data test(list has one item)
		test_data1.op_code = 'A';
		this->temp_list.clear();
		this->temp_list.push_back(test_data1);
		this->get_from_temp_list(get_data);
		// got data check
		// session_id check
		BOOST_CHECK_EQUAL(get_data.session_id, test_data1.session_id);
		// op_code check
		BOOST_CHECK_EQUAL(get_data.op_code, test_data1.op_code);
		// realserver_addr check
		BOOST_CHECK_EQUAL(get_data.realserver_addr, test_data1.realserver_addr);
		// last_time check
		BOOST_CHECK_EQUAL(get_data.last_time, test_data1.last_time);

		// unit_test[31] get data test(list has two items, get the item which first put into)
		test_data1.op_code = 'U';
		test_data2.op_code = 'D';
		this->temp_list.clear();
		this->temp_list.push_back(test_data1);
		this->temp_list.push_back(test_data2);
		this->get_from_temp_list(get_data);
		// got data check
		// session_id check
		BOOST_CHECK_EQUAL(get_data.session_id, test_data1.session_id);
		// op_code check
		BOOST_CHECK_EQUAL(get_data.op_code, test_data1.op_code);
		// realserver_addr check
		BOOST_CHECK_EQUAL(get_data.realserver_addr, test_data1.realserver_addr);
		// last_time check
		BOOST_CHECK_EQUAL(get_data.last_time, test_data1.last_time);
	}

	// get_replication_area_test
	void get_replication_area_test(sslid_replication_data* expecting_sslid_replication_data) {

		// unit_test[32] get_replication_area function test
		sslid_replication_data* get_data = this->get_replication_area();
		BOOST_CHECK_EQUAL(get_data, expecting_sslid_replication_data);
	}
};

// function sslid_replication_data_processor test
void sslid_replication_data_processor_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	int maxlist_test = 5;
	char *header_start_address = replication_data_area;
	char *real_data_start_address = header_start_address + 128*sizeof(struct l7vs::sslid_replication_data_header);
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;
	l7vs::sslid_replication_data_header* first_header = (sslid_replication_data_header*)header_start_address;

	memset(replication_data_area, 0, data_area_size);

	virtual_service_endpoint.port(999);
	sslid_replication_data_processor_test_class test_object(maxlist_test,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	test_object.sslid_replication_data_processor_test(maxlist_test,
			(l7vs::sslid_replication_data*) real_data_start_address,
			virtual_service_endpoint);

	// unit_test[33] while replication_data_area is NULL, exception not occur, test it
	try {
		sslid_replication_data_processor test_object1(maxlist_test,
				NULL, SECTION_NUMBER, virtual_service_endpoint, replication_ingetloglevel,
				replication_inputLogFatal, replication_inputLogError,
				replication_inputLogWarn, replication_inputLogInfo,
				replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[34] while maxlist < 0, exception occur, test it
	bool exception_occured = false;
	try {
		sslid_replication_data_processor test_object(-1,
				replication_data_area, SECTION_NUMBER, virtual_service_endpoint, replication_ingetloglevel,
				replication_inputLogFatal, replication_inputLogError,
				replication_inputLogWarn, replication_inputLogInfo,
				replication_inputLogDebug);
	} catch (...) {
		exception_occured = true;
	}
	BOOST_CHECK(exception_occured);

	// unit_test[35] while maxlist = 0, exception not occur, test it
	try {
		sslid_replication_data_processor test_object(0,
				replication_data_area, SECTION_NUMBER, virtual_service_endpoint, replication_ingetloglevel,
				replication_inputLogFatal, replication_inputLogError,
				replication_inputLogWarn, replication_inputLogInfo,
				replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[36] while maxlist = 1, exception not occur, test it
	try {
		sslid_replication_data_processor test_object(1,
				replication_data_area, SECTION_NUMBER, virtual_service_endpoint, replication_ingetloglevel,
				replication_inputLogFatal, replication_inputLogError,
				replication_inputLogWarn, replication_inputLogInfo,
				replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[37] while sslid_replication_area_size < 0, exception occur, test it
	exception_occured = false;
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = static_cast<size_t>(2 * sizeof(l7vs::sslid_replication_data));
		}
		l7vs::sslid_replication_data_processor test_object2(2,
				replication_data_area, -1, virtual_service_endpoint,
				replication_ingetloglevel, replication_inputLogFatal,
				replication_inputLogError, replication_inputLogWarn,
				replication_inputLogInfo, replication_inputLogDebug);
	} catch (...) {
		exception_occured = true;
	}
	BOOST_CHECK(exception_occured);

	// unit_test[38] while sslid_replication_area_size = 0, exception not occur, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = static_cast<size_t>(2 * sizeof(l7vs::sslid_replication_data));
		}
		l7vs::sslid_replication_data_processor test_object2(2,
				replication_data_area, 0, virtual_service_endpoint,
				replication_ingetloglevel, replication_inputLogFatal,
				replication_inputLogError, replication_inputLogWarn,
				replication_inputLogInfo, replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[39] when virtual service data has exist and the virtual service's data area is samller than needed,
	// unit_test[39] need to reset the virtual service session information, test
	int max_list_size = 3;
	std::string virtual_service_ip = "192.168.120.102";
	memset(replication_data_area, 0, data_area_size);
	virtual_service_endpoint.address(boost::asio::ip::address::from_string(virtual_service_ip));
	virtual_service_endpoint.port(80);
	memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
	first_header->virtualserver_port = 80;
	first_header->size = static_cast<size_t>((max_list_size - 1) * sizeof(l7vs::sslid_replication_data));
	first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
	l7vs::sslid_replication_data_processor test_object3(
			max_list_size, replication_data_area,
			SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	// the virtual service's size is resetted, check it
	size_t expecting_size = static_cast<size_t>(max_list_size * sizeof(l7vs::sslid_replication_data));
	BOOST_CHECK_EQUAL((first_header+1)->size, expecting_size);

	// unit_test[40] when resize the virtual service session information, if header area is full, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = sizeof(l7vs::sslid_replication_data);
		}
		memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
		first_header->virtualserver_port = 80;
		first_header->size = static_cast<size_t>((max_list_size - 1) * sizeof(l7vs::sslid_replication_data));
		first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
		l7vs::sslid_replication_data_processor test_object4(
				max_list_size, replication_data_area,
				SECTION_NUMBER, virtual_service_endpoint,
				replication_ingetloglevel, replication_inputLogFatal,
				replication_inputLogError, replication_inputLogWarn,
				replication_inputLogInfo, replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[41] when resize the virtual service session information, if header area is not full,
	// unit_test[41] virtual service session information is changed, test it
	memset(replication_data_area, 0, data_area_size);
	memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
	first_header->virtualserver_port = 80;
	first_header->size = static_cast<size_t>((max_list_size - 1) * sizeof(l7vs::sslid_replication_data));
	first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
	l7vs::sslid_replication_data_processor test_object5(
			max_list_size, replication_data_area,
			SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	// the virtual service information's size is resized, get it
	expecting_size = static_cast<size_t>(max_list_size * sizeof(l7vs::sslid_replication_data));
	// the virtual service information's offset is resized, get it
	size_t expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address + first_header->size);
	// size check
	BOOST_CHECK_EQUAL((first_header+1)->size, expecting_size);
	// offset check
	BOOST_CHECK_EQUAL((first_header+1)->offset, expecting_offset);

	// unit_test[42] when virtual service data is not exist and header area is not full and the
	// unit_test[42] sslip replication data area there is no enough sapce to add data, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		std::string temp_virtual_service_ip = "255.255.255.255";
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = static_cast<size_t>((max_list_size + 1) * sizeof(l7vs::sslid_replication_data));
			memcpy(temp->virtualserver_ip, temp_virtual_service_ip.c_str(), temp_virtual_service_ip.length());
			if(i == 127) {
				temp->size = 0;
			}
		}
		l7vs::sslid_replication_data_processor test_object6(
				max_list_size, replication_data_area,
				SECTION_NUMBER, virtual_service_endpoint,
				replication_ingetloglevel, replication_inputLogFatal,
				replication_inputLogError, replication_inputLogWarn,
				replication_inputLogInfo, replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[43] when virtual service data is not exist and the sslip replication area there is
	// unit_test[43] enough sapce to add data, add the data to the replication area, test it
	memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
	l7vs::sslid_replication_data_processor test_object7(
			max_list_size, replication_data_area,
			SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	expecting_size = static_cast<size_t>(max_list_size * sizeof(l7vs::sslid_replication_data));
	expecting_offset = static_cast<size_t>(real_data_start_address - header_start_address);
	// session information's size check
	BOOST_CHECK_EQUAL(first_header->size, expecting_size);
	// session information's offset check
	BOOST_CHECK_EQUAL(first_header->offset, expecting_offset);

	// unit_test[44] when virtual service data is not exist and the sslip replication header area
	// unit_test[44] is full, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		std::string temp_virtual_service_ip = "255.255.255.255";
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = static_cast<size_t>(max_list_size * sizeof(l7vs::sslid_replication_data));
			memcpy(temp->virtualserver_ip, temp_virtual_service_ip.c_str(), temp_virtual_service_ip.length());
		}
		l7vs::sslid_replication_data_processor test_object8(
				max_list_size, replication_data_area,
				SECTION_NUMBER, virtual_service_endpoint,
				replication_ingetloglevel, replication_inputLogFatal,
				replication_inputLogError, replication_inputLogWarn,
				replication_inputLogInfo, replication_inputLogDebug);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// unit_test[45] while virtual service ip is ipv6, test(condition: virtual service data has exist and
	// unit_test[45] the virtual service's data area is samller than needed, need to reset the virtual service session information)
	virtual_service_ip = "abcd:21d0:8936:4866:eefe:567d:3a4b:1230";
	memset(replication_data_area, 0, data_area_size);
	virtual_service_endpoint.address(boost::asio::ip::address::from_string(virtual_service_ip));
	virtual_service_endpoint.port(80);
	memcpy(first_header->virtualserver_ip, virtual_service_ip.c_str(), virtual_service_ip.length());
	first_header->virtualserver_port = 80;
	first_header->size = static_cast<size_t>((max_list_size - 1) * sizeof(l7vs::sslid_replication_data));
	first_header->offset = static_cast<size_t>(real_data_start_address - header_start_address);
	l7vs::sslid_replication_data_processor test_object9(
			max_list_size, replication_data_area,
			SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	// the virtual service's size is resetted, check it
	expecting_size = static_cast<size_t>(max_list_size * sizeof(l7vs::sslid_replication_data));
	BOOST_CHECK_EQUAL((first_header+1)->size, expecting_size);

	delete []replication_data_area;
}

// function put_into_temp_list test
void put_into_temp_list_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(3,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);

	test_object.put_into_temp_list_test();
	delete []replication_data_area;
}

// function write_replicaion_area test
void write_replicaion_area_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(3,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	l7vs::sslid_replication_temp_data test_data1;
	boost::function<void(void)> function_pointer;
	boost::asio::ip::tcp::endpoint realserver_addr;

	test_object.write_replicaion_area_test();
	delete []replication_data_area;
}

// function register_replication_area_lock test
void register_replication_area_lock_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(1,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);

	test_object.register_replication_area_lock_test();
	delete []replication_data_area;
}

// function register_replication_area_unlock test
void register_replication_area_unlock_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(1,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);

	test_object.register_replication_area_unlock_test();
	delete []replication_data_area;
}

// function get_from_temp_list test
void get_from_temp_list_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(2,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);
	l7vs::sslid_replication_temp_data test_data1;
	l7vs::sslid_replication_temp_data test_data2;
	l7vs::sslid_replication_temp_data get_data;
	boost::asio::ip::tcp::endpoint endpoint;

	test_object.get_from_temp_list_test();
	delete []replication_data_area;
}

// get_replication_area_test
void get_replication_area_test() {

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_test_class test_object(2,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			replication_ingetloglevel, replication_inputLogFatal,
			replication_inputLogError, replication_inputLogWarn,
			replication_inputLogInfo, replication_inputLogDebug);

	sslid_replication_data* expecting_result;
	expecting_result = reinterpret_cast<sslid_replication_data*> (replication_data_area
					+ 128 * sizeof(struct l7vs::sslid_replication_data_header));

	test_object.get_replication_area_test(expecting_result);
	delete []replication_data_area;
}

void sslid_replication_data_processor_test_main()
{
	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "sslid_replication_data_processor" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &sslid_replication_data_processor_test ) );
	ts->add( BOOST_TEST_CASE( &put_into_temp_list_test ) );
	ts->add( BOOST_TEST_CASE( &write_replicaion_area_test ) );
	ts->add( BOOST_TEST_CASE( &register_replication_area_lock_test ) );
	ts->add( BOOST_TEST_CASE( &register_replication_area_unlock_test ) );
	ts->add( BOOST_TEST_CASE( &get_from_temp_list_test ) );
	ts->add( BOOST_TEST_CASE( &get_replication_area_test ) );

	framework::master_test_suite().add( ts );
}
