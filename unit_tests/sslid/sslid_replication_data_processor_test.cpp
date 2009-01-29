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

int test_value = 0;
// function for testing register_replication_area_lock
void lock_function(){
	test_value = 1;
}

// function for testing register_replication_area_unlock
void unlock_function(){
	test_value = 2;
}

// function sslid_replication_data_processor test
void sslid_replication_data_processor_test(){

	int data_area_size = 128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data);
	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	int maxlist_test = 5;
	char *header_start_address = replication_data_area;
	char *real_data_start_address = header_start_address + 128*sizeof(struct l7vs::sslid_replication_data_header);
	l7vs::sslid_replication_data_header* first_header = (sslid_replication_data_header*)header_start_address;

	l7vs::sslid_replication_data_processor test_object(maxlist_test, replication_data_area, SECTION_NUMBER);

	// test_case[1] maxlist value chek
	BOOST_CHECK_EQUAL(maxlist_test, test_object.maxlist);
	// max_temp_list value check
	BOOST_CHECK_EQUAL(maxlist_test * 2, test_object.max_temp_list);
	// replication_area value check
	BOOST_CHECK_EQUAL(replication_data_area, real_data_start_address);

	// test_case[2] NULL pointer test
	try {
		l7vs::sslid_replication_data_processor test_object1(maxlist_test, NULL, 0);
		BOOST_CHECK(test_object1.replication_area == NULL);
	} catch (...) {
		BOOST_ERROR("exception: sslid_replication_data_processor");
	}

	// test_case[3] if over replication area, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = sizeof(l7vs::sslid_replication_data);
		}
		l7vs::sslid_replication_data_processor replication_data_processor2(2, replication_data_area, 1);
	} catch (...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}

	// test_case[4] when virtual service data has exist and the virtual service's data area is samller than needed,
	// need to resize the virtual service session information, test
	int max_list_size = 2;
	memset(replication_data_area, 0, data_area_size);
	// TODO zhangzg 2008/01/22 how to set the heder's ip and port need a reference(ip && port is the virtual service' key)
	first_header->virtualserver_addr.s_addr = inet_addr("192.168.120.102");
	first_header->virtualserver_port = 80;
	first_header->size = (max_list_size - 1) * sizeof(l7vs::sslid_replication_data);
	first_header->offset = real_data_start_address - header_start_address;
	l7vs::sslid_replication_data_processor replication_data_processor3(max_list_size, replication_data_area, SECTION_NUMBER);
	// the virtual service's size is resized, check it
	int expecting_size = (int)(max_list_size * sizeof(l7vs::sslid_replication_data));
	BOOST_CHECK_EQUAL(first_header->size, expecting_size);

	// test_case[5] when resize the virtual service session information, if header area is full, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = sizeof(l7vs::sslid_replication_data);
		}
		// TODO zhangzg 2008/01/22 how to set the heder's ip and port need a reference(ip && port is the virtual service' key)
		first_header->virtualserver_addr.s_addr = inet_addr("192.168.120.102");
		first_header->virtualserver_port = 80;
		first_header->size = (max_list_size - 1) * sizeof(l7vs::sslid_replication_data);
		first_header->offset = real_data_start_address - header_start_address;
		l7vs::sslid_replication_data_processor replication_data_processor4(max_list_size, replication_data_area, SECTION_NUMBER);
	} catch (...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}

	// test_case[6] when resize the virtual service session information, if header area is not full,
	// virtual service session information is changed, test it
	memset(replication_data_area, 0, data_area_size);
	// TODO zhangzg 2008/01/22 how to set the heder's ip and port need a reference(ip && port is the virtual service' key)
	first_header->virtualserver_addr.s_addr = inet_addr("192.168.120.102");
	first_header->virtualserver_port = 80;
	first_header->size = (max_list_size - 1) * sizeof(l7vs::sslid_replication_data);
	first_header->offset = real_data_start_address - header_start_address;
	l7vs::sslid_replication_data_processor replication_data_processor5(max_list_size, replication_data_area, SECTION_NUMBER);
	// the virtual service information's size is resized, get it
	expecting_size = (int)(max_list_size * sizeof(l7vs::sslid_replication_data));
	// the virtual service information's offset is resized, get it
	int expecting_offset = real_data_start_address - header_start_address + expecting_size;
	// size check
	BOOST_CHECK_EQUAL(first_header->size, expecting_size);
	// offset check
	BOOST_CHECK_EQUAL(first_header->offset, expecting_offset);

	// test_case[6] when virtual service data is not exist and the sslip replication area there is
	// not enough sapce to add data, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = max_list_size * sizeof(l7vs::sslid_replication_data);
			temp->virtualserver_addr.s_addr = inet_addr("255.255.255.255");
			if(i == 127) {
				temp->size = 0;
			}
		}
		l7vs::sslid_replication_data_processor replication_data_processor6(max_list_size, replication_data_area, SECTION_NUMBER);
		l7vs::sslid_session_data_processor test_object6(max_list_size, 0, &replication_data_processor6);
	} catch (...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}

	// test_case[7] when virtual service data is not exist and the sslip replication area there is
	// enough sapce to add data, add the data to the replication area, test it
	memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
	l7vs::sslid_replication_data_processor replication_data_processor7(max_list_size, replication_data_area, SECTION_NUMBER);
	expecting_size = (int)(max_list_size * sizeof(l7vs::sslid_replication_data));
	expecting_offset = real_data_start_address - header_start_address;
	// session information's size check
	BOOST_CHECK_EQUAL(first_header->size, expecting_size);
	// session information's offset check
	BOOST_CHECK_EQUAL(first_header->offset, expecting_size);

	// test_case[8] when virtual service data is not exist and the sslip replication header area
	// is full, nothing to do, test it
	try {
		memset(replication_data_area, 0, 128*sizeof(struct l7vs::sslid_replication_data_header));
		for(int i = 0; i < 128; i++){
			l7vs::sslid_replication_data_header* temp= ((l7vs::sslid_replication_data_header*)replication_data_area) + i;
			temp->size = sizeof(l7vs::sslid_replication_data);
			temp->virtualserver_addr.s_addr = inet_addr("255.255.255.255");
		}
		l7vs::sslid_replication_data_processor replication_data_processor8(max_list_size, replication_data_area, SECTION_NUMBER);
		l7vs::sslid_session_data_processor test_object8(max_list_size, 0, &replication_data_processor8);
	} catch (...) {
		BOOST_ERROR("exception: sslid_session_data_processor");
	}
}

