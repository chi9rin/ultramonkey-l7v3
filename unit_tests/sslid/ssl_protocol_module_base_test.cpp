/*
 * ssl_protocol_module_base_test.cpp
 *
 *  Created on: Jan 20, 2009
 *      Author: root
 */
#include "sslid_to_be_test_file.h"

using namespace boost::unit_test;
using namespace l7vs;

//get_ssl_session_id
void get_ssl_session_id_test(){
	protocol_module_sslid ssl_base;
	std::string session_id;
	int ret = 0;
	char *record_data;

	// unit_test[1]
	ret = ssl_base.get_ssl_session_id(NULL, 0, session_id);
	BOOST_CHECK_EQUAL(ret, -1);

	// unit_test[2]
	record_data = new char[76];
	record_data[43] = 0x20;
	ret = ssl_base.get_ssl_session_id(record_data, 76, session_id);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[3]
	record_data = new char[76];
	record_data[43] = 0x00;
	ret = ssl_base.get_ssl_session_id(record_data, 76, session_id);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[4]
	record_data = new char[100];
	record_data[43] = 0x20;
	ret = ssl_base.get_ssl_session_id(record_data, 100, session_id);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[5]
	record_data = new char[100];
	record_data[43] = 0x00;
	ret = ssl_base.get_ssl_session_id(record_data, 100, session_id);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;
}

//check_ssl_record_sendable
void check_ssl_record_sendable(){
	protocol_module_sslid ssl_base;
	bool is_message_from_client;
	char* record_data;
	int recv_length;
	int all_length;
	int check_all_length;
	bool is_hello_message;
	int ret;
	unsigned short high_length;
	unsigned short low_length;

	// unit_test[6]
	// condition:
	//  recv_length < 6
	// check:
	//  check_ssl_record_sendable() return 1 (送信不可)
	recv_length = 5;
	record_data = new char[recv_length];
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 1);
	delete record_data;

	// unit_test[7]
	// condition:
	//  recv_length = 6, record_data is not a ssl record
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 6;
	record_data = new char[recv_length];
	record_data[0] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[8]
	// condition:
	//  recv_length = 6, record_data is a ssl record(minimal size), but is not a hello message.
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 6;
	record_data = new char[recv_length];
	record_data[0] = 0x17;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x01;
	record_data[5] = 0x00;
	all_length = 0;
	is_hello_message = true;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[9]
	// condition:
	//  recv_length = 6, record_data is a ssl record(minimal size), but is not a hello message.
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 6;
	record_data = new char[recv_length];
	record_data[0] = 0x17;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x01;
	record_data[5] = 0x00;
	all_length = 0;
	is_hello_message = true;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[10]
	// condition:
	//  recv_length = 6, record_data is a ssl record(minimal size), and is a hello message.
	// check:
	//  check_ssl_record_sendable() return 1 (送信不可)
	recv_length = 6;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x01;
	record_data[5] = 0x01;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 1);
	delete record_data;

	// unit_test[11]
	// condition:
	//  recv_length = 6, record_data is a ssl record(minimal size), and is a hello message.
	// check:
	//  check_ssl_record_sendable() return 1 (送信不可)
	recv_length = 6;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x01;
	record_data[5] = 0x01;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 1);
	delete record_data;

	// unit_test[12]
	// condition:
	//  recv_length > 6, record_data is  not ssl record data
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 10;
	record_data = new char[recv_length];
	record_data[0] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[13]
	// condition:
	//  recv_length > 6, record_data is ssl record data, but is not a hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 10;
	record_data = new char[recv_length];
	record_data[0] = 0x17;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x05;
	record_data[5] = 0x00;
	is_hello_message = true;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[14]
	// condition:
	//  recv_length > 6, record_data is ssl record data, but is not a hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 10;
	record_data = new char[recv_length];
	record_data[0] = 0x17;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x05;
	record_data[5] = 0x00;
	is_hello_message = true;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[15]
	// condition:
	//  recv_length > 6, recv_length < 76, record_data is ssl record data, is a hello message
	// check:
	//  check_ssl_record_sendable() return 1 (送信不可)
	recv_length = 10;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x05;
	record_data[5] = 0x01;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 1);
	delete record_data;

	// unit_test[16]
	// condition:
	//  recv_length > 6, recv_length < 76, record_data is ssl record data, is a hello message
	// check:
	//  check_ssl_record_sendable() return 1 (送信不可)
	recv_length = 10;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x05;
	record_data[5] = 0x01;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, 1);
	delete record_data;

	// unit_test[17]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message,
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01;
	record_data[9] = 0x00;
	record_data[10] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[18]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message,
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01;
	record_data[9] = 0x00;
	record_data[10] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[19]
	// condition:
	// 	recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[20]
	// condition:
	// 	recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[21]
	// condition:
	// 	recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[22]
	// condition:
	// 	recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[23]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[24]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[25]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[26]
	// condition:
	//  recv_length = 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[27]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[28]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[29]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[30]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[31]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[32]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[33]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[34]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 76;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x47;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[35]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message,
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01;
	record_data[9] = 0x00;
	record_data[10] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[36]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message,
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01;
	record_data[9] = 0x00;
	record_data[10] = 0x00;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[37]
	// condition:
	// 	recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[38]
	// condition:
	// 	recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[39]
	// condition:
	// 	recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[40]
	// condition:
	// 	recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[41]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[42]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[43]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[44]
	// condition:
	//  recv_length > 76, record_data is ssl record data, but is a error hello message
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = true;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(!is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;

	// unit_test[45]
	// condition:
	//  recv_length > 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[46]
	// condition:
	//  recv_length > 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[47]
	// condition:
	//  recv_length > 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[48]
	// condition:
	//  recv_length > 76, record_data is ssl record data, and is client hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x01; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = true;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[49]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[50]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x01;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[51]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x01;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[52]
	// condition:
	//  recv_length = 76, record_data is ssl record data, and is server hello message
	// check:
	//  check_ssl_record_sendable() return 0 (送信可能)
	recv_length = 100;
	record_data = new char[recv_length];
	record_data[0] = 0x16;
	record_data[1] = 0x03;
	record_data[2] = 0x00;
	record_data[3] = 0x00;
	record_data[4] = 0x64;
	record_data[5] = 0x02; // client hello = 0x01, server hello = 0x02
	record_data[9] = 0x03;
	record_data[10] = 0x00;
	is_message_from_client = false;
	is_hello_message = false;
	all_length = 0;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
	check_all_length = (high_length | low_length) + 5;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data,
			recv_length, all_length, is_hello_message);
	BOOST_CHECK(is_hello_message);
	BOOST_CHECK_EQUAL(all_length, check_all_length);
	BOOST_CHECK_EQUAL(ret, 0);
	delete record_data;

	// unit_test[53]
	// condition:
	//  record_data = NULL
	// check:
	//  check_ssl_record_sendable() return -1 (異常)
	record_data = NULL;
	ret = ssl_base.check_ssl_record_sendable(is_message_from_client, record_data, 0,
			all_length, is_hello_message);
	BOOST_CHECK_EQUAL(ret, -1);
	delete record_data;
}

void ssl_protocol_module_base_test_main()
{
	test_suite* ts = BOOST_TEST_SUITE( "ssl_protocol_module_base_ut" );
	ts->add( BOOST_TEST_CASE( &get_ssl_session_id_test ) );
	ts->add( BOOST_TEST_CASE( &check_ssl_record_sendable ) );
	framework::master_test_suite().add( ts );
}
