#include "sslid_to_be_test_file.h"

using namespace boost::unit_test;
using namespace l7vs;

#define SECTION_NUMBER	200
#define STRUCT_NUMBER	256

bool clear_function_return_fail = false;

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
	boost::mutex check_mutex;
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

	// clear_expired_session_data
	int clear_expired_session_data(){
		if(clear_function_return_fail){
			return 1;
		} else {
			return sslid_session_data_processor::clear_expired_session_data();
		}
	}

	// write_session_data_test thread
	void write_session_data_test_thread(
            const std::string& session_id,
            const boost::asio::ip::tcp::endpoint& endpoint,
            time_t now_time ){
		this->write_session_data(session_id, endpoint, now_time);
	}

	// get_endpoint_from_session_data_test thread
	void get_endpoint_from_session_data_thread(
            const std::string& session_id,
            boost::asio::ip::tcp::endpoint& expecting_endpoint){

		int result = 10;
		boost::asio::ip::tcp::endpoint get_endpoint;

		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		{
			boost::mutex::scoped_lock sclock(check_mutex);
			BOOST_CHECK_EQUAL(result, 0);
			BOOST_CHECK_EQUAL(get_endpoint, expecting_endpoint);
		}
	}

	// sslid_session_data_processor_test
	void sslid_session_data_processor_test(
			int expecting_maxlist,
			int expecting_timeout,
			sslid_replication_data_processor* expecting_replication_data_processor) {

    cout << "[1]------------------------------------------" << endl;
		// unit_test[1] メンバー変数が初期化されるチェックする。
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

	cout << "[2]------------------------------------------" << endl;
		// unit_test[2] maxlist < 0の場合, 例外が発生する。
		bool exception_occur = false;
		try{
			l7vs::sslid_session_data_processor(-1, 1, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			exception_occur = true;
		}
		BOOST_CHECK(exception_occur);

	cout << "[3]------------------------------------------" << endl;
		// unit_test[3] maxlist　が０の場合, 例外が発生しない。
		try{
			l7vs::sslid_session_data_processor(0, 1, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			BOOST_ERROR("exception: sslid_session_data_processor");
		}

	cout << "[4]------------------------------------------" << endl;
		// unit_test[4] timeout < 0の場合, 例外が発生する。
		exception_occur = false;
		try{
			l7vs::sslid_session_data_processor(2, -2, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			exception_occur = true;
		}
		BOOST_CHECK(exception_occur);

	cout << "[5]------------------------------------------" << endl;
		// unit_test[5] timeout が０の場合, 例外が発生しない。
		try{
			l7vs::sslid_session_data_processor(2, 0, expecting_replication_data_processor,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			BOOST_ERROR("exception: sslid_session_data_processor");
		}

	cout << "[6]------------------------------------------" << endl;
		// unit_test[6] replication_data_processorがNULLの場合, 例外が発生する。
		exception_occur = false;
		try{
			l7vs::sslid_session_data_processor(2, 2, NULL,
					ingetloglevel2, inputLogFatal2, inputLogError2, inputLogWarn2,
					inputLogInfo2, inputLogDebug2);
		} catch(...) {
			exception_occur = true;
		}
		BOOST_CHECK(exception_occur);
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

    cout << "[7]------------------------------------------" << endl;
		// unit_test[7] session_endpoint_map と session_lasttime_mapが空の場合、戻り値が失敗（１）で設定する。
		session_id = "test_id123456789abcdefghijklmnop";
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

    cout << "[8]------------------------------------------" << endl;
		// unit_test[8] session_endpoint_mapが空で,session_lasttime_mapが空でない場合、戻り値が失敗（１）で設定する。
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_lasttime_map[session_id] = last_time;
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

    cout << "[9]------------------------------------------" << endl;
		// unit_test[9] session_lasttime_mapが空で,session_endpoint_mapが空でない場合、戻り値が失敗（１）で設定する。
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id] = saved_endpoint;
		result = this->get_endpoint_from_session_data(session_id, get_endpoint);
		// function result check
		BOOST_CHECK_EQUAL(result, 1);

    cout << "[10]------------------------------------------" << endl;
		// unit_test[10] １つのデータがmapに存在して、タイムアウトしない場合、戻り値が正常（０）で設定する。
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

    cout << "[11]------------------------------------------" << endl;
		// unit_test[11] １つのデータがmapに存在して、タイムアウトの場合、戻り値が正常（０）で設定する。
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

    cout << "[12]------------------------------------------" << endl;
		// unit_test[12] mapに3つデータがあり、且つ３つデータが全部タイムアウトしない場合、戻り値が正常（０）で設定する。
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

    cout << "[13]------------------------------------------" << endl;
		// unit_test[13] mapに3つデータがあり、且つ２つデータがタイムアウトしなくて,１つのデータだけタイムアウトの場合、戻り値が正常（０）で設定する。
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

    cout << "[14]------------------------------------------" << endl;
		// unit_test[14] mapに3つデータがあり、且つ３つデータが全部タイムアウトの場合、戻り値が正常（０）で設定する。
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

    cout << "[15]------------------------------------------" << endl;
		// unit_test[15] メンバー関数timeoutが0でなくて、且つ検索対象データがタイムアウトの場合、戻り値が正常（０）で設定する。
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

    cout << "[16]------------------------------------------" << endl;
		// unit_test[16] endpointがipv6で、１つのデータが存在して、且つタイムアウトしない場合、戻り値が正常（０）で設定する。
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

	cout << "[17]------------------------------------------" << endl;
		// unit_test[17] マルチスレッドの場合、データを正常取得する。
		time_t time1 = time(0);
		time_t time2 = time1 - 2000;
		time_t time3 = time1 - 3000;
		std::string session_id1 = "11111111111111111111111111111111";
		std::string session_id2 = "22222222222222222222222222222222";
		std::string session_id3 = "33333333333333333333333333333333";
		boost::asio::ip::tcp::endpoint endpoint1;
		boost::asio::ip::tcp::endpoint endpoint2;
		boost::asio::ip::tcp::endpoint endpoint3;
		boost::asio::ip::tcp::endpoint default_endpoint;
		endpoint1.address(boost::asio::ip::address::from_string("192.168.120.1"));
		endpoint2.address(boost::asio::ip::address::from_string("192.168.120.2"));
		endpoint3.address(boost::asio::ip::address::from_string("192.168.120.3"));
		endpoint1.port(1);
		endpoint2.port(2);
		endpoint3.port(3);
		this->timeout = 1000;
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->session_endpoint_map[session_id1] = endpoint1;
		this->session_endpoint_map[session_id2] = endpoint2;
		this->session_endpoint_map[session_id3] = endpoint3;
		this->session_lasttime_map[session_id1] = time1;
		this->session_lasttime_map[session_id2] = time2;
		this->session_lasttime_map[session_id3] = time3;
		this->lasttime_session_map.insert(std::make_pair(time1, session_id1));
		this->lasttime_session_map.insert(std::make_pair(time2, session_id2));
		this->lasttime_session_map.insert(std::make_pair(time3, session_id3));
		boost::thread_group thread_group;
		thread_group.create_thread(boost::bind(
						&sslid_session_data_processor_test_class::get_endpoint_from_session_data_thread,
						this, session_id1, endpoint1));
		thread_group.create_thread(boost::bind(
						&sslid_session_data_processor_test_class::get_endpoint_from_session_data_thread,
						this, session_id2, default_endpoint));
		thread_group.create_thread(boost::bind(
						&sslid_session_data_processor_test_class::get_endpoint_from_session_data_thread,
						this, session_id3, default_endpoint));
		thread_group.join_all();
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

	cout << "[18]------------------------------------------" << endl;
		// unit_test[18] maxlistが０の場合、戻り値が正常（０）で設定する。
		this->session_endpoint_map.clear();
		this->session_lasttime_map.clear();
		this->lasttime_session_map.clear();
		this->maxlist = 0;
		result = this->write_session_data(session_id, new_saved_endpoint, write_time);
		BOOST_CHECK_EQUAL(result, 0);

    cout << "[19]------------------------------------------" << endl;
		// unit_test[19] mapが空の場合、例外が発生しない。
		try{
			this->maxlist = 3;
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

    cout << "[20]------------------------------------------" << endl;
		// unit_test[20] mapにセッションIDが存在している場合、戻り値が正常（０）で設定する。
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

    cout << "[21]------------------------------------------" << endl;
		// unit_test[21] mapにセッションIDが存在して、session_endpoint_mapのサイズがmaxlist - 1の場合、戻り値が正常（０）で設定する。
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

    cout << "[22]------------------------------------------" << endl;
		// unit_test[22] mapにセッションIDが存在して、session_endpoint_mapのサイズがmaxlistの場合、戻り値が正常（０）で設定する。
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

    cout << "[23]------------------------------------------" << endl;
		// unit_test[23] mapにセッションIDが存在しない、session_endpoint_mapのサイズがmaxlist - 1の場合、戻り値が正常（０）で設定する。
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

    cout << "[24]------------------------------------------" << endl;
		// unit_test[24] mapにセッションIDが存在しない、session_endpoint_mapのサイズがmaxlistの場合、戻り値が正常（０）で設定する。
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

	cout << "[25]------------------------------------------" << endl;
		// unit_test[25]  mapにセッションIDが存在しない、がつ　session_endpoint_mapのサイズがmaxlist、がつ　clear_expired_session_data（）の戻る値が1場合、
		// unit_test[25]  戻り値が正常（0）で設定する。
		clear_function_return_fail = true;
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
		clear_function_return_fail = false;
		BOOST_CHECK_EQUAL(result, 0);

    cout << "[26]------------------------------------------" << endl;
		// unit_test[26] mapにセッションIDが存在しなくて、且つsession_endpoint_mapのサイズが maxlistで１つのデータがタイムアウトの場合
		// unit_test[26] 戻り値が正常（０）で設定する。
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

    cout << "[27]------------------------------------------" << endl;
		// unit_test[27] mapにセッションIDが存在しなくて、且つsession_endpoint_mapのサイズが maxlistで１つのデータがタイムアウトの場合
		// unit_test[27] 戻り値が正常（０）で設定する。
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

    cout << "[28]------------------------------------------" << endl;
		// unit_test[28] マルチスレッドの場合、mapにデータを正常追加する
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

			std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator it1 = this->session_endpoint_map.begin();
			std::map<std::string, time_t>::iterator it2 =  this->session_lasttime_map.begin();
			std::multimap<time_t, std::string>::iterator it3 = this->lasttime_session_map.begin();
			BOOST_CHECK_EQUAL(this->session_endpoint_map.size(), 3u);
			BOOST_CHECK_EQUAL(this->session_lasttime_map.size(), 3u);
			BOOST_CHECK_EQUAL(this->lasttime_session_map.size(), 3u);
			for(; it1 != this->session_endpoint_map.end(); it1++){
				BOOST_CHECK_EQUAL(it1->second, new_saved_endpoint);
			}
			for(; it2 != this->session_lasttime_map.end(); it2++){
				BOOST_CHECK_EQUAL(it2->second, write_time);
			}
			for(; it3 != this->lasttime_session_map.end(); it3++){
				BOOST_CHECK_EQUAL(it3->first, write_time);
			}
		} catch(...){
			BOOST_ERROR("exception: write_session_data");
		}

    cout << "[29]------------------------------------------" << endl;
		// unit_test[29] endpointがipv6で、mapにセッションIDが存在する場合、戻り値が正常（０）で設定する。
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

    cout << "[30]------------------------------------------" << endl;
		// unit_test[30] mapが空の場合、戻り値が失敗（１）で設定する。
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
		BOOST_CHECK_EQUAL(result, 1);

    cout << "[31]------------------------------------------" << endl;
		// unit_test[31] mapに全てのアイテムがタイムアウトの場合、全てのアイテムを削除する。
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

    cout << "[32]------------------------------------------" << endl;
		// unit_test[32] mapに１つのアイテムがタイムアウトで、もう１つのアイテムがタイムアウトしない場合、タイムアウトのアイテムだけ削除する。
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

    cout << "[33]------------------------------------------" << endl;
		// unit_test[33] mapに全てのアイテムがタイムアウトしない場合、一番古いアイテムを削除する。
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

    cout << "[34]------------------------------------------" << endl;
		// unit_test[34] mapに全てのアイテムがタイムアウトしない場合、lasttimeが全て同じの場合、１つ目のアイテムを削除する。
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
		unsigned short port2 = -999;
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

    cout << "[35]------------------------------------------" << endl;
		// unit_test[35] パラメータがNULLの場合、戻り値が異常（-1）で設定する。
		result = 10;
		try {
			result = this->read_session_data_from_replication_area(NULL);
		} catch (...){
			BOOST_ERROR("exception: read_session_data_from_replication_area");
		}
		// function result check
		BOOST_CHECK_EQUAL(result, -1);

    cout << "[36]------------------------------------------" << endl;
		// unit_test[36] maxlistが１で、５つデータが存在して、パラメータがfirst_real_data の場合、replicationエリアから１つ目のデータを取得する。
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

    cout << "[37]------------------------------------------" << endl;
		// unit_test[37] maxlistが１で、５つデータが存在して、パラメータがfirst_real_data + 1の場合、replicationエリアから２つ目のデータを取得する。
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

    cout << "[38]------------------------------------------" << endl;
		// unit_test[38] maxlistが１で、５つデータが存在して、パラメータがfirst_real_data + ２で 、対応するvalidが0の場合, mapのサイズが０である。
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

    cout << "[39]------------------------------------------" << endl;
		// unit_test[39] maxlistが２の場合、mapに２件のデータ読み込む。
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

    cout << "[40]------------------------------------------" << endl;
		// unit_test[40] maxlistが２で、２つ目データのvalidが0の場合、１件目のみ読み込む。
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

    cout << "[41]------------------------------------------" << endl;
		// unit_test[41] maxlistが２で、１つ目データのvalidが0の場合、２件目のみ読み込む。
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

    cout << "[42]------------------------------------------" << endl;
		// unit_test[42] realserverがipv6の場合、取得のipが正しいです。
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
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
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

	delete []replication_data_area;
}

// get_endpoint_from_session_data test
void get_endpoint_from_session_data_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char *replication_data_area = new char[data_area_size];
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
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
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
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
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
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
	bool bret = (replication_data_area != NULL);
	BOOST_REQUIRE_EQUAL(bret, true);
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
