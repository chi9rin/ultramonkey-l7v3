
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "virtualservice.h"
#include "vs_test.h"

using namespace boost::unit_test;

//test case1.create & destroy
void	virtualservice_base_test1(){
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	
	// unit_test[1]  object create
	BOOST_MESSAGE( "-------1" );
	l7vs::vs_test*	vs = new l7vs::vs_test( vsd, rep, element );

	BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
	BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
	BOOST_CHECK( element == vs->get_element() );

	// unit_test[2]  object destroy
	BOOST_MESSAGE( "-------2" );
	delete vs;
}

//test case2 method call1
void	virtualservice_base_test2(){
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//
	element.qos_upstream	= 1234ULL;
	element.qos_downstream	= 5678ULL;

	boost::shared_ptr<l7vs::vs_test>	vs( new l7vs::vs_test( vsd, rep, element ) );

	boost::system::error_code	test_err;
	l7vs::error_code			vs_err;

	//プロトコルモジュールをロードせずに呼ぶ
	// unit_test[3]  ProtocolModule's replication handler call test(module not loaded)
	BOOST_MESSAGE( "-------3" );
	vs->call_handle_protomod_replication( test_err );
	// unit_test[4]  get ProtocolModule ptr(module not loaded)
	BOOST_MESSAGE( "-------4" );
	BOOST_CHECK( NULL == vs->get_protocol_module() );

	//スケジュールモジュールをロードせずに呼ぶ
	// unit_test[5]  ScheduleModule's replication handler call test(module not loaded)
	BOOST_MESSAGE( "-------5" );
	vs->call_handle_schedmod_replication( test_err );
	// unit_test[6]  get ScheduleModule ptr(module not loaded)
	BOOST_MESSAGE( "-------6" );
	BOOST_CHECK( NULL == vs->get_schedule_module() );

	//プロトコルモジュールをロード後に呼ぶ
	//派生先のinitialize()でモジュールがロードされる
	vs->initialize( vs_err );
	// unit_test[7]  ProtocolModule's replication handler call test(module loaded)
	BOOST_MESSAGE( "-------7" );
	vs->call_handle_protomod_replication( test_err );
	// unit_test[8]  get ProtocolModule ptr(module loaded)
	BOOST_MESSAGE( "-------8" );
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	BOOST_CHECK( "PMtest1" == vs->get_protocol_module()->get_name() );

	//スケジュールモジュールをロード後に呼ぶ
	// unit_test[9]  ScheduleModule's replication handler call test(module loaded)
	BOOST_MESSAGE( "-------9" );
	vs->call_handle_schedmod_replication( test_err );
	// unit_test[10]  get ScheduleModule ptr(module loaded)
	BOOST_MESSAGE( "-------10" );
	BOOST_CHECK( NULL != vs->get_schedule_module() );
	BOOST_CHECK( "SMtest1" == vs->get_schedule_module()->get_name() );

	// unit_test[11]  rs_list_lock method
	BOOST_MESSAGE( "-------11" );
	vs->rs_list_lock();
	BOOST_CHECK( 1 == vs->get_refcount() );
	// unit_test[12]  rs_list_unlock method
	BOOST_MESSAGE( "-------12" );
	vs->rs_list_unlock();
	BOOST_CHECK( 0 == vs->get_refcount() );

	// unit_test[13]  rs_list_lock method(ULLMAX + 1)
	BOOST_MESSAGE( "-------13" );
	vs->set_refcount( ULLONG_MAX );
	vs->rs_list_lock();
	std::cout << vs->get_refcount() << std::endl;
	BOOST_CHECK( 0ULL == vs->get_refcount() );

	// unit_test[14]  rs_list_lock method(ULLMIN - 1)
	BOOST_MESSAGE( "-------14" );
	vs->set_refcount( 0 );
	vs->rs_list_unlock();
	std::cout << vs->get_refcount() << std::endl;
	BOOST_CHECK( ULLONG_MAX == vs->get_refcount() );

	// unit_test[15]  get_qos_upstream method call(check value equal element.qos_upstream)
	BOOST_MESSAGE( "-------15" );
	BOOST_CHECK( 1234ULL == vs->get_qos_upstream() );
	// unit_test[16]  get_qos_downstream method call(check value equal element.qos_downstream)
	BOOST_MESSAGE( "-------16" );
	BOOST_CHECK( 5678ULL == vs->get_qos_downstream() );

	// unit_test[17]  test update_up_recv_size method
	BOOST_MESSAGE( "-------17" );
	vs->update_up_recv_size( 12ULL );
	BOOST_CHECK( 12ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );
	// unit_test[18]  test update_up_recv_size method, collectly add value
	BOOST_MESSAGE( "-------18" );
	vs->update_up_recv_size( 28ULL );
	BOOST_CHECK( 40ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );
	// unit_test[19]  test update_up_recv_size method, ULLONG_MAX added
	//ULLONG_MAXの境界値テスト
	BOOST_MESSAGE( "-------19" );
	vs->update_up_recv_size( ULLONG_MAX );
	BOOST_CHECK( 40ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );

	// unit_test[20]  test update_up_recv_size method
	BOOST_MESSAGE( "-------20" );
	vs->update_up_send_size( 1024ULL );
	BOOST_CHECK( 1024ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );
	// unit_test[21]  test update_up_recv_size method, collectly add value
	BOOST_MESSAGE( "-------21" );
	vs->update_up_send_size( 2048ULL );
	BOOST_CHECK( 3072ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );
	// unit_test[22]  test update_up_recv_size method, ULLONG_MAX added
	//ULLONG_MAXの境界値テスト
	BOOST_MESSAGE( "-------22" );
	vs->update_up_send_size( ULLONG_MAX );
	BOOST_CHECK( 3072ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );

	// unit_test[23]  test update_up_recv_size method
	BOOST_MESSAGE( "-------24" );
	vs->update_down_recv_size( 101010ULL );
	BOOST_CHECK( 101010ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );
	// unit_test[25]  test update_up_recv_size method
	BOOST_MESSAGE( "-------25" );
	vs->update_down_recv_size( 505050ULL );
	BOOST_CHECK( 606060ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );
	// unit_test[26]  test update_up_recv_size method
	//ULLONG_MAXの境界値テスト
	BOOST_MESSAGE( "-------26" );
	vs->update_down_recv_size( ULLONG_MAX );
	BOOST_CHECK( 606060ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );

	// unit_test[27]  test update_up_recv_size method
	BOOST_MESSAGE( "-------27" );
	vs->update_down_send_size( 123456789ULL );
	BOOST_CHECK( 123456789ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );
	// unit_test[28]  test update_up_recv_size method
	BOOST_MESSAGE( "-------28" );
	vs->update_down_send_size( 987654321ULL );
	BOOST_CHECK( 1111111110ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );
	// unit_test[29]  test update_up_recv_size method
	//ULLONG_MAXの境界値テスト
	BOOST_MESSAGE( "-------29" );
	vs->update_down_send_size( ULLONG_MAX );
	BOOST_CHECK( 1111111110ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );

	//スループット計算のテスト
	// unit_test[30]  	calcurate throughput
	BOOST_MESSAGE( "-------30" );
	//計算(curr_sizeクリアのため)
	vs->call_handle_throughput_update( test_err );
	//のぼり受信データサイズ更新
	vs->update_up_recv_size( 1000000000ULL );
	vs->update_down_recv_size( 2000000000ULL );
	//それから計算
	vs->call_handle_throughput_update( test_err );
	//コンソールに出して確認
	// unit_test[31]	test get_throughput_upstream method
	BOOST_MESSAGE( "-------31" );
	BOOST_MESSAGE( "bps up   : " << vs->get_throughput_upstream() );
	// unit_test[32]	test get_throughput_downstream method
	BOOST_MESSAGE( "-------32" );
	BOOST_MESSAGE( "bps down : " << vs->get_throughput_downstream() );
}

//test case3 replication data serialization test
void	virtualservice_base_test3(){
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//
	element.qos_upstream	= 1234ULL;
	element.qos_downstream	= 5678ULL;

	boost::shared_ptr<l7vs::vs_test>	vs1( new l7vs::vs_test( vsd, rep, element ) );

	// unit_test[24]	replication data serialization test
	BOOST_MESSAGE( "-------33" );
	vs1->test_replication();
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test2 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test3 ) );

	framework::master_test_suite().add( ts );

	return 0;
}

