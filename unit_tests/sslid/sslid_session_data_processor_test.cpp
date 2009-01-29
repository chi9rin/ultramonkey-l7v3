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

// function sslid_session_data_processor test
void sslid_session_data_processor_test(){

	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	l7vs::sslid_replication_data_processor replication_data_processor(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_session_data_processor test_object(2, 5, &replication_data_processor);

	// test_case[1] maxlist check
	BOOST_CHECK_EQUAL(test_object.maxlist, 2);
	// timeout check
	BOOST_CHECK_EQUAL(test_object.timeout, 5);
	// replication_data_processor check
	BOOST_CHECK_EQUAL(test_object.replication_data_processor, &replication_data_processor);
}

// function get_endpoint_from_session_data test
void get_endpoint_from_session_data_test(){

	int result = 10;
	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	time_t lasttime = time(0);
	std::string session_id;
	boost::asio::ip::address endpoint_address;
	boost::asio::ip::tcp::endpoint saved_endpoint;
	boost::asio::ip::tcp::endpoint get_endpoint;
	l7vs::sslid_replication_data_processor replication_data_processor(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_session_data_processor test_object(2, 0, &replication_data_processor);

	// test_case[2] session_endpoint_map and session_lasttime_map are all empty test
	session_id = "test_id1";
	result = test_object.get_endpoint_from_session_data(session_id, get_endpoint);
	// function result check
	BOOST_CHECK_EQUAL(result, -1);

	// test_case[3] when session_endpoint_map is empty, but session_lasttime_map is not empty test
	result = 10;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map[session_id] = lasttime;
	result = test_object.get_endpoint_from_session_data(session_id, get_endpoint);
	// function result check
	BOOST_CHECK_EQUAL(result, -1);

	// test_case[4] when session_endpoint_map is not empty, but session_lasttime_map is empty test
	result = 10;
	test_object.session_lasttime_map.clear();
	test_object.session_endpoint_map[session_id] = saved_endpoint;
	result = test_object.get_endpoint_from_session_data(session_id, get_endpoint);
	// function result check
	BOOST_CHECK_EQUAL(result, -1);

	// test_case[5] not time out test
	result = 10;
	endpoint_address = boost::asio::ip::address::from_string("192.168.120.102");
	saved_endpoint.address(endpoint_address);
	saved_endpoint.port(80);
	get_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
	test_object.session_endpoint_map[session_id] = saved_endpoint;
	test_object.session_lasttime_map[session_id] = lasttime;
	test_object.timeout = 10000;
	result = test_object.get_endpoint_from_session_data(session_id, get_endpoint);
	// got endpoint check
	BOOST_CHECK_EQUAL(get_endpoint, saved_endpoint);
	// function result check
	BOOST_CHECK_EQUAL(result, 0);

	// test_case[6] time out test
	std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator  it1;
	std::map<std::string, time_t>::iterator it2;
	std::multimap<time_t, std::string>::iterator it3;
	boost::asio::ip::tcp::endpoint temp_endpoint;
	result = 10;
	get_endpoint = temp_endpoint;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map[session_id] = saved_endpoint;
	test_object.session_lasttime_map[session_id] = lasttime;
	test_object.lasttime_session_map.insert(std::make_pair(lasttime, session_id));
	test_object.lasttime_session_map.insert(std::make_pair(lasttime, std::string("test_id2")));
	test_object.timeout = 0;
	result = test_object.get_endpoint_from_session_data(session_id, get_endpoint);
	it1 = test_object.session_endpoint_map.find(session_id);
	it2 = test_object.session_lasttime_map.find(session_id);
	it3 = test_object.lasttime_session_map.find(lasttime);
	sslid_replication_temp_data get_temp_data;
	test_object.replication_data_processor->get_from_temp_list(get_temp_data);
	// session_endpoint_map item removed check
	BOOST_CHECK(it1 == test_object.session_endpoint_map.end());
	// session_lasttime_map item removed check
	BOOST_CHECK(it2 == test_object.session_lasttime_map.end());
	// lasttime_session_map item removed check
	BOOST_CHECK_EQUAL(it3->second, std::string("test_id2"));
	// function result check
	BOOST_CHECK_EQUAL(result, -1);
	// get_endpoint not be changed check
	BOOST_CHECK_EQUAL(get_endpoint, temp_endpoint);
	// item which put into the temp_list check
	char char_d = 'D';
	BOOST_CHECK_EQUAL(get_temp_data.op_code, char_d);
	BOOST_CHECK_EQUAL(get_temp_data.session_id, session_id);
}

// function write_session_data test
void write_session_data_test(){

	int result = 10;
	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	time_t write_time = time(0);
	std::string session_id;
	boost::asio::ip::tcp::endpoint old_saved_endpoint;
	boost::asio::ip::tcp::endpoint new_saved_endpoint;
	boost::asio::ip::tcp::endpoint get_endpoint;
	l7vs::sslid_replication_data_processor replication_data_processor(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_session_data_processor test_object(2, 0, &replication_data_processor);

	// test_case[7] session_id exist test
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	old_saved_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
	new_saved_endpoint.address(boost::asio::ip::address::from_string("192.168.120.102"));
	session_id = "test_id1";
	test_object.session_endpoint_map[session_id] = old_saved_endpoint;
	result = test_object.write_session_data(session_id, new_saved_endpoint, write_time);
	std::multimap<time_t, std::string>::iterator lasttime_session_map_iterator =
				test_object.lasttime_session_map.find(write_time);
	l7vs::sslid_replication_temp_data& get_temp_data = test_object.replication_data_processor->temp_list.front();
	// session_endpoint_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_endpoint_map[session_id], new_saved_endpoint);
	// session_lasttime_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_lasttime_map[session_id], write_time);
	// lasttime_session_map is setted check
	BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// item which put into temp_list check
	// op_code check
	BOOST_CHECK_EQUAL(get_temp_data.op_code, 'U');
	// session_id check
	BOOST_CHECK_EQUAL(get_temp_data.session_id, session_id);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_temp_data.realserver_addr, new_saved_endpoint);
	// last_time check
	BOOST_CHECK_EQUAL(get_temp_data.last_time, write_time);

	// test_case[8] session_id not exist test(session_endpoint_map size = maxlist - 1)
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id2"), new_saved_endpoint));
	result = test_object.write_session_data(session_id, new_saved_endpoint, write_time);
	lasttime_session_map_iterator =test_object.lasttime_session_map.find(write_time);
	get_temp_data = test_object.replication_data_processor->temp_list.front();
	// session_endpoint_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_endpoint_map[session_id], new_saved_endpoint);
	// session_lasttime_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_lasttime_map[session_id], write_time);
	// lasttime_session_map is setted check
	BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// item which put into temp_list check
	// op_code check
	BOOST_CHECK_EQUAL(get_temp_data.op_code, 'A');
	// session_id check
	BOOST_CHECK_EQUAL(get_temp_data.session_id, session_id);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_temp_data.realserver_addr, new_saved_endpoint);
	// last_time check
	BOOST_CHECK_EQUAL(get_temp_data.last_time, write_time);

	// test_case[9] session_id not exist test(session_endpoint_map size = maxlist)
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id2"), new_saved_endpoint));
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id3"), new_saved_endpoint));
	result = test_object.write_session_data(session_id, new_saved_endpoint, write_time);
	l7vs::sslid_replication_temp_data& first_item = test_object.replication_data_processor->temp_list.front();
	l7vs::sslid_replication_temp_data& last_item = test_object.replication_data_processor->temp_list.back();
	// clear_expired_session_data function is called check(the first item in the temp_list is put by clear_expired_session_data function)
	BOOST_CHECK_EQUAL(first_item.op_code, 'D');
	BOOST_CHECK_EQUAL(first_item.session_id, session_id);
	// session_endpoint_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_endpoint_map[session_id], new_saved_endpoint);
	// session_lasttime_map is setted check
	BOOST_CHECK_EQUAL(test_object.session_lasttime_map[session_id], write_time);
	// lasttime_session_map is setted check
	BOOST_CHECK_EQUAL(lasttime_session_map_iterator->second, session_id);
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// the add operation check(the second item in the temp_list is to be added item)
	// op_code check
	BOOST_CHECK_EQUAL(last_item.op_code, 'A');
	// session_id check
	BOOST_CHECK_EQUAL(get_temp_data.session_id, session_id);
	// realserver_addr check
	BOOST_CHECK_EQUAL(get_temp_data.realserver_addr, new_saved_endpoint);
	// last_time check
	BOOST_CHECK_EQUAL(get_temp_data.last_time, write_time);
}

// function clear_expired_session_data test
void clear_expired_session_data_test(){

	int result = 10;
	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	time_t test_time;
	boost::asio::ip::tcp::endpoint saved_endpoint1;
	boost::asio::ip::tcp::endpoint saved_endpoint2;
	l7vs::sslid_replication_data_processor replication_data_processor(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_session_data_processor test_object(2, 0, &replication_data_processor);

	// test_case[10] map empty test
	test_object.timeout = 0;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	try{
		result = test_object.clear_expired_session_data();
	} catch(...) {
		BOOST_ERROR("exception: clear_expired_session_data");
	}
	// function result check
	BOOST_CHECK_EQUAL(result, 0);

	// test_case[11] when time out items exist(all items is time out), items remove test
	test_time = time(0);
	test_time = test_time - 5;
	test_object.timeout = 0;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id1"), saved_endpoint1));
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id2"), saved_endpoint2));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id1"), test_time));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id2"), test_time));
	test_object.lasttime_session_map.insert(std::make_pair(test_time, std::string("test_id1")));
	test_object.lasttime_session_map.insert(std::make_pair(test_time, std::string("test_id2")));
	result = test_object.clear_expired_session_data();
	l7vs::sslid_replication_temp_data& first_data = test_object.replication_data_processor->temp_list.front();
	l7vs::sslid_replication_temp_data& last_data = test_object.replication_data_processor->temp_list.back();
	// remove result check(all items is removed)
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// session_endpoint_map items remove check
	BOOST_CHECK_EQUAL((int)test_object.session_endpoint_map.size(), 0);
	// session_lasttime_map items remove check
	BOOST_CHECK_EQUAL((int)test_object.session_lasttime_map.size(), 0);
	// lasttime_session_map items remove check
	BOOST_CHECK_EQUAL((int)test_object.lasttime_session_map.size(), 0);
	// removed items check
	BOOST_CHECK_EQUAL(first_data.op_code, 'D');
	BOOST_CHECK_EQUAL(first_data.session_id, std::string("test_id1"));
	BOOST_CHECK_EQUAL(last_data.op_code, 'D');
	BOOST_CHECK_EQUAL(last_data.session_id, std::string("test_id2"));

	// test_case[12] when time out item exists(one item is time out, but another item is not), item remove test
	saved_endpoint2.port(88);
	test_object.timeout = 100;
	test_time = time(0);
	time_t out_time = test_time - 1000;
	time_t not_out_time = test_time;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id1"), saved_endpoint1));
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id2"), saved_endpoint2));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id1"), out_time));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id2"), not_out_time));
	test_object.lasttime_session_map.insert(std::make_pair(out_time, std::string("test_id1")));
	test_object.lasttime_session_map.insert(std::make_pair(not_out_time, std::string("test_id2")));
	result = test_object.clear_expired_session_data();
	std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator it1 = test_object.session_endpoint_map.begin();
	std::map<std::string, time_t>::iterator it2 = test_object.session_lasttime_map.begin();
	std::multimap<time_t, std::string>::iterator it3 = test_object.lasttime_session_map.begin();
	l7vs::sslid_replication_temp_data& temp_data = test_object.replication_data_processor->temp_list.front();
	// remove ruslt check(item 'test_id1' is removed, but item 'test_id2' is still existing)
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// session_endpoint_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.session_endpoint_map.size(), 1);
	BOOST_CHECK_EQUAL(it1->first, std::string("test_id2"));
	BOOST_CHECK_EQUAL(it1->second, saved_endpoint2);
	// session_lasttime_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.session_lasttime_map.size(), 1);
	BOOST_CHECK_EQUAL(it2->first, std::string("test_id2"));
	BOOST_CHECK_EQUAL(it2->second, not_out_time);
	// lasttime_session_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.lasttime_session_map.size(), 1);
	BOOST_CHECK_EQUAL(it3->first, not_out_time);
	BOOST_CHECK_EQUAL(it3->second, std::string("test_id2"));
	// removed item check
	BOOST_CHECK_EQUAL(temp_data.op_code, 'D');
	BOOST_CHECK_EQUAL(temp_data.session_id, std::string("test_id1"));

	// test_case[13] when time out items are not exist, item remove test
	test_object.timeout = 10000;
	time_t earlier_time = test_time -5;
	time_t earliest_time = test_time -10;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	test_object.replication_data_processor->temp_list.clear();
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id1"), saved_endpoint1));
	test_object.session_endpoint_map.insert(std::make_pair(std::string("test_id2"), saved_endpoint2));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id1"), earlier_time));
	test_object.session_lasttime_map.insert(std::make_pair(std::string("test_id2"), earliest_time));
	test_object.lasttime_session_map.insert(std::make_pair(earlier_time, std::string("test_id1")));
	test_object.lasttime_session_map.insert(std::make_pair(earliest_time, std::string("test_id2")));
	result = test_object.clear_expired_session_data();
	it1 = test_object.session_endpoint_map.begin();
	it2 = test_object.session_lasttime_map.begin();
	it3 = test_object.lasttime_session_map.begin();
	temp_data = test_object.replication_data_processor->temp_list.front();
	// remove ruslt check(item 'test_id2' which has the earliest lasttime is removed)
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// session_endpoint_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.session_endpoint_map.size(), 1);
	BOOST_CHECK_EQUAL(it1->first, std::string("test_id1"));
	BOOST_CHECK_EQUAL(it1->second, saved_endpoint1);
	// session_lasttime_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.session_lasttime_map.size(), 1);
	BOOST_CHECK_EQUAL(it2->first, std::string("test_id1"));
	BOOST_CHECK_EQUAL(it2->second, earlier_time);
	// lasttime_session_map item remove check
	BOOST_CHECK_EQUAL((int)test_object.lasttime_session_map.size(), 1);
	BOOST_CHECK_EQUAL(it3->first, earlier_time);
	BOOST_CHECK_EQUAL(it3->second, std::string("test_id1"));
	// removed item check
	BOOST_CHECK_EQUAL(temp_data.op_code, 'D');
	BOOST_CHECK_EQUAL(temp_data.session_id, std::string("test_id2"));
}

