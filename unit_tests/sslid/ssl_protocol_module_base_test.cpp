#include "sslid_to_be_test_file.h"

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

class ssl_protocol_module_base_test_class: public ssl_protocol_module_base {
public:
	ssl_protocol_module_base_test_class() :
		ssl_protocol_module_base("sslid") {

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
			boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,
					MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) {
		return STOP;
	}
	EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id,
			boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) {
		return STOP;
	}
	EVENT_TAG handle_realserver_connection_fail(
			const boost::thread::id thread_id,
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
			boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) {
		return STOP;
	}
	EVENT_TAG handle_sorryserver_connection_fail(
			const boost::thread::id thread_id,
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
			boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) {
		return STOP;
	}
	EVENT_TAG handle_client_select(const boost::thread::id thread_id,
			boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,
					MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) {
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

		const int MIN_RECV_LENGTH = 76;
		const int SSL_SESSION_ID_FLAG = 43;
		std::string session_id;
		int ret = 0;
		char *record_data;

		// unit_test[1] condition: recv_length = 0, record_data = NULL
		// unit_test[1] check: get_ssl_session_id() return -1 (error)
		record_data = NULL;
		ret = this->get_ssl_session_id(NULL, 0, session_id);
		BOOST_CHECK_EQUAL(ret, -1);

		// unit_test[2] condition: recv_length = MIN_RECV_LENGTH, record_data != NULL, record_data has a session id
		// unit_test[2] check: get_ssl_session_id() return 0 (get a session id successed!)
		record_data = new char[MIN_RECV_LENGTH];
		record_data[SSL_SESSION_ID_FLAG] = 0x20;
		ret = this->get_ssl_session_id(record_data, MIN_RECV_LENGTH, session_id);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[3] condition: recv_length = MIN_RECV_LENGTH, record_data != NULL, record_data has not a session id
		// unit_test[3] check: get_ssl_session_id() return 1 (get a session id failed!)
		record_data = new char[MIN_RECV_LENGTH];
		record_data[SSL_SESSION_ID_FLAG] = 0x00;
		ret = this->get_ssl_session_id(record_data, MIN_RECV_LENGTH, session_id);
		BOOST_CHECK_EQUAL(ret, 1);
		delete[] record_data;

		// unit_test[4] condition: recv_length > MIN_RECV_LENGTH, record_data != NULL, record_data has a session id
		// unit_test[4] check: get_ssl_session_id() return 0 (get a session id successed!)
		record_data = new char[MIN_RECV_LENGTH+1];
		record_data[SSL_SESSION_ID_FLAG] = 0x20;
		ret = this->get_ssl_session_id(record_data, MIN_RECV_LENGTH+1, session_id);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[5] condition: recv_length > MIN_RECV_LENGTH, record_data != NULL, record_data has not a session id
		// unit_test[5] check: get_ssl_session_id() return 1 (get a session id failed!)
		record_data = new char[MIN_RECV_LENGTH+1];
		record_data[SSL_SESSION_ID_FLAG] = 0x00;
		ret = this->get_ssl_session_id(record_data, MIN_RECV_LENGTH+1, session_id);
		BOOST_CHECK_EQUAL(ret, 1);
		delete[] record_data;
	}

	//check_ssl_record_sendable
	void check_ssl_record_sendable_test() {

		bool is_message_from_client;
		char* record_data;
		int recv_length;
		int all_length;
		bool is_hello_message;
		int ret;

		// unit_test[6] condition: recv_length < 0, record_data = NULL, is_message_from_client = false
		// unit_test[6] check: check_ssl_record_sendable() return -1 (error)
		recv_length = -1;
		record_data = NULL;
		is_message_from_client = false;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);

		// unit_test[7] condition: 0 < recv_length < 6, record_data != NULL, is_message_from_client = false
		// unit_test[7] check: check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 5;
		record_data = new char[recv_length];
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 1);
		delete[] record_data;

		// unit_test[8] condition: recv_length < 6, record_data != NULL, is_message_from_client = true
		// unit_test[8] check: check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 5;
		record_data = new char[recv_length];
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 1);
		delete[] record_data;

		// unit_test[9] condition: recv_length = 6, record_data != NULL, is_message_from_client = true, record_data is not a ssl record
		// unit_test[9] check: check_ssl_record_sendable() return -1 (異常)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[10] condition: recv_length = 6, record_data != NULL, is_message_from_client = false, record_data is not a ssl record
		// unit_test[10] check: check_ssl_record_sendable() return -1 (異常)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[11] condition: recv_length = 6, is_message_from_client = true, record_data is a ssl record(minimal size), but is not a hello message.
		// unit_test[11] check: check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		all_length = 0;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[12] condition: recv_length = 6, is_message_from_client = false, record_data is a ssl record(minimal size), but is not a hello message.
		// unit_test[12] check: check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		all_length = 0;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[13] condition: recv_length = 6, is_message_from_client = true, record_data is a ssl record(minimal size), but is not a hello message.
		// unit_test[13] check: check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		all_length = 0;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[14] condition: recv_length = 6, is_message_from_client = false, record_data is a ssl record(minimal size), but is not a hello message.
		// unit_test[14] check: check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 6;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		all_length = 0;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[15] condition: recv_length = 6, is_message_from_client = true ,record_data is a ssl record(minimal size), and is a hello message.
		// unit_test[15] check: check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 6;
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

		// unit_test[16] condition: recv_length = 6, is_message_from_client = false ,record_data is a ssl record(minimal size), and is a hello message.
		// unit_test[16] check: check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 6;
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

		// unit_test[17] condition:recv_length = 6, is_message_from_client = true ,record_data is a ssl record(minimal size), and is a hello message.
		// unit_test[17] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 6;
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

		// unit_test[18] condition:recv_length = 6, is_message_from_client = false ,record_data is a ssl record(minimal size), and is a hello message.
		// unit_test[18] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 6;
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

		// unit_test[19] condition:recv_length > 6, is_message_from_client = true ,record_data is  not ssl record data
		// unit_test[19] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[20] condition:recv_length > 6, is_message_from_client = false ,record_data is  not ssl record data
		// unit_test[20] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[21] condition:recv_length > 6, is_message_from_client = true ,record_data is ssl record data, but is not a hello message
		// unit_test[21] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[22] condition:recv_length > 6, is_message_from_client = false ,record_data is ssl record data, but is not a hello message
		// unit_test[22] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[23] condition:recv_length > 6, is_message_from_client = true, record_data is ssl record data, but is not a hello message
		// unit_test[23] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[24] condition:recv_length > 6, is_message_from_client = false, record_data is ssl record data, but is not a hello message
		// unit_test[24] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 10;
		record_data = new char[recv_length];
		record_data[0] = 0x17;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[25] condition:recv_length > 6, recv_length < 76, is_message_from_client= true,record_data is ssl record data, is a hello message
		// unit_test[25] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 10;
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

		// unit_test[26] condition:recv_length > 6, recv_length < 76, is_message_from_client= false,record_data is ssl record data, is a hello message
		// unit_test[26] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 10;
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

		// unit_test[27] condition:recv_length > 6, recv_length < 76, is_message_from_client = false,record_data is ssl record data, is a hello message
		// unit_test[27] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 10;
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

		// unit_test[28] condition:recv_length > 6, recv_length < 76, is_message_from_client = true, record_data is ssl record data, is a hello message
		// unit_test[28] check:check_ssl_record_sendable() return 1 (送信不可)
		recv_length = 10;
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

		// unit_test[29] condition:recv_length = 76,is_message_from_client = true, record_data is ssl record data, but is a error hello message,
		// unit_test[29] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[30] condition:recv_length = 76, is_message_from_client = false,record_data is ssl record data, but is a error hello message,
		// unit_test[30] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[31] condition:recv_length = 76, record_data is ssl record data, but is a error hello message,
		// unit_test[31] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[32] condition:recv_length = 76, is_message_from_client = true,record_data is ssl record data, but is a error hello message,
		// unit_test[32] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[33] condition:recv_length = 76, is_message_from_client = false,record_data is ssl record data, but is a error hello message,
		// unit_test[33] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[34] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[34] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[35] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[35] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[36] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[36] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[37] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[37] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[38] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[38] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[39] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[39] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[40] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[40] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[41] condition:recv_length = 76, record_data is ssl record data, but is a error hello message
		// unit_test[41] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[42] condition:recv_length = 76, record_data is ssl record data, and is client hello message
		// unit_test[42] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = false;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[43] condition:recv_length = 76, record_data is ssl record data, and is client hello message
		// unit_test[43] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[44] condition:recv_length = 76, record_data is ssl record data, and is client hello message
		// unit_test[44] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[45] condition:recv_length = 76, record_data is ssl record data, and is client hello message
		// unit_test[45] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[46] condition:recv_length = 76, record_data is ssl record data, and is server hello message
		// unit_test[46] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[47] condition:recv_length = 76, record_data is ssl record data, and is server hello message
		// unit_test[47] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[48] condition:recv_length = 76, record_data is ssl record data, and is server hello message
		// unit_test[48] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[49] condition:recv_length = 76, record_data is ssl record data, and is server hello message
		// unit_test[49] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[50] condition:recv_length > 76, is_message_from_client = true,record_data is ssl record data, but is a error hello message,
		// unit_test[50] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[51] condition:recv_length > 76, is_message_from_client = false,record_data is ssl record data, but is a error hello message,
		// unit_test[51] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_hello_message = true;
		is_message_from_client = false;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[52] condition:recv_length > 76, is_message_from_client = true, record_data is ssl record data, but is a error hello message,
		// unit_test[52] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[53] condition:recv_length > 76, is_message_from_client = false, record_data is ssl record data, but is a error hello message,
		// unit_test[53] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01;
		record_data[9] = 0x00;
		record_data[10] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[54] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[54] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[55] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[55] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[56] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[56] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[57] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[57] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = false;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[58] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[58] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[59] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[59] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[60] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[60] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[61] condition:recv_length > 76, record_data is ssl record data, but is a error hello message
		// unit_test[61] check:check_ssl_record_sendable() return -1 (異常)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x00;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x00;
		is_message_from_client = true;
		is_hello_message = true;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(!is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;

		// unit_test[62] condition:recv_length > 76, record_data is ssl record data, and is client hello message
		// unit_test[62] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
		record_data = new char[recv_length];
		record_data[0] = 0x16;
		record_data[1] = 0x03;
		record_data[2] = 0x01;
		record_data[3] = 0x00;
		record_data[4] = 0x9e;
		record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
		record_data[9] = 0x03;
		record_data[10] = 0x01;
		is_message_from_client = true;
		is_hello_message = false;
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[63] condition:recv_length > 76, record_data is ssl record data, and is client hello message
		// unit_test[63] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[64] condition:recv_length > 76, record_data is ssl record data, and is client hello message
		// unit_test[64] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[65] condition:recv_length > 76, record_data is ssl record data, and is client hello message
		// unit_test[65] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[66] condition:recv_length > 76, record_data is ssl record data, and is server hello message
		// unit_test[66] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[67] condition:recv_length > 76, record_data is ssl record data, and is server hello message
		// unit_test[67] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[68] condition:recv_length > 76, record_data is ssl record data, and is server hello message
		// unit_test[68] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[69] condition:recv_length > 76, record_data is ssl record data, and is server hello message
		// unit_test[69] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = 100;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[70] condition:recv_length = MAX_BUFFER_SIZE + 76, record_data is ssl record data, and is server hello message
		// unit_test[70] check:check_ssl_record_sendable() return 0 (送信可能)
		recv_length = MAX_BUFFER_SIZE + 76;
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
		all_length = 0;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, recv_length, all_length, is_hello_message);
		BOOST_CHECK(is_hello_message);
		BOOST_CHECK_EQUAL(all_length, 163);
		BOOST_CHECK_EQUAL(ret, 0);
		delete[] record_data;

		// unit_test[71] condition:record_data = NULL
		// unit_test[71] check:check_ssl_record_sendable() return -1 (異常)
		record_data = NULL;
		ret = this->check_ssl_record_sendable(is_message_from_client,
				record_data, 0, all_length, is_hello_message);
		BOOST_CHECK_EQUAL(ret, -1);
		delete[] record_data;
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

void ssl_protocol_module_base_test_main() {
	test_suite* ts = BOOST_TEST_SUITE( "ssl_protocol_module_base_ut" );
	ts->add(BOOST_TEST_CASE( &get_ssl_session_id_test ));
	ts->add(BOOST_TEST_CASE( &check_ssl_record_sendable_test ));
	framework::master_test_suite().add(ts);
}
