#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/ref.hpp>
#include "snmpbridge.h"
#include "snmpbridge_test.h"

using namespace boost::unit_test;
using namespace l7vs;

class snmpbridge_testclass : public l7vs::snmpbridge{
public:
	l7ag_parameter&						get_snmp_param(){ return snmp_param; }
	char*								get_send_buffer(){ return send_buffer; }
	size_t&								get_send_buffer_size(){ return send_buffer_size; }
	boost::mutex&						get_send_buffer_mutex(){ return send_buffer_mutex; }
	boost::array<char, READBUF_SIZE>&	get_recv_buffer(){ return recv_buffer; }
	boost::asio::io_service&			get_snmp_io_service(){ return snmp_io_service; }
	boost::asio::ip::tcp::acceptor&		get_snmp_acceptor(){ return snmp_acceptor; }
	boost::asio::ip::tcp::socket&		get_snmp_socket(){ return snmp_socket; }
	bool&								get_connection_state(){ return connection_state; }
	snmp_logcategory_map_type&			get_snmp_loglevel_map(){ return snmp_loglevel_map; }
	string_loglevel_map_type&			get_levelstring_map(){ return levelstring_map; }
	l7vsd&								get_vsd(){ return vsd; }


	snmpbridge_testclass( l7vsd& l7vsd_in, boost::asio::io_service& io_service_in ) : snmpbridge(l7vsd_in, io_service_in){}

	void								set_snmp_param(l7ag_parameter& snmp_param_in){
											snmp_param = snmp_param_in; }
	void								set_connection_state(bool in){ connection_state = in; }

	int									send_message2(){ return send_message(); }
	int									load_config2(){ return load_config(); }
	void								load_loglevel2(){ load_loglevel(); }

	boost::mutex			send_mutex;
	boost::condition		send_condition;

	int send_trap2( int id, const std::string& message ){
		std::stringstream msg1, msg2;
		msg1 << "send_trap start thread:" << id;
		msg2 << "send_trap end thread:" << id;

		{
		boost::mutex::scoped_lock lock( send_mutex );
//		BOOST_MESSAGE(msg1.str());
		std::cout << msg1.str() << std::endl;
		send_condition.wait( lock );
		}
		int return_value = 0;

		return_value = send_trap(message);
//		BOOST_MESSAGE(msg2.str());
		std::cout << msg2.str() << std::endl;
		return return_value;
	}

	void reload_config2(int id){
		std::stringstream msg1, msg2;
		msg1 << "reload_config start thread:" << id;
		msg2 << "reload_config end thread:" << id;

		{
		boost::mutex::scoped_lock lock( send_mutex );
//		BOOST_MESSAGE(msg1.str());
		std::cout << msg1.str() << std::endl;
		send_condition.wait( lock );
		}

		reload_config();
//		BOOST_MESSAGE(msg2.str());
		std::cout << msg2.str() << std::endl;
	}

	int change_loglevel2( int id, const LOG_CATEGORY_TAG snmp_log_category, const LOG_LEVEL_TAG loglevel ){
		std::stringstream msg1, msg2;
		msg1 << "change_loglevel start thread:" << id;
		msg2 << "change_loglevel end thread:" << id;

		{
		boost::mutex::scoped_lock lock( send_mutex );
//		BOOST_MESSAGE(msg1.str());
		std::cout << msg1.str() << std::endl;
		send_condition.wait( lock );
		}
		int return_value = 0;

		return_value = change_loglevel( snmp_log_category, loglevel );
//		BOOST_MESSAGE(msg2.str());
		std::cout << msg2.str() << std::endl;
		return return_value;
	}
	int change_loglevel_allcategory2( int id, const LOG_LEVEL_TAG loglevel ){
		std::stringstream msg1, msg2;
		msg1 << "change_loglevel_allcategory start thread:" << id;
		msg2 << "change_loglevel_allcategory end thread:" << id;

		{
		boost::mutex::scoped_lock lock( send_mutex );
//		BOOST_MESSAGE(msg1.str());
		std::cout << msg1.str() << std::endl;
		send_condition.wait( lock );
		}
		int return_value = 0;

		return_value = change_loglevel_allcategory( loglevel );
//		BOOST_MESSAGE(msg2.str());
		std::cout << msg2.str() << std::endl;
		return return_value;
	}
	int send_mibcollection2( int id, struct l7ag_mibrequest_message* payload ){
		std::stringstream msg1, msg2;
		msg1 << "send_mibcollection start thread:" << id;
		msg2 << "send_mibcollection end thread:" << id;

		{
		boost::mutex::scoped_lock lock( send_mutex );
//		BOOST_MESSAGE(msg1.str());
		std::cout << msg1.str() << std::endl;
		send_condition.wait( lock );
		}
		int return_value = 0;

		return_value = send_mibcollection( payload );
//		BOOST_MESSAGE(msg2.str());
		std::cout << msg2.str() << std::endl;
		return return_value;
	}
};

