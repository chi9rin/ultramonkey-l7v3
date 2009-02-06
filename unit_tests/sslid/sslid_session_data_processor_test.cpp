/*
 * sslid_session_data_processor_test.cpp
 *
 *  Created on: Jan 20, 2009
 *      Author: root
 */
#include "sslid_to_be_test_file.h"

using namespace boost::unit_test;
using namespace l7vs;

#define SECTION_NUMBER	200
#define STRUCT_NUMBER	256

LOG_LEVEL_TAG getloglevel_test(){

	return LOG_LV_NONE;
}

void putLogFatal_test(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void putLogError_test(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void putLogWarn_test(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void putLogInfo_test(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}

void putLogDebug_test(const unsigned int id, const std::string& message, const char* file_name, int line_number){
}
l7vs::sslid_replication_data_processor::getloglevel_func_type ingetloglevel1 = getloglevel_test;
l7vs::sslid_replication_data_processor::logger_func_type inputLogFatal1 = putLogFatal_test;
l7vs::sslid_replication_data_processor::logger_func_type inputLogError1 = putLogError_test;
l7vs::sslid_replication_data_processor::logger_func_type inputLogWarn1 = putLogWarn_test;
l7vs::sslid_replication_data_processor::logger_func_type inputLogInfo1 = putLogInfo_test;
l7vs::sslid_replication_data_processor::logger_func_type inputLogDebug1 = putLogDebug_test;
l7vs::sslid_session_data_processor::getloglevel_func_type ingetloglevel2 = getloglevel_test;
l7vs::sslid_session_data_processor::logger_func_type inputLogFatal2 = putLogFatal_test;
l7vs::sslid_session_data_processor::logger_func_type inputLogError2 = putLogError_test;
l7vs::sslid_session_data_processor::logger_func_type inputLogWarn2 = putLogWarn_test;
l7vs::sslid_session_data_processor::logger_func_type inputLogInfo2 = putLogInfo_test;
l7vs::sslid_session_data_processor::logger_func_type inputLogDebug2 = putLogDebug_test;

class sslid_replication_data_processor_replacement: public sslid_replication_data_processor{
public:
	sslid_replication_data_processor_replacement(int maxlist,
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

	// to call the get_from_temp_list function
	void to_get_from_temp_list( sslid_replication_temp_data& data ){
		this->get_from_temp_list(data);
	}

	// to get the temp_list
	std::deque<sslid_replication_temp_data>& get_temp_list(){
		return this->temp_list;
	}
};

// test class
class sslid_session_data_processor_test_class : public sslid_session_data_processor{
public:
	// sslid_session_data_processor_test_class
	sslid_session_data_processor_test_class(int maxlist, int timeout,
			sslid_replication_data_processor* replication_data_processor,
			getloglevel_func_type ingetloglevel,
			logger_func_type inputLogFatal, logger_func_type inputLogError,
			logger_func_type inputLogWarn, logger_func_type inputLogInfo,
			logger_func_type inputLogDebug) :
				sslid_session_data_processor(maxlist, timeout,
				replication_data_processor, ingetloglevel, inputLogFatal,
				inputLogError, inputLogWarn, inputLogInfo, inputLogDebug) {

	}

	// write_session_data_test thread
	void write_session_data_test_thread(
            const std::string& session_id,
            const boost::asio::ip::tcp::endpoint& endpoint,
            time_t now_time ){
		this->write_session_data(session_id, endpoint, now_time);
	}

	// sslid_session_data_processor_test
	void sslid_session_data_processor_test(
			int expecting_maxlist,
			int expecting_timeout,
			sslid_replication_data_processor* expecting_replication_data_processor) {

		// unit_test[1] member variable initialization test
		// maxlist check
		BOOST_CHECK_EQUAL(this->maxlist, expecting_maxlist);
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
		// session_endpoint_map check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 0u);
		// session_lasttime_map check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 0u);
		// lasttime_session_map
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 0u);
	}

	// get_endpoint_from_session_data_test
	void get_endpoint_from_session_data_test(){

		int result = 10;
		time_t last_time = time(0);
		std::string session_id;
		boost::asio::ip::tcp::endpoint get_endpoint;
		boost::asio::ip::tcp::endpoint saved_endpoint;
		sslid_replication_temp_data temp_list_data;
		sslid_replication_data_processor_replacement *this_replication_data_processor =
			dynamic_cast<sslid_replication_data_processor_replacement *>(this->replication_data_processor);

		// unit_test[2] session_endpoint_map and session_lasttime_map are all empty test
		// unit_test[2] return value: 1
		session_id = "test_id123456789abcdefghijklmnop";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

		// unit_test[3] when session_endpoint_map is empty, but session_lasttime_map is not empty test
		// unit_test[3] return value: 1
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_lasttime_map[session_id] = last_time;
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

		// unit_test[4] when session_endpoint_map is not empty, but session_lasttime_map is empty test
		// unit_test[4] return value: 1
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

		// unit_test[5] while exist one data and does not time out, test
		// unit_test[5] return value: 0
		result = 10;
		session_id = "test_id123456789abcdefghijklmnop";
		saved_endpoint.address(boost::asio::ip::address::from_string("192.168.120.102"));
		saved_endpoint.port(80);
		get_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
		this->timeout = 10000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// got endpoint check
		BOOST_CHECK_EQUAL(get_endpoint, saved_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);

		// unit_test[6] while exist one data and time out test
		// unit_test[6] return value: 0
		result = 10;
		this->timeout = 0;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_lasttime_map[session_id] = last_time - 10;
		this->lasttime_session_map.insert(std::make_pair(last_time - 10, session_id));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map item removed check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 0u);
		// session_lasttime_map item removed check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 0u);
		// lasttime_session_map item removed check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 0u);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);

		// unit_test[7] while map has 3 data and all data does not time out, test
		// unit_test[7] return value: 0
		result = 10;
		std::string temp_session_id1 = "temp_session_idyyyyyuuuu33456780";
		std::string temp_session_id2 = "temp_session_id456789012rtyuerxy";
		boost::asio::ip::tcp::endpoint temp_endpoint;
		temp_endpoint.port(3333);
		this->timeout = 10000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_endpoint_map[temp_session_id1] = temp_endpoint;
		this->session_endpoint_map[temp_session_id2] = temp_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->session_lasttime_map[temp_session_id1] = last_time;
		this->session_lasttime_map[temp_session_id2] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id2));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// got endpoint check
		BOOST_CHECK_EQUAL(get_endpoint, saved_endpoint);

		// unit_test[8] while map has 3 data, two data does not time out, but the other time out
		time_t temp_last_time = time(0) - 10;
		time_t out_time = time(0) - 5000;
		result = 10;
		this->timeout = 1000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_endpoint_map[temp_session_id1] = temp_endpoint;
		this->session_endpoint_map[temp_session_id2] = temp_endpoint;
		this->session_lasttime_map[session_id] = temp_last_time;
		this->session_lasttime_map[temp_session_id1] = temp_last_time;
		this->session_lasttime_map[temp_session_id2] = out_time;
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, session_id));
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(out_time, temp_session_id2));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// got endpoint check
		BOOST_CHECK_EQUAL(get_endpoint, saved_endpoint);

		// unit_test[9] while map has 3 data and all data time out, test
		// unit_test[9] return value: 0
		temp_last_time = last_time - 10;
		this->timeout = 0;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_endpoint_map[temp_session_id1] = temp_endpoint;
		this->session_endpoint_map[temp_session_id2] = temp_endpoint;
		this->session_lasttime_map[session_id] = temp_last_time ;
		this->session_lasttime_map[temp_session_id1] = temp_last_time;
		this->session_lasttime_map[temp_session_id2] = temp_last_time;
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, session_id));
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, temp_session_id2));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map item removed check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 2u);
		// session_lasttime_map item removed check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 2u);
		// lasttime_session_map item removed check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 2u);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);

		// unit_test[10] while vaule of timeout is not 0 and the searching data time out, test
		// unit_test[10] return value: 0
		temp_last_time = time(0) - 20;
		this->timeout = 10;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_lasttime_map[session_id] = temp_last_time;
		this->lasttime_session_map.insert(std::make_pair(temp_last_time, session_id));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map item removed check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 0u);
		// session_lasttime_map item removed check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 0u);
		// lasttime_session_map item removed check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 0u);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);

		// unit_test[11] ipv6 test(exist one data and does not time out)
		saved_endpoint.address(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"));
		result = 10;
		this->timeout = 10000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// got endpoint check
		BOOST_CHECK_EQUAL(get_endpoint, saved_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
	}

	// write_session_data_test
	void write_session_data_test(){

		int result = 10;
		time_t last_time = time(0);
		time_t write_time = time(0) + 55;
		std::string session_id;
		boost::asio::ip::tcp::endpoint old_saved_endpoint;
		boost::asio::ip::tcp::endpoint new_saved_endpoint;
		std::multimap<time_t, std::string>::iterator lasttime_session_map_iterator;
		sslid_replication_temp_data temp_list_data;
		sslid_replication_data_processor_replacement *this_replication_data_processor =
			dynamic_cast<sslid_replication_data_processor_replacement *> (this->replication_data_processor);

		// unit_test[12] while map is empty, test
		try{
			this->session_endpoint_map.clear();
			this->session_lasttime_map.clear();
			this->lasttime_session_map.clear();
			this_replication_data_processor->get_temp_list().clear();
			result = this->write_session_data(session_id, new_saved_endpoint, write_time);
			lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
			// get the item which put into the temp_list
			this_replication_data_processor->to_get_from_temp_list(temp_list_data);
			// session_endpoint_map is setted check
			BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
			// session_lasttime_map is setted check
			BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
			// lasttime_session_map is setted check
			BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
			// function result check
			BOOST_CHECK_EQUAL(result, 0);
			// the item which put into the temp_list check
			BOOST_CHECK_EQUAL(temp_list_data.op_code, 'A');
			BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);
			BOOST_CHECK_EQUAL(temp_list_data.realserver_addr, new_saved_endpoint);
			BOOST_CHECK_EQUAL(temp_list_data.last_time, write_time);
		} catch (...) {
			BOOST_ERROR("exception: write_session_data");
		}

		// unit_test[13] session_id exist test
		old_saved_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
		new_saved_endpoint.address(boost::asio::ip::address::from_string("192.168.120.102"));
		session_id = "test_id123456789abcdefghijklmnop";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = old_saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'U');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(temp_list_data.last_time, write_time);

		// unit_test[14] while session_id exists, test(session_endpoint_map size = maxlist - 1)
		std::string temp_session_id = "test_id2abcdefghijklmnop23456789";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = old_saved_endpoint;
		this->session_endpoint_map[temp_session_id] = old_saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->session_lasttime_map[temp_session_id] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'U');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(temp_list_data.last_time, write_time);

		// unit_test[15] while session_id exists, test(session_endpoint_map size = maxlist)
		std::string temp_session_id1 = "test_idthhhffffeeeeddddffffeeeed";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = old_saved_endpoint;
		this->session_endpoint_map[temp_session_id] = old_saved_endpoint;
		this->session_endpoint_map[temp_session_id1] = old_saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->session_lasttime_map[temp_session_id] = last_time;
		this->session_lasttime_map[temp_session_id1] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id1));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'U');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(temp_list_data.last_time, write_time);

		// unit_test[16] session_id not exist test(session_endpoint_map size = maxlist - 1)
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(temp_session_id, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id1, new_saved_endpoint));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id, last_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id1, last_time));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id1));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// get the item which put into the temp_list
		this_replication_data_processor->to_get_from_temp_list(temp_list_data);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(temp_list_data.op_code, 'A');
		BOOST_CHECK_EQUAL(temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(temp_list_data.last_time, write_time);

		// unit_test[17] session_id not exist test(session_endpoint_map size = maxlist)
		std::string temp_session_id2 = "33rty567234ertgh6890sdfghbnmeeed";
		time_t earlier_time = last_time - 10;
		this->timeout = 10000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(temp_session_id, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id1, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id2, new_saved_endpoint));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id, earlier_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id1, last_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id2, last_time));
		this->lasttime_session_map.insert(std::make_pair(earlier_time, temp_session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id2));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		// get the item which put into the temp_list
		sslid_replication_temp_data& frist_temp_list_data = this_replication_data_processor->get_temp_list().front();
		sslid_replication_temp_data& last_temp_list_data = this_replication_data_processor->get_temp_list().back();
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(frist_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(frist_temp_list_data.session_id, temp_session_id);
		BOOST_CHECK_EQUAL(last_temp_list_data.op_code, 'A');
		BOOST_CHECK_EQUAL(last_temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(last_temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(last_temp_list_data.last_time, write_time);

		// unit_test[18] while session_id not exist and session_endpoint_map size = maxlist and one data is time out, test
		time_t out_of_time = time(0) - 2000;
		this->timeout = 1000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(temp_session_id, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id1, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id2, new_saved_endpoint));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id, out_of_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id1, last_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id2, last_time));
		this->lasttime_session_map.insert(std::make_pair(out_of_time, temp_session_id));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id2));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		// get the item which put into the temp_list
		frist_temp_list_data = this_replication_data_processor->get_temp_list().front();
		last_temp_list_data = this_replication_data_processor->get_temp_list().back();
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		BOOST_CHECK_EQUAL(frist_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(frist_temp_list_data.session_id, temp_session_id);
		BOOST_CHECK_EQUAL(last_temp_list_data.op_code, 'A');
		BOOST_CHECK_EQUAL(last_temp_list_data.session_id, session_id);
		BOOST_CHECK_EQUAL(last_temp_list_data.realserver_addr, new_saved_endpoint);
		BOOST_CHECK_EQUAL(last_temp_list_data.last_time, write_time);

		// unit_test[19] while session_id not exist and session_endpoint_map size = maxlist and two data is time out, test
		out_of_time = time(0) - 2000;
		this->timeout = 1000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(temp_session_id, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id1, new_saved_endpoint));
		this->session_endpoint_map.insert(std::make_pair(temp_session_id2, new_saved_endpoint));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id, out_of_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id1, out_of_time));
		this->session_lasttime_map.insert(std::make_pair(temp_session_id2, last_time));
		this->lasttime_session_map.insert(std::make_pair(out_of_time, temp_session_id));
		this->lasttime_session_map.insert(std::make_pair(out_of_time, temp_session_id1));
		this->lasttime_session_map.insert(std::make_pair(last_time, temp_session_id2));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// the item which put into the temp_list check
		int count = static_cast<int>(this_replication_data_processor->get_temp_list().size());
		for(int i = 0; i < count; i++){
			sslid_replication_temp_data& temp_data = this_replication_data_processor->get_temp_list()[i];
			// temp_session_id and temp_session_id1 which time out are removed check
			if (temp_data.session_id == temp_session_id ||
					temp_data.session_id == temp_session_id1) {
				BOOST_CHECK_EQUAL(temp_data.op_code, 'D');
			}
			if(temp_data.session_id == session_id){
				BOOST_CHECK_EQUAL(temp_data.op_code, 'A');
				BOOST_CHECK_EQUAL(temp_data.realserver_addr, new_saved_endpoint);
				BOOST_CHECK_EQUAL(temp_data.last_time, write_time);
			}
		}

		// unit_test[20] multi-threads test
		try{
			this->session_endpoint_map.clear();
			this->session_lasttime_map.clear();
			this->lasttime_session_map.clear();

			boost::thread_group thread_group;
			thread_group.create_thread(boost::bind(
							&sslid_session_data_processor_test_class::write_session_data_test_thread,
							this, temp_session_id, new_saved_endpoint,
							write_time));
			thread_group.create_thread(boost::bind(
							&sslid_session_data_processor_test_class::write_session_data_test_thread,
							this, temp_session_id1, new_saved_endpoint,
							write_time));
			thread_group.create_thread(boost::bind(
							&sslid_session_data_processor_test_class::write_session_data_test_thread,
							this, temp_session_id2, new_saved_endpoint,
							write_time));
			thread_group.join_all();

			BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 3u);
			BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 3u);
			BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 3u);
		} catch(...){
			BOOST_ERROR("exception: write_session_data");
		}

		// unit_test[21] ipv6 test(session_id exist)
		old_saved_endpoint.address(boost::asio::ip::address::from_string("0:0:0:0:0:0:0:0"));
		new_saved_endpoint.address(boost::asio::ip::address::from_string("abcd:21d0:8936:4866:eefe:567d:3a4b:1230"));
		session_id = "test_id123456789abcdefghijklmnop";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map[session_id] = old_saved_endpoint;
		this->session_lasttime_map[session_id] = last_time;
		this->lasttime_session_map.insert(std::make_pair(last_time, session_id));
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		lasttime_session_map_iterator = this->lasttime_session_map.find(write_time);
		// session_endpoint_map is setted check
		BOOST_CHECK_EQUAL(this->session_endpoint_map[session_id], new_saved_endpoint);
		// session_lasttime_map is setted check
		BOOST_CHECK_EQUAL(this->session_lasttime_map[session_id], write_time);
		// lasttime_session_map is setted check
		BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
	}

	// clear_expired_session_data_test
	void clear_expired_session_data_test(){

		int result = 10;
		time_t test_time = time(0);
		boost::asio::ip::tcp::endpoint saved_endpoint1;
		boost::asio::ip::tcp::endpoint saved_endpoint2;
		std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator it1;
		std::map<std::string, time_t>::iterator it2;
		std::multimap<time_t, std::string>::iterator it3;
		std::string session_id1 = "test_id123456789abcdefghijklmnop";
		std::string session_id2 = "test_id2abcdefghijklmnop23456789";
		sslid_replication_data_processor_replacement *this_replication_data_processor =
			dynamic_cast<sslid_replication_data_processor_replacement *> (this->replication_data_processor);

		// unit_test[22] map empty test
		this->timeout = 0;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		try{
			result = this->clear_expired_session_data();
		} catch(...) {
			BOOST_ERROR("exception: clear_expired_session_data");
		}
		// function result check
		BOOST_CHECK_EQUAL(result, 0);

		// unit_test[23] when time out items exist(all items is time out), items remove test
		test_time = time(0) - 5;
		this->timeout = 0;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(session_id1, saved_endpoint1));
		this->session_endpoint_map.insert(std::make_pair(session_id2, saved_endpoint2));
		this->session_lasttime_map.insert(std::make_pair(session_id1, test_time));
		this->session_lasttime_map.insert(std::make_pair(session_id2, test_time));
		this->lasttime_session_map.insert(std::make_pair(test_time, session_id1));
		this->lasttime_session_map.insert(std::make_pair(test_time, session_id2));
		result = this->clear_expired_session_data();
		// get data which put into temp_list
		sslid_replication_temp_data& first_temp_list_data = this_replication_data_processor->get_temp_list().front();
		sslid_replication_temp_data& last_temp_list_data = this_replication_data_processor->get_temp_list().back();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map items remove check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 0u);
		// session_lasttime_map items remove check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 0u);
		// lasttime_session_map items remove check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 0u);
		// temp_list item check
		BOOST_CHECK_EQUAL(this_replication_data_processor->get_temp_list().size(), 2u);
		BOOST_CHECK_EQUAL(first_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(first_temp_list_data.session_id, session_id1);
		BOOST_CHECK_EQUAL(last_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(last_temp_list_data.session_id, session_id2);

		// unit_test[24] when time out item exists(one item is time out, but another item is not), item remove test
		saved_endpoint2.port(88);
		this->timeout = 100;
		test_time = time(0);
		time_t out_time = test_time - 1000;
		time_t not_out_time = test_time;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(session_id1, saved_endpoint1));
		this->session_endpoint_map.insert(std::make_pair(session_id2, saved_endpoint2));
		this->session_lasttime_map.insert(std::make_pair(session_id1, out_time));
		this->session_lasttime_map.insert(std::make_pair(session_id2, not_out_time));
		this->lasttime_session_map.insert(std::make_pair(out_time, session_id1));
		this->lasttime_session_map.insert(std::make_pair(not_out_time, session_id2));
		result = this->clear_expired_session_data();
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// get data which put into temp_list
		first_temp_list_data = this_replication_data_processor->get_temp_list().front();
		// remove ruslt check(item 'test_id123456789abcdefghijklmnop' is removed, but item 'test_id2abcdefghijklmnop23456789' is still existing)
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map item remove check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		BOOST_CHECK_EQUAL(it1->first, session_id2);
		BOOST_CHECK_EQUAL(it1->second, saved_endpoint2);
		// session_lasttime_map item remove check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		BOOST_CHECK_EQUAL(it2->first, session_id2);
		BOOST_CHECK_EQUAL(it2->second, not_out_time);
		// lasttime_session_map item remove check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		BOOST_CHECK_EQUAL(it3->first, not_out_time);
		BOOST_CHECK_EQUAL(it3->second, session_id2);
		// temp_list item check
		BOOST_CHECK_EQUAL(first_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(first_temp_list_data.session_id, session_id1);

		// unit_test[25] when all items are not time out, item remove test
		this->timeout = 10000;
		time_t earlier_time = test_time -5;
		time_t earliest_time = test_time -10;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(session_id1, saved_endpoint1));
		this->session_endpoint_map.insert(std::make_pair(session_id2, saved_endpoint2));
		this->session_lasttime_map.insert(std::make_pair(session_id1, earlier_time));
		this->session_lasttime_map.insert(std::make_pair(session_id2, earliest_time));
		this->lasttime_session_map.insert(std::make_pair(earlier_time, session_id1));
		this->lasttime_session_map.insert(std::make_pair(earliest_time, session_id2));
		result = this->clear_expired_session_data();
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// get data which put into temp_list
		first_temp_list_data = this_replication_data_processor->get_temp_list().front();
		// remove ruslt check(item 'test_id2abcdefghijklmnop23456789' which has the earliest lasttime is removed)
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map item remove check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		BOOST_CHECK_EQUAL(it1->first, session_id1);
		BOOST_CHECK_EQUAL(it1->second, saved_endpoint1);
		// session_lasttime_map item remove check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		BOOST_CHECK_EQUAL(it2->first, session_id1);
		BOOST_CHECK_EQUAL(it2->second, earlier_time);
		// lasttime_session_map item remove check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		BOOST_CHECK_EQUAL(it3->first, earlier_time);
		BOOST_CHECK_EQUAL(it3->second, session_id1);
		// temp_list item check
		BOOST_CHECK_EQUAL(first_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(first_temp_list_data.session_id, session_id2);

		// unit_test[26] while all items are not time out and all item's lasttime is equal, first item remove test
		std::string session_id3 = "test_id3wasfgasdasdwasdrggrtrrrr";
		this->timeout = 10000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this_replication_data_processor->get_temp_list().clear();
		this->session_endpoint_map.insert(std::make_pair(session_id1, saved_endpoint1));
		this->session_endpoint_map.insert(std::make_pair(session_id2, saved_endpoint1));
		this->session_endpoint_map.insert(std::make_pair(session_id3, saved_endpoint1));
		this->session_lasttime_map.insert(std::make_pair(session_id1, test_time));
		this->session_lasttime_map.insert(std::make_pair(session_id2, test_time));
		this->session_lasttime_map.insert(std::make_pair(session_id3, test_time));
		this->lasttime_session_map.insert(std::make_pair(test_time, session_id1));
		this->lasttime_session_map.insert(std::make_pair(test_time, session_id2));
		this->lasttime_session_map.insert(std::make_pair(test_time, session_id3));
		// get the first item of session_endpoint_map
		it1 = this->session_endpoint_map.begin();
		std::string expecting_delete_session_id = it1->first;
		result = this->clear_expired_session_data();
		// get data which put into temp_list
		first_temp_list_data = this_replication_data_processor->get_temp_list().front();
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map item remove check
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 2u);
		// session_lasttime_map item remove check
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 2u);
		// lasttime_session_map item remove check
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 2u);
		// temp_list item check
		BOOST_CHECK_EQUAL(first_temp_list_data.op_code, 'D');
		BOOST_CHECK_EQUAL(first_temp_list_data.session_id, expecting_delete_session_id);
	}

	// read_session_data_from_replication_area_test
	void read_session_data_from_replication_area_test(){

		int result = 10;
		unsigned short port1 = 888;
		unsigned short port2 = 999;
		unsigned short port3 = 333;
		unsigned short port4 = 444;
		unsigned short port5 = 555;
		time_t last_time1 = time(0);
		time_t last_time2 = last_time1 - 10;
		time_t last_time3 = last_time2 - 10;
		time_t last_time4 = last_time3 - 10;
		time_t last_time5 = last_time4 - 10;
		std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator it1;
		std::map<std::string, time_t>::iterator it2;
		std::multimap<time_t, std::string>::iterator it3;

		sslid_replication_data* first_real_data = this->replication_data_processor->get_replication_area();

		memset(reinterpret_cast<char*>(first_real_data), 0, 10*sizeof(sslid_replication_data));

		// set the real data area(the first data)
		std::string session_id1 = "test_id123456789abcdefghijklmnop";
		std::string realserver_ip1 = "192.168.120.242";
		memcpy(first_real_data->session_id, session_id1.c_str(), session_id1.length());
		memcpy(first_real_data->realserver_ip, realserver_ip1.c_str(), realserver_ip1.length());
		first_real_data->valid = 1;
		first_real_data->realserver_port = port1;
		first_real_data->last_time = last_time1;

		// set the real data area(the second data)
		std::string session_id2 = "test_id2abcdefghijklmnop23456789";
		std::string realserver_ip2 = "255.255.255.255";
		memcpy((first_real_data + 1)->session_id, session_id2.c_str(), session_id2.length());
		memcpy((first_real_data + 1)->realserver_ip, realserver_ip2.c_str(), realserver_ip2.length());
		(first_real_data + 1)->valid = 1;
		(first_real_data + 1)->realserver_port = port2;
		(first_real_data + 1)->last_time = last_time2;

		// set the real data area(the third data)
		std::string session_id3 = "test_id33333333333333333333333ty";
		std::string realserver_ip3 = "202.195.0.255";
		memcpy((first_real_data + 2)->session_id, session_id3.c_str(), session_id3.length());
		memcpy((first_real_data + 2)->realserver_ip, realserver_ip3.c_str(), realserver_ip3.length());
		(first_real_data + 2)->valid = 0;
		(first_real_data + 2)->realserver_port = port3;
		(first_real_data + 2)->last_time = last_time3;

		// set the real data area(the fourth data)
		std::string session_id4 = "test_id4444444444444444444444444";
		std::string realserver_ip4 = "110.101.0.255";
		memcpy((first_real_data + 3)->session_id, session_id4.c_str(), session_id4.length());
		memcpy((first_real_data + 3)->realserver_ip, realserver_ip4.c_str(), realserver_ip4.length());
		(first_real_data + 3)->valid = 1;
		(first_real_data + 3)->realserver_port = port4;
		(first_real_data + 3)->last_time = last_time4;

		// set the real data area(the fifth data)
		std::string session_id5 = "test_id5555555555555555555555555";
		std::string realserver_ip5 = "120.120.0.120";
		memcpy((first_real_data + 4)->session_id, session_id5.c_str(), session_id5.length());
		memcpy((first_real_data + 4)->realserver_ip, realserver_ip5.c_str(), realserver_ip5.length());
		(first_real_data + 4)->valid = 1;
		(first_real_data + 4)->realserver_port = port5;
		(first_real_data + 4)->last_time = last_time5;

		// unit_test[27] NULL pointer test
		result = 10;
		try {
			result = this->read_session_data_from_replication_area(NULL);
		} catch (...){
			BOOST_ERROR("exception: read_session_data_from_replication_area");
		}
		// function result check
		BOOST_CHECK_EQUAL(result, -1);

		// unit_test[28] read data test(read the first data)
		result = 10;
		this->maxlist = 1;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data);
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		// session_endpoint_map's session_id check
		BOOST_CHECK_EQUAL(it1->first, session_id1);
		// session_endpoint_map's endpoint (ip && port) check
		BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip1);
		BOOST_CHECK_EQUAL(it1->second.port(), port1);
		// session_lasttime_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		// session_lasttime_map's session_id check
		BOOST_CHECK_EQUAL(it2->first, session_id1);
		// session_lasttime_map's lasttime check
		BOOST_CHECK_EQUAL(it2->second, last_time1);
		// lasttime_session_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		// lasttime_session_map's lasttime check
		BOOST_CHECK_EQUAL(it3->first, last_time1);
		// lasttime_session_map's session_id check
		BOOST_CHECK_EQUAL(it3->second, session_id1);

		// unit_test[29] read data test(read the second data)
		result = 10;
		this->maxlist = 1;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data + 1);
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		// session_endpoint_map's session_id check
		BOOST_CHECK_EQUAL(it1->first, session_id2);
		// session_endpoint_map's endpoint (ip && port) check
		BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip2);
		BOOST_CHECK_EQUAL(it1->second.port(), port2);
		// session_lasttime_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		// session_lasttime_map's session_id check
		BOOST_CHECK_EQUAL(it2->first, session_id2);
		// session_lasttime_map's lasttime check
		BOOST_CHECK_EQUAL(it2->second, last_time2);
		// lasttime_session_map check(only maxlist(1) data can read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		// lasttime_session_map's lasttime check
		BOOST_CHECK_EQUAL(it3->first, last_time2);
		// lasttime_session_map's session_id check
		BOOST_CHECK_EQUAL(it3->second, session_id2);

		// unit_test[30] read data test(read the thrid data, because the valid is 0, so nothing can be read)
		result = 10;
		this->maxlist = 1;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data + 2);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(nothing to read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 0u);
		// session_lasttime_map check(nothing to read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 0u);
		// lasttime_session_map check(nothing to read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 0u);

		// unit_test[31] while  maxlist is 2, once can read two data, test
		result = 10;
		this->maxlist = 2;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data);
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(only maxlist(2) data can read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 2u);
		// session_lasttime_map check(only maxlist(2) data can read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 2u);
		// lasttime_session_map check(only maxlist(2) data can read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 2u);
		// session_endpoint_map items check
		while(it1 != this->session_endpoint_map.end()){
			std::string temp = it1->first;
			if(temp == session_id1){
				BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip1);
				BOOST_CHECK_EQUAL(it1->second.port(), port1);
			} else if(temp == session_id2){
				BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip2);
				BOOST_CHECK_EQUAL(it1->second.port(), port2);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it1++;
		}
		// session_lasttime_map items check
		while(it2 != this->session_lasttime_map.end()){
			std::string temp = it2->first;
			if(temp == session_id1){
				BOOST_CHECK_EQUAL(it2->second, last_time1);
			} else if(temp == session_id2){
				BOOST_CHECK_EQUAL(it2->second, last_time2);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it2++;
		}
		// lasttime_session_map items check
		while(it3 != this->lasttime_session_map.end()){
			time_t temp = it3->first;
			if(temp == last_time1){
				it3->second = session_id1;
			} else if(temp == last_time2){
				it3->second = session_id2;
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it3++;
		}

		// unit_test[32] while  maxlist is 2 and the second to read data's valid is 0, test
		result = 10;
		this->maxlist = 2;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data + 1);
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		// session_lasttime_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		// lasttime_session_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		// session_endpoint_map items check
		while(it1 != this->session_endpoint_map.end()){
			std::string temp = it1->first;
			if(temp == session_id2){
				BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip2);
				BOOST_CHECK_EQUAL(it1->second.port(), port2);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it1++;
		}
		// session_lasttime_map items check
		while(it2 != this->session_lasttime_map.end()){
			std::string temp = it2->first;
			if(temp == session_id2){
				BOOST_CHECK_EQUAL(it2->second, last_time2);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it2++;
		}
		// lasttime_session_map items check
		while(it3 != this->lasttime_session_map.end()){
			time_t temp = it3->first;
			if(temp == last_time2){
				it3->second = session_id2;
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it3++;
		}

		// unit_test[33] while  maxlist is 2 and the first to read data's valid is 0, test
		result = 10;
		this->maxlist = 2;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data + 2);
		it1 = this->session_endpoint_map.begin();
		it2 = this->session_lasttime_map.begin();
		it3 = this->lasttime_session_map.begin();
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 1u);
		// session_lasttime_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 1u);
		// lasttime_session_map check(only maxlist(2) data can read and if the valid is 0 not to read)
		BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 1u);
		// session_endpoint_map items check
		while(it1 != this->session_endpoint_map.end()){
			std::string temp = it1->first;
			if(temp == session_id4){
				BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ip4);
				BOOST_CHECK_EQUAL(it1->second.port(), port4);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it1++;
		}
		// session_lasttime_map items check
		while(it2 != this->session_lasttime_map.end()){
			std::string temp = it2->first;
			if(temp == session_id4){
				BOOST_CHECK_EQUAL(it2->second, last_time4);
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it2++;
		}
		// lasttime_session_map items check
		while(it3 != this->lasttime_session_map.end()){
			time_t temp = it3->first;
			if(temp == last_time4){
				it3->second = session_id4;
			} else {
				BOOST_ERROR("error: read_session_data_from_replication_area");
			}
			it3++;
		}

		// unit_test[34] when the realserver_ip is ipv6, test
		// set the real data area(ipv6)
		std::string realserver_ipv6_ip1 = "abcd:21d0:8936:4866:eefe:567d:3a4b:1230";
		memcpy(first_real_data->realserver_ip, realserver_ipv6_ip1.c_str(), realserver_ipv6_ip1.length());
		result = 10;
		this->maxlist = 1;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->read_session_data_from_replication_area(first_real_data);
		// function result check
		BOOST_CHECK_EQUAL(result, 0);
		// session_endpoint_map's endpoint (ip && port) check
		it1 = this->session_endpoint_map.begin();
		BOOST_CHECK_EQUAL(it1->second.address().to_string(), realserver_ipv6_ip1);
		BOOST_CHECK_EQUAL(it1->second.port(), port1);
	}
};