// function read_session_data_from_replication_area test
void read_session_data_from_replication_area_test(){

	int result = 10;
	unsigned short port1 = 888;
	unsigned short port2 = 999;
	char replication_data_area[128*sizeof(struct l7vs::sslid_replication_data_header) + STRUCT_NUMBER*sizeof(struct l7vs::sslid_replication_data)] = { 0 };
	char *header_start_address = replication_data_area;
	char *real_data_start_address = header_start_address + 128*sizeof(struct l7vs::sslid_replication_data_header);
	time_t last_time1 = time(0);
	l7vs::sslid_replication_data_header* first_header = (sslid_replication_data_header*)header_start_address;
	l7vs::sslid_replication_data* first_real_data = (sslid_replication_data*)real_data_start_address;
	l7vs::sslid_replication_data_processor replication_data_processor(2, replication_data_area, SECTION_NUMBER);
	l7vs::sslid_session_data_processor test_object(2, 0, &replication_data_processor);

	// setting for the replication area
	// set the header area
	first_header->virtualserver_addr.s_addr = inet_addr("192.168.120.102");
	first_header->virtualserver_port = 80;
	first_header->offset = real_data_start_address - header_start_address;
	first_header->size = 2*sizeof(struct l7vs::sslid_replication_data);
	// set the real data area(the first data)
	char session_id1[] = "test_id1";
	memcpy(first_real_data->session_id, session_id1, sizeof(session_id1));
	first_real_data->valid = 1;
	first_real_data->realserver_addr.sin_family = AF_INET;
	first_real_data->realserver_addr.sin_addr.s_addr = inet_addr("192.168.120.242");
	first_real_data->realserver_addr.sin_port = port1;
	first_real_data->last_time = last_time1;
	// set the real data area(the second data)
	char session_id2[] = "test_id2";
	time_t last_time2 = last_time1 - 10;
	memcpy((first_real_data + 1)->session_id, session_id2, sizeof(session_id2));
	(first_real_data + 1)->valid = 1;
	(first_real_data + 1)->realserver_addr.sin_family = AF_INET;
	(first_real_data + 1)->realserver_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	(first_real_data + 1)->realserver_addr.sin_port = port2;
	(first_real_data + 1)->last_time = last_time2;

	// test_case[14] NULL pointer test
	try {
		result = test_object.read_session_data_from_replication_area(NULL);
	} catch (...){
		BOOST_ERROR("exception: read_session_data_from_replication_area");
	}
	// function result check
	BOOST_CHECK_EQUAL(result, -1);

	std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator it1;
	std::map<std::string, time_t>::iterator it2;
	std::multimap<time_t, std::string>::iterator it3;

	// test_case[15] read data test(read the first data)
	result = 10;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	result = test_object.read_session_data_from_replication_area(first_real_data);
	it1 = test_object.session_endpoint_map.begin();
	it2 = test_object.session_lasttime_map.begin();
	it3 = test_object.lasttime_session_map.begin();
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// session_endpoint_map check
	BOOST_CHECK_EQUAL((int)test_object.session_endpoint_map.size(), 1);
	// session_endpoint_map's session_id check
	BOOST_CHECK_EQUAL(it1->first, std::string("test_id1"));
	// session_endpoint_map's endpoint (ip && port) check
	BOOST_CHECK_EQUAL(it1->second.address().to_string(), std::string("192.168.120.242"));
	BOOST_CHECK_EQUAL(it1->second.port(), port1);
	// session_lasttime_map check
	BOOST_CHECK_EQUAL((int)test_object.session_lasttime_map.size(), 1);
	// session_lasttime_map's session_id check
	BOOST_CHECK_EQUAL(it2->first, std::string("test_id1"));
	// session_lasttime_map's lasttime check
	BOOST_CHECK_EQUAL(it2->second, last_time1);
	// lasttime_session_map check
	BOOST_CHECK_EQUAL((int)test_object.lasttime_session_map.size(), 1);
	// lasttime_session_map's lasttime check
	BOOST_CHECK_EQUAL(it3->first, last_time1);
	// lasttime_session_map's session_id check
	BOOST_CHECK_EQUAL(it3->second, std::string("test_id1"));

	// test_case[16] read data test(read the second data)
	result = 10;
	test_object.session_endpoint_map.clear();
	test_object.session_lasttime_map.clear();
	test_object.lasttime_session_map.clear();
	result = test_object.read_session_data_from_replication_area(first_real_data + 1);
	it1 = test_object.session_endpoint_map.begin();
	it2 = test_object.session_lasttime_map.begin();
	it3 = test_object.lasttime_session_map.begin();
	// function result check
	BOOST_CHECK_EQUAL(result, 0);
	// session_endpoint_map check
	BOOST_CHECK_EQUAL((int)test_object.session_endpoint_map.size(), 1);
	// session_endpoint_map's session_id check
	BOOST_CHECK_EQUAL(it1->first, std::string("test_id2"));
	// session_endpoint_map's endpoint (ip && port) check
	BOOST_CHECK_EQUAL(it1->second.address().to_string(), std::string("255.255.255.255"));
	BOOST_CHECK_EQUAL(it1->second.port(), port2);
	// session_lasttime_map check
	BOOST_CHECK_EQUAL((int)test_object.session_lasttime_map.size(), 1);
	// session_lasttime_map's session_id check
	BOOST_CHECK_EQUAL(it2->first, std::string("test_id2"));
	// session_lasttime_map's lasttime check
	BOOST_CHECK_EQUAL(it2->second, last_time2);
	// lasttime_session_map check
	BOOST_CHECK_EQUAL((int)test_object.lasttime_session_map.size(), 1);
	// lasttime_session_map's lasttime check
	BOOST_CHECK_EQUAL(it3->first, last_time2);
	// lasttime_session_map's session_id check
	BOOST_CHECK_EQUAL(it3->second, std::string("test_id2"));
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