l7vsd vsd;
boost::asio::io_service io_service;
boost::asio::io_service io_service1;
snmpbridge_testclass test(vsd, io_service);
snmpbridge_testclass test1(vsd, io_service1);
int count = 0;
bool exit_flag = false;
boost::array<char, READBUF_SIZE> recvbuffer;

void	sendtrap( int id, const std::string& msg ){
	int ret = 0;
	ret = test.send_trap2( id, msg );
//	BOOST_CHECK_EQUAL( ret, 0 );
}

void	reloadconfig( int id ){
	test.reload_config2( id );
}

void	changeloglevel( int id, const LOG_CATEGORY_TAG snmp_log_category, const LOG_LEVEL_TAG loglevel ){
	int ret = 0;
	ret = test.change_loglevel2( id, snmp_log_category, loglevel );
//	BOOST_CHECK_EQUAL( ret, 0 );
}

void	changeloglevelallcategory( int id, const LOG_LEVEL_TAG loglevel ){
	int ret = 0;
	ret = test.change_loglevel_allcategory2( id, loglevel );
//	BOOST_CHECK_EQUAL( ret, 0 );
}

void	sendmibcollection( int id, struct l7ag_mibrequest_message* payload ){
	int ret = 0;
	ret = test.send_mibcollection2( id, payload );
//	BOOST_CHECK_EQUAL( ret, 0 );
}