// sslid_session_data_processor test
void sslid_session_data_processor_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;
	memset(replication_data_area, 0, data_area_size);
	l7vs::sslid_replication_data_processor replication_data_processor(3,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	sslid_session_data_processor_test_class test_object(3, 5,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.sslid_session_data_processor_test(3, 5, &replication_data_processor);

	// unit_test[35] while maxlist < 0, exception occur, test
	bool exception_occur = false;
	try{
		l7vs::sslid_session_data_processor(-1, 1, &replication_data_processor,
				ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
				inputLogInfo2, inputLogDebug2);
	} catch(...) {
		exception_occur = true;
	}
	BOOST_CHECK(exception_occur);

	// unit_test[36] while maxlist = 0, exception not occur, test
	try{
		l7vs::sslid_session_data_processor(0, 1, &replication_data_processor,
				ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
				inputLogInfo2, inputLogDebug2);
	} catch(...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}

	// unit_test[37] while timeout < 0, exception occur, test
	exception_occur = false;
	try{
		l7vs::sslid_session_data_processor(2, -2, &replication_data_processor,
				ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
				inputLogInfo2, inputLogDebug2);
	} catch(...) {
		exception_occur = true;
	}
	BOOST_CHECK(exception_occur);

	// unit_test[38] while timeout = 0, exception not occur, test
	try{
		l7vs::sslid_session_data_processor(2, 0, &replication_data_processor,
				ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
				inputLogInfo2, inputLogDebug2);
	} catch(...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}

	delete []replication_data_area;
}

// get_endpoint_from_session_data test
void get_endpoint_from_session_data_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_replacement replication_data_processor(5,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	sslid_session_data_processor_test_class test_object(5, 0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.get_endpoint_from_session_data_test();
	delete []replication_data_area;
}

// write_session_data test
void write_session_data_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_replacement replication_data_processor(3,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	sslid_session_data_processor_test_class test_object(3, 0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.write_session_data_test();
	delete []replication_data_area;
}

// clear_expired_session_data test
void clear_expired_session_data_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	sslid_replication_data_processor_replacement replication_data_processor(2,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	sslid_session_data_processor_test_class test_object(2, 0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.clear_expired_session_data_test();
	delete []replication_data_area;
}

// read_session_data_from_replication_area test
void read_session_data_from_replication_area_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	boost::asio::ip::tcp::endpoint virtual_service_endpoint;

	memset(replication_data_area, 0, data_area_size);

	l7vs::sslid_replication_data_processor replication_data_processor(1,
			replication_data_area, SECTION_NUMBER, virtual_service_endpoint,
			ingetloglevel1, inputLogFatal1, inputLogError1, inputLogWarn1,
			inputLogInfo1, inputLogDebug1);
	sslid_session_data_processor_test_class test_object(1, 0,
			&replication_data_processor, ingetloglevel2, inputLogFatal2,
			inputLogError2, inputLogWarn2, inputLogInfo2, inputLogDebug2);

	test_object.read_session_data_from_replication_area_test();

	delete []replication_data_area;
}

void sslid_session_data_processor_test_main()
{

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "sslid_session_data_processor" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &sslid_session_data_processor_test ) );
	ts->add( BOOST_TEST_CASE( &get_endpoint_from_session_data_test ) );
	ts->add( BOOST_TEST_CASE( &write_session_data_test ) );
	ts->add( BOOST_TEST_CASE( &clear_expired_session_data_test ) );
	ts->add( BOOST_TEST_CASE( &read_session_data_from_replication_area_test ) );

	framework::master_test_suite().add( ts );
}
