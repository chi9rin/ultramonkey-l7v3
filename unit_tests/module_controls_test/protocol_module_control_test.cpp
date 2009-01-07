
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
	//============================================
	//protocol_module_control
	//1
	//getInstanceメソッドのテスト
	//getInstanceによって取得したcontrolとcontrol_2のインスタンスが同一であることを確認する。 
	//============================================
	l7vs::protocol_module_control& control = l7vs::protocol_module_control::getInstance();
	l7vs::protocol_module_control& control_2 = l7vs::protocol_module_control::getInstance();

	BOOST_CHECK_EQUAL( &control, &control_2 );

	//test initialize and finalize
	control.initialize( "./" );
	control.finalize();


	//============================================
	//protocol_module_control
	//2
	//load_moduleメソッドのテスト(正常系その１)
	//ProtocolModuleクラスのインスタンスが取得できること
	//例外が発生しないこと
	//指定したモジュール名と取得したモジュール名が同じこと 
	//============================================
	control.initialize( "./" );
	l7vs::protocol_module_base*		protomod = NULL;
	try{
		protomod = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != protomod );
//	BOOST_CHECK_EQUAL( PM1, protomod.get_name() );

}

//test case2
void	protocol_module_control_test_thread(){

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