void	snmpbridge_test_thread1(){
	int ret = 0;
	ret = test.initialize();

	// unit_test[1] 複数スレッドからsend_trapを呼び出してもエラーが発生しないことを確認
#if 1
	boost::thread	thd1( boost::bind( &sendtrap, 1, "message1" ) );
	boost::thread	thd2( boost::bind( &sendtrap, 2, "message2" ) );
	boost::thread	thd3( boost::bind( &sendtrap, 3, "message3" ) );
	boost::thread	thd4( boost::bind( &sendtrap, 4, "message4" ) );
	boost::thread	thd5( boost::bind( &sendtrap, 5, "message5" ) );
#endif
	sleep(2);
	BOOST_MESSAGE( "notify_all" );
	test.send_condition.notify_all();
	thd1.join();
	thd2.join();
	thd3.join();
	thd4.join();
	thd5.join();
	// unit_test[2] send_trapとreload_configを同時に呼び出してもエラーが発生しないことを確認
#if 0
	boost::thread	thd6( boost::bind( &reloadconfig, 6 ) );
	boost::thread	thd7( boost::bind( &reloadconfig, 7 ) );
	boost::thread	thd8( boost::bind( &reloadconfig, 8 ) );
	boost::thread	thd9( boost::bind( &reloadconfig, 9 ) );
	boost::thread	thd10( boost::bind( &reloadconfig, 10 ) );
#else
	boost::thread	thd6( boost::bind( &sendtrap, 6, "message6" ) );
	boost::thread	thd7( boost::bind( &sendtrap, 7, "message7" ) );
	boost::thread	thd8( boost::bind( &sendtrap, 8, "message8" ) );
	boost::thread	thd9( boost::bind( &sendtrap, 9, "message9" ) );
	boost::thread	thd10( boost::bind( &reloadconfig, 10 ) );
#endif
	sleep(2);
	BOOST_MESSAGE( "notify_all" );
	test.send_condition.notify_all();

	thd6.join();
	thd7.join();
	thd8.join();
	thd9.join();
	thd10.join();

	// unit_test[3] send_trapとchange_loglevelを同時に呼び出してもエラーが発生しないことを確認
#if 0
	boost::thread	thd11( boost::bind( &changeloglevel, 11, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
	boost::thread	thd12( boost::bind( &changeloglevel, 12, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
	boost::thread	thd13( boost::bind( &changeloglevel, 13, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
	boost::thread	thd14( boost::bind( &changeloglevel, 14, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
	boost::thread	thd15( boost::bind( &changeloglevel, 15, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
#else
	boost::thread	thd11( boost::bind( &sendtrap, 11, "message11" ) );
	boost::thread	thd12( boost::bind( &sendtrap, 12, "message12" ) );
	boost::thread	thd13( boost::bind( &sendtrap, 13, "message13" ) );
	boost::thread	thd14( boost::bind( &sendtrap, 14, "message14" ) );
	boost::thread	thd15( boost::bind( &changeloglevel, 15, LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG ) );
#endif
	sleep(2);
	BOOST_MESSAGE( "notify_all" );
	test.send_condition.notify_all();

	thd11.join();
	thd12.join();
	thd13.join();
	thd14.join();
	thd15.join();

	// unit_test[4] send_trapとchange_loglevel_allcategoryを同時に呼び出してもエラーが発生しないことを確認
#if 0
	boost::thread	thd16( boost::bind( &changeloglevelallcategory, 16, LOG_LV_DEBUG ) );
	boost::thread	thd17( boost::bind( &changeloglevelallcategory, 17, LOG_LV_DEBUG ) );
	boost::thread	thd18( boost::bind( &changeloglevelallcategory, 18, LOG_LV_DEBUG ) );
	boost::thread	thd19( boost::bind( &changeloglevelallcategory, 19, LOG_LV_DEBUG ) );
	boost::thread	thd20( boost::bind( &changeloglevelallcategory, 20, LOG_LV_DEBUG ) );
#else
	boost::thread	thd16( boost::bind( &sendtrap, 16, "message16" ) );
	boost::thread	thd17( boost::bind( &sendtrap, 17, "message17" ) );
	boost::thread	thd18( boost::bind( &sendtrap, 18, "message18" ) );
	boost::thread	thd19( boost::bind( &sendtrap, 19, "message19" ) );
	boost::thread	thd20( boost::bind( &changeloglevelallcategory, 20, LOG_LV_DEBUG ) );
#endif
	sleep(2);
	BOOST_MESSAGE( "notify_all" );
	test.send_condition.notify_all();

	thd16.join();
	thd17.join();
	thd18.join();
	thd19.join();
	thd20.join();

	// unit_test[5] send_trapとsend_mibcollectionを同時に呼び出してもエラーが発生しないことを確認
	struct l7ag_mibrequest_message payload;
#if 0
	boost::thread	thd21( boost::bind( &sendmibcollection, 21, &payload ) );
	boost::thread	thd22( boost::bind( &sendmibcollection, 22, &payload ) );
	boost::thread	thd23( boost::bind( &sendmibcollection, 23, &payload ) );
	boost::thread	thd24( boost::bind( &sendmibcollection, 24, &payload ) );
	boost::thread	thd25( boost::bind( &sendmibcollection, 25, &payload ) );
#else
	boost::thread	thd21( boost::bind( &sendtrap, 21, "message11" ) );
	boost::thread	thd22( boost::bind( &sendtrap, 22, "message12" ) );
	boost::thread	thd23( boost::bind( &sendtrap, 23, "message13" ) );
	boost::thread	thd24( boost::bind( &sendtrap, 24, "message14" ) );
	boost::thread	thd25( boost::bind( &sendmibcollection, 25, &payload ) );
#endif
	sleep(2);
	BOOST_MESSAGE( "notify_all" );
	test.send_condition.notify_all();

	thd21.join();
	thd22.join();
	thd23.join();
	thd24.join();
	thd25.join();

	test.finalize();
}

void	serverthread(){
	int ret = 0;
	ret = test1.initialize();
#if 1
	//vsをひとつだけ追加しておく
	l7vs::replication rep( io_service1 );
	l7vs::error_code err;
	l7vs::virtualservice_element element;
	boost::asio::ip::tcp::endpoint vs_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 10001);
	boost::asio::ip::tcp::endpoint sorry_endpoint(boost::asio::ip::address::from_string("127.0.0.2"), 10002);
	element.udpmode = false;
	element.tcp_accept_endpoint	= vs_endpoint;
	element.protocol_module_name = "protocol1";
	element.schedule_module_name = "schedule1";
	element.sorry_maxconnection = 1000;
	element.sorry_endpoint = sorry_endpoint;
	element.sorry_flag = 0;
	element.qos_upstream = 100;
	element.qos_downstream = 100;
	vsd.add_virtual_service(&element, err);
#endif

	// main loop
	for(;;){
		if(count >= 5 ){
			break;
		}
		io_service1.poll();
	}
	exit_flag = true;
	test1.finalize();
//	std::cout << "server end" << std::endl;
}


void	handle_write(boost::asio::ip::tcp::socket& s);
void	handle_read(boost::asio::ip::tcp::socket& s){
//	std::cout << "client handle_read size:" << recvbuffer.size() << std::endl;
//	std::cout << recvbuffer.data() << std::endl;//debug
	// unit_test[6] 通信を行い、snmpbridgeが送信したデータのチェック（送信データはsend_mibcollectionの場合）
	//check recv data
	struct l7ag_message_header* header = (struct l7ag_message_header*) recvbuffer.data();
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)2 );//vs 1 + rs 1(0)
	unsigned long long send_buffer_size = 0;
	send_buffer_size = sizeof( struct l7ag_message_header ) +
				 ( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
				 ( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;// vs 1 + rs 1(0)
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );
#if 1
//VS 1
	void* pos = (void *) (header + 1);
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	struct l7ag_mibdata_payload_vs* vs = (struct l7ag_mibdata_payload_vs*) pos;
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( vs->index, 1 );//
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
//	BOOST_CHECK_EQUAL( vs->ipAddress,  );
	BOOST_CHECK_EQUAL( vs->portNumber, 10001 );
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
//	BOOST_CHECK_EQUAL( vs->sorryIpAddress, );
	BOOST_CHECK_EQUAL( vs->sorryPortNumber, 10002 );
	BOOST_CHECK_EQUAL( vs->sorryThreshold, 1000 );
	BOOST_CHECK_EQUAL( vs->sorryForceFlag, 0 );
	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, (unsigned long long)100 );
	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, (unsigned long long)100 );
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->vs_count, 1);
	BOOST_CHECK_EQUAL( vs->scheduleModule, "schedule1" );
	BOOST_CHECK_EQUAL( vs->protocolModule, "protocol1" );

	pos = vs + 1;
//RS 0
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;
	struct l7ag_mibdata_payload_rs* rs = (struct l7ag_mibdata_payload_rs*) pos;
	BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
	BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( rs->index, 0 );
	BOOST_CHECK_EQUAL( rs->rs_count, 0);
#endif
	//create send data
    long long buf_size = sizeof(struct l7ag_message_header)
                                + sizeof(struct l7ag_payload_header)
                                + sizeof(struct l7ag_mibrequest_message);
	char sendbuffer[buf_size];
	memset( sendbuffer, 0, buf_size );
	struct l7ag_message_header* message_header;
	message_header = (struct l7ag_message_header*)sendbuffer;
	message_header->magic[0] = 0x4d;
	message_header->magic[1] = 0x47;
	message_header->version = 1;
	time_t tim;
	time( &tim );
	message_header->time = ( unsigned long long ) tim;
	message_header->payload_count = 1;
	message_header->size = buf_size;

    struct l7ag_payload_header* payload  = (struct l7ag_payload_header*) (message_header + 1);
    payload->magic[0]         = 0x50; // P
    payload->magic[1]         = 0x59; // Y
    payload->message_id       = MESSAGE_ID_MIBCOLLECTREQUEST;
    payload->payload_datasize = sizeof(struct l7ag_payload_header)
                              + sizeof(struct l7ag_mibrequest_message);

    struct l7ag_mibrequest_message* request = (struct l7ag_mibrequest_message*) (payload + 1);
    memset(request, 0, sizeof(struct l7ag_mibrequest_message));
    request->magic[0] = 0x52; // R
    request->magic[1] = 0x51; // Q

//	std::cout << "client write size:" << buf_size << std::endl;
	count++;
//	std::cout << "count:" << count << std::endl;
	if( count < 5 ){
		s.async_send( boost::asio::buffer( sendbuffer, buf_size ), boost::bind( &handle_write, boost::ref( s ) ) );
	}
}

void	handle_write(boost::asio::ip::tcp::socket& s){
//	std::cout << "client handle_write" << std::endl;
	if( count < 5 ){
		s.async_receive( boost::asio::buffer( recvbuffer, READBUF_SIZE ), 
								boost::bind(&handle_read,
								boost::ref( s ) ) );
	}
}

void	handle_connect(boost::asio::ip::tcp::socket& s){
//	std::cout << "client handle_connect" << std::endl;
	//create send data
    long long buf_size = sizeof(struct l7ag_message_header)
                                + sizeof(struct l7ag_payload_header)
                                + sizeof(struct l7ag_mibrequest_message);
	char sendbuffer[buf_size];
	memset( sendbuffer, 0, buf_size );
	struct l7ag_message_header* message_header;
	message_header = (struct l7ag_message_header*)sendbuffer;
	message_header->magic[0] = 0x4d;
	message_header->magic[1] = 0x47;
	message_header->version = 1;
	time_t tim;
	time( &tim );
	message_header->time = ( unsigned long long ) tim;
	message_header->payload_count = 1;
	message_header->size = buf_size;

    struct l7ag_payload_header* payload  = (struct l7ag_payload_header*) (message_header + 1);
    payload->magic[0]         = 0x50; // P
    payload->magic[1]         = 0x59; // Y
    payload->message_id       = MESSAGE_ID_MIBCOLLECTREQUEST;
    payload->payload_datasize = sizeof(struct l7ag_payload_header)
                              + sizeof(struct l7ag_mibrequest_message);

    struct l7ag_mibrequest_message* request = (struct l7ag_mibrequest_message*) (payload + 1);
    memset(request, 0, sizeof(struct l7ag_mibrequest_message));
    request->magic[0] = 0x52; // R
    request->magic[1] = 0x51; // Q
//	for(int i = 0; i < 100; i++){
	for(;;){
		if(test1.get_connectionstate() == true){
			break;
		}
		sleep(1);
	}
	s.async_send( boost::asio::buffer( sendbuffer, buf_size ), boost::bind( &handle_write, boost::ref( s ) ) );
}

void	clientthread(){
	boost::asio::io_service dispacher;
	boost::asio::ip::tcp::endpoint e( boost::asio::ip::address::from_string( ADDR_DEFAULT ), PORT_DEFAULT );
	boost::asio::ip::tcp::socket s( dispacher );
	count = 0;
	exit_flag = false;
	recvbuffer.assign( '\0' );
//	std::cout << "client async_connect start" << std::endl;
	s.async_connect( e, boost::bind( &handle_connect, boost::ref( s ) ) );
//	std::cout << "client async_connect end" << std::endl;
	for(;;){
		if(exit_flag == true){
			break;
		}
		dispacher.poll();
	}
//	std::cout << "client end" << std::endl;
}

void	snmpbridge_test_thread2(){
	boost::thread	thd1( boost::bind( &serverthread ) );
	boost::thread	thd2( boost::bind( &clientthread ) );

	thd1.join();
	thd2.join();

}

test_suite* init_unit_test_suite( int args, char* argv[]){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "snmpbridge thread test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &snmpbridge_test_thread1 ) );
	ts->add( BOOST_TEST_CASE( &snmpbridge_test_thread2 ) );

	framework::master_test_suite().add( ts );

	return 0;
}
