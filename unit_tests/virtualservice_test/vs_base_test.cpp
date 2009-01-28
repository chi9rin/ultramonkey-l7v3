
#include <iostream>
#include <boost/test/included/unit_test.hpp>
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

	// unit_test[]
	vs->get_qos_upstream();
	// unit_test[]
	vs->get_qos_downstream();
	// unit_test[]
	vs->get_throughput_upstream();
	// unit_test[]
	vs->get_throughput_downstream();

}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test2 ) );

	framework::master_test_suite().add( ts );

	return 0;
}

