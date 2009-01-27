
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "protocol_module_control.h"
//#include "../../src/protocol_module_control.cpp"

#define	PM1	"test1"
#define	PM2	"test2"

class	l7vs::protocol_module_control;

using namespace boost::unit_test;

//test case1.
void	protocol_module_control_test(){
	// unit_test[1]  getInstanceメソッドのテスト
	l7vs::protocol_module_control& control = l7vs::protocol_module_control::getInstance();
	l7vs::protocol_module_control& control_2 = l7vs::protocol_module_control::getInstance();

	//Compile Error
	//l7vs::protocol_module_control	control_a;

	BOOST_CHECK_EQUAL( &control, &control_2 );

	//test initialize and finalize
	control.initialize( "./" );
	control.finalize();

	// unit_test[2]  load_moduleメソッドのテスト(正常系その１)
	control.initialize( "./" );
	l7vs::protocol_module_base*		protomod1 = NULL;
	try{
		protomod1 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != protomod1 );
//	BOOST_CHECK_EQUAL( PM1, protomod.get_name() );

	// unit_test[2]  load_moduleメソッドのテスト(正常系その１)
	control.initialize( "./" );
	l7vs::protocol_module_base*		protomod2 = NULL;
	try{
		protomod2 = control.load_module( PM2 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != protomod2 );
//	BOOST_CHECK_EQUAL( PM1, protomod.get_name() );

}

//test case2
void	protocol_module_control_test_thread(){
	// unit_test[3]  テスト

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_control" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test ) );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread ) );

	framework::master_test_suite().add( ts );

	return 0;
}