// function put_into_temp_list test
void put_into_temp_list_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	time_t last_time = time(0);
	boost::asio::ip::tcp::endpoint endpoint;

	l7vs::sslid_replication_temp_data test_data1;
	test_data1.session_id = "test_id1";
	test_data1.last_time = last_time;
	test_data1.op_code = 'A';
	test_data1.realserver_addr = endpoint;

	l7vs::sslid_replication_temp_data test_data2;
	test_data2.session_id = "test_id2";
	test_data2.last_time = last_time;
	test_data2.op_code = 'U';
	test_data2.realserver_addr = endpoint;

	l7vs::sslid_replication_temp_data test_data3;
	test_data3.session_id = "test_id3";
	test_data3.last_time = last_time;
	test_data3.op_code = 'D';
	test_data3.realserver_addr = endpoint;

	// test_case[8] element put into list test(list is not full and put inot 1 item)
	l7vs::sslid_replication_data_processor test_object(2, replication_data_area, SECTION_NUMBER);
	test_object.put_into_temp_list(test_data1);
	l7vs::sslid_replication_temp_data& temp_data = test_object.temp_list.back();
	std::string expecting_session_id = "test_id1";
	// the item which put into the temp_list check
	// session_id check
	BOOST_CHECK_EQUAL(temp_data.session_id, expecting_session_id);
	// last_time check
	BOOST_CHECK_EQUAL(temp_data.last_time, last_time);
	// op_code check
	BOOST_CHECK_EQUAL(temp_data.op_code, 'A');
	// realserver_addr check
	BOOST_CHECK_EQUAL(temp_data.realserver_addr, endpoint);

	// test_case[9] element put into list test(put into 2 items)
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data2);
	test_object.put_into_temp_list(test_data3);
	l7vs::sslid_replication_temp_data& expecting_data1 = test_object.temp_list.front();
	l7vs::sslid_replication_temp_data& expecting_data2 = test_object.temp_list.back();
	// the items which put into the temp_list check
	// session_id check
	BOOST_CHECK_EQUAL(expecting_data1.session_id, test_data2.session_id);
	BOOST_CHECK_EQUAL(expecting_data2.session_id, test_data3.session_id);
	// last_time check
	BOOST_CHECK_EQUAL(expecting_data1.last_time, test_data2.last_time);
	BOOST_CHECK_EQUAL(expecting_data2.last_time, test_data3.last_time);
	// op_code check
	BOOST_CHECK_EQUAL(expecting_data1.op_code, 'U');
	BOOST_CHECK_EQUAL(expecting_data2.op_code, 'D');
	// realserver_addr check
	BOOST_CHECK_EQUAL(expecting_data1.realserver_addr, test_data2.realserver_addr);
	BOOST_CHECK_EQUAL(expecting_data2.realserver_addr, test_data3.realserver_addr);

	// TODO zhangzg 2008/01/20 when list is full, how to test?(multi-ply thread)
}

// function write_replicaion_area test
void write_replicaion_area_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	time_t test_time = time(0);
	l7vs::sslid_replication_data_processor test_object(1, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_replication_temp_data test_data1;
	boost::function<void(void)> function_pointer;
	boost::asio::ip::tcp::endpoint realserver_addr;

	// test_case[10] when temp_list is empty, test it
	try {
		test_object.temp_list.clear();
		boost::thread test_thread1(boost::bind(&l7vs::sslid_replication_data_processor::write_replicaion_area, &test_object));
		test_thread1.timed_join(boost::posix_time::milliseconds(100));
	} catch(...) {
		BOOST_ERROR("exception: write_replicaion_area");
	}

	// test_case[11] data add test(when data area has one sslip_replication_data)
	l7vs::sslid_replication_data temp_data;
	std::string temp_session_id = "temp_id1";
	memset(test_object.replication_area, 0, 3*sizeof(struct l7vs::sslid_replication_data));
	temp_data.last_time = time(0);
	memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
	temp_data.valid = 1;
	memcpy(test_object.replication_area, &temp_data, sizeof(struct l7vs::sslid_replication_data));
	// set the added data
	realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
	realserver_addr.port(80);
	test_data1.session_id = "test_id1";
	test_data1.last_time = test_time;
	test_data1.op_code = 'A';
	test_data1.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	// get the existing data
	l7vs::sslid_replication_data old_data;
	memset(&old_data, 0, sizeof(sslid_replication_data));
	memcpy(&old_data, test_object.replication_area, sizeof(sslid_replication_data));
	boost::thread test_thread2(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread2.timed_join(boost::posix_time::milliseconds(100));
	// data add position: after the old existing data, check it
	// get the added data information
	std::string added_session_id((test_object.replication_area + 1)->session_id);
	std::string added_address_ip(inet_ntoa((test_object.replication_area + 1)->realserver_addr.sin_addr));
	unsigned short added_address_port = ntohs((test_object.replication_area + 1)->realserver_addr.sin_port);
	// the old existing data not changed, check it
	int compare_result = memcmp(&old_data, test_object.replication_area, sizeof(sslid_replication_data));
	BOOST_CHECK_EQUAL(compare_result, 0);
	// added session_id check
	BOOST_CHECK_EQUAL(added_session_id, test_data1.session_id);
	// added last_time check
	BOOST_CHECK_EQUAL((test_object.replication_area + 1)->last_time, test_data1.last_time);
	// added valid flag check
	BOOST_CHECK_EQUAL((test_object.replication_area + 1)->valid, 1);
	// added realserver_addr ip check
	BOOST_CHECK_EQUAL(added_address_ip, test_data1.realserver_addr.address().to_string());
	// added realserver_addr port check
	BOOST_CHECK_EQUAL(added_address_port, test_data1.realserver_addr.port());

	// test_case[12] data add test(when data area has two sslip_replication_data,and the first sslip_replication_data's valid flag is unused)
	memset(test_object.replication_area, 0, 3*sizeof(struct l7vs::sslid_replication_data));
	temp_session_id = "temp_id2";
	temp_data.last_time = time(0);
	memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
	temp_data.valid = 0;
	memcpy(test_object.replication_area, &temp_data, sizeof(struct l7vs::sslid_replication_data));
	temp_session_id = "temp_id3";
	temp_data.last_time = time(0);
	memcpy(temp_data.session_id, temp_session_id.c_str(), temp_session_id.length());
	temp_data.valid = 1;
	memcpy(test_object.replication_area + 1, &temp_data, sizeof(struct l7vs::sslid_replication_data));
	// set the added data
	realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
	realserver_addr.port(80);
	test_data1.session_id = "test_id1";
	test_data1.last_time = test_time;
	test_data1.op_code = 'A';
	test_data1.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	// get the second existing data
	memset(&old_data, 0, sizeof(sslid_replication_data));
	memcpy(&old_data, test_object.replication_area + 1, sizeof(sslid_replication_data));
	boost::thread test_thread3(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread3.timed_join(boost::posix_time::milliseconds(100));
	// data add position: the first existing data position, check it
	// get the added data information
	added_session_id = std::string(test_object.replication_area->session_id);
	added_address_ip = std::string(inet_ntoa(test_object.replication_area->realserver_addr.sin_addr));
	added_address_port = ntohs(test_object.replication_area->realserver_addr.sin_port);
	// the second existing data not changed, check it
	compare_result = memcmp(&old_data, test_object.replication_area + 1, sizeof(sslid_replication_data));
	BOOST_CHECK_EQUAL(compare_result, 0);
	// added session_id check
	BOOST_CHECK_EQUAL(added_session_id, test_data1.session_id);
	// added last_time check
	BOOST_CHECK_EQUAL((test_object.replication_area)->last_time, test_data1.last_time);
	// added valid flag check
	BOOST_CHECK_EQUAL((test_object.replication_area)->valid, 1);
	// added realserver_addr ip check
	BOOST_CHECK_EQUAL(added_address_ip, test_data1.realserver_addr.address().to_string());
	// added realserver_addr port check
	BOOST_CHECK_EQUAL(added_address_port, test_data1.realserver_addr.port());

	// test_case[13] data add test(when data area has no sslip_replication_data)
	memset(test_object.replication_area, 0, 3*sizeof(struct l7vs::sslid_replication_data));
	realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
	realserver_addr.port(80);
	test_data1.session_id = "test_id1";
	test_data1.last_time = test_time;
	test_data1.op_code = 'A';
	test_data1.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	boost::thread test_thread4(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread4.timed_join(boost::posix_time::milliseconds(100));
	std::string saved_session_id(test_object.replication_area->session_id);
	std::string saved_address_ip(inet_ntoa(test_object.replication_area->realserver_addr.sin_addr));
	unsigned short saved_address_port = ntohs(test_object.replication_area->realserver_addr.sin_port);
	// session_id is saved, check it
	BOOST_CHECK_EQUAL(saved_session_id, std::string("test_id1"));
	// last_time is saved,  check it
	BOOST_CHECK_EQUAL(test_object.replication_area->last_time, test_time);
	// valid flag check
	BOOST_CHECK_EQUAL(test_object.replication_area->valid, 1);
	// realserver_addr ip check
	BOOST_CHECK_EQUAL(saved_address_ip, std::string("192.168.120.102"));
	// realserver_addr port check
	BOOST_CHECK_EQUAL(saved_address_port, 80);

	// test_case[14] data update test
	time_t update_time = test_time - 10;
	realserver_addr.address(boost::asio::ip::address::from_string("255.255.255.255"));
	realserver_addr.port(999);
	test_data1.op_code = 'U';
	test_data1.last_time = update_time;
	test_data1.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	boost::thread test_thread11(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread11.timed_join(boost::posix_time::milliseconds(100));
	// get saved information
	saved_session_id = std::string(test_object.replication_area->session_id);
	saved_address_ip = std::string(inet_ntoa(test_object.replication_area->realserver_addr.sin_addr));
	saved_address_port = ntohs(test_object.replication_area->realserver_addr.sin_port);
	// session_id is not changed, check it
	BOOST_CHECK_EQUAL(saved_session_id, std::string("test_id1"));
	// last_time is updated , check it
	BOOST_CHECK_EQUAL(test_object.replication_area->last_time, update_time);
	// valid flag is not changed ,check it
	BOOST_CHECK_EQUAL(test_object.replication_area->valid, 1);
	// realserver_addr ip is changed, check it
	BOOST_CHECK_EQUAL(saved_address_ip, std::string("255.255.255.255"));
	// realserver_addr port is changed, check it
	BOOST_CHECK_EQUAL(saved_address_port, 999);

	// test_case[15] data delete test(using above test_case saved data)
	realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
	realserver_addr.port(80);
	test_data1.op_code = 'D';
	test_data1.realserver_addr = realserver_addr;
	test_data1.last_time = test_time;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	boost::thread test_thread5(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread5.timed_join(boost::posix_time::milliseconds(100));
	// get saved information
	saved_session_id = std::string(test_object.replication_area->session_id);
	saved_address_ip = std::string(inet_ntoa(test_object.replication_area->realserver_addr.sin_addr));
	saved_address_port = ntohs(test_object.replication_area->realserver_addr.sin_port);
	// sslid_replication_data is deleted logically, valid flag is setted to 0, check it
	BOOST_CHECK_EQUAL(test_object.replication_area->valid, 0);
	// last_time is not updated , check it
	BOOST_CHECK_EQUAL(test_object.replication_area->last_time, update_time);
	// realserver_addr ip is not changed, check it
	BOOST_CHECK_EQUAL(saved_address_ip, std::string("255.255.255.255"));
	// realserver_addr port is not changed, check it
	BOOST_CHECK_EQUAL(saved_address_port, 999);

	// test_case[16] when op_code is out of 'A','U','D', not add test
	l7vs::sslid_replication_temp_data test_data2;
	memset(test_object.replication_area, 0, 3*sizeof(struct l7vs::sslid_replication_data));
	realserver_addr.address(boost::asio::ip::address::from_string("192.168.120.102"));
	realserver_addr.port(80);
	test_data2.session_id = "test_id2";
	test_data2.op_code = 'T';
	test_data2.last_time = test_time;
	test_data2.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data2);
	// get old session data
	l7vs::sslid_replication_data old_session_data[3];
	memcpy(&old_session_data, test_object.replication_area, 3*sizeof(sslid_replication_data));
	boost::thread test_thread6(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread6.timed_join(boost::posix_time::milliseconds(100));
	// test_data2 is not saved, so old data is not changed, check it
	// old data not changed check
	compare_result = memcmp(&old_session_data, test_object.replication_area, 3*sizeof(sslid_replication_data));
	BOOST_CHECK_EQUAL(compare_result, 0);

	// test_case[17] when op_code is out of 'A','U','D', not update, not delete test
	// add one data to the sslid replication data area, if the data not changed proving not update, not delete
	temp_session_id = "test_id3";
	memset(test_object.replication_area, 0, 3*sizeof(struct l7vs::sslid_replication_data));
	test_object.replication_area->last_time = test_time;
	memcpy(test_object.replication_area->session_id, temp_session_id.c_str(), temp_session_id.length());
	// set testing data
	l7vs::sslid_replication_temp_data test_data3;
	realserver_addr.address(boost::asio::ip::address::from_string("255.255.255.255"));
	realserver_addr.port(80);
	test_data3.session_id = temp_session_id;
	test_data3.op_code = 'T';
	test_data3.last_time = test_time;
	test_data3.realserver_addr = realserver_addr;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data3);
	// get old session data
	memcpy(&old_session_data, test_object.replication_area, 3*sizeof(sslid_replication_data));
	boost::thread test_thread7(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
	test_thread7.timed_join(boost::posix_time::milliseconds(100));
	// old data not changed check
	compare_result = memcmp(&old_session_data, test_object.replication_area, 3*sizeof(sslid_replication_data));
	BOOST_CHECK_EQUAL(compare_result, 0);

	// test_case[18] when replication_area_lock and replication_area_unlock function is NULL test
	test_data1.session_id = "test_id3";
	test_data1.op_code = 'A';
	test_value = 0;
	test_object.replication_area_lock = NULL;
	test_object.replication_area_unlock = NULL;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	try{
		boost::thread test_thread8(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
		test_thread8.timed_join(boost::posix_time::milliseconds(100));
	} catch(...) {
		BOOST_ERROR("exception: write_replicaion_area_test");
	}

	// test_case[19] when replication_area_lock function is not NULL test
	test_value = 0;
	function_pointer = &lock_function;
	test_object.replication_area_lock = function_pointer;
	test_object.replication_area_unlock = NULL;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	try{
		boost::thread test_thread9(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
		test_thread9.timed_join(boost::posix_time::milliseconds(100));
	} catch(...) {
		BOOST_ERROR("exception: write_replicaion_area_test");
	}
	// replication_area_lock function called check
	BOOST_CHECK_EQUAL(test_value, 1);

	// test_case[20] when replication_area_unlock function is not NULL test
	test_value = 0;
	function_pointer = &unlock_function;
	test_object.replication_area_lock = NULL;
	test_object.replication_area_unlock = unlock_function;
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	try{
		boost::thread test_thread10(boost::bind(&sslid_replication_data_processor::write_replicaion_area, &test_object));
		test_thread10.timed_join(boost::posix_time::milliseconds(100));
	} catch(...) {
		BOOST_ERROR("exception: write_replicaion_area_test");
	}
	// replication_area_unlock function called check
	BOOST_CHECK_EQUAL(test_value, 2);

}

// function register_replication_area_lock test
void register_replication_area_lock_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	l7vs::sslid_replication_data_processor test_object(1, replication_data_area, SECTION_NUMBER);
	boost::function<void(void)> register_function;

	// test_case[21] registered function is NULL test
	register_function = NULL;
	test_object.register_replication_area_lock(register_function);
	// function registered correctly check;
	BOOST_CHECK(test_object.replication_area_lock == NULL);

	// test_case[22] registered replication_area_lock function test
	test_value = 0;
	register_function = &lock_function;
	test_object.register_replication_area_lock(register_function);
	// function registered correctly check;
	test_object.replication_area_lock();
	BOOST_CHECK_EQUAL(test_value, 1);
}

// function register_replication_area_unlock test
void register_replication_area_unlock_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	l7vs::sslid_replication_data_processor test_object(1, replication_data_area, SECTION_NUMBER);
	boost::function<void(void)> register_function;

	// test_case[23] registered function is NULL test
	register_function = NULL;
	test_object.register_replication_area_unlock(register_function);
	// function registered correctly check;
	BOOST_CHECK(test_object.replication_area_lock == NULL);

	// test_case[24] registered replication_area_unlock function test
	test_value = 0;
	register_function = &unlock_function;
	test_object.register_replication_area_unlock(register_function);
	// function registered correctly check;
	test_object.replication_area_unlock();
	BOOST_CHECK_EQUAL(test_value, 2);
}

void get_from_temp_list_thread1(l7vs::sslid_replication_data_processor* test_object){

	l7vs::sslid_replication_temp_data test_data1;
	boost::asio::ip::tcp::endpoint endpoint;

	test_data1.session_id = "test_id1";
	test_data1.op_code = 'A';
	test_data1.realserver_addr = endpoint;
	test_data1.last_time = time(0);
	test_object->temp_list.push_back(test_data1);
}

void get_from_temp_list_thread2(
		l7vs::sslid_replication_data_processor* test_object,
		l7vs::sslid_replication_temp_data& get_data) {

	test_object->get_from_temp_list(get_data);
}

// function get_from_temp_list test
void get_from_temp_list_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	l7vs::sslid_replication_data_processor test_object(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_replication_temp_data test_data1;
	l7vs::sslid_replication_temp_data test_data2;
	l7vs::sslid_replication_temp_data get_data;
	boost::asio::ip::tcp::endpoint endpoint;

	// test_case[25] when temp_list size is empty, test
	try {
		test_object.temp_list.clear();
		boost::thread_group thread_group1;
		thread_group1.create_thread(boost::bind(&get_from_temp_list_thread1, &test_object));
		thread_group1.create_thread(boost::bind(&get_from_temp_list_thread2, &test_object, get_data));
		thread_group1.join_all();
	} catch(...) {
		BOOST_ERROR("exception: get_from_temp_list");
	}

	// test_case[26] first element of temp_list is removed test
	test_data1.session_id = "test_id1";
	test_data1.op_code = 'T';
	test_data1.realserver_addr = endpoint;
	test_data1.last_time = time(0);
	test_data2.session_id = "test_id2";
	test_data2.op_code = 'A';
	test_data2.realserver_addr = endpoint;
	test_data2.last_time = time(0);
	test_object.temp_list.push_back(test_data1);
	test_object.temp_list.push_back(test_data2);
	memset(&get_data, 0, sizeof(l7vs::sslid_replication_temp_data));
	test_object.get_from_temp_list(get_data);
	// first item removed check,so only one item leave, check it
	BOOST_CHECK_EQUAL((int)test_object.temp_list.size(), 1);
	// leaved item check(the leaved item should be test_data2)
	// session_id check
	BOOST_CHECK_EQUAL(get_data.session_id, test_data2.session_id);
	// op_code check
	BOOST_CHECK_EQUAL(get_data.op_code, test_data2.op_code);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_data.realserver_addr, test_data2.realserver_addr);
	// last_time check
	BOOST_CHECK_EQUAL(get_data.last_time, test_data2.last_time);

	// test_case[27] get data test(list has one item)
	test_data1.session_id = "test_id1";
	test_data1.op_code = 'A';
	test_data1.realserver_addr = endpoint;
	test_data1.last_time = time(0);
	memset(&get_data, 0, sizeof(l7vs::sslid_replication_temp_data));
	test_object.temp_list.clear();
	test_object.put_into_temp_list(test_data1);
	test_object.get_from_temp_list(get_data);
	// got data check
	// session_id check
	BOOST_CHECK_EQUAL(get_data.session_id, test_data1.session_id);
	// op_code check
	BOOST_CHECK_EQUAL(get_data.op_code, test_data1.op_code);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_data.realserver_addr, test_data1.realserver_addr);
	// last_time check
	BOOST_CHECK_EQUAL(get_data.last_time, test_data1.last_time);

	// test_case[28] get data test(list has two items, get the item which first put into)
	test_data1.session_id = "test_id1";
	test_data1.op_code = 'U';
	test_data1.realserver_addr = endpoint;
	test_data1.last_time = time(0);
	test_data2.session_id = "test_id2";
	test_data2.op_code = 'D';
	test_data2.realserver_addr = endpoint;
	test_data2.last_time = time(0);
	test_object.put_into_temp_list(test_data2);
	test_object.put_into_temp_list(test_data1);
	memset(&get_data, 0, sizeof(l7vs::sslid_replication_temp_data));
	test_object.get_from_temp_list(get_data);
	// got data check
	// session_id check
	BOOST_CHECK_EQUAL(get_data.session_id, test_data2.session_id);
	// op_code check
	BOOST_CHECK_EQUAL(get_data.op_code, test_data2.op_code);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_data.realserver_addr, test_data2.realserver_addr);
	// last_time check
	BOOST_CHECK_EQUAL(get_data.last_time, test_data2.last_time);
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

	framework::master_test_suite().add( ts );
}
